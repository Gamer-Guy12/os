ARCH?=x86_64
DECLS?=

ifeq ($(ARCH), x86_64)
CC=x86_64-elf-gcc
INCLUDE := kernel/include
CFLAGS := -Wall -Werror -Wpedantic -O3 -ffreestanding -nostdlib -nostartfiles -c -I $(INCLUDE) -I $(dev_includes) -std=c11 -mcmodel=kernel -no-pie -mmmx -msse -mavx -mno-red-zone

ASM := nasm
ASMFLAGS?=-felf64

LD:=ld
LDFLAGS=-T targets/x86_64/linker.ld -z noexecstack -L.
endif

kernel-c-srcs := $(shell find kernel -name "*.c")
kernel-c-objs := $(patsubst kernel/%, kernel/obj/%.o, $(kernel-c-srcs))

kernel-libs := $(dev_drivers)

kernel-objs = $(kernel-c-objs)

ifeq ($(ARCH), x86_64)
kernel-asm-srcs := $(shell find kernel -name "*.asm")
kernel-asm-objs := $(patsubst kernel/%, kernel/obj/%.o, $(kernel-asm-srcs))

kernel-objs += $(kernel-asm-objs)

build/bin/kernel.bin: $(kernel-objs) $(kernel-libs)
	@echo The Kernel does not yet support many things for floating point numbers
	@echo The types exist but not the declerations backing them
	@mkdir -p build/bin
	@$(LD) $(LDFLAGS) $(kernel-objs) $(addprefix -l:, $(kernel-libs)) -o $@
	@echo LD $(kernel-objs) $(kernel-libs)

kernel/obj/%.asm.o: kernel/%.asm
	@mkdir -p $(dir $@)
	@$(ASM) $(ASMFLAGS) -o $@ $<
	@echo ASM $< $@

-include $(patsubst kernel/obj/%.o, kernel/obj/%.d, $(kernel-objs))

kernel/obj/%.c.o: kernel/%.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -MD -I kernel/arch/x86_64/include $(addprefix -D , $(DECLS)) -D _$(ARCH)_ -o $@ $<
	@echo CC $< $@
	
endif

.PHONY: clean-kernel
clean-kernel:
	@rm -rf kernel/obj
	@echo Cleaned kernel

.PHONY: install-kernel
install-kernel: build/bin/kernel.bin
	@mkdir -p build/iso/boot
	@cp build/bin/kernel.bin build/iso/boot
	@echo "Installed Kernel"
