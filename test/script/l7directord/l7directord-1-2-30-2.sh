#!/bin/bash
. ${SET_DEFAULT_CONF}

#Start l7directord
$INIT_L7DIRECTORD start
if [ $? -ne 0 ]
then
        echo "Test failed: $INIT_L7DIRECTORD start"
        exit 1
fi
usleep 100000


RET=`ps -ef | grep "l7directord start"| grep -v grep`
if [ -z "$RET" ]
then
        echo "Test failed: ps -ef | grep l7directord start | grep -v grep"
        exit 1
fi

#Restart l7directord
$INIT_L7DIRECTORD try-restart
if [ $? -ne 0 ]
then
        echo "Test failed: $INIT_L7DIRECTORD try-restart"
        exit 1
fi
usleep 100000


RET=`ps -ef | grep "l7directord try-restart"| grep -v grep`
if [ -z "$RET" ]
then
        echo "Test failed: ps -ef | grep l7directord try-restart | grep -v grep"
        exit 1
fi

exit 0

