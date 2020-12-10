#Project version minimum
cmake_minimum_required(VERSION 3.14.3)

set(CMAKE_SYSTEM_NAME               Generic)
set(CMAKE_SYSTEM_PROCESSOR          aarch64)

# Without that flag CMake is not able to pass test compilation check
set(CMAKE_TRY_COMPILE_TARGET_TYPE   STATIC_LIBRARY)

set(CMAKE_AR                        ${BAREMETAL_ARM_TOOLCHAIN_PATH}aarch64-none-elf-ar${CMAKE_EXECUTABLE_SUFFIX})
set(CMAKE_ASM_COMPILER              ${BAREMETAL_ARM_TOOLCHAIN_PATH}aarch64-none-elf-gcc${CMAKE_EXECUTABLE_SUFFIX})
set(CMAKE_C_COMPILER                ${BAREMETAL_ARM_TOOLCHAIN_PATH}aarch64-none-elf-gcc${CMAKE_EXECUTABLE_SUFFIX})
set(CMAKE_CXX_COMPILER              ${BAREMETAL_ARM_TOOLCHAIN_PATH}aarch64-none-elf-g++${CMAKE_EXECUTABLE_SUFFIX})
set(CMAKE_LINKER                    ${BAREMETAL_ARM_TOOLCHAIN_PATH}aarch64-none-elf-ld${CMAKE_EXECUTABLE_SUFFIX})
set(CMAKE_OBJCOPY                   ${BAREMETAL_ARM_TOOLCHAIN_PATH}aarch64-none-elf-objcopy${CMAKE_EXECUTABLE_SUFFIX} CACHE INTERNAL "")
set(CMAKE_RANLIB                    ${BAREMETAL_ARM_TOOLCHAIN_PATH}aarch64-none-elf-ranlib${CMAKE_EXECUTABLE_SUFFIX} CACHE INTERNAL "")
set(CMAKE_SIZE                      ${BAREMETAL_ARM_TOOLCHAIN_PATH}aarch64-none-elf-size${CMAKE_EXECUTABLE_SUFFIX} CACHE INTERNAL "")
set(CMAKE_STRIP                     ${BAREMETAL_ARM_TOOLCHAIN_PATH}aarch64-none-elf-strip${CMAKE_EXECUTABLE_SUFFIX} CACHE INTERNAL "")

#Project name
project(SugarPi VERSION 1.0.0)

# Flags for different target
set(TARGET_LIST "")
LIST(APPEND TARGET_LIST "3")
LIST(APPEND TARGET_LIST "4")

set(TARGET_PI "4" CACHE STRING "Target")
set_property(CACHE TARGET_PI PROPERTY STRINGS ${TARGET_LIST})

if (TARGET_PI EQUAL "3")
   set (ARCH   "-DAARCH=64 -mcpu=cortex-a53 -mlittle-endian -mcmodel=small")
   set (TARGET "kernel8")
else ()
   if (TARGET_PI EQUAL "4")
      set (ARCH  	"-DAARCH=64 -mcpu=cortex-a72 -mlittle-endian -mcmodel=small")
      set (TARGET "kernel8-rpi4")
   else()
      MESSAGE (FATAL_ERROR "Target not specified/supported : TARGET_PI = ${TARGET_PI}" )
   endif()
endif()

set(SUGARPI_C_FLAGS                 "-D__circle__ -DMINIMUM_DEPENDENCIES -DUSE_VCHIQ_SOUND -DNO_MULTITHREAD -DNOFILTER -DNOZLIB -DLOG_MIXER -DLOGFDC  -DNO_RAW_FORMAT" CACHE INTERNAL "")
set(SUGARPI_C_FLAGS                 "${SUGARPI_C_FLAGS} -DRASPPI=${TARGET_PI}")
set(SUGARPI_C_FLAGS                 "${SUGARPI_C_FLAGS} ${ARCH}")
set(SUGARPI_CXX_FLAGS               "-std=c++1z" CACHE INTERNAL "")

