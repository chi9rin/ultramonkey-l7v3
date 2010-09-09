#!/bin/bash
. ${SET_DEFAULT_CONF}


RET=`ps -ef | grep "l7directord start"| grep -v grep`
if [ -n "$RET" ]
then
        echo "Test failed: ps -ef | grep l7directord start | grep -v grep"
        exit 1
fi

RET=`$L7DIRECTORD status 2>&1 | egrep "l7directord process is not running."`
if [ -z "$RET" ]
then
        echo "Test failed: $L7DIRECTORD status"
        exit 1
fi

exit 0

