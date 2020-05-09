cmake_minimum_required(VERSION 3.8)
project(xv6 LANGUAGES NONE)

# setup
# ---------------------------------------------------------------------------

include(CheckCCompilerFlag)

# first, apply needed compiler flags from toolchain file (must do this _before_ the project() statement so CMake sees it when scanning compiler configuration)
include(cmake/X86Toolchain.cmake)

# control build type
set(CMAKE_BUILD_TYPE "DEBUG" CACHE STRING "Build type, Debug or Release")

# Set the possible values of build type for cmake-gui
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "DEBUG" "RELEASE")

# now, enable languages and verify we have the right compiler
enable_language(C ASM ASM-ATT)
include(cmake/CheckCompilerConfig.cmake)

# get rest of compiler flags
include(cmake/CompilerFlags.cmake)

# set up dir structure
set(USER_FS_DIR ${CMAKE_CURRENT_BINARY_DIR}/user_exes)
set(ASM_DUMP_DIR ${CMAKE_CURRENT_BINARY_DIR}/asm_dumps)
set(SYM_DUMP_DIR ${CMAKE_CURRENT_BINARY_DIR}/symbol_dumps)
set(HOST_EXE_DIR ${CMAKE_CURRENT_BINARY_DIR}/host)

file(MAKE_DIRECTORY ${USER_FS_DIR} ${ASM_DUMP_DIR} ${SYM_DUMP_DIR} ${HOST_EXE_DIR})

# utility functions
# ---------------------------------------------------------------------------

# dump the code in a target to a .asm file
macro(add_asm_dump TARGET)
	add_custom_command(
		TARGET ${TARGET} POST_BUILD
		COMMAND ${CMAKE_OBJDUMP} -S $<TARGET_FILE:${TARGET}> > ${ASM_DUMP_DIR}/${TARGET}.asm
		COMMENT "Generating ASM dump of ${TARGET}"
		VERBATIM)

	set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES ${ASM_DUMP_DIR}/${TARGET}.asm)

endmacro(add_asm_dump)

# dump the symbols in a target to a .sym file
macro(add_sym_dump TARGET)
	add_custom_command(
		TARGET ${TARGET} POST_BUILD
		COMMAND ${CMAKE_OBJDUMP} -t $<TARGET_FILE:${TARGET}> | sed "1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d" > ${SYM_DUMP_DIR}/${TARGET}.sym
		COMMENT "Generating symbol dump of ${TARGET}"
		VERBATIM)

	set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES ${SYM_DUMP_DIR}/${TARGET}.sym)

endmacro(add_sym_dump)

# build mini libc
# ---------------------------------------------------------------------------
set(ULIB_SOURCES ulib.c usys.S printf.c umalloc.c)

add_library(ulib OBJECT ${ULIB_SOURCES})

# build user executables
# ---------------------------------------------------------------------------
set(USER_EXECUTABLES 
	cat
	echo
	forktest
	grep
	init
	kill
	ln
	ls
	mkdir
	rm
	sh
	stressfs
	usertests
	wc
	zombie)

# list of all executable output names
set(USER_EXECUTABLE_OUTPUT_NAMES "")

foreach(USER_EXECUTABLE ${USER_EXECUTABLES})

	if("${USER_EXECUTABLE}" STREQUAL "forktest")
		# forktest has less library code linked in - needs to be small
		# in order to be able to max out the proc table.
		set(ULIB_DEPENDS ulib.c usys.S)
	else()
		set(ULIB_DEPENDS $<TARGET_OBJECTS:ulib>)
	endif()

	add_executable(${USER_EXECUTABLE} ${USER_EXECUTABLE}.c ${ULIB_DEPENDS})
	set_property(TARGET ${USER_EXECUTABLE} PROPERTY LINK_FLAGS "-Wl,-N -Wl,-e,main -Wl,-Ttext,0")

	# generate ASM and symbol dumps
	add_asm_dump(${USER_EXECUTABLE})
	add_sym_dump(${USER_EXECUTABLE})

	set_property(TARGET ${USER_EXECUTABLE} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${USER_FS_DIR})

	# set output name (add leading underscore)
	set_property(TARGET ${USER_EXECUTABLE} PROPERTY OUTPUT_NAME _${USER_EXECUTABLE})
	list(APPEND USER_EXECUTABLE_OUTPUT_NAMES _${USER_EXECUTABLE})

endforeach()

# build kernel
# ---------------------------------------------------------------------------

