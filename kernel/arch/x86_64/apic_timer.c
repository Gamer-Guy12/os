#include "libk/kio.h"
#include <libk/spinlock.h>
#include <irq.h>
#include <apic.h>
#include <apic_timer.h>
#include <asm.h>
#include <interrupts.h>
#include <stdatomic.h>
#include <stddef.h>
#include <stdint.h>
#include <threading.h>

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

size_t frequency = 0;
spinlock_t frequency_lock = ATOMIC_FLAG_INIT;

void on_preempt(idt_registers_t *regs) {
  // __asm__ volatile ("mov $0x38, %rax; mov $0x0, %rbx; div %rbx");
  irq_t irq = get_irq();
  irq.eoi();
  return;
  run_next_thread();
}

static void dummy_irq(idt_registers_t* regs) {
  irq_t irq = get_irq();
  irq.eoi();
}

/// The APIC should have masked everything so this shouldn't make an interrupt
void calculate_frequency(void) {
  spinlock_acquire(&frequency_lock);
  
  if (frequency != 0) {
    spinlock_release(&frequency_lock);
    return;
  }

  write_apic_register(TIMER_DIV_CONFIG_REG, TIMER_DIV_16);

  register_interrupt_handler(dummy_irq, 0x60);

  write_apic_register(LVT_TIMER_REG, LVT_VECTOR(0x60) | TIMER_ONE_SHOT);

  /// The apic will count down from max int
  uint32_t apic_count = -1;

  // This will run for 25 ms or 1/40 of a second
  // The clock should be in 1 shot mode at a speed of 40 hz
  // This means that the counter should be set to 1193182 / 40
  // which is 29830
  uint16_t count = 29830;
  CLI;
  WRITE_PIT_COMMAND(PIT_16_BINARY | PIT_MODE_0 | PIT_LOHIBYTE | PIT_CHANNEL_0);
  io_wait();
  WRITE_PIT_DATA_0(count && 0xff);
  io_wait();
  WRITE_PIT_DATA_0((count && 0xff00) >> 8);
  
  write_apic_register(TIMER_INITIAL_COUNT_REG, apic_count);

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

  // Stop the timer
  write_apic_register(LVT_TIMER_REG, LVT_VECTOR(0x60) | LVT_MASK);

  // Get the current apic time count
  uint32_t cur_count = read_apic_register(TIMER_CUR_COUNT_REG);
  
  // We used a 16 divider so multiply by 16 and also make it into a number that counted up
  // We are using a uint32_t so its important that we set the same bits 
  uint32_t bits_set = -1;
  size_t calculation_frequency = bits_set;
  calculation_frequency -= cur_count;
  calculation_frequency *= 16;

  // We ran the pit in oneshot a ta frequency of 40 hz
  calculation_frequency *= 40;
  frequency = calculation_frequency;

  STI;
  spinlock_release(&frequency_lock);
}

void init_apic_timer(void) {
  calculate_frequency();
}

void enable_preemption(void) {
  // Assumptions: This CPU supports cpuid leaf 0x15, This CPU is using an
  // integrated LAPIC and not discrete This means that I can use the core
  // crystal frequency from ecx in leaf 0x15 to find the frequency of the APIC
  // Timer

  // Divide by 2 should be used
  // This is because on the wiki it says bochs cant handle 1
  // so we must be nice
  write_apic_register(TIMER_DIV_CONFIG_REG, TIMER_DIV_2);

  // The preemption vector is 0x60
  register_interrupt_handler(on_preempt, 0x60);

  // Set up LVT register
  write_apic_register(LVT_TIMER_REG, LVT_VECTOR(0x60) | TIMER_PERIODIC);

  // This is in hertz
  const size_t targeted_frequency = 1000 / QUANTUM_LENGTH;
  // The timer runs at half the speed of the clock this means that we want to wait for half as many ticks
  const size_t count_value = (frequency / targeted_frequency) / 2;
  write_apic_register(TIMER_INITIAL_COUNT_REG, count_value);
}

void disable_preemption(void) {
  write_apic_register(TIMER_INITIAL_COUNT_REG, 0);
}
