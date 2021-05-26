
#Project version minimum
cmake_minimum_required(VERSION 3.12.1)

#Project name
project(SugarPiWorkspace VERSION 1.2.0)

# include circle dir
set ( CIRCLEHOME "${CMAKE_CURRENT_SOURCE_DIR}/circle" CACHE STRING "Circle home directory")

# change optimization to max
set( OPTIMIZE "-O3" CACHE STRING "Optimisation")

# specific define to add
set ( DEFINE -DARM_ALLOW_MULTI_CORE CACHE STRING "Multicore application")


set( CFLAGS "${CFLAGS} -DMINIMUM_DEPENDENCIES -DUSE_VCHIQ_SOUND -DNO_MULTITHREAD -I. -Isrc -ICPCCore/zlib_pi -DNOFILTER -DNOZLIB -DNO_RAW_FORMAT -I${CIRCLEHOME}/addon -DLOG_MIXER -DLOGFDC")
set( CPPFLAGS "${CPPFLAGS} -DMINIMUM_DEPENDENCIES -DUSE_VCHIQ_SOUND -DNO_MULTITHREAD -I. -Isrc -ICPCCore/zlib_pi -DNOFILTER -DNOZLIB -DNO_RAW_FORMAT -I${CIRCLEHOME}/addon -DLOG_MIXER -DLOGFDC -std=c++17")

#include (CMake/circle.cmake)
add_subdirectory(CMake/libcircle)
add_subdirectory(src)
