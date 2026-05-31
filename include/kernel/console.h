#ifndef _KERNEL_CONSOLE_H_
#define _KERNEL_CONSOLE_H_

#define FONT_SCALE 2
#define TAB_LEN 2

void console_init(void);
void console_putchar(char c);
void console_clear(void);
void console_scroll(void);

// Doesn't have to do anything
void serial_init(void);
void serial_writechar(char c);
void serial_clear(void);

#endif

