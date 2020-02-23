# first, find QEMU
# ---------------------------------------------------------------------------
find_program(QEMU_I386
    NAMES qemu-system-i386 qemu-system-x86_64
    DOC "Path to the i386 QEMU emulator executable")

if(NOT QEMU_I386)
    message(WARNING "QEMU was not found.  QEMU simulation targets will not be available.  Use -DQEMU_I386=/path/to/qemu-system-i386 to set a custom location.")
endif()

set(QEMU_PROCESSORS 2 CACHE STRING "Number of processors to use in the QEMU simulation")

if(QEMU_I386)
    # add standard QEMU targets
    # ---------------------------------------------------------------------------
    set(QEMU_COMMON_OPTS -drive file=${CMAKE_CURRENT_BINARY_DIR}/fs.img,index=1,media=disk,format=raw -drive file=${CMAKE_CURRENT_BINARY_DIR}/xv6.img,index=0,media=disk,format=raw -smp ${QEMU_PROCESSORS} -m 512)

    set(QEMU_MEMFS_COMMON_OPTS -drive file=${CMAKE_CURRENT_BINARY_DIR}/xv6memfs.img,index=0,media=disk,format=raw -smp ${QEMU_PROCESSORS} -m 256)

    add_custom_target(qemu
        COMMAND ${QEMU_I386} -serial mon:stdio ${QEMU_COMMON_OPTS}
        DEPENDS fs_img xv6_img
        COMMENT "Running QEMU in windowed mode...")

    add_custom_target(qemu-memfs
        COMMAND ${QEMU_I386} -serial mon:stdio ${QEMU_MEMFS_COMMON_OPTS}
        DEPENDS xv6memfs_img
        COMMENT "Running memFS QEMU in windowed mode...")

    add_custom_target(qemu-nox
        COMMAND ${QEMU_I386} -nographic ${QEMU_COMMON_OPTS}
        DEPENDS fs_img xv6_img
        COMMENT "Running QEMU in console mode...")

    # add GDB targets
    # ---------------------------------------------------------------------------
    set(GDB_PORT 5072 CACHE STRING "Port to run QEMU GDB server on")

    set(QEMU_GDB_ARGS
        -S # pause processor until GDB connects
        -gdb tcp::${GDB_PORT})

    add_custom_target(qemu-gdb
        COMMAND ${QEMU_I386} -serial mon:stdio ${QEMU_COMMON_OPTS} ${QEMU_GDB_ARGS}
        DEPENDS fs_img xv6_img
        COMMENT "Running QEMU in windowed mode with GDB server enabled...")

    add_custom_target(qemu-nox-gdb
        COMMAND ${QEMU_I386} -nographic ${QEMU_COMMON_OPTS} ${QEMU_GDB_ARGS}
        DEPENDS fs_img xv6_img
        COMMENT "Running QEMU in windowed mode with GDB server enabled...")

    set(GDBINIT_CONTENT
"set $lastcs = -1

define hook-stop
  # There doesn't seem to be a good way to detect if we're in 16- or
  # 32-bit mode, but in 32-bit mode we always run with CS == 8 in the
  # kernel and CS == 35 in user space
  if $cs == 8 || $cs == 35
    if $lastcs != 8 && $lastcs != 35
      set architecture i386
    end
    x/i $pc
  else
    if $lastcs == -1 || $lastcs == 8 || $lastcs == 35
      set architecture i8086
    end
    # Translate the segment:offset into a physical address
    printf \"[%4x:%4x] \", $cs, $eip
    x/i $cs*16+$eip
  end
  set $lastcs = $cs
end

echo + symbol-file $<TARGET_FILE:kernel>\\n
symbol-file $<TARGET_FILE:kernel>

echo + add-symbol-file $<TARGET_FILE:bootblock> 0x7C00\\n
add-symbol-file $<TARGET_FILE:bootblock> 0x7C00
")

    # only add "target remote" if this is not CLion
    if($ENV{CLION_IDE})
        message(STATUS "CLion detected, creating CLion .gdbinit")
    else()
        message(STATUS "CLion not detected, creating .gdbinit for command-line GDB")
        set(GDBINIT_CONTENT
"${GDBINIT_CONTENT}
echo + target remote localhost:${GDB_PORT}\\n
target remote localhost:${GDB_PORT}")
    endif()

    # generate gdbinit (has to be in source dir for CLion run GDB configuration to see it).
    # This makes gdb auto-connect to QEMU, and sets up debugging symbols.
    file(GENERATE OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/.gdbinit CONTENT "${GDBINIT_CONTENT}")

endif()


