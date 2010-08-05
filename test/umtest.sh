#!/bin/bash

# SET PATH
TEST_DIR="dirname $0"
DATE=`date +'%Y-%m-%d(%Hh%Mm%Ss)'`

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
REPORT_FILE="${REPORT_DIR}/${DATE}.rep"
if [ ! -d ${REPORT_DIR} ]
then
	mkdir ${REPORT_DIR}
fi

# Functions
usage (){
	echo "usage : umtest [dirname [TestScriptName]]"
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

#Check option
check_option "$@"

####################
# Pretreatment
####################
cd ${TEST_DIR}

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
for KIND in ${1:-`ls ${SCRIPT_DIR}`}
do
	if [ ! -d "${SCRIPT_DIR}/${KIND}" ]
	then
		LOG_WARN "${SCRIPT_DIR}/${KIND} not exist."
		continue
	fi
	if [ $KIND == "common" ]
	then
		continue
	fi
	EVIDENCE_DIR="${LOG_DIR}/${KIND}"
	LOG "Make evidence directory ${EVIDENCE_DIR}."
	mkdir -p ${EVIDENCE_DIR}
	cd ${SCRIPT_DIR}/${KIND}
	for SCRIPT in ${2:-*}
	do
		if [ -x ${SCRIPT} ]
		then
			make_lighttpd_tmpdir
			# Execute script
			LOG "Execute ${SCRIPT} ."
			(
				. ${SCRIPT}
			) 2> /dev/null 1> ${TMP_DIR}/tmp
			# Write report.
			if [ $? -eq 0 ]
			then
				echo -e "${KIND}\t${SCRIPT}\tOK" | tee -a ${REPORT_FILE}
			else
				echo -e "${KIND}\t${SCRIPT}\tNG" | tee -a ${REPORT_FILE}
				cat ${TMP_DIR}/tmp | tee -a ${REPORT_FILE}
			fi
			# Stop UltraMonkey-L7.
			. ${STOP_MONKEY}
			# Collect logs.
			TAR_DIR=${EVIDENCE_DIR}/`echo "${SCRIPT}" | cut -d "." -f 1`
			mkdir ${TAR_DIR}
			. ${COLLECT_FILE}
			#tarで圧縮
			# tar cfz dist.tar.gz targetfolder
			# Stop HTTP server.
			stop_all_lighttpd	
			clean_lighttpd_tmpdir
		else
			LOG_WARN "${SCRIPT_DIR}/${KIND}/${SCRIPT} cannot execute."
			continue
		fi
	done
done
# Count OK and NG
echo "############# Summary ###############" | tee -a ${REPORT_FILE}
grep -v "^Test failed" ${REPORT_FILE} | 
awk 'BEGIN{OkCon=0;NgCon=0;}
     {if($3 == "OK")
	 OkCon++;
      if($3 == "NG")
	 NgCon++;}
     END{printf("OK=%d\tNG=%d\n",OkCon,NgCon)}' | tee -a ${REPORT_FILE}
LOG "Test scripts end."
cd ${TEST_DIR}
###################
# Aftertreatment
###################
# Stop UltraMonkey-L7.
. ${STOP_MONKEY}
# Return log and config files.
. ${RETURN_FILE}

