#include <ps_2.h>

uint8_t key_states[256] = { 0 }; 

void set_key_state(uint8_t key_code, uint8_t state) {
  key_states[key_code] = state; 
}

uint8_t get_key_state(uint8_t key_code) {
  return key_states[key_code];
}

