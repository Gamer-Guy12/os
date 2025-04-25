#ifndef X86_64_HAL_H
#define X86_64_HAL_H

#include <stdbool.h>
#include <stdint.h>

bool xapic_available(void);
void init_xapic(void);

#endif
