#ifndef GHEAP_H
#define GHEAP_H

#include <libk/spinlock.h>
#include <stddef.h>
#include <stdint.h>

typedef struct gheap_entry_struct {
    union {
        size_t size;
        struct {
            uint64_t free : 1;
            uint64_t reserved : 2;
            uint64_t useless : 61;
        };
    };
    struct gheap_entry_struct* next;
} gheap_entry_t;

spinlock_t* get_heap_lock(void);
gheap_entry_t* get_last_entry(void);
void add_entry(gheap_entry_t* ptr);

void* gmalloc(size_t size);
void gfree(void* ptr);

#endif 
