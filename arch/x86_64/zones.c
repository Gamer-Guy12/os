#include "kernel/mem.h"
#include "util.h"
#include <stdint.h>

// DMA and normal
struct zone zones[2] = {
    {.base = 0x0, .length = MB * 16, .type = ZONE_DMA},
    {.base = (void *)(MB * 16), .length = 0, .type = ZONE_NORMAL}};

struct zone *__get_zone(int zone) {
  switch (zone) {
  case ZONE_DMA:
    return &zones[0];
  case ZONE_NORMAL:
    return &zones[1];
  default:
    panic();
  }
}

