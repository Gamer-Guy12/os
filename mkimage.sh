# Clean up extras
rm -rf boot.img core.img fs.img os.img

# create a 256 Mib disk image
dd if=/dev/zero of=os.img bs=512 count=524288
parted os.img mklabel msdos
parted os.img mkpart primary ext2 2048s 100%

# Copy boot.img
cp /usr/lib/grub/i386-pc/boot.img .

# Make core.img
grub-mkimage -o core.img -O i386-pc -p . -c targets/x86_64/boot.cfg normal ext2 part_msdos biosdisk

# create the file system image
dd if=os.img of=fs.img bs=512 skip=2048 count=522240 conv=sync
mkfs.ext2 fs.img

# Put kernel and config into disk image
e2mkdir fs.img:/boot/grub
e2cp build/bin/kernel.bin fs.img:/boot
e2cp targets/x86_64/grub.cfg fs.img:/boot/grub

# Put images back together
dd if=boot.img of=os.img bs=446 count=1 conv=notrunc
dd if=core.img of=os.img bs=512 seek=1 count=62 conv=notrunc
dd if=fs.img of=os.img bs=512 seek=2048 count=522240 conv=notrunc
#rm boot.img core.img fs.img