#include <libk/mem.h>
int memcmp (const void * ptr1, const void * ptr2, size_t num) {
  const unsigned char *p1 = (const unsigned char *)ptr1;
  const unsigned char *p2 = (const unsigned char *)ptr2;
  
    int i = 0;
    while (i < num) {
      if (int(p1[i]) > int(p2[i])) {
        return 1;
      }
      else if (int(p1[i]) < int(p2[i])) {
        return -1;
      } 
      else if (int(p1[i]) == int(p2[i])) {
        i++;
      }
  }
  return 0;
}
