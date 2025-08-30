#include "libk/kio.h"
#include <apic_timer.h>
#include <asm.h>
#include <cls.h>
#include <threading.h>
#include <threading/threading.h>

static void handler(void *data) {
  kio_printf("Here\n");
  run_preemption();

  if (get_cls()->preemption_enabled) {
    run_next_thread();
  }
}

void run_preemption(void) {
  schedule_event(QUANTUM_LENGTH, NULL, handler);
}

void enable_preemption(void) {
  STI;
  get_cls()->preemption_enabled = true;
}

void disable_preemption(void) {
  CLI;
  get_cls()->preemption_enabled = false;
}
