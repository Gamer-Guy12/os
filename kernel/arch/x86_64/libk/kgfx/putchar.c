#include <libk/kgfx.h>
#include <libk/spinlock.h>
#include <libk/vga_kgfx.h>
#include <stdatomic.h>
#include <stddef.h>
#include <stdint.h>

uint32_t cursor_x = 0;
uint32_t cursor_y = 0;

spinlock_t char_lock = ATOMIC_FLAG_INIT;

void increment_cursor(void) {
  vga_kgfx_properties_t properties = vga_kgfx_get_properties();

  size_t text_width = properties.width / FONT_WIDTH;
  size_t text_height = properties.height / FONT_HEIGHT;

  cursor_x++;

  if (cursor_x >= text_width) {
    cursor_x = 0;
    cursor_y++;
  }

  if (cursor_y >= text_height) {
    kgfx_scroll();
    cursor_y--;
  }
}

void decrement_cursor(void) {
  vga_kgfx_properties_t properties = vga_kgfx_get_properties();

  size_t text_width = properties.width / FONT_WIDTH;

  if (cursor_x == 0) {
    if (cursor_y == 0)
      return;

    cursor_x = text_width;
    cursor_y--;
  }

  cursor_x--;
}

void handle_escape(char c) {
  vga_kgfx_properties_t properties = vga_kgfx_get_properties();

  size_t text_height = properties.height / FONT_HEIGHT;

  switch (c) {
  case '\n':
    cursor_x = 0;
    cursor_y++;

    if (cursor_y >= text_height) {
      kgfx_scroll();
      cursor_y--;
    }

    break;
  case '\b':
    decrement_cursor();
    vga_kgfx_print_glyph(cursor_x, cursor_y, 0, 0, 0, 0, FONT_SCALE);
    break;
  case '\r':
    cursor_x = 0;
    break;
  case '\t':
    increment_cursor();
    increment_cursor();
    break;
  default:
    break;
  }
}

static void handle_text(char c) {

  vga_kgfx_print_glyph(cursor_x, cursor_y, 255, 255, 255, vga_kgfx_get_glyph(c),
                       FONT_SCALE);

  increment_cursor();
}

void kgfx_putchar(char c) {
  spinlock_acquire(&char_lock);

  if (c == ' ') {
    vga_kgfx_print_glyph(cursor_x, cursor_y, 0, 0, 0, 0, FONT_SCALE);
    increment_cursor();
  } else if (c == 0) {
    
  } else if ((uint8_t)c < 32) {
    handle_escape(c);
  } else {
    handle_text(c);
  }

  spinlock_release(&char_lock);
}

void vga_kgfx_set_cursor(uint32_t x, uint32_t y) {
  cursor_y = y;
  cursor_x = x;
}

uint32_t vga_kgfx_get_row(void) {
  return cursor_y;
}
