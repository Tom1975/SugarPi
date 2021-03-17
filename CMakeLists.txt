
#Project version minimum
cmake_minimum_required(VERSION 3.12.1)

#Project name
project(SugarPi VERSION 1.2.0)

# include circle dir
set ( CIRCLEHOME "${CMAKE_CURRENT_SOURCE_DIR}/circle")

# change optimization to max
set( OPTIMIZE "-O3")

# specific define to add
set ( DEFINE -DARM_ALLOW_MULTI_CORE)


set( CFLAGS "${CFLAGS} -DMINIMUM_DEPENDENCIES -DUSE_VCHIQ_SOUND -DNO_MULTITHREAD -I. -Isrc -ICPCCore/zlib_pi -DNOFILTER -DNOZLIB -DNO_RAW_FORMAT -I${CIRCLEHOME}/addon -DLOG_MIXER -DLOGFDC")
set( CPPFLAGS "${CPPFLAGS} -DMINIMUM_DEPENDENCIES -DUSE_VCHIQ_SOUND -DNO_MULTITHREAD -I. -Isrc -ICPCCore/zlib_pi -DNOFILTER -DNOZLIB -DNO_RAW_FORMAT -I${CIRCLEHOME}/addon -DLOG_MIXER -DLOGFDC -std=c++17")

include (circle.cmake)

include_directories( 
                     ${CMAKE_CURRENT_SOURCE_DIR}/.
                     ${CMAKE_CURRENT_SOURCE_DIR}/src
                     ${CMAKE_CURRENT_SOURCE_DIR}/src/vc4
                     ${CMAKE_CURRENT_SOURCE_DIR}/CPCCore/zlib_pi
                     ${CIRCLEHOME}/include 
                     ${CIRCLEHOME}/addon
                     ${CIRCLEHOME}/addon/vc4
                     ${CIRCLEHOME}/addon/vc4/interface/Khronos/include
                     ${CIRCLEHOME}/app/lib
                   )

#source files directory
set(Source_Files
      src/main.cpp
		src/kernel.cpp
		src/SugarPiSetup.cpp
		src/ConfigurationManager.cpp
		src/DisplayPi.cpp
		src/emulation.cpp
		src/KeyboardPi.cpp
		src/log.cpp
		src/SoundPi.cpp
		src/ScreenMenu.cpp
		src/Windows.cpp
		res/coolspot.cpp
		CPCCore/CPCCoreEmu/Asic.cpp
		CPCCore/CPCCoreEmu/Bus.cpp
		CPCCore/CPCCoreEmu/CAPSFile.cpp
		CPCCore/CPCCoreEmu/ClockLine.cpp
		CPCCore/CPCCoreEmu/CRC.cpp
		CPCCore/CPCCoreEmu/CRTC.cpp
		CPCCore/CPCCoreEmu/CRTC_0.cpp
		CPCCore/CPCCoreEmu/CRTC_1.cpp
		CPCCore/CPCCoreEmu/CRTC_2.cpp
		CPCCore/CPCCoreEmu/CRTC_3_4.cpp
		CPCCore/CPCCoreEmu/DiskBuilder.cpp
		CPCCore/CPCCoreEmu/DiskContainer.cpp
		CPCCore/CPCCoreEmu/DiskGen.cpp
		CPCCore/CPCCoreEmu/DMA.cpp
		CPCCore/CPCCoreEmu/DskTypeManager.cpp
		CPCCore/CPCCoreEmu/FDC.cpp
		CPCCore/CPCCoreEmu/FileAccess.cpp
		CPCCore/CPCCoreEmu/FormatTypeCTRAW.cpp
		CPCCore/CPCCoreEmu/FormatTypeDSK.cpp
		CPCCore/CPCCoreEmu/FormatTypeEDSK.cpp
		CPCCore/CPCCoreEmu/FormatTypeHFE.cpp
		CPCCore/CPCCoreEmu/FormatTypeHFEv3.cpp
		CPCCore/CPCCoreEmu/FormatTypeIPF.cpp
		CPCCore/CPCCoreEmu/IDisk.cpp
		CPCCore/CPCCoreEmu/KeyboardHandler.cpp
		CPCCore/CPCCoreEmu/MediaManager.cpp
		CPCCore/CPCCoreEmu/Memoire.cpp
		CPCCore/CPCCoreEmu/Monitor.cpp
		CPCCore/CPCCoreEmu/Motherboard.cpp
		CPCCore/CPCCoreEmu/MultifaceII.cpp
		CPCCore/CPCCoreEmu/PPI.cpp
		CPCCore/CPCCoreEmu/PlayCity.cpp
		CPCCore/CPCCoreEmu/PrinterDefault.cpp
		CPCCore/CPCCoreEmu/PSG.cpp
		CPCCore/CPCCoreEmu/rand.cpp
		CPCCore/CPCCoreEmu/Sig.cpp
		CPCCore/CPCCoreEmu/simple_filesystem.cpp
		CPCCore/CPCCoreEmu/simple_math.cpp
		CPCCore/CPCCoreEmu/simple_regex.cpp
		CPCCore/CPCCoreEmu/simple_stdio.cpp
		CPCCore/CPCCoreEmu/simple_string.cpp
		CPCCore/CPCCoreEmu/Snapshot.cpp
		CPCCore/CPCCoreEmu/SoundMixer.cpp
		CPCCore/CPCCoreEmu/Tape.cpp
		CPCCore/CPCCoreEmu/VGA.cpp
		CPCCore/CPCCoreEmu/YMZ294.cpp
		CPCCore/CPCCoreEmu/Z80_Full.cpp
		CPCCore/CPCCoreEmu/Z80_Opcodes_fetch.cpp
      CPCCore/CPCCoreEmu/Z80_Opcodes_ior.cpp
      CPCCore/CPCCoreEmu/Z80_Opcodes_iow.cpp
      CPCCore/CPCCoreEmu/Z80_Opcodes_memr.cpp
      CPCCore/CPCCoreEmu/Z80_Opcodes_memw.cpp
      CPCCore/CPCCoreEmu/Z80_Opcodes_z80wait.cpp
		CPCCore/CPCCoreEmu/Z84C30.cpp
)

set(ALL_FILES ${Source_Files})
add_executable(${PROJECT_NAME} ${ALL_FILES})