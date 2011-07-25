#!/bin/bash
. ${SET_DEFAULT_CONF}
\cp ./materials/l7directord-5-2-12-1-l7directord.cf ${L7DIRECTORD_CONF_DIR}/l7directord.cf

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
if [ $? -eq 0 ]
then
        echo "Test failed: $L7DIRECTORD start"
        exit 1
fi
EXPECT="Configuration error. Reading file \`/etc/ha.d/conf/l7directord.cf' at line 33: Invalid value (set natural number) \`    checktimeout     = 0'."
if [ "$RET" != "$EXPECT" ]
then
        echo "Test failed: $L7DIRECTORD start"
        exit 1
fi

exit 0

