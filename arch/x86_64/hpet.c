#include "hpet.h"
#include "acpi.h"
#include "include/apic.h"
#include "interrupts.h"
#include "kernel/gheap.h"
#include "kernel/kprintf.h"
#include "kernel/mem.h"
#include "kernel/timers.h"
#include "util.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

static uintptr_t hpet_regs;
static uint64_t comparator_count;
static uint64_t frequency;

struct hpet_timer {
  struct timer timer;
  uint16_t hpet;
};

// True means 64 bit
static bool counter_size;
// True means 64 bit (don't support 32 bit counters)
static bool comparator_sizes[32] = {false};

typedef void (*hpet_callback_t)(void *);

static hpet_callback_t callbacks[32] = {NULL};
static void *datas[32] = {NULL};

uint64_t hpet_read_reg(uint16_t reg) {
  volatile uint64_t *reg_addr = (volatile uint64_t *)(hpet_regs + reg);
  return *reg_addr;
}

static void hpet_write_reg(uint16_t reg, uint64_t val) {
  volatile uint64_t *reg_addr = (volatile uint64_t *)(hpet_regs + reg);
  *reg_addr = val;
}

spinlock_t hpet_handler_lock = SPINLOCK_ZERO;

static void hpet_handler(void *context) {
  // Find which hpet interrupted and acknowledge
  uint8_t hpet = 32;
  spinlock_acquire(&hpet_handler_lock);
  uint64_t statuses = hpet_read_reg(HPET_GEN_INT_STATUS);

  for (int i = 0; i < 32; i++) {
    if (statuses & (1 << i)) {
      hpet = i;
      break;
    }
  }

  hpet_write_reg(HPET_GEN_INT_STATUS, 1 << hpet);
  spinlock_release(&hpet_handler_lock);
  apic_eoi();

  hpet_write_reg(HPET_TIMER_CONF_CAPS(hpet),
                 hpet_read_reg(HPET_TIMER_CONF_CAPS(hpet)) & ~(1 << 2));

  hpet_callback_t callback = callbacks[hpet];
  void *data = datas[hpet];
  callbacks[hpet] = NULL;
  datas[hpet] = NULL;
  callback(data);
}

static void hpet_cancel(struct timer *timer_struct) {
  disable_interrupts();

  struct hpet_timer *timer = (void *)timer_struct;
  // Basically make it so that interrupts will happen in about 5.2k years
  uint64_t cur_ticks = hpet_read_reg(HPET_MAIN_COUNTER);
  hpet_write_reg(HPET_TIMER_COMPARATOR_VAL(timer->hpet), cur_ticks + MAX_64);

  uint64_t config = hpet_read_reg(HPET_TIMER_CONF_CAPS(timer->hpet));
  // Disable interrupts
  config &= ~(1 << 2);
  hpet_write_reg(HPET_TIMER_CONF_CAPS(timer->hpet), config);

  // Clear any interrupts if they happened
  hpet_write_reg(HPET_GEN_INT_STATUS, 1 << timer->hpet);

  enable_interrupts();
}

static void hpet_int_microseconds(void (*callback)(void *), void *data,
                                  struct timer *timer_struct,
                                  uint64_t micro_seconds) {
  disable_interrupts();

  uint64_t cur_ticks = hpet_read_reg(HPET_MAIN_COUNTER);
  struct hpet_timer *timer = (void *)timer_struct;
  callbacks[timer->hpet] = callback;
  datas[timer->hpet] = data;

  uint64_t tick_count = frequency * micro_seconds / 1000000;
  hpet_write_reg(HPET_TIMER_COMPARATOR_VAL(timer->hpet),
                 cur_ticks + tick_count);
  // Enable interrupts
  hpet_write_reg(HPET_TIMER_CONF_CAPS(timer->hpet),
                 hpet_read_reg(HPET_TIMER_CONF_CAPS(timer->hpet)) | (1 << 2));

  enable_interrupts();
}

static void hpet_int_ms(void (*callback)(void *), void *data,
                        struct timer *timer_struct, uint64_t ms) {
  disable_interrupts();

  uint64_t cur_ticks = hpet_read_reg(HPET_MAIN_COUNTER);
  struct hpet_timer *timer = (void *)timer_struct;
  callbacks[timer->hpet] = callback;
  datas[timer->hpet] = data;

  uint64_t tick_count = frequency * ms / 1000;
  hpet_write_reg(HPET_TIMER_COMPARATOR_VAL(timer->hpet),
                 cur_ticks + tick_count);

  // Enable interrupts
  hpet_write_reg(HPET_TIMER_CONF_CAPS(timer->hpet),
                 hpet_read_reg(HPET_TIMER_CONF_CAPS(timer->hpet)) | (1 << 2));

  enable_interrupts();
}

