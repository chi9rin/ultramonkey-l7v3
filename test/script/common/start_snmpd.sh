#!/bin/bash

LOG "start_snmpd start."

if [ -z "`pgrep snmpd`" ]
then
	${INIT_SNMPD} start > /dev/null 2>&1
	if [ $? -ne 0 ]
	then
		LOG_WARN "Can not start snmpd."
	else
		LOG "Starting snmpd done."
	fi
else
	LOG "snmpd is running."
fi

if [ -n "`pgrep snmptrapd`" ]
then
	killall ${SNMPTRAPD}
	if [ $? -ne 0 ]
	then
		LOG_WARN "Can not stop snmptrapd."
	else
		LOG "Stopping l7vsd done."
	fi
fi

LOG "start_snmpd end."
