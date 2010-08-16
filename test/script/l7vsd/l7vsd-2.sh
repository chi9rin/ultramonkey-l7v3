#!/bin/bash
. ${SET_DEFAULT_CONF}

#Add Service
RET=`$L7VSD --help`
EXPECT="Usage: l7vsd [-h]
   -h    --help         print this help messages and exit"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSD --help"
        exit 1
fi

exit 0

