#include "lib/vector.h"
#include "kernel/gheap.h"
#include "lib/string.h"
#include <stddef.h>
#include <stdint.h>

static void *valloc(size_t size) { return gmalloc(size, ZONE_ANY); }

static void vfree(void *ptr) { gfree(ptr); }

void vector_init(struct vector *vector, size_t object_size) {
  vector->object_size = object_size;
  vector->capacity = 1;
  vector->usage = 0;
  vector->alloc = valloc;
  vector->free = vfree;
  vector->data = vector->alloc(object_size);
}

void vector_insert(struct vector *vector, void *data, uint64_t index,
                   size_t count) {
  if (vector->usage == vector->capacity)
    vector_resize(vector, vector->capacity * 2);

  size_t offset = vector->object_size * index;
  size_t copy_offset = vector->object_size * (index + count);
  size_t copy_count = vector->usage - (index + count);

  void *ptr = (void *)((uintptr_t)vector->data + offset);
  void *copy_ptr = (void *)((uintptr_t)vector->data + copy_offset);
  vector->usage++;
  memcpy(copy_ptr, ptr, vector->object_size * copy_count);
  memcpy(ptr, data, vector->object_size * count);
}

void *vector_at(struct vector *vector, uint64_t index) {
  size_t offset = vector->object_size * index;
  return (void *)((uintptr_t)vector->data + offset);
}

void vector_remove(struct vector *vector, void *data, uint64_t index,
                   size_t count) {
  size_t offset = vector->object_size * index;
  void *ptr = (void *)((uintptr_t)vector->data + offset);
  memcpy(data, ptr, vector->object_size * count);

  size_t copy_count = vector->usage - (index + count);
  if (copy_count == 0)
    return;

  void *copy_ptr = (void *)((uintptr_t)ptr + vector->object_size);

  vector->usage--;
  memcpy(ptr, copy_ptr, copy_count * vector->object_size);
}

void vector_destroy(struct vector *vector) { vector->free(vector->data); }

void vector_resize(struct vector *vector, uint64_t new_capacity) {
  if (new_capacity == vector->capacity) {
    return;
  } else if (new_capacity < vector->capacity) {
    __vector_shrink(vector, new_capacity);
  } else {
    __vector_grow(vector, new_capacity);
  }
}

void vector_pushback(struct vector *vector, void *data) {
  if (vector->capacity == vector->usage) {
    // Resize
    vector_resize(vector, vector->capacity * 2);
  }

  size_t offset = vector->object_size * vector->usage;
  void *ptr = (void *)((uintptr_t)vector->data + offset);
  vector->usage++;

  memcpy(ptr, data, vector->object_size);
}

void vector_popback(struct vector *vector, void *data) {
  if (vector->usage == 0) {
    memset(data, 0, vector->object_size);
    return;
  }

  vector_remove(vector, data, vector->usage - 1, 1);
}

void __vector_grow(struct vector *vector, uint64_t capacity) {
  void *new_data = vector->alloc(capacity * vector->object_size);
  memcpy(new_data, vector->data, vector->object_size * vector->usage);
  vector->free(vector->data);
  vector->data = new_data;
}

void __vector_shrink(struct vector *vector, uint64_t capacity) {
  void *new_data = vector->alloc(capacity * vector->object_size);
  memcpy(new_data, vector->data, vector->object_size * capacity);
  vector->free(vector->data);
  vector->data = new_data;
}
