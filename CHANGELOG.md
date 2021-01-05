# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- TODO

### Changed
- TODO

### Fixed
- TODO


## [v0.2.1] - 2020-05-31

### Added
- Support for up to three virtual serial ports. 29613b52, 52a2edcd, ff3db43e, 37eef1c0, ac6844a1, 5787138d, cd12a526
  - By default, only one virtual serial port is enumerated, which is connected to the GDBServer of the RV-Link.
  - A second virtual serial port can be configured to be enabled with the GDB command `monitor rvl vcom on`. The vcom setting is stored in the build-in Flash memory. 2bf263e1, a63d1a77, 65fc43df, 07f264f9
  - When the second virtual serial port is enabled it streams all data to the physical serial port connected to the target machine. 229fa555, 5e5cd5c5
  - The 3rd virtual serial port (disabled by defaut) when enabled simply echos data. ebfff8f8

### Changed
- Adopt Mulan's loose license, version 1. 34b9bf98, dcf0c697
- Improved Longan Nano build variant regarding the JTAG reset signal. 9bdcc28c, 6e626315

### Fixed
- Fixed bugs found in v0.2.

### References
- [gitee.com/zoomdy/RV-LINK at tag 'v0.2.1'](https://gitee.com/zoomdy/RV-LINK/tree/07f264f981df8dea3ea512aaf6a7107ed15c8ae2)

## [v0.2] - 2019-10-26

### Added
- Supports for types of breakpoints:
  - Hardware breakpoint
  - Software breakpoint
  - Write watchpoint
  - Read watchpoint
  - Access watchpoint
- Support program upload to RAM, support program debugging in RAM.

### Changed
- Improved debugging and program upload speed.

### Fixed
- Fixed bugs found in v0.1.

### Additional Notes

Available RV-Link firmware images:
- *longan-nano+gd32vf103.hex* for **Longan Nano**, please refer to [Turning Longan Nano Development Board into RISC-V Emulator](https://gitee.com/zoomdy/RV-LINK/wikis/%E5%B0%86%20Longan%20Nano%20%E5%BC%80%E5%8F%91%E6%9D%BF%E5%8F%98%E6%88%90%20RISC-V%20%E4%BB%BF%E7%9C%9F%E5%99%A8?sort_id=1667644).
- *gd32vf103c-start+gd32vf103.hex* for **GD32VF103C-START**, please refer to [Turning the GD32VF103C-START development board into a RISCV-V emulator](https://gitee.com/zoomdy/RV-LINK/wikis/%E5%B0%86%20GD32VF103C-START%20%E5%BC%80%E5%8F%91%E6%9D%BF%E5%8F%98%E6%88%90%20RISCV-V%20%E4%BB%BF%E7%9C%9F%E5%99%A8?sort_id=1667646).

### References
- [Release notes](https://gitee.com/zoomdy/RV-LINK/wikis/%E5%8F%91%E8%A1%8C%E8%AF%B4%E6%98%8E?sort_id=1696558#rv-link-v02-2019%E5%B9%B410%E6%9C%8826%E6%97%A5).
- [Release v0.2](https://gitee.com/zoomdy/RV-LINK/releases/v0.2).


## [v0.1] - 2019-09-28

### Added
- Support for the Sipeed Longan Nano as link platform.
- Support register write operation, memory write operation.

### Changed
- Optimized JTAG operating speed, TCK is set to 1MHz.
- The JTAG port line to correspond to the SPI interface, in preparation for using SPI to drive JTAG in the future.

### Additional Notes

Supported link platforms:
- Sipeed Longan Nano
- GD32VF103C-START

Supported targets:
- GD32VF103

### References
- [Release notes](https://gitee.com/zoomdy/RV-LINK/wikis/%E5%8F%91%E8%A1%8C%E8%AF%B4%E6%98%8E?sort_id=1696558#rv-link-v01-2019%E5%B9%B409%E6%9C%8828%E6%97%A5).
- [Release v0.1](https://gitee.com/zoomdy/RV-LINK/releases/v0.1).


## [v0.0.1] - 2019-09-20

Initial release.

### Additional Notes

Realized basic debugging functions:
- Program download
- Hardware breakpoint
- Single step
- Halt, Resume
- Register read, memory read

Supported link platforms:
- GD32VF103C-START

Supported targets:
- GD32VF103

### References
- [Release notes](https://gitee.com/zoomdy/RV-LINK/wikis/%E5%8F%91%E8%A1%8C%E8%AF%B4%E6%98%8E?sort_id=1696558#rv-link-v001-2019%E5%B9%B409%E6%9C%8820%E6%97%A5).
- [Release v0.0.1](https://gitee.com/zoomdy/RV-LINK/releases/v0.0.1).

[Unreleased]: https://gitlab.melroy.org/micha/RV-LINK/compare/v0.2.1...master
[v0.2.1]: https://gitlab.melroy.org/micha/RV-LINK/compare/v0.2...v0.2.1
[v0.2]: https://gitlab.melroy.org/micha/RV-LINK/compare/v0.1...v0.2
[v0.1]: https://gitlab.melroy.org/micha/RV-LINK/compare/v0.0.1...v0.1
[v0.0.1]: https://gitlab.melroy.org/micha/RV-LINK/tree/v0.0.1
