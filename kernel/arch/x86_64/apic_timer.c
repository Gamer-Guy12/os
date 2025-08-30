#include <apic.h>
#include <apic_timer.h>
#include <asm.h>
#include <cls.h>
#include <decls.h>
#include <interrupts.h>
#include <irq.h>
#include <libk/err.h>
#include <libk/spinlock.h>
#include <libk/sys.h>
#include <stdatomic.h>
#include <stddef.h>
#include <stdint.h>
#include <threading.h>

#define IA32_TSC_DEADLINE_MSR 0x6E0

#define PIT_16_BINARY 0
#define PIT_4_BCD 1

#define PIT_MODE_0 0
#define PIT_MODE_1 (1 << 1)
#define PIT_MODE_2 (2 << 1)
#define PIT_MODE_3 (3 << 1)
#define PIT_MODE_4 (4 << 1)
#define PIT_MODE_5 (5 << 1)
/// Same as PIT_MODE_2
#define PIT_MODE_2_2 (6 << 1)
/// Same as PIT_MODE_3
#define PIT_MODE_3_2 (7 << 1)

#define PIT_LATCH_COUNT_VAL 0
#define PIT_LOBYTE (1 << 4)
#define PIT_HIBYTE (2 << 4)
#define PIT_LOHIBYTE (3 << 4)

#define PIT_CHANNEL_0 0
#define PIT_CHANNEL_1 (1 << 6)
#define PIT_CHANNEL_2 (2 << 6)
#define PIT_READ_BACK (3 << 6)

#define PIT_RB_LATCH_COUNT 0
#define PIT_RB_NO_LATCH_COUNT (1 << 5)
#define PIT_RB_LATCH_STATUS 0
#define PIT_RB_NO_LATCH_STATUS (1 << 4)
#define PIT_RB_CHANNEL_2 (1 << 3)
#define PIT_RB_CHANNEL_1 (1 << 2)
#define PIT_RB_CHANNEL_0 (1 << 1)

#define PIT_DATA_0 0x40
#define PIT_DATA_1 0x41
#define PIT_DATA_2 0x42
#define PIT_COMMAND 0x43

#define WRITE_PIT_DATA_0(data) outb(0x40, data)
#define WRITE_PIT_DATA_1(data) outb(0x41, data)
#define WRITE_PIT_DATA_2(data) outb(0x42, data)
#define WRITE_PIT_COMMAND(command) outb(0x43, command)

#define PIT_FREQUENCY 1193182

size_t tsc_frequency = 0;
size_t apic_frequency = 0;
spinlock_t frequency_lock = ATOMIC_FLAG_INIT;

static void dummy_irq(idt_registers_t *regs) {
  irq_t irq = get_irq();
  irq.eoi();
}

void (*interrupt_callback)(uint64_t tsc_deadline,
                           void (*callback)(void)) = NULL;

/// The APIC should have masked everything so this shouldn't make an interrupt
void calculate_frequency(void) {
  spinlock_acquire(&frequency_lock);

  if (tsc_frequency != 0) {
    spinlock_release(&frequency_lock);
    return;
  }

  write_apic_register(TIMER_DIV_CONFIG_REG, TIMER_DIV_16);

  register_interrupt_handler(dummy_irq, 0x60);

  // This will run for 25 ms or 1/40 of a second
  // The clock should be in 1 shot mode at a speed of 40 hz
  // This means that the counter should be set to 1193182 / 40
  // which is 29830
  uint16_t count = 29830;
  WRITE_PIT_COMMAND(PIT_16_BINARY | PIT_MODE_0 | PIT_LOHIBYTE | PIT_CHANNEL_0);
  io_wait();
  WRITE_PIT_DATA_0(count && 0xff);
  io_wait();
  WRITE_PIT_DATA_0((count && 0xff00) >> 8);

  const size_t start_tsc = rdtsc();

  // wait for pit to finish

  while (true) {
    WRITE_PIT_COMMAND(PIT_LATCH_COUNT_VAL | PIT_CHANNEL_0);
    io_wait();
    uint16_t value = inb(PIT_DATA_0);
    value |= inb(PIT_DATA_0) << 8;

    if (value == 0) {
      break;
    }
  }

  const size_t end_tsc = rdtsc();
  const size_t time_passed = end_tsc - start_tsc;

  /// The timer ran at 40 hz which means we need to divide time passed by 40
  tsc_frequency = time_passed / 40;

  write_apic_register(TIMER_DIV_CONFIG_REG, TIMER_DIV_2);
  write_apic_register(LVT_TIMER_REG, LVT_VECTOR(0x60) | TIMER_ONE_SHOT);

  WRITE_PIT_COMMAND(PIT_16_BINARY | PIT_MODE_0 | PIT_LOHIBYTE | PIT_CHANNEL_0);
  io_wait();
  WRITE_PIT_DATA_0(count && 0xff);
  io_wait();
  WRITE_PIT_DATA_0((count && 0xff00) >> 8);

  write_apic_register(TIMER_INITIAL_COUNT_REG, -1);

  while (true) {
    WRITE_PIT_COMMAND(PIT_LATCH_COUNT_VAL | PIT_CHANNEL_0);
    io_wait();
    uint16_t value = inb(PIT_DATA_0);
    value |= inb(PIT_DATA_0) << 8;

    if (value == 0) {
      break;
    }
  }

  write_apic_register(LVT_TIMER_REG, LVT_MASK);

  size_t current_value = read_apic_register(TIMER_CUR_COUNT_REG);
  size_t difference = MAX_32 - current_value;
  write_apic_register(TIMER_INITIAL_COUNT_REG, 0);

  // This was done at half speed so the apic in reality runs 2 times as fast
  // But this was done over the course of 25 ms aka 40 hz so multiply this by 40
  // to get how many times it would have gone in a second. This is because this
  // ran for 1/40th of a second
  apic_frequency = difference * 2 * 40;

  spinlock_release(&frequency_lock);
}

