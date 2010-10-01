#!/bin/bash
. ${SET_DEFAULT_CONF}
sed "s|XXX|127.0.0.1:40001|g" ./materials/l7directord-5-2-8-l7directord.cf > ${L7DIRECTORD_CONF_DIR}/l7directord.cf

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
sleep 1

RET=`$L7VSADM -l -n`
EXPECT="Layer-7 Virtual Server version 3.0.0
Prot LocalAddress:Port ProtoMod Scheduler
  -> RemoteAddress:Port           Forward Weight ActiveConn InactConn
TCP 127.0.0.1:40001 sessionless rr"
if [ "$RET" != "$EXPECT" ]
then
        echo "Test failed: $L7VSADM -l -n"
        exit 1
fi

exit 0

