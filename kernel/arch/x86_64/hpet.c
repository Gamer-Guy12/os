#include "libk/kio.h"
#include <acpi/acpi.h>
#include <asm.h>
#include <hal/clk.h>
#include <hpet.h>
#include <interrupts.h>
#include <irq.h>
#include <libk/err.h>
#include <libk/math.h>
#include <libk/mem.h>
#include <libk/spinlock.h>
#include <libk/sys.h>
#include <mem/memory.h>
#include <mem/pimemory.h>
#include <mem/vimemory.h>
#include <stdatomic.h>
#include <stddef.h>
#include <stdint.h>
#include <threading.h>

size_t clock_period = 0;

/// There are 1000000000000 femtoseconds in a millisecond
/// divide that by clock_period to get how many cycles to wait for a millisecond
#define CYCLES_TO_WAIT(ms) ((ms * 1000000000000) / clock_period)
typedef void (*clock_callback)(void);
static volatile clock_callback callbacks[32] = {NULL};
static bool bits64 = false;
spinlock_t hpet_int_lock = ATOMIC_FLAG_INIT;

static void interrupt_in(uint32_t ms, uint8_t hpet) {
  size_t cycles_to_wait = CYCLES_TO_WAIT(ms);
  HPET_gen_config_t *gen_config =
      (HPET_gen_config_t *)(HPET_ADDR + HPET_GEN_CONFIG_OFFSET);
  HPET_gen_config_t save_gen = *gen_config;
  save_gen.enable_timer = 0;
  *gen_config = save_gen;
  size_t current_cycle_count =
      *(volatile size_t *)(HPET_ADDR + MAIN_COUNTER_VALUE_OFFSET);
  size_t store_value = current_cycle_count + cycles_to_wait;

  volatile size_t *comparator_register =
      (volatile size_t *)(HPET_ADDR + HPET_TIMER_COMPARATOR_VAL_OFFSET(hpet));
  *comparator_register = store_value;
  save_gen = *gen_config;
  save_gen.enable_timer = 1;
  *gen_config = save_gen;

  HPET_timer_config_caps_t *config =
      (HPET_timer_config_caps_t *)(HPET_ADDR +
                                   HPET_TIMER_CONFIG_CAP_OFFSET(hpet));

  HPET_timer_config_caps_t save_conf = *config;
  save_conf.int_enable = 1;
  *config = save_conf;
}

#define CREATE_HPET_CALLBACK(x)                                                \
  static void interrupt_in_hpet_##x(uint32_t ms, void (*callback)(void)) {     \
    size_t i = x;                                                              \
                                                                               \
    callbacks[i] = callback;                                                   \
                                                                               \
    interrupt_in(ms, i);                                                       \
  }

CREATE_HPET_CALLBACK(0)
CREATE_HPET_CALLBACK(1)
CREATE_HPET_CALLBACK(2)
CREATE_HPET_CALLBACK(3)
CREATE_HPET_CALLBACK(4)
CREATE_HPET_CALLBACK(5)
CREATE_HPET_CALLBACK(6)
CREATE_HPET_CALLBACK(7)
CREATE_HPET_CALLBACK(8)
CREATE_HPET_CALLBACK(9)
CREATE_HPET_CALLBACK(10)
CREATE_HPET_CALLBACK(11)
CREATE_HPET_CALLBACK(12)
CREATE_HPET_CALLBACK(13)
CREATE_HPET_CALLBACK(14)
CREATE_HPET_CALLBACK(15)
CREATE_HPET_CALLBACK(16)
CREATE_HPET_CALLBACK(17)
CREATE_HPET_CALLBACK(18)
CREATE_HPET_CALLBACK(19)
CREATE_HPET_CALLBACK(20)
CREATE_HPET_CALLBACK(21)
CREATE_HPET_CALLBACK(22)
CREATE_HPET_CALLBACK(23)
CREATE_HPET_CALLBACK(24)
CREATE_HPET_CALLBACK(25)
CREATE_HPET_CALLBACK(26)
CREATE_HPET_CALLBACK(27)
CREATE_HPET_CALLBACK(28)
CREATE_HPET_CALLBACK(29)
CREATE_HPET_CALLBACK(30)
CREATE_HPET_CALLBACK(31)

