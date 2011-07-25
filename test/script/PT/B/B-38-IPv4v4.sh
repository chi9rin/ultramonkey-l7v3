#!/bin/bash

VS1="127.0.0.1"
RS1="127.0.0.1"

. ${SET_DEFAULT_CONF}

#start
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi
usleep 100000

#Add Service 
$L7VSADM -A -t $VS1:8080 -m sessionless -s rr
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t $VS1:8080 -m sessionless -s rr"
        exit 1
fi

$L7VSADM -a -t $VS1:8080 -m sessionless -r $RS1:80
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -a -t $VS1:8080 -m sessionless -r $RS1:80"
        exit 1
fi



RET=`$L7VSADM -V 2>&1 | grep "Agent Status"`
EXPECT="Agent Status                   inactive"

if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -V"
        exit 1
fi

#enable snmpagent
$L7VSADM -S -f 1
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -S -f 1"
        exit 1
fi

RET=`$L7VSADM -V 2>&1 | grep "Agent Status"`
EXPECT="Agent Status                   active"

if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -V"
        exit 1
fi


exit 0


