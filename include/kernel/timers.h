#ifndef _KERNEL_TIMERS_H_
#define _KERNEL_TIMERS_H_

#include "lib/rbtree.h"
#include "lib/rlist.h"
#include <stdbool.h>
#include <stdint.h>

#define TICK_LEN_MS 1

struct timer {
  struct {
    struct rlist_node node;
    struct rbtree timer_handlers;
  } internal;
  void (*cancel)(struct timer *timer);
  void (*wait_microseconds)(void (*callback)(void *), void *data,
                            struct timer *timer, uint64_t microseconds);
  void (*wait_ms)(void (*callback)(void *), void *data, struct timer *timer,
                  uint64_t ms);
  void (*wait_deadline)(void (*callback)(void *), void *data,
                        struct timer *timer, uint64_t deadline);
};

// Architecture specific tick handling
// Each architecture should handle which timer tick
void increment_tick(void);
uint64_t get_cur_tick(void);

void int_at_ticks(uint64_t ticks, void (*handler)(void *), void *data);
void init_timers(void);

void register_abs_timer(uint64_t (*read_time)(void),
                        uint64_t (*read_freq)(void),
                        uint64_t freq);

// Arch specific
uint64_t abs_time(void);
// What is the timestamp in x ms
uint64_t abs_ms_deadline(uint64_t ms);
uint64_t abs_microsecond_deadline(uint64_t microseconds);
uint64_t abs_freq(void);

void register_timer(struct timer *timer);
void int_in_ms(uint64_t ms, void (*handler)(void *), void *data);

#endif
