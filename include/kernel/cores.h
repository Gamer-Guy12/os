#ifndef _KERNEL_CORES_H_
#define _KERNEL_CORES_H_

#include <stdint.h>

void init_cores(void);
void core_entry(void);

uint64_t get_core_count(void);
uint32_t get_core_id(void);

#endif

