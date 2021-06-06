# Main CMake for ImageMaker

cmake_minimum_required(VERSION 3.12)
project(ImageMaker VERSION 0.1)

# Verbose
set(CMAKE_VERBOSE_MAKEFILE ON)

# Debug cmake
message(STATUS "CMake source directory is ${CMAKE_SOURCE_DIR}")

#--Compiler to use
set(CMAKE_C_COMPILER "C:/TDM-GCC-64/bin/gcc.exe")
set(CMAKE_CXX_COMPILER "C:/TDM-GCC-64/bin/g++.exe")

#--Bits (compile flags)
set(COMPILE_64BITS "0")
if(COMPILE_64BITS)
	list(APPEND COMPILE_FLAGS_CXX "-m64")
	else()
	list(APPEND COMPILE_FLAGS_CXX "-m32")
endif()

#Build type
set(CMAKE_BUILD_TYPE DEBUG)
#set(CMAKE_BUILD_TYPE RELEASE)

#--Compile flags
list(APPEND COMPILE_FLAGS_CXX "-Wall")
list(APPEND COMPILE_FLAGS_CXX "-std=c++17")

list(JOIN COMPILE_FLAGS_CXX " " COMPILE_FLAGS_CXX)
message(STATUS "Compiling with flags: ${COMPILE_FLAGS_CXX}")

set(CMAKE_CXX_FLAGS ${COMPILE_FLAGS_CXX})

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-s -O3")

#--Libraries
#Includes and library dir
include_directories("${PROJECT_SOURCE_DIR}/libraries/includes")
include_directories("${PROJECT_SOURCE_DIR}/src")

#Source Files
#file(GLOB Engine_services_sources  "${CMAKE_SOURCE_DIR}/src/engine/services/*.cpp")
#file(GLOB Engine_sources  "${CMAKE_SOURCE_DIR}/src/engine/*.cpp")
file(GLOB Main_sources "${CMAKE_SOURCE_DIR}/src/*.cpp")
file(GLOB Main_sources_c "${CMAKE_SOURCE_DIR}/src/*.c")
file(GLOB Lib_sources "${CMAKE_SOURCE_DIR}/libraries/src/*.c")

#--Add executable
add_executable(mainOut "${Main_sources}" "${Main_sources_c}")


target_link_libraries(mainOut)