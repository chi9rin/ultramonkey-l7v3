

#include <iostream>
//#include "../../../l7vsd/src/logger_impl.cpp"
#include <boost/test/unit_test.hpp>

#include "./parameter_stub/parameter_impl.h"
//#include "./parameter_stub/parameter_impl.cpp"
#include "./parameter_stub/parameter.h"
//#include "./parameter_stub/parameter.cpp"

#include "../../src/logger_impl.cpp"
#include "../../src/logger.cpp"

#include <log4cxx/logmanager.h>
#include <log4cxx/helpers/loglog.h>
#include <log4cxx/rolling/rollingfileappender.h>
#include <log4cxx/rolling/fixedwindowrollingpolicy.h>
#include <log4cxx/rolling/sizebasedtriggeringpolicy.h>
#include <log4cxx/rolling/timebasedrollingpolicy.h>
#include <log4cxx/consoleappender.h>
#include <log4cxx/appender.h>
#include <log4cxx/logmanager.h>
#include <log4cxx/patternlayout.h>
#include <log4cxx/fileappender.h>
#include <log4cxx/net/syslogappender.h>
#include <log4cxx/hierarchy.h>

#include <dlfcn.h>
#include <limits.h>
#include "./strict_time_based_rolling_policy_stub.cpp"
#include "./time_and_size_based_rolling_policy_stub.cpp"
#include "./logger_implement_access_stub/logger_implement_access.h"
#include "logger_enum.h"
//#include "logger_access_manager.h"
#include "../../src/logger_logrotate_utility.cpp"


#include "../../src/logger_access_manager.cpp"


#include <boost/test/included/unit_test.hpp>


using namespace boost::unit_test_framework;

using namespace l7vs;

using namespace std;


using namespace boost::unit_test_framework;

using namespace l7vs;

extern void *_start;    //開始位置
extern void *_etext;    //終了位置

#define BUF_LEN (4096)

typedef std::map< LOG_CATEGORY_TAG, LOG_LEVEL_TAG>
category_level_map_type;


int resetConfigurationCalled = 0;

int getRootLoggerCalled = 0;
bool getRootLoggerReturnNull = false;

int patternLayoutCtorCalled = 0;
void *patternLayoutCreated = 0;
bool patternLayoutNewFail = false;

int fileAppenderCtorCalled = 0;
void *fileAppenderCreated = 0;
bool fileAppenderNewFail = false;
std::string fileAppenderCtorArgFilename = "";
log4cxx::LayoutPtr fileAppenderCtorArgLayout = 0;
int fileAppenderSetFileCalled = 0;
std::string fileAppenderSetFileArgFilename = "";
bool fileAppenderSetFileArgAppend = false;
bool fileAppenderSetFileArgBufferedIO = false;
size_t fileAppenderSetFileArgBufferSize = 0;
void *fileAppenderSetFileArgPool = 0;
bool fileAppenderSetFileException = false;

int poolCtorCalled = 0;
void *poolCreated = 0;

int syslogAppenderCtorCalled = 0;
void *syslogAppenderCreated = 0;
bool syslogAppenderNewFail = false;
log4cxx::LayoutPtr syslogAppenderCtorArgLayout = 0;
int syslogAppenderCtorArgFacility = 0;

int loggerAddAppenderCalled = 0;
log4cxx::AppenderPtr loggerAddAppenderArg[2] = {0, 0};

int loggerForcedLogCalled = 0;
log4cxx::LevelPtr loggerForcedLogArgLevel = 0;
std::string loggerForcedLogArgMessage = "";
char loggerForcedLogArgFile[256];
int loggerForcedLogArgLine = 0;
bool loggerForcedLogEnabled = true;
bool loggerForcedLogException = false;

int loggerGetLoggerCalled = 0;
std::string loggerGetLoggerArgName = "";
bool loggerGetLoggerReturnNull = false;

int fixedWindowCtorCalled = 0;
void *fixedWindowCreated = 0;
bool fixedWindowNewFail = false;

int sizeBaseCtorCalled = 0;
void *sizeBaseCreated = 0;
bool sizeBaseNewFail = false;

int strictPolicyCtorCalled = 0;
void *strictPolicyCreated = 0;
bool strictPolicyNewFail = false;

int timeSizePolicyCtorCalled = 0;
void *timeSizePolicyCreated = 0;
bool timeSizePolicyNewFail = false;

int rollingAppenderCtorCalled = 0;
void *rollingAppenderCreated = 0;
bool rollingAppenderNewFail = false;

int loggerGetLevelCalled = 0;
int loggerSetLevelCalled = 0;

int snmpChangeLevelCalled = 0;
int snmpChangeLevelArgCat = 0;
int snmpChangeLevelArgLevel = 0;

int snmpGetLevelCalled = 0;
LOG_CATEGORY_TAG snmpGetLevelArgCat;
LOG_LEVEL_TAG snmpGetLevelReturns;

int appenderSkeletonSetLayoutCalled = 0;
void *appenderSkeletonSetLayoutArgLayout = 0;

int rollingAppenderSkeletonActivateOptionsCalled = 0;

int hierarchySetConfiguredCalled = 0;


#define BUF_LEN (4096)

typedef std::map< std::string , logger_implement_access * >
logimp_access_map_type;

typedef std::map< std::string , std::string >
accesslog_rotate_default_map_type;

typedef std::map< std::string , std::string >::iterator
accesslog_rotate_default_map_type_iterator;

typedef std::map< std::string , std::string > accesslog_rotate_map_type;

typedef std::map< std::string , std::string >::iterator accesslog_rotate_map_type_iterator;

class logger_access_manager_test : public l7vs::logger_access_manager
{
public:
        logimp_access_map_type &get_logimp_access_map() {
                return(logimp_access_map);
        }

        appender_property &get_access_log_default_property() {
                return(access_log_default_property);
        }

        void get_access_log_default_property_init() {
                access_log_default_property.log_filename_key                = "";
                access_log_default_property.log_filename_value              = "";
                access_log_default_property.rotation_key                    = "";
                access_log_default_property.rotation_value                  = LOG_ROT_SIZE;
                access_log_default_property.max_backup_index_key            = "";
                access_log_default_property.max_backup_index_value          = 1;
                access_log_default_property.max_file_size_key               = "";
                access_log_default_property.max_file_size_value             = 1;
                access_log_default_property.rotation_timing_key             = "";
                access_log_default_property.rotation_timing_value           = LOG_TIM_YEAR;
                access_log_default_property.rotation_timing_value_key       = "";
                access_log_default_property.rotation_timing_value_value     = "";
        }

        accesslog_rotate_default_map_type &get_access_log_default_data() {
                return(access_log_default_data);
        }

        bool get_rotate_default_load_flag() {
                return(rotate_default_load_flag);
        }

        void set_rotate_default_load_flag(bool in) {
                rotate_default_load_flag = in;
        }

        std::string &call_get_rotate_default_verbose_displayed_contents() {
                return(rotate_default_verbose_displayed_contents);
        }

        void set_rotate_default_verbose_displayed_contents(std::string in) {
                rotate_default_verbose_displayed_contents = in;
        }
        /*
            logger_implement_access *find_logger_implement_access(
                                        const std::string &access_log_filename,
                                        std::map< std::string , std::string > rotatedata,
                                        l7vs::error_code& err)
            {
                return( logger_access_manager::getInstance().find_logger_implement_access(
                                        access_log_filename,
                                        rotatedata,
                                        err) );
            }

            void erase_logger_implement_access(
                                    const std::string &access_log_filename,
                                    l7vs::error_code& err)
            {
                logger_access_manager::getInstance().erase_logger_implement_access(
                                    access_log_filename,
                                    err);
            }

            void access_log_rotate_loadConf()
            {
                logger_access_manager::access_log_rotate_loadConf();
            }

                static  logger_access_manager_test&     get_instance(){
                        static logger_access_manager_test       instance;
                        return instance;
                }
        */
        logger_access_manager_test() {}
        //~logger_access_manager_test() {}

};



void logger_access_manager_access_log_rotate_loadConf_test()
{

        BOOST_MESSAGE("----- logger_access_manager_access_log_rotate_loadConf_test start -----");

        logger_access_manager_test implTest;

        Parameter pm;


        std::cout << "access_log_rotate_loadConf normal case01 size" << std::endl;
        {
                accesslog_rotate_default_map_type rotatedata;
                l7vs::error_code vs_err;

                std::string no_thorw_error_msg = "exception throw!!";

                std::string       l7cf_file_path = "./test_cf_files/l7vs.cf";

                std::string       l7vsd_accesslog_rotate_type_val = "size";
                //std::string       l7vsd_accesslog_rotate_type_val = "date";
                //std::string       l7vsd_accesslog_rotate_type_val = "datesize";
                std::string       l7vsd_accesslog_rotate_max_backup_index_val = "1";
                std::string       l7vsd_accesslog_rotate_max_filesize_val = "10M";
                //std::string       l7vsd_accesslog_rotate_rotation_timing_val = "year";
                //std::string       l7vsd_accesslog_rotation_timing_value_val = "05/05 01:31";


                pm.setStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_ROTATION_KEY, l7vsd_accesslog_rotate_type_val);
                pm.setStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_MAX_BACKUP_INDEX_KEY, l7vsd_accesslog_rotate_max_backup_index_val);
                pm.setStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_MAX_FILE_SIZE_KEY, l7vsd_accesslog_rotate_max_filesize_val);

                try {

                        implTest.access_log_rotate_loadConf();
                        // unit_test[1] エラーが発生していないこと
                        std::cout << "access_log_rotate_loadConf succeed\n";
                } catch (const std::logic_error &ex) {
                        BOOST_ERROR(no_thorw_error_msg);
                }

                std::string result_msg = "--ac-rotate-type size --ac-rotate-max-backup-index 1 --ac-rotate-max-filesize 10M";

                // unit_test[2] エラーが発生していないこと
                BOOST_CHECK(implTest.call_get_rotate_default_verbose_displayed_contents() == result_msg);

                pm.deleteStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_ROTATION_KEY);
                pm.deleteStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_MAX_BACKUP_INDEX_KEY);
                pm.deleteStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_MAX_FILE_SIZE_KEY);

