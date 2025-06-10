#include <libk/kio.h>
#include <libk/spinlock.h>

static spinlock_t strLock;

void kio_puts(char *c) {
  spinlock_acquire(&strLock);
  uint16_t index = 0;

  while (c[index] != '\0') {
    kio_putchar(c[index]);
    index++;
  }

  spinlock_release(&strLock);
}
