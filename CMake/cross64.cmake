##################################################################################################
## Setup RASPBERRY Env build
##
##################################################################################################
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

set( AARCH 64 CACHE STRING "Raspberry pi 64 bits architecture")


# User specific configuration
message(STATUS "including path")
include(CMake/CMakeListsUserConfig64.txt OPTIONAL )

message(STATUS "Using custom build environnement")

set(CMAKE_C_COMPILER ${CUSTOM_C_COMPILER})
set(CMAKE_CXX_COMPILER ${CUSTOM_CXX_COMPILER})
set(CMAKE_ASM_COMPILER ${CUSTOM_ASM_COMPILER} )
set(CMAKE_OBJCOPY ${CUSTOM_OBJCOPY} )
set(CMAKE_OBJDUMP ${CUSTOM_OBJDUMP} )
set(CMAKE_AR ${CUSTOM_AR} )
set(CMAKE_LINKER ${CUSTOM_LD})
set(CMAKE_CXX_LINK_EXECUTABLE
"<CMAKE_LINKER> -o <LINK_FLAGS> --start-group <LINK_LIBRARIES> --end-group")


set(CMAKE_SYSROOT ${CUSTOM_SYSROOT})

message(STATUS "CMAKE_SYSROOT : ${CMAKE_SYSROOT}")
message(STATUS "CMAKE_C_COMPILER : ${CMAKE_C_COMPILER}")
message(STATUS "CMAKE_CXX_COMPILER : ${CMAKE_CXX_COMPILER}")
message(STATUS "CMAKE_ASM_COMPILER : ${CMAKE_ASM_COMPILER}")
message(STATUS "CMAKE_OBJCOPY : ${CMAKE_OBJCOPY}")
message(STATUS "CMAKE_OBJDUMP : ${CMAKE_OBJDUMP}")
message(STATUS "CMAKE_LINKER : ${CMAKE_LINKER}")
message(STATUS "CMAKE_CXX_LINK_EXECUTABLE : ${CMAKE_CXX_LINK_EXECUTABLE}")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set( CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY )

#
## Set a toolchain path. You only need to set this if the toolchain isn't in
## your system path.
#
## Specific Sugarpi : Configuration of compiler / build
## The toolchain prefix for all toolchain executables
#set( CROSS_COMPILE aarch64-none-elf- CACHE STRING "Tools prefix")
#set( CMAKE_C_COMPILER ${CROSS_COMPILE}gcc )
#set( CMAKE_CXX_COMPILER ${CROSS_COMPILE}g++)
#set( CMAKE_ASM_COMPILER ${CROSS_COMPILE}gcc )
#set( CMAKE_C_LINK_EXECUTABLE ${CROSS_COMPILE}ld )
#set( CMAKE_LINKER ${CROSS_COMPILE}ld )
#
#find_path(
#    LIBM_LIB_DIR
#    NAMES "libm.a"
#    PATHS "/mnt/d/PERSO/sync/Dropbox/Dev/02-Github/gcc-arm-9.2-2019.12-x86_64-aarch64-none-elf"
#)
#
#MESSAGE ( STATUS "LIBM PATH : ${LIBM_LIB_DIR}")
#
#set(CMAKE_SYSROOT "/mnt/d/PERSO/sync/Dropbox/Dev/02-Github/gcc-arm-9.2-2019.12-x86_64-aarch64-none-elf/lib/gcc/aarch64-none-elf/9.2.1")
##set( CMAKE_CXX_LINK_EXECUTABLE ${CROSS_COMPILE}ld )
##set(CMAKE_CXX_LINK_EXECUTABLE "<CMAKE_LINKER> <FLAGS> <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")
#set( CMAKE_AR ${CROSS_COMPILE}ar )
#
#set( CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY )
#
#set( CMAKE_OBJCOPY      ${TC_PATH}${CROSS_COMPILE}objcopy
#    CACHE FILEPATH "The toolchain objcopy command " FORCE )
#
#set( CMAKE_OBJDUMP      ${TC_PATH}${CROSS_COMPILE}objdump
#CACHE FILEPATH "The toolchain objdump command " FORCE )
#
#
#set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
#set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
#set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
#set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
#

# Set the common build flags, depending ARCH and RASPI
