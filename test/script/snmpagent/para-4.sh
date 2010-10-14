#!/bin/bash

# set config file
. ${SET_DEFAULT_CONF}
\cp -f materials/para-4-l7vs.cf ${L7VSD_CONF_DIR}/l7vs.cf

#Add Service
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi
usleep 100000

RET=`$L7VSADM -V | grep "cache update interval"`
EXPECT="cache update interval          4"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -V"
        exit 1
fi

exit 0
