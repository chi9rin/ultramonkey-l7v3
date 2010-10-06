#!/bin/bash

# set config file
. ${SET_DEFAULT_CONF}
chmod 000 ${L7VSD_CONF_DIR}/sslproxy/sslproxy.target.cf

#Add Service
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi

$L7VSADM -A -t 127.0.0.1:44431 -m sessionless -z ${L7VSD_CONF_DIR}/sslproxy/sslproxy.target.cf
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:44431 -m sessionless -z ${L7VSD_CONF_DIR}/sslproxy/Temp_sslproxy.target.cf"
        exit 1
fi

RET=`$L7VSADM -V | awk '/SSL Config File/ { print $4 }' `
EXPECT="/etc/l7vs/sslproxy/sslproxy.target.cf"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -V"
        exit 1
fi

exit 0

