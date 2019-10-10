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
        #"-mcmodel=%s" % board.get("build.mcmodel"),
        "-fshort-wchar",
        "-ffunction-sections",
        "-fdata-sections",
        "-fstack-usage",
        "-msmall-data-limit=8",
    ],


    LINKFLAGS=[
        "-march=%s" % board.get("build.march"),
        "-mabi=%s" % board.get("build.mabi"),
        #"-mcmodel=%s" % board.get("build.mcmodel"),
        "-msmall-data-limit=8",
        "-nostartfiles",
        "-Wl,--gc-sections",
        #"-specs=nano.specs"
    ],

    LIBS=["c_nano"]
)


# copy CCFLAGS to ASFLAGS (-x assembler-with-cpp mode)
env.Append(ASFLAGS=env.get("CCFLAGS", [])[:])


env.Append(

    CPPPATH = [
        join(PROJ_DIR, "src", "app", "gdb-server"),
        join(PROJ_DIR, "src", "include"),
        join(PROJ_DIR, "src", "link"),
        join(PROJ_DIR, "src", "target", "family", "gd32vf103"),
        join(PROJ_DIR, "src", "target", "arch", "riscv"),
        join(PROJ_DIR, "src"),
        join(PROJ_DIR, "src", "link", "longan-nano"),
        join(PROJ_DIR, "src", "link", "gd32vf103c-start"),
        join(PROJ_DIR, "src", "link", "gd32vf103c-start", "GD32VF103_standard_peripheral"),
        join(PROJ_DIR, "src", "link", "gd32vf103c-start", "GD32VF103_standard_peripheral", "Include"),
        join(PROJ_DIR, "src", "link", "gd32vf103c-start", "GD32VF103_usbfs_driver", "Include"),
        join(PROJ_DIR, "src", "link", "gd32vf103c-start", "RISCV", "drivers"),
    ]
)

env.Replace(
    LDSCRIPT_PATH = join(PROJ_DIR, "src", "link", "gd32vf103c-start", "RISCV", "gcc", board.get("build.ldscript")) 
)


