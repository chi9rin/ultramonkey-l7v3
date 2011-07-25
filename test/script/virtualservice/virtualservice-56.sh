#!/bin/bash

# set config file
cp materials/virtualservice-56-l7vs.cf ${L7VSD_CONF_DIR}/l7vs.cf

#Add Service
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi
usleep 100000

$L7VSADM -A -t localhost:40001 -m sessionless 
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t localhost:40001 -m sessionless"
        exit 1
fi


RET=`ps -eLf | grep l7vsd | grep -v grep wc -l`

if [ ${RET} -ne 515 ]
then
        echo "Test failed: $L7VSADM -V"
        exit 1
fi

exit 0

