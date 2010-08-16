#!/bin/bash

# set config file
. ${SET_DEFAULT_CONF}
\cp -f materials/virtualservice-77-1-l7vs.cf ${L7VSD_CONF_DIR}/l7vs.cf
\cp -f materials/virtualservice-77-1-sslproxy.target.cf ${L7VSD_CONF_DIR}/sslproxy/sslproxy.target.cf
touch ${L7VSD_CONF_DIR}/sslproxy/TESTroot.pem

#Add Service
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi
usleep 100000

$L7VSADM -A -t 127.0.0.1:40001 -m sessionless -z ${L7VSD_CONF_DIR}/sslproxy/sslproxy.target.cf
if [ $? -eq 0 ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m sessionless -z ${L7VSD_CONF_DIR}/sslproxy/sslproxy.target.cf"
        exit 1
fi

RET=`cat ${L7VS_LOG_DIR}/l7vsd.log | grep "\[ERROR\] l7vsd_virtualservice VSD40700999 Set root CA file error : Invalid argument."`
if [ -z "${RET}" ]
then
        echo "Test failed: cat ${L7VS_LOG_DIR}/l7vsd.log"
        exit 1
fi

exit 0

