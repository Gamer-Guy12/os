#include "kernel/console.h"
#include "util.h"
#include <limine.h>
#include <stddef.h>
#include <stdint.h>

LIMINE_REQUEST static volatile struct limine_framebuffer_request
    framebuffer_request = {.id = LIMINE_FRAMEBUFFER_REQUEST, .revision = 0};

uint8_t red_bits;
uint8_t red_shift;
uint8_t green_bits;
uint8_t green_shift;
uint8_t blue_bits;
uint8_t blue_shift;

/// How wide the screen is (pixels)
uint64_t width;
/// How tall the screen is (pixels)
uint64_t height;
/// How many bytes in a row
uint64_t pitch;
/// Bits per pixel
uint16_t bpp;

struct limine_framebuffer *framebuffer;

// Color is always white
static void console_put_pixel(uint64_t x, uint64_t y) {
  uint64_t data = 0;

  // Add the red component
  data |= ((1 << red_bits) - 1) << red_shift;
  data |= ((1 << green_bits) - 1) << green_shift;
  data |= ((1 << blue_bits) - 1) << blue_shift;

  // Read the framebuffer so that i can change the according bits
  uint64_t byte_offset = pitch * y + bpp * x / 8;
  uint64_t bit_offset = bpp % 8;
  uint64_t write_mask = ((1 << bpp) - 1) << bit_offset;
  uint64_t *fb_data =
      (uint64_t *)((uintptr_t)framebuffer->address + byte_offset);
  *fb_data |= (data << bit_offset) & write_mask;
}

void console_init(void) {
  if (framebuffer_request.response->framebuffer_count < 1) {
    HLT;
  }

  framebuffer = framebuffer_request.response->framebuffers[0];

  red_bits = framebuffer->red_mask_size;
  red_shift = framebuffer->red_mask_shift;
  green_bits = framebuffer->green_mask_size;
  green_shift = framebuffer->green_mask_shift;
  blue_bits = framebuffer->blue_mask_size;
  blue_shift = framebuffer->blue_mask_shift;

  width = framebuffer->width;
  height = framebuffer->height;
  pitch = framebuffer->pitch;
  bpp = framebuffer->bpp;

  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      console_put_pixel(i, j);
    }
  }
}
