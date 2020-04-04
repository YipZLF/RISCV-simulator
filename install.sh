#!/bin/bash
PROJECT_PATH=`pwd`

# build test bin
mkdir ${PROJECT_PATH}/test/build
cd ${PROJECT_PATH}/test/build
cmake ..
make

# build sim
mkdir ${PROJECT_PATH}/build
cd ${PROJECT_PATH}/build
cmake ..
make


