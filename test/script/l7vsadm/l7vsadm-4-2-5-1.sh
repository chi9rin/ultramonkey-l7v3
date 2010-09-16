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

RET=`$L7VSADM -D -t -m ip 2>&1 | grep "PARSE ERROR: Argument argc is illegal for -D command."`
EXPECT="PARSE ERROR: Argument argc is illegal for -D command."
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -D -t -m ip"
        exit 1
fi

exit 0

