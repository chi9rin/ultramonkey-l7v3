#!/bin/bash
. ${SET_DEFAULT_CONF}

#test l7directord config file
RET=`$L7DIRECTORD help`
if [ $? -ne 0 ]
then
        echo "Test failed: $L7DIRECTORD help"
        exit 1
fi
EXPECT=`cat ./materials/l7directord-help.txt`
if [ "$RET" != "$EXPECT" ]
then
        echo "Test failed: $L7DIRECTORD help"
        exit 1
fi

exit 0

