#!/bin/bash
PROJECT_PATH=`pwd`

# build sim
mkdir ${PROJECT_PATH}/build
cd ${PROJECT_PATH}/build
cmake ..
make


