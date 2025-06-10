#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

#define MAX_MMAP_ORDER 11

/// In the kernel this should be a bst instead of a linked list
/// Also the deletion needs to be fixed for the bst
/// This should probably be stored on the break part and not in the mmap part because it depends on the mmap part
/// One of the flags for kmalloc should be ALLOC_BRK and ALLOC_MMAP
/// This is pretty much the buddy allocator so im not gonna finish this
typedef struct mmap_block_struct {
   size_t base;
   size_t free_page_count;
   struct mmap_block_struct* next;
   struct mmap_block_struct* prev;
} mmap_block_t;

int main(int argc, char** argv) {
   printf("Hello World\n");
}