//        std::string result_msg = "--ac-rotate-type date --ac-rotate-max-backup-index 1 --ac-rotate-rotation-timing year --ac-rotate-rotation-timing-value 05/05 01:31";
//        std::string result_msg = "--ac-rotate-type datesize --ac-rotate-max-backup-index 1 --ac-rotate-max-filesize 10M --ac-rotate-rotation-timing year --ac-rotate-rotation-timing-value 05/05 01:31";

        }

        std::cout << "access_log_rotate_loadConf normal case02 date" << std::endl;
        {
                accesslog_rotate_default_map_type rotatedata;
                l7vs::error_code vs_err;

                std::string no_thorw_error_msg = "exception throw!!";

                std::string       l7cf_file_path = "./test_cf_files/l7vs.cf";

                //std::string       l7vsd_accesslog_rotate_type_val = "size";
                std::string       l7vsd_accesslog_rotate_type_val = "date";
                //std::string       l7vsd_accesslog_rotate_type_val = "datesize";
                std::string       l7vsd_accesslog_rotate_max_backup_index_val = "1";
                //std::string       l7vsd_accesslog_rotate_max_filesize_val = "10M";
                std::string       l7vsd_accesslog_rotate_rotation_timing_val = "year";
                std::string       l7vsd_accesslog_rotation_timing_value_val = "05/05 01:31";


                pm.setStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_ROTATION_KEY, l7vsd_accesslog_rotate_type_val);
                pm.setStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_MAX_BACKUP_INDEX_KEY, l7vsd_accesslog_rotate_max_backup_index_val);
                pm.setStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_ROTATION_TIMING_KEY, l7vsd_accesslog_rotate_rotation_timing_val);
                pm.setStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_ROTATION_TIMING_VALUE_KEY, l7vsd_accesslog_rotation_timing_value_val);


                try {

                        implTest.access_log_rotate_loadConf();
                        // unit_test[3] エラーが発生していないこと
                        std::cout << "access_log_rotate_loadConf succeed\n";
                } catch (const std::logic_error &ex) {
                        BOOST_ERROR(no_thorw_error_msg);
                }

//        std::string result_msg = "--ac-rotate-type size --ac-rotate-max-backup-index 1 --ac-rotate-max-filesize 10M";
                std::string result_msg = "--ac-rotate-type date --ac-rotate-max-backup-index 1 --ac-rotate-rotation-timing year --ac-rotate-rotation-timing-value 05/05 01:31";

                // unit_test[4] エラーが発生していないこと
                BOOST_CHECK(implTest.call_get_rotate_default_verbose_displayed_contents() == result_msg);

                pm.deleteStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_ROTATION_KEY);
                pm.deleteStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_MAX_BACKUP_INDEX_KEY);
                pm.deleteStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_ROTATION_TIMING_KEY);
                pm.deleteStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_ROTATION_TIMING_VALUE_KEY);

//        std::string result_msg = "--ac-rotate-type datesize --ac-rotate-max-backup-index 1 --ac-rotate-max-filesize 10M --ac-rotate-rotation-timing year --ac-rotate-rotation-timing-value 05/05 01:31";

        }

        std::cout << "access_log_rotate_loadConf normal case03 datesize" << std::endl;
        {
                accesslog_rotate_default_map_type rotatedata;
                l7vs::error_code vs_err;

                std::string no_thorw_error_msg = "exception throw!!";

                std::string       l7cf_file_path = "./test_cf_files/l7vs.cf";

                //std::string       l7vsd_accesslog_rotate_type_val = "size";
                //std::string       l7vsd_accesslog_rotate_type_val = "date";
                std::string       l7vsd_accesslog_rotate_type_val = "datesize";
                std::string       l7vsd_accesslog_rotate_max_backup_index_val = "1";
                std::string       l7vsd_accesslog_rotate_max_filesize_val = "10M";
                std::string       l7vsd_accesslog_rotate_rotation_timing_val = "year";
                std::string       l7vsd_accesslog_rotation_timing_value_val = "05/05 01:31";


                pm.setStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_ROTATION_KEY, l7vsd_accesslog_rotate_type_val);
                pm.setStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_MAX_BACKUP_INDEX_KEY, l7vsd_accesslog_rotate_max_backup_index_val);
                pm.setStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_MAX_FILE_SIZE_KEY, l7vsd_accesslog_rotate_max_filesize_val);
                pm.setStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_ROTATION_TIMING_KEY, l7vsd_accesslog_rotate_rotation_timing_val);
                pm.setStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_ROTATION_TIMING_VALUE_KEY, l7vsd_accesslog_rotation_timing_value_val);


                try {

                        implTest.access_log_rotate_loadConf();
                        // unit_test[5] エラーが発生していないこと
                        std::cout << "access_log_rotate_loadConf succeed\n";
                } catch (const std::logic_error &ex) {
                        BOOST_ERROR(no_thorw_error_msg);
                }

//        std::string result_msg = "--ac-rotate-type size --ac-rotate-max-backup-index 1 --ac-rotate-max-filesize 10M";
                std::string result_msg = "--ac-rotate-type datesize --ac-rotate-max-backup-index 1 --ac-rotate-max-filesize 10M --ac-rotate-rotation-timing year --ac-rotate-rotation-timing-value 05/05 01:31";

                // unit_test[6] エラーが発生していないこと
                BOOST_CHECK(implTest.call_get_rotate_default_verbose_displayed_contents() == result_msg);

                pm.deleteStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_ROTATION_KEY);
                pm.deleteStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_MAX_BACKUP_INDEX_KEY);
                pm.deleteStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_MAX_FILE_SIZE_KEY);
                pm.deleteStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_ROTATION_TIMING_KEY);
                pm.deleteStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_ROTATION_TIMING_VALUE_KEY);

//        std::string result_msg = "--ac-rotate-type date --ac-rotate-max-backup-index 1 --ac-rotate-rotation-timing year --ac-rotate-rotation-timing-value 05/05 01:31";

        }

        std::cout << "access_log_rotate_loadConf err case01 rotate_type not set" << std::endl;
        {
                accesslog_rotate_default_map_type rotatedata;
                l7vs::error_code vs_err;

                std::string no_thorw_error_msg = "exception throw!!";

                std::string       l7cf_file_path = "./test_cf_files/l7vs.cf";

                //std::string       l7vsd_accesslog_rotate_type_val = "size";
                //std::string       l7vsd_accesslog_rotate_type_val = "date";
                //std::string       l7vsd_accesslog_rotate_type_val = "datesize";
                std::string       l7vsd_accesslog_rotate_max_backup_index_val = "1";
                std::string       l7vsd_accesslog_rotate_max_filesize_val = "10M";
                //std::string       l7vsd_accesslog_rotate_rotation_timing_val = "year";
                //std::string       l7vsd_accesslog_rotation_timing_value_val = "05/05 01:31";


                //pm.setStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_ROTATION_KEY, l7vsd_accesslog_rotate_type_val );
                pm.setStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_MAX_BACKUP_INDEX_KEY, l7vsd_accesslog_rotate_max_backup_index_val);
                pm.setStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_MAX_FILE_SIZE_KEY, l7vsd_accesslog_rotate_max_filesize_val);
                //pm.setStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_ROTATION_TIMING_KEY, l7vsd_accesslog_rotate_rotation_timing_val);
                //pm.setStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_ROTATION_TIMING_VALUE_KEY, l7vsd_accesslog_rotation_timing_value_val);


                try {

                        implTest.access_log_rotate_loadConf();
                        BOOST_ERROR(no_thorw_error_msg);
                } catch (const std::logic_error &ex) {
                        // unit_test[7] エラーが発生していること
                        BOOST_CHECK_EQUAL(ex.what(), "Not Exist Log Rotation Setting.");
                }

                //pm.deleteStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_ROTATION_KEY );
                pm.deleteStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_MAX_BACKUP_INDEX_KEY);
                pm.deleteStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_MAX_FILE_SIZE_KEY);
                //pm.deleteStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_ROTATION_TIMING_KEY );
                //pm.deleteStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_ROTATION_TIMING_VALUE_KEY );

        }

        std::cout << "access_log_rotate_loadConf err case02 max_backup_index not set" << std::endl;
        {
                accesslog_rotate_default_map_type rotatedata;
                l7vs::error_code vs_err;

                std::string no_thorw_error_msg = "exception throw!!";

                std::string       l7cf_file_path = "./test_cf_files/l7vs.cf";

                std::string       l7vsd_accesslog_rotate_type_val = "size";
                //std::string       l7vsd_accesslog_rotate_type_val = "date";
                //std::string       l7vsd_accesslog_rotate_type_val = "datesize";
                //std::string       l7vsd_accesslog_rotate_max_backup_index_val = "1";
                std::string       l7vsd_accesslog_rotate_max_filesize_val = "10M";
                //std::string       l7vsd_accesslog_rotate_rotation_timing_val = "year";
                //std::string       l7vsd_accesslog_rotation_timing_value_val = "05/05 01:31";



                pm.setStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_ROTATION_KEY, l7vsd_accesslog_rotate_type_val);
                //pm.setStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_MAX_BACKUP_INDEX_KEY, l7vsd_accesslog_rotate_max_backup_index_val);
                pm.setStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_MAX_FILE_SIZE_KEY, l7vsd_accesslog_rotate_max_filesize_val);
                //pm.setStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_ROTATION_TIMING_KEY, l7vsd_accesslog_rotate_rotation_timing_val);
                //pm.setStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_ROTATION_TIMING_VALUE_KEY, l7vsd_accesslog_rotation_timing_value_val);

                try {

                        implTest.access_log_rotate_loadConf();
                        BOOST_ERROR(no_thorw_error_msg);
                } catch (const std::logic_error &ex) {
                        // unit_test[8] エラーが発生していること
                        BOOST_CHECK_EQUAL(ex.what(), "Not Exist Log MaxBackupIndex Setting.");
                }

                pm.deleteStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_ROTATION_KEY);
                //pm.deleteStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_MAX_BACKUP_INDEX_KEY );
                pm.deleteStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_MAX_FILE_SIZE_KEY);
                //pm.deleteStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_ROTATION_TIMING_KEY );
                //pm.deleteStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_ROTATION_TIMING_VALUE_KEY );

        }

        std::cout << "access_log_rotate_loadConf err case03 max_filesize not set" << std::endl;
        {
                accesslog_rotate_default_map_type rotatedata;
                l7vs::error_code vs_err;

                std::string no_thorw_error_msg = "exception throw!!";

                std::string       l7cf_file_path = "./test_cf_files/l7vs.cf";

                std::string       l7vsd_accesslog_rotate_type_val = "size";
                //std::string       l7vsd_accesslog_rotate_type_val = "date";
                //std::string       l7vsd_accesslog_rotate_type_val = "datesize";
                std::string       l7vsd_accesslog_rotate_max_backup_index_val = "1";
                //std::string       l7vsd_accesslog_rotate_max_filesize_val = "10M";
                //std::string       l7vsd_accesslog_rotate_rotation_timing_val = "year";
                //std::string       l7vsd_accesslog_rotation_timing_value_val = "05/05 01:31";


                pm.setStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_ROTATION_KEY, l7vsd_accesslog_rotate_type_val);
                pm.setStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_MAX_BACKUP_INDEX_KEY, l7vsd_accesslog_rotate_max_backup_index_val);
                //pm.setStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_MAX_FILE_SIZE_KEY, l7vsd_accesslog_rotate_max_filesize_val);
                //pm.setStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_ROTATION_TIMING_KEY, l7vsd_accesslog_rotate_rotation_timing_val);
                //pm.setStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_ROTATION_TIMING_VALUE_KEY, l7vsd_accesslog_rotation_timing_value_val);

                try {

                        implTest.access_log_rotate_loadConf();
                        BOOST_ERROR(no_thorw_error_msg);
                } catch (const std::logic_error &ex) {
                        // unit_test[9] エラーが発生していること
                        BOOST_CHECK_EQUAL(ex.what(), "Not Exist Log MaxFileSize Setting.");
                }

                pm.deleteStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_ROTATION_KEY);
                pm.deleteStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_MAX_BACKUP_INDEX_KEY);
                //pm.deleteStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_MAX_FILE_SIZE_KEY );
                //pm.deleteStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_ROTATION_TIMING_KEY );
                //pm.deleteStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_ROTATION_TIMING_VALUE_KEY );

        }


        std::cout << "access_log_rotate_loadConf err case04 rotation_timing not set" << std::endl;
        {
                accesslog_rotate_default_map_type rotatedata;
                l7vs::error_code vs_err;

                std::string no_thorw_error_msg = "exception throw!!";

                std::string       l7cf_file_path = "./test_cf_files/l7vs.cf";

                //std::string       l7vsd_accesslog_rotate_type_val = "size";
                //std::string       l7vsd_accesslog_rotate_type_val = "date";
                std::string       l7vsd_accesslog_rotate_type_val = "datesize";
                std::string       l7vsd_accesslog_rotate_max_backup_index_val = "1";
                std::string       l7vsd_accesslog_rotate_max_filesize_val = "10M";
                //std::string       l7vsd_accesslog_rotate_rotation_timing_val = "year";
                std::string       l7vsd_accesslog_rotation_timing_value_val = "05/05 01:31";


                pm.setStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_ROTATION_KEY, l7vsd_accesslog_rotate_type_val);
                pm.setStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_MAX_BACKUP_INDEX_KEY, l7vsd_accesslog_rotate_max_backup_index_val);
                pm.setStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_MAX_FILE_SIZE_KEY, l7vsd_accesslog_rotate_max_filesize_val);
                //pm.setStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_ROTATION_TIMING_KEY, l7vsd_accesslog_rotate_rotation_timing_val);
                pm.setStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_ROTATION_TIMING_VALUE_KEY, l7vsd_accesslog_rotation_timing_value_val);


                try {

                        implTest.access_log_rotate_loadConf();
                        BOOST_ERROR(no_thorw_error_msg);
                } catch (const std::logic_error &ex) {
                        // unit_test[10] エラーが発生していること
                        BOOST_CHECK_EQUAL(ex.what(), "Not Exist Log RotaionTiming Setting.");
                }

                pm.deleteStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_ROTATION_KEY);
                pm.deleteStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_MAX_BACKUP_INDEX_KEY);
                pm.deleteStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_MAX_FILE_SIZE_KEY);
                //pm.deleteStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_ROTATION_TIMING_KEY );
                pm.deleteStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_ROTATION_TIMING_VALUE_KEY);

        }

        std::cout << "access_log_rotate_loadConf err case05 rotation_timing_val not set" << std::endl;
        {
                accesslog_rotate_default_map_type rotatedata;
                l7vs::error_code vs_err;

                std::string no_thorw_error_msg = "exception throw!!";

                std::string       l7cf_file_path = "./test_cf_files/l7vs.cf";

                //std::string       l7vsd_accesslog_rotate_type_val = "size";
                //std::string       l7vsd_accesslog_rotate_type_val = "date";
                std::string       l7vsd_accesslog_rotate_type_val = "datesize";
                std::string       l7vsd_accesslog_rotate_max_backup_index_val = "1";
                std::string       l7vsd_accesslog_rotate_max_filesize_val = "10M";
                std::string       l7vsd_accesslog_rotate_rotation_timing_val = "year";
                //std::string       l7vsd_accesslog_rotation_timing_value_val = "05/05 01:31";


                pm.setStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_ROTATION_KEY, l7vsd_accesslog_rotate_type_val);
                pm.setStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_MAX_BACKUP_INDEX_KEY, l7vsd_accesslog_rotate_max_backup_index_val);
                pm.setStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_MAX_FILE_SIZE_KEY, l7vsd_accesslog_rotate_max_filesize_val);
                pm.setStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_ROTATION_TIMING_KEY, l7vsd_accesslog_rotate_rotation_timing_val);
                //pm.setStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_ROTATION_TIMING_VALUE_KEY, l7vsd_accesslog_rotation_timing_value_val);


                try {

                        implTest.access_log_rotate_loadConf();
                        BOOST_ERROR(no_thorw_error_msg);
                } catch (const std::logic_error &ex) {
                        // unit_test[11] エラーが発生していること
                        std::string ex_msg = "Not Exist Log RotaionTiming " + l7vsd_accesslog_rotate_rotation_timing_val + " Setting.";
                        BOOST_CHECK_EQUAL(ex.what(), ex_msg);
                }

                pm.deleteStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_ROTATION_KEY);
                pm.deleteStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_MAX_BACKUP_INDEX_KEY);
                pm.deleteStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_MAX_FILE_SIZE_KEY);
                pm.deleteStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_ROTATION_TIMING_KEY);
                //pm.deleteStringValue(PARAM_COMP_LOGGER, ACCESS_LOG_ROTATION_TIMING_VALUE_KEY );

        }


        BOOST_MESSAGE("----- logger_access_manager_access_log_rotate_loadConf_test end -----");
}



