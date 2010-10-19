#!/bin/bash
. ${SET_DEFAULT_CONF}
\cp -f materials/B-32-l7vs.cf ${L7VSD_CONF_DIR}/l7vs.cf
\cp -f materials/B-32-IPv6v6-l7directord.cf ${L7DIRECTORD_CONF_DIR}/l7directord.cf

VS="[::1]"
RS="[::1]"

#Run http server
RealServer1=RealServer1
RealServer1_ADDR=$RS
RealServer1_PORT=50001
start_lighttpd -s $RealServer1 -a $RealServer1_ADDR -p $RealServer1_PORT -i
if [ $? -ne 0 ]
then
        echo "Test failed: start_lighttpd RealServer1"
        exit 1
fi

#Add Service
$INIT_L7VSD start
if [ $? -ne 0 ]
then
        echo "Test failed: $INIT_L7VSD start"
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

RET=`$L7VSADM -V | grep -e "^l7vsd_virtualservice "`
EXPECT="l7vsd_virtualservice           debug"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -V | grep l7vsd_virtualservice"
        exit 1
fi

RET=`$L7VSADM`
EXPECT="Layer-7 Virtual Server version 3.0.0
Prot LocalAddress:Port ProtoMod Scheduler
  -> RemoteAddress:Port           Forward Weight ActiveConn InactConn
TCP localhost:50000 sessionless rr
  -> localhost:50001              Masq    1      0          0         "
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM"
        exit 1
fi

#Connect
for ((i=0;i<256;i++)){
	RET=`$WGET -t 1 -qO- http://$VS:50000/`
	if [ "${RET}" != "${RealServer1}" ]
	then
        	echo "Test failed: $WGET -t 1 -qO- http://$VS:40001/"
        	exit 1
	fi
}

RET=`$L7VSADM | grep "localhost:50001" | awk '{print $6}'`
if [ 256 -ne ${RET} ]
then
        echo "Test failed: $L7VSADM | grep localhost:50001 | awk '{print $6}'"
        exit 1
fi

RET=`ls ${L7VS_LOG_DIR}/l7vsd.log* | wc -l`
if [ ${RET} -ne 2 ]
then
        echo "Test failed: ls ${L7VS_LOG_DIR}/l7vsd.log*"
        exit 1
fi

RET=`ls ${L7VS_LOG_DIR}/l7vsd.log`
EXPECT="${L7VS_LOG_DIR}/l7vsd.log"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: ls ${L7VS_LOG_DIR}/l7vsd.log"
        exit 1
fi

RET=`ls ${L7VS_LOG_DIR}/l7vsd.log.1`
EXPECT="${L7VS_LOG_DIR}/l7vsd.log.1"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: ls ${L7VS_LOG_DIR}/l7vsd.log.1"
        exit 1
fi


exit 0
