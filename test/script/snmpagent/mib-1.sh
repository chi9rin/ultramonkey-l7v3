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

#Check L7VSD Service
$L7VSADM
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM"
        exit 1
fi
#Check SNMPAgent function disable
RET=`snmpwalk -v 2c -c public 127.0.0.1 ULTRAMONKEY-L7-MIB::ultramonkey-l7`

EXPECT="ULTRAMONKEY-L7-MIB::ultramonkey-l7 = No Such Object available on this agent at this OID"

if [ "${RET}" != "${EXPECT}" ]
then
        echo "snmpwalk -v 2c -c public 127.0.0.1 ULTRAMONKEY-L7-MIB::ultramonkey-l7 test failed"
        exit 1
fi

exit 0
