#!/bin/bash
. ${SET_DEFAULT_CONF}
\cp ./materials/l7directord-1-2-48-l7directord.cf ${L7DIRECTORD_CONF_DIR}/specified.cf

#test l7directord config file
RET=`$L7DIRECTORD ${L7DIRECTORD_CONF_DIR}/specified.cf configtest`
if [ $? -ne 0 ]
then
        echo "Test failed: $L7DIRECTORD ${L7DIRECTORD_CONF_DIR}/specified.cf configtest"
        exit 1
fi

EXPECT="Syntax OK"
if [ "$RET" != "$EXPECT" ]
then
        echo "Test failed: $L7DIRECTORD ${L7DIRECTORD_CONF_DIR}/specified.cf configtest"
        exit 1
fi

exit 0

