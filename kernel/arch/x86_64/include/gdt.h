#ifndef X86_64_GDT
#define X86_64_GDT

#include <decls.h>
#include <stddef.h>
#include <stdint.h>

// We need the null descriptor which adds 1
#define DESCRIPTOR_COUNT 5

#define CREATE_SELECTOR(index, table, rpl)                                     \
  (uint16_t)((index << 3) | (table << 2) | rpl)

#define KERNEL_CODE_SELECTOR CREATE_SELECTOR(1, 0, 0)
#define KERNEL_DATA_SELECTOR CREATE_SELECTOR(2, 0, 0)
#define USER_CODE_SELECTOR CREATE_SELECTOR(3, 0, 3)
#define USER_DATA_SELECTOR CREATE_SELECTOR(4, 0, 3)

typedef enum {
  GDT_LONG_MODE = 1 << 1,
  GDT_SIZE = 1 << 2,
  GDT_GRANULARITY = 1 << 3
} gdt_flags_t;

typedef enum {
  GDT_ACCESS_ACCESSED = 1,
  GDT_ACCESS_READ_WRITE = 1 << 1,
  GDT_ACCESS_CONFORMING = 1 << 2,
  GDT_ACCESS_EXECUTABLE = 1 << 3,
  GDT_ACCESS_DESCRIPTOR = 1 << 4,
  GDT_ACCESS_PRESENT = 1 << 7
} gdt_access_byte;

typedef struct {
  uint64_t limit_1 : 16;
  uint64_t base_1 : 16;
  uint64_t base_2 : 8;
  uint64_t access_byte : 8;
  uint64_t limit_2 : 4;
  uint64_t flags : 4;
  uint64_t base_3 : 8;
  uint32_t base_4;
  uint32_t reserved;
} PACKED gdt_descriptor_t;

typedef struct {
  uint16_t size;
  uint64_t offset;
} PACKED gdt_pointer_t;

void create_gdt(void);

#endif
