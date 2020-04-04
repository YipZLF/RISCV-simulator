cmake_minimum_required(VERSION 3.14)

set( CMAKE_C_COMPILER "riscv64-unknown-elf-gcc")
set(EXECUTABLE_OUTPUT_PATH "./bin" )

project(riscv_test)

file(GLOB test_list RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/*.c")

foreach(file ${test_list})
  get_filename_component(filename ${file} NAME_WE)
  add_executable(${filename} ${file})
endforeach()
