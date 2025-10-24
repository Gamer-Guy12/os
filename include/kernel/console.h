#ifndef _KERNEL_CONSOLE_H_
#define _KERNEL_CONSOLE_H_

#define FONT_SCALE 2
#define TAB_LEN 2

void console_init(void);
void console_putchar(char c);
void console_clear(void);
void console_scroll(void);

#endif

