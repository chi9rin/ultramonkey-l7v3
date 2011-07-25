#!/bin/bash
. ${SET_DEFAULT_CONF}

\cp -p ./materials/l7directord-5-1-2-callback.sh ${L7DIRECTORD_CONF_DIR}/callback.sh
sed "s|XXX|${L7DIRECTORD_CONF_DIR}/callback.sh|g" ./materials/l7directord-5-1-2-l7directord.cf > ${L7DIRECTORD_CONF_DIR}/l7directord.cf

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
sleep 1
touch ${L7DIRECTORD_CONF_DIR}/l7directord.cf
sleep 10

RET=`ls ${L7VS_LOG_DIR}/callbacktest`
if [ -n "$RET" ]
then
        echo "Test failed: ls ${L7VS_LOG_DIR}/callbacktest"
        exit 1
fi

exit 0

