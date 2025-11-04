#include "kernel/mem.h"
#include "util.h"

enum arch_zones {
  ARCH_ZONE_DMA = 0,
  ARCH_ZONE_LOW = 1,
  ARCH_ZONE_HIGH = 2,
};

uint32_t get_zone(uint32_t zone) {
  switch (zone) {
  case ZONE_DMA:
    return ARCH_ZONE_DMA;
  case ZONE_LOW:
    return ARCH_ZONE_LOW;
  case ZONE_HIGH:
    return ARCH_ZONE_HIGH;
  default:
    return ARCH_ZONE_HIGH;
  }
}

uint32_t get_zone_fallback(uint32_t zone) {
  zone = get_zone(zone);

  switch (zone) {
  case ARCH_ZONE_DMA:
    return ZONE_NULL;
  case ARCH_ZONE_LOW:
    return ARCH_ZONE_DMA;
  case ARCH_ZONE_HIGH:
    return ARCH_ZONE_LOW;
  default:
    return ZONE_NULL;
  }
}

void get_zone_info(uint32_t zone, uintptr_t *start, uintptr_t *end,
                   uint32_t *max_order) {
  zone = get_zone(zone);

  switch (zone) {
  case ARCH_ZONE_DMA:
    *start = 0x0;
    *end = 16 * MB - 1;
    *max_order = 5;
    break;
  case ARCH_ZONE_LOW:
    *start = 16 * MB;
    *end = GB * 4 - 1;
    *max_order = 10;
    break;
  case ARCH_ZONE_HIGH:
    *start = GB * 4;
    *end = MAX_64;
    *max_order = 10;
    break;
  default:
    *start = 0x0;
    *end = 0x0;
    *max_order = 0;
  }
}
