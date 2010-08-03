#!/bin/bash

# set config file
. ${SET_DEFAULT_CONF}

#Add Service
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi

RET=`$L7VSADM -A -t 127.0.0.1:44431 -m sessionless -z ${L7VSD_CONF_DIR}/sslproxy/Temp_sslproxy.target.cf 2>&1 | grep "PARSE ERROR : ssl config file cannot open."`
EXPECT="PARSE ERROR : ssl config file cannot open."
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:44431 -m sessionless -z ${L7VSD_CONF_DIR}/sslproxy/Temp_sslproxy.target.cf"
        exit 1
fi

exit 0

