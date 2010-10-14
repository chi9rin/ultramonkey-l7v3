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

RET=`$L7VSADM -S -t 1 -t a 2>&1 | grep "PARSE ERROR"`
EXPECT="PARSE ERROR : Option -t is conflict." 
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -S -t 1 -t a"
        exit 1
fi

exit 0


