#!/bin/bash
. ${SET_DEFAULT_CONF}

#Start l7directord
$L7DIRECTORD start
if [ $? -ne 0 ]
then
        echo "Test failed: $L7DIRECTORD start"
        exit 1
fi
usleep 100000

RET=`ps -eo ppid,args | grep $(which l7directord)| grep -v grep | awk '{print $1}'`
if [ $RET -ne 1 ]
then
        echo "Test failed: ps -ef | grep l7directord start | grep -v grep"
        exit 1
fi

#Restart l7directord
$L7DIRECTORD -d restart &
if [ $? -ne 0 ]
then
        echo "Test failed: $L7DIRECTORD -d restart"
        exit 1
fi
usleep 100000

RET=`ps -eo ppid,args | grep $(which l7directord)| grep -v grep | awk '{print $1}'`
if [ $RET -eq 1 ]
then
        echo "Test failed: ps -ef | grep l7directord start | grep -v grep"
        exit 1
fi

exit 0

