make debug -j 4
make image
qemu-system-x86_64 -drive file=build/os.img,format=raw \
  -m 4096M -d int,cpu_reset \
  -drive if=pflash,format=raw,readonly=on,file=/usr/share/OVMF/OVMF_CODE_4M.fd \
  -drive if=pflash,format=raw,file=OVMF_VARS.fd \
  -no-reboot \
  -M accel=tcg,smm=off \
  -smp cores=4,threads=1,sockets=1 \
  -cpu qemu64,+pdpe1gb
