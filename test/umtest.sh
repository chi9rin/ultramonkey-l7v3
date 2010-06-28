#!/bin/bash

# 基本設定
TEST_DIR="/home/hibari/test"
DATE=`date +'%Y-%m-%d(%Hh%Mm)'`

CONF_DIR="${TEST_DIR}/config"
SCRIPT_DIR="${TEST_DIR}/script"
COMMON_SCRIPT_DIR="${SCRIPT_DIR}/common"
LOG_DIR="${TEST_DIR}/log/$DATE"
LOG_FILE="${LOG_DIR}/${DATE}.log"
TMP_DIR="${TEST_DIR}/work"
REPORT_DIR="${TEST_DIR}/report"
REPORT_FILE="${REPORT_DIR}/${DATE}.rep"

ENV_CONF="${CONF_DIR}/env_conf.sh"
CHECK_ENV="${COMMON_SCRIPT_DIR}/check_env.sh"
INIT_ENV="${COMMON_SCRIPT_DIR}/init_env.sh"
LOGGER="${COMMON_SCRIPT_DIR}/logger.sh"
STOP_MONKEY="${COMMON_SCRIPT_DIR}/stop_um_ps.sh"
SAVE_FILE="${COMMON_SCRIPT_DIR}/save_file.sh"
RETURN_FILE="${COMMON_SCRIPT_DIR}/return_file.sh"
COLLECT_FILE="${COMMON_SCRIPT_DIR}/collect_file.sh"

L7VSD="/usr/sbin/l7vsd"
L7VSADM="/usr/sbin/l7vsadm"
L7DIRECTORD="/usr/sbin/l7directord"
INIT_L7VSD="/etc/init.d/l7vsd"
INIT_L7DIRECTORD="/etc/init.d/l7directord"
L7VSD_CONF_DIR="/etc/l7vs"
L7DIRECTORD_CONF_DIR="/etc/ha.d/conf"
L7VS_LOG_DIR="/var/log/l7vs"

####################
# 前処理
####################
cd ${TEST_DIR}

# Loggerの準備
. ${LOGGER}
# 環境設定ファイルの読み込み
. ${ENV_CONF}
# 環境チェック
. ${CHECK_ENV}
# l7vsd,l7directordプロセス停止
. ${STOP_MONKEY}
# コンフィグファイルとログファイル退避
. ${SAVE_FILE}

# 試験環境初期化
#　ここで、httpサーバ立ち上げたりする
#. ${INIT_ENV}

###################
# 試験実施
###################

# メモ　後で消すこと↓
#コンフィグファイルの配置はスクリプトの中で行う

for KIND in `cat ${CONF_DIR}/testlist.cf | sed '/^ *$/d' | grep -v "^#"`
do
        EVIDENCE_DIR="${LOG_DIR}/${KIND}"
        mkdir ${EVIDENCE_DIR}

        cd ${SCRIPT_DIR}/${KIND}
        for SCRIPT in *.sh *.pl *.py
        do
                if [ -f ${SCRIPT} ]
                then
                # スクリプト実行
                (
                . ${SCRIPT}
                ) > /dev/null 2>&1
                # レポート記入
                if [ $? -eq 0 ]
                then
                        echo -e "${KIND}\t${SCRIPT}\tOK" | tee -a ${REPORT_FILE}
                else
                        echo -e "${KIND}\t${SCRIPT}\tNG" | tee -a ${REPORT_FILE}
                fi
                # l7vsd,l7directordプロセス停止
                . ${STOP_MONKEY}
                # ログ収集
                TAR_DIR=${EVIDENCE_DIR}/`echo "${SCRIPT}" | cut -d "." -f 1`
                mkdir ${TAR_DIR}
                . ${COLLECT_FILE}
                #tarで圧縮
                # tar cfz dist.tar.gz targetfolder
                fi
        done
done
cd ${TEST_DIR}
###################
# 後処理
###################
# l7vsd,l7directordプロセス停止
. ${STOP_MONKEY}
# ログファイル・コンフィグファイル復帰
. ${RETURN_FILE}
