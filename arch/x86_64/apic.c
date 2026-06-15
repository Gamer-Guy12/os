#include "apic.h"
#include "asm.h"
#include "interrupts.h"
#include "kernel/cores.h"
#include "kernel/kprintf.h"
#include "kernel/threads.h"
#include "pit.h"
#include "util.h"
#include <stdint.h>

CLS(uint32_t, core_apic_freq);
void enable_apic(void) {
  // PIC is already masked so nothing has to be done

  // Set Spurious Interrupt Vector and TPR
  apic_write(LAPIC_TPR_REG, 0);
  // Spurious interrupt is vector 255 and (1 << 8) enables the apic
  apic_write(LAPIC_SVR_REG, 0xFF | (1 << 8));
}

bool apic_up(void) { return apic_read(LAPIC_SVR_REG) & (1 << 8); }

void apic_write(uint16_t reg, uint64_t value) {
  WRMSR(APIC_MSR_BASE + reg, value);
}

uint64_t apic_read(uint16_t reg) { return rdmsr(APIC_MSR_BASE + reg); }

void apic_eoi(void) { apic_write(LAPIC_EOI_REG, 0); }

void apic_ipi(int type, uint32_t dest, uint8_t interrupt) {
  uint64_t data = (uint64_t)dest << 32;
  data |= interrupt;
  // Edge triggered, assert, and physical destination
  switch (type) {
  case IPI_NORMAL:
    data |= (0x0 << 18);
    break;
  case IPI_BROADCAST:
    data |= (0x3 << 18);
    break;
  case IPI_ALL:
    data |= (0x2 << 18);
    break;
  default:
    kprintf("Invalid IPI type 0x%x\n", type);
    break;
  }

  apic_write(LAPIC_ICR_REG, data);
}

static void timer_handler(void *_) {
  apic_eoi();
  preempt();
}

static void setup_apic_timer(void) {
  // 25 ms countdown
  pit_prep(25);
  apic_write(LAPIC_TDC_REG, LAPIC_DIV_4);
  apic_write(LAPIC_TIMER_REG,
             LVT_VECTOR(0x90) | LVT_MASK | LAPIC_TIMER_ONESHOT);
  apic_write(LAPIC_TIC_REG, MAX_32);
  pit_count();
  const uint32_t count = apic_read(LAPIC_TCC_REG);
  // stop timer
  apic_write(LAPIC_TIC_REG, 0);
  const uint32_t diff = MAX_32 - count;
  // diff is how many ticks have happened in 25 ms
  // mulitply by 4 because the div_4 made them happen 4 times less
  // multiply by 40 because 1 second contains 40 25 ms periods
  const uint32_t freq = diff * 4 * 40;
  uint32_t *local_freq = GET_CLS(core_apic_freq);
  *local_freq = freq;

  // Uses interrupt 0x90
  register_interrupt(timer_handler, 0x90);
}

void do_preemption(void) {
  disable_interrupts();
  // THe / 4 is to account for the fact that ticks come 4 times less often
  uint32_t ticks =
      THREAD_QUANTUM * *((uint32_t *)GET_CLS(core_apic_freq)) / 4 / 1000;

  apic_write(LAPIC_TDC_REG, LAPIC_DIV_4);
  apic_write(LAPIC_TIMER_REG, LVT_VECTOR(0x90) | LAPIC_TIMER_PERIODIC);
  apic_write(LAPIC_TIC_REG, ticks);
  enable_interrupts();
}

void init_apic(void) {
  check_apic();
  enable_apic();
  setup_apic_timer();
}
INITFUNC(init_apic, CALL_CLS);
