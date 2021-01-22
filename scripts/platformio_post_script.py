Import("env", "projenv")

env.Replace(
    OBJDUMP="riscv-nuclei-elf-objdump"
)


# Custom disassemble from ELF target
env.AddPostAction(
    "$BUILD_DIR/${PROGNAME}.elf",
    env.VerboseAction(" ".join([
        "$OBJDUMP", "--disassemble-all", "--source ",
        "${TARGET}", "> ${TARGET}.S"
    ]), "Building ${TARGET}.S")
)


def dump_env_to_file(source, target, env):
    '''Dump PlatformIO global, project construction and build environment to file'''
    print(source[0])
    print(target[0])
    env_filename = env.File("$BUILD_DIR/${PROGNAME}.env.txt")
    env_file = open(str(env_filename), "w")
    env_file.write("env\n---\n" + str(env))
    env_file.write("\n\nprojenv\n-------\n" + str(projenv))
    env_file.write("\n\nenv.Dump()\n----------\n" + str(env.Dump()))
    env_file.write("\n\nprojenv.Dump()\n--------------\n" + str(projenv.Dump()))
    env_file.close()

#env.AddPostAction("$BUILD_DIR/${PROGNAME}.elf", dump_env_to_file)
