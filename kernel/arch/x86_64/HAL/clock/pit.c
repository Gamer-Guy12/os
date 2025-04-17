#include "../include/clock.h"
#include <hal/clock.h>

bool pit_available(void) { return true; }

extern void init_pit(void);

void pit_init(void) { init_pit(); }
