ARCH?=x86_64

.PHONY: all
all: build/bin/kernel.bin build/os.iso

ifeq ($(ARCH), x86_64)
build/os.iso: build/bin/kernel.bin
	@mkdir -p build/iso/boot/grub
	@cp targets/$(ARCH)/grub.cfg build/iso/boot/grub
	@cp build/bin/kernel.bin build/iso/boot
	@grub-mkrescue -o $@ build/iso
else
$(error The architecture $(ARCH) is not supported)
endif

include kernel/Makefile

.PHONY: clean
clean: clean-kernel
	@rm -rf build
	@echo Cleaned build