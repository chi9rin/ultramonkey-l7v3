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

#check L7VSD exist
RET=`pgrep l7vsd`
if [ -z "${RET}" ]
then
        echo "Test failed: pgrep l7vsd"
        exit 1
fi

echo $RET
#Start L7VSD Service again
$INIT_L7VSD start
if [ $? -eq 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi
usleep 100000


#check L7VSD start again
RET=`pgrep l7vsd | wc -l`

EXPECT="1"

if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: pgrep l7vsd | wc -l"
        exit 1
fi

exit 0
