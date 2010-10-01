#!/bin/bash
. ${SET_DEFAULT_CONF}
\cp ./materials/l7directord-5-3-1-l7directord.cf ${L7DIRECTORD_CONF_DIR}/l7directord.cf

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
EXPECT="Layer-7 Virtual Server version 3.0.0
Prot LocalAddress:Port ProtoMod Scheduler
  -> RemoteAddress:Port           Forward Weight ActiveConn InactConn
TCP 127.0.0.1:50000 sessionless rr
TCP 127.0.0.1:50001 ip wrr
TCP 127.0.0.1:50002 sslid lc
TCP 127.0.0.1:50003 sessionless rr"
if [ "$RET" != "$EXPECT" ]
then
        echo "Test failed: $L7VSADM -l -n"
        exit 1
fi

exit 0
