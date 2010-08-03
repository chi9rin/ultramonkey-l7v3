#!/bin/bash

. ${SET_DEFAULT_CONF}

#Add Service
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi
usleep 100000

RET=`$L7VSADM -x 2>&1`
EXPECT="PARSE ERROR : command not found.
Usage: 
  l7vsadm -A -t service-address -m proto-module [module-args]
          [-s scheduler] [-u connection-count] [-b sorry-server]
          [-f sorry-flag] [-Q QoSval-up] [-q QoSval-down] [-z ssl-config-file]
          [-O socket-option] [-L access-log-flag] [-a access-log-file [logrotate-args]]
  l7vsadm -E -t service-address -m proto-module [module-args]
          [-s scheduler] [-u connection-count] [-b sorry-server]
          [-f sorry-flag] [-Q QoSval-up] [-q QoSval-down] [-L access-log-flag]
  l7vsadm -D -t service-address -m proto-module [module-args]
  l7vsadm -C
  l7vsadm -a|e -t service-address -m proto-module [module-args]
          -r server-address [-w weight]
  l7vsadm -d -t service-address -m proto-module [module-args]
          -r server-address
  l7vsadm -R -s replication-switch
  l7vsadm -R -f
  l7vsadm -R -d
  l7vsadm -L -c log-category -l log-level
  l7vsadm -S -c log-category -l log-level
  l7vsadm -P -r reload-parameter
  l7vsadm -l [-n]
  l7vsadm -V [-n]
  l7vsadm -K [-n]
  l7vsadm -h"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -x"
        exit 1
fi

exit 0

