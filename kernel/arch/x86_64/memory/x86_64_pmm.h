#ifndef X86_64_PMM_H
#define X86_64_PMM_H

#include <stdint.h>

typedef struct {
    uint64_t base_addr;
    uint64_t length;
    /// This is of type memory_type_t but is a uint32 for sizing purposes
    uint32_t type;
    uint32_t reserved;
} __attribute__((packed)) memory_map_entry_t;

typedef struct {
    memory_map_entry_t* ptr;
    uint64_t entryCount;
} memory_map_t;

typedef enum {
    Available = 1,
    ACPI = 3,
    Reserved = 4,
    Defective = 5
} memory_type_t;

memory_map_t get_memory_map(uint8_t* multiboot);
uint64_t get_memory_size(memory_map_t map);

#endif
