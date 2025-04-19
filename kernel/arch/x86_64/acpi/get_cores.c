#include <acpi.h>
#include <stddef.h>

uint16_t get_cores(uint8_t *cores, MADT_t *madt) {
  size_t size_left = madt->header.length;
  size_left -= sizeof(madt->header) + 8;
  uint8_t curIndex = 0;
  size_t core_count = 0;

  while (size_left > 0) {
    size_t size = madt->entries[curIndex + 1];
    if (madt->entries[curIndex] == 0) {
      if (cores != NULL)
        cores[core_count] = madt->entries[curIndex + 3];
      core_count++;
    }

    size_left -= size;
    curIndex += size;
  }

  return core_count;
}