static void hpet_int_deadline(void (*callback)(void *), void *data,
                              struct timer *timer_struct, uint64_t deadline) {
  disable_interrupts();

  uint64_t cur_hpet_ticks = hpet_read_reg(HPET_MAIN_COUNTER);
  uint64_t time_diff = deadline - abs_time();
  uint64_t hpet_tick_count = time_diff * frequency / abs_freq();
  struct hpet_timer *timer = (void *)timer_struct;
  callbacks[timer->hpet] = callback;
  datas[timer->hpet] = data;

  hpet_write_reg(HPET_TIMER_COMPARATOR_VAL(timer->hpet),
                 cur_hpet_ticks + hpet_tick_count);
  // Enable interrupts
  hpet_write_reg(HPET_TIMER_CONF_CAPS(timer->hpet),
                 hpet_read_reg(HPET_TIMER_CONF_CAPS(timer->hpet)) | (1 << 2));

  enable_interrupts();
}

INIT void init_hpet(void) {
  disable_interrupts();
  struct hpet_table *table = get_acpi_table("HPET");
  if (table == NULL) {
    kprintf("[HPET] HPET not supported\n");
    return;
  }

  hpet_regs = table->address + IDENTITY_MAP_OFFSET;
  map_phys((void *)(hpet_regs - IDENTITY_MAP_OFFSET),
           PM_RW | PM_WRITE_THROUGH | PM_PINNED);

  uint64_t capabilities = hpet_read_reg(HPET_GEN_CAPS);
  if ((capabilities & 0xFF) == 0) {
    kprintf("[HPET] HPET not supported\n");
    return;
  }

  comparator_count = ((capabilities >> 8) & 0x1F) + 1;
  counter_size = capabilities & (1 << 13);

  // We don't care about 32 bit hpet cuz its kinda annoying and useless
  if (!counter_size) {
    kprintf("[HPET] HPET not supported\n");
    return;
  }

  frequency = 1000000000000000 / (capabilities >> 32);

  // Set counter to 1 to not trigger interrupts
  hpet_write_reg(HPET_MAIN_COUNTER, 1);
  // Disable legacy remapping
  hpet_write_reg(HPET_GEN_CONF, 0);
  // Clear all pending interrupts
  hpet_write_reg(HPET_GEN_INT_STATUS, MAX_32);

  register_int_handler(0x90, hpet_handler);

  bool unmask_2 = false;
  bool unmask_16 = false;
  bool unmask_17 = false;
  bool unmask_18 = false;

  for (int i = 0; i < comparator_count; i++) {
    uint64_t timer_setup = hpet_read_reg(HPET_TIMER_CONF_CAPS(i));

    // Timer doesn't support 64 bit
    if (!(timer_setup & (1 << 5))) {
      continue;
    }

    comparator_sizes[i] = true;

    // Set the comparator to trigger with value 0
    // hpet_write_reg(HPET_TIMER_COMPARATOR_VAL(i), MAX_64);

    // Level Triggered
    timer_setup |= (1 << 1);
    // Disable Interrupts
    timer_setup &= ~(1 << 2);
    // One Shot
    timer_setup &= ~(1 << 3);
    // 64 bit mode
    timer_setup &= ~(1 << 8);
    // Normal mapping
    timer_setup &= ~(1 << 14);

    // Clear irq bits (5 bits)
    timer_setup &= ~(0x1F << 9);

    // Irqs 2, 16, 17, and 18 are allowed
    if ((timer_setup >> 32) & (1 << 2)) {
      timer_setup |= (2 << 9);
      configure_ioapic_entry(0x90, 0, false, false, true, true, 2);
      unmask_2 = true;
    } else if ((timer_setup >> 32) & (1 << 16)) {
      timer_setup |= (16 << 9);
      configure_ioapic_entry(0x90, 0, false, false, true, true, 16);
      unmask_16 = true;
    } else if ((timer_setup >> 32) & (1 << 17)) {
      timer_setup |= (17 << 9);
      configure_ioapic_entry(0x90, 0, false, false, true, true, 17);
      unmask_17 = true;
    } else if ((timer_setup >> 32) & (1 << 18)) {
      timer_setup |= (18 << 9);
      configure_ioapic_entry(0x90, 0, false, false, true, true, 18);
      unmask_18 = true;
    } else {
      comparator_sizes[i] = false;
      continue;
    }

    hpet_write_reg(HPET_TIMER_CONF_CAPS(i), timer_setup);

    struct hpet_timer *timer = gmalloc(sizeof(struct hpet_timer), ZONE_ANY);
    timer->timer.cancel = hpet_cancel;
    timer->timer.wait_microseconds = hpet_int_microseconds;
    timer->timer.wait_ms = hpet_int_ms;
    timer->timer.wait_deadline = hpet_int_deadline;
    timer->hpet = i;

    register_timer((void *)timer);

    kprintf("\t[HPET] Initialized HPET Comparator %d\n", i);
  }

  if (unmask_2) {
    unmask_ioapic_irq(2);
  }

  if (unmask_16) {
    unmask_ioapic_irq(16);
  }

  if (unmask_17) {
    unmask_ioapic_irq(17);
  }

  if (unmask_18) {
    unmask_ioapic_irq(18);
  }

  hpet_write_reg(HPET_GEN_INT_STATUS, MAX_32);
  enable_interrupts();
  // Enable counter
  hpet_write_reg(HPET_GEN_CONF, 1);
  hpet_write_reg(HPET_GEN_INT_STATUS, MAX_32);
}
