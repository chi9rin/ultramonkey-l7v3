#!/bin/bash
. ${SET_DEFAULT_CONF}

RET=`ps -ef | grep "l7directord start"| grep -v grep`
if [ -n "$RET" ]
then
        echo "Test failed: ps -ef | grep l7directord start | grep -v grep"
        exit 1
fi

#Start l7directord
RET=`$INIT_L7DIRECTORD restart 2>&1 | grep "Stopping l7directord ... l7directord process is not running."`
if [ $? -ne 0 ]
then
        echo "Test failed: $INIT_L7DIRECTORD restart"
        exit 1
fi
usleep 100000

EXPECT="Stopping l7directord ... l7directord process is not running."
if [ "$RET" != "$EXPECT" ]
then
        echo "Test failed: $L7DIRECTORD restart"
        exit 1
fi

RET=`ps -ef | grep "l7directord start"| grep -v grep`
if [ -z "$RET" ]
then
        echo "Test failed: ps -ef | grep l7directord start | grep -v grep"
        exit 1
fi

exit 0

