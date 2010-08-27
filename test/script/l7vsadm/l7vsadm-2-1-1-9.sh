#!/bin/bash

# set config file
cp materials/l7vsadm-2-1-1-9-l7vs.cf ${L7VSD_CONF_DIR}/l7vs.cf

#Add Service
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi
usleep 100000

$L7VSADM -l
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM"
        exit 1
fi

RET=`cat ${L7VS_LOG_DIR}/l7vsadm.log` #| grep "\[WARN\] l7vsadm_common ADM33100009 Command wait value too long. Use default value."`
echo "$RET"
exit 1
if [ -z "${RET}" ]
then
        echo "Test failed: cat ${L7VS_LOG_DIR}/l7vsadm.log"
        exit 1
fi

exit 0

