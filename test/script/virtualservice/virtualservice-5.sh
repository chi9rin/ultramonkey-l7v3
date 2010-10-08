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

for (( i=40001; i <= 40100; i++ )){
	$L7VSADM -A -t 127.0.0.1:$i -m sessionless 
	if [ $? -ne 0 ]
	then
	        echo "Test failed: $L7VSADM -A -t 127.0.0.1:$i -m sessionless"
	        exit 1
	fi
}

RET=`$L7VSADM -l -n | grep "TCP" | wc -l`

if [ ${RET} -ne 100 ]
then
        echo "Test failed: $L7VSADM -l -n"
        exit 1
fi

exit 0

