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
Is the directory with i386-jos-elf-gcc in your PATH?
If your i386-*-elf toolchain is installed with a command
prefix other than 'i386-jos-elf-', define CMAKE_C_COMPILER to point to
the gcc to use.  Then delete your build directory and run cmake again.")
endif()

# all done!
message(STATUS "Compatible elf32-i386 GCC compiler detected.")