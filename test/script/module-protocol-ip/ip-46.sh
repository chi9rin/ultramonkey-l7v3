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
$L7VSADM -A -t 127.0.0.1:40001 -m ip --forwarded-for -F
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m ip --forwarded-for -F"
        exit 1
fi

RET=`l7vsadm -V -n | grep "TCP 127.0.0.1:40001" | awk '{print $7}'`
if [ "$RET" != "--forwarded-for" ]
then
        echo "Test failed: l7vsadm -V -n | grep TCP 127.0.0.1:40001 | awk '{print $7}"
        exit 1
fi
exit 0

