#include <hal/imemory.h>
#include <hal/memory.h>
#include <libk/lock.h>
#include <libk/math.h>
#include <stddef.h>

static inline void copy_last_page(phys_mem_region_t *copy_to,
                                  phys_mem_section_t *section) {
  size_t *space_used = (size_t *)PHYSICAL_MEMORY_MANAGER_INFO;

  *space_used -= sizeof(phys_mem_region_t);

  phys_mem_region_t *cur_region = section->first;
  while (cur_region != NULL) {
    // This one is the last one
    if (cur_region->next == NULL) {
      *copy_to = *cur_region;
      copy_to->prev->next = copy_to;
    }

    cur_region = cur_region->next;
  }
}

/// This function requires the memlock which means that u must give up the
/// memlock
/// You should probably do this last
void phys_manager_combine(phys_mem_section_t *section) {
  lock_acquire(get_mem_lock());

  phys_mem_region_t *cur_region = section->first;

  while (cur_region->next != NULL) {
    if (cur_region->prev == NULL) {
      cur_region = cur_region->next;
      continue;
    }

    size_t prevBase = (size_t)cur_region->prev->base;
    size_t prevLen = cur_region->prev->len;
    size_t curBase = (size_t)cur_region->base;

    if (prevBase + prevLen == curBase) {
      // Check for same type
      if (cur_region->type != cur_region->prev->type) {
        cur_region = cur_region->next;
        continue;
      }

      // Combine the two
      // Policy is to reuse the old one
      cur_region->prev->len += cur_region->len;
      cur_region->prev->pages = ROUND_DOWN(cur_region->prev->len / 4096, 4096);
      cur_region->prev->next = cur_region->next;

      // Decrease the amount of space used in the manager
      copy_last_page(cur_region, get_base_section(NULL));
    }

    cur_region = cur_region->next;
  }

  lock_release(get_mem_lock());
}
