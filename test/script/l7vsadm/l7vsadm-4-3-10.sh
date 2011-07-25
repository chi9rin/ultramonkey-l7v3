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

RET=`$L7VSADM -A -t 0.0.0.0:40001 -m ip -s wrr 2>&1`

if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t 0.0.0.0:40001 -m ip -s wrr"
        exit 1
fi

if [ -n "$RET" ]
then
        echo "Test failed: $L7VSADM -A -t 0.0.0.0:40001 -m ip -s wrr"
        exit 1
fi

RET=`$L7VSADM -E -t 0.0.0.0:40001 -m ip -s rr 2>&1`
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -E -t 0.0.0.0:40001 -m ip -s rr"
        exit 1
fi

if [ -n "$RET" ]
then
        echo "Test failed: $L7VSADM -E -t 0.0.0.0:40001 -m ip -s rr"
        exit 1
fi

exit 0

