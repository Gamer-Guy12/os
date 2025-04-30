ARCH?=x86_64
DECLS?=

.PHONY: all
all: build/bin/kernel.bin build/os.iso

kernel-libs=

ifeq ($(ARCH), x86_64)
build/os.iso: install-kernel
	@mkdir -p build/iso/boot/grub
	@cp targets/$(ARCH)/grub.cfg build/iso/boot/grub
	@grub-mkrescue -o $@ build/iso
else
$(error The architecture $(ARCH) is not supported)
endif

include kernel/Makefile
include targets/$(ARCH)/Makefile

.PHONY: clean
clean: clean-kernel
	@rm -rf build
	@echo Cleaned build