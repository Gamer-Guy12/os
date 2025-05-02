#ifndef X86_64_MEMORY_H
#define X86_64_MEMORY_H

#include <hal/memory.h>
#include <libk/lock.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

extern void *kernel_gp;
extern void *kernel_gp_end;
extern void *start_kernel;
extern void *end_kernel;

#define PHYSICAL_MEMORY_MANAGER_INFO kernel_gp
#define PML4_LOCATION (uint8_t *)kernel_gp + 0x1000
#define PAGE_TABLE_ENTRY_ADDR_MASK 0x0007fffffffff000

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
      uint16_t flags : 12;
      /// Bits 12 - 15
      uint16_t addr_bottom : 4;
      /// bits 16 - 47
      uint32_t addr_main;
      /// Bits 48 - 50
      uint8_t addr_top;
      uint8_t ignored : 3;
      uint8_t memory_protection_key : 4;
      /// If this is set the page cannot be executed
      uint8_t not_executable : 1;
    } __attribute__((packed));
  };
} __attribute__((packed)) PT_entry_t;

/// Mem lock is required to use kernel_gp for writes
/// Dont care about reads
lock_t *get_mem_lock(void);

/// Allows you to get and set the base section that others are allowed to be
/// built on but this is the base
///
/// This base section should be the only one cuz any page can be used for every
/// program This is not being removed because its kinda too late and im too lazy
/// to go and refactor everything
phys_mem_section_t *get_base_section(phys_mem_section_t *section);

/// Combines all the adjacent regions
void phys_manager_combine(phys_mem_section_t *section);

void init_memory_manager(void);

#endif
