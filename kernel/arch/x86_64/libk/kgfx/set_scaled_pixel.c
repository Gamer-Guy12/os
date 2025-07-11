#include <libk/kgfx.h>
#include <libk/vga_kgfx.h>
#include <stddef.h>

void vga_kgfx_set_scaled_pixel(uint32_t x, uint32_t y, uint32_t r, uint32_t g,
                               uint32_t b, uint32_t scale) {
  vga_kgfx_properties_t properties = vga_kgfx_get_properties();

  size_t scaled_x = x * scale;
  size_t scaled_y = y * scale;

  if (scaled_x > properties.width || scaled_y > properties.height) {
    return;
  }

  for (size_t i = 0; i < scale; i++) {
    for (size_t j = 0; j < scale; j++) {
      vga_kgfx_set_pixel(scaled_x + i, scaled_y + j, r, g, b);
    }
  }
}
