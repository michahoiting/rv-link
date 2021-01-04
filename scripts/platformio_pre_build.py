import sys
from os.path import isdir, join

Import("env")

board = env.BoardConfig()

PROJ_DIR = "$PROJECT_DIR"

env.Replace(PROGNAME="rvlink_fw_%s" % env.get("PIOENV")) # defines.get("VERSION"))

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
