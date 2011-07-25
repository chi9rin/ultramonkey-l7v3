#!/bin/bash

. ${SET_DEFAULT_CONF}

#Add Service
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi
usleep 100000

RET=`$L7VSADM -A -t localhost:40001 -m sslid -z ${L7VSD_CONF_DIR}/sslproxy/sslproxy.target.cf 2>&1 | grep "PARSE ERROR: You cannot set \"-z\" option with \"sslid\" protocol module."`
EXPECT="PARSE ERROR: You cannot set \"-z\" option with \"sslid\" protocol module."
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -A -t localhost:40001 -m sslid -z ${L7VSD_CONF_DIR}/sslproxy/sslproxy.target.cf"
        exit 1
fi

exit 0

