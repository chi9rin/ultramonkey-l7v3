#!/bin/bash

. ${SET_DEFAULT_CONF}

#Add Service
start_l7vsd || exit 1

$L7VSADM -L -c vs -l debug
$L7VSADM -A -t 127.0.0.1:40001 -m ip 
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t localhost:40001 -m ip"
        exit 1
fi

wget -O - -t 1 http://127.0.0.1:40001/ > /dev/null 2>&1
sleep 1

logfile=/var/log/l7vs/l7vsd.log
pool_result=`grep pool_session $logfile | awk '{print $8}' | sed 1d | uniq`
pool_expect="31
30
31"
if [ "$pool_result" != "$pool_expect" ]; then
	echo "Test failed: Mismatch pool session threads."
	exit 1
fi

active_result=`grep active_session $logfile | awk '{print $8}' | uniq`
active_expect="0
1
0"
if [ "$active_result" != "$active_expect" ]; then
	echo "Test failed: Mismatch active session threads."
	exit 1
fi
exit 0

