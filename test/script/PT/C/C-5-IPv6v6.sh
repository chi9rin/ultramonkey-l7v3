#!/bin/bash
. ${SET_DEFAULT_CONF}
\cp ./materials/C-5-IPv6v6-l7directord.cf ${L7DIRECTORD_CONF_DIR}/l7directord.cf

VS="[::1]"
RS1="[::1]"
RS2="[::2]"
ifconfig lo:0 add ::2
#Run http server
RealServer1=RealServer1
RealServer1_ADDR=$RS1
RealServer1_PORT=50001
start_lighttpd -s $RealServer1 -a $RealServer1_ADDR -p $RealServer1_PORT -i
if [ $? -ne 0 ]
then
        echo "Test failed: start_lighttpd RealServer1"
        exit 1
fi

RealServer2=RealServer2
RealServer2_ADDR=$RS2
RealServer2_PORT=50002
start_lighttpd -s $RealServer2 -a $RealServer2_ADDR -p $RealServer2_PORT -i
if [ $? -ne 0 ]
then
        echo "Test failed: start_lighttpd RealServer2"
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
sleep 5

RET=`$INIT_L7DIRECTORD status 2>&1 | grep "l7directord for .${L7DIRECTORD_CONF_DIR}/l7directord.cf' is running with pid:"`
if [ -z "$RET"  ]
then
        echo "Test failed: $INIT_L7DIRECTORD status "
        exit 1
fi

RET=`$L7VSADM -l -n`
EXPECT="Layer-7 Virtual Server version 3.0.0
Prot LocalAddress:Port ProtoMod Scheduler
  -> RemoteAddress:Port           Forward Weight ActiveConn InactConn
TCP [::1]:50000 sessionless rr
  -> [::1]:50001                  Masq    1      0          0         
  -> [::2]:50002                  Masq    1      0          0         "
if [ "$RET" != "$EXPECT" ]
then
        echo "Test failed: $L7VSADM"
        exit 1
fi

ip6tables -A INPUT -s ::2 -j DROP
sleep 30
RET=`$L7VSADM -l -n`
EXPECT="Layer-7 Virtual Server version 3.0.0
Prot LocalAddress:Port ProtoMod Scheduler
  -> RemoteAddress:Port           Forward Weight ActiveConn InactConn
TCP [::1]:50000 sessionless rr
  -> [::1]:50001                  Masq    1      0          0         
  -> [::2]:50002                  Masq    0      0          0         "
if [ "$RET" != "$EXPECT" ]
then
	ip6tables -F
        echo "Test failed: $L7VSADM"
        exit 1
fi

ip6tables -F
exit 0

