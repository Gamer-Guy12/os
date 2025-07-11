
# HEAP

Each entry in the heap looks like this:

before each data is a struct:
    - size (8 bytes)
        - since the minimum allocation size is 8 bytes the bottom 3 bits aren't used
        - bit 0: free
    - pointer (8 bytes in long mode)
        - when this is used it points to the next (previous in memory) entry
        - when this is free it points to the next free chunk

There is a pointer to the last struct for searching

```c
typedef struct heap_entry_struct {
    union {
        size_t size;
        struct {
            uint64_t free : 1;
            uint64_t reserved : 2;
            uint64_t useless : 63;
        };
    };
    struct heap_entry_struct* next;
} heap_entry_t;
```

