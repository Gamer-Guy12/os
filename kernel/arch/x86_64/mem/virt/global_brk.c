#include <libk/kio.h>
#include <libk/math.h>
#include <libk/spinlock.h>
#include <mem/memory.h>
#include <mem/pimemory.h>
#include <mem/vimemory.h>
#include <stdatomic.h>
#include <stddef.h>

void *global_brk = (void *)(INDICES_TO_ADDR(0, 0, 0, 259ull));
spinlock_t global_brk_lock = ATOMIC_FLAG_INIT;

void set_global_brk(void *ptr) { global_brk = ptr; }

void *get_global_brk(void) { return global_brk; }

void init_global_brk(void) {
  map_page(global_brk, PT_PRESENT | PT_READ_WRITE, 1);
}

void *increment_global_brk(size_t amount) {
  spinlock_acquire(&global_brk_lock);

  size_t brk = (size_t)global_brk;
  size_t cur_page_index = ROUND_DOWN(brk, PAGE_SIZE) / PAGE_SIZE;
  size_t new_page_index = ROUND_DOWN(brk + amount, PAGE_SIZE) / PAGE_SIZE;
  size_t diff_pages = new_page_index - cur_page_index;

  for (size_t i = 0; i < diff_pages; i++) {
    map_page((void *)(cur_page_index * PAGE_SIZE + (i + 1) * PAGE_SIZE),
             PT_PRESENT | PT_READ_WRITE, 1);
  }

  brk += amount;
  global_brk = (void *)brk;

  spinlock_release(&global_brk_lock);

  return global_brk;
}

void *decrement_global_brk(size_t amount) {
  spinlock_acquire(&global_brk_lock);

  size_t brk = (size_t)global_brk;
  size_t cur_page_index = ROUND_DOWN(brk, PAGE_SIZE) / PAGE_SIZE;
  size_t new_page_index = ROUND_DOWN(brk - amount, PAGE_SIZE) / PAGE_SIZE;
  size_t diff_pages = cur_page_index - new_page_index;

  for (size_t i = 0; i < diff_pages; i++) {
    unmap_page((void *)(cur_page_index * PAGE_SIZE - i * PAGE_SIZE), true);
  }

  brk -= amount;
  global_brk = (void *)brk;

  spinlock_release(&global_brk_lock);

  return global_brk;
}
