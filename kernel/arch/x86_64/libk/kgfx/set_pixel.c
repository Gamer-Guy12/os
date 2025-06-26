#include <libk/bit.h>
#include <libk/kgfx.h>
#include <libk/vga_kgfx.h>
#include <stddef.h>
#include <stdint.h>

void vga_kgfx_set_pixel(uint32_t x, uint32_t y, uint32_t r, uint32_t g,
                        uint32_t b) {

  vga_kgfx_properties_t properties = vga_kgfx_get_properties();

  size_t row_offset = y * properties.framebuffer_pitch;
  size_t column_bit_offset = x * properties.framebuffer_bits_pp;

  uint64_t data = 0;

  // Set r 
  for (size_t i = 0; i < properties.framebuffer_red_mask_size; i++) {
    if (r & (1 << i)) {
      data |= 1 << (i + properties.framebuffer_red_field_pos);
    }
  }

  // Set g
  for (size_t i = 0; i < properties.framebuffer_green_mask_size; i++) {
    if (g & (1 << i)) {
      data |= 1 << (i + properties.framebuffer_green_field_pos);
    }
  }

  // Set b
  for (size_t i = 0; i < properties.framebuffer_blue_mask_size; i++) {
    if (b & (1 << i)) {
      data |= 1 << (i + properties.framebuffer_blue_field_pos);
    }
  }

  __asm__ volatile("movq %0, %%r9" ::"r"(data):"r9");
  __asm__ volatile("movq %0, %%r10" ::"r"((size_t)properties.framebuffer_red_field_pos):"r10");
  __asm__ volatile("movq %0, %%r11" ::"r"((size_t)properties.framebuffer_green_field_pos):"r11");
  __asm__ volatile("movq %0, %%r12" ::"r"((size_t)properties.framebuffer_blue_field_pos):"r12");
  __asm__ volatile("movq %0, %%r13" ::"r"((size_t)properties.framebuffer_bits_pp):"r13");
  __asm__ volatile("movq %0, %%r14" ::"r"((size_t)properties.framebuffer_red_mask_size):"r14");

  uint8_t* ptr = (void*)0x0;
  *ptr = 48;

  // Set in framebuffer
  uint8_t* offset_framebuffer = (uint8_t*)properties.framebuffer_addr + row_offset;

  for (size_t i = 0; i < properties.framebuffer_bits_pp; i++) {
    if (data & (1 << i)) {
      set_bit_in_ptr(offset_framebuffer, i + column_bit_offset);
    }
  }
}
