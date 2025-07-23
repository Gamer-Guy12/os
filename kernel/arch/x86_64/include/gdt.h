#ifndef X86_64_GDT
#define X86_64_GDT

#include <decls.h>
#include <stddef.h>
#include <stdint.h>

// We need the null descriptor which adds 1 and 2 more for the tss descriptor
#define DESCRIPTOR_COUNT 7

#define CREATE_SELECTOR(index, table, rpl)                                     \
  (uint16_t)((index << 3) | (table << 2) | rpl)

#define KERNEL_CODE_SELECTOR CREATE_SELECTOR(1, 0, 0)
#define KERNEL_DATA_SELECTOR CREATE_SELECTOR(2, 0, 0)
#define USER_DATA_SELECTOR CREATE_SELECTOR(3, 0, 3)
#define USER_CODE_SELECTOR CREATE_SELECTOR(4, 0, 3)

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
} PACKED gdt_descriptor_t;

typedef struct {
  uint16_t size;
  uint64_t offset;
} PACKED gdt_pointer_t;

typedef enum {
  GDT_SYSTEM_LDT = 0x2,
  GDT_SYSTEM_TSS_AVAILABLE = 0x9,
  GDT_SYSTEM_TSS_BUSY = 0xB
} GDT_system_access_t;

typedef struct {
  uint16_t limit_0;
  uint16_t base_0;
  uint8_t base_1;
  uint8_t access_byte;
  uint8_t limit_1 : 4;
  uint8_t flags : 4;
  uint8_t base_2;
  uint32_t base_3;
  uint32_t reserved;
} PACKED gdt_system_segment_t;

typedef struct {
  uint32_t reserved_0;
  uint64_t rsp0;
  uint64_t rsp1;
  uint64_t rsp2;
  uint64_t reserved_1;
  uint64_t ist1;
  uint64_t ist2;
  uint64_t ist3;
  uint64_t ist4;
  uint64_t ist5;
  uint64_t ist6;
  uint64_t ist7;
  uint64_t reserved_2;
  uint16_t reserved_3;
  uint16_t iopb;
} PACKED tss_t;

void create_gdt(void);

#endif
