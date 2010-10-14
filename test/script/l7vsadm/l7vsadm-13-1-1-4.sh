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

$L7VSADM -S -f 0 -l debug
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -S -f 0 debug"
        exit 1
fi

RET=`$L7VSADM -V 2>&1 | awk '/SNMPAgent:/ { while($0 != "") {getline;  print $0;} }'`
EXPECT="Agent Status                   inactive
log trap status                none
log trap level                 none
cache update interval          none
start date                     none
last request date              none
last trap date                 none
total GET requests             none
total SET requests             none
total trap counts              none"

if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -V"
        exit 1
fi

exit 0


