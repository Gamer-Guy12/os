#include <libk/kgfx.h>
#include <libk/kio.h>
#include <libk/lock.h>

static uint16_t curX = 0;
static uint16_t curY = 0;
static uint16_t width = 0;
static uint16_t height = 0;

static lock_t charLock;

void kio_putchar(char c) {
  lock_acquire(&charLock);
  KGFXProperties props = kgfx_getproperties();
  width = props.width;
  height = props.height;

  if (c == '\n') {
    curX = 0;
    curY++;

    lock_release(&charLock);
    return;
  } else if (c == '\r') {
    curX = 0;

    lock_release(&charLock);
    return;
  } else if (c == '\b') {
    if (curX == 0) {

      lock_release(&charLock);
      return;
    }

    curX--;

    lock_release(&charLock);
    return;
  } else if (c == '\t') {
    if (curX >= width - 1) {

      lock_release(&charLock);
      return;
    }

    curX++;

    lock_release(&charLock);
    return;
  }

  if (curX >= width) {
    curY++;
    curX = 0;
  }

  if (curY >= height) {
    kgfx_scroll();
    curY--;
    curX = 0;
  }

  kgfx_putcharat(curX, curY, c);
  curX++;

  lock_release(&charLock);
}

void kio_clear(void) {
  kgfx_clear();
  curX = 0;
  curY = 0;
}