set(CMAKE_C_FLAGS                   "${SUGARPI_C_FLAGS} -Wno-psabi --specs=nosys.specs -fdata-sections -ffunction-sections -Wl,--gc-sections" CACHE INTERNAL "")
set(CMAKE_CXX_FLAGS                 "${SUGARPI_CXX_FLAGS} ${CMAKE_C_FLAGS} -fno-exceptions" CACHE INTERNAL "")

set(CMAKE_C_FLAGS_DEBUG             "${SUGARPI_C_FLAGS} -Os -g " CACHE INTERNAL "")
set(CMAKE_C_FLAGS_RELEASE           "${SUGARPI_C_FLAGS} -Os -DNDEBUG" CACHE INTERNAL "")
set(CMAKE_CXX_FLAGS_DEBUG           "${CMAKE_C_FLAGS_DEBUG} " CACHE INTERNAL "")
set(CMAKE_CXX_FLAGS_RELEASE         "${CMAKE_C_FLAGS_RELEASE} " CACHE INTERNAL "")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(CMAKE_EXE_LINKER_FLAGS "--section-start=.init=0x80000")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Map kernel.map")


set (CIRCLEHOME ${CMAKE_CURRENT_LIST_DIR}/circle)

set (LIBS
   	${CIRCLEHOME}/lib/libcircle.a 
      ${CIRCLEHOME}/lib/fs/fat/libfatfs.a
      ${CIRCLEHOME}/lib/fs/libfs.a
      ${CIRCLEHOME}/lib/usb/libusb.a
      ${CIRCLEHOME}/lib/input/libinput.a
      ${CIRCLEHOME}/lib/sched/libsched.a
      ${CIRCLEHOME}/addon/linux/liblinuxemu.a
		${CIRCLEHOME}/addon/SDCard/libsdcard.a
		${CIRCLEHOME}/addon/fatfs/libfatfs.a
      ${CIRCLEHOME}/addon/vc4/sound/libvchiqsound.a
      ${CIRCLEHOME}/addon/vc4/vchiq/libvchiq.a
)