void logger_access_manager_getInstance_constructor_test()
{

        BOOST_MESSAGE("----- logger_access_manager_getInstance_constructor_test start -----");

        logger_access_manager_test implTest;

        // unit_test[1] コンストラクタでメンバ変数が初期化されているか。
        BOOST_CHECK_EQUAL(implTest.get_rotate_default_load_flag(), false);
        // unit_test[2] コンストラクタでメンバ変数が初期化されているか。
        BOOST_CHECK_EQUAL(implTest.call_get_rotate_default_verbose_displayed_contents(), "");
        // unit_test[3] コンストラクタでメンバ変数が初期化されているか。
        BOOST_CHECK(implTest.get_access_log_default_data().size() == 0);
        // unit_test[4] コンストラクタでメンバ変数が初期化されているか。
        BOOST_CHECK(implTest.get_logimp_access_map().size() == 0);

        BOOST_MESSAGE("----- logger_access_manager_getInstance_constructor_test end -----");
}


void logger_access_manager_find_and_erase_logger_implement_access_test()
{

        BOOST_MESSAGE("----- logger_access_manager_find_and_erase_logger_implement_access_test start -----");

        logger_access_manager_test implTest;

        l7vs::logger_implement_access::rtn_init_flag = true;

        l7vs::logger_implement_access::rtn_checkRotateParameterComp_flag = true;

        l7vs::logger_implement_access::rtn_setAcLoggerConf_flag = true;

        std::cout << "find_logger_implement_access normal case01 size" << std::endl;
        {
                std::string aclogFilename = "aclog1";
                accesslog_rotate_default_map_type rotatedata;
                l7vs::error_code vs_err;

                rotatedata.insert(make_pair(ACCESS_LOG_ROTATE_TYPE , "size"));
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "date" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "datesize" ) );
                rotatedata.insert(make_pair(ACCESS_LOG_ROTATE_MAX_BACKUP_INDEX , "1"));

                rotatedata.insert(make_pair(ACCESS_LOG_ROTATE_MAX_FILESIZE , "10M"));

                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "year" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "month" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "week" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "date" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "hour" ) );

                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "5/10 11:11" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "11 5:5" ) );

                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "sun 10:10" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "mon 10:10" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "tue 10:10" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "wed 10:10" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "thu 10:10" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "fri 10:10" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "sat 10:10" ) );

                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "12:21" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "55" ) );

                l7vs::logger_implement_access::rtn_init_flag = true;

                logger_implement_access *acinstance = implTest.find_logger_implement_access(aclogFilename, rotatedata, vs_err);

                // unit_test[1] エラーが発生していないこと
                BOOST_CHECK(vs_err == false);
                // unit_test[2] ファイルがマップに登録されていること
                BOOST_CHECK(implTest.get_logimp_access_map().find(aclogFilename) != implTest.get_logimp_access_map().end());
                // unit_test[3] アクセスログのインスタンスが生成されていること
                BOOST_CHECK(acinstance != NULL);

                vs_err.setter(false , "");
                implTest.erase_logger_implement_access(aclogFilename, vs_err);

                // unit_test[4] エラーが発生していないこと
                BOOST_CHECK(vs_err == false);
                // unit_test[5] ファイルがマップに登録されていないこと
                BOOST_CHECK(implTest.get_logimp_access_map().find(aclogFilename) == implTest.get_logimp_access_map().end());

        }

        std::cout << "find_logger_implement_access normal case02 size 0" << std::endl;
        {
                std::string aclogFilename = "aclog2";
                accesslog_rotate_default_map_type rotatedata;
                l7vs::error_code vs_err;

                rotatedata.clear();

                l7vs::logger_implement_access::rtn_init_flag = true;

                logger_implement_access *acinstance = implTest.find_logger_implement_access(aclogFilename, rotatedata, vs_err);

                // unit_test[6] エラーが発生していないこと
                BOOST_CHECK(vs_err == false);
                // unit_test[7] ファイルがマップに登録されていること
                BOOST_CHECK(implTest.get_logimp_access_map().find(aclogFilename) != implTest.get_logimp_access_map().end());
                // unit_test[8] アクセスログのインスタンスが生成されていること
                BOOST_CHECK(acinstance != NULL);

                vs_err.setter(false , "");
                implTest.erase_logger_implement_access(aclogFilename, vs_err);

                // unit_test[9] エラーが発生していないこと
                BOOST_CHECK(vs_err == false);
                // unit_test[10] ファイルがマップに登録されていないこと
                BOOST_CHECK(implTest.get_logimp_access_map().find(aclogFilename) == implTest.get_logimp_access_map().end());

        }


        std::cout << "find_logger_implement_access vs_err case01 init err" << std::endl;
        {
                std::string aclogFilename = "aclog3";
                accesslog_rotate_default_map_type rotatedata;
                l7vs::error_code vs_err;

                rotatedata.insert(make_pair(ACCESS_LOG_ROTATE_TYPE , "size"));
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "date" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "datesize" ) );
                rotatedata.insert(make_pair(ACCESS_LOG_ROTATE_MAX_BACKUP_INDEX , "1"));

                rotatedata.insert(make_pair(ACCESS_LOG_ROTATE_MAX_FILESIZE , "10M"));

                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "year" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "month" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "week" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "date" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "hour" ) );

                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "5/10 11:11" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "11 5:5" ) );

                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "sun 10:10" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "mon 10:10" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "tue 10:10" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "wed 10:10" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "thu 10:10" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "fri 10:10" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "sat 10:10" ) );

                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "12:21" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "55" ) );

                l7vs::logger_implement_access::rtn_init_flag = false;


                logger_implement_access *acinstance = implTest.find_logger_implement_access(aclogFilename, rotatedata, vs_err);

                // unit_test[11] エラーが発生していること
                BOOST_CHECK(vs_err == true);
                // unit_test[12] エラーメッセージが格納されていること
                BOOST_CHECK(vs_err.get_message() == "logger_implement_access initialize err.");
                // unit_test[13] アクセスログのインスタンスが生成されていないこと
                BOOST_CHECK(acinstance == NULL);

                l7vs::logger_implement_access::rtn_init_flag = false;

                vs_err.setter(false , "");
                implTest.erase_logger_implement_access(aclogFilename, vs_err);

                // unit_test[14] エラーが発生していること
                BOOST_CHECK(vs_err == true);
                // unit_test[15] ファイルがマップに登録されていないこと
                BOOST_CHECK(vs_err.get_message() == "access log erase instance find err.");

        }


        std::cout << "find_logger_implement_access vs_err case02 init err size0" << std::endl;
        {
                std::string aclogFilename = "aclog4";
                accesslog_rotate_default_map_type rotatedata;
                l7vs::error_code vs_err;

                rotatedata.clear();

                l7vs::logger_implement_access::rtn_init_flag = false;


                logger_implement_access *acinstance = implTest.find_logger_implement_access(aclogFilename, rotatedata, vs_err);

                // unit_test[16] エラーが発生していること
                BOOST_CHECK(vs_err == true);
                // unit_test[17] エラーメッセージが格納されていること
                BOOST_CHECK(vs_err.get_message() == "logger_implement_access initialize err.");
                // unit_test[18] アクセスログのインスタンスが生成されていないこと
                BOOST_CHECK(acinstance == NULL);

                l7vs::logger_implement_access::rtn_init_flag = false;

                vs_err.setter(false , "");
                implTest.erase_logger_implement_access(aclogFilename, vs_err);

                // unit_test[19] エラーが発生していること
                BOOST_CHECK(vs_err == true);
                // unit_test[20] ファイルがマップに登録されていないこと
                BOOST_CHECK(vs_err.get_message() == "access log erase instance find err.");

        }

        std::cout << "find_logger_implement_access vs_err case02 init err size0" << std::endl;
        {
                std::string aclogFilename = "aclog4";
                accesslog_rotate_default_map_type rotatedata;
                l7vs::error_code vs_err;

                rotatedata.clear();

                l7vs::logger_implement_access::rtn_init_flag = false;


                logger_implement_access *acinstance = implTest.find_logger_implement_access(aclogFilename, rotatedata, vs_err);

                // unit_test[21] エラーが発生していること
                BOOST_CHECK(vs_err == true);
                // unit_test[22] エラーメッセージが格納されていること
                BOOST_CHECK(vs_err.get_message() == "logger_implement_access initialize err.");
                // unit_test[23] アクセスログのインスタンスが生成されていないこと
                BOOST_CHECK(acinstance == NULL);

                l7vs::logger_implement_access::rtn_init_flag = false;

                vs_err.setter(false , "");
                implTest.erase_logger_implement_access(aclogFilename, vs_err);

                // unit_test[24] エラーが発生していること
                BOOST_CHECK(vs_err == true);
                // unit_test[25] ファイルがマップに登録されていないこと
                BOOST_CHECK(vs_err.get_message() == "access log erase instance find err.");

        }

        std::cout << "find_logger_implement_access etc case01 find⇒find⇒find⇒erase⇒erase⇒erase⇒erase" << std::endl;
        {
                std::string aclogFilename = "aclog4";
                accesslog_rotate_default_map_type rotatedata;
                l7vs::error_code vs_err;

                rotatedata.insert(make_pair(ACCESS_LOG_ROTATE_TYPE , "size"));

                l7vs::logger_implement_access::rtn_init_flag = true;

                vs_err.setter(false , "");
                logger_implement_access *acinstance = implTest.find_logger_implement_access(aclogFilename, rotatedata, vs_err);

                // unit_test[26] エラーが発生していないこと
                BOOST_CHECK(vs_err == false);
                // unit_test[27] エラーメッセージが格納されていること
                BOOST_CHECK(vs_err.get_message() == "");
                // unit_test[28] アクセスログのインスタンスが生成されていないこと
                BOOST_CHECK(acinstance != NULL);

                vs_err.setter(false , "");
                logger_implement_access *acinstance2 = implTest.find_logger_implement_access(aclogFilename, rotatedata, vs_err);

                // unit_test[29] エラーが発生していないこと
                BOOST_CHECK(vs_err == false);
                // unit_test[30] エラーメッセージが格納されていること
                BOOST_CHECK(vs_err.get_message() == "");
                // unit_test[31] アクセスログのインスタンスが生成されていること
                BOOST_CHECK(acinstance == acinstance2);

                vs_err.setter(false , "");
                logger_implement_access *acinstance3 = implTest.find_logger_implement_access(aclogFilename, rotatedata, vs_err);

                // unit_test[32] エラーが発生していないこと
                BOOST_CHECK(vs_err == false);
                // unit_test[33] エラーメッセージが格納されていること
                BOOST_CHECK(vs_err.get_message() == "");
                // unit_test[34] アクセスログのインスタンスが生成されていること
                BOOST_CHECK(acinstance == acinstance3);

                vs_err.setter(false , "");
                implTest.erase_logger_implement_access(aclogFilename, vs_err);

                // unit_test[35] エラーが発生していないこと
                BOOST_CHECK(vs_err == false);
                // unit_test[36] エラーメッセージが格納されていないこと
                BOOST_CHECK(vs_err.get_message() == "");

                vs_err.setter(false , "");
                implTest.erase_logger_implement_access(aclogFilename, vs_err);

                // unit_test[37] エラーが発生していないこと
                BOOST_CHECK(vs_err == false);
                // unit_test[38] エラーメッセージが格納されていないこと
                BOOST_CHECK(vs_err.get_message() == "");

                vs_err.setter(false , "");
                implTest.erase_logger_implement_access(aclogFilename, vs_err);

                // unit_test[39] エラーが発生していないこと
                BOOST_CHECK(vs_err == false);
                // unit_test[40] エラーメッセージが格納されていないこと
                BOOST_CHECK(vs_err.get_message() == "");

                vs_err.setter(false , "");
                implTest.erase_logger_implement_access(aclogFilename, vs_err);

                // unit_test[41] エラーが発生していること
                BOOST_CHECK(vs_err == true);
                // unit_test[42] エラーメッセージが格納されていること
                BOOST_CHECK(vs_err.get_message() == "access log erase instance find err.");

        }

        std::cout << "find_logger_implement_access etc case01 erase⇒erase⇒erase⇒erase" << std::endl;
        {
                std::string aclogFilename = "aclog4";
                accesslog_rotate_default_map_type rotatedata;
                l7vs::error_code vs_err;

                rotatedata.insert(make_pair(ACCESS_LOG_ROTATE_TYPE , "size"));

                l7vs::logger_implement_access::rtn_init_flag = true;

                vs_err.setter(false , "");
                implTest.erase_logger_implement_access(aclogFilename, vs_err);

                // unit_test[43] エラーが発生していないこと
                BOOST_CHECK(vs_err == true);
                // unit_test[44] エラーメッセージが格納されていないこと
                BOOST_CHECK(vs_err.get_message() == "access log erase instance find err.");

                vs_err.setter(false , "");
                implTest.erase_logger_implement_access(aclogFilename, vs_err);

                // unit_test[45] エラーが発生していないこと
                BOOST_CHECK(vs_err == true);
                // unit_test[46] エラーメッセージが格納されていないこと
                BOOST_CHECK(vs_err.get_message() == "access log erase instance find err.");

                vs_err.setter(false , "");
                implTest.erase_logger_implement_access(aclogFilename, vs_err);

                // unit_test[47] エラーが発生していないこと
                BOOST_CHECK(vs_err == true);
                // unit_test[48] エラーメッセージが格納されていないこと
                BOOST_CHECK(vs_err.get_message() == "access log erase instance find err.");

        }




        BOOST_MESSAGE("----- logger_access_manager_find_and_erase_logger_implement_access_test end -----");
}

