CC := x86_64-elf-gcc
CFLAGS := -Wall -Werror -Wpedantic -O3

ASM := nasm
ASMFLAGS := -felf64

LD:=ld
ifeq ($(ARCH), x86_64)
LDFLAGS=-T targets/x86_64/linker.ld
endif

ARCH?=x86_64

kernel-x86_64-asm-srcs=$(wildcard kernel/arch/x86_64/*.asm)
kernel-x86_64-asm-objs=$(patsubst kernel/arch/x86_64/%.asm, kernel/obj/%.o, $(kernel-x86_64-asm-srcs))

.PHONY: kernel-all
kernel-all: build/bin/kernel.bin

ifeq ($(ARCH), x86_64)

kernel/obj/%.o: kernel/arch/x86_64/%.asm
	@mkdir -p kernel/obj
	@$(ASM) $(ASMFLAGS) -o $@ $<
	@echo ASM $@ to $<

build/bin/kernel.bin: $(kernel-x86_64-asm-objs)
	@$(LD) -o $@ $(LDFLAGS) $(kernel-x86_64-asm-objs)
	@echo Linked kernel

else

$(error Target $(ARCH) is not supported)

endif

.PHONY: kernel-clean
kernel-clean:
	@rm -rf kernel/obj
	@echo Cleaned kernel output