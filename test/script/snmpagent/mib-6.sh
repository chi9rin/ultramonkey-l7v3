#!/bin/bash

# set config file
. ${SET_DEFAULT_CONF}

\cp -f materials/mib-6-l7vs.cf ${L7VSD_CONF_DIR}/l7vs.cf


#Start L7VSD Service
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi
usleep 100000


#Add Service
$L7VSADM -A -t 127.0.0.1:40001 -m sessionless
if [ $? -ne 0 ]
then
         echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m sessionless"
         exit 1
fi

snmpget -v 2c -c public 127.0.0.1 ULTRAMONKEY-L7-MIB::l7vsReplicationMode.0
#Check Return Value
RET=`snmpget -v 2c -c public 127.0.0.1 ULTRAMONKEY-L7-MIB::l7vsReplicationMode.0`
echo "$RET"
EXPECT="ULTRAMONKEY-L7-MIB::l7vsReplicationMode.0 = INTEGER: single(1)"

if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: snmpget -v 2c -c public 127.0.0.1 ULTRAMONKEY-L7-MIB::l7vsReplicationMode.0"
        exit 1
fi

exit 0
