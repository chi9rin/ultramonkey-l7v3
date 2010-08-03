#!/bin/bash

# set config file
. ${SET_DEFAULT_CONF}

cp -f materials/parameter-69-l7vs.cf ${L7VSD_CONF_DIR}/l7vs.cf
mv -f ${L7VSD_CONF_DIR}/sslproxy/root.pem ${L7VSD_CONF_DIR}/sslproxy/Temp_root.pem
cp -f materials/parameter-69-sslproxy.target.cf ${L7VSD_CONF_DIR}/sslproxy/sslproxy.target.cf

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
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:44431 -m sessionless -z ${L7VSD_CONF_DIR}/sslproxy/sslproxy.target.cf"
        exit 1
fi

RET=`cat ${L7VS_LOG_DIR}/l7vsd.log | grep "\[DEBUG\] l7vsd_virtualservice VSD10700105 out_function :" | grep "ca_file = Temp_root.pem"`
if [ -z "${RET}" ]
then
        echo "Test failed: cat ${L7VS_LOG_DIR}/l7vsd.log"
        exit 1
fi

exit 0

