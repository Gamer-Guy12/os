#include <cls.h>
#include <hal/irq.h>
#include <hal/kbd.h>
#include <interrupts.h>
#include <libk/key.h>
#include <libk/kgfx.h>
#include <libk/kio.h>
#include <ps_2.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

key_event_reciever key_event_recievers[KEY_EVENT_RECIEVER_COUNT];

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

  key_event_t event;
  char c = 0;
  bool shift = false;

  if (get_key_state(QWERTY_KEY_L_SHIFT) == KEY_STATE_DOWN) {
    shift = true;
  } else if (get_key_state(QWERTY_KEY_R_SHIFT) == KEY_STATE_DOWN) {
    shift = true;
  }

  if (get_caps_key_on()) {
    // Use this to check if alphabet
    c = shift_keycode_to_char(code);

    if ((uint8_t)c > 64 && (uint8_t)c < 91) {
      shift = !shift;
    } else {
    }

    c = 0;
  }

  if (shift) {
    c = shift_keycode_to_char(code);
  } else {
    c = keycode_to_char(code);
  }

  event.ascii_code = c;
  event.key_code = code;
  event.key_press = !(flags & KEY_RELEASED);

  for (size_t i = 0; i < KEY_EVENT_RECIEVER_COUNT; i++) {
    if (key_event_recievers[i] == NULL) {
      continue;
    } else {
      key_event_recievers[i](event);
    }
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
