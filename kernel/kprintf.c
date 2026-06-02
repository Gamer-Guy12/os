// This shit is EXTREMELY dangerous and may not even work so may have to change
// later

#include "kernel/kprintf.h"
#include "kernel/console.h"
#include "lib/io.h"
#include "lib/spinlock.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>

static spinlock_t print_lock = SPINLOCK_ZERO(print_lock);

int _kprintf(const char *format, ...) {
  va_list list;
  va_start(list, format);
  va_list list2;
  va_copy(list2, list);
  int len = vsnprintf(NULL, 0, format, list);
  char buf[len];
  vsnprintf(buf, len, format, list2);

  _spinlock_acquire(&print_lock);
  // - 1 to skip null
  for (int i = 0; i < len - 1; i++) {
    console_putchar(buf[i]);
  }
  _spinlock_release(&print_lock);

  va_end(list);
  return 0;
}

int kprintf(const char *format, ...) {
  va_list list;
  va_start(list, format);
  va_list list2;
  va_copy(list2, list);
  int len = vsnprintf(NULL, 0, format, list);
  char buf[len];
  vsnprintf(buf, len, format, list2);

  spinlock_acquire(&print_lock);
  // - 1 to skip null
  for (int i = 0; i < len - 1; i++) {
    console_putchar(buf[i]);
  }
  spinlock_release(&print_lock);

  va_end(list);
  return 0;
}
