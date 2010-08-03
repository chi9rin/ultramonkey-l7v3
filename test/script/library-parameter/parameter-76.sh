#!/bin/bash

# set config file
. ${SET_DEFAULT_CONF}
mv ${L7VSD_CONF_DIR}/sslproxy/sslproxy.target.cf ${L7VSD_CONF_DIR}/sslproxy/Temp_sslproxy.target.cf
touch ${L7VSD_CONF_DIR}/sslproxy/sslproxy.target.cf

#Add Service
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi

RET=`$L7VSADM -A -t 127.0.0.1:44431 -m sessionless -z ${L7VSD_CONF_DIR}/sslproxy/sslproxy.target.cf 2>&1`
EXPECT="add vs error : get ssl parameter failed"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:44431 -m sessionless -z ${L7VSD_CONF_DIR}/sslproxy/sslproxy.target.cf"
        exit 1
fi

RET=`$L7VSADM -V | grep "TCP localhost:44431"`
if [ -n "${RET}" ]
then
        echo "Test failed: $L7VSADM -V"
        exit 1
fi

RET=`cat ${L7VS_LOG_DIR}/l7vsd.log | grep "\[ERROR\] l7vsd_virtualservice VSD40700011 get ssl parameter failed"`
if [ -z "${RET}" ]
then
        echo "Test failed: cat ${L7VS_LOG_DIR}/l7vsd.log"
        exit 1
fi

exit 0

