ARCH?=x86_64
DECLS?=

ifeq (,$(wildcard ./fart.txt))
$(error Must have fart.txt (according to bluehalooo))
endif

.PHONY: all
all: inter

include dev_drivers/$(ARCH)/Makefile

.PHONY: inter
inter: build/bin/kernel.bin build/os.iso

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
clean: clean-kernel clean_drivers
	@rm -rf build
	@echo Cleaned build
