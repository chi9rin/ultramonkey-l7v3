#!/bin/bash

# set config file
cp materials/l7vsadm-1-1-3-l7vs.cf ${L7VSD_CONF_DIR}/l7vs.cf

#Add Service
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi
usleep 100000

flock -n -x $L7VSADM l7vsadm
TIME1=`date '+%s'`
$L7VSADM
TIME2=`date '+%s'`
 
if [ $TIME1 -ne $TIME2 ]
then
        echo "Test failed: flock -n -x $L7VSADM l7vsadm"
        exit 1
fi

exit 0

