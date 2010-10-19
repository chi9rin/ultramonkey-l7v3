#!/bin/bash
. ${SET_DEFAULT_CONF}
VS="[::1]"
RS="127.0.0.1"
echo "virtual=$RS:40001" >  ${L7DIRECTORD_CONF_DIR}/l7directord.cf
echo "    real=$RS:50001" >>  ${L7DIRECTORD_CONF_DIR}/l7directord.cf
echo "    real=$RS:50002" >>  ${L7DIRECTORD_CONF_DIR}/l7directord.cf

#Add Service
$INIT_L7VSD start
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi
usleep 100000

#Start l7directord
$INIT_L7DIRECTORD start
if [ $? -ne 0 ]
then
        echo "Test failed: $L7DIRECTORD start"
        exit 1
fi
sleep 2

RET=`$L7VSADM -l`
EXPECT="Layer-7 Virtual Server version 3.0.0
Prot LocalAddress:Port ProtoMod Scheduler
  -> RemoteAddress:Port           Forward Weight ActiveConn InactConn
TCP localhost:40001 sessionless rr
  -> localhost:50001              Masq    1      0          0         
  -> localhost:50002              Masq    1      0          0         "
if [ "$RET" != "$EXPECT" ]
then
        echo "Test failed: $L7VSADM -l"
        exit 1
fi

exit 0

