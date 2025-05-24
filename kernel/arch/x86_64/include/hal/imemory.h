#ifndef X86_64_MEMORY_H
#define X86_64_MEMORY_H

#include <hal/memory.h>
#include <libk/lock.h>
#include <libk/mem.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define PAGE_TABLE_ENTRY_ADDR_MASK 0x0007fffffffff000
#define PAGE_SIZE 0x1000
/// A block is 4mb and is used in the buddy system
#define BLOCK_SIZE (MB * 2)
#define BUDDY_MAX_ORDER 10

#define BLOCK_DESCRIPTORS_ADDR 0xFFFF800000000000

#define PT_ADDR 0xFFFFFF0000000000
#define PDT_ADDR 0xFFFFFF7F80000000
#define PDPT_ADDR 0xFFFFFF7FBFC00000
#define PML4_ADDR 0xFFFFFF7FBFDFE000

#define MAX_BLOCK_COUNT (GB * 512ull / BLOCK_SIZE)

typedef enum {
  PML4_PRESENT = 1,
  PML4_READ_WRITE = 1 << 1,
  PML4_USER_PAGE = 1 << 2,
  PML4_PAGE_WRITE_THROUGH = 1 << 3,
  PML4_PAGE_CACHE_DISABLED = 1 << 4,
  PML4_ACCESSED = 1 << 5,
} PML4_flags_t;

typedef struct {
  union {
    uint64_t full_entry;
    struct {
      uint8_t flags;
      uint8_t ignored_1 : 4;
      /// The real bottom 12 bits are required to be zersos so it works
      /// Bits 12 - 15
      uint8_t addr_bottom : 4;
      /// Bits 16 - 47
      uint32_t addr_main;
      /// Top three bits of the address (top must be 0)
      /// Bits 48 - 50
      uint8_t addr_top;
      uint8_t ignored_2 : 7;
      // Being set means it can't be executed
      uint8_t not_executable : 1;
    } __attribute__((packed));
  };
} __attribute__((packed)) PML4_entry_t;

typedef enum {
  PDPT_PRESENT = 1,
  PDPT_READ_WRITE = 1 << 1,
  PDPT_USER_PAGE = 1 << 2,
  PDPT_PAGE_WRITE_THROUGH = 1 << 3,
  PDPT_PAGE_CACHE_DISABLED = 1 << 4,
  PDPT_ACCESSED = 1 << 5,
  PDPT_PAGE_SIZE = 1 << 7
} PDPT_flags_t;

typedef struct {
  union {
    uint64_t full_entry;
    struct {
      uint8_t flags;
      uint8_t ignored_1 : 4;
      /// The bottom 12 bits are not included cuz they are not in the tables
      /// Bits 12 - 15
      uint8_t addr_bottom : 4;
      /// Bits 16 - 47
      uint32_t addr_main;
      /// Bits 48 - 50
      uint8_t addr_top;
      uint8_t ignored_2 : 7;
      /// If set the page is not executable
      uint8_t not_executable : 1;
    } __attribute__((packed));
  };
} __attribute__((packed)) PDPT_entry_t;

typedef enum {
  PDT_PRESENT = 1,
  PDT_READ_WRITE = 1 << 1,
  PDT_USER_PAGE = 1 << 2,
  PDT_PAGE_WRITE_THROUGH = 1 << 3,
  PDT_PAGE_CACHE_DISABLED = 1 << 4,
  PDT_ACCESSED = 1 << 5,
  PDT_PAGE_SIZE = 1 << 7
} PDT_flags_t;

typedef struct {
  union {
    uint64_t full_entry;
    struct {
      uint8_t flags;
      uint8_t ignored_1 : 4;
      /// The bottom 12 bits are not included cuz they are not in the tables
      /// Bits 12 - 15
      uint8_t addr_bottom : 4;
      /// Bits 16 - 47
      uint32_t addr_main;
      /// Bits 48 - 50
      uint8_t addr_top;
      uint8_t ignored_2 : 7;
      /// If set the page is not executable
      uint8_t not_executable : 1;
    } __attribute__((packed));
  };
} __attribute__((packed)) PDT_entry_t;

typedef enum {
  PT_PRESENT = 1,
  PT_READ_WRITE = 1 << 1,
  PT_USER_PAGE = 1 << 2,
  PT_PAGE_WRITE_THROUGH = 1 << 3,
  PT_PAGE_CACHE_DISABLED = 1 << 4,
  PT_ACCESSED = 1 << 5,
  PT_DIRTY = 1 << 6,
  PT_PAGE_ATTRIBUTE_TABLE = 1 << 7,
  PT_GLOBAL = 1 << 8
} PT_flags_t;

typedef struct {
  union {
    uint64_t full_entry;
    struct {
      uint16_t flags : 9;
      uint16_t ignored_1 : 3;
      /// Bits 12 - 15
      uint16_t addr_bottom : 4;
      /// bits 16 - 47
      uint32_t addr_main;
      /// Bits 48 - 50
      uint8_t addr_top;
      uint8_t ignored_2 : 3;
      uint8_t memory_protection_key : 4;
      /// If this is set the page cannot be executed
      uint8_t not_executable : 1;
    } __attribute__((packed));
  };
} __attribute__((packed)) PT_entry_t;

typedef enum { BLOCK_DESCRIPTOR_RESERVED = 1 } block_descriptor_flags;

typedef struct block_descriptor_struct {
  struct {
    uint64_t free_pages : 16;
    uint64_t flags : 5;
    uint64_t addr : 43;
  };
  void *buddy_data;
} block_descriptor_t;

/// Mem lock is required to use kernel_gp for writes
/// Dont care about reads
lock_t *get_mem_lock(void);

void init_memory_manager(void);

inline size_t virt_to_phys(size_t addr) {
  PT_entry_t *pt = (PT_entry_t *)PT_ADDR;

  size_t pml4_index = (addr >> 39) & 0x1FF;
  size_t pdpt_index = (addr >> 30) & 0x1FF;
  size_t pdt_index = (addr >> 21) & 0x1FF;
  size_t pt_index = (addr >> 12) & 0x1FF;

  PT_entry_t entry = pt[pt_index + pdt_index * 512 + pdpt_index * 512 * 512 +
                        pml4_index * 512 * 512 * 512];

  return entry.full_entry & PAGE_TABLE_ENTRY_ADDR_MASK;
}

const block_descriptor_t *get_block_descriptor_ptr(void);
void set_block_descriptor_ptr(const block_descriptor_t *new_ptr);

void *map_virt_to_phys(void *virt, void *phys, bool not_executable,
                       uint16_t flags);

void *unmap_virt(void *virt);

#endif
