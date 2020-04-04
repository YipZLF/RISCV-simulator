cmake_minimum_required(VERSION 2.8)


set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS "-g")
set(EXECUTABLE_OUTPUT_PATH "./bin" )

project(riscv_sim)


INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src)


option(DEBUG_MODE "Activate DEBUG mode." ON)

if(DEBUG_MODE)
  add_definitions(-DDEBUG)
endif(DEBUG_MODE)

aux_source_directory(./src SRCS)

add_executable(sim ${SRCS})