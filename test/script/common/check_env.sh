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

# check Lighty installed
check_http_server (){
        LIGHTTPD="/usr/sbin/lighttpd"
	INIT_LIGHTTPD="/etc/init.d/lighttpd"
	if [ ! -e ${LIGHTTPD} ]
	then
		LOG_FATAL "${LIGHTTPD} not exist. "
	fi

	if [ !  -e ${INIT_LIGHTTPD} ]
        then
                LOG_FATAL "${INIT_LIGHTTPD} not exist. "
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
		LOG_FATAL "${TEST_CLIENT} not exist."
		exit 1
	fi
}
#check_env main
LOG "check_env start."
check_uml7
check_http_server
check_commands
check_original_tools

LOG "check_env end."
