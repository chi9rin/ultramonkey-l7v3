#!/bin/bash

. ${SET_DEFAULT_CONF}


RET=`$L7VSADM -A -t localhost:------ 2>&1 | grep "PARSE ERROR :"` 
EXPECT="PARSE ERROR : Argument argc is illegal for -A command."
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -A -t localhost:------"
        exit 1
fi

exit 0

