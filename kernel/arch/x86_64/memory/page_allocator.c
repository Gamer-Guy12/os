#include "./x86_64_pmm.h"
#include "libk/bit.h"
#include "libk/kio.h"
#include <libk/mem.h>
#include <pmm.h>
#include <stddef.h>
#include <stdint.h>

extern uint8_t *page_bitmap_start;
extern uint8_t *page_bitmap_end;

#define GET_ADDR(index) (index / 8)
#define GET_OFFSET(index) (index % 8)
#define GET_INDEX(addr) (addr - addr % 4096) / 4096

void setup_page_frame_allocation(uint8_t *multiboot) {
  memset(page_bitmap_start, 0xFF, page_bitmap_end - page_bitmap_start);
  memory_map_t map = get_memory_map(multiboot);

  for (size_t i = 0; i < map.entryCount; i++) {
    size_t localPageCount = map.ptr[i].length / 4096;
    if (localPageCount * 4096 < map.ptr[i].length) {
      localPageCount++;
    }

    for (size_t j = 0; j < localPageCount; j++) {
      uint64_t pageIndex = GET_INDEX(map.ptr[i].base_addr + j * 4096);
      pageIndex = pageIndex - 1 + 1;
      if (map.ptr[i].type == 1 &&
          map.ptr[i].base_addr + j * 4096 >= page_bitmap_end)
        page_bitmap_start[GET_ADDR(pageIndex)] ^= 1 << GET_OFFSET(pageIndex);
    }
  }
}

pageframe_t allocate_page_frame(void) {
  size_t i = 0;
  for (i = 0; i < page_bitmap_end - page_bitmap_start; i++) {
    if (page_bitmap_start[i] != 0xFF) {
      break;
    }
  }

  uint8_t bit = find_zero8(page_bitmap_start[i]);
  page_bitmap_start[i] |= 1 << bit;
  uint8_t *addr = (uint8_t *)((i * 8 + bit) * 4096);

  return addr;
}

void free_page_frame(pageframe_t *page) {
  size_t index = (size_t)page / 4096;
  uint8_t bit = index % 8;
  index = (index - bit) / 8;

  page_bitmap_start[index] &= ~(1 << bit);
}
