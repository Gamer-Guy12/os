#include <libk/kgfx.h>
#include <libk/vga_kgfx.h>
#include <mem/pimemory.h>
#include <multiboot.h>
#include <stddef.h>

void kgfx_init(void) {
  mltbt_framebuffer_info_t *framebuffer =
      multiboot_get_tag(MLTBT_FRAMEBUFFER_INFO);

  vga_kgfx_properties_t properties;
  properties.width = framebuffer->framebuffer_width;
  properties.height = framebuffer->framebuffer_height;
  properties.framebuffer_pitch = framebuffer->framebuffer_pitch;
  properties.framebuffer_bits_pp = framebuffer->framebuffer_bpp;
  properties.framebuffer_addr = (void *)VGA_MEM_ADDR;

  mltbt_framebuffer_type_1_t *type_1_info =
      (void *)((size_t)framebuffer + sizeof(mltbt_framebuffer_info_t));
  properties.framebuffer_blue_field_pos =
      type_1_info->framebuffer_blue_field_pos;
  properties.framebuffer_blue_mask_size =
      type_1_info->framebuffer_blue_mask_size;
  properties.framebuffer_green_field_pos =
      type_1_info->framebuffer_green_field_pos;
  properties.framebuffer_green_mask_size =
      type_1_info->framebuffer_green_mask_size;
  properties.framebuffer_red_field_pos = type_1_info->framebuffer_red_field_pos;
  properties.framebuffer_red_mask_size = type_1_info->framebuffer_red_mask_size;

  vga_kgfx_set_properties(properties);
}
