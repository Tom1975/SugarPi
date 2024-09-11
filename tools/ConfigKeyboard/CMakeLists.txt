
#Project version minimum
cmake_minimum_required(VERSION 3.12.1)

##################################################
#Project name - GenerateFont
project(ConfigKeyboard VERSION 1.2.0)

set(CMAKE_CXX_STANDARD 17)

#source files directory
set(Source_Files
		${CMAKE_CURRENT_SOURCE_DIR}/ConfigKeyboard.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/res/ConfigKeyboard.rc
		${CMAKE_CURRENT_SOURCE_DIR}/res/resource.h
)

add_definitions()

set(ALL_FILES ${Source_Files})

add_executable(ConfigKeyboard WIN32 )

target_sources(ConfigKeyboard PUBLIC ${ALL_FILES} )
target_link_libraries(ConfigKeyboard PUBLIC Shlwapi.lib Dwmapi.lib Winmm.lib D2d1.lib Msimg32.lib)

target_compile_features(ConfigKeyboard PRIVATE cxx_std_17)
