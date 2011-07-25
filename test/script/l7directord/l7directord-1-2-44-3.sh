#!/bin/bash
. ${SET_DEFAULT_CONF}
\cp ./materials/l7directord-1-2-44-l7directord.cf ${L7DIRECTORD_CONF_DIR}/l7directord.cf

#test l7directord config file
RET=`$INIT_L7DIRECTORD configtest`
if [ $? -ne 0 ]
then
        echo "Test failed: $INIT_L7DIRECTORD configtest"
        exit 1
fi

echo "$RET" | grep "Syntax OK" > /dev/null 2>&1
if [ $? -ne 0 ]
then
        echo "Test failed: $INIT_L7DIRECTORD configtest"
        exit 1
fi

exit 0

