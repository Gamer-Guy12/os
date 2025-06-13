#include <mem/pimemory.h>
#include <stddef.h>

size_t block_count;

size_t get_block_count(void) { return block_count; }

void set_block_count(size_t count) { block_count = count; }
