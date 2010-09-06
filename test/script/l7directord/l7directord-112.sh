#!/bin/bash
. ${SET_DEFAULT_CONF}
\cp ./materials/l7directord-112-a-l7directord.cf ${L7DIRECTORD_CONF_DIR}/l7directord.cf

#Run http server
FallbackServer1=FallbackServer1
FallbackServer1_ADDR=127.0.0.1
FallbackServer1_PORT=50001
start_lighttpd -s $FallbackServer1 -a $FallbackServer1_ADDR -p $FallbackServer1_PORT
if [ $? -ne 0 ]
then
        echo "Test failed: start_lighttpd FallbackServer1"
        exit 1
fi

FallbackServer2=FallbackServer2
FallbackServer2_ADDR=127.0.0.1
FallbackServer2_PORT=50005
start_lighttpd -s $FallbackServer2 -a $FallbackServer2_ADDR -p $FallbackServer2_PORT
if [ $? -ne 0 ]
then
        echo "Test failed: start_lighttpd FallbackServer1"
        exit 1
fi

RealServer1=RealServer1
RealServer1_ADDR=127.0.0.1
RealServer1_PORT=50002
start_lighttpd -s $RealServer1 -a $RealServer1_ADDR -p $RealServer1_PORT
if [ $? -ne 0 ]
then
        echo "Test failed: start_lighttpd RealServer1"
        exit 1
fi

RealServer2=RealServer2
RealServer2_ADDR=[::1]
RealServer2_PORT=50003
start_lighttpd -s $RealServer2 -a $RealServer2_ADDR -p $RealServer2_PORT -i 
if [ $? -ne 0 ]
then
        echo "Test failed: start_lighttpd RealServer2"
        exit 1
fi

RealServer3=RealServer3
RealServer3_ADDR=[::1]
RealServer3_PORT=50004
start_lighttpd -s $RealServer3 -a $RealServer3_ADDR -p $RealServer3_PORT -i
if [ $? -ne 0 ]
then
        echo "Test failed: start_lighttpd RealServer3"
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
sleep 3

RET=`$L7VSADM -l -n`
EXPECT="Layer-7 Virtual Server version 3.0.0-1
Prot LocalAddress:Port ProtoMod Scheduler
  -> RemoteAddress:Port           Forward Weight ActiveConn InactConn
TCP 127.0.0.1:40001 sessionless rr
  -> 127.0.0.1:50002              Masq    1      0          0         
TCP [::1]:40002 sessionless rr
  -> [::1]:50003                  Masq    1      0          0         
  -> [::1]:50004                  Masq    1      0          0         "
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -l -n"
        exit 1
fi

stop_lighttpd $RealServer1
if [ $? -ne 0 ]
then
        echo "Test failed: stop_lighttpd RealServer1"
        exit 1
fi

sleep 60


RET=`$L7VSADM -l -n`
EXPECT="Layer-7 Virtual Server version 3.0.0-1
Prot LocalAddress:Port ProtoMod Scheduler
  -> RemoteAddress:Port           Forward Weight ActiveConn InactConn
TCP 127.0.0.1:40001 sessionless rr
  -> 127.0.0.1:50001              Masq    1      0          0         
TCP [::1]:40002 sessionless rr
  -> [::1]:50003                  Masq    1      0          0         
  -> [::1]:50004                  Masq    1      0          0         "
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -l -n"
        exit 1
fi


start_lighttpd -s $RealServer1 -a $RealServer1_ADDR -p $RealServer1_PORT -l ${L7VS_LOG_DIR}/l7directord-112-real1a-access.log
if [ $? -ne 0 ]
then
        echo "Test failed: start_lighttpd RealServer1"
        exit 1
fi

sleep 20

RET=`$L7VSADM -l -n`
EXPECT="Layer-7 Virtual Server version 3.0.0-1
Prot LocalAddress:Port ProtoMod Scheduler
  -> RemoteAddress:Port           Forward Weight ActiveConn InactConn
TCP 127.0.0.1:40001 sessionless rr
  -> 127.0.0.1:50002              Masq    1      0          0         
TCP [::1]:40002 sessionless rr
  -> [::1]:50003                  Masq    1      0          0         
  -> [::1]:50004                  Masq    1      0          0         "
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -l -n"
        exit 1
fi

$INIT_L7DIRECTORD stop
if [ $? -ne 0 ]
then
        echo "Test failed: $INIT_L7DIRECTORD stop"
        exit 1
fi


RET=`cat ${L7VS_LOG_DIR}/l7directord.log | egrep "\[WRN1102\] Service check NG. HTTP response is not ok. Response status line is .500 Can't connect to 127.0.0.1:50002" | wc -l`
# checkcount 3
if [ $RET -ne 3 ]
then
        echo "Test failed: cat ${L7VS_LOG_DIR}/l7directord.log"
        exit 1
fi

TIME1=""
TIME2=""
for TMP in `cat ${L7VS_LOG_DIR}/l7directord.log | egrep "\[WRN1102\] Service check NG. HTTP response is not ok. Response status line is .500 Can't connect to 127.0.0.1:50002" | awk '{ print $4 }'`
do
        TIME2=`date -d "$TMP" +'%s'`
        if [ -n "$TIME1" ]
        then
                # retryinterval 1
                if [ $((TIME1+1)) -ne $((TIME2)) ]
                then
                        echo "Test failed: cat ${L7VS_LOG_DIR}/l7directord.log"
                        exit 1
                fi
        fi
        TIME1="$TIME2"
done


