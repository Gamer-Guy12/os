#ifndef X86_64_GLOBAL_BRK
#define X86_64_GLOBAL_BRK

#include <decls.h>

void set_global_brk(void *ptr);
void *WUNUSED get_global_brk(void);

#endif
