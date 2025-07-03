#include "libk/key.h"
#include "libk/kgfx.h"
#include <hal/irq.h>
#include <interrupts.h>
#include <libk/kio.h>
#include <hal/kbd.h>
#include <ps_2.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

bool check_ps_2(void) {
  // IMPORTANT: TODO: Do an actual check for the ps_2 controller

  return true;
}

static bool ps_2_check_keycode_down(uint8_t keycode) {
  uint8_t state = get_key_state(keycode);

  if (state == KEY_STATE_UP) {
    return false;
  }

  return true;
}

bool ps_2_is_capslock_on(void) { return get_caps_key_on(); }

#define KEY_EVENT_RECIEVER_COUNT 512

key_event_reciever key_event_recievers[KEY_EVENT_RECIEVER_COUNT] = {NULL};

static bool ps_2_register_key_event_handler(key_event_reciever reciever) {
  for (size_t i = 0; i < KEY_EVENT_RECIEVER_COUNT; i++) {
    if (key_event_recievers[i] != NULL) {
      continue;
    }

    key_event_recievers[i] = reciever;
    return true;
  }

  return false;
}

static void ps_2_unregister_key_event_handler(key_event_reciever reciever) {
  for (size_t i = 0; i < KEY_EVENT_RECIEVER_COUNT; i++) {
    if (key_event_recievers[i] == reciever) {
      key_event_recievers[i] = NULL;
    }
  }
}

static void on_key_event(idt_registers_t *registers) {
  uint16_t info = handle_key_press();
  uint8_t flags = info >> 8;
  uint8_t code = info & 0xff;

  if (flags & KEY_RELEASED) {
    kgfx_putchar(keycode_to_char(code));
  }

  hal_irq_t irqs = get_hal_irq();
  irqs.unmask_irq(0x1);
  irqs.eoi();
}

static void setup_irq(void) {
  register_interrupt_handler(on_key_event, 0x30);
  hal_irq_t irqs = get_hal_irq();
  irqs.map_irq(0x30, 0x1);
  irqs.unmask_irq(0x1);
}

hal_kbd_t init_ps_2(void) {
  init_ps_2_driver();

  setup_irq();

  hal_kbd_t kbd;
  kbd.check_keycode_down = ps_2_check_keycode_down;
  kbd.register_key_event_handler = ps_2_register_key_event_handler;
  kbd.unregister_key_event_handler = ps_2_unregister_key_event_handler;
  kbd.is_capslock_on = ps_2_is_capslock_on;

  return kbd;
}
