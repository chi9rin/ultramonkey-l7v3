#!/bin/bash
. ${SET_DEFAULT_CONF}

#Start l7directord
$L7DIRECTORD -d start &
usleep 100000

RET=`ps -eo ppid,args | grep $(which l7directord)| grep -v grep | awk '{print $1}'`
if [ $RET -eq 1 ]
then
        echo "Test failed: ps -ef | grep l7directord start | grep -v grep"
        exit 1
fi

RET=`$INIT_L7DIRECTORD start 2>&1 | grep "Starting l7directord ...  Other l7directord process is running."`
if [ -z "$RET" ]
then
        echo "Test failed: $INIT_L7DIRECTORD start"
        exit 1
fi

exit 0

