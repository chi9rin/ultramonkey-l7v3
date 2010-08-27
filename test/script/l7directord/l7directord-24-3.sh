#!/bin/bash
. ${SET_DEFAULT_CONF}
cat /dev/null > ${L7DIRECTORD_CONF_DIR}/l7directord.cf

#test l7directord config file
RET=`$INIT_L7DIRECTORD configtest`
if [ $? -ne 0 ]
then
        echo "Test failed: $INIT_L7DIRECTORD configtest"
        exit 1
fi
usleep 100000

EXPECT="Syntax OK"
if [ "$RET" != "$EXPECT" ]
then
        echo "Test failed: $INIT_L7DIRECTORD configtest"
        exit 1
fi

exit 0