static void handler(idt_registers_t *registers) {
  cls_t *cls = get_cls();
  irq_t irq = get_irq();
  irq.eoi();

  wrmsr(IA32_TSC_DEADLINE_MSR, 0);
  write_apic_register(LVT_TIMER_REG, LVT_MASK);

  if (cls->apic_timer_callback == NULL) {
    return;
  } else {
    void (*callback)(void) = cls->apic_timer_callback;
    cls->apic_timer_callback = NULL;

    callback();
  }
}

void apic_interrupt_at(size_t tsc_deadline, void (*callback)(void)) {
  interrupt_callback(tsc_deadline, callback);
}

void apic_interrupt_at_tsc(size_t tsc_deadline, void (*callback)(void)) {
  cls_t *cls = get_cls();
  spinlock_acquire(&cls->apic_timer_lock);

  cls->apic_timer_callback = callback;

  /// Idk if this matters but imma set it anyways
  write_apic_register(TIMER_DIV_CONFIG_REG, TIMER_DIV_1);

  register_interrupt_handler(handler, 0x60);
  write_apic_register(LVT_TIMER_REG, LVT_VECTOR(0x60) | TIMER_TSC);

  wrmsr(IA32_TSC_DEADLINE_MSR, tsc_deadline);

  spinlock_release(&cls->apic_timer_lock);
}

void apic_interrupt_at_oneshot(uint64_t tsc_deadline, void (*callback)(void)) {
  cls_t *cls = get_cls();
  spinlock_acquire(&cls->apic_timer_lock);

  /// Convert TSC Deadline back into ms and then into apic ticks
  const size_t ms = (tsc_deadline * 1000 / tsc_frequency) - rdtsc();
  const size_t ticks = (ms * apic_frequency) / 1000;

  cls->apic_timer_callback = callback;

  register_interrupt_handler(handler, 0x60);

  write_apic_register(TIMER_DIV_CONFIG_REG, TIMER_DIV_2);
  write_apic_register(LVT_TIMER_REG, LVT_VECTOR(0x60) | TIMER_ONE_SHOT);
  /// Because the timer runs at half speed, we must divide by 2 to get the
  /// accurate amount;
  write_apic_register(TIMER_INITIAL_COUNT_REG, ticks / 2);

  spinlock_release(&cls->apic_timer_lock);
}

void init_apic_timer(void) {
  /// Check for tsc deadline
  uint32_t a = 0;
  uint32_t b = 0;
  uint32_t c = 0;
  uint32_t d = 0;

  register_interrupt_handler(handler, 0x60);
  cpuid(0x1, &a, &d, &c, &b);
  if ((c & (1 << 24))) {
    interrupt_callback = apic_interrupt_at_tsc;
  } else {
    interrupt_callback = apic_interrupt_at_oneshot;
  }

  calculate_frequency();
  write_apic_register(LVT_TIMER_REG, LVT_MASK);

  STI;
  ASM("sti");
}

size_t ms_to_deadline(size_t ms) {
  size_t cycles = (ms * tsc_frequency) / 1000;

  return rdtsc() + cycles;
}
