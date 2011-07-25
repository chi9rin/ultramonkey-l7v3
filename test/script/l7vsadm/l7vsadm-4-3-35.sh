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

EXPECT="PARSE ERROR: target endpoint parse error(--tcp-service): invalid endpoint:Host not found.*:\[3ae3:90a0:bd05:01d2:288a:1fc0:0001:10ee-ErrorAdder\]:40001"
$L7VSADM -E -t [3ae3:90a0:bd05:01d2:288a:1fc0:0001:10ee-ErrorAdder]:40001 -m sessionless -s wrr 2>&1 | grep "${EXPECT}" > /dev/null
if [ "$?" -ne 0 ]
then
        echo "Test failed: $L7VSADM -E -t [3ae3:90a0:bd05:01d2:288a:1fc0:0001:10ee-ErrorAdder]:40001 -m sessionless -s wrr"
        exit 1
fi

exit 0

