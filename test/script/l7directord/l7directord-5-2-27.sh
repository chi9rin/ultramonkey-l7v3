#!/bin/bash
. ${SET_DEFAULT_CONF}
\cp ./materials/l7directord-5-2-27-a-l7directord.cf ${L7DIRECTORD_CONF_DIR}/l7directord.cf

#Run http server
RealServer1=RealServer1
RealServer1_ADDR=127.0.0.1
RealServer1_PORT=50001
start_lighttpd -s $RealServer1 -a $RealServer1_ADDR -p $RealServer1_PORT -l ${L7VS_LOG_DIR}/l7directord-5-2-27-a-access.log
if [ $? -ne 0 ]
then
        echo "Test failed: start_lighttpd RealServer1"
        exit 1
fi

# Start l7vsd
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi
usleep 100000

# Start l7directord
$INIT_L7DIRECTORD start 
if [ $? -ne 0 ]
then
        echo "Test failed: $INIT_L7DIRECTORD start"
        exit 1
fi
sleep 3

RET=`$L7VSADM -l -n`
EXPECT="Layer-7 Virtual Server version 3.0.1
Prot LocalAddress:Port ProtoMod Scheduler
  -> RemoteAddress:Port           Forward Weight ActiveConn InactConn
TCP 127.0.0.1:50000 sessionless rr
  -> 127.0.0.1:50001              Masq    1      0          0         "
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -l -n"
        exit 1
fi

RET=`cat ${L7VS_LOG_DIR}/l7directord-5-2-27-a-access.log | grep "GET /index.html HTTP/1.1"`
if [ -z "$RET" ]
then
        echo "Test failed: ${L7VS_LOG_DIR}/l7directord-5-2-27-a-access.log | grep GET /index.html HTTP/1.1"
        exit 1
fi

$INIT_L7DIRECTORD stop
if [ $? -ne 0 ]
then
        echo "Test failed: $INIT_L7DIRECTORD stop"
        exit 1
fi

\cp ./materials/l7directord-5-2-27-b-l7directord.cf ${L7DIRECTORD_CONF_DIR}/l7directord.cf

stop_lighttpd $RealServer1
start_lighttpd -s $RealServer1 -a $RealServer1_ADDR -p $RealServer1_PORT -l ${L7VS_LOG_DIR}/l7directord-5-2-27-b-access.log

# Start l7directord
$INIT_L7DIRECTORD start 
if [ $? -ne 0 ]
then
        echo "Test failed: $INIT_L7DIRECTORD start"
        exit 1
fi
sleep 3

RET=`$L7VSADM -l -n`
EXPECT="Layer-7 Virtual Server version 3.0.1
Prot LocalAddress:Port ProtoMod Scheduler
  -> RemoteAddress:Port           Forward Weight ActiveConn InactConn
TCP 127.0.0.1:50000 sessionless rr
  -> 127.0.0.1:50001              Masq    1      0          0         "
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -l -n"
        exit 1
fi

RET=`cat ${L7VS_LOG_DIR}/l7directord-5-2-27-b-access.log | grep "GET /index.html HTTP/1.1"`
if [ -z "$RET" ]
then
        echo "Test failed: ${L7VS_LOG_DIR}/l7directord-5-2-27-b-access.log | grep GET /index.html HTTP/1.1"
        exit 1
fi

exit 0

