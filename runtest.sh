#! /bin/bash
PROJECT_PATH=`pwd`
# run tests
for file in `ls ${PROJECT_PATH}/test/build/bin`
do
        echo "Running ${file}"
        ${PROJECT_PATH}/build/bin/sim -p ${PROJECT_PATH}/test/build/bin/${file}
        echo "Press any key to continue"
        read -n 1
done

