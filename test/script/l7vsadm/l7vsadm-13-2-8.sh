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

#module name's max length is 16
RET=`$L7VSADM -S -r -t 172.16.58.126:80 -m 12345678901234567 2>&1 | grep "PARSE ERROR"`
EXPECT="PARSE ERROR: protomod name is too long." 
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -S -r -t 172.16.58.126:80 -m 12345678901234567"
        exit 1
fi

exit 0