#source files directory
set(Source_Files
   "src/main.cpp"
   "src/main.cpp"
   "src/kernel.cpp"
   "src/SugarPiSetup.cpp"
   "src/ConfigurationManager.cpp"
   "src/DisplayPi.cpp"
   "src/emulation.cpp"
   "src/KeyboardPi.cpp"
   "src/log.cpp"
   "src/SoundPi.cpp"
   "src/ScreenMenu.cpp"
   "res/coolspot.cpp"
   "CPCCore/CPCCoreEmu/Asic.cpp"
   "CPCCore/CPCCoreEmu/Bus.cpp"
   "CPCCore/CPCCoreEmu/CAPSFile.cpp"
   "CPCCore/CPCCoreEmu/ClockLine.cpp"
   "CPCCore/CPCCoreEmu/CRC.cpp"
   "CPCCore/CPCCoreEmu/CRTC.cpp"
   "CPCCore/CPCCoreEmu/CRTC_0.cpp"
   "CPCCore/CPCCoreEmu/CRTC_1.cpp"
   "CPCCore/CPCCoreEmu/CRTC_2.cpp"
   "CPCCore/CPCCoreEmu/CRTC_3_4.cpp"
   "CPCCore/CPCCoreEmu/DiskBuilder.cpp"
   "CPCCore/CPCCoreEmu/DiskContainer.cpp"
   "CPCCore/CPCCoreEmu/DiskGen.cpp"
   "CPCCore/CPCCoreEmu/DMA.cpp"
   "CPCCore/CPCCoreEmu/DskTypeManager.cpp"
   "CPCCore/CPCCoreEmu/FDC.cpp"
   "CPCCore/CPCCoreEmu/FileAccess.cpp"
   "CPCCore/CPCCoreEmu/FormatTypeCTRAW.cpp"
   "CPCCore/CPCCoreEmu/FormatTypeDSK.cpp"
   "CPCCore/CPCCoreEmu/FormatTypeEDSK.cpp"
   "CPCCore/CPCCoreEmu/FormatTypeHFE.cpp"
   "CPCCore/CPCCoreEmu/FormatTypeHFEv3.cpp"
   "CPCCore/CPCCoreEmu/FormatTypeIPF.cpp"
   "CPCCore/CPCCoreEmu/IDisk.cpp"
   "CPCCore/CPCCoreEmu/KeyboardHandler.cpp"
   "CPCCore/CPCCoreEmu/MediaManager.cpp"
   "CPCCore/CPCCoreEmu/Memoire.cpp"
   "CPCCore/CPCCoreEmu/Monitor.cpp"
   "CPCCore/CPCCoreEmu/Motherboard.cpp"
   "CPCCore/CPCCoreEmu/MultifaceII.cpp"
   "CPCCore/CPCCoreEmu/PPI.cpp"
   "CPCCore/CPCCoreEmu/PlayCity.cpp"
   "CPCCore/CPCCoreEmu/PrinterDefault.cpp"
   "CPCCore/CPCCoreEmu/PSG.cpp"
   "CPCCore/CPCCoreEmu/rand.cpp"
   "CPCCore/CPCCoreEmu/Sig.cpp"
   "CPCCore/CPCCoreEmu/simple_filesystem.cpp"
   "CPCCore/CPCCoreEmu/simple_math.cpp"
   "CPCCore/CPCCoreEmu/simple_regex.cpp"
   "CPCCore/CPCCoreEmu/simple_stdio.cpp"
   "CPCCore/CPCCoreEmu/simple_string.cpp"
   "CPCCore/CPCCoreEmu/Snapshot.cpp"
   "CPCCore/CPCCoreEmu/SoundMixer.cpp"
   "CPCCore/CPCCoreEmu/Tape.cpp"
   "CPCCore/CPCCoreEmu/VGA.cpp"
   "CPCCore/CPCCoreEmu/YMZ294.cpp"
   "CPCCore/CPCCoreEmu/Z80_Full.cpp"
   "CPCCore/CPCCoreEmu/Z80_Opcodes_fetch.cpp"
   "CPCCore/CPCCoreEmu/Z80_Opcodes_ior.cpp"
   "CPCCore/CPCCoreEmu/Z80_Opcodes_iow.cpp"
   "CPCCore/CPCCoreEmu/Z80_Opcodes_memr.cpp"
   "CPCCore/CPCCoreEmu/Z80_Opcodes_memw.cpp"
   "CPCCore/CPCCoreEmu/Z80_Opcodes_z80wait.cpp"
   "CPCCore/CPCCoreEmu/Z84C30.cpp"
)

set(ALL_FILES ${Source_Files})
add_executable(SugarPi ${ALL_FILES} )
target_link_libraries (SugarPi ${LIBS} )



target_include_directories (SugarPi PRIVATE ${CMAKE_CURRENT_LIST_DIR}/circle/include  )
target_include_directories (SugarPi PRIVATE ${CMAKE_CURRENT_LIST_DIR}/circle/addon  )
target_include_directories (SugarPi PRIVATE ${CMAKE_CURRENT_LIST_DIR} )

add_custom_command(
    TARGET SugarPi POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} ./kernel.${TUTORIAL}.${BOARD}${CMAKE_EXECUTABLE_SUFFIX} -O binary ./kernel.${TUTORIAL}.${BOARD}.img
    COMMAND ${CMAKE_OBJDUMP} -l -S -D ./kernel.${TUTORIAL}.${BOARD}${CMAKE_EXECUTABLE_SUFFIX} > ./kernel.${TUTORIAL}.${BOARD}.asm
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Convert the ELF output file to a binary image" )
