#!/bin/bash

# set config file
. ${SET_DEFAULT_CONF}

#Start L7VSD Service
$INIT_L7VSD start
if [ $? -ne 0 ]
then
        echo "Test failed: $INIT_L7VSD start"
        exit 1
fi
usleep 100000

kill -9 `pidof l7vsd`
usleep 100000

if [ $? -ne 0 ]
then
        echo "Test failed: kill -9 `pidof l7vsd`"
        exit 1
fi
echo 1
#Check L7VSD not exist
RET=`pgrep l7vsd`

if [ -n "${RET}" ]
then
        echo "Test failed: pgrep l7vsd"
        exit 1
fi


#Start L7VSD Service
$INIT_L7VSD start
if [ $? -ne 0 ]
then
        echo "Test failed: $INIT_L7VSD start"
        exit 1
fi
usleep 100000

#check L7VSD exist
RET1=`pgrep l7vsd`
if [ -z "${RET1}" ]
then
        echo "Test failed: pgrep l7vsd"
        exit 1
fi

exit 0
