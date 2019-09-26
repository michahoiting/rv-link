# RV-LINK

用 RISC-V 开发板实现的 RISC-V 仿真器。

RV-LINK 的目标是做 RISC-V 仿真器，但是 RV-LINK 项目并不包含硬件，RV-LINK 为现有的 RISC-V 开发板编写固件，烧录了 RV-LINK 固件的开发板可以做为仿真器使用。

与其它仿真器不同的是：RV-LINK 通过 USB 串口直接与 GDB 交互，不需要 OpenOCD 之类的中介。

可以作为仿真器的开发板有：
- Longan Nano，淘宝有售
- GD32VF103C-START，淘宝有售

RV-LINK 可以调试的 RISC-V MCU：
- GD32VF103 系列

## RV-LINK 的使用

- [将 Longan Nano 开发板变成 RISC-V 仿真器](/zoomdy/RV-LINK/wikis/将%20Longan%20Nano%20开发板变成%20RISC-V%20仿真器)
- [将 GD32VF103C-START 开发板变成 RISCV-V 仿真器](/zoomdy/RV-LINK/wikis/将%20GD32VF103C-START%20开发板变成%20RISCV-V%20仿真器)
- [GDB 使用 RV-LINK 仿真器调试 RISC-V 程序](/zoomdy/RV-LINK/wikis/GDB%20使用%20RV-LINK%20仿真器调试%20RISC-V%20程序)

## RV-LINK 开发过程中的杂碎

- 开发 RV-LINK 中碰到的问题，记录在[这里](https://blog.csdn.net/zoomdy/article/category/9258422)

