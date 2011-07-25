#!/bin/bash

. ${SET_DEFAULT_CONF}

chkconfig daytime-stream on

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

$L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r 127.0.0.1:13
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r 127.0.0.1:13"
        exit 1
fi

#Connect

RET=`nc 127.0.0.1 40001`
if [ -z "$RET" ]
then
        echo "Test failed: nc 127.0.0.1 40001"
        exit 1
fi

exit 0

