#include <mem/gheap.h>

gheap_entry_t *last_entry = NULL;

gheap_entry_t *get_last_entry(void) { return last_entry; }

void add_entry(gheap_entry_t *ptr) { last_entry = ptr; }
