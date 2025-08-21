#ifndef HAL_CLK_H
#define HAL_CLK_H

#include <decls.h>
#include <libk/list.h>
#include <stddef.h>
#include <stdint.h>

/// Clocks are given away from a list
typedef struct {
  void (*interrupt_in)(uint32_t ms, void (*callback)(void));
  list_node_t node;
} hal_clk_t;

hal_clk_t *WUNUSED hal_get_clock(void);
/// Should only be used upon clock creation
void hal_give_clock(hal_clk_t *clock);

#endif
