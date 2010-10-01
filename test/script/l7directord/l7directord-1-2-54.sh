#!/bin/bash
. ${SET_DEFAULT_CONF}
\cp ./materials/l7directord-1-2-54-l7directord.cf ${L7DIRECTORD_CONF_DIR}/l7directord.cf
echo "virtual=127.0.0.1:40001
    real=127.0.0.1:50001
    checktype=ping" > ${L7DIRECTORD_CONF_DIR}/a.cf

echo "virtual=127.0.0.1:40002
    real=127.0.0.1:50002
    checktype=ping" > ${L7DIRECTORD_CONF_DIR}/b.cf

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
sleep 15

RET=`$L7VSADM -l -n`
EXPECT="Layer-7 Virtual Server version 3.0.0
Prot LocalAddress:Port ProtoMod Scheduler
  -> RemoteAddress:Port           Forward Weight ActiveConn InactConn
TCP 127.0.0.1:40001 sessionless rr
  -> 127.0.0.1:50001              Masq    1      0          0         
TCP 127.0.0.1:40002 sessionless rr
  -> 127.0.0.1:50002              Masq    1      0          0         "
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -l -n"
        exit 1
fi

echo "virtual=127.0.0.1:40003
    real=[::1]:50003
    real=[::1]:50004" >> ${L7DIRECTORD_CONF_DIR}/l7directord.cf

echo "virtual=127.0.0.1:40001
    real=127.0.0.1:50001 10
    checktype=ping" > ${L7DIRECTORD_CONF_DIR}/a.cf

echo "virtual=127.0.0.1:40002
    real=127.0.0.1:50002 20
    checktype=ping" > ${L7DIRECTORD_CONF_DIR}/b.cf

# Restart l7directord
$INIT_L7DIRECTORD restart 
if [ $? -ne 0 ]
then
        echo "Test failed: $INIT_L7DIRECTORD restart"
        exit 1
fi
sleep 15

RET=`$L7VSADM -l -n`
EXPECT="Layer-7 Virtual Server version 3.0.0
Prot LocalAddress:Port ProtoMod Scheduler
  -> RemoteAddress:Port           Forward Weight ActiveConn InactConn
TCP 127.0.0.1:40001 sessionless rr
  -> 127.0.0.1:50001              Masq    10     0          0         
TCP 127.0.0.1:40002 sessionless rr
  -> 127.0.0.1:50002              Masq    20     0          0         
TCP 127.0.0.1:40003 sessionless rr
  -> [::1]:50003                  Masq    1      0          0         
  -> [::1]:50004                  Masq    1      0          0         "
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -l -n"
        exit 1
fi

RET=`$INIT_L7DIRECTORD status 2>&1 | egrep "l7directord for ./etc/ha.d/conf/l7directord.cf' is running with pid: .*
l7directord for ./etc/ha.d/conf/a.cf' is running with pid: .*
l7directord for ./etc/ha.d/conf/b.cf' is running with pid: .*"`
if [ -z "${RET}" ]
then
        echo "Test failed: $INIT_L7DIRECTORD status"
        exit 1
fi

# Stop l7directord
$INIT_L7DIRECTORD stop
if [ $? -ne 0 ]
then
        echo "Test failed: $INIT_L7DIRECTORD stop"
        exit 1
fi
sleep 3

RET=`ps -ef | grep l7directord | grep -v grep | grep -v umtest.sh`
if [ -n "$RET"  ]
then
        echo "Test failed: ps -ef | grep l7directord | grep -v grep | grep -v umtest.sh"
        exit 1
fi

exit 0

