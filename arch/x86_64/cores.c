#include "kernel/cores.h"
#include "limine.h"
#include "util.h"
#include <stdint.h>

LIMINE_REQUEST static volatile struct limine_mp_request mp_request = {
    .id = LIMINE_MP_REQUEST, .flags = 0, .revision = 0};

static uint32_t bsp_id = 0;
static uint64_t cpu_count = 0;
static struct limine_mp_info **cpus;

INIT void init_cores(void) {}

uint64_t get_core_count(void) {
  return cpu_count;
}

