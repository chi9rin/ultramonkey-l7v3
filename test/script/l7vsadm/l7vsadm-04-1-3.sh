#!/bin/bash

. ${SET_DEFAULT_CONF}

#Run http server
SorryServer1=SorryServer1
SorryServer1_ADDR=127.0.0.1
SorryServer1_PORT=50001
start_lighttpd -s $SorryServer1 -a $SorryServer1_ADDR -p $SorryServer1_PORT
if [ $? -ne 0 ]
then
        echo "Test failed: start_lighttpd SorryServer1"
        exit 1
fi

#Add Service
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi
usleep 100000

$L7VSADM --add-service --tcp-service 127.0.0.1:40001 --proto-module ip --scheduler rr --upper 100 --bypass ${SorryServer1_ADDR}:${SorryServer1_PORT} --flag 1 --qos-up 1G --qos-down 1G --ssl /etc/l7vs/sslproxy/sslproxy.target.cf --sockopt deferaccept --access-log 1 --access-log-name /var/log/l7vs/access_virtualservice_1.log --ac-rotate-type "datesize" --ac-rotate-max-backup-index "10" --ac-rotate-max-filesize "10M" --ac-rotate-rotation-timing "year" --ac-rotate-rotation-timing-value "11/11 11:11"
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM --add-service --tcp-service 127.0.0.1:40001 --proto-module ip --scheduler rr --upper 100 --bypass ${SorryServer1_ADDR}:${SorryServer1_PORT} --flag 1 --qos-up 1G --qos-down 1G --ssl /etc/l7vs/sslproxy/sslproxy.target.cf --sockopt deferaccept --access-log 1 --access-log-name /var/log/l7vs/access_virtualservice_1.log --ac-rotate-type datesize --ac-rotate-max-backup-index 10 --ac-rotate-max-filesize 10M --ac-rotate-rotation-timing year --ac-rotate-rotation-timing-value 11/11 11:11"
        exit 1
fi


exit 0

