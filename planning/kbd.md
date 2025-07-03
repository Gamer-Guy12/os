
# Keyboard

Scancodes:
Organize into a grid, bits 0 - 4 contain the column, bits 5 - 8 contain row


| | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B | C | D |
|-|-|-|- | - | - | - |-|-| -|-| - | -|-|-|
| 0 | Esc | | F1 | F2 | F3 | F4 | F5 | F6 | F7 | F8 | F9 | F10 | F11 | F11 |
| 1 | Grave | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 0 | - | = | Bkspc |
| 2 | Tab | Q | W | E | R | T | Y | U | I | O | P | [ | ] | \ |
| 3 | Caps | A | S | D | F | G | H | J | K | L | ; | ' | Enter | |
| 4 | L Shift | Z | X | C | V | B | N | M | , | . | / | | R Shift | |
| 5 | L Ctrl | Win | L Alt| Space | | | | | R Alt | | FN | Menu | R Ctrl | |

Ex:
```c
#define KEY_CODE_Q (1 | (2 << 5))
#define KEY_CODE_W (2 | (2 << 5))
#define KEY_CODE_E (3 | (2 << 5))
#define KEY_CODE_A (1 | (3 << 5))
#define KEY_CODE_S (2 | (3 << 5))
#define KEY_CODE_D (3 | (3 << 5))
```

(Unsafe) Uses
```c
void find_key_below(uint8_t code) {
    return code + (1 << 5);
}

void find_key_above(uint8_t code) {
    return code - (1 << 5);
}

void find_key_right(uint8_t code) {
    return code + 1;
}

void find_key_left(uint8_t code) {
    return code - 1;
}
```

## Command Queue
An array of bytes to send to the queue each one is sent one by one and then it waits for 0xFA
if it gets 0xFE it resends a max of 3 times
anything else is a quit (echo be dammed)

There is an array of 256 commands and a read and a write pointer (prolly indexes).
The write pointer needs a lock and then will write at the write pointer and increment
The read pointer will go through and read the next byte to send and increment

## HAL API
- [ ] You should be able to check whether a specific keycode is down
- [ ] Register to recieve key event packets
    - `void (*on_key_pressed)(key_event_t packet);`
    - Can be held in a circular array
- [ ] Check the state of certain toggle keys
    - Caps lock 
    - Later Num and Scroll Lock when i extend the keyboard (for numpad, arrows, extra keys)

## NOTES

- Later I might have to make a seperate ps_2 module and do inits for that stuff seperatly to use both kbd and mouse

## TODO
- [x] Create a system to hold all the keystates
- [-] Create the interrupt handler that handles the irq
    - [ ] State machine to fully get all the bytes
    - [ ] Translation table to translate from scan code to key code
    - [ ] Call all the registered handlers
- [ ] Enable the irq (hal_irq_t)
- [ ] Create the hal functions based off of it
    - [ ] check_keycode_down
    - [ ] register and unregister key event handler
        - have an array and if it fills up too bad so sad (return false)
    - [ ] is capslock on

