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

kill -SIGUSR1 `pgrep l7vsd`
if [ $? -ne 0 ]
then
        echo "Test failed: kill -SIGUSER1"
        exit 1
fi
usleep 100000

RET=`pgrep l7vsd`
if [ $? -ne 0 ]
then
        echo "Test failed: pgrep l7vsd"
        exit 1
fi

if [ -z "${RET}" ]
then
        echo "Test failed: pgrep l7vsd"
        exit 1
fi

exit 0

