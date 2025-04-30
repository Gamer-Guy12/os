# TODO: Make the build system things
ARCH?=x86_64

build_system: /root/opt/cross/bin/x86_64-elf-ld /root/opt/cross/bin/x86_64-elf-gdb

/root/opt/cross/bin/x86_64-elf-ld:
	@curl -sSL -o targets/$(ARCH)/binutils.tar.xz https://ftp.gnu.org/gnu/binutils/binutils-2.44.tar.xz
	@mkdir -p targets/$(ARCH)/binutils
	@tar xvJf targets/$(ARCH)/binutils.tar.xz -C targets/$(ARCH)/binutils --strip-components=1
	@mkdir -p targets/$(ARCH)/build-binutils
	@./targets/$(ARCH)/binutils/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror
	@make -C targets/$(ARCH)/build-binutils
	@make install -C targets/$(ARCH)/build-binutils
	@rm -r targets/$(ARCH)/binutils
	@rm -r targets/$(ARCH/)build-binutils
	@rm targets/$(ARCH)/binutils.tar.xz
	@echo Built binutils

/root/opt/cross/bin/x86_64-elf-gdb:
	@curl -sSl -o targets/$(ARCH)/gdb.tar.xz https://ftp.gnu.org/gnu/gdb/gdb-16.2.tar.xz
	@mkdir -p targets/$(ARCH)/gdb
	@tar xvJf targets/$(ARCH)/gdb.tar.xz -C targets/$(ARCH)/gdb --strip-components=1
	@mkdir -p targets/$(ARCH)/build-gdb
	@./targets/$(ARCH)/gdb/configure --target=$TARGET --prefix="$PREFIX" --disable-werror
	@make all-gdb -C targets/$(ARCH)/build-gdb
	@make install-gdb -C targets/$(ARCH)/build-gdb
	@rm -r targets/$(ARCH)/gdb
	@rm -r targets/$(ARCH)/build-gdb
	@rm targets/$(ARCH)/gdb.tar.xz
	@echo Built gdb

/root/opt/cross/bin/x86_64-elf-gcc:
	@curl -sSl -o targets/$(ARCH)/gcc.tar.xz https://ftp.gnu.org/gnu/gcc/gcc-14.2.0/gcc-14.2.0.tar.xz
	@mkdir -p targets/$(ARCH)/gcc
	@tar xvJf targets/$(ARCH)/gcc.tar.xz -C targets/$(ARCH)/gcc --strip-components=1
	@mkdir -p targets/$(ARCH)/build-gcc
	
