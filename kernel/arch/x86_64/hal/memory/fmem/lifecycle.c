#include <hal/imemory.h>
#include <stddef.h>
#include <stdint.h>

static bool enabled = false;
static uint8_t *ptr;

void fmem_init(void) {
  enabled = true;
  ptr = end_kernel;
}

void fmem_destroy(void) { enabled = false; }

bool fmem_get_lifecycle(void) { return enabled; }

void *fmem_get_ptr(void *new_ptr) {
  ptr = new_ptr != NULL ? new_ptr : NULL;
  return ptr;
}
