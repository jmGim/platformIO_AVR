

Import("env")
def after_build(source, target, env):
    objdump = env.subst("${OBJCOPY}").replace("objcopy", "objdump")
    src_elf = env.subst("${BUILD_DIR}/${PROGNAME}.elf")
    env.Execute(f"{objdump} -S -C -d {src_elf} > ${PROGNAME}.lst")
env.AddPostAction("${BUILD_DIR}/${PROGNAME}.elf", after_build)
