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

RET=`$L7VSADM --help`
echo "$RET"
EXPECT="Usage: 
  l7vsadm -A -t service-address -m proto-module [module-args]
          [-s scheduler] [-u connection-count] [-b sorry-server] [-T|M]
          [-f sorry-flag] [-Q QoSval-up] [-q QoSval-down] [-z ssl-config-file]
          [-O socket-option] [-L access-log-flag] [-a access-log-file [logrotate-args]]
  l7vsadm -E -t service-address -m proto-module [module-args]
          [-s scheduler] [-u connection-count] [-b sorry-server] [-T|M]
          [-f sorry-flag] [-Q QoSval-up] [-q QoSval-down] [-L access-log-flag]
  l7vsadm -D -t service-address -m proto-module [module-args]
  l7vsadm -C
  l7vsadm -a|e -t service-address -m proto-module [module-args]
          -r server-address [-w weight] [-T|M]
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
  l7vsadm -h

Commands:
  --add-service      -A        add virtual service with options
  --edit-service     -E        edit virtual service with options
  --delete-service   -D        delete virtual service with options
  --flush            -C        flush virtual service
  --add-server       -a        add real server with options
  --edit-server      -e        edit real server with options
  --delete-server    -d        delete real server with options
  --replication      -R        control replication-function
  --log              -L        control logger-function
  --snmp             -S        control SNMP Agent-function
  --parameter        -P        control parameter-function
  --list             -l        list the table
  --verbose          -V        list the table in verbose format
  --key              -K        list the table in key setting format
  --help             -h        show usage

Options:
  --tcp-service      -t service-address     service-address is host:port
  --proto-module     -m proto-module        protocol module name and module argument
                        [module-args]
  --scheduler        -s scheduler           one of rr,lc,wrr
  --upper            -u connection-count    maximum number of connections
  --bypass           -b sorry-server        sorry server address is host:port
  --tproxy           -T                     set sorry server connection to IP transparent mode.
  --masq             -M                     set sorry server connection to IP masquerade mode.
  --flag             -f sorry-flag          sorry status set to virtual service
  --qos-up           -Q QoSval-up           QoS Threshold(bps) set to real server direction
  --qos-down         -q QoSval-down         QoS Threshold(bps) set to client direction
  --ssl              -z ssl-config-file     SSL configuration file(Use SSL)
  --sockopt          -O socket-option       deferaccept,nodelay,cork,quickackon or quickackoff set to socket option
  --access-log       -L access-log-flag     access log flag 0(none) or 1(output)
  --access-log-name  -a access-log-file     access log file
                        [logrotate-args]
  --real-server      -r server-address      server-address is host:port
  --weight           -w weight              scheduling weight set to real server
  --tproxy           -T                     set real server connection to IP transparent mode.
  --masq             -M                     set real server connection to IP masquerade mode.
  --switch           -s replication-switch  start or stop replication
  --force            -f                     force replication start
  --dump             -d                     dump replication memory
  --category         -c log-category        set log category for l7vsd or SNMP Agent
  --level            -l log-level           set log level for l7vsd or SNMP Agent
  --reload           -r reload-parameter    reload specified config parameter
  --numeric          -n                     list the table in numeric"

if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM --help"
        exit 1
fi

exit 0

