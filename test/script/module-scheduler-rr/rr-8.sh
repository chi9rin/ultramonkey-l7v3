#!/bin/bash

. ../../config/env_conf.sh

#cp ./materials/l7directord.cf /etc/ha.d/conf/
#cp ./materials/l7vs.cf /etc/l7vs/
#cp -r ./materials/sslproxy /etc/l7vs/

/etc/init.d/l7vsd start > /dev/null

l7vsadm -A -t ${VS1ADDR} -m sessionless -s rr

RET=`LANG=C wget -t 1 -O- http://${VS1ADDR}/ 2>&1`
if [ $? -ne 1 ]
then        exit 1
fi

RET=`echo "$RET" | grep "Connecting to .* connected."`
if [ -z "${RET}" ]
then        exit 1
fi

/etc/init.d/l7vsd stop > /dev/null

exit 0

