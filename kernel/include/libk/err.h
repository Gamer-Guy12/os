#ifndef ERR_H
#define ERR_H

// FLAGS
#define TOO_MUCH_SPACE_ERR 1
#define MISSING_BLOCK_ERR (1 << 1)

#define MEMORY_ALLOCATOR_ERR (0)
#define MULTIBOOT_ERR (1ull << 48ull)
#define MEMORY_INIT_ERR (2ull << 48ull)
#define NO_INTERRUPT_HANDLER_ERR (3ull << 48ull)

#define GET_CODE(full_err) (((full_err) >> 48) & 0xFFFF)

#endif
