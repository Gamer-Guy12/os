#ifndef HAL_CLK_H
#define HAL_CLK_H

#include <libk/list.h>
#include <stddef.h>
#include <stdint.h>

/// Clocks are held in a list they must be returned or else they cant be used
/// again
typedef struct {
  void (*interrupt_in)(uint32_t ms, void (*callback)(void));
  list_node_t node;
} hal_clk_t;

hal_clk_t *hal_get_clock(void);
void hal_give_clock(hal_clk_t *clock);

#endif
