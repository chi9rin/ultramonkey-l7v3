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

# rename config file
mv ${L7VSD_CONF_DIR}/l7vs.cf ${L7VSD_CONF_DIR}/Temp_l7vs.cf
EXEC_DATE=`LANG=C date +'%Y/%m/%d %H:%M:%S'`
$L7VSADM -A -t 127.0.0.1:40001 -m sessionless 
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m sessionless"
        exit 1
fi

RET=`cat /var/log/messages | grep "l7vsadm: ${EXEC_DATE}" | grep "\[FATAL\] l7vsadm_parameter ADM53300001 Parameter initialize failure"`
if [ -z "${RET}" ]
then
        echo "Test failed: cat /var/log/messages"
        exit 1
fi

sleep 1

EXEC_DATE=`LANG=C date +'%Y/%m/%d %H:%M:%S'`
RET=`$L7VSADM | grep "TCP"` 
EXPECT="TCP localhost:40001 sessionless rr"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM"
        exit 1
fi

RET=`cat /var/log/messages | grep "l7vsadm: ${EXEC_DATE}" | grep "\[FATAL\] l7vsadm_parameter ADM53300001 Parameter initialize failure"`
if [ -z "${RET}" ]
then
        echo "Test failed: cat /var/log/messages"
        exit 1
fi

exit 0

