set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Set a toolchain path. You only need to set this if the toolchain isn't in
# your system path.

# Specific Sugarpi : Configuration of compiler / build
# The toolchain prefix for all toolchain executables
set( AARCH 64 CACHE STRING "Raspberry pi architecture")
set( CROSS_COMPILE aarch64-none-elf- CACHE STRING "Tools prefix")
set( CMAKE_C_COMPILER ${CROSS_COMPILE}gcc )
set( CMAKE_CXX_COMPILER ${CROSS_COMPILE}g++)
set( CMAKE_ASM_COMPILER ${CROSS_COMPILE}gcc )
set( CMAKE_C_LINK_EXECUTABLE ${CROSS_COMPILE}ld )
set( CMAKE_CXX_LINK_EXECUTABLE ${CROSS_COMPILE}ld )
set( CMAKE_AR ${CROSS_COMPILE}ar )

set( CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY )

set( CMAKE_OBJCOPY      ${TC_PATH}${CROSS_COMPILE}objcopy
    CACHE FILEPATH "The toolchain objcopy command " FORCE )

set( CMAKE_OBJDUMP      ${TC_PATH}${CROSS_COMPILE}objdump
CACHE FILEPATH "The toolchain objdump command " FORCE )

# Set the common build flags, depending ARCH and RASPI
