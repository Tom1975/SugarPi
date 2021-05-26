
#Project version minimum
cmake_minimum_required(VERSION 3.12.1)

#Project name
project(SugarPi VERSION 1.2.0)

include (${CMAKE_CURRENT_SOURCE_DIR}/../CMake/circle.cmake)

add_definitions("-std=c++1z")
add_definitions("-DMINIMUM_DEPENDENCIES")

include_directories( 
                     ${CMAKE_CURRENT_SOURCE_DIR}/..
                     ${CMAKE_CURRENT_SOURCE_DIR}/.
                     ${CMAKE_CURRENT_SOURCE_DIR}/vc4
                     ${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/zlib_pi
                     ${CIRCLEHOME}/include 
                     ${CIRCLEHOME}/addon
                     ${CIRCLEHOME}/addon/vc4
                     ${CIRCLEHOME}/addon/vc4/interface/Khronos/include
                     ${CIRCLEHOME}/app/lib
                   )

#source files directory
set(Source_Files
		${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/kernel.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/SugarPiSetup.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/ConfigurationManager.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/DisplayPi.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/emulation.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/KeyboardPi.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/log.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/SoundPi.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/ScreenMenu.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Windows.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../res/coolspot.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/Asic.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/Bus.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/CAPSFile.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/ClockLine.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/CRC.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/CRTC.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/CRTC_0.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/CRTC_1.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/CRTC_2.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/CRTC_3_4.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/DiskBuilder.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/DiskContainer.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/DiskGen.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/DMA.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/DskTypeManager.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/FDC.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/FileAccess.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/FormatTypeCTRAW.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/FormatTypeDSK.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/FormatTypeEDSK.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/FormatTypeHFE.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/FormatTypeHFEv3.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/FormatTypeIPF.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/IDisk.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/KeyboardHandler.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/MediaManager.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/Memoire.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/Monitor.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/Motherboard.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/MultifaceII.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/PPI.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/PlayCity.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/PrinterDefault.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/PSG.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/rand.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/Sig.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/simple_filesystem.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/simple_math.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/simple_regex.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/simple_stdio.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/simple_string.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/Snapshot.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/SoundMixer.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/Tape.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/VGA.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/YMZ294.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/Z80_Full.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/Z80_Opcodes_fetch.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/Z80_Opcodes_ior.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/Z80_Opcodes_iow.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/Z80_Opcodes_memr.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/Z80_Opcodes_memw.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/Z80_Opcodes_z80wait.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/Z84C30.cpp
)

set(ALL_FILES ${Source_Files})
add_executable(SugarPi ${ALL_FILES} )
target_link_libraries(SugarPi circle)
