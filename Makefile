ARCH?=x86_64

INCLUDE=include

CC:=$(ARCH)-elf-gcc
# General flags for compiling non-architecture-specific code 
# Assumes that the rule name is the rule for outputting the object file
CFLAGS=-Werror -Wall -Wpedantic -ffreestanding -nostdlib -nostartfiles -no-pie -mno-red-zone -fno-pie -mcmodel=kernel -I $(INCLUDE) -D _$(ARCH)_ -MP -MD -mno-sse -mno-avx -mno-mmx

LD:=$(ARCH)-elf-ld
# Flags for making the final binary, for make object files just use -r and a linker script if necessary
LDFLAGS=-T targets/$(ARCH)/linker.ld -z noexecstack -L . -no-pie 

kernel-mods=

.PHONY: build
build: CFLAGS += -s -pipe -O3 -D _BUILD_
build: build/bin/kernel.bin

.PHONY: debug
debug: CFLAGS += -g3 -Og -ggdb -D _DEBUG_
debug: build/bin/kernel.bin

include $(filter-out arch/%, $(wildcard **/Makefile))

build/bin/kernel.bin: tools/limine/limine $(kernel-mods)
	$(LD) $(LDFLAGS) $(kernel-mods) -o $@
	@echo "Kernel Build Complete!"

# Useful for making tools that are used
include targets/$(ARCH)/tools.make
# Contains the image target which is called after either build or debug which is used to make the final image (os.img)
include targets/$(ARCH)/image.make

-include $(shell find -name "*.d")

.PHONY: clean
clean:
	@rm -rf build
	@mkdir -p build/obj build/bin build/deps build/mods
	@echo "Cleaned Build"

build/obj/%.o: %.c
	@mkdir -p $(dir $@)
	@mkdir -p $(patsubst build/obj/%,build/deps/%,$(dir $@))
	@# I'll figure out a better way of including limine.h later
	$(CC) $(CFLAGS) -c -o $@ $< -MF $(patsubst build/obj/%.o,build/deps/%.d,$@) -I tools/limine

.PHONY: clean-tools
clean-tools:
	@rm -rf tools
	@mkdir -p tools
	@echo "Cleaned Tools"