void logger_access_manager_get_rotate_default_verbose_displayed_contents_test()
{

        BOOST_MESSAGE("----- logger_access_manager_get_rotate_default_verbose_displayed_contents_test start -----");

        logger_access_manager_test implTest;

        std::string test_Msg = "default_info";
        std::string rtn_Msg = "";

        implTest.set_rotate_default_verbose_displayed_contents(test_Msg);

        rtn_Msg = implTest.call_get_rotate_default_verbose_displayed_contents();

        // unit_test[1] コンストラクタでメンバ変数が初期化されているか。
        BOOST_CHECK_EQUAL(test_Msg, rtn_Msg);

        BOOST_MESSAGE("----- logger_access_manager_get_rotate_default_verbose_displayed_contents_test end -----");
}


void logger_logrotate_utility_loglotation_utility_logic_error_test()
{

        BOOST_MESSAGE("----- logger_logrotate_utility_loglotation_utility_logic_error_test start -----");

        logger_access_manager_test implTest;
        std::string no_thorw_error_msg = "err_msg";

        try {

                logger_logrotate_utility::loglotation_utility_logic_error(999, no_thorw_error_msg, __FILE__, __LINE__);
                BOOST_ERROR(no_thorw_error_msg);

        } catch (const std::logic_error &ex) {
                // unit_test[1] コンストラクタでメンバ変数が初期化されているか。
                BOOST_CHECK_EQUAL(ex.what(), no_thorw_error_msg);
        }


        BOOST_MESSAGE("----- logger_logrotate_utility_loglotation_utility_logic_error_test end -----");
}


