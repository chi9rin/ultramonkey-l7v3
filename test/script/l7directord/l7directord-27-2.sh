#!/bin/bash
. ${SET_DEFAULT_CONF}
\cp ./materials/l7directord-27-l7directord.cf ${L7DIRECTORD_CONF_DIR}/l7directord.cf

#test l7directord config file
RET=`$L7DIRECTORD -t`
if [ $? -ne 0 ]
then
        echo "Test failed: $L7DIRECTORD -t"
        exit 1
fi
usleep 100000

EXPECT="Syntax OK"
if [ "$RET" != "$EXPECT" ]
then
        echo "Test failed: $L7DIRECTORD -t"
        exit 1
fi

exit 0

