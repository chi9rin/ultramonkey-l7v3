#!/bin/bash

# set config file
. ${SET_DEFAULT_CONF}

#Start L7VSD Service
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi
usleep 100000

#Set SNMPAgent function enable

$L7VSADM -S -f 1
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -S -f 1"
	exit 1
fi

#Check SNMPAgent function enable
RET=`snmpwalk -v 2c -c public 127.0.0.1 ULTRAMONKEY-L7-MIB::ultramonkey-l7`

EXPECT="ULTRAMONKEY-L7-MIB::l7vsVsNumber.0 = INTEGER: 0
ULTRAMONKEY-L7-MIB::l7vsReplicationMode.0 = INTEGER: single(1)"

if [ "${RET}" != "${EXPECT}" ]
then
        echo "snmpwalk -v 2c -c public 127.0.0.1 ULTRAMONKEY-L7-MIB::ultramonkey-l7"
        exit 1
fi

exit 0
