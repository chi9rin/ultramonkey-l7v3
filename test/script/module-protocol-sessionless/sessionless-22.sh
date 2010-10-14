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
$L7VSADM -A -t 127.0.0.1:40001 -m sessionless
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m sessionless"
        exit 1
fi

#Check default value
RET=`l7vsadm -V -n | grep "TCP 127.0.0.1:40001" | awk '{print $5,$6,$7,$8}'`

if [ "${RET}" != "--sorry-uri '/' --statistic 0" ]
then
        echo "Test failed: l7vsadm -V -n | grep TCP 127.0.0.1:40001 | awk '{print $5,$6,$7,$8}"
        exit 1
fi
exit 0

