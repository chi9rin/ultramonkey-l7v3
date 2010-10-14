#!/bin/bash

. ${SET_DEFAULT_CONF}

# start l7vsd
start_l7vsd || exit 1

# set virtual service log level to debug
l7vsadm -L -c vs -l debug

# create session thread
l7vsadm -A -t 0:22222 -m sessionless

# about ...
session_thread_pid=`expr \`pidof l7vsd\` + 30`

# get cpumask
cpumask=`taskset -p $session_thread_pid | awk '{print $6}'`

stop_l7vsd || exit 1

if [ "$cpumask" != "f" ]; then
	echo "Test failed: Default cpu mask should be 'f'... but '$cpumask'"
	exit 1
fi

sleep 1

\cp -f materials/virtualservice-61-l7vs.cf ${L7VSD_CONF_DIR}/l7vs.cf

# start l7vsd
start_l7vsd || exit 1

# set virtual service log level to debug
l7vsadm -L -c vs -l debug

# create session thread
l7vsadm -A -t 0:22222 -m sessionless

# about ...
session_thread_pid=`expr \`pidof l7vsd\` + 30`

# get cpumask
cpumask=`taskset -p $session_thread_pid | awk '{print $6}'`

stop_l7vsd || exit 1

if [ "$cpumask" != "1" ]; then
	echo "Test failed: Cpu mask must be '1'... but '$cpumask'"
	exit 1
fi

exit 0
