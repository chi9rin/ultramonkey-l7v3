#!/bin/bash

# set config file
. ${SET_DEFAULT_CONF}

#Start L7VSD Service
$INIT_L7VSD start
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi
usleep 100000

#Check Not Time Out
$L7VSADM
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM"
        exit 1
fi

exit 0
