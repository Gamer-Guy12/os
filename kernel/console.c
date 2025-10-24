#include "kernel/console.h"
#include "lib/string.h"
#include "util.h"
#include <limine.h>
#include <stdbool.h>
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

uint64_t font[256] = {0};
uint64_t cursor_glyph = 0xF8F8F8F8F8F8F8F8;
uint64_t text_width = 0;
uint64_t text_height = 0;
uint64_t cursor_x = 0;
uint64_t cursor_y = 0;

// Color is always white
//
// I'll figure out other formats later
static void put_pixel(uint64_t x, uint64_t y) {
  uint64_t data = 0;

  // Add the red component
  data |= ((1 << red_bits) - 1) << red_shift;
  data |= ((1 << green_bits) - 1) << green_shift;
  data |= ((1 << blue_bits) - 1) << blue_shift;

  // Read the framebuffer so that i can change the according bits
  uint64_t byte_offset = pitch * y + bpp * x / 8;
  *(uint32_t *)((uintptr_t)framebuffer->address + byte_offset) = data;
}

static void clear_pixel(uint64_t x, uint64_t y) {
  uint64_t byte_offset = pitch * y + bpp * x / 8;
  *(uint32_t *)((uintptr_t)framebuffer->address + byte_offset) = 0;
}

static void put_scaled_pixel(uint64_t x, uint64_t y, size_t scale) {
  uint64_t scaled_x = x * scale;
  uint64_t scaled_y = y * scale;

  if (scaled_x > width || scaled_y > height) {
    return;
  }

  for (uint64_t i = 0; i < scale; i++) {
    for (uint64_t j = 0; j < scale; j++) {
      put_pixel(scaled_x + i, scaled_y + j);
    }
  }
}

static void clear_scaled_pixel(uint64_t x, uint64_t y, size_t scale) {
  uint64_t scaled_x = x * scale;
  uint64_t scaled_y = y * scale;

  if (scaled_x > width || scaled_y > height) {
    return;
  }

  for (uint64_t i = 0; i < scale; i++) {
    for (uint64_t j = 0; j < scale; j++) {
      clear_pixel(scaled_x + i, scaled_y + j);
    }
  }
}

void console_clear(void) { memset(framebuffer->address, 0, pitch * height); }

static void print_glyph(uint64_t x, uint64_t y, uint64_t glyph, size_t scale) {
  for (size_t i = 0; i < 8; i++) {
    for (size_t j = 0; j < 8; j++) {
      size_t glyph_index = i + 8 * j;
      size_t scaled_x = x * 8 + i;
      size_t scaled_y = y * 8 + j;

      if (glyph & (1ull << glyph_index)) {
        put_scaled_pixel(scaled_x, scaled_y, scale);
      } else {
        clear_scaled_pixel(scaled_x, scaled_y, scale);
      }
    }
  }
}

#define ROW_ADDR(row) (void *)((uintptr_t)framebuffer->address + pitch * (row))

// Does not update the y index
void console_scroll(void) {
  for (uint64_t i = 1; i < height; i++) {
    memcpy(ROW_ADDR(i - 8 * FONT_SCALE), ROW_ADDR(i), pitch);
  }

  // Set the last row to all zeros
  memset(ROW_ADDR(height - 8 * FONT_SCALE), 0, pitch * 8 * FONT_SCALE);
}

static void newline(void) {
  cursor_x = 0;
  cursor_y++;

  if (cursor_y == text_height) {
    console_scroll();
    cursor_y--;
  }
}

static void increment_cursor(bool clear_old) {
  // Clear the cursor
  if (clear_old)
    print_glyph(cursor_x, cursor_y, 0, FONT_SCALE);

  cursor_x++;

  if (cursor_x >= text_width) {
    newline();
  }

  print_glyph(cursor_x, cursor_y, cursor_glyph, FONT_SCALE);
}

static void decrement_cursor(void) {
  print_glyph(cursor_x, cursor_y, 0, FONT_SCALE);

  if (cursor_x == 0) {
    cursor_y--;
    cursor_x = text_width - 1;
  } else {
    cursor_x--;
  }

  print_glyph(cursor_x, cursor_y, cursor_glyph, FONT_SCALE);
}

static void clear_line(uint32_t row) {
  memset(ROW_ADDR(row * FONT_SCALE * 8), 0, pitch * FONT_SCALE * 8);
}

void console_putchar(char c) {
  switch (c) {
  case '\n':
    print_glyph(cursor_x, cursor_y, 0, FONT_SCALE);
    newline();
    print_glyph(cursor_x, cursor_y, cursor_glyph, FONT_SCALE);
    break;
  case '\b':
    decrement_cursor();
    break;
  case '\t':
    for (int i = 0; i < TAB_LEN; i++) {
      print_glyph(cursor_x, cursor_y, 0, FONT_SCALE);
      increment_cursor(false);
    }
    break;
  case '\r':
    clear_line(cursor_y);
    cursor_x = 0;
    print_glyph(cursor_x, cursor_y, cursor_glyph, FONT_SCALE);
    break;
  default:
    print_glyph(cursor_x, cursor_y, font[(unsigned int)c], FONT_SCALE);
    increment_cursor(false);
    break;
  }
}

