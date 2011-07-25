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

#invalid endpoint(no port): 172.16.58.126
RET=`$L7VSADM -S -r -t 172.16.58.126 -m sessionless 2>&1 | grep "PARSE ERROR"`
EXPECT="PARSE ERROR: target endpoint parse error:invalid endpoint:Service not found:172.16.58.126" 
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -S -r -t 172.16.58.126 -m sessionless"
        exit 1
fi

exit 0


