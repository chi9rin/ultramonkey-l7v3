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
$L7VSADM -A -t 127.0.0.1:44431 -m ip
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:44431 -m ip"
        exit 1
fi

#Check default value
RET=`$L7VSADM -V -n | grep "127.0.0.1:44431" | awk '{print $5,$6,$7,$8,$9}'`
EXPECT="--timeout 3600 --no-reschedule --sorry-uri '/'"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -V -n"
        exit 1
fi

exit 0

