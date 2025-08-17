#include <hpet.h>
#include <libk/list.h>
#include <hal/clk.h>
#include <stddef.h>

list_t clock_list;

void init_x86_64_clocks(void) {
  list_create(&clock_list);

  if (check_for_hpet()) {
    enable_hpet();
  }
}

hal_clk_t *hal_get_clock(void) {
  list_node_t *node = list_delete(&clock_list, NULL);
  hal_clk_t* clock = (hal_clk_t*)((size_t)node - offsetof(hal_clk_t, node));

  return clock;
}

void hal_give_clock(hal_clk_t *clock) {
  list_insert(&clock_list, NULL, &clock->node);
}

