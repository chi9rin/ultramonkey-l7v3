#!/bin/bash
. ${SET_DEFAULT_CONF}
\cp ./materials/l7directord-2-2-3-l7directord.cf ${L7DIRECTORD_CONF_DIR}/l7directord.cf

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
sleep 10

RET=`$L7VSADM -l -n`
EXPECT="Layer-7 Virtual Server version 3.0.0-1
Prot LocalAddress:Port ProtoMod Scheduler
  -> RemoteAddress:Port           Forward Weight ActiveConn InactConn
TCP 127.0.0.1:40001 sessionless rr
  -> 127.0.0.1:50001              Masq    1      0          0         
  -> 127.0.0.1:50002              Masq    1      0          0         
  -> 127.0.0.1:50003              Masq    1      0          0         
  -> 127.0.0.1:50004              Masq    1      0          0         "
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -l -n"
        exit 1
fi

RET=`ps -ef | grep l7directord | grep -v grep | grep -v umtest.sh | awk '{print $8,$9}'`
EXPECT="/usr/sbin/l7directord start
l7directord: ping:127.0.0.1:50001:UP
l7directord: ping:127.0.0.1:50002:UP
l7directord: ping:127.0.0.1:50003:UP
l7directord: ping:127.0.0.1:50004:UP"
if [ "$RET" != "$EXPECT" ]
then
        echo "Test failed: ps -ef | grep l7directord | grep -v grep | grep -v umtest.sh | awk '{print \$8,\$9}'"
        exit 1
fi

PID=`ps -ef | grep "l7directord: ping:127.0.0.1:50002:UP" | awk '{print $2}'`
kill -KILL $PID

sleep 5

RET=`$L7VSADM -l -n`
EXPECT="Layer-7 Virtual Server version 3.0.0-1
Prot LocalAddress:Port ProtoMod Scheduler
  -> RemoteAddress:Port           Forward Weight ActiveConn InactConn
TCP 127.0.0.1:40001 sessionless rr
  -> 127.0.0.1:50001              Masq    1      0          0         
  -> 127.0.0.1:50002              Masq    1      0          0         
  -> 127.0.0.1:50003              Masq    1      0          0         
  -> 127.0.0.1:50004              Masq    1      0          0         "
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -l -n"
        exit 1
fi

RET=`ps -ef | grep l7directord | grep -v grep | grep -v umtest.sh | awk '{print $8,$9}'`
EXPECT="/usr/sbin/l7directord start
l7directord: ping:127.0.0.1:50001:UP
l7directord: ping:127.0.0.1:50003:UP
l7directord: ping:127.0.0.1:50004:UP
l7directord: ping:127.0.0.1:50002:UP"
if [ "$RET" != "$EXPECT" ]
then
        echo "Test failed: ps -ef | grep l7directord | grep -v grep | grep -v umtest.sh | awk '{print \$8,\$9}'"
        exit 1
fi

exit 0

