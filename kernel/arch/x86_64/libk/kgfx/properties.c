#include <libk/kgfx.h>
#include <libk/vga_kgfx.h>

vga_kgfx_properties_t global_properties;

void vga_kgfx_set_properties(vga_kgfx_properties_t properties) {
 global_properties = properties; 
}

vga_kgfx_properties_t vga_kgfx_get_properties(void) {
  return global_properties;
}

