#!/bin/bash

cp ./materials/l7directord.cf /etc/ha.d/conf/
cp ./materials/l7vs.cf /etc/l7vs/
cp -r ./materials/sslproxy /etc/l7vs/

/etc/init.d/l7vsd start

l7vsadm -A -t ${VS1ADDR} -m sessionless
l7vsadm -a -t ${VS1ADDR} -m sessionless -r ${RS1ADDR}
l7vsadm -a -t ${VS1ADDR} -m sessionless -r ${RS2ADDR}

RET=`wget -t 1 -T 1 -qO- http://${VS1ADDR}/`
if [ "${RET}" != "${RS1}" ]
then
exit 1
fi

RET=`wget -t 1 -T 1 -qO- http://${VS1ADDR}/`
if [ "${RET}" != "${RS2}" ]
then
exit 1
fi

RET=`wget -t 1 -T 1 -qO- http://${VS1ADDR}/`
if [ "${RET}" != "${RS1}" ]
then
exit 1
fi


/etc/init.d/l7vsd stop

exit 0
