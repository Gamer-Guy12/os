#ifndef X86_64_CLOCK
#define X86_64_CLOCK

#include <stdbool.h>

// PIT
void pit_init(void);
bool pit_available(void);

#endif
