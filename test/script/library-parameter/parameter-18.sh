#!/bin/bash

# set config file
cp materials/parameter-18-l7vs.cf ${L7VSD_CONF_DIR}/l7vs.cf

#Add Service
EXEC_DATE=`LANG=C date +'%Y/%m/%d %H:%M:%S'`
$L7VSD
if [ $? -eq 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi

RET=`cat /var/log/messages | grep "l7vsd: ${EXEC_DATE}" | grep "\[FATAL\] l7vsd_parameter VSD51600001 Parameter initialize failure"`
if [ -z "${RET}" ]
then
        echo "Test failed: cat /var/log/messages"
        exit 1
fi

RET=`ps -efl | grep l7vsd | grep -v grep`
if [ -n "${RET}" ]
then
        echo "Test failed: ps -efl | grep l7vsd | grep -v grep"
        exit 1
fi

exit 0

