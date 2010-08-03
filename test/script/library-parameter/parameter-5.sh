#!/bin/bash

# set config file
cp materials/parameter-5-l7vs.cf ${L7VSD_CONF_DIR}/l7vs.cf

#Add Service
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi
usleep 100000

RET=`netstat -a -u -p -n | grep l7vsd | awk '{print $4}'`
EXPECT="127.0.0.1:40000"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: netstat -a -u -p | grep l7vsd"
        exit 1
fi

exit 0

