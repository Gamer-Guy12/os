#ifndef _KERNEL_TIMERS_H_
#define _KERNEL_TIMERS_H_

#include <stdbool.h>
#include <stdint.h>

#define TICK_LEN_MS 1

// Architecture specific tick handling
// Each architecture should handle which timer tick
void increment_tick(void);
uint64_t get_cur_tick(void);

void int_at_ticks(uint64_t ticks, void (*handler)(void *), void *data);
void init_timers(void);

enum vtimer_mode { VTIMER_ONESHOT, VTIMER_PERIODIC };

struct vtimer {
  void (*handler)(struct vtimer *timer);
  // Can be set by the user
  void *extra_info;
  enum vtimer_mode mode;
};

struct timer {
  void (*int_micro_seconds)(void (*callback)(void), struct timer *timer,
                            uint64_t micro_seconds);
  void (*cancel)(struct timer *timer);
};

void vtimer_create(struct vtimer *timer);
void vtimer_configure(struct vtimer *timer, enum vtimer_mode mode,
                      uint64_t micro_seconds);
void vtimer_cancel(struct vtimer *timer);
void vtimer_destroy(struct vtimer *timer);

void register_timer(struct timer *timer);

#endif
