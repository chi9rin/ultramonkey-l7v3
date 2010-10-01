#!/bin/bash
. ${SET_DEFAULT_CONF}
\cp ./materials/l7directord-5-2-10-6-l7directord.cf  ${L7DIRECTORD_CONF_DIR}/l7directord.cf

#Start l7vsd
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi
usleep 100000

#Start l7directord
$L7DIRECTORD start
if [ $? -ne 0 ]
then
        echo "Test failed: $L7DIRECTORD start"
        exit 1
fi
sleep 2

RET=`$L7VSADM -l -n`
EXPECT="Layer-7 Virtual Server version 3.0.0
Prot LocalAddress:Port ProtoMod Scheduler
  -> RemoteAddress:Port           Forward Weight ActiveConn InactConn
TCP 127.0.0.1:50000 sessionless rr
  -> 127.0.0.1:50001              Masq    5      0          0         "
if [ "$RET" != "$EXPECT" ]
then
        echo "Test failed: $L7VSADM"
        exit 1
fi

exit 0

