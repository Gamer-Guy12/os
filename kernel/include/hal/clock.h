#ifndef CLOCK_H
#define CLOCK_H

#include <stdint.h>

typedef struct {
  /// Length is passed in milliseconds
  void (*wait)(uint64_t);
} hal_clock_t;

hal_clock_t *get_hal_clock(void);

#endif
