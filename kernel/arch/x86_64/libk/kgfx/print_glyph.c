#include <libk/kgfx.h>
#include <libk/vga_kgfx.h>
#include <stddef.h>

void vga_kgfx_print_glyph(uint32_t x, uint32_t y, uint32_t r, uint32_t g,
                          uint32_t b, uint64_t glyph, uint32_t scale) {
  for (size_t i = 0; i < 8; i++) {
    for (size_t j = 0; j < 8; j++) {
      size_t glyph_index = i + 8 * j;

      size_t scaled_x = x * 8 + i;
      size_t scaled_y = y * 8 + j;

      if (glyph & (1ull << glyph_index)) {
        vga_kgfx_set_scaled_pixel(scaled_x, scaled_y, r, g, b, scale);
      } else {
        vga_kgfx_set_scaled_pixel(scaled_x, scaled_y, 0, 0, 0, scale);
      }
    }
  }
}
