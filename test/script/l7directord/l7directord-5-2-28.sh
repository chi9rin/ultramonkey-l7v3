#!/bin/bash
. ${SET_DEFAULT_CONF}
\cp ./materials/l7directord-5-2-28-l7directord.cf ${L7DIRECTORD_CONF_DIR}/l7directord.cf
touch ${L7DIRECTORD_CONF_DIR}/1.cf
touch ${L7DIRECTORD_CONF_DIR}/2.cf
touch ${L7DIRECTORD_CONF_DIR}/3.cf

#Run http server
RealServer1=RealServer1
RealServer1_ADDR=127.0.0.1
RealServer1_PORT=50001
start_lighttpd -s $RealServer1 -a $RealServer1_ADDR -p $RealServer1_PORT
if [ $? -ne 0 ]
then
        echo "Test failed: start_lighttpd RealServer1"
        exit 1
fi

RealServer2=RealServer2
RealServer2_ADDR=127.0.0.1
RealServer2_PORT=50002
start_lighttpd -s $RealServer2 -a $RealServer2_ADDR -p $RealServer2_PORT
if [ $? -ne 0 ]
then
        echo "Test failed: start_lighttpd RealServer2"
        exit 1
fi

RealServer3=RealServer3
RealServer3_ADDR=[::1]
RealServer3_PORT=50003
start_lighttpd -s $RealServer3 -a $RealServer3_ADDR -p $RealServer3_PORT -i
if [ $? -ne 0 ]
then
        echo "Test failed: start_lighttpd RealServer3"
        exit 1
fi

RealServer4=RealServer4
RealServer4_ADDR=[::1]
RealServer4_PORT=50004
start_lighttpd -s $RealServer4 -a $RealServer4_ADDR -p $RealServer4_PORT -i
if [ $? -ne 0 ]
then
        echo "Test failed: start_lighttpd RealServer4"
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
sleep 15

RET=`$L7VSADM -l -n`
EXPECT="Layer-7 Virtual Server version 3.0.0-1
Prot LocalAddress:Port ProtoMod Scheduler
  -> RemoteAddress:Port           Forward Weight ActiveConn InactConn
TCP 127.0.0.1:40001 sessionless rr
TCP 127.0.0.1:40002 sessionless rr
  -> 127.0.0.1:50001              Masq    1      0          0         
  -> 127.0.0.1:50002              Masq    1      0          0         
TCP 127.0.0.1:40003 sessionless rr
  -> [::1]:50003                  Masq    1      0          0         
  -> [::1]:50004                  Masq    1      0          0         "
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -l -n"
        exit 1
fi

RET=`ps -ef | grep l7directord | grep -v grep | grep -v umtest.sh | awk '{print $8,$9}'`
EXPECT="/usr/sbin/l7directord start
/usr/sbin/l7directord /etc/ha.d/conf/2.cf
/usr/sbin/l7directord /etc/ha.d/conf/3.cf
/usr/sbin/l7directord /etc/ha.d/conf/1.cf
l7directord: none:127.0.0.1:50001:UP
l7directord: none:127.0.0.1:50002:UP
l7directord: none:[::1]:50003:UP
l7directord: none:[::1]:50004:UP"
if [ "$RET" != "$EXPECT" ]
then
        echo "Test failed: ps -ef | grep l7directord | grep -v grep | grep -v umtest.sh | awk '{print \$8,\$9}'"
        exit 1
fi

exit 0

