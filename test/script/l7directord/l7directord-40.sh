#!/bin/bash
. ${SET_DEFAULT_CONF}
\cp ./materials/l7directord-40-l7directord.cf ${L7DIRECTORD_CONF_DIR}/l7directord.cf

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

RET=`$L7VSADM -K -n`
EXPECT="Layer-7 Virtual Server version 3.0.0-1
Prot LocalAddress:Port ProtoMod Scheduler
     SSL_config_file
     Socket option
     Access_log_flag
     Access_log_file
     Access_log_rotate option
  -> RemoteAddress:Port           Forward Weight ActiveConn InactConn
TCP 127.0.0.1:50000 sessionless rr
    none
    none
    0
    none
    none
  -> 127.0.0.1:50001              Masq    1      0          0         
  -> 127.0.0.1:50002              Masq    2      0          0         
  -> 127.0.0.1:50003              Masq    3      0          0         
  -> 127.0.0.1:50004              Masq    4      0          0         
  -> 127.0.0.1:50005              Masq    5      0          0         
TCP [::1]:50100 sessionless rr
    none
    none
    0
    none
    none
  -> [::1]:50101                  Masq    1      0          0         
  -> [::1]:50102                  Masq    2      0          0         
  -> [::1]:50103                  Masq    3      0          0         
  -> [::1]:50104                  Masq    4      0          0         
  -> [::1]:50105                  Masq    5      0          0         "

if [ "$RET" != "$EXPECT" ]
then
        echo "Test failed: $L7VSADM -K -n"
        exit 1
fi

RET=`ps -ef | grep l7directord | grep -v grep | grep -v umtest.sh | awk '{print $8,$9}'`
EXPECT="/usr/sbin/l7directord start
l7directord: ping:127.0.0.1:50001:UP
l7directord: ping:127.0.0.1:50002:UP
l7directord: ping:127.0.0.1:50003:UP
l7directord: ping:127.0.0.1:50004:UP
l7directord: ping:127.0.0.1:50005:UP
l7directord: ping:[::1]:50101:UP
l7directord: ping:[::1]:50102:UP
l7directord: ping:[::1]:50103:UP
l7directord: ping:[::1]:50104:UP
l7directord: ping:[::1]:50105:UP"
if [ "$RET" != "$EXPECT" ]
then
        echo "Test failed: ps -ef | grep l7directord | grep -v grep | grep -v umtest.sh | awk '{print \$8,\$9}'"
        exit 1
fi

kill -SEGV `ps -ef | grep "$L7DIRECTORD" | grep -v grep | awk '{print $2}'`
if [ $? -ne 0 ]
then
        echo "Test failed: kill -SEGV \`ps -ef | grep "$L7DIRECTORD" | grep -v grep | awk '{print \$2}'\`"
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
  -> RemoteAddress:Port           Forward Weight ActiveConn InactConn"
if [ "$RET" != "$EXPECT" ]
then
        echo "Test failed: $L7VSADM -K -n"
        exit 1
fi

RET=`ps -ef | grep l7directord | grep -v grep | grep -v umtest.sh`
if [ -n "$RET" ]
then
        echo "Test failed: ps -ef | grep l7directord | grep -v grep | grep -v umtest.sh"
        exit 1
fi

exit 0

