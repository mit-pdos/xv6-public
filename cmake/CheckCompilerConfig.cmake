# script to make sure we are using the correct C compiler to compile xv6

# first make sure it's GCC
if(NOT "${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
	message(FATAL_ERROR "GCC is required to compile xv6.")
endif()

# now make sure the toolchain supports the elf32-i386 object format
execute_process(COMMAND ${CMAKE_OBJDUMP} -i
	RESULT_VARIABLE OBJDUMP_RESULT
	OUTPUT_VARIABLE OBJDUMP_OUTPUT)

if(NOT OBJDUMP_RESULT EQUAL 0)
	message(FATAL_ERROR "Failed to dump info from the objdump located at ${CMAKE_OBJDUMP}")
endif()

if(NOT "${OBJDUMP_OUTPUT}" MATCHES "elf32-i386")
	message(FATAL_ERROR 
"Couldn't find an i386-*-elf version of GCC/binutils. 
Please clear your build directory, then rerun cmake
with CMAKE_C_COMPILER defined to point to
the gcc executable to use.")
endif()

# all done!
message(STATUS "Compatible elf32-i386 GCC compiler detected.")