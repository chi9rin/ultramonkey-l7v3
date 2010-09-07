#!/bin/bash

# set config file
cp materials/logger-5-l7vs.cf ${L7VSD_CONF_DIR}/l7vs.cf

#Add Service
$L7VSD
if [ $? -ne 0 ]
then
	echo "Test failed: $L7VSD"
        exit 1
fi
usleep 100000

RET=`cat ${L7VS_LOG_DIR}/l7vsadm.log | grep "\[ERROR\]" | wc -l`
if [ ${RET} -le 0 ]
then
        echo "Test failed: cat ${L7VS_LOG_DIR}/l7vsadm.log"
        exit 1
fi

exit 0

