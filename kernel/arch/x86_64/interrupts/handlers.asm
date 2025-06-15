extern common_interrupt_handler

%macro idt_no_error 1

global idt_handler_%1
idt_handler_%1:

   iret

%endmacro

