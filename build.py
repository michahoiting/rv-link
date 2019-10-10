from os.path import isdir, join

# from SCons.Script import DefaultEnvironment

# env = DefaultEnvironment()

# board = env.BoardConfig()

# p = env.PioPlatform()

# BARE_PATH = join(p.get_dir(), "builder", "frameworks", "_bare.py")

# print(BARE_PATH)

#env.SConscript(BARE_PATH, exports="env")
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
        "-mcmodel=%s" % board.get("build.mcmodel"),
        "-fshort-wchar",
        "-ffunction-sections",
        "-fdata-sections",
        "-fstack-usage",
        "-msmall-data-limit=8",
    ],

    CFLAGS = [
        "-std=gnu11"
    ],

    CXXFLAGS = [
        "-std=gnu++17"
    ],

    CPPDEFINES = [
        "USE_STDPERIPH_DRIVER",
        ("HXTAL_VALUE", "%sU" % board.get("build.hxtal_value"))
    ],

    LINKFLAGS=[
        "-march=%s" % board.get("build.march"),
        "-mabi=%s" % board.get("build.mabi"),
        "-mcmodel=%s" % board.get("build.mcmodel"),
        "-nostartfiles",
        #"--gc-sections",
        #"-specs=nano.specs"
        # "-Wl,--wrap=_exit",
        # "-Wl,--wrap=close",
        # "-Wl,--wrap=fatat",
        # "-Wl,--wrap=isatty",
        # "-Wl,--wrap=lseek",
        # "-Wl,--wrap=read",
        # "-Wl,--wrap=sbrk",
        # "-Wl,--wrap=stub",
        # "-Wl,--wrap=write_hex",
        # "-Wl,--wrap=write"
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
    ],
    # LIBS = [
    #     "c_nano"
    # ]
)

env.Replace(
    LDSCRIPT_PATH = join(PROJ_DIR, "src", "link", "gd32vf103c-start", "RISCV", "gcc", board.get("build.ldscript")) 
)

# libs = [
#     env.BuildLibrary(
#         join("$BUILD_DIR", "app"),
#         join(PROJ_DIR, "src", "app")
#     ),
#     env.BuildLibrary(
#         join("$BUILD_DIR", "target", "gd32vf103"),
#         join(PROJ_DIR, "src", "target", "family", "gd32vf103")
#     ),
#     env.BuildLibrary(
#         join("$BUILD_DIR", "target", "arch"),
#         join(PROJ_DIR, "src", "target", "arch", "riscv")
#     ),
#     "c"
# ]



# env.Replace(LIBS=libs)



#print(env.Dump())

#env.Sconscript(join(PROJ_DIR, "src", "app", "gdb-server", "SConscript"))

