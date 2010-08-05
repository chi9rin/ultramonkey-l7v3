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

RET=`$L7VSADM -P -r command 2>&1 | grep "parameter error : parameter reload command not found."`
EXPECT="parameter error : parameter reload command not found."
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -P -r command"
        exit 1
fi

exit 0

