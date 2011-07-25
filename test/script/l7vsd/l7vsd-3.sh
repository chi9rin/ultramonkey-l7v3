#!/bin/bash
. ${SET_DEFAULT_CONF}

#Add Service
RET=`$L7VSD -hoge 2>&1`
EXPECT="l7vsd: unknown option: -hoge
Usage: l7vsd [-h]
   -h    --help         print this help messages and exit"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSD -hoge"
        exit 1
fi

exit 0

