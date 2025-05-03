#include <hal/imemory.h>
#include <hal/memory.h>
#include <libk/lock.h>
#include <stddef.h>

static phys_mem_region_t *find_free_region(phys_mem_section_t *section,
                                           size_t min_size) {
  phys_mem_region_t *cur_region = section->first;

  while (cur_region->next != NULL) {
    if (cur_region->type == phys_free && cur_region->len >= min_size) {
      break;
    }

    cur_region = cur_region->next;
  }

  if (cur_region->type != phys_free)
    return NULL;

  return cur_region;
}

// TODO: make this
static void add_another_page(phys_mem_section_t *section) {}

void *phys_alloc(phys_mem_section_t *section) {
  lock_acquire(get_mem_lock());

  phys_mem_region_t *free_region = find_free_region(section, 4096);

  if (free_region == NULL)
    return NULL;

  size_t *info = (size_t *)PHYSICAL_MEMORY_MANAGER_INFO;

  if (info[0] + sizeof(phys_mem_region_t) > info[1] * 4096)
    add_another_page(section);

  lock_release(get_mem_lock());

  phys_manager_combine(section);
}
