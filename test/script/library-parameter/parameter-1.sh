#!/bin/bash

# set config file
cp materials/parameter-1-l7vs.cf ${L7VSD_CONF_DIR}/l7vs.cf

#Add Service
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi
usleep 100000

RET=`$L7VSADM -V | grep "l7vsd_mainthread"` 
EXPECT="l7vsd_mainthread               debug"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -V"
        exit 1
fi

RET=`cat ${L7VS_LOG_DIR}/l7vsd_TEST.log | grep "\[DEBUG\] l7vsd_mainthread"`
if [ -z "${RET}" ]
then
        echo "Test failed: cat ${L7VS_LOG_DIR}/l7vsd_TEST.log"
        exit 1
fi

exit 0

