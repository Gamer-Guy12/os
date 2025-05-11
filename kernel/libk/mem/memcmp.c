#include <libk/mem.h>
int memcmp (const void * ptr1, const void * ptr2, size_t num) {
  if ptr1 == ptr2 {
    num = 0;
    return num;
    }
  else {
    int i = 0;
    while true {
      if (int(ptr1[i]) > int(ptr2[i])) {
        num = 1;
        return num;
      }
      else if (int(ptr1[i]) < int(ptr2[i])) {
        num = -1;
        return num;
      } 
      else if (int(ptr1[i]) == int(ptr2[i])) {
        i++;
      }
    }
  }
}
