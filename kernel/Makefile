ARCH?=x86_64

CC := x86_64-elf-gcc
CFLAGS := -Wall -Werror -Wpedantic -O3 -ffreestanding -nostdlib -nostartfiles -c

ASM := nasm
ASMFLAGS := -felf64

LD:=ld
ifeq ($(ARCH), x86_64)
LDFLAGS=-T targets/x86_64/linker.ld
endif

kernel-c-srcs := $(shell find kernel -name "*.c")
kernel-c-objs := $(patsubst kernel/%, kernel/obj/%.o, $(kernel-c-srcs))

kernel-objs = $(kernel-c-objs)

ifeq ($(ARCH), x86_64)
kernel-asm-srcs := $(shell find kernel -name "*.asm")
kernel-asm-objs := $(patsubst kernel/%, kernel/obj/%.o, $(kernel-asm-srcs))

kernel-objs += $(kernel-asm-objs)

build/bin/kernel.bin: $(kernel-objs)
	@mkdir -p build/bin
	@$(LD) $(LDFLAGS) $(kernel-objs) -o $@
	@echo LD $(kernel-objs)

kernel/obj/%.asm.o: kernel/%.asm
	@mkdir -p $(dir $@)
	@$(ASM) $(ASMFLAGS) -o $@ $<
	@echo ASM $< $@

kernel/obj/%.c.o: kernel/%.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -o $@ $<
	@echo CC $< $@
	
endif

.PHONY: clean-kernel
clean-kernel:
	@rm -rf kernel/obj
	@echo Cleaned kernel