#!/bin/bash

# set config file
cp materials/parameter-54-l7vs.cf ${L7VSD_CONF_DIR}/l7vs.cf

#Add Service
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi

RET=`$L7VSADM -V | awk '/Replication Mode:/ { getline; print $0 }'`
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -V"
        exit 1
fi

EXPECT="SINGLE"

if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -V"
        exit 1
fi

exit 0

