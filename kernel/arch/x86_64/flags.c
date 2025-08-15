#include <cls.h>
#include <stddef.h>
#include <x86_64.h>

size_t get_feature_flags(void) {
  cls_t* cls = get_cls();

  if ((size_t) cls == 0) {
    return 0;
  } else {
    return cls->feature_flags;
  }
}

