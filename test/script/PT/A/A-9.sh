#!/bin/bash
. ${SET_DEFAULT_CONF}

#Start l7vsd
$INIT_L7VSD start
if [ $? -ne 0 ]
then
        echo "Test failed: $INIT_L7VSD start"
        exit 1
fi
usleep 100000

#Start l7directord
$INIT_L7DIRECTORD start
if [ $? -ne 0 ]
then
        echo "Test failed: $INIT_L7DIRECTORD start"
        exit 1
fi
usleep 100000

RET=`ps ax | grep l7directord | grep -v grep`
if [ -z "$RET" ]
then
        echo "Test failed: ps -ax | grep l7directord | grep -v grep"
        exit 1
fi


#Stop l7directord
$INIT_L7DIRECTORD stop
if [ $? -ne 0 ]
then
        echo "Test failed: $INIT_L7DIRECTORD stop"
        exit 1
fi
usleep 100000

RET=`ps ax | grep l7directord | grep -v grep`
if [ -n "$RET" ]
then
        echo "Test failed: ps -ax | grep l7directord | grep -v grep"
        exit 1
fi

exit 0

