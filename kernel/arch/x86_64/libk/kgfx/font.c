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
}

uint64_t vga_kgfx_get_glyph(char c) {
  return font[(uint8_t)c];
}


