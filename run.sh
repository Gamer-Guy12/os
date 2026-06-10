make debug -j 4
make image
sudo qemu-system-x86_64 -drive file=build/os.img,format=raw \
  -m 4096M -d int,cpu_reset \
  -D ints.log \
  -drive if=pflash,format=raw,readonly=on,file=/usr/share/OVMF/OVMF_CODE_4M.fd \
  -drive if=pflash,format=raw,file=OVMF_VARS.fd \
  -no-reboot \
  -M accel=tcg,smm=off \
  -smp cores=4,threads=1,sockets=1 \
  -cpu host,+pdpe1gb,+x2apic \
  -machine hpet=on \
  -serial file:output.log \
  -enable-kvm
#  -s -S 

