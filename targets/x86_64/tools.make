tools/limine/limine:
	@echo "Building bootloader"
	git clone https://codeberg.org/Limine/Limine.git tools/limine --depth=1 --branch=v10.2.0-binary --single-branch
	$(MAKE) -C tools/limine \
		CC="gcc" \
		CFLAGS="-O3 -s -pipe" \
		CPPFLAGS="" \
		LDFLAGS=""

