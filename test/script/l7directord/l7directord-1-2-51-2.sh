#!/bin/bash
. ${SET_DEFAULT_CONF}

#test l7directord config file
$INIT_L7DIRECTORD > ${TMP_DIR}/l7directord-32-2 2>&1
if [ $? -eq 0 ]
then
        echo "Test failed: $INIT_L7DIRECTORD"
        exit 1
fi

RET=`diff ./materials/l7directord-usage.txt ${TMP_DIR}/l7directord-32-2`
if [ -n "$RET" ]
then
        echo "Test failed: $INIT_L7DIRECTORD"
        exit 1
fi

exit 0

