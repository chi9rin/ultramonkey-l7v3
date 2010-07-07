#!/bin/bash

. ../../config/env_conf.sh

#cp ./materials/l7directord.cf /etc/ha.d/conf/
#cp ./materials/l7vs.cf /etc/l7vs/
#cp -r ./materials/sslproxy /etc/l7vs/

/etc/init.d/l7vsd start > /dev/null

l7vsadm -A -t ${VS1ADDR} -m sessionless -s wrr
l7vsadm -a -t ${VS1ADDR} -m sessionless -r ${RS1ADDR} -w 1
l7vsadm -a -t ${VS1ADDR} -m sessionless -r ${RS2ADDR} -w 2
l7vsadm -a -t ${VS1ADDR} -m sessionless -r ${RS3ADDR} -w 3

RET=`wget -t 1 -qO- http://${VS1ADDR}/`
if [ "${RET}" != "${RS3}" ]
then
	exit 1
fi

RET=`wget -t 1 -qO- http://${VS1ADDR}/`
if [ "${RET}" != "${RS2}" ]
then
        exit 1
fi

RET=`wget -t 1 -qO- http://${VS1ADDR}/`
if [ "${RET}" != "${RS3}" ]
then
        exit 1
fi

RET=`wget -t 1 -qO- http://${VS1ADDR}/`
if [ "${RET}" != "${RS1}" ]
then
        exit 1
fi

RET=`wget -t 1 -qO- http://${VS1ADDR}/`
if [ "${RET}" != "${RS2}" ]
then
        exit 1
fi

RET=`wget -t 1 -qO- http://${VS1ADDR}/`
if [ "${RET}" != "${RS3}" ]
then
        exit 1
fi

RET=`wget -t 1 -qO- http://${VS1ADDR}/`
if [ "${RET}" != "${RS3}" ]
then        exit 1
fi

RET=`wget -t 1 -qO- http://${VS1ADDR}/`
if [ "${RET}" != "${RS2}" ]
then
        exit 1
fi

RET=`wget -t 1 -qO- http://${VS1ADDR}/`
if [ "${RET}" != "${RS3}" ]
then
        exit 1
fi

RET=`wget -t 1 -qO- http://${VS1ADDR}/`
if [ "${RET}" != "${RS1}" ]
then
        exit 1
fi

RET=`wget -t 1 -qO- http://${VS1ADDR}/`
if [ "${RET}" != "${RS2}" ]
then        exit 1
fi

RET=`wget -t 1 -qO- http://${VS1ADDR}/`
if [ "${RET}" != "${RS3}" ]
then        exit 1
fi

/etc/init.d/l7vsd stop > /dev/null

exit 0

