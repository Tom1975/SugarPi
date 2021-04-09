
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
      main.cpp
		kernel.cpp
		SugarPiSetup.cpp
		ConfigurationManager.cpp
		DisplayPi.cpp
		emulation.cpp
		KeyboardPi.cpp
		log.cpp
		SoundPi.cpp
		ScreenMenu.cpp
		Windows.cpp
		../res/coolspot.cpp
		../CPCCore/CPCCoreEmu/Asic.cpp
		../CPCCore/CPCCoreEmu/Bus.cpp
		../CPCCore/CPCCoreEmu/CAPSFile.cpp
		../CPCCore/CPCCoreEmu/ClockLine.cpp
		../CPCCore/CPCCoreEmu/CRC.cpp
		../CPCCore/CPCCoreEmu/CRTC.cpp
		../CPCCore/CPCCoreEmu/CRTC_0.cpp
		../CPCCore/CPCCoreEmu/CRTC_1.cpp
		../CPCCore/CPCCoreEmu/CRTC_2.cpp
		../CPCCore/CPCCoreEmu/CRTC_3_4.cpp
		../CPCCore/CPCCoreEmu/DiskBuilder.cpp
		../CPCCore/CPCCoreEmu/DiskContainer.cpp
		../CPCCore/CPCCoreEmu/DiskGen.cpp
		../CPCCore/CPCCoreEmu/DMA.cpp
		../CPCCore/CPCCoreEmu/DskTypeManager.cpp
		../CPCCore/CPCCoreEmu/FDC.cpp
		../CPCCore/CPCCoreEmu/FileAccess.cpp
		../CPCCore/CPCCoreEmu/FormatTypeCTRAW.cpp
		../CPCCore/CPCCoreEmu/FormatTypeDSK.cpp
		../CPCCore/CPCCoreEmu/FormatTypeEDSK.cpp
		../CPCCore/CPCCoreEmu/FormatTypeHFE.cpp
		../CPCCore/CPCCoreEmu/FormatTypeHFEv3.cpp
		../CPCCore/CPCCoreEmu/FormatTypeIPF.cpp
		../CPCCore/CPCCoreEmu/IDisk.cpp
		../CPCCore/CPCCoreEmu/KeyboardHandler.cpp
		../CPCCore/CPCCoreEmu/MediaManager.cpp
		../CPCCore/CPCCoreEmu/Memoire.cpp
		../CPCCore/CPCCoreEmu/Monitor.cpp
		../CPCCore/CPCCoreEmu/Motherboard.cpp
		../CPCCore/CPCCoreEmu/MultifaceII.cpp
		../CPCCore/CPCCoreEmu/PPI.cpp
		../CPCCore/CPCCoreEmu/PlayCity.cpp
		../CPCCore/CPCCoreEmu/PrinterDefault.cpp
		../CPCCore/CPCCoreEmu/PSG.cpp
		../CPCCore/CPCCoreEmu/rand.cpp
		../CPCCore/CPCCoreEmu/Sig.cpp
		../CPCCore/CPCCoreEmu/simple_filesystem.cpp
		../CPCCore/CPCCoreEmu/simple_math.cpp
		../CPCCore/CPCCoreEmu/simple_regex.cpp
		../CPCCore/CPCCoreEmu/simple_stdio.cpp
		../CPCCore/CPCCoreEmu/simple_string.cpp
		../CPCCore/CPCCoreEmu/Snapshot.cpp
		../CPCCore/CPCCoreEmu/SoundMixer.cpp
		../CPCCore/CPCCoreEmu/Tape.cpp
		../CPCCore/CPCCoreEmu/VGA.cpp
		../CPCCore/CPCCoreEmu/YMZ294.cpp
		../CPCCore/CPCCoreEmu/Z80_Full.cpp
		../CPCCore/CPCCoreEmu/Z80_Opcodes_fetch.cpp
      ../CPCCore/CPCCoreEmu/Z80_Opcodes_ior.cpp
      ../CPCCore/CPCCoreEmu/Z80_Opcodes_iow.cpp
      ../CPCCore/CPCCoreEmu/Z80_Opcodes_memr.cpp
      ../CPCCore/CPCCoreEmu/Z80_Opcodes_memw.cpp
      ../CPCCore/CPCCoreEmu/Z80_Opcodes_z80wait.cpp
		../CPCCore/CPCCoreEmu/Z84C30.cpp
)

#set(ALL_FILES ${Source_Files})
add_executable(SugarPi ${Source_Files})
#add_dependencies (SugarPi circle)
#target_link_libraries(SugarPi circle.a)
