#ifndef _KERNEL_CORES_H_
#define _KERNEL_CORES_H_

#include <stddef.h>

void setup_core_info(void);
void init_cores(void);
size_t get_core_count(void);
size_t get_core_id(void);

#endif
