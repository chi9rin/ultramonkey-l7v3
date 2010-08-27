#!/bin/bash
. ${SET_DEFAULT_CONF}

RET=`ps -ef | grep "l7directord start"| grep -v grep`
if [ -n "$RET" ]
then
        echo "Test failed: ps -ef | grep l7directord start | grep -v grep"
        exit 1
fi

#Start l7directord
RET=`$INIT_L7DIRECTORD try-restart 2>&1 | grep "l7directord process is not running."`
EXPECT="Try restarting l7directord ...  l7directord process is not running."
if [ "$RET" != "$EXPECT" ]
then
        echo "Test failed: $INIT_L7DIRECTORD try-restart"
        exit 1
fi

exit 0

