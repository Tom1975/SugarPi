set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Set a toolchain path. You only need to set this if the toolchain isn't in
# your system path.
IF( $TC_PATH )
STRING(APPEND TC_PATH "/")
ENDIF()

set (CIRCLEHOME CACHE STRING "./circle")

# Specific Sugarpi : Configuration of compiler / build
# The toolchain prefix for all toolchain executables
set( AARCH 64 CACHE STRING "Raspberry pi architecture")

# Target : Raspberry Pi 3 / 4
set( RASPI 4 CACHE STRING "Raspberry pi Target")
set_property(CACHE RASPI PROPERTY STRINGS {3 4})
set( CROSS_COMPILE aarch64-none-elf- CACHE STRING "Tools prefix")

set (PREFIX	arm-none-eabi- )
set (PREFIX64 aarch64-none-elf-)


set( STDLIB_SUPPORT 1 CACHE STRING "STD lib support")
set( CHECK_DEPS 1 CACHE STRING "set this to 0 to globally disable dependency checking")
set( FLOAT_ABI hard CACHE STRING "set this to softfp if you want to link specific libraries")
set( GC_SECTIONS 0 CACHE STRING "set this to 1 to enable garbage collection on sections, may cause side effects")

# specific define to add
set ( DEFINE -DARM_ALLOW_MULTI_CORE)

# specify the cross compiler. We force the compiler so that CMake doesn't
# attempt to build a simple test program as this will fail without us using
# the -nostartfiles option on the command line

set( CMAKE_C_COMPILER ${CROSS_COMPILE}gcc )
set( CMAKE_CXX_COMPILER ${CROSS_COMPILE}g++)
set( CMAKE_ASM_COMPILER ${CROSS_COMPILE}gcc )
set( CMAKE_C_LINK_EXECUTABLE ${CROSS_COMPILE}ld )
set( CMAKE_CXX_LINK_EXECUTABLE ${CROSS_COMPILE}ld )
set( CMAKE_AR ${CROSS_COMPILE}ar )

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

# Set the common build flags, depending ARCH and RASPI
if ( ${AARCH} STREQUAL 32)
    if ( ${RASPI} STREQUAL 1)
        if ( NOT ARCH )
            set ( ARCH "${ARCH} -DAARCH=32 -mcpu=arm1176jzf-s -marm -mfpu=vfp -mfloat-abi=${FLOAT_ABI}")
        endif()
        if ( NOT TARGET)
            set ( TARGET kernel)
        endif()
    elseif ( ${RASPI} STREQUAL 2)
        if ( NOT ARCH )
            set ( ARCH "-DAARCH=32 -mcpu=cortex-a7 -marm -mfpu=neon-vfpv4 -mfloat-abi=${FLOAT_ABI}")
        endif()
        if ( NOT TARGET)
            set ( TARGET kernel7)
        endif()
    elseif ( ${RASPI} STREQUAL 3)
        if ( NOT ARCH )
            set ( ARCH "-DAARCH=32 -mcpu=cortex-a53 -marm -mfpu=neon-fp-armv8 -mfloat-abi=${FLOAT_ABI}")
        endif()
        if ( NOT TARGET)
            set ( TARGET kernel8-32)
        endif()
    elseif ( ${RASPI} STREQUAL 4)
        if ( NOT ARCH )
            set ( ARCH "-DAARCH=32 -mcpu=cortex-a72 -marm -mfpu=neon-fp-armv8 -mfloat-abi=${FLOAT_ABI}")
        endif()
        if ( NOT TARGET)
            set ( TARGET kernel7l)
        endif()
    else ()
        MESSAGE (ERROR "RASPPI must be set to 1, 2, 3 or 4")
    endif()
    set( LOADADDR  0x8000)
elseif (${AARCH} STREQUAL 64)
    if ( ${RASPI} STREQUAL 3)
        if ( NOT ARCH )
            set ( ARCH "-DAARCH=64 -mcpu=cortex-a53 -mlittle-endian -mcmodel=small")
        endif()
        if ( NOT TARGET)
            set ( TARGET kernel8)
        endif()
    elseif ( ${RASPI} STREQUAL 4)
        if ( NOT ARCH )
            set ( ARCH "-DAARCH=64 -mcpu=cortex-a72 -mlittle-endian -mcmodel=small")
        endif()
        if ( NOT TARGET)
            set ( TARGET kernel8-rpi4)
        endif()
    else ()
        MESSAGE (ERROR "RASPPI must be set to 3 or 4")
    endif()

    set( PREFIX	= ${PREFIX64} )
    set( LOADADDR 0x80000)

else ()
    MESSAGE (ERROR "AARCH must be set to 32 or 64")
endif()

# Handle STDLIB_SUPPORT
if ( ${STDLIB_SUPPORT} STREQUAL 3)
endif()

if ( ${STDLIB_SUPPORT} STREQUAL 0)
endif()

if ( ${STDLIB_SUPPORT} STREQUAL 1)
    if ( NOT LIBM )
        set( LIBM "${CPP} ${ARCH} -print-file-name=libm.a")
    else ()
        if ( ${LIBM} STREQUAL "libm.a")
            set( EXTRALIBS ${LIBM})
        endif()
    endif()
endif()

if ( ${GC_SECTIONS} STREQUAL 1)
    set ( CFLAGS "${CFLAGS} -ffunction-sections -fdata-sections")
    set ( LDFLAGS "${LDFLAGS} --gc-sections")
endif()

# Handle GC_SECTIONS

if ( NOT OPTIMIZE)
    set( OPTIMIZE "-O2")
endif()

set ( INCLUDE "${INCLUDE} -I ${CIRCLEHOME}/include -I ${CIRCLEHOME}/addon -I ${CIRCLEHOME}/app/lib")
set ( INCLUDE "${INCLUDE} -I ${CIRCLEHOME}/addon/vc4 -I ${CIRCLEHOME}/addon/vc4/interface/khronos/include")

set ( DEFINE "${DEFINE} -D__circle__ -DRASPPI=${RASPI} -DSTDLIB_SUPPORT=${STDLIB_SUPPORT}")
set ( DEFINE "${DEFINE} -D__VCCOREVER__=0x04000000 -U__unix__ -U__linux__")

set( AFLAGS "${AFLAGS} ${ARCH} ${DEFINE} ${INCLUDE} ${OPTIMIZE}")
set( CFLAGS "${CFLAGS} ${ARCH} -Wall -fsigned-char -ffreestanding ${DEFINE} ${INCLUDE} ${OPTIMIZE} -g")
set( CPPFLAGS "${CPPFLAGS} ${CFLAGS} -std=c++14 -Wno-aligned-new")
set( LDFLAGS "${LDFLAGS} --section-start=.init=${LOADADDR}")


# Set the CMAKE C flags (which should also be used by the assembler!
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CFLAGS}" )
set( CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS} ${AFLAGS}" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CPPFLAGS}" )

add_definitions( -DRPI0=${RASPI} )
add_definitions( ${DEFINE} )

#set toolchain path
#set(tools /mnt/d/PERSO/sync/Dropbox/Dev/02-Github/gcc-arm-9.2-2019.12-x86_64-aarch64-none-elf/bin)
#set(CMAKE_C_COMPILER ${tools}/aarch64-none-elf-gcc)
#set(CMAKE_CXX_COMPILER ${tools}/aarch64-none-elf-g++)

#set(CMAKE_EXE_LINKER_FLAGS "--specs=nosys.specs" CACHE INTERNAL "")

#set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
#set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
#set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
#set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