void logger_logrotate_utility_getRotationTimingValue_test()
{

        BOOST_MESSAGE("----- logger_logrotate_utility_getRotationTimingValue_test start -----");
        std::cout << "----- logger_logrotate_utility_getRotationTimingValue_test start -----\n";

        logger_access_manager_test implTest;
        std::string no_thorw_error_msg = "err_msg";

        // unit_test[1] loadConf method call
        //＊ローテーション日時を取得すること　年

        LOG_ROTATION_TIMING_TAG rotation_timing = LOG_TIM_YEAR;
        std::string rotation_timing_value_key = "";
        std::string rtn_data = "";

        rotation_timing_value_key = "05/05 01:31";
        rotation_timing = LOG_TIM_YEAR;
        rtn_data = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key, rotation_timing);
        BOOST_CHECK_EQUAL(rtn_data , "05050131");


        // unit_test[2] loadConf method call
        try {
                //＊ローテーション日時(年)が不正な時　区切り文字無し　すべて無し
                rotation_timing_value_key = "05050131";
                rotation_timing = LOG_TIM_YEAR;
                rtn_data = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key, rotation_timing);
                BOOST_ERROR(no_thorw_error_msg);

        } catch (const std::logic_error &ex) {
                BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Year Error.");
        }


        // unit_test[3] loadConf method call
        try {
                //＊ローテーション日時(年)が不正な時　区切り文字無し　日付の区切りが無し
                rotation_timing_value_key = "0505 01:31";
                rotation_timing = LOG_TIM_YEAR;
                rtn_data = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key, rotation_timing);
                BOOST_ERROR(no_thorw_error_msg);

        } catch (const std::logic_error &ex) {
                BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Year Error.");
        }

        // unit_test[4] loadConf method call
        try {
                //＊ローテーション日時(年)が不正な時　区切り文字無し　日付、時間の間の区切りが無し
                rotation_timing_value_key = "05/0501:31";
                rotation_timing = LOG_TIM_YEAR;
                rtn_data = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key, rotation_timing);
                BOOST_ERROR(no_thorw_error_msg);

        } catch (const std::logic_error &ex) {
                BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Year Error.");
        }

        // unit_test[5] loadConf method call
        try {
                //＊ローテーション日時(年)が不正な時　区切り文字無し　時間の間の区切りが無し
                rotation_timing_value_key = "05/05 0131";
                rotation_timing = LOG_TIM_YEAR;
                rtn_data = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key, rotation_timing);
                BOOST_ERROR(no_thorw_error_msg);

        } catch (const std::logic_error &ex) {
                BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Year Error.");
        }

        // unit_test[6] loadConf method call
        try {
                //＊ローテーション日時(年)が不正な時　値無し　月が無し
                rotation_timing_value_key = "/05 01:31";
                rotation_timing = LOG_TIM_YEAR;
                rtn_data = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key, rotation_timing);
                BOOST_ERROR(no_thorw_error_msg);

        } catch (const std::logic_error &ex) {
                BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Year Error.");
        }

        // unit_test[7] loadConf method call
        try {
                //＊ローテーション日時(年)が不正な時　値無し　日が無し
                rotation_timing_value_key = "05/ 01:31";
                rotation_timing = LOG_TIM_YEAR;
                rtn_data = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key, rotation_timing);
                BOOST_ERROR(no_thorw_error_msg);

        } catch (const std::logic_error &ex) {
                BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Year Error.");
        }

        // unit_test[8] loadConf method call
        try {
                //＊ローテーション日時(年)が不正な時　値無し　時間が無し
                rotation_timing_value_key = "05/05 :31";
                rotation_timing = LOG_TIM_YEAR;
                rtn_data = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key, rotation_timing);
                BOOST_ERROR(no_thorw_error_msg);

        } catch (const std::logic_error &ex) {
                BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Year Error.");
        }


        // unit_test[9] loadConf method call
        try {
                //＊ローテーション日時(年)が不正な時　値無し　分が無し
                rotation_timing_value_key = "05/05 01:";
                rotation_timing = LOG_TIM_YEAR;

                rtn_data = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key, rotation_timing);
                BOOST_ERROR(no_thorw_error_msg);

        } catch (const std::logic_error &ex) {
                BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Year Error.");
        }


        // unit_test[10] loadConf method call
        try {
                //＊ローテーション日時(年)が不正な時　値不正　月が不正
                rotation_timing_value_key = "a/05 01:31";
                rotation_timing = LOG_TIM_YEAR;
                rtn_data = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key, rotation_timing);
                BOOST_ERROR(no_thorw_error_msg);

        } catch (const std::logic_error &ex) {
                BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Year Error.");
        }

        // unit_test[11] loadConf method call
        try {
                //＊ローテーション日時(年)が不正な時　値不正　日が不正
                rotation_timing_value_key = "05/b 01:31";
                rotation_timing = LOG_TIM_YEAR;
                rtn_data = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key, rotation_timing);
                BOOST_ERROR(no_thorw_error_msg);

        } catch (const std::logic_error &ex) {
                BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Year Error.");
        }

        // unit_test[12] loadConf method call
        try {
                //＊ローテーション日時(年)が不正な時　値不正　時間が不正
                rotation_timing_value_key = "05/05 c:31";
                rotation_timing = LOG_TIM_YEAR;
                rtn_data = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key, rotation_timing);
                BOOST_ERROR(no_thorw_error_msg);

        } catch (const std::logic_error &ex) {
                BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Year Error.");
        }

        // unit_test[13] loadConf method call
        try {
                //＊ローテーション日時(年)が不正な時　値不正　分が不正
                rotation_timing_value_key = "05/05 01:d";
                rotation_timing = LOG_TIM_YEAR;
                rtn_data = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key, rotation_timing);
                BOOST_ERROR(no_thorw_error_msg);

        } catch (const std::logic_error &ex) {
                BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Year Error.");
        }

        // unit_test[14] loadConf method call
        try {
                //＊ローテーション日時(年)が不正な時　値範囲不正　月が不正
                rotation_timing_value_key = "13/05 01:31";
                rotation_timing = LOG_TIM_YEAR;
                rtn_data = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key, rotation_timing);
                BOOST_ERROR(no_thorw_error_msg);

        } catch (const std::logic_error &ex) {
                BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Year Error.");
        }

        // unit_test[15] loadConf method call
        try {
                //＊ローテーション日時(年)が不正な時　値範囲不正　日が不正
                rotation_timing_value_key = "05/32 01:31";
                rotation_timing = LOG_TIM_YEAR;
                rtn_data = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key, rotation_timing);
                BOOST_ERROR(no_thorw_error_msg);

        } catch (const std::logic_error &ex) {
                BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Year Error.");
        }

        // unit_test[16] loadConf method call
        try {
                //＊ローテーション日時(年)が不正な時　値範囲不正　時間が不正
                rotation_timing_value_key = "05/05 24:31";
                rotation_timing = LOG_TIM_YEAR;
                rtn_data = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key, rotation_timing);
                BOOST_ERROR(no_thorw_error_msg);

        } catch (const std::logic_error &ex) {
                BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Year Error.");
        }

        // unit_test[17] loadConf method call
        try {
                //＊ローテーション日時(年)が不正な時　値範囲不正　時間が不正
                rotation_timing_value_key = "05/05 01:60";
                rotation_timing = LOG_TIM_YEAR;
                rtn_data = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key, rotation_timing);
                BOOST_ERROR(no_thorw_error_msg);

        } catch (const std::logic_error &ex) {
                BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Year Error.");
        }

        // unit_test[18] loadConf method call
        try {
                //＊ローテーション日時(年)が不正な時　日付不正　存在しない日付
                rotation_timing_value_key = "04/31 01:31";
                rotation_timing = LOG_TIM_YEAR;
                rtn_data = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key, rotation_timing);
                BOOST_ERROR(no_thorw_error_msg);

        } catch (const std::logic_error &ex) {
                BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Year Error.");
        }

        // unit_test[19] loadConf method call
        try {
                //＊ローテーション日時(年)が存在しなかった時にエラーとなること
                rotation_timing_value_key = "";
                rotation_timing = LOG_TIM_YEAR;
                rtn_data = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key, rotation_timing);
                BOOST_ERROR(no_thorw_error_msg);

        } catch (const std::logic_error &ex) {
                BOOST_CHECK_EQUAL(ex.what(), "Not Exist Log RotaionTiming Year Setting.");
        }

        // unit_test[20] loadConf method call
        //＊ローテーション日時を取得すること　月
        rotation_timing_value_key = "15 18:21";
        rotation_timing = LOG_TIM_MONTH;
        rtn_data = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key, rotation_timing);
        BOOST_CHECK_EQUAL(rtn_data, "151821");


        // unit_test[21] loadConf method call
        try {
                //＊ローテーション日時(月)が不正な時　区切り文字無し　すべて無し
                rotation_timing_value_key = "151821";
                rotation_timing = LOG_TIM_MONTH;
                rtn_data = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key, rotation_timing);
                BOOST_ERROR(no_thorw_error_msg);

        } catch (const std::logic_error &ex) {
                BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Month Error.");
        }

        // unit_test[22] loadConf method call
        try {
                //＊ローテーション日時(月)が不正な時　区切り文字無し　日付、時間の間の区切りが無し
                rotation_timing_value_key = "1518:21";
                rotation_timing = LOG_TIM_MONTH;
                rtn_data = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key, rotation_timing);
                BOOST_ERROR(no_thorw_error_msg);

        } catch (const std::logic_error &ex) {
                BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Month Error.");
        }

        // unit_test[23] loadConf method call
        try {
                //＊ローテーション日時(月)が不正な時　区切り文字無し　時間と分の間の区切りが無し
                rotation_timing_value_key = "15 1821";
                rotation_timing = LOG_TIM_MONTH;
                rtn_data = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key, rotation_timing);
                BOOST_ERROR(no_thorw_error_msg);

        } catch (const std::logic_error &ex) {
                BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Month Error.");
        }

        // unit_test[24] loadConf method call
        try {
                //＊ローテーション日時(月)が不正な時　値無し　日が無し
                rotation_timing_value_key = " 18:21";
                rotation_timing = LOG_TIM_MONTH;
                rtn_data = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key, rotation_timing);
                BOOST_ERROR(no_thorw_error_msg);

        } catch (const std::logic_error &ex) {
                BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Month Error.");
        }

        // unit_test[25] loadConf method call
        try {
                //＊ローテーション日時(月)が不正な時　値無し　時間が無し
                rotation_timing_value_key = "15 :21";
                rotation_timing = LOG_TIM_MONTH;
                rtn_data = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key, rotation_timing);
                BOOST_ERROR(no_thorw_error_msg);

        } catch (const std::logic_error &ex) {
                BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Month Error.");
        }

        // unit_test[26] loadConf method call
        try {
                //＊ローテーション日時(月)が不正な時　値無し　分が無し
                rotation_timing_value_key = "15 18:";
                rotation_timing = LOG_TIM_MONTH;
                rtn_data = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key, rotation_timing);
                BOOST_ERROR(no_thorw_error_msg);

        } catch (const std::logic_error &ex) {
                BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Month Error.");
        }

        // unit_test[27] loadConf method call
        try {
                //＊ローテーション日時(月)が不正な時　値不正　日が不正
                rotation_timing_value_key = "a 18:21";
                rotation_timing = LOG_TIM_MONTH;
                rtn_data = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key, rotation_timing);
                BOOST_ERROR(no_thorw_error_msg);

        } catch (const std::logic_error &ex) {
                BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Month Error.");
        }

        // unit_test[28] loadConf method call
        try {
                //＊ローテーション日時(月)が不正な時　値不正　時間が不正
                rotation_timing_value_key = "15 b:21";
                rotation_timing = LOG_TIM_MONTH;
                rtn_data = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key, rotation_timing);
                BOOST_ERROR(no_thorw_error_msg);

        } catch (const std::logic_error &ex) {
                BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Month Error.");
        }

        // unit_test[29] loadConf method call
        try {
                //＊ローテーション日時(月)が不正な時　値不正　分が不正
                rotation_timing_value_key = "15 18:c";
                rotation_timing = LOG_TIM_MONTH;
                rtn_data = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key, rotation_timing);
                BOOST_ERROR(no_thorw_error_msg);

        } catch (const std::logic_error &ex) {
                BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Month Error.");
        }

        // unit_test[30] loadConf method call
        try {
                //＊ローテーション日時(月)が不正な時　値範囲不正　日が不正
                rotation_timing_value_key = "0 18:21";
                rotation_timing = LOG_TIM_MONTH;
                rtn_data = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key, rotation_timing);
                BOOST_ERROR(no_thorw_error_msg);

        } catch (const std::logic_error &ex) {
                BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Month Error.");
        }

        // unit_test[31] loadConf method call
        try {
                //＊ローテーション日時(月)が不正な時　値範囲不正　時間が不正
                rotation_timing_value_key = "15 -1:21";
                rotation_timing = LOG_TIM_MONTH;
                rtn_data = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key, rotation_timing);
                BOOST_ERROR(no_thorw_error_msg);

        } catch (const std::logic_error &ex) {
                BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Month Error.");
        }

        // unit_test[32] loadConf method call
        try {
                //＊ローテーション日時(月)が不正な時　値範囲不正　時間が不正
                rotation_timing_value_key = "15 18:-15";
                rotation_timing = LOG_TIM_MONTH;
                rtn_data = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key, rotation_timing);
                BOOST_ERROR(no_thorw_error_msg);

        } catch (const std::logic_error &ex) {
                BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Month Error.");
        }

        // unit_test[33] loadConf method call
        try {
                //＊ローテーション日時(月)が存在しなかった時にエラーとなること
                rotation_timing_value_key = "";
                rotation_timing = LOG_TIM_MONTH;
                rtn_data = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key, rotation_timing);
                BOOST_ERROR(no_thorw_error_msg);

        } catch (const std::logic_error &ex) {
                BOOST_CHECK_EQUAL(ex.what(), "Not Exist Log RotaionTiming Month Setting.");
        }

        // unit_test[34] loadConf method call
        //＊ローテーション日時を取得すること　週
        rotation_timing_value_key = "mon 23:59";
        rotation_timing = LOG_TIM_WEEK;
        rtn_data = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key, rotation_timing);
        BOOST_CHECK_EQUAL(rtn_data, "12359");           //0:日 1:月 2:火 3:水 4:木 5:金 6:土


        // unit_test[35] loadConf method call
        try {
                //＊ローテーション日時(週)が不正な時　区切り文字無し　すべて無し
                rotation_timing_value_key = "mon2359";
                rotation_timing = LOG_TIM_WEEK;
                rtn_data = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key, rotation_timing);
                BOOST_ERROR(no_thorw_error_msg);

        } catch (const std::logic_error &ex) {
                BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Week Error.");
        }

        // unit_test[36] loadConf method call
        try {
                //＊ローテーション日時(週)が不正な時　区切り文字無し　週、時間の間の区切りが無し
                rotation_timing_value_key = "mon23:59";
                rotation_timing = LOG_TIM_WEEK;
                rtn_data = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key, rotation_timing);
                BOOST_ERROR(no_thorw_error_msg);

        } catch (const std::logic_error &ex) {
                BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Week Error.");
        }

        // unit_test[37] loadConf method call
        try {
                //＊ローテーション日時(週)が不正な時　区切り文字無し　時間と分の間の区切りが無し
                rotation_timing_value_key = "mon 2359";
                rotation_timing = LOG_TIM_WEEK;
                rtn_data = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key, rotation_timing);
                BOOST_ERROR(no_thorw_error_msg);

        } catch (const std::logic_error &ex) {
                BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Week Error.");
        }


        // unit_test[38] loadConf method call
        try {
                //＊ローテーション日時(週)が不正な時　値無し　週が無し
                rotation_timing_value_key = " 23:59";
                rotation_timing = LOG_TIM_WEEK;
                rtn_data = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key, rotation_timing);
                BOOST_ERROR(no_thorw_error_msg);

        } catch (const std::logic_error &ex) {
                BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Week Error.");
        }

        // unit_test[39] loadConf method call
        try {
                //＊ローテーション日時(週)が不正な時　値無し　時間が無し
                rotation_timing_value_key = "mon :59";
                rotation_timing = LOG_TIM_WEEK;
                rtn_data = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key, rotation_timing);
                BOOST_ERROR(no_thorw_error_msg);

        } catch (const std::logic_error &ex) {
                BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Week Error.");
        }

        // unit_test[40] loadConf method call
        try {
                //＊ローテーション日時(週)が不正な時　値無し　分が無し
                rotation_timing_value_key = "mon 23:";
                rotation_timing = LOG_TIM_WEEK;
                rtn_data = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key, rotation_timing);
                BOOST_ERROR(no_thorw_error_msg);

        } catch (const std::logic_error &ex) {
                BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Week Error.");
        }

        // unit_test[41] loadConf method call
        try {
                //＊ローテーション日時(週)が不正な時　値不正　週が不正
                rotation_timing_value_key = "monday 23:59";
                rotation_timing = LOG_TIM_WEEK;
                rtn_data = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key, rotation_timing);
                BOOST_ERROR(no_thorw_error_msg);

        } catch (const std::logic_error &ex) {
                BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Week Error.");
        }

        // unit_test[42] loadConf method call
        try {
                //＊ローテーション日時(週)が不正な時　値不正　時間が不正
                rotation_timing_value_key = "mon a:59";
                rotation_timing = LOG_TIM_WEEK;
                rtn_data = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key, rotation_timing);
                BOOST_ERROR(no_thorw_error_msg);

        } catch (const std::logic_error &ex) {
                BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Week Error.");
        }

        // unit_test[43] loadConf method call
        try {
                //＊ローテーション日時(週)が不正な時　値不正　分が不正
                rotation_timing_value_key = "mon 23:b";
                rotation_timing = LOG_TIM_WEEK;
                rtn_data = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key, rotation_timing);
                BOOST_ERROR(no_thorw_error_msg);

        } catch (const std::logic_error &ex) {
                BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Week Error.");
        }

        // unit_test[44] loadConf method call
        try {
                //＊ローテーション日時(週)が不正な時　値範囲不正　日が不正
                rotation_timing_value_key = "0 18:21";
                rotation_timing = LOG_TIM_WEEK;
                rtn_data = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key, rotation_timing);
                BOOST_ERROR(no_thorw_error_msg);

        } catch (const std::logic_error &ex) {
                BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Week Error.");
        }

        // unit_test[45] loadConf method call
        try {
                //＊ローテーション日時(週)が不正な時　値範囲不正　時間が不正
                rotation_timing_value_key = "mon 99:59";
                rotation_timing = LOG_TIM_WEEK;
                rtn_data = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key, rotation_timing);
                BOOST_ERROR(no_thorw_error_msg);

        } catch (const std::logic_error &ex) {
                BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Week Error.");
        }

        // unit_test[46] loadConf method call
        try {
                //＊ローテーション日時(週)が不正な時　値範囲不正　時間が不正
                rotation_timing_value_key = "mon 23:100";
                rotation_timing = LOG_TIM_WEEK;
                rtn_data = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key, rotation_timing);
                BOOST_ERROR(no_thorw_error_msg);

        } catch (const std::logic_error &ex) {
                BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Week Error.");
        }

        // unit_test[47] loadConf method call
        try {
                //＊ローテーション日時(週)が存在しなかった時にエラーとなること
                rotation_timing_value_key = "";
                rotation_timing = LOG_TIM_WEEK;
                rtn_data = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key, rotation_timing);
                BOOST_ERROR(no_thorw_error_msg);

        } catch (const std::logic_error &ex) {
                BOOST_CHECK_EQUAL(ex.what(), "Not Exist Log RotaionTiming Week Setting.");
        }

        // unit_test[48] loadConf method call
        //＊ローテーション日時を取得すること　日
        rotation_timing_value_key = "12:00";
        rotation_timing = LOG_TIM_DATE;
        rtn_data = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key, rotation_timing);
        BOOST_CHECK_EQUAL(rtn_data, "1200");

        // unit_test[49] loadConf method call
        try {
                //＊ローテーション日時(日)が不正な時　区切り文字無し　時間と分の間の区切りが無し
                rotation_timing_value_key = "1200";
                rotation_timing = LOG_TIM_DATE;
                rtn_data = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key, rotation_timing);
                BOOST_ERROR(no_thorw_error_msg);

        } catch (const std::logic_error &ex) {
                BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Date Error.");
        }

        // unit_test[50] loadConf method call
        try {
                //＊ローテーション日時(日)が不正な時　値無し　時間が無し
                rotation_timing_value_key = ":00";
                rotation_timing = LOG_TIM_DATE;
                rtn_data = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key, rotation_timing);
                BOOST_ERROR(no_thorw_error_msg);

        } catch (const std::logic_error &ex) {
                BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Date Error.");
        }

        // unit_test[51] loadConf method call
        try {
                //＊ローテーション日時(日)が不正な時　値無し　分が無し
                rotation_timing_value_key = "12:";
                rotation_timing = LOG_TIM_DATE;
                rtn_data = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key, rotation_timing);
                BOOST_ERROR(no_thorw_error_msg);

        } catch (const std::logic_error &ex) {
                BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Date Error.");
        }

        // unit_test[52] loadConf method call
        try {
                //＊ローテーション日時(日)が不正な時　値不正　時間が不正
                rotation_timing_value_key = "a:00";
                rotation_timing = LOG_TIM_DATE;
                rtn_data = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key, rotation_timing);
                BOOST_ERROR(no_thorw_error_msg);

        } catch (const std::logic_error &ex) {
                BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Date Error.");
        }

        // unit_test[53] loadConf method call
        try {
                //＊ローテーション日時(日)が不正な時　値不正　分が不正
                rotation_timing_value_key = "12:b";
                rotation_timing = LOG_TIM_DATE;
                rtn_data = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key, rotation_timing);
                BOOST_ERROR(no_thorw_error_msg);

        } catch (const std::logic_error &ex) {
                BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Date Error.");
        }

        // unit_test[54] loadConf method call
        try {
                //＊ローテーション日時(日)が不正な時　値範囲不正　時間が不正
                rotation_timing_value_key = "-21:00";
                rotation_timing = LOG_TIM_DATE;
                rtn_data = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key, rotation_timing);
                BOOST_ERROR(no_thorw_error_msg);

        } catch (const std::logic_error &ex) {
                BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Date Error.");
        }

        // unit_test[55] loadConf method call
        try {
                //＊ローテーション日時(日)が不正な時　値範囲不正　時間が不正
                rotation_timing_value_key = "12:-59";
                rotation_timing = LOG_TIM_DATE;
                rtn_data = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key, rotation_timing);
                BOOST_ERROR(no_thorw_error_msg);

        } catch (const std::logic_error &ex) {
                BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Date Error.");
        }

        // unit_test[56] loadConf method call
        try {
                //＊ローテーション日時(日)が存在しなかった時にエラーとなること
                rotation_timing_value_key = "";
                rotation_timing = LOG_TIM_DATE;
                rtn_data = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key, rotation_timing);
                BOOST_ERROR(no_thorw_error_msg);

        } catch (const std::logic_error &ex) {
                BOOST_CHECK_EQUAL(ex.what(), "Not Exist Log RotaionTiming Date Setting.");
        }


        // unit_test[57] loadConf method call
        //＊ローテーション日時を取得すること　時
        rotation_timing_value_key = "45";
        rotation_timing = LOG_TIM_HOUR;
        rtn_data = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key, rotation_timing);
        BOOST_CHECK_EQUAL(rtn_data, "45");

        // unit_test[58] loadConf method call
        try {
                //＊ローテーション日時(日)が不正な時　値無し　分が無し
                rotation_timing_value_key = "";
                rotation_timing = LOG_TIM_HOUR;
                rtn_data = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key, rotation_timing);
                BOOST_ERROR(no_thorw_error_msg);

        } catch (const std::logic_error &ex) {
                BOOST_CHECK_EQUAL(ex.what(), "Not Exist Log RotaionTiming Hour Setting.");
        }

        // unit_test[59] loadConf method call
        try {
                //＊ローテーション日時(日)が不正な時　値不正　分が不正
                rotation_timing_value_key = "a";
                rotation_timing = LOG_TIM_HOUR;
                rtn_data = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key, rotation_timing);
                BOOST_ERROR(no_thorw_error_msg);

        } catch (const std::logic_error &ex) {
                BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Hour Error.");
        }

        // unit_test[60] loadConf method call
        try {
                //＊ローテーション日時(日)が不正な時　値範囲不正　分が不正
                rotation_timing_value_key = "120";
                rotation_timing = LOG_TIM_HOUR;
                rtn_data = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key, rotation_timing);
                BOOST_ERROR(no_thorw_error_msg);

        } catch (const std::logic_error &ex) {
                BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Hour Error.");
        }

        // unit_test[61] loadConf method call
        try {
                //＊ローテーション日時(日)が存在しなかった時にエラーとなること
                rotation_timing_value_key = "";
                rotation_timing = LOG_TIM_HOUR;
                rtn_data = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key, rotation_timing);
                BOOST_ERROR(no_thorw_error_msg);

        } catch (const std::logic_error &ex) {
                BOOST_CHECK_EQUAL(ex.what(), "Not Exist Log RotaionTiming Hour Setting.");
        }


        std::cout << "----- logger_logrotate_utility_getRotationTimingValue_test end -----\n";

        BOOST_MESSAGE("----- logger_logrotate_utility_getRotationTimingValue_test end -----");
}


