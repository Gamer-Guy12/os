#include <hal/imemory.h>
#include <hal/memory.h>

static phys_mem_section_t base_section;

phys_mem_section_t *get_base_section(phys_mem_section_t *section) {
  if (section->region_count != 0)
    base_section = *section;

  return &base_section;
}
