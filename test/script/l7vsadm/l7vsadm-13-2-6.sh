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

#valid option: '-n'
RET=`$L7VSADM -S -r -t 172.16.58.126:80 -n sessionless 2>&1 | grep "PARSE ERROR"`
EXPECT="PARSE ERROR: Invalid option value for -r option." 
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -S -r -t 172.16.58.126:80 -n sessionless"
        exit 1
fi

exit 0


