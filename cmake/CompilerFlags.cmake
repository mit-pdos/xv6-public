set(CMAKE_C_VERSION 99)
set(CMAKE_C_EXTENSIONS TRUE)

# check stack protector flag
check_c_compiler_flag(-fno-stack-protector SUPPORTS_FNO_STACK_PROTECTOR)
if(SUPPORTS_FNO_STACK_PROTECTOR)
	# this shadows the cache variable with a local variable
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-stack-protector")
endif()

# try to disable position independent executables
check_c_compiler_flag(-no-pie SUPPORTS_NO_PIE)
if(SUPPORTS_NO_PIE)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -no-pie")
endif()