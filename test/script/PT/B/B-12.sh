#!/bin/bash

# set config file
. ${SET_DEFAULT_CONF}

\cp -f materials/B-12-l7vs.cf ${L7VSD_CONF_DIR}/l7vs.cf
#Start L7VSD Service
$INIT_L7VSD start
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi
usleep 100000

#Check The Status Of L7vsd
RET=`$L7VSADM -V | grep "l7vsd_snmpagent                  info"`

if [ -n "${RET}" ]
then
        echo "Test failed: $L7VSADM -V"
        exit 1
fi


#Change The Log Level Of L7vsd_snmpagent To Error
$L7VSADM -L -c l7vsd_snmpagent -l error
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -L -c l7vsd_snmpagent -l error"
        exit 1
fi

#Check The Status Of L7vsd
RET=`$L7VSADM -V | grep "l7vsd_snmpagent                  error"`

if [ -n "${RET}" ]
then
        echo "Test failed: $L7VSADM -V"
        exit 1
fi

exit 0
