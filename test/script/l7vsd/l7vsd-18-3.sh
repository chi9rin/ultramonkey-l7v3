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

kill -SIGQUIT `pgrep l7vsd`
if [ $? -ne 0 ]
then
        echo "Test failed: kill -SIGQUIT"
        exit 1
fi
usleep 100000

RET=`pgrep l7vsd`
if [ $? -eq 0 ]
then
        echo "Test failed: pgrep l7vsd"
        exit 1
fi


EXPECT=""
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: pgrep l7vsd"
        exit 1
fi

exit 0

