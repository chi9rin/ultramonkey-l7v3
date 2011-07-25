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

RET=`$L7VSADM -A -t [::99]:40001 -m sessionless 2>&1`
EXPECT="add vs error : Cannot assign requested address"

if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -A -t [::99]:40001 -m sessionless"
        exit 1
fi

exit 0

