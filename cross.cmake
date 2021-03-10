set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(test-project_path YOUR_TEST_PROJECT_PATH)

#set toolchain path
set(tools /mnt/d/PERSO/sync/Dropbox/Dev/02-Github/gcc-arm-9.2-2019.12-x86_64-aarch64-none-elf/bin)
set(CMAKE_C_COMPILER ${tools}/aarch64-none-elf-gcc)
set(CMAKE_CXX_COMPILER ${tools}/aarch64-none-elf-g++)

set( CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY )

set(CMAKE_SYSROOT ${sysroot_target})
SET(CMAKE_CXX_FLAGS " -nostdinc++")
SET(CMAKE_C_FLAGS ${CMAKE_CXX_FLAGS})
SET(CMAKE_EXE_LINKER_FLAGS " --specs=nosys.specs")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
