#include "kernel/mem.h"

struct page *get_page(page_ptr_t ptr) { return &pages[ptr]; }
