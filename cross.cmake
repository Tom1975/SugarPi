set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Set a toolchain path. You only need to set this if the toolchain isn't in
# your system path.
IF( $TC_PATH )
STRING(APPEND TC_PATH "/")
ENDIF()

# The toolchain prefix for all toolchain executables
set( CROSS_COMPILE arm-none-eabi- )

# specify the cross compiler. We force the compiler so that CMake doesn't
# attempt to build a simple test program as this will fail without us using
# the -nostartfiles option on the command line

set( CMAKE_C_COMPILER ${CROSS_COMPILE}gcc )
set( CMAKE_ASM_COMPILER ${CROSS_COMPILE}gcc )

# Because the cross-compiler cannot directly generate a binary without complaining, just test
# compiling a static library instead of an executable program
set( CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY )

# We must set the OBJCOPY setting into cache so that it's available to the
# whole project. Otherwise, this does not get set into the CACHE and therefore
# the build doesn't know what the OBJCOPY filepath is

set( CMAKE_OBJCOPY      ${TC_PATH}${CROSS_COMPILE}objcopy
    CACHE FILEPATH "The toolchain objcopy command " FORCE )

set( CMAKE_OBJDUMP      ${TC_PATH}${CROSS_COMPILE}objdump
CACHE FILEPATH "The toolchain objdump command " FORCE )

# Set the common build flags

# Set the CMAKE C flags (which should also be used by the assembler!
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -nostartfiles" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfloat-abi=hard" )

set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfpu=vfp" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=armv6zk" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mtune=arm1176jzf-s" )

set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS}" CACHE STRING "" )
set( CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS}" CACHE STRING "" )

add_definitions( -DRPI0=1 )

#set toolchain path
#set(tools /mnt/d/PERSO/sync/Dropbox/Dev/02-Github/gcc-arm-9.2-2019.12-x86_64-aarch64-none-elf/bin)
#set(CMAKE_C_COMPILER ${tools}/aarch64-none-elf-gcc)
#set(CMAKE_CXX_COMPILER ${tools}/aarch64-none-elf-g++)

#set(CMAKE_EXE_LINKER_FLAGS "--specs=nosys.specs" CACHE INTERNAL "")

#set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
#set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
#set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
#set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
