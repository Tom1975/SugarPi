
#Project version minimum
cmake_minimum_required(VERSION 3.12.1)

##################################################
#Project name - GenerateFont
project(GenerateFont VERSION 1.2.0)

set(CMAKE_CXX_STANDARD 17)

#source files directory
set(Source_Files
		${CMAKE_CURRENT_SOURCE_DIR}/GenerateFont.cpp
)

add_definitions()

set(ALL_FILES ${Source_Files})

add_executable(GenerateFont )

target_sources(GenerateFont PUBLIC ${ALL_FILES} )
target_link_libraries(GenerateFont PUBLIC Shlwapi.lib Dwmapi.lib Winmm.lib D2d1.lib)

target_compile_features(GenerateFont PRIVATE cxx_std_17)

##################################################
#Project name - PNG2BIN
project(Img2bin VERSION 1.2.0)

set(CMAKE_CXX_STANDARD 17)

#source files directory
set(Source_Files
		${CMAKE_CURRENT_SOURCE_DIR}/Img2bin.cpp
)

add_definitions()

set(ALL_FILES ${Source_Files})

add_executable(Img2bin )

target_sources(Img2bin PUBLIC ${ALL_FILES} )
#target_link_libraries(Img2bin PUBLIC Shlwapi.lib Dwmapi.lib Winmm.lib D2d1.lib)

target_compile_features(Img2bin PRIVATE cxx_std_17)


##################################################
#Project name - ConfigKeyboard
add_subdirectory(ConfigKeyboard)
