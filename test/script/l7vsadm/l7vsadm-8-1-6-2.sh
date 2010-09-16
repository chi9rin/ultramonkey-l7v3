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

RET=`$L7VSADM -P -r x 2>&1 | grep "PARSE ERROR: reload component not found(--reload): x"`
EXPECT="PARSE ERROR: reload component not found(--reload): x"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -P -r x"
        exit 1
fi

exit 0

