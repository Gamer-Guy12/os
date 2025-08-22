#ifndef HAL_CLK_H
#define HAL_CLK_H

#include <libk/math.h>
#include <decls.h>
#include <libk/list.h>
#include <stddef.h>
#include <stdint.h>

/// Clocks are given away from a list
typedef struct {
  /// When this or the function below is called it automatically stops the previous interrupt in request
  void (*interrupt_in)(uint32_t ms, void (*callback)(void));
  void (*interrupt_in_ticks)(uint128_t ticks, void (*callback)(void));
  uint128_t (*ms_to_ticks)(uint32_t ms);
  uint128_t (*get_current_progress)(void);
  list_node_t node;
} hal_clk_t;

hal_clk_t *WUNUSED hal_get_clock(void);
/// Should only be used upon clock creation
void hal_give_clock(hal_clk_t *clock);

#endif
