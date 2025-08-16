#include <acpi/acpi.h>
#include <asm.h>
#include <hal/clk.h>
#include <hpet.h>
#include <interrupts.h>
#include <irq.h>
#include <libk/math.h>
#include <mem/vimemory.h>
#include <stddef.h>
#include <stdint.h>

size_t clock_period = 0;
typedef void (*clock_callback)(void);
static clock_callback callbacks[32] = {NULL};

void hpet_int_handler(idt_registers_t *registers) {
  size_t hpet_int_status_reg_val =
      *(volatile size_t *)(HPET_ADDR + HPET_GENERAL_INT);
  volatile size_t *hpet_int_status_reg =
      (volatile size_t *)(HPET_ADDR + HPET_GENERAL_INT);
  size_t hpet_num = math_log(hpet_int_status_reg_val, 2);

  if (callbacks[hpet_num] != NULL)
    callbacks[hpet_num]();

  *hpet_int_status_reg |= (1 << hpet_num);
  irq_t irq = get_irq();
  irq.eoi();
}

size_t enable_hpet(void) {
  const HPET_t *hpet = acpi_get_struct("HPET");

  // Map hpet into addr space
  const size_t addr = hpet->address.addr;
  map_phys_page((void *)HPET_ADDR,
                PT_PRESENT | PT_READ_WRITE | PT_PAGE_WRITE_THROUGH, 1,
                (void *)addr);

  HPET_gen_config_t *config =
      (HPET_gen_config_t *)(HPET_ADDR + HPET_GEN_CONFIG_OFFSET);

  config->enable_timer = 1;
  config->legacy_mapping_enabled = 0;

  HPET_gen_caps_t *capabilities =
      (HPET_gen_caps_t *)(HPET_ADDR + HPET_GEN_CAPS_OFFSET);

  size_t timer_count = capabilities->timer_count + 1;
  size_t usable_timer_count = timer_count;

  irq_t irq = get_irq();
  irq.map_irq(HPET_GENERAL_INT, 0x2);
  irq.map_irq(HPET_GENERAL_INT, 0x0);
  irq.map_irq(HPET_GENERAL_INT, 0x8);

  irq.unmask_irq(0x2);
  irq.unmask_irq(0x0);
  irq.unmask_irq(0x8);

  /// The irqs are level triggered
  irq.set_edge_triggered(false, 0x2);
  irq.set_edge_triggered(false, 0x0);
  irq.set_edge_triggered(false, 0x8);

  for (size_t i = 0; i < timer_count; i++) {
    HPET_timer_config_caps_t *config =
        (HPET_timer_config_caps_t *)(HPET_ADDR +
                                     HPET_TIMER_CONFIG_CAP_OFFSET(i));

    /// 1 is level triggered
    config->trigger_type = 1;
    config->periodic = 0;
    config->fsb_int_mapping = 0;

    /// Attempt to map to irq 2
    if (config->ioapic_support_bit & (1 << 2)) {
      config->ioapic_route = 2;
      continue;
    }

    /// Attempt to map to irq 0
    if (config->ioapic_support_bit & (1 << 0)) {
      config->ioapic_route = 0;
      continue;
    }

    /// Attempt to map to irq 8
    if (config->ioapic_support_bit & (1 << 8)) {
      config->ioapic_route = 8;
      continue;
    }

    // This timer can't be used
    usable_timer_count--;
  }

  clock_period = capabilities->clock_period;

  return usable_timer_count;
}

bool check_for_hpet(void) {
  const HPET_t *hpet = acpi_get_struct("HPET");

  if (hpet == NULL) {
    return 0;
  }

  return 1;
}
