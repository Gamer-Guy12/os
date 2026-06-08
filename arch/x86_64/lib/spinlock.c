#include "lib/spinlock.h"
#include "interrupts.h"
#include "kernel/console.h"
#include "kernel/cores.h"
#include "kernel/kprintf.h"
#include "util.h"
#include <stdbool.h>

#ifdef _DEBUG_
#include "lib/string.h"
#endif

// Interrupts are disabled while this function is in use
void _spinlock_acquire(spinlock_t *spinlock) {
  RMEMB();
#ifdef _DEBUG_
  if (spinlock->current_core == get_core_id()) {
    _kprintf("Deadlock detected on lock: ");
    for (int i = 0; i < strlen(spinlock->name); i++) {
      console_putchar(spinlock->name[i]);
    }
    _kprintf("\n");
    panic();
  }
#endif
  size_t index = __atomic_fetch_add(&spinlock->cur_index, 1, __ATOMIC_RELEASE);

  while (__atomic_load_n(&spinlock->running_index, __ATOMIC_ACQUIRE) != index) {
    __builtin_ia32_pause();
  }

#ifdef _DEBUG_
  spinlock->current_core = get_core_id();
#endif
}

void _spinlock_release(spinlock_t *spinlock) {
#ifdef _DEBUG_
  spinlock->current_core = -1;
#endif
  __atomic_fetch_add(&spinlock->running_index, 1, __ATOMIC_RELEASE);
}

void spinlock_acquire(spinlock_t *spinlock) {
  disable_interrupts();
#ifdef _DEBUG_
  if (spinlock->current_core == get_core_id()) {
    _kprintf("Deadlock detected on lock: ");
    for (int i = 0; i < strlen(spinlock->name); i++) {
      console_putchar(spinlock->name[i]);
    }
    _kprintf("\n");
    panic();
  }
#endif
  size_t index = __atomic_fetch_add(&spinlock->cur_index, 1, __ATOMIC_RELEASE);

  while (__atomic_load_n(&spinlock->running_index, __ATOMIC_ACQUIRE) != index) {
    __builtin_ia32_pause();
  }

#ifdef _DEBUG_
  spinlock->current_core = get_core_id();
#endif
}

void spinlock_release(spinlock_t *spinlock) {
#ifdef _DEBUG_
  spinlock->current_core = -1;
#endif
  __atomic_fetch_add(&spinlock->running_index, 1, __ATOMIC_RELEASE);
  enable_interrupts();
  WMEMB();
}