#define USE_HPET_CALLBACK(i)                                                   \
  if (i >= full_timer_count)                                                   \
    goto done;                                                                 \
  if (usable_timers[i] == false)                                               \
    goto done_##i;                                                             \
  hal_clk_t *clock_##i = gmalloc(sizeof(hal_clk_t));                           \
  clock_##i->interrupt_in = &interrupt_in_hpet_##i;                            \
  hal_give_clock(clock_##i);                                                   \
  done_##i :;

static void create_clocks(bool *usable_timers, size_t full_timer_count) {
  USE_HPET_CALLBACK(0)
  USE_HPET_CALLBACK(1)
  USE_HPET_CALLBACK(2)
  USE_HPET_CALLBACK(3)
  USE_HPET_CALLBACK(4)
  USE_HPET_CALLBACK(5)
  USE_HPET_CALLBACK(6)
  USE_HPET_CALLBACK(7)
  USE_HPET_CALLBACK(8)
  USE_HPET_CALLBACK(9)
  USE_HPET_CALLBACK(10)
  USE_HPET_CALLBACK(11)
  USE_HPET_CALLBACK(12)
  USE_HPET_CALLBACK(13)
  USE_HPET_CALLBACK(14)
  USE_HPET_CALLBACK(15)
  USE_HPET_CALLBACK(16)
  USE_HPET_CALLBACK(17)
  USE_HPET_CALLBACK(18)
  USE_HPET_CALLBACK(19)
  USE_HPET_CALLBACK(20)
  USE_HPET_CALLBACK(21)
  USE_HPET_CALLBACK(22)
  USE_HPET_CALLBACK(23)
  USE_HPET_CALLBACK(24)
  USE_HPET_CALLBACK(25)
  USE_HPET_CALLBACK(26)
  USE_HPET_CALLBACK(27)
  USE_HPET_CALLBACK(28)
  USE_HPET_CALLBACK(29)
  USE_HPET_CALLBACK(30)
  USE_HPET_CALLBACK(31)
done:;
}

void hpet_int_handler(idt_registers_t *registers) {
  volatile size_t *hpet_int_status_reg =
      (volatile size_t *)(HPET_ADDR + HPET_GEN_INT_STATUS_OFFSET);
  uint8_t hpet_num = 32;

  spinlock_acquire(&hpet_int_lock);

  for (size_t i = 0; i < 32; i++) {
    if (*hpet_int_status_reg & (1 << i)) {
      hpet_num = i;
      break;
    }
  }

  if (hpet_num == 32) {
    sys_panic(HPET_ERR | INVALID_IDX);
  }

  spinlock_release(&hpet_int_lock);

  HPET_timer_config_caps_t *config =
      (HPET_timer_config_caps_t *)(HPET_ADDR +
                                   HPET_TIMER_CONFIG_CAP_OFFSET(hpet_num));
  HPET_timer_config_caps_t save_config = *config;

  save_config.int_enable = 0;

  *config = save_config;

  if (callbacks[hpet_num] != NULL)
    callbacks[hpet_num]();

  size_t status_reg = *hpet_int_status_reg;
  status_reg |= (1 << hpet_num);
  *hpet_int_status_reg = status_reg;

  irq_t irq = get_irq();
  irq.eoi();
}

// static void test(void) { kio_printf("Done\n"); }

/// Initialize the main up counter and all of the comparators
///
/// Set the clock_period and whether the registers are 64 bits or 32 bits
///
/// You will have to map the hpet into memory
size_t enable_hpet(void) {
  const HPET_t *hpet = acpi_get_struct("HPET");

  /// Map it into memory
  map_phys_page((void *)HPET_ADDR,
                PT_PRESENT | PT_READ_WRITE | PT_PAGE_WRITE_THROUGH, 1,
                (void *)hpet->address.addr);

  const HPET_gen_caps_t *capabilities =
      (HPET_gen_caps_t *)(HPET_ADDR + HPET_GEN_CAPS_OFFSET);

  if (capabilities->count_size_cap) {
    bits64 = true;
  }

  clock_period = capabilities->clock_period;

  const size_t timer_count = capabilities->timer_count + 1;
  size_t usable_timer_count = timer_count;

  HPET_gen_config_t *config =
      (HPET_gen_config_t *)(HPET_ADDR + HPET_GEN_CONFIG_OFFSET);
  HPET_gen_config_t save_config = *config;

  save_config.legacy_mapping_enabled = false;
  save_config.enable_timer = false;

  *config = save_config;

  bool usable_timers[timer_count];

  memset(usable_timers, true, timer_count);

  // Quickly disable the PIT :)
  // This makes it so that the PIT is stuck in a low state and since the hpet is
  // active high this should mean that it will never trigger an irq
  outb(0x43, 0 | 0 | (1 << 4) | 0);
  io_wait();

  register_interrupt_handler(hpet_int_handler, HPET_GENERAL_INT);

  irq_t irq = get_irq();

  irq.map_irq(HPET_GENERAL_INT, irq.get_pass_irq(2));
  irq.map_irq(HPET_GENERAL_INT, irq.get_pass_irq(16));
  irq.map_irq(HPET_GENERAL_INT, irq.get_pass_irq(17));
  irq.map_irq(HPET_GENERAL_INT, irq.get_pass_irq(18));

  /// Hpet is active high
  irq.set_trigger_mode(false, false, irq.get_pass_irq(2));
  irq.set_trigger_mode(false, false, irq.get_pass_irq(16));
  irq.set_trigger_mode(false, false, irq.get_pass_irq(17));
  irq.set_trigger_mode(false, false, irq.get_pass_irq(18));

  irq.unmask_irq(irq.get_pass_irq(2));
  irq.unmask_irq(irq.get_pass_irq(16));
  irq.unmask_irq(irq.get_pass_irq(17));
  irq.unmask_irq(irq.get_pass_irq(18));

  for (size_t i = 0; i < timer_count; i++) {
    HPET_timer_config_caps_t *timer_config_caps =
        (HPET_timer_config_caps_t *)(HPET_ADDR +
                                     HPET_TIMER_CONFIG_CAP_OFFSET(i));

    HPET_timer_config_caps_t save_config = *timer_config_caps;

    save_config.int_enable = 0;
    save_config.trigger_type = 1;
    save_config.periodic = 0;
    save_config.fsb_int_mapping = 0;

    if (save_config.ioapic_support_bit & (1 << 2)) {
      save_config.ioapic_route = 2;
      *timer_config_caps = save_config;
      continue;
    }

    if (timer_config_caps->ioapic_support_bit & (1 << 16)) {
      save_config.ioapic_route = 16;
      *timer_config_caps = save_config;
      continue;
    }

    if (timer_config_caps->ioapic_support_bit & (1 << 16)) {
      save_config.ioapic_route = 16;
      *timer_config_caps = save_config;
      continue;
    }

    if (timer_config_caps->ioapic_support_bit & (1 << 16)) {
      save_config.ioapic_route = 16;
      *timer_config_caps = save_config;
      continue;
    }

    usable_timers[i] = false;
    usable_timer_count--;
  }

  config->enable_timer = true;
  create_clocks(usable_timers, timer_count);

  return usable_timer_count;
}

bool check_for_hpet(void) {
  const HPET_t *hpet = acpi_get_struct("HPET");

  if (hpet == NULL) {
    return 0;
  }

  return 1;
}
