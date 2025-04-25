#include <arch/pmm.h>
#include <libk/bit.h>
#include <stddef.h>

void allocate_page_frames(pageframe_t *pages, uint64_t count) {
  for (size_t i = 0; i < count; i++) {
    pages[i] = allocate_page_frame();
  }
}