INIT void console_init(void) {
  if (framebuffer_request.response->framebuffer_count < 1) {
    panic();
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

  // 8 pixels are used per glyph and then font scale
  text_width = width / FONT_SCALE / 8;
  text_height = height / FONT_SCALE / 8;

  font['A'] = 0x33333F33331E0C;
  font['B'] = 0xF33330F33330F;
  font['C'] = 0xC33030303330C;
  font['D'] = 0xF33333333330F;
  font['E'] = 0x3F03033F03033F;
  font['F'] = 0x303030F03033F;
  font['G'] = 0xC333B0303330C;
  font['H'] = 0x3333333F333333;
  font['I'] = 0x3F0C0C0C0C0C3F;
  font['J'] = 0x30C0C0C0C0C3f;
  font['K'] = 0x63331B0F1B3363;
  font['L'] = 0x3F030303030303;
  font['M'] = 0x6363636B7F7763;
  font['N'] = 0x6363737B6F6763;
  font['O'] = 0x1E33333333331E;
  font['P'] = 0x303030F33330F;
  font['Q'] = 0x381E3B3333331E;
  font['R'] = 0x3333331F33331F;
  font['S'] = 0x1E33381E07331E;
  font['T'] = 0xC0C0C0C0C0C3F;
  font['U'] = 0xC1E3333333333;
  font['V'] = 0x1C1C3636366363;
  font['W'] = 0x6377776B6B6363;
  font['X'] = 0x6363361C366363;
  font['Y'] = 0xC0C0C0C1E3333;
  font['Z'] = 0x3F03071E38303F;

  font['a'] = 0x1E19191E180F00;
  font['b'] = 0x1E33331F030303;
  font['c'] = 0x1C060303061C00;
  font['d'] = 0x1E33333E303030;
  font['e'] = 0x3C66037F663C00;
  font['f'] = 0x606063F060E3C;
  font['g'] = 0x1E33303E31313E60;
  font['h'] = 0x6363673B030303;
  font['i'] = 0xC0C0C0C000C00;
  font['j'] = 0x619181818001800;
  font['k'] = 0x321A0E1A322202;
  font['l'] = 0xC0C0C0C0C0C0C;
  font['m'] = 0x4141495D370000;
  font['n'] = 0x212121331F0000;
  font['o'] = 0xE1111110E0000;
  font['p'] = 0x1010709090700;
  font['q'] = 0x18080E09090E00;
  font['r'] = 0x20202261A0000;
  font['s'] = 0xC100C020C0000;
  font['t'] = 0x1808081C080000;
  font['u'] = 0xC121212120000;
  font['v'] = 0x8141422220000;
  font['w'] = 0xA151511110000;
  font['x'] = 0x22361C36220000;
  font['y'] = 0x2060C1814362200;
  font['z'] = 0x3E0C18303E0000;

  font['0'] = 0x7E464E5A72627E;
  font['1'] = 0x7C101010141810;
  font['2'] = 0x7E081020161C00;
  font['3'] = 0x3E20203E20203E;
  font['4'] = 0x2020203C242424;
  font['5'] = 0x1E20201E02023E;
  font['6'] = 0x3E22223E02023E;
  font['7'] = 0x2020202020203E;
  font['8'] = 0x7E42427E42427E;
  font['9'] = 0x4040407C44447C;

  font[':'] = 0x6060000060600;
  font[';'] = 0x2060000060600;
  font['<'] = 0x301C061C3000;
  font['='] = 0x3E003E0000;
  font['>'] = 0x61C301C0600;
  font['?'] = 0x8000818101800;
  font['@'] = 0x1E033B6B733E00;
  font['!'] = 0x8000808080800;
  font['"'] = 0x141400;
  font['#'] = 0x247E24247E2400;
  font['$'] = 0x1C2A281C0A2A1C;
  font['%'] = 0x6192946816294986;
  font['&'] = 0xC32122C0412120C;
  font['\''] = 0x80800;
  font['('] = 0x4020101010102040;
  font[')'] = 0x204080808080402;
  font['*'] = 0xA040A00;
  font['+'] = 0x18187E7E181800;
  font[','] = 0x2040400000000;
  font['-'] = 0x7E7E000000;
  font['.'] = 0x6060000000000;
  font['/'] = 0x8081010202000;

  font['{'] = 0x18080C06060C0818;
  font['|'] = 0x202020202020202;
  font['}'] = 0x1810306060301018;
  font['~'] = 0x327E4C000000;

  font['['] = 0x6020202020202060;
  font['\\'] = 0x1010080808040400;
  font[']'] = 0x604040404040406;
  font['^'] = 0xA0400;
  font['_'] = 0x7E000000000000;
  font['`'] = 0x40200;
}
