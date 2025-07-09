#include <libk/spinlock.h>
#include <mem/gheap.h>

void gfree(void* ptr) {
  uint8_t* real_ptr = ptr;
  gheap_entry_t* entry = (gheap_entry_t*)(real_ptr - sizeof(gheap_entry_t));
  entry->free = 1;
}

