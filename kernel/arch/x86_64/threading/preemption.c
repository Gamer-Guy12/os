#include "libk/kio.h"
#include <asm.h>
#include <cls.h>
#include <threading.h>
#include <threading/threading.h>

static void handler(void *data) {
  // get_cls()->preemption_handle = 0;

  if (get_cls()->preemption_enabled) {
    run_next_thread();
  }

  run_preemption();
}

void run_preemption(void) {
  cls_t *cls = get_cls();

  if (cls->preemption_handle != 0) {
    cancel_event(cls->preemption_handle);
  }

  cls->preemption_handle = schedule_event(QUANTUM_LENGTH, NULL, handler);
}

void enable_preemption(void) {
  get_cls()->preemption_enabled = true;
  STI;
}

void disable_preemption(void) {
  CLI;
  get_cls()->preemption_enabled = false;
}
