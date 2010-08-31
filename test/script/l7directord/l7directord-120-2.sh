#!/bin/bash
. ${SET_DEFAULT_CONF}
sed "s|XXX|${L7VS_LOG_DIR}/l7directord-120-2.log|g" ./materials/l7directord-120-l7directord.cf > ${L7DIRECTORD_CONF_DIR}/l7directord.cf

#Start l7vsd
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi
usleep 100000

#Start l7directord
$L7DIRECTORD start
if [ $? -ne 0 ]
then
        echo "Test failed: $L7DIRECTORD start"
        exit 1
fi

RET=`ls ${L7VS_LOG_DIR}/l7directord-120-2.log`
if [ -z "$RET" ]
then
        echo "Test failed: ls ${L7VS_LOG_DIR}/l7directord-120-2.log"
        exit 1
fi

exit 0

