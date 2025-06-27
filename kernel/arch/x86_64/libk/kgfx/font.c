#include <libk/vga_kgfx.h>
#include <stdint.h>

uint64_t font[256] = {0};

void vga_kgfx_init_font(void) {
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
}

uint64_t vga_kgfx_get_glyph(char c) {
  return font[(uint8_t)c];
}


