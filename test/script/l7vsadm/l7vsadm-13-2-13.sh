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

RET=`$L7VSADM -S -f 2  2>&1 | grep "PARSE ERROR"`
EXPECT="PARSE ERROR: Invalid option value for -f option." 
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -S -f 2"
        exit 1
fi

exit 0


