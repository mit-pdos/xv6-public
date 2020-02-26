
# C flags
set(CMAKE_C_FLAGS_INIT "-m32 -fno-pic -fno-builtin -fno-strict-aliasing -Wall -Werror -fno-omit-frame-pointer -nostdinc")
set(CMAKE_C_FLAGS_DEBUG_INIT "-O0 -ggdb")
set(CMAKE_C_FLAGS_RELEASE_INIT "-O2 -ggdb")

# ASM flags
set(CMAKE_ASM_FLAGS_INIT "-m32 -gdwarf-2 -Wa,-divide")

# set linker flags
set(CMAKE_EXE_LINKER_FLAGS_INIT "${CMAKE_EXE_LINKER_FLAGS} -nostdlib -Wl,-m,elf_i386 -Wl,--build-id=none")