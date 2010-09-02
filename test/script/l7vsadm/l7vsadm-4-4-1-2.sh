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


for (( i = 40001; i <= 40030; i++)){
	$L7VSADM -A -t 127.0.0.1:$i -m ip
	if [ $? -ne 0 ]
	then
        	echo "Test failed: $L7VSADM -A -t 127.0.0.1:$i -m ip"
	        exit 1
	fi
}

for (( i = 50001; i <= 50030; i++)){
	$L7VSADM -a -t 127.0.0.1:40001 -m ip -r 127.0.0.1:$i
	if [ $? -ne 0 ]
	then
	        echo "Test failed: $L7VSADM -a -t 127.0.0.1:40001 -m ip -r 127.0.0.1:$1"
        	exit 1
	fi
}

$L7VSADM -C
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -C"
        exit 1
fi

RET=`$L7VSADM`
EXPECT="Layer-7 Virtual Server version 3.0.0-1
Prot LocalAddress:Port ProtoMod Scheduler
  -> RemoteAddress:Port           Forward Weight ActiveConn InactConn"

if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM"
        exit 1
fi

exit 0

