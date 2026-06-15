#include "stack.h"
#include <stdint.h>
#include <stddef.h>

void *get_func(struct callstack_context *ctxt) {
  if (ctxt->rbp == NULL) return NULL;
  uint64_t *new_rbp = (uint64_t *)*(ctxt->rbp++);
  if (new_rbp == NULL) {
    ctxt->rbp = new_rbp;
    return NULL;
  }
  uint64_t rip = *(ctxt->rbp);
  ctxt->rbp = new_rbp;
  return (void *)rip;
}
