#!/bin/bash
. ${SET_DEFAULT_CONF}
echo "virtual=127.0.0.1:10000" >  ${L7DIRECTORD_CONF_DIR}/l7directord.cf

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

RET=`$L7VSADM -K -n`
EXPECT="Layer-7 Virtual Server version 3.0.0-1
Prot LocalAddress:Port ProtoMod Scheduler
     SSL_config_file
     Socket option
     Access_log_flag
     Access_log_file
     Access_log_rotate option
  -> RemoteAddress:Port           Forward Weight ActiveConn InactConn
TCP 127.0.0.1:10000 sessionless rr
    none
    none
    0
    none
    none"
if [ "$RET" != "$EXPECT" ]
then
        echo "Test failed: $L7VSADM -K -n"
        exit 1
fi

# Edit l7directord config file
echo "virtual=127.0.0.1:20000" >  ${L7DIRECTORD_CONF_DIR}/l7directord.cf

kill -HUP `ps -ef | grep "$L7DIRECTORD" | grep -v grep | awk '{print $2}'`
if [ $? -ne 0 ]
then
        echo "Test failed: kill -HUP \`ps -ef | grep "$L7DIRECTORD" | grep -v grep | awk '{print \$2}'\`"
        exit 1
fi
sleep 1

RET=`$L7VSADM -K -n`
EXPECT="Layer-7 Virtual Server version 3.0.0-1
Prot LocalAddress:Port ProtoMod Scheduler
     SSL_config_file
     Socket option
     Access_log_flag
     Access_log_file
     Access_log_rotate option
  -> RemoteAddress:Port           Forward Weight ActiveConn InactConn
TCP 127.0.0.1:20000 sessionless rr
    none
    none
    0
    none
    none"
if [ "$RET" != "$EXPECT" ]
then
        echo "Test failed: $L7VSADM -K -n"
        exit 1
fi

exit 0

