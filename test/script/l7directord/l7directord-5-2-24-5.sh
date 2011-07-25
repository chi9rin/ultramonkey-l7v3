#!/bin/bash
. ${SET_DEFAULT_CONF}

\cp -p ./materials/l7directord-5-2-24-no_exec_callback.sh ${L7DIRECTORD_CONF_DIR}/no_exec_callback.sh
sed "s|XXX|${L7DIRECTORD_CONF_DIR}/no_exec_callback.sh|g" ./materials/l7directord-5-2-24-recover-l7directord.cf > ${L7DIRECTORD_CONF_DIR}/l7directord.cf

#Start l7vsd
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi
usleep 100000

#Start l7directord
RET=`$L7DIRECTORD start 2>&1`
if [ $? -eq 0 ]
then
        echo "Test failed: $L7DIRECTORD start"
        exit 1
fi
EXPECT="Configuration error. Reading file \`/etc/ha.d/conf/l7directord.cf' at line 46: File not found or cannot execute \`    realrecovercallback   = /etc/ha.d/conf/no_exec_callback.sh'."
if [ "$RET" != "$EXPECT" ]
then
        echo "Test failed: $L7DIRECTORD start"
        exit 1
fi

exit 0

