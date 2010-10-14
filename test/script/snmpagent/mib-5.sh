#!/bin/bash

# set config file
.${SET_DEFAULT_CONF}

\cp -f materials/mib-5-l7vs.cf ${L7VSD_CONF_DIR}/l7vs.cf


#Start L7VSD Service
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi
usleep 100000

#Check Return Value
RET=`snmpget -v 2c -c public 127.0.0.1 ULTRAMONKEY-L7-MIB::l7vsReplicationMode.0`
echo "${RET}"11
EXPECT="ULTRAMONKEY-L7-MIB::l7vsReplicationMode.0 = INTEGER: slave(3)"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: snmpget -v 2c -c public 127.0.0.1 ULTRAMONKEY-L7-MIB::l7vsReplicationMode.0"
        exit 1
fi

exit 0
