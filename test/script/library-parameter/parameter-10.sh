#!/bin/bash

. ${SET_DEFAULT_CONF}

#Add Service
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi
usleep 100000

RET=`$L7VSADM -V | grep "l7vsd_mainthread"` 
EXPECT="l7vsd_mainthread               warn"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -V"
        exit 1
fi

# set config file
cp materials/parameter-10-l7vs.cf ${L7VSD_CONF_DIR}/l7vs.cf

$L7VSADM -P -r logger
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -P -r logger"
        exit 1
fi

RET=`$L7VSADM -V | grep "l7vsd_mainthread"` 
EXPECT="l7vsd_mainthread               debug"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -V"
        exit 1
fi

exit 0

