#include "util.h"
#include <stdint.h>

extern char _start_init_func[];
extern char _end_init_func[];

void do_calls(uint32_t type) {
  const uintptr_t start_addr = (uintptr_t)_start_init_func;
  const uintptr_t end_addr = (uintptr_t)_end_init_func;

  struct init_entry *entries = (struct init_entry *)start_addr;
  const size_t count = (end_addr - start_addr) / sizeof(struct init_entry);

  for (size_t i = 0; i < count; i++) {
    if (entries[i].type == type)
      entries[i].ptr();
  }
}
