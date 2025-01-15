# Set a toolchain path. You only need to set this if the toolchain isn't in
# your system path.
#IF( $TC_PATH )
#STRING(APPEND TC_PATH "/")
#ENDIF()

#set (CIRCLEHOME CACHE STRING "./circle")

# Specific Sugarpi : Configuration of compiler / build
# The toolchain prefix for all toolchain executables
set( AARCH 64 CACHE STRING "Raspberry pi architecture")

# Target : Raspberry Pi 3 / 4
set( RASPI 4 CACHE STRING "Raspberry pi Target")
set_property(CACHE RASPI PROPERTY STRINGS {3 4})
#set( CROSS_COMPILE /mnt/d/PERSO/sync/Dropbox/Dev/02-Github/gcc-arm-9.2-2019.12-x86_64-aarch64-none-elf/bin/aarch64-none-elf- CACHE STRING "Tools prefix")

#set (PREFIX	arm-none-eabi- )
#set (PREFIX64 aarch64-none-elf-)

set( STDLIB_SUPPORT 1 CACHE STRING "STD lib support")
set( CHECK_DEPS 1 CACHE STRING "set this to 0 to globally disable dependency checking")
set( FLOAT_ABI hard CACHE STRING "set this to softfp if you want to link specific libraries")
set( GC_SECTIONS 0 CACHE STRING "set this to 1 to enable garbage collection on sections, may cause side effects")

# specify the cross compiler. We force the compiler so that CMake doesn't
# attempt to build a simple test program as this will fail without us using
# the -nostartfiles option on the command line

#set( CMAKE_C_COMPILER ${CROSS_COMPILE}gcc )
#set( CMAKE_CXX_COMPILER ${CROSS_COMPILE}g++)
#set( CMAKE_ASM_COMPILER ${CROSS_COMPILE}gcc )
#set( CMAKE_C_LINK_EXECUTABLE ${CROSS_COMPILE}ld )
#set( CMAKE_CXX_LINK_EXECUTABLE ${CROSS_COMPILE}ld )
#set( CMAKE_AR ${CROSS_COMPILE}ar )

# Because the cross-compiler cannot directly generate a binary without complaining, just test
# compiling a static library instead of an executable program
set( CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY )

# We must set the OBJCOPY setting into cache so that it's available to the
# whole project. Otherwise, this does not get set into the CACHE and therefore
# the build doesn't know what the OBJCOPY filepath is

#set( CMAKE_OBJCOPY      ${TC_PATH}${CROSS_COMPILE}objcopy
#    CACHE FILEPATH "The toolchain objcopy command " FORCE )

#set( CMAKE_OBJDUMP      ${TC_PATH}${CROSS_COMPILE}objdump
#CACHE FILEPATH "The toolchain objdump command " FORCE )

# Set the common build flags, depending ARCH and RASPI
if ( ${AARCH} STREQUAL 32)
    if ( ${RASPI} STREQUAL 1)
        set ( ARCH  -DAARCH=32 -mcpu=arm1176jzf-s -marm -mfpu=vfp -mfloat-abi=${FLOAT_ABI})
        set ( KERNEL  kernel)
    elseif ( ${RASPI} STREQUAL 2)
        set ( ARCH -DAARCH=32 -mcpu=cortex-a7 -marm -mfpu=neon-vfpv4 -mfloat-abi=${FLOAT_ABI})
        set ( KERNEL  kernel7 )
    elseif ( ${RASPI} STREQUAL 3)
        set ( ARCH -DAARCH=32 -mcpu=cortex-a53 -marm -mfpu=neon-fp-armv8 -mfloat-abi=${FLOAT_ABI})
        set ( KERNEL  kernel8-32 )
    elseif ( ${RASPI} STREQUAL 4)
        set ( ARCH -DAARCH=32 -mcpu=cortex-a72 -marm -mfpu=neon-fp-armv8 -mfloat-abi=${FLOAT_ABI})
        set ( KERNEL  kernel7l )
    else ()
        MESSAGE (ERROR "RASPPI must be set to 1, 2, 3 or 4")
    endif()
    set( LOADADDR  0x8000)
elseif (${AARCH} STREQUAL 64)
    if ( ${RASPI} STREQUAL 3)
        set ( ARCH -DAARCH=64 -mcpu=cortex-a53 -mlittle-endian -mcmodel=small)
        set ( KERNEL  kernel8 )
    elseif ( ${RASPI} STREQUAL 4)
        set ( ARCH -DAARCH=64 -mcpu=cortex-a72 -mlittle-endian -mcmodel=small)
        set ( KERNEL  kernel8-rpi4 )
    else ()
        MESSAGE (ERROR "RASPPI must be set to 3 or 4")
    endif()

    set( PREFIX	= ${PREFIX64} )
    set( LOADADDR 0x80000)

else ()
    MESSAGE (ERROR "AARCH must be set to 32 or 64")
endif()

#add_compile_definitions(circle)
add_compile_definitions(RASPPI=${RASPI})

add_compile_options(${ARCH})
add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>)
add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>)
add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-nostdinc++>)
add_compile_options(-O2)
add_compile_options(-Wall -fsigned-char -ffreestanding)
add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-std=c++17>)
add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wno-aligned-new>)

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

#set ( INCLUDE "${INCLUDE} -I ${CIRCLEHOME}/include -I ${CIRCLEHOME}/addon -I ${CIRCLEHOME}/app/lib")
#set ( INCLUDE "${INCLUDE} -I ${CIRCLEHOME}/addon/vc4 -I ${CIRCLEHOME}/addon/vc4/interface/khronos/include")

#set ( DEFINE "${DEFINE} -D__circle__  -DSTDLIB_SUPPORT=${STDLIB_SUPPORT}")
#set ( DEFINE "${DEFINE} -D__VCCOREVER__=0x04000000 -U__unix__ -U__linux__")

add_compile_definitions( STDLIB_SUPPORT=${STDLIB_SUPPORT} VCCOREVER=0x04000000)
add_compile_options( -U__unix__ -U__linux__ )

#set( AFLAGS "${AFLAGS} ${ARCH} ${INCLUDE} ${OPTIMIZE}")
#set( CFLAGS "${CFLAGS} ${ARCH} -Wall -fsigned-char -ffreestanding ${INCLUDE} ${OPTIMIZE} -g")
#set( CPPFLAGS "${CPPFLAGS} ${CFLAGS} -std=c++14 -Wno-aligned-new")


# Set the CMAKE C flags (which should also be used by the assembler!
#set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CFLAGS}" )
#set( CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS} ${AFLAGS}" )
#set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CPPFLAGS}" )

#SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${LDFLAGS}")

add_definitions( -DRPI0=${RASPI} )
add_definitions( -D__circle__)
#add_definitions( ${DEFINE} )

execute_process(COMMAND ${CMAKE_C_COMPILER} ${ARCH} -print-file-name=libgcc.a
OUTPUT_VARIABLE LIBGCC_PATH
OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(COMMAND ${CMAKE_C_COMPILER} ${ARCH} -print-file-name=libm.a
OUTPUT_VARIABLE LIBM_PATH
OUTPUT_STRIP_TRAILING_WHITESPACE)

add_link_options(--section-start=.init=${LOADADDR})
add_link_options(-T ${CIRCLEHOME}/circle.ld)

