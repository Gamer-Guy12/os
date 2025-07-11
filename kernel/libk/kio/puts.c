#include <libk/kgfx.h>
#include <libk/kio.h>
#include <libk/spinlock.h>
#include <stdatomic.h>

static spinlock_t strLock = ATOMIC_FLAG_INIT;

void kio_puts(char *c) {
  spinlock_acquire(&strLock);
  uint16_t index = 0;

  while (c[index] != '\0') {
    kgfx_putchar(c[index]);
    index++;
  }

  spinlock_release(&strLock);
}