# build entryother 
add_executable(entryother_bin entryother.S)
set_property(TARGET entryother_bin PROPERTY LINK_FLAGS "-Wl,-N -Wl,-e,start -Wl,-Ttext,0x7000")

# convert the elf file into a binary code block which then gets bundled with the kernel
add_custom_command(
	TARGET entryother_bin POST_BUILD
	COMMAND ${CMAKE_OBJCOPY} -S -O binary -j .text $<TARGET_FILE:entryother_bin> ${CMAKE_CURRENT_BINARY_DIR}/entryother
	COMMENT "Generating entryother from entryother_bin object"
	VERBATIM)
add_asm_dump(entryother_bin)

# build initcode 
add_executable(initcode_bin initcode.S)
set_property(TARGET initcode_bin PROPERTY LINK_FLAGS "-Wl,-N -Wl,-e,start -Wl,-Ttext,0")

# convert the elf file into a binary code block which then gets bundled with the kernel
add_custom_command(
	TARGET initcode_bin POST_BUILD
	COMMAND ${CMAKE_OBJCOPY} -S -O binary $<TARGET_FILE:initcode_bin> ${CMAKE_CURRENT_BINARY_DIR}/initcode
	COMMENT "Generating initcode from initcode_bin object"
	VERBATIM)
add_asm_dump(initcode_bin)

# generate vectors.S
add_custom_command(
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/vectors.S
	COMMAND perl ${CMAKE_CURRENT_SOURCE_DIR}/vectors.pl > ${CMAKE_CURRENT_BINARY_DIR}/vectors.S
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/vectors.pl
	VERBATIM)

set(KERNEL_SOURCES 
	bio.c
	console.c
	exec.c
	entry.S
	file.c
	fs.c
	ioapic.c
	kalloc.c
	kbd.c
	lapic.c
	log.c
	main.c
	mp.c
	picirq.c
	pipe.c
	proc.c
	sleeplock.c
	spinlock.c
	string.c
	swtch.S
	syscall.c
	sysfile.c
	sysproc.c
	trapasm.S
	trap.c
	uart.c
	${CMAKE_CURRENT_BINARY_DIR}/vectors.S
	vm.c)

set(KERNEL_IDE_FS_SOURCES 
	ide.c)

set(KERNEL_MEM_FS_SOURCES 
	memide.c)

add_library(kernel_obj OBJECT ${KERNEL_SOURCES})

add_executable(kernel $<TARGET_OBJECTS:kernel_obj> ${KERNEL_IDE_FS_SOURCES})

# NOTE: the binary blobs HAVE to be relative paths because that's how ld determines their symbol name
set_property(TARGET kernel PROPERTY LINK_FLAGS "-Wl,-T,\"${CMAKE_CURRENT_SOURCE_DIR}/kernel.ld\" -Wl,-b,binary,entryother,initcode -Wl,-b,elf32-i386")

# make sure binary blocks are built before kernel
add_dependencies(kernel entryother_bin initcode_bin)

add_asm_dump(kernel)
add_sym_dump(kernel)

# build boot block
# ---------------------------------------------------------------------------

add_executable(bootblock bootasm.S bootmain.c)
set_property(TARGET bootblock PROPERTY LINK_FLAGS "-Wl,-N -Wl,-e,start -Wl,-Ttext,0x7C00")
add_asm_dump(bootblock)

# bootmain.c needs to be built with size optimizations or it won't fit in the boot sector
set_property(SOURCE bootmain.c PROPERTY COMPILE_FLAGS "-Os")

# convert the elf file into a binary code block which then gets written to the boot image
add_custom_command(
	TARGET bootblock POST_BUILD
	COMMAND ${CMAKE_OBJCOPY} -S -O binary -j .text $<TARGET_FILE:bootblock> ${CMAKE_CURRENT_BINARY_DIR}/bootblock.img
	COMMENT "Generating bootblock.img from bootblock object"
	VERBATIM)

add_custom_command(
	TARGET bootblock POST_BUILD
	COMMAND perl ${CMAKE_CURRENT_SOURCE_DIR}/sign.pl ${CMAKE_CURRENT_BINARY_DIR}/bootblock.img
	COMMENT "Signing bootblock.img"
	VERBATIM)

