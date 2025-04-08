CC := x86_64-elf-gcc
CFLAGS := -Wall -Werror -Wpedantic -O3 -ffreestanding -nostdlib -nostartfiles

ASM := nasm
ASMFLAGS := -felf64

LD:=ld
ifeq ($(ARCH), x86_64)
LDFLAGS=-T targets/x86_64/linker.ld
endif

ARCH?=x86_64

kernel-x86_64-asm-srcs=$(wildcard kernel/arch/x86_64/boot/*.asm)
kernel-x86_64-asm-objs=$(patsubst kernel/arch/x86_64/boot/%, kernel/obj/%.o, $(kernel-x86_64-asm-srcs))

kernel-c-srcs=$(wildcard kernel/*.c)
kernel-c-objs=$(patsubst kernel/%, kernel/obj/%.o, $(kernel-c-srcs))

kernel-x86_64-objs = $(kernel-x86_64-asm-objs) $(kernel-c-objs)

.PHONY: kernel-all
kernel-all: build/bin/kernel.bin

ifeq ($(ARCH), x86_64)

kernel/obj/%.asm.o: kernel/arch/x86_64/boot/%.asm
	@mkdir -p kernel/obj
	@$(ASM) $(ASMFLAGS) -o $@ $<
	@echo ASM $@ to $<

kernel/obj/%.c.o: kernel/%.c
	@mkdir -p kernel/obj
	@$(CC) $(CFLAGS) -c -o $@ $<
	@echo CC $@ to $<

build/bin/kernel.bin: $(kernel-x86_64-objs)
	@$(LD) -o $@ $(LDFLAGS) $(kernel-x86_64-objs)
	@echo Linked kernel

else

$(error Target $(ARCH) is not supported)

endif

.PHONY: kernel-clean
kernel-clean:
	@rm -rf kernel/obj
	@echo Cleaned kernel output