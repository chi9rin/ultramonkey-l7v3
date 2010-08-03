#!/bin/bash

. ${SET_DEFAULT_CONF}


RET=`$L7VSADM 2>&1` 
EXPECT="COMMON ERROR : connect() failed: No such file or directory."
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM"
        exit 1
fi

exit 0

