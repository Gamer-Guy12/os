#ifndef X86_64_VGA_KGFX_H
#define X86_64_VGA_KGFX_H

#include <stdint.h>

typedef struct {
  uint32_t width;
  uint32_t height;
  // Copied from multiboot.h
  uint8_t framebuffer_red_field_pos;
  uint8_t framebuffer_red_mask_size;
  uint8_t framebuffer_green_field_pos;
  uint8_t framebuffer_green_mask_size;
  uint8_t framebuffer_blue_field_pos;
  uint8_t framebuffer_blue_mask_size;
  uint32_t framebuffer_pitch;
  uint8_t framebuffer_bits_pp;
  void *framebuffer_addr;
} vga_kgfx_properties_t;

void vga_kgfx_set_properties(vga_kgfx_properties_t properties);
vga_kgfx_properties_t vga_kgfx_get_properties(void);

void vga_kgfx_set_pixel(uint32_t x, uint32_t y, uint32_t r, uint32_t g,
                        uint32_t b);
void vga_kgfx_print_ascii(uint32_t x, uint32_t y, char c);

/// Scale is in pixels
/// Coordinates are also scaled with it
void vga_kgfx_set_scaled_pixel(uint32_t x, uint32_t y, uint32_t r, uint32_t g,
                               uint32_t b, uint32_t scale);
/// Coordinates are scaled to 8 * scale
void vga_kgfx_print_glyph(uint32_t x, uint32_t y, uint32_t r, uint32_t g,
                          uint32_t b, uint64_t glyph, uint32_t scale);

void vga_kgfx_init_font(void);
uint64_t vga_kgfx_get_glyph(char c);

#endif
