ARCH?=x86_64

.PHONY: all
all: makebuildfs build/os.iso

build/os.iso: build/bin/kernel.bin
	@mkdir -p build/iso/boot/grub
	@cp build/bin/kernel.bin build/iso/boot
	@cp targets/$(ARCH)/grub.cfg build/iso/boot/grub
	@grub-mkrescue -o build/os.iso build/iso
	@echo Built ISO

include kernel/Makefile

.PHONY: makebuildfs
makebuildfs:
	@mkdir -p build/{bin,iso}
	@echo Made build file system

.PHONY: clean
clean: kernel-clean
	@rm -rf build
	@echo Cleaned main build