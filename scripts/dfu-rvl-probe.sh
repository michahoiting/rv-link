/home/micha/.platformio/packages/tool-dfuutil/bin/dfu-util \
    -d 28e9:0189 -a 0 --dfuse-address 0x08000000:leave -D \
    ".pio/build/rvl-probe/rvlink_fw_rvl-probe.bin"
