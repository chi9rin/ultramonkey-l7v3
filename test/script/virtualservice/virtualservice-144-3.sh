#!/bin/bash

# set config file
. ${SET_DEFAULT_CONF}
\cp -f materials/virtualservice-144-3-l7vs.cf ${L7VSD_CONF_DIR}/l7vs.cf
\cp -f materials/virtualservice-144-3-sslproxy.target.cf ${L7VSD_CONF_DIR}/sslproxy/sslproxy.target.cf

#Add Service
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi
usleep 100000

$L7VSADM -A -t 127.0.0.1:40001 -m sessionless -z ${L7VSD_CONF_DIR}/sslproxy/sslproxy.target.cf
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m sessionless -z ${L7VSD_CONF_DIR}/sslproxy/sslproxy.target.cf"
        exit 1
fi

RET=`cat ${L7VS_LOG_DIR}/l7vsd.log | grep "\[WARN\] l7vsd_virtualservice VSD30700021 session_cache_timeout parameter not found. Use default value."`
if [ -z "${RET}" ]
then
        echo "Test failed: cat ${L7VS_LOG_DIR}/l7vsd.log"
        exit 1
fi

exit 0

