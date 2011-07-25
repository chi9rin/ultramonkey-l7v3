#!/bin/bash

# set config file
cp materials/parameter-24-l7vs.cf ${L7VSD_CONF_DIR}/l7vs.cf

#Add Service
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi

RET=`ps -efl | grep l7vsd | grep -v grep`
if [ -z "${RET}" ]
then
        echo "Test failed: ps -efl | grep l7vsd | grep -v grep"
        exit 1
fi

RET=`ls -l /var/log/\ l7vs/l7vsd.log`
if [ -z "${RET}" ]
then
        echo "Test failed: ls -l /var/log/\ l7vs/l7vsd.log"
        exit 1
fi

rm -rf /var/log/\ l7vs

exit 0

