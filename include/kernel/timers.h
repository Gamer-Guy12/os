#ifndef _KERNEL_TIMERS_H_
#define _KERNEL_TIMERS_H_

#include "lib/rlist.h"
#include <stdbool.h>
#include <stdint.h>

#define TICK_LEN_MS 1

// Architecture specific tick handling
// Each architecture should handle which timer tick
void increment_tick(void);
uint64_t get_cur_tick(void);

void int_at_ticks(uint64_t ticks, void (*handler)(void *), void *data);
void init_timers(void);

#endif
