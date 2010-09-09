#!/bin/bash
. ${SET_DEFAULT_CONF}

#Start l7directord
$L7DIRECTORD -d start &
usleep 100000


RET=`ps -ef | grep "l7directord -d start"| grep -v grep`
if [ -z "$RET" ]
then
        echo "Test failed: ps -ef | grep -d l7directord start | grep -v grep"
        exit 1
fi


$L7DIRECTORD start
if [ $? -eq 0 ]
then
        echo "Test failed: $L7DIRECTORD start"
        exit 1
fi

RET=`$L7DIRECTORD start 2>&1 | grep "Other l7directord process is running."`
if [ -z "$RET" ]
then
        echo "Test failed: $L7DIRECTORD start"
        exit 1
fi

exit 0

