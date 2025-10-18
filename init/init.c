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

    void *memcpy(void *dest, const void *src, size_t n) {
  uint8_t *dest_ptr = dest;
  const uint8_t *src_ptr = src;

  for (size_t i = 0; i < n; i++) {
    dest_ptr[i] = src_ptr[i];
  }

  return dest;
}

void *memset(void *ptr, int v, size_t n) {
  uint8_t *dest = ptr;

  for (size_t i = 0; i < n; i++) {
    dest[i] = v;
  }

  return ptr;
}

void *memmove(void *dest, const void *src, size_t n) {
  uint8_t *dest_ptr = dest;
  const uint8_t *src_ptr = src;

  if (src > dest) {
    for (size_t i = 0; i < n; i++) {
      dest_ptr[i] = src_ptr[i];
    }
  } else {
    for (size_t i = n; i > 0; i--) {
      dest_ptr[i - 1] = src_ptr[i - 1];
    }
  }

  return dest;
}

int memcmp(const void *s1, const void *s2, size_t n) {
  const uint8_t *p1 = s1;
  const uint8_t *p2 = s2;

  for (size_t i = 0; i < n; i++) {
    if (p1[i] != p2[i]) {
      return p1[i] < p2[i] ? -1 : 1;
    }
  }

  return 0;
}

#define HLT __asm__ volatile("hlt")

void kinit(void) {
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
