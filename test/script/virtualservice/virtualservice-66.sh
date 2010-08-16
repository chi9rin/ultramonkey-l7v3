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

$L7VSADM -A -t 127.0.0.1:40001 -m sessionless -z ${L7VSD_CONF_DIR}/sslproxy/sslproxy.target.cf
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m sessionless -z ${L7VSD_CONF_DIR}/sslproxy/sslproxy.target.cf"
        exit 1
fi

$L7VSADM -A -t 127.0.0.1:40002 -m ip -z ${L7VSD_CONF_DIR}/sslproxy/sslproxy.target.cf
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40002 -m ip -z ${L7VSD_CONF_DIR}/sslproxy/sslproxy.target.cf"
        exit 1
fi

RET=`$L7VSADM`
EXPECT="Layer-7 Virtual Server version 3.0.0-1
Prot LocalAddress:Port ProtoMod Scheduler
  -> RemoteAddress:Port           Forward Weight ActiveConn InactConn
TCP localhost:40001 sessionless rr
TCP localhost:40002 ip rr"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM"
        exit 1
fi

$L7VSADM -E -t 127.0.0.1:40001 -m sessionless -s wrr
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -E -t 127.0.0.1:40001 -m sessionless -s wrr"
        exit 1
fi

$L7VSADM -E -t 127.0.0.1:40002 -m ip -s lc
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -E -t 127.0.0.1:40002 -m ip -s lc"
        exit 1
fi

RET=`$L7VSADM`
EXPECT="Layer-7 Virtual Server version 3.0.0-1
Prot LocalAddress:Port ProtoMod Scheduler
  -> RemoteAddress:Port           Forward Weight ActiveConn InactConn
TCP localhost:40001 sessionless wrr
TCP localhost:40002 ip lc"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM"
        exit 1
fi

exit 0

