#ifndef MEMORY_H
#define MEMORY_H

#include <libk/spinlock.h>
#include <mem/kheap.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct vmm_region_struct vmm_region_t;

typedef struct vmm_mmap_region_struct {
  vmm_region_t *region;
} vmm_mmap_region_t;

/// This is for user mode
typedef struct vmm_region_struct {
  /// No need to store the stack top that is stored in rsp
  void *stack_bottom;

  void *start_brk;
  void *brk;

  void *start_code;
  void *end_code;

  void *start_data;
  void *end_data;

  void *start_read_only;
  void *end_read_only;

  void *start_autogen;
  void *end_autogen;

  void *start_mmap;
  void *end_mmap;

  spinlock_t stack_lock;
  spinlock_t brk_lock;
  spinlock_t code_lock;
  spinlock_t data_lock;
  spinlock_t read_only_lock;
  spinlock_t autogen_lock;
  spinlock_t mmap_lock;
  spinlock_t global_lock;

  // Stack doesn't rlly need a lock but ill leve it.
  // The break just moves linearlly so that doesn't needmore
  // Code is allocated statically so that doesn' need more
  // Data is same as code
  // Readonly is same as code
  // Autogen is the same as break
  // Mmap is different and needs information on where to alloc pages, these are
  // stored in a bst with the key being the addr
  size_t padding;
} vmm_region_t;

/// This is for the kernel
/// brk is at the bottom of address space
/// MMap grows up to the top
/// Autogen grows down to meet the break
/// ignore that it goes brk, autogen, mmap, stack where stack and autogen grow
/// down
typedef struct vmm_kernel_region_struct {
  void *start_brk;
  void *brk;

  // Start is the higher address and it grows down
  void *start_autogen;
  void *end_autogen;

  void *start_mmap;
  void *end_mmap;

  void *stacks_bottom;
  size_t stack_index;

  spinlock_t autogen_lock;
  spinlock_t mmap_lock;
  spinlock_t brk_lock;
  spinlock_t stack_lock;

  size_t padding;
} vmm_kernel_region_t;

/// Allocate a physical page
void *phys_alloc(void);

/// Free a physical page
void phys_free(void *addr);

// Virtual Memory Management
// see vmm.txt

// Kmalloc (kernel malloc)
typedef enum {
  /// Tells the allocator to not go to the page mapping if this is big
  KMALLOC_BRK = 1,
} kmalloc_flags_t;

/// Use like malloc but for kernel
void *kmalloc(size_t size, uint8_t flags);

/// Use like free but for kernel
void kfree(void *addr);

typedef struct gheap_entry_struct {
  union {
    size_t size;
    struct {
      uint64_t free : 1;
      uint64_t reserved : 2;
      uint64_t useless : 61;
    };
  };
  struct gheap_entry_struct *next;
  struct gheap_entry_struct *prev;
} gheap_entry_t;

typedef struct {
  heap_entry_t *used_list;
  heap_entry_t *free_list;
  spinlock_t heap_lock;
} heap_info_t;

heap_info_t *get_heap_info(void);

void *gmalloc(size_t size);
void gfree(void *ptr);

/// Kernel side
///
/// To delete just free the region
void create_kernel_region(vmm_kernel_region_t *region);
void *increment_kernel_brk(vmm_kernel_region_t *region, uint64_t amount);
void *decrement_kernel_brk(vmm_kernel_region_t *region, uint64_t amount);
void *move_kernel_brk(vmm_kernel_region_t *region, int64_t amount);

void *increment_global_brk(size_t amount);
void *decrement_global_brk(size_t amount);
void init_global_brk(void);

/// returns the addr for rsp
void *create_new_kernel_stack(vmm_kernel_region_t *region, bool map,
                              size_t *stack_index);
/// Returns the first addr to delete
///
/// The second one is the return value + page size
void *delete_kernel_stack(size_t stack_index, vmm_kernel_region_t *region,
                          bool unmap);

vmm_kernel_region_t *get_kernel_region(void);

#endif
