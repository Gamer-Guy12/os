#include "lib/spinlock.h"
#include "kernel/console.h"
#include "kernel/cores.h"
#include "kernel/kprintf.h"
#include "util.h"
#include <stdbool.h>

#ifdef _DEBUG_
#include "lib/string.h"
#endif

void _spinlock_acquire(spinlock_t *spinlock) {
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
  size_t index = __atomic_fetch_add(&spinlock->cur_index, 1, __ATOMIC_RELEASE);

  while (__atomic_load_n(&spinlock->running_index, __ATOMIC_ACQUIRE) != index) {
    __asm__ volatile("pause" ::: "memory");
  }

  // Just incase someone has acquired it through attempt instead of acquire
  // Will always succeed first tree if the lock never uses attempt
  while (!atomic_cas(&spinlock->value, 0, 1)) {
    __asm__ volatile("pause" ::: "memory");
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
  atomic_store(&spinlock->value, 0);
}
