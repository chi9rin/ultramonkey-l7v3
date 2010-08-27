#!/bin/bash
. ${SET_DEFAULT_CONF}


RET=`ps -ef | grep "l7directord start"| grep -v grep`
if [ -n "$RET" ]
then
        echo "Test failed: ps -ef | grep l7directord start | grep -v grep"
        exit 1
fi

RET=`$L7DIRECTORD stop 2>&1`
if [ $? -eq 0 ]
then
        echo "Test failed: $L7DIRECTORD stop"
        exit 1
fi
usleep 100000

EXPECT="l7directord process is not running."
if [ "$RET" != "$EXPECT" ]
then
        echo "Test failed: $L7DIRECTORD stop"
        exit 1
fi

RET=`ps -ef | grep "l7directord start"| grep -v grep`
if [ -n "$RET" ]
then
        echo "Test failed: ps -ef | grep l7directord start | grep -v grep"
        exit 1
fi

exit 0


