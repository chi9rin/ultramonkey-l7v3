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

RET=`$L7VSADM -R -s start -d 2>&1 | grep "PARSE ERROR : Argument argc is illegal for -R command."`
EXPECT="PARSE ERROR : Argument argc is illegal for -R command."
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -R -s start -d"
        exit 1
fi

exit 0

