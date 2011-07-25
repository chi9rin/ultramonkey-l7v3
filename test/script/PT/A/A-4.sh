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

#Check L7VSD Service
RET=`pgrep l7vsd`
if [ -z "${RET}" ]
then
        echo "Test failed: pgrep l7vsd"
        exit 1
fi

#Check PID file
RET1=`cat /var/run/l7vsd.pid | grep "$RET"`
if [ -z "${RET1}" ]
then
        echo "Test failed: cat /var/run/l7vsd.pid | grep \"$RET1\""
        exit 1
fi

#Stop L7vsd
$INIT_L7VSD stop
if [ $? -ne 0 ]
then
        echo "Test failed: $INIT_L7VSD stop"
        exit 1
fi
sleep 1

#check L7VSD Stoped
RET2=`pgrep l7vsd`
if [ -n "${RET2}" ]
then
        echo "Test failed: pgrep l7vsd"
        exit 1
fi

#Check PID file delete
if [ -e /var/run/l7vsd.pid ]
then
        echo "Test failed:  /var/run/l7vsd.pid exist."
        exit 1
fi

exit 0
