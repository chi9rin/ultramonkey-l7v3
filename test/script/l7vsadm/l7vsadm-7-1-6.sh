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

RET=`$L7VSADM -V | grep -e "^l7vsd"`
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

$L7VSADM -L -c all -l error
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -L -c vs -l error"
        exit 1
fi

RET=`$L7VSADM -V | grep -e "^l7vsd"`
EXPECT="l7vsd_network                  error
l7vsd_network_qos              error
l7vsd_network_bandwidth        error
l7vsd_network_num_connection   error
l7vsd_network_access           error
l7vsd_mainthread               error
l7vsd_virtualservice           error
l7vsd_virtualservice_thread    error
l7vsd_session                  error
l7vsd_session_thread           error
l7vsd_realserver               error
l7vsd_sorryserver              error
l7vsd_module                   error
l7vsd_replication              error
l7vsd_replication_sendthread   error
l7vsd_parameter                error
l7vsd_logger                   error
l7vsd_command                  error
l7vsd_start_stop               error
l7vsd_system                   error
l7vsd_system_memory            error
l7vsd_system_endpoint          error
l7vsd_system_signal            error
l7vsd_system_environment       error
l7vsd_snmpagent                error
l7vsd_protocol                 error
l7vsd_schedule                 error"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -V | grep l7vsd"
        exit 1
fi

exit 0

