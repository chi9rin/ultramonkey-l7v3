#!/bin/bash
. ${SET_DEFAULT_CONF}
\cp ./materials/l7directord-131-6-l7directord.cf ${L7DIRECTORD_CONF_DIR}/l7directord.cf

#Start l7vsd
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi
usleep 100000

#Start l7directord
RET=`$L7DIRECTORD start 2>&1`
if [ $? -ne 0 ]
then
        echo "Test failed: $L7DIRECTORD start"
        exit 1
fi

if [ -n "$RET" ]
then
        echo "Test failed: $L7DIRECTORD start"
        exit 1
fi

exit 0

