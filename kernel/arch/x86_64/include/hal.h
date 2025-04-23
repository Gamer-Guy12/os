#ifndef X86_64_HAL_H
#define X86_64_HAL_H

#include <stdbool.h>

bool xapic_available(void);
void init_xapic(void);

void init_hal_interupts(void);

#endif
