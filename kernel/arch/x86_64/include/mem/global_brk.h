#ifndef X86_64_GLOBAL_BRK
#define X86_64_GLOBAL_BRK

void set_global_brk(void *ptr);
void *get_global_brk(void);

#endif
