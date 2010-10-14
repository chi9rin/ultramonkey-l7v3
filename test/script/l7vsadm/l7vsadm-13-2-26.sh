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

RET=`$L7VSADM -S -l 2>&1 | grep "PARSE ERROR"`
EXPECT="PARSE ERROR : Need option value for -l option." 
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -S -l"
        exit 1
fi

exit 0


