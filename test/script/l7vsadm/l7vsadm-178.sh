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

$L7VSADM -A -t [::1]:40001 -m sessionless -s rr
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t [::1]:40001 -m sessionless -s rr"
        exit 1
fi




RET=`$L7VSADM -E -t [::2]:40001 -m sessionless -s wrr 2>&1`
EXPECT="edit vs error : virtual service not found."
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -E -t [::2]:40001 -m sessionless -s wrr"
        exit 1
fi

exit 0

