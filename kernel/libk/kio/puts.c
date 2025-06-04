#include <libk/kio.h>
#include <libk/lock.h>

static lock_t strLock;

void kio_puts(char *c) {
  lock_acquire(&strLock);
  uint16_t index = 0;

  while (c[index] != '\0') {
    kio_putchar(c[index]);
    index++;
  }

  lock_release(&strLock);
}
