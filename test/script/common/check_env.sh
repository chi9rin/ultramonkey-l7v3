#!/bin/bash

# Check UltraMonkey-L7 installed
check_uml7 (){
        L7VSD="/usr/sbin/l7vsd"
	L7VSADM="/usr/sbin/l7vsadm"
	L7DIRECTORD="/usr/sbin/l7directord"
	INIT_L7VSD="/etc/init.d/l7vsd"
	INIT_L7DIRECTORD="/etc/init.d/l7directord"
	L7VSD_CONF_DIR="/etc/l7vs"
	L7DIRECTORD_CONF_DIR="/etc/ha.d/conf"
	L7VS_LOG_DIR="/var/log/l7vs"

	if [ !  -e ${L7VSD} ]
	then
		LOG_FATAL "${L7VSD} not exist. " 
		exit 1
	fi

	if [ !  -e ${L7VSADM} ]
	then       
        	LOG_FATAL "${L7VSADM} not exist." 
	        exit 1
	fi
	
	if [ !  -e ${L7DIRECTORD} ]
	then       
        	LOG_FATAL "${L7DIRECTORD} not exist." 
        	exit 1
	fi

	if [ !  -e ${INIT_L7VSD} ]
	then
        	LOG_FATAL "${INIT_L7VSD} not exist. " 
        	exit 1
	fi          

	if [ !  -e ${INIT_L7DIRECTORD} ]
	then       
        	LOG_FATAL "${INIT_L7DIRECTORD} not exist." 
        	exit 1
	fi

	if [ !  -d ${L7VSD_CONF_DIR} ]
	then       
        	LOG_WARN "${L7VSD_CONF_DIR} not exist. Create ${L7VSD_CONF_DIR} ." 
        	mkdir ${L7VSD_CONF_DIR}
	fi

	if [ !  -d ${L7DIRECTORD_CONF_DIR} ]
	then
		LOG_WARM "${L7DIRECTORD_CONF_DIR} not exist. Create ${L7DIRECTORD_CONF_DIR} ." 
        	mkdir ${L7DIRECTORD_CONF_DIR}
	fi

	if [ !  -d ${L7VS_LOG_DIR} ]
	then
		LOG_WARN "${L7VS_LOG_DIR} not exist. Create ${L7VS_LOG_DIR} ." 
        	mkdir ${L7VS_LOG_DIR}
	fi
}

# check lighttpd installed
check_http_server (){
        LIGHTTPD=`which lighttpd`
	if [ $? -ne 0 ]
	then
		LOG_FATAL "lighttpd not exist. "
                exit 1
	fi
}

# check Net-snmp installed
check_net_snmp (){
	SNMPD=`which snmpd`
	if [ $? -ne 0 ]
	then
		LOG_FATAL "snmpd not exist."
		exit 1
	fi

	INIT_SNMPD="/etc/init.d/snmpd"
	if [ ! -e ${INIT_SNMPD} ]
	then       
        	LOG_FATAL "/etc/init.d/snmpd not exist." 
        	exit 1
	fi
	
	SNMPTRAPD=`which snmptrapd`
	if [ $? -ne 0 ]
	then
		LOG_FATAL "snmptrapd not exist."
		exit 1
	fi

	INIT_SNMPTRAPD="/etc/init.d/snmptrapd"
	if [ ! -e ${INIT_SNMPTRAPD} ]
	then       
        	LOG_FATAL "/etc/init.d/snmptrapd not exist." 
        	exit 1
	fi
}

	
# check commands installed
check_commands (){
	WGET="/usr/bin/wget"
	if [ ! -e ${WGET} ]
	then
		LOG_FATAL "${WGET} not exist. "
		exit 1
	fi

	PERL="/usr/bin/perl"
	if [ ! -e ${PERL} ]
	then
		LOG_FATAL "${PERL} not exist."
		exit 1
	fi

	PYTHON="/usr/bin/python"
	if [ ! -e ${PYTHON} ]
	then
		LOG_FATAL "${PYTHON} not exist."
		exit 1
	fi
}

# check original tools
check_original_tools (){
	TEST_CLIENT="${COMMON_SCRIPT_DIR}/test_client"
	if [ ! -e ${TEST_CLIENT} ]
	then
		if [ -e "${TEST_CLIENT}.c" ]
		then
			gcc -g -o ${TEST_CLIENT} ${TEST_CLIENT}.c
		else
			LOG_FATAL "${TEST_CLIENT} not exist."
			exit 1
		fi
		if [ ! -e ${TEST_CLIENT} ]
		then
			LOG_FATAL "${TEST_CLIENT} not exist."
			exit 1
		fi
	fi
}
#check_env main
LOG "check_env start."
check_uml7
check_http_server
check_net_snmp
check_commands
check_original_tools

LOG "check_env end."
