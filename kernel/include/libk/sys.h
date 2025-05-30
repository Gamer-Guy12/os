#ifndef SYS_H
#define SYS_H

#include "decls.h"
#include <stdint.h>

void NORETURN sys_panic(uint64_t error_code);

#endif
