tools/limine/limine:
	@echo "Building bootloader"
	git clone https://github.com/Limine-Bootloader/Limine tools/limine --depth=1 --branch=v11.4.1-binary --single-branch
	$(MAKE) -C tools/limine \
		CC="gcc" \
		CFLAGS="-O3 -s -pipe" \
		CPPFLAGS="" \
		LDFLAGS=""

tools/limine-protocol:
	git clone https://github.com/Limine-Bootloader/limine-protocol tools/limine-protocol --depth=1 --single-branch