void logger_logrotate_utility_set_appender_test()
{

        BOOST_MESSAGE("----- logger_logrotate_utility_set_appender_test start -----");

        logger_access_manager_test implTest;
        std::string no_thorw_error_msg = "err_msg";
        appender_property log_property;

        log_property.log_filename_key                = "";
        log_property.log_filename_value              = "";
        log_property.rotation_key                    = "";
        log_property.rotation_value                  = LOG_ROT_SIZE;
        log_property.max_backup_index_key            = "";
        log_property.max_backup_index_value          = 1;
        log_property.max_file_size_key               = "";
        log_property.max_file_size_value             = 1;
        log_property.rotation_timing_key             = "";
        log_property.rotation_timing_value           = LOG_TIM_YEAR;
        log_property.rotation_timing_value_key       = "";
        log_property.rotation_timing_value_value     = "";

        char *logger_layout = (char *)LOGGER_ACCESS_LAYOUT;

        std::string log_category = "./test";

        std::cout << "set_appender normal case01 size" << std::endl;
        {
                no_thorw_error_msg = "exception throw";

                log_category = "./test1";

                log_property.log_filename_key                = "";
                log_property.log_filename_value              = log_category;
                log_property.rotation_key                    = "";
                log_property.rotation_value                  = LOG_ROT_SIZE;
                log_property.max_backup_index_key            = "";
                log_property.max_backup_index_value          = 1;
                log_property.max_file_size_key               = "";
                log_property.max_file_size_value             = (100U * 1024 * 1024);
                log_property.rotation_timing_key             = "";
                log_property.rotation_timing_value           = LOG_TIM_YEAR;
                log_property.rotation_timing_value_key       = "";
                log_property.rotation_timing_value_value     = "";

                try {

                        logger_logrotate_utility::set_appender(log_property, logger_layout, log_category);
                        std::cout << "set_appender succeed\n";
                } catch (const std::logic_error &ex) {
                        // unit_test[1] コンストラクタでメンバ変数が初期化されているか。
                        BOOST_ERROR(no_thorw_error_msg);
                }

        }

        std::cout << "set_appender normal case02 date" << std::endl;
        {
                no_thorw_error_msg = "exception throw";

                log_category = "./test2";

                log_property.log_filename_key                = "";
                log_property.log_filename_value              = log_category;
                log_property.rotation_key                    = "";
                log_property.rotation_value                  = LOG_ROT_DATE;
                log_property.max_backup_index_key            = "";
                log_property.max_backup_index_value          = 1;
                log_property.max_file_size_key               = "";
                log_property.max_file_size_value             = (100U * 1024 * 1024);
                log_property.rotation_timing_key             = "";
                log_property.rotation_timing_value           = LOG_TIM_YEAR;
                log_property.rotation_timing_value_key       = "";
                log_property.rotation_timing_value_value     = "05050131";

                try {

                        logger_logrotate_utility::set_appender(log_property, logger_layout, log_category);
                        std::cout << "set_appender succeed\n";
                } catch (const std::logic_error &ex) {
                        // unit_test[1] コンストラクタでメンバ変数が初期化されているか。
                        BOOST_ERROR(no_thorw_error_msg);
                }

        }

        std::cout << "set_appender normal case03 datesize" << std::endl;
        {
                no_thorw_error_msg = "exception throw";

                log_category = "./test3";

                log_property.log_filename_key                = "";
                log_property.log_filename_value              = log_category;
                log_property.rotation_key                    = "";
                log_property.rotation_value                  = LOG_ROT_DATESIZE;
                log_property.max_backup_index_key            = "";
                log_property.max_backup_index_value          = 1;
                log_property.max_file_size_key               = "";
                log_property.max_file_size_value             = (100U * 1024 * 1024);
                log_property.rotation_timing_key             = "";
                log_property.rotation_timing_value           = LOG_TIM_YEAR;
                log_property.rotation_timing_value_key       = "";
                log_property.rotation_timing_value_value     = "05050131";

                try {

                        logger_logrotate_utility::set_appender(log_property, logger_layout, log_category);
                        std::cout << "set_appender succeed\n";
                } catch (const std::logic_error &ex) {
                        // unit_test[1] コンストラクタでメンバ変数が初期化されているか。
                        BOOST_ERROR(no_thorw_error_msg);
                }

        }

        /*
            std::cout << "set_appender err case01 size" << std::endl;
            {
                no_thorw_error_msg = "not exception throw";

                log_category = "./test4";

                log_property.log_filename_key                = "";
                //log_property.log_filename_value              = "";
                log_property.log_filename_value              = "";
                log_property.rotation_key                    = "";
                log_property.rotation_value                  = LOG_ROT_SIZE;
                log_property.max_backup_index_key            = "";
                log_property.max_backup_index_value          = 1;
                log_property.max_file_size_key               = "";
                log_property.max_file_size_value             = (100U * 1024 * 1024);
                log_property.rotation_timing_key             = "";
                log_property.rotation_timing_value           = LOG_TIM_YEAR;
                log_property.rotation_timing_value_key       = "";
                log_property.rotation_timing_value_value     = "";

                try {

                    logger_logrotate_utility::set_appender(log_property,logger_layout,log_category);

                    BOOST_ERROR(no_thorw_error_msg);

                }
                catch ( const std::logic_error& ex ) {
                    // unit_test[1] コンストラクタでメンバ変数が初期化されているか。
                    no_thorw_error_msg = "File Create Failed:" + log_property.log_filename_value;
                    BOOST_ERROR(no_thorw_error_msg);
                }

            }

            std::cout << "set_appender err case02 date" << std::endl;
            {
                no_thorw_error_msg = "not exception throw";

                log_category = "./test5";

                log_property.log_filename_key                = "";
                log_property.log_filename_value              = "";
                log_property.rotation_key                    = "";
                log_property.rotation_value                  = LOG_ROT_DATE;
                log_property.max_backup_index_key            = "";
                log_property.max_backup_index_value          = 1;
                log_property.max_file_size_key               = "";
                log_property.max_file_size_value             = (100U * 1024 * 1024);
                log_property.rotation_timing_key             = "";
                log_property.rotation_timing_value           = LOG_TIM_YEAR;
                log_property.rotation_timing_value_key       = "";
                log_property.rotation_timing_value_value     = "05050131";

                try {

                    logger_logrotate_utility::set_appender(log_property,logger_layout,log_category);

                    BOOST_ERROR(no_thorw_error_msg);

                }
                catch ( const std::logic_error& ex ) {
                    // unit_test[1] コンストラクタでメンバ変数が初期化されているか。
                    no_thorw_error_msg = "File Create Failed:" + log_property.log_filename_value;
                    BOOST_ERROR(no_thorw_error_msg);
                }

            }

            std::cout << "set_appender err case03 datesize" << std::endl;
            {
                no_thorw_error_msg = "not exception throw";

                log_category = "./test6";

                log_property.log_filename_key                = "";
                log_property.log_filename_value              = "";
                log_property.rotation_key                    = "";
                log_property.rotation_value                  = LOG_ROT_DATESIZE;
                log_property.max_backup_index_key            = "";
                log_property.max_backup_index_value          = 1;
                log_property.max_file_size_key               = "";
                log_property.max_file_size_value             = (100U * 1024 * 1024);
                log_property.rotation_timing_key             = "";
                log_property.rotation_timing_value           = LOG_TIM_YEAR;
                log_property.rotation_timing_value_key       = "";
                log_property.rotation_timing_value_value     = "05050131";

                try {

                    logger_logrotate_utility::set_appender(log_property,logger_layout,log_category);

                    BOOST_ERROR(no_thorw_error_msg);
                }
                catch ( const std::logic_error& ex ) {
                    // unit_test[1] コンストラクタでメンバ変数が初期化されているか。
                    no_thorw_error_msg = "File Create Failed:" + log_property.log_filename_value;
                    BOOST_CHECK_EQUAL(ex.what(), no_thorw_error_msg);
                }

            }

            std::cout << "set_appender err case04 etc" << std::endl;
            {
                no_thorw_error_msg = "not exception throw";

                log_category = "./test7";

                log_property.log_filename_key                = "";
                log_property.log_filename_value              = log_category;
                log_property.rotation_key                    = "";
                log_property.rotation_value                  = LOG_ROT_DATESIZE;
                log_property.max_backup_index_key            = "";
                log_property.max_backup_index_value          = -100;
                log_property.max_file_size_key               = "";
                log_property.max_file_size_value             = -1;
                log_property.rotation_timing_key             = "";
                log_property.rotation_timing_value           = LOG_TIM_YEAR;
                log_property.rotation_timing_value_key       = "";
                log_property.rotation_timing_value_value     = "zzz";

                try {

                    logger_logrotate_utility::set_appender(log_property,logger_layout,log_category);

                    BOOST_ERROR(no_thorw_error_msg);
                }
                catch ( const std::logic_error& ex ) {
                    // unit_test[1] コンストラクタでメンバ変数が初期化されているか。
                    no_thorw_error_msg = "File Create Failed:" + log_property.log_filename_value;
                    BOOST_CHECK_EQUAL(ex.what(), no_thorw_error_msg);
                }

            }
        */

        BOOST_MESSAGE("----- logger_logrotate_utility_set_appender_test end -----");
}


