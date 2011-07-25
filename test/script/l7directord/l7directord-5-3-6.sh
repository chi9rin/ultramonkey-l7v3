#!/bin/bash
. ${SET_DEFAULT_CONF}
# Set l7directord config file
echo "virtual=127.0.0.1:50000
virtual=127.0.0.1:50001
    module=ip
    scheduler=wrr" > ${L7DIRECTORD_CONF_DIR}/l7directord.cf

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
EXPECT="Layer-7 Virtual Server version 3.0.1
Prot LocalAddress:Port ProtoMod Scheduler
  -> RemoteAddress:Port           Forward Weight ActiveConn InactConn
TCP 127.0.0.1:50000 sessionless rr
TCP 127.0.0.1:50001 ip wrr"
if [ "$RET" != "$EXPECT" ]
then
        echo "Test failed: $L7VSADM -l -n"
        exit 1
fi

kill -KILL `pgrep l7directord`
if [ $? -ne 0 ]
then
        echo "Test failed: kill -KILL \`pgrep l7directord\`"
        exit 1
fi
sleep 5

# Edit l7director config file
echo "virtual=127.0.0.1:50000
virtual=127.0.0.1:50001
    module=ip
    scheduler=wrr
virtual=127.0.0.1:50002
    module=sslid
    scheduler=lc" > ${L7DIRECTORD_CONF_DIR}/l7directord.cf

# Start l7directord
$INIT_L7DIRECTORD start 
if [ $? -ne 0 ]
then
        echo "Test failed: $INIT_L7DIRECTORD start"
        exit 1
fi
sleep 10

RET=`$L7VSADM -l -n`
EXPECT="Layer-7 Virtual Server version 3.0.1
Prot LocalAddress:Port ProtoMod Scheduler
  -> RemoteAddress:Port           Forward Weight ActiveConn InactConn
TCP 127.0.0.1:50000 sessionless rr
TCP 127.0.0.1:50001 ip wrr
TCP 127.0.0.1:50002 sslid lc"
if [ "$RET" != "$EXPECT" ]
then
        echo "Test failed: $L7VSADM -l -n"
        exit 1
fi

exit 0
