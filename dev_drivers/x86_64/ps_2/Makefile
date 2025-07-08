
PS_2_CC=x86_64-elf-gcc
PS_2_INCLUDE := dev_drivers/x86_64/ps_2/include
PS_2_CFLAGS := -Wall -Werror -Wpedantic -O3 -ffreestanding -nostdlib -nostartfiles -c -I $(PS_2_INCLUDE) -I kernel/include -I kernel/arch/x86_64/include -std=c11 -mcmodel=kernel -no-pie -mmmx -msse -mavx

PS_2_SRCS := $(shell find dev_drivers/x86_64/ps_2 -name "*.c")
PS_2_OBJS := $(patsubst dev_drivers/x86_64/ps_2/%, dev_drivers/obj/x86_64/ps_2/%.o, $(PS_2_SRCS))

build/dev_drivers/ps_2.lib: $(PS_2_OBJS)
	@mkdir -p build/dev_drivers
	@ar rcs build/dev_drivers/ps_2.lib $(PS_2_OBJS)
	@echo Built PS/2 Driver

-include $(patsubst %.o, %.d, $(PS_2_OBJS))

dev_drivers/obj/x86_64/ps_2/%.c.o: dev_drivers/x86_64/ps_2/%.c
	@mkdir -p $(dir $@)
	@$(PS_2_CC) $(PS_2_CFLAGS) -MD -o $@ $<
	@echo CC $< $@

