#include "lib/spinlock.h"
#include "interrupts.h"
#include "kernel/console.h"
#include "kernel/cores.h"
#include "kernel/kprintf.h"
#include "lib/atomic.h"
#include "lib/string.h"
#include "util.h"
#include <stdbool.h>

void spinlock_acquire(spinlock_t *spinlock) {
  disable_interrupts();
#ifdef _DEBUG_
  if (spinlock->current_core == get_core_id()) {
    kprintf("Deadlock detected on lock: ");
    for (int i = 0; i < strlen(spinlock->name); i++) {
      console_putchar(spinlock->name[i]);
    }
    kprintf("\n");
    panic();
  }
#endif

  while (!atomic_cas(&spinlock->val, 0, 1)) {
    __asm__ volatile("pause" ::: "memory");
  }
#ifdef _DEBUG_
  spinlock->current_core = get_core_id();
#endif
}

bool spinlock_attempt(spinlock_t *spinlock) {
  disable_interrupts();
  if (atomic_cas(&spinlock->val, 0, 1)) {
    enable_interrupts();
    return true;
  }
  enable_interrupts();
  return false;
}

void spinlock_release(spinlock_t *spinlock) {
#ifdef _DEBUG_
  spinlock->current_core = -1;
#endif
  atomic_store(&spinlock->val, 0);
  enable_interrupts();
}
