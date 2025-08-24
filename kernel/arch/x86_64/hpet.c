#include "libk/kio.h"
#include <acpi/acpi.h>
#include <asm.h>
#include <decls.h>
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
///
/// Imma just cast everything because im not sure which parts may have issues
uint128_t hpet_cycles_wait(uint32_t ms) {
  uint128_t temp = mul_128(ms, 1000000000000);
  return div_128(temp, clock_period);
}

typedef void (*clock_callback)(void);
static volatile clock_callback callbacks[32] = {NULL};
static bool bits64 = false;
spinlock_t hpet_int_lock = ATOMIC_FLAG_INIT;
bool usable_timers[32] = {false};

uint128_t wait_times[32] = {0};

uint128_t hpet_get_progress(uint8_t hpet) {
  size_t current_cycle_count =
      *(volatile size_t *)(HPET_ADDR + MAIN_COUNTER_VALUE_OFFSET);

  return add_128(wait_times[hpet], NUM_128(current_cycle_count));
}

void hpet_interrupt_in_cycles(uint128_t cycles, uint8_t hpet) {
  size_t current_cycle_count =
      *(volatile size_t *)(HPET_ADDR + MAIN_COUNTER_VALUE_OFFSET);
  size_t current_cycle_wait = 0;

  /// U'll notice that im adding and subtracting 1, this is because if i were to
  /// just do a full 64 bits or 32 bits the value would be the same as the
  /// current count in the counter which would instantly trigger an interrupt
  /// which would be bad
  if (bits64) {
    if (less_than_128(cycles, (uint128_t){.lower = MAX_64, .upper = 0})) {
      current_cycle_wait = cycles.lower;
      wait_times[hpet] = ZERO_128;
    } else {
      current_cycle_wait = MAX_64 - 1;
      wait_times[hpet] =
          sub_128(cycles, (uint128_t){.upper = 0, .lower = MAX_64 - 1});
    }
  } else {
    if (less_than_128(cycles, (uint128_t){.lower = MAX_32, .upper = 0})) {
      current_cycle_wait = cycles.lower;
      wait_times[hpet] = ZERO_128;
    } else {
      current_cycle_wait = MAX_32 - 1;
      wait_times[hpet] =
          sub_128(cycles, (uint128_t){.upper = 0, .lower = MAX_32 - 1});
    }
  }

  HPET_gen_config_t *gen_config =
      (HPET_gen_config_t *)(HPET_ADDR + HPET_GEN_CONFIG_OFFSET);
  HPET_gen_config_t save_gen = *gen_config;
  save_gen.enable_timer = 0;
  *gen_config = save_gen;

  size_t store_value = current_cycle_count + current_cycle_wait;

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

void hpet_interrupt_in(uint32_t ms, uint8_t hpet) {
  uint128_t cycles_to_wait = hpet_cycles_wait(ms);
  hpet_interrupt_in_cycles(cycles_to_wait, hpet);
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

  bool reenable = false;

  if (IS_ZERO_128(wait_times[hpet_num])) {
    if (callbacks[hpet_num] != NULL)
      callbacks[hpet_num]();
  } else {
    hpet_interrupt_in_cycles(wait_times[hpet_num], hpet_num);
    reenable = true;
  }

  size_t status_reg = *hpet_int_status_reg;
  status_reg |= (1 << hpet_num);
  *hpet_int_status_reg = status_reg;

  irq_t irq = get_irq();
  irq.eoi();

  if (!reenable)
    return;

  save_config = *config;

  save_config.int_enable = 1;

  *config = save_config;
}

uint8_t reserve_hpet(void) {
  for (size_t i = 0; i < 32; i++) {
    if (usable_timers[i]) {
      usable_timers[i] = false;
      return i;
    }
  }

  return NO_HPET;
}

void return_hpet(uint8_t hpet) { usable_timers[hpet] = true; }

void bind_hpet_callback(void (*callback)(void), uint8_t hpet) {
  callbacks[hpet] = callback;
}

volatile size_t clean_count = 0;

static void clean_callback(void) { ATOMIC_INC(clean_count); }

void clean_hpets(void) {
  size_t desired_count = 0;

  for (size_t i = 0; i < 32; i++) {
    if (!usable_timers[i])
      continue;

    desired_count++;
    bind_hpet_callback(clean_callback, i);
    hpet_interrupt_in(1, i);
  }

  while (clean_count != desired_count) {
    // kio_printf("D %x %x\n", desired_count, clean_count);
  }

  /// 32 callbacks need to be cleared
  memset((void *)callbacks, 0, sizeof(void (*)(void)) * 32);
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
  clean_hpets();

  return usable_timer_count;
}

bool check_for_hpet(void) {
  const HPET_t *hpet = acpi_get_struct("HPET");

  if (hpet == NULL) {
    return 0;
  }

  return 1;
}
