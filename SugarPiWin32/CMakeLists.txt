
#Project version minimum
cmake_minimum_required(VERSION 3.12.1)

#Project name

add_subdirectory(../tools ${CMAKE_CURRENT_BINARY_DIR}/tools)


project(SugarPiWin32 VERSION 1.2.0)

set(CMAKE_CXX_STANDARD 17)

#source files directory
set(Source_Files
		${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/CLogger.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/CLogger.h
		${CMAKE_CURRENT_SOURCE_DIR}/DisplayPiDesktop.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/DisplayPiDesktop.h
		${CMAKE_CURRENT_SOURCE_DIR}/emulation.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/emulation.h
		${CMAKE_CURRENT_SOURCE_DIR}/KeyboardPiDesktop.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/KeyboardPiDesktop.h
		${CMAKE_CURRENT_SOURCE_DIR}/SoundPiDesktop.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/SoundPiDesktop.h
		${CMAKE_CURRENT_SOURCE_DIR}/SugarPiSetupDesktop.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/SugarPiSetupDesktop.h
#		${CMAKE_CURRENT_SOURCE_DIR}/ConfigurationManagerDesktop.cpp
#		${CMAKE_CURRENT_SOURCE_DIR}/ConfigurationManagerDesktop.h
		

#		${CMAKE_CURRENT_SOURCE_DIR}/kernel.cpp
#		${CMAKE_CURRENT_SOURCE_DIR}/kernel.h
#		${CMAKE_CURRENT_SOURCE_DIR}/SugarPiSetup.cpp
#		${CMAKE_CURRENT_SOURCE_DIR}/SugarPiSetup.h
#		${CMAKE_CURRENT_SOURCE_DIR}/ConfigurationManager.cpp
#		${CMAKE_CURRENT_SOURCE_DIR}/ConfigurationManager.h
#		${CMAKE_CURRENT_SOURCE_DIR}/DisplayPi.cpp
#		${CMAKE_CURRENT_SOURCE_DIR}/DisplayPi.h
#		${CMAKE_CURRENT_SOURCE_DIR}/emulation.cpp
#		${CMAKE_CURRENT_SOURCE_DIR}/emulation.h
#		${CMAKE_CURRENT_SOURCE_DIR}/KeyboardPi.cpp
#		${CMAKE_CURRENT_SOURCE_DIR}/KeyboardPi.h
#		${CMAKE_CURRENT_SOURCE_DIR}/log.cpp
#		${CMAKE_CURRENT_SOURCE_DIR}/log.h
#		${CMAKE_CURRENT_SOURCE_DIR}/SoundPi.cpp
#		${CMAKE_CURRENT_SOURCE_DIR}/SoundPi.h
#		${CMAKE_CURRENT_SOURCE_DIR}/ScreenMenu.cpp
#		${CMAKE_CURRENT_SOURCE_DIR}/ScreenMenu.h
#		${CMAKE_CURRENT_SOURCE_DIR}/Window.cpp
#		${CMAKE_CURRENT_SOURCE_DIR}/Window.h
#		${CMAKE_CURRENT_SOURCE_DIR}/../res/coolspot.cpp
#		${CMAKE_CURRENT_SOURCE_DIR}/../res/coolspot.h
)

set(SugarPi_Files
	${CMAKE_CURRENT_SOURCE_DIR}/../src/BackFrame.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../src/BackFrame.h
	${CMAKE_CURRENT_SOURCE_DIR}/../src/BasicFrame.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../src/BasicFrame.h
	${CMAKE_CURRENT_SOURCE_DIR}/../src/ConfigurationManager.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../src/ConfigurationManager.h
	${CMAKE_CURRENT_SOURCE_DIR}/../src/CString.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../src/CString.h
	${CMAKE_CURRENT_SOURCE_DIR}/../src/DisplayPi.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../src/DisplayPi.h
	${CMAKE_CURRENT_SOURCE_DIR}/../src/EmulationFrame.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../src/EmulationFrame.h
	${CMAKE_CURRENT_SOURCE_DIR}/../src/Engine.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../src/Engine.h
	${CMAKE_CURRENT_SOURCE_DIR}/../src/files.h
	${CMAKE_CURRENT_SOURCE_DIR}/../src/files.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../src/log.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../src/log.h
	${CMAKE_CURRENT_SOURCE_DIR}/../src/MainMenuWindows.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../src/MainMenuWindows.h
	${CMAKE_CURRENT_SOURCE_DIR}/../src/MenuFrame.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../src/MenuFrame.h
	${CMAKE_CURRENT_SOURCE_DIR}/../src/Morphing.h
	${CMAKE_CURRENT_SOURCE_DIR}/../src/Morphings.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../src/Morphings.h
	${CMAKE_CURRENT_SOURCE_DIR}/../src/schrift.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../src/schrift.h
	${CMAKE_CURRENT_SOURCE_DIR}/../src/ScreenMenu.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../src/ScreenMenu.h
	${CMAKE_CURRENT_SOURCE_DIR}/../src/Window.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../src/Window.h
	${CMAKE_CURRENT_SOURCE_DIR}/../res/SugarboxLogo.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../res/SugarboxLogo.h

)

set(CpcCore_Files
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
		${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/MachineSettings.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/MachineSettings.h
		${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/Memoire.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/Monitor.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/Motherboard.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/MultifaceII.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/PPI.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/PlayCity.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/PrinterDefault.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/PSG.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu/Screen.h
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

add_definitions()

source_group("Application" FILES ${RES} )
source_group("CPCCORE" FILES ${CpcCore_Files} )
source_group("Sugarpi" FILES ${SugarPi_Files} )

set(ALL_FILES ${Source_Files} ${CpcCore_Files} ${SugarPi_Files})

add_executable(SugarPiWin32 WIN32)

target_sources(SugarPiWin32 PUBLIC ${ALL_FILES} )
target_link_libraries(SugarPiWin32 PUBLIC Shlwapi.lib Dwmapi.lib Winmm.lib D2d1.lib)

target_include_directories ( SugarPiWin32 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/.)
target_include_directories ( SugarPiWin32 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../)
target_include_directories ( SugarPiWin32 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../src)
target_include_directories ( SugarPiWin32 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../CPCCore/CPCCoreEmu)
                     
target_compile_definitions(SugarPiWin32 PUBLIC -DNO_MULTITHREAD)

target_compile_features(SugarPiWin32  PRIVATE cxx_std_17)
target_compile_definitions(SugarPiWin32 PUBLIC -DNOFILTER -DNOZLIB -DNO_RAW_FORMAT -DLOG_MIXER -DLOGFDC -DNO_CUSTOM_OPCODES -DARM_ALLOW_MULTI_CORE)
target_compile_definitions(SugarPiWin32 PUBLIC -DPROFILE -DTEST_VECTOR)


set (CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/Install)
install (TARGETS SugarPiWin32 RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})

install (DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../CART DESTINATION ${CMAKE_INSTALL_PREFIX})
install (FILES ${CMAKE_CURRENT_SOURCE_DIR}/../config_win32  DESTINATION ${CMAKE_INSTALL_PREFIX}/Config RENAME config)
install (FILES ${CMAKE_CURRENT_SOURCE_DIR}/../gamecontrollerdb.txt DESTINATION ${CMAKE_INSTALL_PREFIX}/Config)
install (FILES ${CMAKE_CURRENT_SOURCE_DIR}/../res/101_keyboard  DESTINATION ${CMAKE_INSTALL_PREFIX}/LAYOUT)
install (FILES "${CMAKE_CURRENT_SOURCE_DIR}/../res/Facile_Sans.ttf"  DESTINATION ${CMAKE_INSTALL_PREFIX}/FONTS)
install (FILES "${CMAKE_CURRENT_SOURCE_DIR}/../res/logo.bin"  DESTINATION ${CMAKE_INSTALL_PREFIX}/RES)
install (DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../res/ROMS DESTINATION ${CMAKE_INSTALL_PREFIX}/)
install (DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../res/AMSTRAD DESTINATION ${CMAKE_INSTALL_PREFIX})



set_property(TARGET SugarPiWin32 PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_INSTALL_PREFIX}")
