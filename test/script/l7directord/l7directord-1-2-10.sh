#!/bin/bash
if [ -e  ${L7DIRECTORD_CONF_DIR}/l7directord.cf ]
then
	rm -f  ${L7DIRECTORD_CONF_DIR}/l7directord.cf
fi

if [ -e  /etc/ha.d/l7directord.cf ]
then
	rm -f  /etc/ha.d/l7directord.cf
fi

cd $TMP_DIR
if [ -e ./l7directord.cf ]
then
	rm -f ./l7directord.cf
fi

#Start l7directord
RET=`$L7DIRECTORD start 2>&1`
if [ $? -eq 0 ]
then
        echo "Test failed: $L7DIRECTORD start"
        exit 1
fi

EXPECT="\`l7directord.cf' is not found at default search paths."
if [ "$RET" != "$EXPECT" ]
then
        echo "Test failed: $L7DIRECTORD start"
        exit 1
fi

exit 0

