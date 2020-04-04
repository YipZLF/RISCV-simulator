cmake_minimum_required(VERSION 2.8)

project(riscv_sim)

set( CMAKE_CXX_COMPILER "g++")
set( CMAKE_BUILD_TYPE debug)
set( CMAKE_CXX_FLAGS "-g")
set( EXECUTABLE_OUTPUT_PATH "./bin" )

INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src)


option(DEBUG_MODE "Activate DEBUG mode." OFF)

if(DEBUG_MODE)
#  add_definitions(-DDEBUG)
endif(DEBUG_MODE)

aux_source_directory(./src SRCS)

add_executable(sim ${SRCS})