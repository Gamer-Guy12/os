#include "lib/io.h"
#include "util.h"
#include <limine.h>
#include <stdbool.h>
#include <stddef.h>

#define LIMINE_SECTION(name) __attribute__((used, section(name)))

// clang-format off
LIMINE_SECTION(".limine_requests") static volatile LIMINE_BASE_REVISION(3)

LIMINE_SECTION(".limine_requests") 
static volatile struct limine_framebuffer_request
    limine_framebuffer = {.id = LIMINE_FRAMEBUFFER_REQUEST, .revision = 0};

LIMINE_SECTION(".limine_requests_start")
static volatile LIMINE_REQUESTS_START_MARKER

LIMINE_SECTION(".limine_requests_end") static volatile LIMINE_REQUESTS_END_MARKER
    // clang-format on

    // clang-format off
void kinit(void) {
  // clang-format on
  if (LIMINE_BASE_REVISION_SUPPORTED == false) {
    HLT;
  }

  if (limine_framebuffer.response == NULL ||
      limine_framebuffer.response->framebuffer_count < 1) {
    HLT;
  }

  struct limine_framebuffer *framebuffer =
      limine_framebuffer.response->framebuffers[0];

  for (size_t i = 0; i < 100; i++) {
    volatile uint32_t *fb_ptr = framebuffer->address;
    fb_ptr[i * (framebuffer->pitch / 4) + i] = 0xffffff;
  }

  HLT;
}
