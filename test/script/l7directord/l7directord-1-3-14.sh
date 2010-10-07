#!/bin/bash
. ${SET_DEFAULT_CONF}
\cp ./materials/l7directord-1-3-14-l7directord.cf ${L7DIRECTORD_CONF_DIR}/l7directord.cf

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

RET=`$L7VSADM -l`
EXPECT="Layer-7 Virtual Server version 3.0.0
Prot LocalAddress:Port ProtoMod Scheduler
  -> RemoteAddress:Port           Forward Weight ActiveConn InactConn
TCP localhost:40001 sessionless rr
  -> localhost:50001              Masq    1      0          0         "
if [ "$RET" != "$EXPECT" ]
then
        echo "Test failed: $L7VSADM -l"
        exit 1
fi

RET=`ps -ef | grep l7directord | grep -v grep | grep -v umtest.sh | awk '{print $8,$9}'`
EXPECT="$L7DIRECTORD start
l7directord: ping:127.0.0.1:50001:UP"
if [ "$RET" != "$EXPECT" ]
then
        echo "Test failed: ps -ef | grep l7directord | grep -v grep | grep -v umtest.sh | awk '{print \$8,\$9}'"
        exit 1
fi

kill -USR1 `ps -ef | grep "$L7DIRECTORD" | grep -v grep | awk '{print $2}'`
if [ $? -ne 0 ]
then
        echo "Test failed: kill -KILL \`ps -ef | grep "$L7DIRECTORD" | grep -v grep | awk '{print \$2}'\`"
        exit 1
fi
sleep 10

RET=`$L7VSADM -l`
EXPECT="Layer-7 Virtual Server version 3.0.0
Prot LocalAddress:Port ProtoMod Scheduler
  -> RemoteAddress:Port           Forward Weight ActiveConn InactConn
TCP localhost:40001 sessionless rr
  -> localhost:50001              Masq    1      0          0         "
if [ "$RET" != "$EXPECT" ]
then
        echo "Test failed: $L7VSADM -l"
        exit 1
fi

RET=`ps -ef | grep l7directord | grep -v grep | grep -v umtest.sh`
if [ -n "$RET" ]
then
        echo "Test failed: ps -ef | grep l7directord | grep -v grep | grep -v umtest.sh"
        exit 1
fi

exit 0

