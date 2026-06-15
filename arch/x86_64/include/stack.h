#ifndef _x86_64_STACK_H_
#define _x86_64_STACK_H_

// Used to unwind the call stack
#include <stdint.h>

struct callstack_context {
  uint64_t *rbp;
};

// Gets the next function in the call stack
void *get_func(struct callstack_context *ctxt);

#endif

