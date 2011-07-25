#!/bin/bash
. ${SET_DEFAULT_CONF}
\cp ./materials/C-12-IPv6v4-l7directord.cf ${L7DIRECTORD_CONF_DIR}/l7directord.cf

VS="[::1]"
RS="127.0.0.1"

#Run http server
RealServer1=RealServer1
RealServer1_ADDR=$RS
RealServer1_PORT=50001
start_lighttpd -s $RealServer1 -a $RealServer1_ADDR -p $RealServer1_PORT
if [ $? -ne 0 ]
then
        echo "Test failed: start_lighttpd RealServer1"
        exit 1
fi

RealServer2=RealServer2
RealServer2_ADDR=$RS
RealServer2_PORT=50002
start_lighttpd -s $RealServer2 -a $RealServer2_ADDR -p $RealServer2_PORT 
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

RET=`$L7VSADM`
EXPECT="Layer-7 Virtual Server version 3.0.0
Prot LocalAddress:Port ProtoMod Scheduler
  -> RemoteAddress:Port           Forward Weight ActiveConn InactConn
TCP localhost:50000 sessionless rr
  -> localhost:50001              Masq    1      0          0         "
if [ "$RET" != "$EXPECT" ]
then
        echo "Test failed: $L7VSADM"
        exit 1
fi

echo "Error" > ${TMP_DIR}/index.html
set_lighttpd_content $RealServer1 ${TMP_DIR}/index.html

sleep 30
RET=`$L7VSADM`
EXPECT="Layer-7 Virtual Server version 3.0.0
Prot LocalAddress:Port ProtoMod Scheduler
  -> RemoteAddress:Port           Forward Weight ActiveConn InactConn
TCP localhost:50000 sessionless rr
  -> localhost:50001              Masq    0      0          0         "
if [ "$RET" != "$EXPECT" ]
then
        echo "Test failed: $L7VSADM"
        exit 1
fi

RET=`cat ${L7VS_LOG_DIR}/l7directord.log | grep "\[WRN1103\]" | wc -l`
if [ $RET -ne 3 ]
then
        echo "Test failed: cat ${L7VS_LOG_DIR}/l7directord.log"
        exit 1
fi

TIME1=""
TIME2=""
for TMP in `cat ${L7VS_LOG_DIR}/l7directord.log | grep "\[WRN1103\]" | awk '{ print $4 }'`
do
        TIME2=`date -d "$TMP" +'%s'`
        if [ -n "$TIME1" ]
        then
                if [ $((TIME1+5)) -gt $((TIME2)) ]
                then
                        echo "Test failed: cat ${L7VS_LOG_DIR}/l7directord.log"
                        exit 1
                fi
        fi
        TIME1="$TIME2"
done


TMP=`cat ${L7VS_LOG_DIR}/l7directord.log | grep "\[ERR0602\]" | awk '{ print $4 }'`
TIME2=`date -d "$TMP" +'%s'`

if [ $((TIME1)) -gt $((TIME2)) ]
then
        echo "Test failed: cat ${L7VS_LOG_DIR}/l7directord.log"
        exit 1
fi

exit 0

