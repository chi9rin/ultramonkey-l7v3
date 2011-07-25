#!/bin/bash
. ${SET_DEFAULT_CONF}

#Start l7directord
$L7DIRECTORD /dev/null/cf stop
if [ $? -eq 0 ]
then
        echo "Test failed: $L7DIRECTORD /dev/null/cf stop"
        exit 1
fi

RET=`$L7DIRECTORD /dev/null/cf stop 2>&1 | egrep "Config file ./dev/null/cf' is not found."`
if [ -z "$RET" ]
then
        echo "Test failed: $L7DIRECTORD /dev/null/cf stop"
        exit 1
fi

exit 0

