#ifndef _KERNEL_FMEM_H_
#define _KERNEL_FMEM_H_

// Allocs a virtual fmem page
void *_fmem_alloc(void);
void _fmem_free(void *addr);
void *_fmem_phys(void);

void _fmem_add_range(void *start, void *end);
void _fmem_free_to_mem(void);

#endif

