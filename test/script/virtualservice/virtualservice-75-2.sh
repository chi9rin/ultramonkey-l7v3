#!/bin/bash

# set config file
. ${SET_DEFAULT_CONF}
\cp -f materials/virtualservice-75-2-sslproxy.target.cf ${L7VSD_CONF_DIR}/sslproxy/sslproxy.target.cf

#Add Service
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi
usleep 100000

RET=`$L7VSADM -A -t 127.0.0.1:40001 -m sessionless -z ${L7VSD_CONF_DIR}/sslproxy/sslproxy.target.cf 2>&1`
EXPECT="add vs error : get ssl parameter failed"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m sessionless -z ${L7VSD_CONF_DIR}/sslproxy/sslproxy.target.cf"
        exit 1
fi

exit 0

