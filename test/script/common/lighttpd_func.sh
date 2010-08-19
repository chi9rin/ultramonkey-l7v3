#!/bin/bash

ENV_CONF_FILE="${CONF_DIR}/server_env.cf"
DEFAULT_CONF_DIR="${CONF_DIR}/lighttpd"
LIGHTTPD_TMP_DIR="/tmp/umtest$DATE"

clean_lighttpd_tmpdir (){
	if [ -d ${LIGHTTPD_TMP_DIR} ]
	then
		rm -rf ${LIGHTTPD_TMP_DIR} > /dev/null 2>&1
		RET=$?
	fi
	return ${RET}
}

up_lighttpd (){
	if [ -n "$1" -a -f "${LIGHTTPD_TMP_DIR}/$1.cf" ]
	then
		${LIGHTTPD} -f "${LIGHTTPD_TMP_DIR}/$1.cf" > /dev/null 2>&1
		RET=$?
	fi
	return ${RET}
}

stop_lighttpd (){
	PID=`ps aux | grep "${LIGHTTPD} -f ${LIGHTTPD_TMP_DIR}/$1.cf" | grep -v grep | awk '{print $2}'`
	if [ -n "$PID" ]
	then
		kill $PID > /dev/null 2>&1
		RET=$?
	fi
	PID=""
	return ${RET}
}

stop_all_lighttpd (){
	killall lighttpd > /dev/null 2>&1
	return $?
}

#make tmp dir
make_lighttpd_tmpdir (){
	if [ ! -d $LIGHTTPD_TMP_DIR ]
	then
		mkdir ${LIGHTTPD_TMP_DIR}
	else
		LOG_WARN "${LIGHTTPD_TMP_DIR} is already exist."
	fi
}

#make lighhtpd_configfile
start_lighttpd (){
#Set default value
template_file=${DEFAULT_CONF_DIR}/default.conf
server_ipaddr=127.0.0.1
OPTIND=1
while getopts s:a:p:t:iS option
do
	case "$option" in
	  s)
		LIGHTTPD_CONF_FILE="${LIGHTTPD_TMP_DIR}/${OPTARG}.cf"
		if [ -e ${LIGHTTPD_CONF_FILE} ]
		then
			echo "SERVERNAME:${OPTARG} is already exist."
			return 1
		else
			SERVERNAME=${OPTARG}
			#declare	${OPTARG}=${OPTARG}
			echo "server.name	= \"${OPTARG}\"" >> ${LIGHTTPD_CONF_FILE}
			echo "server.pid-file	= \"${LIGHTTPD_TMP_DIR}/$OPTARG.pid\"" >> ${LIGHTTPD_CONF_FILE}
			echo "server.document-root	= \"${LIGHTTPD_TMP_DIR}/${OPTARG}\"" >> ${LIGHTTPD_CONF_FILE}
			mkdir ${LIGHTTPD_TMP_DIR}/${OPTARG}
			echo "${OPTARG}" > ${LIGHTTPD_TMP_DIR}/${OPTARG}/index.html
		fi
		;;
	  t)
		template_file=${DEFAULT_CONF_DIR}/${OPTARG}
		;;

	  a)
		server_ipaddr=${OPTARG}
		;;
	  p)
		echo "server.port	= \"${OPTARG}\"" >> ${LIGHTTPD_CONF_FILE}
		;;
	  i)
		echo "server.use-ipv6	= \"enable\"" >> ${LIGHTTPD_CONF_FILE}
		;;
	  S)
		echo "ssl.engine	= \"enable\"" >> ${LIGHTTPD_CONF_FILE}
		echo "ssl.pemfile	= \"${LIGHTTPD_TMP_DIR}/${SERVERNAME}/lighttpd.pem\"" >> ${LIGHTTPD_CONF_FILE}
		cp ${DEFAULT_CONF_DIR}/lighttpd.pem ${LIGHTTPD_TMP_DIR}/${SERVERNAME}/lighttpd.pem
		;;
	  \?)
		:
		;;
	esac
done

echo "server.bind	= \"${server_ipaddr}\"" >>  ${LIGHTTPD_CONF_FILE}
cat ${template_file} >> ${LIGHTTPD_CONF_FILE}

up_lighttpd $SERVERNAME
return $?
}

