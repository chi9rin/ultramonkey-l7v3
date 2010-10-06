#!/bin/bash

. ${SET_DEFAULT_CONF}

STR=`for ((i = 0 ; i < 126; i++ )){
	echo -n "a"
}`

#Add Service
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi

usleep 100000
$L7VSADM -A -t 127.0.0.1:40001 -m ip --sorry-uri /$STR
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m ip --sorry-uri /$STR"
        exit 1
fi

$L7VSADM -A -t 127.0.0.1:40002 -m ip -S /$STR
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m ip -S /$STR"
        exit 1
fi

RET=`$L7VSADM -V -n | grep "TCP 127.0.0.1:40001"`
if [ "$RET" != "TCP 127.0.0.1:40001 ip rr --timeout 3600 --no-reschedule --sorry-uri '/$STR' --statistic 0" ]
then
        echo "Test failed: $L7VSADM -V -n | grep TCP 127.0.0.1:40001"
        exit 1
fi

RET=`$L7VSADM -V -n | grep "TCP 127.0.0.1:40002"`
if [ "$RET" != "TCP 127.0.0.1:40002 ip rr --timeout 3600 --no-reschedule --sorry-uri '/$STR' --statistic 0" ]
then
        echo "Test failed: $L7VSADM -V -n | grep TCP 127.0.0.1:40002"
        exit 1
fi

exit 0

