#!/bin/bash
. ${SET_DEFAULT_CONF}

RET=`$L7VSD -h`
EXPECT="Usage: l7vsd [-h]
   -h    --help         print this help messages and exit"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSD -h"
        exit 1
fi

exit 0

