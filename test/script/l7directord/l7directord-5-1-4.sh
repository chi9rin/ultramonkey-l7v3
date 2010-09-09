#!/bin/bash
. ${SET_DEFAULT_CONF}
echo "" > ${L7DIRECTORD_CONF_DIR}/l7directord.cf

#Start l7directord
$L7DIRECTORD start
if [ $? -ne 0 ]
then
        echo "Test failed: $L7DIRECTORD start"
        exit 1
fi
sleep 1

rm -f ${L7DIRECTORD_CONF_DIR}/l7directord.cf
sleep 10
RET=`cat ${L7VS_LOG_DIR}/l7directord.log | grep "\[ERR0410\] Cannot get mtime of configuration file ./etc/ha.d/conf/l7directord.cf"`
if [ -z "$RET" ]
then
        echo "Test failed: cat ${L7VS_LOG_DIR}/l7directord.cf"
        exit 1
fi

exit 0

