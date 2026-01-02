#ifndef _LIB_VECTOR_H_
#define _LIB_VECTOR_H_

#include <stddef.h>
#include <stdint.h>

struct vector {
  uint64_t capacity;
  uint64_t usage;
  size_t object_size;
  void *data;
  void *(*alloc)(size_t size);
  void (*free)(void *ptr);
};

// Capacity of 1
void vector_init(struct vector *vector, size_t object_size);
void vector_insert(struct vector *vector, void *data, size_t index, size_t count);
void *vector_at(struct vector *vector, uint64_t index);
void vector_remove(struct vector *vector, void *data, uint64_t index, size_t count);
void vector_destroy(struct vector *vector);
void vector_resize(struct vector *vector, uint64_t new_capacity);

void vector_pushback(struct vector *vector, void *data);
void vector_popback(struct vector *vector, void *data);

void __vector_grow(struct vector *vector, uint64_t capacity);
void __vector_shrink(struct vector *vector, uint64_t capacity);

#endif

