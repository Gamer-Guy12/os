#include "libk/math.h"
#include <acpi/acpi.h>
#include <hpet.h>
#include <interrupts.h>
#include <irq.h>
#include <libk/spinlock.h>
#include <mem/pimemory.h>
#include <mem/vimemory.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <threading.h>
#include <threading/threading.h>

bool bits64 = false;
/// This is in femtoseconds (10^-15 seconds)
size_t clock_frequency = 0;
bool usable_timers[32] = {false};
typedef void (*callback)(void);
size_t timer_count = 0;
static spinlock_t lock;
uint128_t wait_times[32] = {ZERO_128};

callback callbacks[32];

uint128_t hpet_cycles_wait(uint64_t ms) {
  uint128_t temp = mul_128(ms, 1000000000000);
  return div_128(temp, clock_frequency);
}

void bind_hpet_callback(void (*callback)(void), uint8_t hpet) {
  callbacks[hpet] = callback;
}

uint8_t reserve_hpet(void) {
  spinlock_acquire(&lock);

  uint8_t ret = NO_HPET;

  for (size_t i = 0; i < timer_count; i++) {
    if (usable_timers[i] == true) {
      ret = i;
      usable_timers[i] = false;
      break;
    }
  }

  spinlock_release(&lock);

  return ret;
}

void return_hpet(uint8_t hpet) {
  spinlock_acquire(&lock);

  usable_timers[hpet] = true;

  spinlock_release(&lock);
}

static const void *read_register(size_t reg) {
  return (void *)(HPET_ADDR + reg);
}

static size_t read_reg_value(size_t reg) {
  return *(volatile size_t *)(HPET_ADDR + reg);
}

static void write_register(size_t reg, uint64_t value) {
  volatile uint64_t *addr = (void *)(HPET_ADDR + reg);
  *addr = value;
}

uint128_t hpet_get_progress(uint8_t hpet) {
  volatile size_t comparator = read_reg_value(HPET_TIMER_COMPARATOR_VAL(hpet));
  volatile size_t main_counter = read_reg_value(MAIN_COUNTER_VALUE);
  return add_128(NUM_128(comparator - main_counter), wait_times[hpet]);
}

void hpet_int_handler(idt_registers_t* registers) {

}

void hpet_interrupt_in_cycles(uint128_t cycles, uint8_t hpet) {

}

void hpet_interrupt_in(uint64_t ms, uint8_t hpet) {
  hpet_interrupt_in_cycles(hpet_cycles_wait(ms), hpet);
}

size_t enable_hpet(void) {
  const HPET_t *hpet = acpi_get_struct("HPET");

  map_phys_page((void *)HPET_ADDR,
                PT_PRESENT | PT_READ_WRITE | PT_PAGE_WRITE_THROUGH, 1,
                (void *)hpet->address.addr);

  HPET_gen_caps_t *capabilities = read_register(HPET_GEN_CAPS);
  const size_t comparator_count = capabilities->timer_count + 1;
  if (capabilities->count_size_cap == 1)
    bits64 = true;
  clock_frequency = capabilities->clock_period;

  /// Clear the bottom 2 bits
  write_register(HPET_GEN_CONFIG, 0);

  irq_t irq = get_irq();
  irq.map_irq(HPET_GENERAL_INT, irq.get_pass_irq(2));
  irq.map_irq(HPET_GENERAL_INT, irq.get_pass_irq(16));
  irq.map_irq(HPET_GENERAL_INT, irq.get_pass_irq(17));
  irq.map_irq(HPET_GENERAL_INT, irq.get_pass_irq(18));

  irq.set_trigger_mode(false, false, irq.get_pass_irq(2));
  irq.set_trigger_mode(false, false, irq.get_pass_irq(16));
  irq.set_trigger_mode(false, false, irq.get_pass_irq(17));
  irq.set_trigger_mode(false, false, irq.get_pass_irq(18));

  irq.unmask_irq(irq.get_pass_irq(2));
  irq.unmask_irq(irq.get_pass_irq(16));
  irq.unmask_irq(irq.get_pass_irq(17));
  irq.unmask_irq(irq.get_pass_irq(18));

  size_t usable_count = comparator_count;
  memset(usable_timers, true, comparator_count);

  for (size_t i = 0; i < comparator_count; i++) {
    HPET_timer_config_caps_t timer_config;
    timer_config.value = read_reg_value(HPET_TIMER_CONFIG_CAP(i));

    timer_config.periodic = 0;
    timer_config.trigger_type = 1;
    timer_config.periodic = 0;
    timer_config.fsb_int_mapping = 0;

    if (timer_config.ioapic_support_bit & (1 << 2)) {
      timer_config.ioapic_route = 2;
      write_register(HPET_TIMER_CONFIG_CAP(i), timer_config.value);
      continue;
    }

    if (timer_config.ioapic_support_bit & (1 << 16)) {
      timer_config.ioapic_route = 16;
      write_register(HPET_TIMER_CONFIG_CAP(i), timer_config.value);
      continue;
    }

    if (timer_config.ioapic_support_bit & (1 << 17)) {
      timer_config.ioapic_route = 17;
      write_register(HPET_TIMER_CONFIG_CAP(i), timer_config.value);
      continue;
    }

    if (timer_config.ioapic_support_bit & (1 << 18)) {
      timer_config.ioapic_route = 18;
      write_register(HPET_TIMER_CONFIG_CAP(i), timer_config.value);
      continue;
    }

    usable_count--;
    usable_timers[i] = false;
  }

  timer_count = usable_count;
  register_interrupt_handler(hpet_int_handler, HPET_GENERAL_INT);

  write_register(HPET_GEN_INT_STATUS, 0);
  /// The comparators start at 0 so by setting this to 1 it means that it will
  /// take a while before it triggers and interrupt
  write_register(MAIN_COUNTER_VALUE, 1);
  /// Enable the main counter
  write_register(HPET_GEN_CONFIG, 1);

  return usable_count;
}

bool check_for_hpet(void) {
  const HPET_t *hpet = acpi_get_struct("HPET");

  if (hpet == NULL) {
    return 0;
  }

  return 1;
}
