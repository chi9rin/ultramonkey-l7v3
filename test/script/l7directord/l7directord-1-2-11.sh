#!/bin/bash
. ${SET_DEFAULT_CONF}

#Start l7directord
$L7DIRECTORD /dev/null/cf start
if [ $? -eq 0 ]
then
        echo "Test failed: $L7DIRECTORD /dev/null/cf start"
        exit 1
fi

RET=`$L7DIRECTORD /dev/null/cf start 2>&1 | egrep "Config file ./dev/null/cf' is not found."`
if [ -z "$RET" ]
then
        echo "Test failed: $L7DIRECTORD /dev/null/cf start"
        exit 1
fi

exit 0

