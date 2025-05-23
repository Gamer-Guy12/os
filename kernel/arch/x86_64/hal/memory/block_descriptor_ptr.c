#include <hal/imemory.h>

const static block_descriptor_t *ptr = NULL;

const block_descriptor_t *get_block_descriptor_ptr(void) { return ptr; }

void set_block_descriptor_ptr(const block_descriptor_t *new_ptr) {
  ptr = new_ptr;
}
