import sys
from os.path import isdir, join

Import("env")

board = env.BoardConfig()

PROJ_DIR = "$PROJECT_DIR"

env.Replace(

    ASFLAGS = ["-x", "assembler-with-cpp"],

    CCFLAGS=[
        "-Og",
        "-g",
        "-Wall",
        "-march=%s" % board.get("build.march"),
        "-mabi=%s" % board.get("build.mabi"),
        "-fshort-wchar",
        "-ffunction-sections",
        "-fdata-sections",
        "-fstack-usage",
        "-msmall-data-limit=8",
    ],

    LINKFLAGS=[
        "-march=%s" % board.get("build.march"),
        "-mabi=%s" % board.get("build.mabi"),
        "-msmall-data-limit=8",
        "-nostartfiles",
        "-Wl,--gc-sections",
        "-Wl,-Map=${TARGET}.map",
    ],

    LIBS=["c_nano"]
)

# copy CCFLAGS to ASFLAGS (-x assembler-with-cpp mode)
env.Append(ASFLAGS=env.get("CCFLAGS", [])[:])

env.Append(

    CPPPATH = [
        join(PROJ_DIR), # all project includes are relative to the project directory

        # project external library dependencies
        join(PROJ_DIR, "lib"),

        # include paths needed to build the gd32vf103-sdk
        join(PROJ_DIR, "lib", "gd32vf103-sdk", "GD32VF103_standard_peripheral"),
        join(PROJ_DIR, "lib", "gd32vf103-sdk", "GD32VF103_standard_peripheral", "Include"),
        join(PROJ_DIR, "lib", "gd32vf103-sdk", "GD32VF103_usbfs_driver", "Include"),
        join(PROJ_DIR, "lib", "gd32vf103-sdk", "RISCV", "drivers"),
 
        # needed for gd32vf103_libopt.h, usb_conf.h, usbd_conf.h to build with the gd32vf103-sdk
        join(PROJ_DIR, "rv-link", "link", "arch", "gd32vf103c"),
    ]
)

env.Replace(
    LDSCRIPT_PATH = join(PROJ_DIR, "lib", "gd32vf103-sdk", "RISCV", "gcc", board.get("build.gd32vf103-sdk.ldscript"))
)
