#!/bin/bash

# set config file
. ${SET_DEFAULT_CONF}


#stop snmpd
${INIT_SNMPD} stop
#check service snmpd is running
RET=`pgrep snmpd`
if [ -n "${RET}" ]
then
    echo "Test failed: ${INIT_SNMPD} stop"
    exit 1
fi


#Start L7VAD Service
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi

usleep 100000

#Set Trap Log Level
$L7VSADM -L -c all -l debug
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -L -c all -l debug"
        exit 1
fi

$L7VSADM -S -f 1 -t  1 -l debug
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -S -f 1 -t  1 -l debug"
        exit 1
fi


exit 0
