#!/bin/bash

echo "virtual=127.0.0.1:8000" >  ${L7DIRECTORD_CONF_DIR}/l7directord.cf


#Start l7directord
RET=`$L7DIRECTORD stop 2>&1`
if [ $? -eq 0 ]
then
        echo "Test failed: $L7DIRECTORD stop"
        exit 1
fi

EXPECT="l7directord process is not running."
if [ "$RET" != "$EXPECT" ]
then
        echo "Test failed: $L7DIRECTORD stop"
        exit 1
fi

exit 0

