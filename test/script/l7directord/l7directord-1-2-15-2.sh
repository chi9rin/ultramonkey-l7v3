#!/bin/bash
. ${SET_DEFAULT_CONF}

RET=`ps -ef | grep "l7directord start"| grep -v grep`
if [ -n "$RET" ]
then
        echo "Test failed: ps -ef | grep l7directord start | grep -v grep"
        exit 1
fi

RET=`$INIT_L7DIRECTORD stop 2>&1 | grep "Stopping l7directord ... l7directord process is not running."`
usleep 100000
if [ -z "$RET" ]
then
        echo "Test failed: $INIT_L7DIRECTORD stop"
        exit 1
fi

RET=`ps -ef | grep "l7directord start"| grep -v grep`
if [ -n "$RET" ]
then
        echo "Test failed: ps -ef | grep l7directord start | grep -v grep"
        exit 1
fi

exit 0


