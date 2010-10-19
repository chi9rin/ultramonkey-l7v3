#!/bin/bash
. ${SET_DEFAULT_CONF}
cp materials/B-30-l7vs.cf ${L7VSD_CONF_DIR}/l7vs.cf

#Add Service
$INIT_L7VSD start
if [ $? -ne 0 ]
then
        echo "Test failed: $INIT_L7VSD start"
        exit 1
fi
usleep 100000

RET=`$L7VSADM -V 2>&1 | grep -e "^l7vsd_network "`
EXPECT="l7vsd_network                  info"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -V | grep l7vsd_network"
        exit 1
fi

sed -i 's/l7vsd_network                    = "info"/l7vsd_network                    = "error"/' ${L7VSD_CONF_DIR}/l7vs.cf

$L7VSADM -P -r all
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM --parameter --reload logger"
        exit 1
fi

RET=`$L7VSADM -V 2>&1 | grep -e "^l7vsd_network "`
EXPECT="l7vsd_network                  error"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -V | grep l7vsd_network"
        exit 1
fi

exit 0
