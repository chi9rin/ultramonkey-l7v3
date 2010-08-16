#!/bin/bash
. ${SET_DEFAULT_CONF}

#Run http server
RealServer1=RealServer1
RealServer1_ADDR=127.0.0.1
RealServer1_PORT=50001
start_lighttpd -s $RealServer1 -a $RealServer1_ADDR -p $RealServer1_PORT

if [ $? -ne 0 ]
then
        echo "Test failed: start_lighttpd RealServer1"
        exit 1
fi

RealServer2=RealServer2
RealServer2_ADDR=127.0.0.1
RealServer2_PORT=50002
start_lighttpd -s $RealServer2 -a $RealServer2_ADDR -p $RealServer2_PORT

if [ $? -ne 0 ]
then
        echo "Test failed: start_lighttpd RealServer2"
        exit 1
fi

#Add Service
$L7VSD
if [ $? -ne 0 ]
then
	echo "Test failed: $L7VSD"
        exit 1
fi

usleep 100000
$L7VSADM -A -t [::1]:40001 -m sessionless
if [ $? -ne 0 ]
then
	echo "Test failed: $L7VSADM -A -t [::1]:40001 -m sessionless"
        exit 1
fi

$L7VSADM -a -t [::1]:40001 -m sessionless -r ${RealServer1_ADDR}:${RealServer1_PORT}
if [ $? -ne 0 ]
then
	echo "Test failed: $L7VSADM -a -t [::1]:40001 -m sessionless -r ${RealServer1_ADDR}:${RealServer1_PORT}"
        exit 1
fi

$L7VSADM -a -t [::1]:40001 -m sessionless -r ${RealServer2_ADDR}:${RealServer2_PORT}
if [ $? -ne 0 ]
then
	echo "Test failed: $L7VSADM -a -t [::1]:40001 -m sessionless -r ${RealServer2_ADDR}:${RealServer2_PORT}"
        exit 1
fi

$L7VSADM -d -t [::1]:40001 -m sessionless -r ${RealServer2_ADDR}:${RealServer2_PORT}
if [ $? -ne 0 ]
then
	echo "Test failed: $L7VSADM -d -t [::1]:40001 -m sessionless -r ${RealServer2_ADDR}:${RealServer2_PORT}"
        exit 1
fi

RET=`$L7VSADM -l -n`
EXPECT="Layer-7 Virtual Server version 3.0.0-1
Prot LocalAddress:Port ProtoMod Scheduler
  -> RemoteAddress:Port           Forward Weight ActiveConn InactConn
TCP [::1]:40001 sessionless rr
  -> 127.0.0.1:50001              Masq    1      0          0         "

if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -l -n"
        exit 1
fi

#Del IPaliasing
stop_lighttpd $RealServer1
stop_lighttpd $RealServer2

exit 0

