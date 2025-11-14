#ifndef _LIB_UTIL_H_
#define _LIB_UTIL_H_

// Thanks to Stack Overflow user michaeljt for finding this macro
// Original blog post was forbidden for me
// Blog Post: http://lolengine.net/blog/2011/12/20/cpp-constant-string-hash
// Stack Overflow: https://stackoverflow.com/a/25646689
#define H1(s, i, x)                                                            \
  (x * 65599u + (uint8_t)s[(i) < sizeof(s) ? sizeof(s) - 1 - (i) : sizeof(s)])
#define H4(s, i, x) H1(s, i, H1(s, i + 1, H1(s, i + 2, H1(s, i + 3, x))))
#define H16(s, i, x) H4(s, i, H4(s, i + 4, H4(s, i + 8, H4(s, i + 12, x))))
#define H64(s, i, x)                                                           \
  H16(s, i, H16(s, i + 16, H16(s, i + 32, H16(s, i + 48, x))))
#define H256(s, i, x)                                                          \
  H64(s, i, H64(s, i + 64, H64(s, i + 128, H64(s, i + 192))))

#define HASH(s) ((uint32_t)(H256(s, 0, 0) ^ (H256(s, 0, 0) >> 16)))

#endif
