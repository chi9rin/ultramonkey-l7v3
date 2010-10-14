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
RET=`$L7VSADM -S -r -m sessionless -m sessionless 2>&1 | grep "PARSE ERROR"`
EXPECT="PARSE ERROR : Option -m is conflict." 
echo "$RET"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -S -r -m sessionless -m sessionless"
        exit 1
fi

exit 0


