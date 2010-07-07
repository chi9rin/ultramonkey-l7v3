#!/bin/bash

. ../../config/env_conf.sh

#cp ./materials/l7directord.cf /etc/ha.d/conf/
#cp ./materials/l7vs.cf /etc/l7vs/
#cp -r ./materials/sslproxy /etc/l7vs/

/etc/init.d/l7vsd start > /dev/null

l7vsadm -A -t ${VS1ADDR} -m sessionless -s lc
l7vsadm -a -t ${VS1ADDR} -m sessionless -r ${RS1ADDR} -w 1


RET=`l7vsadm -l -n | grep "${RS1ADDR}" | awk '{print $5}'`
if [ "${RET}" -ne 0 ]
then
	exit 1
fi

./test_client `echo "${VS1ADDR}" | sed 's/:/ /g'` &
CONNECT1=$!
usleep 100000
RET=`l7vsadm -l -n | grep "${RS1ADDR}" | awk '{print $5}'`
if [ "${RET}" -ne 1 ]
then 
        exit 1
fi

./test_client `echo "${VS1ADDR}" | sed 's/:/ /g'` &
CONNECT2=$!
usleep 100000
RET=`l7vsadm -l -n | grep "${RS1ADDR}" | awk '{print $5}'`
if [ "${RET}" -ne 2 ]
then
         exit 1
fi


l7vsadm -a -t ${VS1ADDR} -m sessionless -r ${RS2ADDR} -w 1

RET=`l7vsadm -l -n | grep "${RS1ADDR}" | awk '{print $5}'`
if [ "${RET}" -ne 2 ]
then
	exit 1
fi

RET=`l7vsadm -l -n | grep "${RS2ADDR}" | awk '{print $5}'`
if [ "${RET}" -ne 0 ]
then
        exit 1
fi

./test_client `echo "${VS1ADDR}" | sed 's/:/ /g'` &
CONNECT3=$!
usleep 100000
RET=`l7vsadm -l -n | grep "${RS2ADDR}" | awk '{print $5}'`
if [ "${RET}" -ne 1 ]
then
        exit 1
fi

./test_client `echo "${VS1ADDR}" | sed 's/:/ /g'` &
CONNECT4=$!
usleep 100000
RET=`l7vsadm -l -n | grep "${RS2ADDR}" | awk '{print $5}'`
if [ "${RET}" -ne 2 ]
then
	exit 1
fi

l7vsadm -a -t ${VS1ADDR} -m sessionless -r ${RS3ADDR} -w 1
RET=`l7vsadm -l -n | grep "${RS1ADDR}" | awk '{print $5}'`
if [ "${RET}" -ne 2 ]
then
        exit 1
fi

RET=`l7vsadm -l -n | grep "${RS2ADDR}" | awk '{print $5}'`
if [ "${RET}" -ne 2 ]
then
        exit 1
fi

RET=`l7vsadm -l -n | grep "${RS3ADDR}" | awk '{print $5}'`
if [ "${RET}" -ne 0 ]
then
        exit 1
fi

./test_client `echo "${VS1ADDR}" | sed 's/:/ /g'` &
CONNECT5=$!
usleep 100000
RET=`l7vsadm -l -n | grep "${RS3ADDR}" | awk '{print $5}'`
if [ "${RET}" -ne 1 ]
then
        exit 1
fi

./test_client `echo "${VS1ADDR}" | sed 's/:/ /g'` &
CONNECT6=$!
usleep 100000
RET=`l7vsadm -l -n | grep "${RS3ADDR}" | awk '{print $5}'`
if [ "${RET}" -ne 2 ]
then
        exit 1
fi

kill ${CONNECT1}
kill ${CONNECT2}
kill ${CONNECT3}
kill ${CONNECT4}
kill ${CONNECT5}
kill ${CONNECT6}

/etc/init.d/l7vsd stop > /dev/null

exit 0

