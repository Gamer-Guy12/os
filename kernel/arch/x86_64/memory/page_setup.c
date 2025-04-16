#include "./x86_64_pmm.h"
#include "libk/lock.h"
#include <libk/bit.h>
#include <libk/mem.h>
#include <pmm.h>
#include <stddef.h>
#include <stdint.h>

#define GET_ADDR(index) (index / 8)
#define GET_OFFSET(index) (index % 8)
#define GET_INDEX(addr) (addr - addr % 4096) / 4096

void setup_page_frame_allocation(uint8_t *multiboot) {
  lock_acquire(get_mem_lock());
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

  lock_release(get_mem_lock());
}
