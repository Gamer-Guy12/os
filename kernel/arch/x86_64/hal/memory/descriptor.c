#include <hal/imemory.h>

static block_descriptor_t *first_descriptor;

block_descriptor_t *get_first_block_descriptor(void) {
  return first_descriptor;
}

void set_first_block_descriptor(block_descriptor_t *descriptor) {
  first_descriptor = descriptor;
}