void logger_logrotate_utility_acccess_log_LogrotateParamCheck_test()
{

        BOOST_MESSAGE("----- logger_logrotate_utility_acccess_log_LogrotateParamCheck_test start -----");

        bool rtn_flag = true;
        accesslog_rotate_map_type rotatedata;
        appender_property log_property;

        log_property.log_filename_key                = "";
        log_property.log_filename_value              = "";
        log_property.rotation_key                    = "";
        log_property.rotation_value                  = LOG_ROT_DATESIZE;
        log_property.max_backup_index_key            = "";
        log_property.max_backup_index_value          = 0;
        log_property.max_file_size_key               = "";
        log_property.max_file_size_value             = 0;
        log_property.rotation_timing_key             = "";
        log_property.rotation_timing_value           = LOG_TIM_YEAR;
        log_property.rotation_timing_value_key       = "";
        log_property.rotation_timing_value_value     = "";

        std::cout << "acccess_log_LogrotateParamCheck normal case01 size" << std::endl;
        {
                rotatedata.clear();

                rotatedata.insert(make_pair(ACCESS_LOG_ROTATE_TYPE , "size"));
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "date" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "datesize" ) );
                rotatedata.insert(make_pair(ACCESS_LOG_ROTATE_MAX_BACKUP_INDEX , "1"));

                rotatedata.insert(make_pair(ACCESS_LOG_ROTATE_MAX_FILESIZE , "1000K"));

                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "year" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "month" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "week" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "date" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "hour" ) );

                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "5/10 11:11" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "11 5:5" ) );

                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "sun 10:10" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "mon 10:10" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "tue 10:10" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "wed 10:10" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "thu 10:10" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "fri 10:10" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "sat 10:10" ) );

                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "12:21" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "55" ) );


                log_property.log_filename_key                = "";
                log_property.log_filename_value              = "";
                log_property.rotation_key                    = "";
                log_property.rotation_value                  = LOG_ROT_DATESIZE;
                log_property.max_backup_index_key            = "";
                log_property.max_backup_index_value          = 0;
                log_property.max_file_size_key               = "";
                log_property.max_file_size_value             = 0;
                log_property.rotation_timing_key             = "";
                log_property.rotation_timing_value           = LOG_TIM_YEAR;
                log_property.rotation_timing_value_key       = "";
                log_property.rotation_timing_value_value     = "";


                rtn_flag = logger_logrotate_utility::acccess_log_LogrotateParamCheck(
                                   rotatedata,
                                   log_property);

                BOOST_CHECK_EQUAL(rtn_flag, true);

                BOOST_CHECK(log_property.rotation_value == LOG_ROT_SIZE);
                BOOST_CHECK(log_property.max_backup_index_value == 1);
                BOOST_CHECK(log_property.max_file_size_value == (1000U * 1024));

        }

        std::cout << "acccess_log_LogrotateParamCheck normal case02 date" << std::endl;
        {
                rotatedata.clear();

                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "size" ) );
                rotatedata.insert(make_pair(ACCESS_LOG_ROTATE_TYPE , "date"));
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "datesize" ) );
                rotatedata.insert(make_pair(ACCESS_LOG_ROTATE_MAX_BACKUP_INDEX , "1"));

                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_MAX_FILESIZE , "1000K" ) );

                rotatedata.insert(make_pair(ACCESS_LOG_ROTATE_ROTATION_TIMING , "year"));
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "month" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "week" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "date" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "hour" ) );

                rotatedata.insert(make_pair(ACCESS_LOG_ROTATION_TIMING_VALUE , "05/05 01:31"));
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "11 5:5" ) );

                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "sun 10:10" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "mon 10:10" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "tue 10:10" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "wed 10:10" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "thu 10:10" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "fri 10:10" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "sat 10:10" ) );

                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "12:21" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "55" ) );


                log_property.log_filename_key                = "";
                log_property.log_filename_value              = "";
                log_property.rotation_key                    = "";
                log_property.rotation_value                  = LOG_ROT_DATESIZE;
                log_property.max_backup_index_key            = "";
                log_property.max_backup_index_value          = 0;
                log_property.max_file_size_key               = "";
                log_property.max_file_size_value             = 0;
                log_property.rotation_timing_key             = "";
                log_property.rotation_timing_value           = LOG_TIM_YEAR;
                log_property.rotation_timing_value_key       = "";
                log_property.rotation_timing_value_value     = "";


                rtn_flag = logger_logrotate_utility::acccess_log_LogrotateParamCheck(
                                   rotatedata,
                                   log_property);

                BOOST_CHECK_EQUAL(rtn_flag, true);

                BOOST_CHECK(log_property.rotation_value == LOG_ROT_DATE);
                BOOST_CHECK(log_property.max_backup_index_value == 1);
                BOOST_CHECK(log_property.rotation_timing_value == LOG_TIM_YEAR);
                BOOST_CHECK(log_property.rotation_timing_value_value == "05050131");

        }

        std::cout << "acccess_log_LogrotateParamCheck normal case03 datesize" << std::endl;
        {
                rotatedata.clear();

                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "size" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "date" ) );
                rotatedata.insert(make_pair(ACCESS_LOG_ROTATE_TYPE , "datesize"));
                rotatedata.insert(make_pair(ACCESS_LOG_ROTATE_MAX_BACKUP_INDEX , "1"));

                rotatedata.insert(make_pair(ACCESS_LOG_ROTATE_MAX_FILESIZE , "1000K"));

                rotatedata.insert(make_pair(ACCESS_LOG_ROTATE_ROTATION_TIMING , "year"));
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "month" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "week" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "date" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "hour" ) );

                rotatedata.insert(make_pair(ACCESS_LOG_ROTATION_TIMING_VALUE , "05/05 01:31"));
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "11 5:5" ) );

                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "sun 10:10" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "mon 10:10" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "tue 10:10" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "wed 10:10" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "thu 10:10" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "fri 10:10" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "sat 10:10" ) );

                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "12:21" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "55" ) );


                log_property.log_filename_key                = "";
                log_property.log_filename_value              = "";
                log_property.rotation_key                    = "";
                log_property.rotation_value                  = LOG_ROT_DATESIZE;
                log_property.max_backup_index_key            = "";
                log_property.max_backup_index_value          = 0;
                log_property.max_file_size_key               = "";
                log_property.max_file_size_value             = 0;
                log_property.rotation_timing_key             = "";
                log_property.rotation_timing_value           = LOG_TIM_YEAR;
                log_property.rotation_timing_value_key       = "";
                log_property.rotation_timing_value_value     = "";


                rtn_flag = logger_logrotate_utility::acccess_log_LogrotateParamCheck(
                                   rotatedata,
                                   log_property);

                BOOST_CHECK_EQUAL(rtn_flag, true);

                BOOST_CHECK(log_property.rotation_value == LOG_ROT_DATESIZE);
                BOOST_CHECK(log_property.max_backup_index_value == 1);
                BOOST_CHECK(log_property.max_file_size_value == (1000U * 1024));
                BOOST_CHECK(log_property.rotation_timing_value == LOG_TIM_YEAR);
                BOOST_CHECK(log_property.rotation_timing_value_value == "05050131");

        }

        std::cout << "acccess_log_LogrotateParamCheck err case01 datesize no size" << std::endl;
        {
                rotatedata.clear();

                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "size" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "date" ) );
                rotatedata.insert(make_pair(ACCESS_LOG_ROTATE_TYPE , "datesize"));
                rotatedata.insert(make_pair(ACCESS_LOG_ROTATE_MAX_BACKUP_INDEX , "1"));

                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_MAX_FILESIZE , "1000K" ) );

                rotatedata.insert(make_pair(ACCESS_LOG_ROTATE_ROTATION_TIMING , "year"));
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "month" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "week" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "date" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "hour" ) );

                rotatedata.insert(make_pair(ACCESS_LOG_ROTATION_TIMING_VALUE , "05/05 01:31"));
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "11 5:5" ) );

                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "sun 10:10" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "mon 10:10" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "tue 10:10" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "wed 10:10" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "thu 10:10" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "fri 10:10" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "sat 10:10" ) );

                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "12:21" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "55" ) );


                log_property.log_filename_key                = "";
                log_property.log_filename_value              = "";
                log_property.rotation_key                    = "";
                log_property.rotation_value                  = LOG_ROT_DATESIZE;
                log_property.max_backup_index_key            = "";
                log_property.max_backup_index_value          = 0;
                log_property.max_file_size_key               = "";
                log_property.max_file_size_value             = 0;
                log_property.rotation_timing_key             = "";
                log_property.rotation_timing_value           = LOG_TIM_YEAR;
                log_property.rotation_timing_value_key       = "";
                log_property.rotation_timing_value_value     = "";


                rtn_flag = logger_logrotate_utility::acccess_log_LogrotateParamCheck(
                                   rotatedata,
                                   log_property);

                BOOST_CHECK_EQUAL(rtn_flag, false);

        }

        std::cout << "acccess_log_LogrotateParamCheck err case02 datesize no date" << std::endl;
        {
                rotatedata.clear();

                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "size" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "date" ) );
                rotatedata.insert(make_pair(ACCESS_LOG_ROTATE_TYPE , "datesize"));
                rotatedata.insert(make_pair(ACCESS_LOG_ROTATE_MAX_BACKUP_INDEX , "1"));

                rotatedata.insert(make_pair(ACCESS_LOG_ROTATE_MAX_FILESIZE , "1000K"));

                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "year" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "month" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "week" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "date" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "hour" ) );

                rotatedata.insert(make_pair(ACCESS_LOG_ROTATION_TIMING_VALUE , "05/05 01:31"));
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "11 5:5" ) );

                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "sun 10:10" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "mon 10:10" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "tue 10:10" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "wed 10:10" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "thu 10:10" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "fri 10:10" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "sat 10:10" ) );

                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "12:21" ) );
                //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "55" ) );


                log_property.log_filename_key                = "";
                log_property.log_filename_value              = "";
                log_property.rotation_key                    = "";
                log_property.rotation_value                  = LOG_ROT_DATESIZE;
                log_property.max_backup_index_key            = "";
                log_property.max_backup_index_value          = 0;
                log_property.max_file_size_key               = "";
                log_property.max_file_size_value             = 0;
                log_property.rotation_timing_key             = "";
                log_property.rotation_timing_value           = LOG_TIM_YEAR;
                log_property.rotation_timing_value_key       = "";
                log_property.rotation_timing_value_value     = "";


                rtn_flag = logger_logrotate_utility::acccess_log_LogrotateParamCheck(
                                   rotatedata,
                                   log_property);

                BOOST_CHECK_EQUAL(rtn_flag, false);

        }

        BOOST_MESSAGE("----- logger_logrotate_utility_acccess_log_LogrotateParamCheck_test end -----");
}

test_suite*
init_unit_test_suite(int argc, char *argv[])
{

        test_suite *test = BOOST_TEST_SUITE("logger_access_manager_test");
        test->add(BOOST_TEST_CASE(logger_access_manager_getInstance_constructor_test));
        test->add(BOOST_TEST_CASE(logger_access_manager_access_log_rotate_loadConf_test));
        test->add(BOOST_TEST_CASE(logger_access_manager_find_and_erase_logger_implement_access_test));
        test->add(BOOST_TEST_CASE(logger_access_manager_get_rotate_default_verbose_displayed_contents_test));
        test->add(BOOST_TEST_CASE(logger_logrotate_utility_loglotation_utility_logic_error_test));
        test->add(BOOST_TEST_CASE(logger_logrotate_utility_getRotationTimingValue_test));
        test->add(BOOST_TEST_CASE(logger_logrotate_utility_set_appender_test));
        test->add(BOOST_TEST_CASE(logger_logrotate_utility_acccess_log_LogrotateParamCheck_test));

        return test;

}

