#!/bin/bash
cat /dev/null > ${L7DIRECTORD_CONF_DIR}/l7directord.cf



RET=`$L7DIRECTORD -d 2> /dev/null | grep "DEBUG"`
if [ -n "$RET" ]
then
        echo "Test failed: $L7DIRECTORD -d 2> /dev/null | grep DEBUG"
        exit 1
fi

RET=`$L7DIRECTORD -d 2>&1 | grep "DEBUG"`
if [ -z "$RET" ]
then
        echo "Test failed: $L7DIRECTORD -d 2>&1 | grep DEBUG"
        exit 1
fi

exit 0