RET=`cat ${L7VS_LOG_DIR}/l7directord-112-real1a-access.log`
if [ -z "$RET" ]
then
        echo "Test failed: ${L7VS_LOG_DIR}/l7directord-112-real1a-access.log"
        exit 1
fi


TIME1=""
TIME2=""
for TMP in `cat ${L7VS_LOG_DIR}/l7directord-112-real1a-access.log | awk '{ print $4 }' | sed 's/:/ /' | awk '{ print $2}'`
do
        TIME2=`date -d "$TMP" +'%s'`
        if [ -n "$TIME1" ]
        then
		# checkinterval 5
                if [ $((TIME1+5)) -ne $((TIME2)) ]
                then
                        echo "Test failed: cat ${L7VS_LOG_DIR}/l7directord-112-real1a-access.log"
                        exit 1
                fi
        fi
        TIME1="$TIME2"
done


\cp ./materials/l7directord-112-b-l7directord.cf ${L7DIRECTORD_CONF_DIR}/l7directord.cf
mv  ${L7VS_LOG_DIR}/l7directord.log  ${L7VS_LOG_DIR}/l7directord_a.log

# Start l7directord
$INIT_L7DIRECTORD start 
if [ $? -ne 0 ]
then
        echo "Test failed: $INIT_L7DIRECTORD start"
        exit 1
fi
sleep 3

RET=`$L7VSADM -l -n`
EXPECT="Layer-7 Virtual Server version 3.0.0-1
Prot LocalAddress:Port ProtoMod Scheduler
  -> RemoteAddress:Port           Forward Weight ActiveConn InactConn
TCP 127.0.0.1:40001 sessionless rr
  -> 127.0.0.1:50002              Masq    1      0          0         
TCP [::1]:40002 sessionless rr
  -> [::1]:50003                  Masq    1      0          0         
  -> [::1]:50004                  Masq    1      0          0         "
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -l -n"
        exit 1
fi

stop_lighttpd $RealServer1
if [ $? -ne 0 ]
then
        echo "Test failed: stop_lighttpd RealServer1"
        exit 1
fi

sleep 60


RET=`$L7VSADM -l -n`
EXPECT="Layer-7 Virtual Server version 3.0.0-1
Prot LocalAddress:Port ProtoMod Scheduler
  -> RemoteAddress:Port           Forward Weight ActiveConn InactConn
TCP 127.0.0.1:40001 sessionless rr
  -> 127.0.0.1:50002              Masq    0      0          0         
  -> 127.0.0.1:50005              Masq    1      0          0         
TCP [::1]:40002 sessionless rr
  -> [::1]:50003                  Masq    1      0          0         
  -> [::1]:50004                  Masq    1      0          0         "
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -l -n"
        exit 1
fi


start_lighttpd -s $RealServer1 -a $RealServer1_ADDR -p $RealServer1_PORT -l ${L7VS_LOG_DIR}/l7directord-112-real1b-access.log
if [ $? -ne 0 ]
then
        echo "Test failed: start_lighttpd RealServer1"
        exit 1
fi

sleep 20

RET=`$L7VSADM -l -n`
EXPECT="Layer-7 Virtual Server version 3.0.0-1
Prot LocalAddress:Port ProtoMod Scheduler
  -> RemoteAddress:Port           Forward Weight ActiveConn InactConn
TCP 127.0.0.1:40001 sessionless rr
  -> 127.0.0.1:50002              Masq    1      0          0         
TCP [::1]:40002 sessionless rr
  -> [::1]:50003                  Masq    1      0          0         
  -> [::1]:50004                  Masq    1      0          0         "
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -l -n"
        exit 1
fi

$INIT_L7DIRECTORD stop
if [ $? -ne 0 ]
then
        echo "Test failed: $INIT_L7DIRECTORD stop"
        exit 1
fi


RET=`cat ${L7VS_LOG_DIR}/l7directord.log | egrep "\[WRN1102\] Service check NG. HTTP response is not ok. Response status line is .500 Can't connect to 127.0.0.1:50002" | wc -l`
# checkcount 2
if [ $RET -ne 2 ]
then
        echo "Test failed: cat ${L7VS_LOG_DIR}/l7directord.log"
        exit 1
fi

TIME1=""
TIME2=""
for TMP in `cat ${L7VS_LOG_DIR}/l7directord.log | egrep "\[WRN1102\] Service check NG. HTTP response is not ok. Response status line is .500 Can't connect to 127.0.0.1:50002" | awk '{ print $4 }'`
do
        TIME2=`date -d "$TMP" +'%s'`
        if [ -n "$TIME1" ]
        then
                # retryinterval 3
                if [ $((TIME1+3)) -ne $((TIME2)) ]
                then
                        echo "Test failed: cat ${L7VS_LOG_DIR}/l7directord.log"
                        exit 1
                fi
        fi
        TIME1="$TIME2"
done


RET=`cat ${L7VS_LOG_DIR}/l7directord-112-real1b-access.log`
if [ -z "$RET" ]
then
        echo "Test failed: ${L7VS_LOG_DIR}/l7directord-112-real1b-access.log"
        exit 1
fi


TIME1=""
TIME2=""
for TMP in `cat ${L7VS_LOG_DIR}/l7directord-112-real1b-access.log | awk '{ print $4 }' | sed 's/:/ /' | awk '{ print $2}'`
do
        TIME2=`date -d "$TMP" +'%s'`
        if [ -n "$TIME1" ]
        then
		# checkinterval 1
                if [ $((TIME1+1)) -ne $((TIME2)) ]
                then
                        echo "Test failed: cat ${L7VS_LOG_DIR}/l7directord-112-real1b-access.log"
                        exit 1
                fi
        fi
        TIME1="$TIME2"
done

exit 0

