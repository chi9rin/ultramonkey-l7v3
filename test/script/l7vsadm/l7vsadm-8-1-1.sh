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

$L7VSADM -L -c all -l fatal
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -L -c all -l fatal"
        exit 1
fi

RET=`$L7VSADM -V 2>&1 | grep -e "^l7vsd"`
EXPECT="l7vsd_network                  fatal
l7vsd_network_qos              fatal
l7vsd_network_bandwidth        fatal
l7vsd_network_num_connection   fatal
l7vsd_network_access           fatal
l7vsd_mainthread               fatal
l7vsd_virtualservice           fatal
l7vsd_virtualservice_thread    fatal
l7vsd_session                  fatal
l7vsd_session_thread           fatal
l7vsd_realserver               fatal
l7vsd_sorryserver              fatal
l7vsd_module                   fatal
l7vsd_replication              fatal
l7vsd_replication_sendthread   fatal
l7vsd_parameter                fatal
l7vsd_logger                   fatal
l7vsd_command                  fatal
l7vsd_start_stop               fatal
l7vsd_system                   fatal
l7vsd_system_memory            fatal
l7vsd_system_endpoint          fatal
l7vsd_system_signal            fatal
l7vsd_system_environment       fatal
l7vsd_snmpagent                fatal
l7vsd_protocol                 fatal
l7vsd_schedule                 fatal"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -V | grep l7vs"
        exit 1
fi

$L7VSADM -P -r logger
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM --parameter --reload logger"
        exit 1
fi

RET=`$L7VSADM -V 2>&1 | grep -e "^l7vsd"`
EXPECT="l7vsd_network                  warn
l7vsd_network_qos              warn
l7vsd_network_bandwidth        warn
l7vsd_network_num_connection   warn
l7vsd_network_access           warn
l7vsd_mainthread               warn
l7vsd_virtualservice           warn
l7vsd_virtualservice_thread    warn
l7vsd_session                  warn
l7vsd_session_thread           warn
l7vsd_realserver               warn
l7vsd_sorryserver              warn
l7vsd_module                   warn
l7vsd_replication              warn
l7vsd_replication_sendthread   warn
l7vsd_parameter                warn
l7vsd_logger                   warn
l7vsd_command                  warn
l7vsd_start_stop               warn
l7vsd_system                   warn
l7vsd_system_memory            warn
l7vsd_system_endpoint          warn
l7vsd_system_signal            warn
l7vsd_system_environment       warn
l7vsd_snmpagent                info
l7vsd_protocol                 warn
l7vsd_schedule                 warn"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -V | grep l7vsd"
        exit 1
fi

exit 0

