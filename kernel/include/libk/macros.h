#ifndef MACROS_H
#define MACROS_H

#define container_of(ptr, type, element)                                       \
  (type *)((size_t)(ptr) - offsetof(type, element))

#endif
