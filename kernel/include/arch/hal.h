#ifndef HAL_H
#define HAL_H

#include <stdint.h>

typedef struct {
  void (*wait_ms)(uint32_t ms);
  /// Wait microsecond
  void (*wait_us)(uint32_t us);
} hal_clock;

void init_hal(void);

#endif
