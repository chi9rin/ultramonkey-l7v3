#!/bin/bash

. ${SET_DEFAULT_CONF}

# rename config file
chmod 000 ${L7VSD_CONF_DIR}/l7vs.cf

#Add Service
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi

RET=`ps -efl | grep l7vsd | grep -v grep`
if [ -z "${RET}" ]
then
        echo "Test failed: ps -efl | grep l7vsd | grep -v grep"
        exit 1
fi

exit 0

