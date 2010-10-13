#!/bin/bash
. ${SET_DEFAULT_CONF}
\cp ./materials/l7directord-1-2-48-l7directord.cf ${L7DIRECTORD_CONF_DIR}/specified.cf

#test l7directord config file
RET=`$L7DIRECTORD -t ${L7DIRECTORD_CONF_DIR}/specified.cf`
if [ $? -ne 0 ]
then
        echo "Test failed: $L7DIRECTORD -t ${L7DIRECTORD_CONF_DIR}/specified.cf"
        exit 1
fi

echo "$RET" | grep "Syntax OK" > /dev/null 2>&1
if [ $? -ne 0 ]
then
        echo "Test failed: $L7DIRECTORD -t ${L7DIRECTORD_CONF_DIR}/specified.cf"
        exit 1
fi

exit 0

