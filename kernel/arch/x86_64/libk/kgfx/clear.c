#include <libk/kgfx.h>
#include <libk/vga_kgfx.h>
#include <stddef.h>

void kgfx_clear(void) {
  vga_kgfx_properties_t properties = vga_kgfx_get_properties();

  for (size_t i = 0; i < properties.width; i++) {
    for (size_t j = 0; j < properties.height; j++) {
      vga_kgfx_set_pixel(i, j, 0, 0, 0);
    }
  }

  vga_kgfx_set_cursor(0, 0);
}
