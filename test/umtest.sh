#!/bin/bash

# SET PATH
TEST_DIR=$(cd $(dirname "$0") && pwd)
DATE=`date +'%Y%m%d-%H%M%S'`

CONF_DIR="${TEST_DIR}/config"
SCRIPT_DIR="${TEST_DIR}/script"
COMMON_SCRIPT_DIR="${SCRIPT_DIR}/common"

CHECK_ENV="${COMMON_SCRIPT_DIR}/check_env.sh"
LOGGER="${COMMON_SCRIPT_DIR}/logger.sh"
LIGHTTPD_FUNC="${COMMON_SCRIPT_DIR}/lighttpd_func.sh"
STOP_MONKEY="${COMMON_SCRIPT_DIR}/stop_um_ps.sh"
SAVE_FILE="${COMMON_SCRIPT_DIR}/save_file.sh"
RETURN_FILE="${COMMON_SCRIPT_DIR}/return_file.sh"
COLLECT_FILE="${COMMON_SCRIPT_DIR}/collect_file.sh"
SET_DEFAULT_CONF="${COMMON_SCRIPT_DIR}/set_default_conf.sh"

if [ ! -d "$TEST_DIR" ]
then
	echo "Please set TEST_DIR."
	exit 1
fi

# Make work directory
LOG_BASE_DIR="${TEST_DIR}/log"
if [ ! -d ${LOG_BASE_DIR} ]
then
	mkdir ${LOG_BASE_DIR}
fi

TMP_DIR="${TEST_DIR}/work"
if [ ! -d ${TMP_DIR} ]
then
	mkdir ${TMP_DIR}
fi

REPORT_DIR="${TEST_DIR}/report"
REPORT_FILE="${REPORT_DIR}/${DATE}_report"
if [ ! -d ${REPORT_DIR} ]
then
	mkdir ${REPORT_DIR}
fi

# Functions
usage (){
	echo "usage : umtest [DIR|TESTSCRIPT]"
}

check_option (){
	while getopts hC option
	do
		case "$option" in
		  h)
			usage
			exit 0
			;;
		  C)
			rm -rf ${REPORT_DIR}
			rm -rf ${LOG_BASE_DIR}
			rm -rf ${TMP_DIR}
			exit 0
			;;
                  \?)
			usage
			exit 1
			;;
		esac
		shift
	done
	shift `expr "$OPTIND" - 1`
}

# main function
# $1 dirname
# $2 filename
um_test (){
	if [ ! -d "$1" ]
	then
		LOG_WARN "$1 not exist."
		return
	fi
	KIND=`basename $1`
	if [ $KIND == "common" -o $KIND == "materials" ]
	then
		return
	fi

	echo -e "\n[$KIND]" | tee -a ${REPORT_FILE}
	EVIDENCE_DIR="${LOG_DIR}/${KIND}"
	LOG "Make evidence directory ${EVIDENCE_DIR}."
	mkdir -p ${EVIDENCE_DIR}
	SCRIPT_LIST=`ls $1 | sort -n -t - -k2 -k3 -k4 -k5`
	for SCRIPT_NAME in ${2:-${SCRIPT_LIST}}
	do
		SCRIPT=$1/${SCRIPT_NAME}
		if [ -d ${SCRIPT} ]
		then
			( um_test ${SCRIPT} )
		elif [ -x ${SCRIPT} ]
		then
			SCRIPT_NAME=`basename ${SCRIPT}`
			make_lighttpd_tmpdir
			# Execute script
			LOG "Execute ${SCRIPT_NAME} ."
			(
				cd $1
				. ${SCRIPT}
			) 2> /dev/null 1> ${TMP_DIR}/umtest_tmp
			# Write report.
			if [ $? -eq 0 ]
			then
				echo -e "${SCRIPT_NAME}\tOK" | tee -a ${REPORT_FILE}
			else
				echo -e "${SCRIPT_NAME}\tNG" | tee -a ${REPORT_FILE}
				cat ${TMP_DIR}/umtest_tmp | tee -a ${REPORT_FILE}
			fi
			# Stop UltraMonkey-L7.
			. ${STOP_MONKEY}
			# Collect logs.
			TAR_DIR=${EVIDENCE_DIR}/`echo "${SCRIPT_NAME}" | cut -d "." -f 1`
			mkdir ${TAR_DIR}
			. ${COLLECT_FILE}
			# tar zip
			# tar cfz dist.tar.gz targetfolder
			# Stop HTTP server.
			stop_all_lighttpd	
			clean_lighttpd_tmpdir
		else
			LOG_WARN "${SCRIPT} cannot execute."
			continue
		fi
	done
#done
}

####################
#Check option
####################
check_option "$@"

####################
# Pretreatment
####################

# Logger
. ${LOGGER}
# Check environment.
. ${CHECK_ENV}
# Stop UltraMonkey-L7.
. ${STOP_MONKEY}
# Save log and config files.
. ${SAVE_FILE}
# Initialize environment.
. ${LIGHTTPD_FUNC}


###################
# Test
###################
LOG "Execute test scripts."
for ARG in ${@:-${SCRIPT_DIR}}
do
	ABSPATH=$(cd $(dirname "$ARG") && pwd)/$(basename "$ARG")
	if [ -d ${ABSPATH} ]
	then
		um_test ${ABSPATH}
	elif [ -x ${ABSPATH} ]
	then
		um_test `dirname ${ABSPATH}` `basename ${ABSPATH}`
	else
		LOG_WARN "${ABSPATH} cannot execute."
	fi
done

# Count OK and NG
echo "############# Summary ###############" | tee -a ${REPORT_FILE}
grep -v "^Test failed" ${REPORT_FILE} | 
awk 'BEGIN{OkCon=0;NgCon=0;}
     {if($2 == "OK")
	 OkCon++;
      if($2 == "NG")
	 NgCon++;}
     END{printf("OK=%d\tNG=%d\n",OkCon,NgCon)}' | tee -a ${REPORT_FILE}
LOG "Test scripts end."
###################
# Aftertreatment
###################
# Stop UltraMonkey-L7.
. ${STOP_MONKEY}
# Return log and config files.
. ${RETURN_FILE}

exit 0
