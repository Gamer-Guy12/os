#include <libk/kgfx.h>
#include <libk/math.h>
#include <libk/mem.h>
#include <libk/vga_kgfx.h>
#include <stddef.h>
#include <stdint.h>

void clear_row(size_t row) {
  vga_kgfx_properties_t properties = vga_kgfx_get_properties();
  const size_t glyph_count =
      ROUND_DOWN(properties.width, FONT_WIDTH) / FONT_WIDTH;

  for (size_t i = 0; i < glyph_count; i++) {
    vga_kgfx_print_glyph(i, row, 256, 256, 256, 0, FONT_SCALE);
  }
}

void move_row_up(size_t row) {
  if (row == 0)
    return;

  vga_kgfx_properties_t properties = vga_kgfx_get_properties();

  size_t addr = (size_t)properties.framebuffer_addr;

  size_t cur_row = addr + properties.framebuffer_pitch * row * FONT_HEIGHT;
  size_t prev_row =
      addr + properties.framebuffer_pitch * (row - 1) * FONT_HEIGHT;

  for (size_t i = 0; i < FONT_HEIGHT; i++) {
    memcpy((void *)(cur_row + i * properties.framebuffer_pitch),
           (void *)(prev_row + i * properties.framebuffer_pitch),
           properties.framebuffer_pitch);
  }
}

void kgfx_scroll(void) {
  vga_kgfx_properties_t properties = vga_kgfx_get_properties();
  size_t row_count = ROUND_DOWN(properties.height, FONT_HEIGHT) / FONT_HEIGHT;

  for (size_t i = 1; i < row_count; i++) {
    move_row_up(i);
  }

  clear_row(row_count - 1);
}