# build kernel image
# ---------------------------------------------------------------------------
add_custom_target(xv6_img
	ALL
	DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/xv6.img)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/xv6.img
	COMMAND dd if=/dev/zero of=${CMAKE_CURRENT_BINARY_DIR}/xv6.img count=10000
	COMMAND dd if=${CMAKE_CURRENT_BINARY_DIR}/bootblock.img of=${CMAKE_CURRENT_BINARY_DIR}/xv6.img conv=notrunc
	COMMAND dd if=$<TARGET_FILE:kernel> of=${CMAKE_CURRENT_BINARY_DIR}/xv6.img seek=1 conv=notrunc
	DEPENDS kernel bootblock
	VERBATIM
	COMMENT "Building kernel drive image")

# build user filesystem image
# ---------------------------------------------------------------------------

# Unfortunately, we now need to compile an executable for the host system, not our target.
# Luckily it's a simple one so we should be able to get away with using the default C compiler

set(MKFS_EXECUTABLE ${HOST_EXE_DIR}/mkfs)

add_custom_command(OUTPUT ${MKFS_EXECUTABLE}
	COMMAND cc -Wall -o ${MKFS_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/mkfs.c
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/mkfs.c
	VERBATIM
	COMMENT "Building mkfs host executable")

# now we can make the actual image!
add_custom_target(fs_img
	ALL
	DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/fs.img)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/fs.img
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/README README
	COMMAND ${MKFS_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/fs.img README ${USER_EXECUTABLE_OUTPUT_NAMES}
	WORKING_DIRECTORY ${USER_FS_DIR}
	DEPENDS ${MKFS_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/README ${USER_EXECUTABLES}
	VERBATIM
	COMMENT "Building user drive image")

# build memory fs kernel
# ---------------------------------------------------------------------------

# kernelmemfs is a copy of kernel that maintains the
# disk image in memory instead of writing to a disk.
# This is not so useful for testing persistent storage or
# exploring disk buffering implementations, but it is
# great for testing the kernel on real hardware without
# needing a scratch disk.

add_executable(kernelmemfs $<TARGET_OBJECTS:kernel_obj> ${KERNEL_MEM_FS_SOURCES})

# NOTE: the binary blobs HAVE to be relative paths because that's how ld determines their symbol name
set_property(TARGET kernelmemfs PROPERTY LINK_FLAGS "-Wl,-T,\"${CMAKE_CURRENT_SOURCE_DIR}/kernel.ld\" -Wl,-b,binary,entryother,initcode,fs.img -Wl,-b,elf32-i386")

# make sure binary blocks are built before kernel
add_dependencies(kernelmemfs entryother_bin initcode_bin fs_img)

add_asm_dump(kernelmemfs)
add_sym_dump(kernelmemfs)

# build memory fs kernel image
# ---------------------------------------------------------------------------

add_custom_target(xv6memfs_img
	ALL
	DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/xv6memfs.img)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/xv6memfs.img
	COMMAND dd if=/dev/zero of=${CMAKE_CURRENT_BINARY_DIR}/xv6memfs.img count=10000
	COMMAND dd if=${CMAKE_CURRENT_BINARY_DIR}/bootblock.img of=${CMAKE_CURRENT_BINARY_DIR}/xv6memfs.img conv=notrunc
	COMMAND dd if=$<TARGET_FILE:kernelmemfs> of=${CMAKE_CURRENT_BINARY_DIR}/xv6memfs.img seek=1 conv=notrunc
	DEPENDS kernelmemfs bootblock
	VERBATIM
	COMMENT "Building memory FS kernel drive image")

# Add QEMU targets
# ---------------------------------------------------------------------------
include(cmake/QEMUTargets.cmake)

# Print build report
# ---------------------------------------------------------------------------
message(STATUS "")
message(STATUS "xv6 configuration complete.")
message(STATUS "-----------------------------------------")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "")
message(STATUS "C Compiler: ${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION} (${CMAKE_C_COMPILER})")
message(STATUS "C Flags: ${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_${CMAKE_BUILD_TYPE}}")
message(STATUS "")
message(STATUS "ASM Compiler: ${CMAKE_ASM_COMPILER_ID} (${CMAKE_ASM-ATT_COMPILER})")
message(STATUS "ASM Flags: ${CMAKE_ASM_FLAGS} ${CMAKE_ASM_FLAGS_${CMAKE_BUILD_TYPE}}")
message(STATUS "")
message(STATUS "Linker Flags: ${CMAKE_EXE_LINKER_FLAGS}")
message(STATUS "")
if(QEMU_I386)
	message(STATUS "QEMU Executable: ${QEMU_I386}")
	message(STATUS "QEMU runs will use ${QEMU_PROCESSORS} processors.")
else()
	message(STATUS "QEMU was not found so qemu targets are unavailable.")
endif()
message(STATUS "-----------------------------------------")
