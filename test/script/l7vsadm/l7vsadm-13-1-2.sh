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

$L7VSADM -S -f 0 -t 1 -l debug -i 5
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -S -f 0 -t 1 -l debug -i 5"
        exit 1
fi


$L7VSADM -S -f 1
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -S -f 1"
        exit 1
fi

I=0
RET=`$L7VSADM -V 2>&1 | awk '/SNMPAgent:/ { while(I<4) {getline;  print $0; I++;} }'`
echo "$RET"
EXPECT="Agent Status                   active
log trap status                on
log trap level                 debug
cache update interval          5"

if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -V"
        exit 1
fi

exit 0


