
#include <iostream>
//#include "../../../l7vsd/src/logger_impl.cpp"
#include <boost/test/unit_test.hpp>
#include <log4cxx/appender.h>
#include <log4cxx/logmanager.h>
#include <log4cxx/patternlayout.h>
#include <log4cxx/fileappender.h>
#include <log4cxx/net/syslogappender.h>
#include <log4cxx/hierarchy.h>
#include <dlfcn.h>
#include <limits.h>
//#include "parameter_stub/parameter.h"
#include "./logger_implement_access_stub/logger_implement_access.h"
#include "logger_enum.h"
#include "logger_access_manager.h"
#include "logger_logrotate_utility.h"

#include <boost/test/included/unit_test.hpp>


using namespace boost::unit_test_framework;

using namespace l7vs;

using namespace std;


using namespace boost::unit_test_framework;

using namespace l7vs;

extern void* _start;    //開始位置
extern void* _etext;    //終了位置

#define BUF_LEN (4096)

typedef	std::map< LOG_CATEGORY_TAG, LOG_LEVEL_TAG>
		category_level_map_type;


int resetConfigurationCalled = 0;

int getRootLoggerCalled = 0;
bool getRootLoggerReturnNull = false;

int patternLayoutCtorCalled = 0;
void* patternLayoutCreated = 0;
bool patternLayoutNewFail = false;

int fileAppenderCtorCalled = 0;
void* fileAppenderCreated = 0;
bool fileAppenderNewFail = false;
std::string fileAppenderCtorArgFilename = "";
log4cxx::LayoutPtr fileAppenderCtorArgLayout = 0;
int fileAppenderSetFileCalled = 0;
std::string fileAppenderSetFileArgFilename = "";
bool fileAppenderSetFileArgAppend = false;
bool fileAppenderSetFileArgBufferedIO = false;
size_t fileAppenderSetFileArgBufferSize = 0;
void* fileAppenderSetFileArgPool = 0;
bool fileAppenderSetFileException = false;

int poolCtorCalled = 0;
void* poolCreated = 0;

int syslogAppenderCtorCalled = 0;
void* syslogAppenderCreated = 0;
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
void* fixedWindowCreated = 0;
bool fixedWindowNewFail = false;

int sizeBaseCtorCalled = 0;
void* sizeBaseCreated = 0;
bool sizeBaseNewFail = false;

int strictPolicyCtorCalled = 0;
void* strictPolicyCreated = 0;
bool strictPolicyNewFail = false;

int timeSizePolicyCtorCalled = 0;
void* timeSizePolicyCreated = 0;
bool timeSizePolicyNewFail = false;

int rollingAppenderCtorCalled = 0;
void* rollingAppenderCreated = 0;
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
void* appenderSkeletonSetLayoutArgLayout = 0;

int rollingAppenderSkeletonActivateOptionsCalled = 0;

int hierarchySetConfiguredCalled = 0;


#define BUF_LEN (4096)

    typedef std::map< std::string , logger_implement_access* > 
                    logimp_access_map_type;

    typedef std::map< std::string , std::string > 
                    accesslog_rotate_default_map_type;

    typedef std::map< std::string , std::string >::iterator 
                    accesslog_rotate_default_map_type_iterator;

class logger_access_manager_test : public l7vs::logger_access_manager
{
public:    
    logimp_access_map_type &get_logimp_access_map(){ return(logimp_access_map); }

    appender_property &get_access_log_default_property(){ return(access_log_default_property); }

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

    accesslog_rotate_default_map_type &get_access_log_default_data(){ return(access_log_default_data); }

    bool get_rotate_default_load_flag(){ return( rotate_default_load_flag); }

    void set_rotate_default_load_flag(bool in){ rotate_default_load_flag = in; }
    
    std::string &call_get_rotate_default_verbose_displayed_contents(){ return(rotate_default_verbose_displayed_contents); }
    
    void set_rotate_default_verbose_displayed_contents(std::string in){ rotate_default_verbose_displayed_contents = in; }
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

	static	logger_access_manager_test&	get_instance(){
		static logger_access_manager_test	instance;
		return instance;
	}
*/
    logger_access_manager_test() {}
    //~logger_access_manager_test() {}

};



void logger_access_manager_getInstance_constructor_test()
{
    
    BOOST_MESSAGE( "----- logger_access_manager_getInstance_constructor_test start -----" );

    logger_access_manager_test implTest;

    // unit_test[1] コンストラクタでメンバ変数が初期化されているか。1
    BOOST_CHECK_EQUAL(implTest.get_rotate_default_load_flag(), false);
    // unit_test[2] コンストラクタでメンバ変数が初期化されているか。2
    BOOST_CHECK_EQUAL(implTest.call_get_rotate_default_verbose_displayed_contents(), "");

    BOOST_MESSAGE( "----- logger_access_manager_getInstance_constructor_test end -----" );
}


void logger_access_manager_access_log_rotate_loadConf_test()
{
    
    BOOST_MESSAGE( "----- logger_access_manager_access_log_rotate_loadConf_test start -----" );

    logger_access_manager_test implTest;

    implTest.get_access_log_default_property_init();
    
    implTest.access_log_rotate_loadConf();
    
    // unit_test[1] コンストラクタでメンバ変数が初期化されているか。1
    BOOST_CHECK_EQUAL( implTest.get_rotate_default_load_flag() , false );
    // unit_test[2] コンストラクタでメンバ変数が初期化されているか。2
    BOOST_CHECK_EQUAL( implTest.call_get_rotate_default_verbose_displayed_contents() , "" );

    BOOST_MESSAGE( "----- logger_access_manager_access_log_rotate_loadConf_test end -----" );

}


void logger_access_manager_find_and_erase_logger_implement_access_test()
{
    
    BOOST_MESSAGE( "----- logger_access_manager_find_and_erase_logger_implement_access_test start -----" );

    logger_access_manager_test implTest;

    l7vs::logger_implement_access::rtn_init_flag = true;

    l7vs::logger_implement_access::rtn_checkRotateParameterComp_flag = true;

    l7vs::logger_implement_access::rtn_setAcLoggerConf_flag = true;

    std::cout << "find_logger_implement_access normal case01 size" << std::endl;
    {
        std::string aclogFilename = "aclog1";
        accesslog_rotate_default_map_type rotatedata;
        l7vs::error_code vs_err;
    
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "size" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "date" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "datesize" ) );
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_MAX_BACKUP_INDEX , "1" ) );
        
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_MAX_FILESIZE , "10M" ) );
        
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
        
        logger_implement_access *acinstance = implTest.find_logger_implement_access(aclogFilename,rotatedata,vs_err);
    
        // unit_test[1] エラーが発生していないこと
        BOOST_CHECK( vs_err == false );
        // unit_test[2] ファイルがマップに登録されていること
        BOOST_CHECK( implTest.get_logimp_access_map().find(aclogFilename) != implTest.get_logimp_access_map().end() );
        // unit_test[3] アクセスログのインスタンスが生成されていること
        BOOST_CHECK( acinstance != NULL );

        vs_err.setter( false , "" );
        implTest.erase_logger_implement_access(aclogFilename,vs_err);

        // unit_test[3] エラーが発生していないこと
        BOOST_CHECK( vs_err == false );
        // unit_test[4] ファイルがマップに登録されていないこと
        BOOST_CHECK( implTest.get_logimp_access_map().find(aclogFilename) == implTest.get_logimp_access_map().end() );
        
    }


    
    std::cout << "find_logger_implement_access normal case02 date year" << std::endl;
    {
        std::string aclogFilename = "aclog2";
        accesslog_rotate_default_map_type rotatedata;
        l7vs::error_code vs_err;
    
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "size" ) );
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "date" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "datesize" ) );
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_MAX_BACKUP_INDEX , "1" ) );
        
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_MAX_FILESIZE , "10M" ) );
        
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "year" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "month" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "week" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "date" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "hour" ) );

        rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "5/10 11:11" ) );
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

        logger_implement_access *acinstance = implTest.find_logger_implement_access(aclogFilename,rotatedata,vs_err);
    
        // unit_test[1] エラーが発生していないこと
        BOOST_CHECK( vs_err == false );
        // unit_test[2] ファイルがマップに登録されていること
        BOOST_CHECK( implTest.get_logimp_access_map().find(aclogFilename) != implTest.get_logimp_access_map().end() );
        // unit_test[3] アクセスログのインスタンスが生成されていること
        BOOST_CHECK( acinstance != NULL );

        vs_err.setter( false , "" );
        implTest.erase_logger_implement_access(aclogFilename,vs_err);

        // unit_test[3] エラーが発生していないこと
        BOOST_CHECK( vs_err == false );
        // unit_test[4] ファイルがマップに登録されていないこと
        BOOST_CHECK( implTest.get_logimp_access_map().find(aclogFilename) == implTest.get_logimp_access_map().end() );
        
    }

    
    std::cout << "find_logger_implement_access normal case03 date month" << std::endl;
    {
        std::string aclogFilename = "aclog3";
        accesslog_rotate_default_map_type rotatedata;
        l7vs::error_code vs_err;
    
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "size" ) );
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "date" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "datesize" ) );
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_MAX_BACKUP_INDEX , "1" ) );
        
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_MAX_FILESIZE , "10M" ) );
        
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "year" ) );
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "month" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "week" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "date" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "hour" ) );

        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "5/10 11:11" ) );
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "11 5:5" ) );

        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "sun 10:10" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "mon 10:10" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "tue 10:10" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "wed 10:10" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "thu 10:10" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "fri 10:10" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "sat 10:10" ) );
        
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "12:21" ) );
        
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "55" ) );

        logger_implement_access *acinstance = implTest.find_logger_implement_access(aclogFilename,rotatedata,vs_err);
    
        // unit_test[1] エラーが発生していないこと
        BOOST_CHECK( vs_err == false );
        // unit_test[2] ファイルがマップに登録されていること
        BOOST_CHECK( implTest.get_logimp_access_map().find(aclogFilename) != implTest.get_logimp_access_map().end() );
        // unit_test[3] アクセスログのインスタンスが生成されていること
        BOOST_CHECK( acinstance != NULL );

        vs_err.setter( false , "" );
        implTest.erase_logger_implement_access(aclogFilename,vs_err);

        // unit_test[3] エラーが発生していないこと
        BOOST_CHECK( vs_err == false );
        // unit_test[4] ファイルがマップに登録されていないこと
        BOOST_CHECK( implTest.get_logimp_access_map().find(aclogFilename) == implTest.get_logimp_access_map().end() );
        
    }
    
    std::cout << "find_logger_implement_access normal case04 date week" << std::endl;
    {
        std::string aclogFilename = "aclog4";
        accesslog_rotate_default_map_type rotatedata;
        l7vs::error_code vs_err;
    
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "size" ) );
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "date" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "datesize" ) );
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_MAX_BACKUP_INDEX , "1" ) );
        
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_MAX_FILESIZE , "10M" ) );
        
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "year" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "month" ) );
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "week" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "date" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "hour" ) );

        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "5/10 11:11" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "11 5:5" ) );

        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "sun 10:10" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "mon 10:10" ) );
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "tue 10:10" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "wed 10:10" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "thu 10:10" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "fri 10:10" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "sat 10:10" ) );
        
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "12:21" ) );
        
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "55" ) );

        logger_implement_access *acinstance = implTest.find_logger_implement_access(aclogFilename,rotatedata,vs_err);
    
        // unit_test[1] エラーが発生していないこと
        BOOST_CHECK( vs_err == false );
        // unit_test[2] ファイルがマップに登録されていること
        BOOST_CHECK( implTest.get_logimp_access_map().find(aclogFilename) != implTest.get_logimp_access_map().end() );
        // unit_test[3] アクセスログのインスタンスが生成されていること
        BOOST_CHECK( acinstance != NULL );

        vs_err.setter( false , "" );
        implTest.erase_logger_implement_access(aclogFilename,vs_err);

        // unit_test[3] エラーが発生していないこと
        BOOST_CHECK( vs_err == false );
        // unit_test[4] ファイルがマップに登録されていないこと
        BOOST_CHECK( implTest.get_logimp_access_map().find(aclogFilename) == implTest.get_logimp_access_map().end() );
        
    }

    std::cout << "find_logger_implement_access normal case05 date date" << std::endl;
    {
        std::string aclogFilename = "aclog5";
        accesslog_rotate_default_map_type rotatedata;
        l7vs::error_code vs_err;
    
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "size" ) );
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "date" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "datesize" ) );
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_MAX_BACKUP_INDEX , "1" ) );
        
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_MAX_FILESIZE , "10M" ) );
        
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "year" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "month" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "week" ) );
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "date" ) );
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
        
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "12:21" ) );
        
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "55" ) );

        logger_implement_access *acinstance = implTest.find_logger_implement_access(aclogFilename,rotatedata,vs_err);
    
        // unit_test[1] エラーが発生していないこと
        BOOST_CHECK( vs_err == false );
        // unit_test[2] ファイルがマップに登録されていること
        BOOST_CHECK( implTest.get_logimp_access_map().find(aclogFilename) != implTest.get_logimp_access_map().end() );
        // unit_test[3] アクセスログのインスタンスが生成されていること
        BOOST_CHECK( acinstance != NULL );

        vs_err.setter( false , "" );
        implTest.erase_logger_implement_access(aclogFilename,vs_err);

        // unit_test[3] エラーが発生していないこと
        BOOST_CHECK( vs_err == false );
        // unit_test[4] ファイルがマップに登録されていないこと
        BOOST_CHECK( implTest.get_logimp_access_map().find(aclogFilename) == implTest.get_logimp_access_map().end() );
        
    }

    std::cout << "find_logger_implement_access normal case06 date hour" << std::endl;
    {
        std::string aclogFilename = "aclog6";
        accesslog_rotate_default_map_type rotatedata;
        l7vs::error_code vs_err;
    
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "size" ) );
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "date" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "datesize" ) );
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_MAX_BACKUP_INDEX , "1" ) );
        
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_MAX_FILESIZE , "10M" ) );
        
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "year" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "month" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "week" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "date" ) );
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "hour" ) );

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
        
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "55" ) );

        logger_implement_access *acinstance = implTest.find_logger_implement_access(aclogFilename,rotatedata,vs_err);
    
        // unit_test[1] エラーが発生していないこと
        BOOST_CHECK( vs_err == false );
        // unit_test[2] ファイルがマップに登録されていること
        BOOST_CHECK( implTest.get_logimp_access_map().find(aclogFilename) != implTest.get_logimp_access_map().end() );
        // unit_test[3] アクセスログのインスタンスが生成されていること
        BOOST_CHECK( acinstance != NULL );

        vs_err.setter( false , "" );
        implTest.erase_logger_implement_access(aclogFilename,vs_err);

        // unit_test[3] エラーが発生していないこと
        BOOST_CHECK( vs_err == false );
        // unit_test[4] ファイルがマップに登録されていないこと
        BOOST_CHECK( implTest.get_logimp_access_map().find(aclogFilename) == implTest.get_logimp_access_map().end() );
        
    }

    std::cout << "find_logger_implement_access normal case07 datesize hour" << std::endl;
    {
        std::string aclogFilename = "aclog7";
        accesslog_rotate_default_map_type rotatedata;
        l7vs::error_code vs_err;
    
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "size" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "date" ) );
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "datesize" ) );
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_MAX_BACKUP_INDEX , "1" ) );
        
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_MAX_FILESIZE , "10M" ) );
        
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "year" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "month" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "week" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "date" ) );
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "hour" ) );

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
        
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "55" ) );

        logger_implement_access *acinstance = implTest.find_logger_implement_access(aclogFilename,rotatedata,vs_err);
    
        // unit_test[1] エラーが発生していないこと
        BOOST_CHECK( vs_err == false );
        // unit_test[2] ファイルがマップに登録されていること
        BOOST_CHECK( implTest.get_logimp_access_map().find(aclogFilename) != implTest.get_logimp_access_map().end() );
        // unit_test[3] アクセスログのインスタンスが生成されていること
        BOOST_CHECK( acinstance != NULL );

        vs_err.setter( false , "" );
        implTest.erase_logger_implement_access(aclogFilename,vs_err);

        // unit_test[3] エラーが発生していないこと
        BOOST_CHECK( vs_err == false );
        // unit_test[4] ファイルがマップに登録されていないこと
        BOOST_CHECK( implTest.get_logimp_access_map().find(aclogFilename) == implTest.get_logimp_access_map().end() );
        
    }

    std::cout << "find_logger_implement_access normal case08 access log common name find" << std::endl;
    {
        std::string aclogFilename = "aclog8";
        accesslog_rotate_default_map_type rotatedata;
        l7vs::error_code vs_err;
    
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "size" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "date" ) );
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "datesize" ) );
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_MAX_BACKUP_INDEX , "1" ) );
        
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_MAX_FILESIZE , "10M" ) );
        
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "year" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "month" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "week" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "date" ) );
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "hour" ) );

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
        
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "55" ) );

        logger_implement_access *acinstance = implTest.find_logger_implement_access(aclogFilename,rotatedata,vs_err);
    
        // unit_test[1] エラーが発生していないこと
        BOOST_CHECK( vs_err == false );
        // unit_test[2] ファイルがマップに登録されていること
        BOOST_CHECK( implTest.get_logimp_access_map().find(aclogFilename) != implTest.get_logimp_access_map().end() );
        // unit_test[3] アクセスログのインスタンスが生成されていること
        BOOST_CHECK( acinstance != NULL );

        logger_implement_access *acinstance2 = implTest.find_logger_implement_access(aclogFilename,rotatedata,vs_err);
    
        // unit_test[1] エラーが発生していないこと
        BOOST_CHECK( vs_err == false );
        // unit_test[2] ファイルがマップに登録されていること
        BOOST_CHECK( implTest.get_logimp_access_map().find(aclogFilename) != implTest.get_logimp_access_map().end() );
        // unit_test[3] 同じインスタンスである事を確認
        BOOST_CHECK( acinstance == acinstance2 );
        
        
        vs_err.setter( false , "" );
        implTest.erase_logger_implement_access(aclogFilename,vs_err);

        // unit_test[3] エラーが発生していないこと
        BOOST_CHECK( vs_err == false );
        // unit_test[4] ファイルがマップに登録されていないこと
        BOOST_CHECK( implTest.get_logimp_access_map().find(aclogFilename) == implTest.get_logimp_access_map().end() );
        
        vs_err.setter( false , "" );
        implTest.erase_logger_implement_access(aclogFilename,vs_err);

        // unit_test[3] エラーが発生していないこと
        BOOST_CHECK( vs_err == false );
        // unit_test[4] ファイルがマップに登録されていないこと
        BOOST_CHECK( implTest.get_logimp_access_map().find(aclogFilename) == implTest.get_logimp_access_map().end() );
        
        
    }

    std::cout << "find_logger_implement_access vs_err case01 init err" << std::endl;
    {
        std::string aclogFilename = "aclog_err1";
        accesslog_rotate_default_map_type rotatedata;
        l7vs::error_code vs_err;
    
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "size" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "date" ) );
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "datesize" ) );
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_MAX_BACKUP_INDEX , "1" ) );
        
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_MAX_FILESIZE , "10M" ) );
        
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "year" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "month" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "week" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "date" ) );
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "hour" ) );

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

        logger_implement_access *acinstance = implTest.find_logger_implement_access(aclogFilename,rotatedata,vs_err);
    
        // unit_test[1] エラーが発生していること
        BOOST_CHECK( vs_err == true );
        // unit_test[2] エラーメッセージが格納されていること
        BOOST_CHECK( vs_err.get_message() == "logger_implement_access initialize err." );
        // unit_test[3] アクセスログのインスタンスが生成されていないこと
        BOOST_CHECK( acinstance == NULL );

        vs_err.setter( false , "" );
        implTest.erase_logger_implement_access(aclogFilename,vs_err);

        // unit_test[3] エラーが発生していること
        BOOST_CHECK( vs_err == true );
        // unit_test[4] ファイルがマップに登録されていないこと
        BOOST_CHECK( vs_err.get_message() == "access log erase instance find err." );
        
    }

    std::cout << "find_logger_implement_access vs_err case02 init err" << std::endl;
    {
        std::string aclogFilename = "aclog_err2";
        accesslog_rotate_default_map_type rotatedata;
        l7vs::error_code vs_err;
    
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "size" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "date" ) );
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "datesize" ) );
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_MAX_BACKUP_INDEX , "1" ) );
        
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_MAX_FILESIZE , "10M" ) );
        
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "year" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "month" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "week" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "date" ) );
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "hour" ) );

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

        logger_implement_access *acinstance = implTest.find_logger_implement_access(aclogFilename,rotatedata,vs_err);
    
        // unit_test[1] エラーが発生していること
        BOOST_CHECK( vs_err == true );
        // unit_test[2] エラーメッセージが格納されていること
        BOOST_CHECK( vs_err.get_message() == "logger_implement_access initialize err." );
        // unit_test[3] アクセスログのインスタンスが生成されていないこと
        BOOST_CHECK( acinstance == NULL );

        vs_err.setter( false , "" );
        implTest.erase_logger_implement_access(aclogFilename,vs_err);

        // unit_test[3] エラーが発生していること
        BOOST_CHECK( vs_err == true );
        // unit_test[4] ファイルがマップに登録されていないこと
        BOOST_CHECK( vs_err.get_message() == "access log erase instance find err." );
        
    }
    
    
    BOOST_MESSAGE( "----- logger_access_manager_find_and_erase_logger_implement_access_test end -----" );
}


void logger_access_manager_find_and_erase_logger_implement_access_test2()
{
    
    BOOST_MESSAGE( "----- logger_access_manager_find_and_erase_logger_implement_access_test start -----" );

    logger_access_manager_test implTest;

    l7vs::logger_implement_access::rtn_init_flag = true;

    l7vs::logger_implement_access::rtn_checkRotateParameterComp_flag = true;

    l7vs::logger_implement_access::rtn_setAcLoggerConf_flag = true;

    std::cout << "find_logger_implement_access normal case01 size" << std::endl;
    {
        std::string aclogFilename = "aclog1";
        accesslog_rotate_default_map_type rotatedata;
        l7vs::error_code vs_err;
    
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "size" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "date" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "datesize" ) );
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_MAX_BACKUP_INDEX , "1" ) );
        
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_MAX_FILESIZE , "10M" ) );
        
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
        
        logger_implement_access *acinstance = implTest.find_logger_implement_access(aclogFilename,rotatedata,vs_err);
    
        // unit_test[1] エラーが発生していないこと
        BOOST_CHECK( vs_err == false );
        // unit_test[2] ファイルがマップに登録されていること
        BOOST_CHECK( implTest.get_logimp_access_map().find(aclogFilename) != implTest.get_logimp_access_map().end() );
        // unit_test[3] アクセスログのインスタンスが生成されていること
        BOOST_CHECK( acinstance != NULL );

        vs_err.setter( false , "" );
        implTest.erase_logger_implement_access(aclogFilename,vs_err);

        // unit_test[3] エラーが発生していないこと
        BOOST_CHECK( vs_err == false );
        // unit_test[4] ファイルがマップに登録されていないこと
        BOOST_CHECK( implTest.get_logimp_access_map().find(aclogFilename) == implTest.get_logimp_access_map().end() );
        
    }


    
    std::cout << "find_logger_implement_access normal case02 date year" << std::endl;
    {
        std::string aclogFilename = "aclog2";
        accesslog_rotate_default_map_type rotatedata;
        l7vs::error_code vs_err;
    
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "size" ) );
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "date" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "datesize" ) );
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_MAX_BACKUP_INDEX , "1" ) );
        
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_MAX_FILESIZE , "10M" ) );
        
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "year" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "month" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "week" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "date" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "hour" ) );

        rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "5/10 11:11" ) );
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

        logger_implement_access *acinstance = implTest.find_logger_implement_access(aclogFilename,rotatedata,vs_err);
    
        // unit_test[1] エラーが発生していないこと
        BOOST_CHECK( vs_err == false );
        // unit_test[2] ファイルがマップに登録されていること
        BOOST_CHECK( implTest.get_logimp_access_map().find(aclogFilename) != implTest.get_logimp_access_map().end() );
        // unit_test[3] アクセスログのインスタンスが生成されていること
        BOOST_CHECK( acinstance != NULL );

        vs_err.setter( false , "" );
        implTest.erase_logger_implement_access(aclogFilename,vs_err);

        // unit_test[3] エラーが発生していないこと
        BOOST_CHECK( vs_err == false );
        // unit_test[4] ファイルがマップに登録されていないこと
        BOOST_CHECK( implTest.get_logimp_access_map().find(aclogFilename) == implTest.get_logimp_access_map().end() );
        
    }

    
    std::cout << "find_logger_implement_access normal case03 date month" << std::endl;
    {
        std::string aclogFilename = "aclog3";
        accesslog_rotate_default_map_type rotatedata;
        l7vs::error_code vs_err;
    
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "size" ) );
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "date" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "datesize" ) );
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_MAX_BACKUP_INDEX , "1" ) );
        
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_MAX_FILESIZE , "10M" ) );
        
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "year" ) );
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "month" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "week" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "date" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "hour" ) );

        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "5/10 11:11" ) );
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "11 5:5" ) );

        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "sun 10:10" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "mon 10:10" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "tue 10:10" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "wed 10:10" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "thu 10:10" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "fri 10:10" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "sat 10:10" ) );
        
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "12:21" ) );
        
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "55" ) );

        logger_implement_access *acinstance = implTest.find_logger_implement_access(aclogFilename,rotatedata,vs_err);
    
        // unit_test[1] エラーが発生していないこと
        BOOST_CHECK( vs_err == false );
        // unit_test[2] ファイルがマップに登録されていること
        BOOST_CHECK( implTest.get_logimp_access_map().find(aclogFilename) != implTest.get_logimp_access_map().end() );
        // unit_test[3] アクセスログのインスタンスが生成されていること
        BOOST_CHECK( acinstance != NULL );

        vs_err.setter( false , "" );
        implTest.erase_logger_implement_access(aclogFilename,vs_err);

        // unit_test[3] エラーが発生していないこと
        BOOST_CHECK( vs_err == false );
        // unit_test[4] ファイルがマップに登録されていないこと
        BOOST_CHECK( implTest.get_logimp_access_map().find(aclogFilename) == implTest.get_logimp_access_map().end() );
        
    }
    
    std::cout << "find_logger_implement_access normal case04 date week" << std::endl;
    {
        std::string aclogFilename = "aclog4";
        accesslog_rotate_default_map_type rotatedata;
        l7vs::error_code vs_err;
    
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "size" ) );
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "date" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "datesize" ) );
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_MAX_BACKUP_INDEX , "1" ) );
        
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_MAX_FILESIZE , "10M" ) );
        
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "year" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "month" ) );
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "week" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "date" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "hour" ) );

        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "5/10 11:11" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "11 5:5" ) );

        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "sun 10:10" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "mon 10:10" ) );
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "tue 10:10" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "wed 10:10" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "thu 10:10" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "fri 10:10" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "sat 10:10" ) );
        
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "12:21" ) );
        
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "55" ) );

        logger_implement_access *acinstance = implTest.find_logger_implement_access(aclogFilename,rotatedata,vs_err);
    
        // unit_test[1] エラーが発生していないこと
        BOOST_CHECK( vs_err == false );
        // unit_test[2] ファイルがマップに登録されていること
        BOOST_CHECK( implTest.get_logimp_access_map().find(aclogFilename) != implTest.get_logimp_access_map().end() );
        // unit_test[3] アクセスログのインスタンスが生成されていること
        BOOST_CHECK( acinstance != NULL );

        vs_err.setter( false , "" );
        implTest.erase_logger_implement_access(aclogFilename,vs_err);

        // unit_test[3] エラーが発生していないこと
        BOOST_CHECK( vs_err == false );
        // unit_test[4] ファイルがマップに登録されていないこと
        BOOST_CHECK( implTest.get_logimp_access_map().find(aclogFilename) == implTest.get_logimp_access_map().end() );
        
    }

    std::cout << "find_logger_implement_access normal case05 date date" << std::endl;
    {
        std::string aclogFilename = "aclog5";
        accesslog_rotate_default_map_type rotatedata;
        l7vs::error_code vs_err;
    
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "size" ) );
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "date" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "datesize" ) );
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_MAX_BACKUP_INDEX , "1" ) );
        
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_MAX_FILESIZE , "10M" ) );
        
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "year" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "month" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "week" ) );
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "date" ) );
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
        
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "12:21" ) );
        
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "55" ) );

        logger_implement_access *acinstance = implTest.find_logger_implement_access(aclogFilename,rotatedata,vs_err);
    
        // unit_test[1] エラーが発生していないこと
        BOOST_CHECK( vs_err == false );
        // unit_test[2] ファイルがマップに登録されていること
        BOOST_CHECK( implTest.get_logimp_access_map().find(aclogFilename) != implTest.get_logimp_access_map().end() );
        // unit_test[3] アクセスログのインスタンスが生成されていること
        BOOST_CHECK( acinstance != NULL );

        vs_err.setter( false , "" );
        implTest.erase_logger_implement_access(aclogFilename,vs_err);

        // unit_test[3] エラーが発生していないこと
        BOOST_CHECK( vs_err == false );
        // unit_test[4] ファイルがマップに登録されていないこと
        BOOST_CHECK( implTest.get_logimp_access_map().find(aclogFilename) == implTest.get_logimp_access_map().end() );
        
    }

    std::cout << "find_logger_implement_access normal case06 date hour" << std::endl;
    {
        std::string aclogFilename = "aclog6";
        accesslog_rotate_default_map_type rotatedata;
        l7vs::error_code vs_err;
    
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "size" ) );
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "date" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "datesize" ) );
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_MAX_BACKUP_INDEX , "1" ) );
        
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_MAX_FILESIZE , "10M" ) );
        
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "year" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "month" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "week" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "date" ) );
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "hour" ) );

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
        
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "55" ) );

        logger_implement_access *acinstance = implTest.find_logger_implement_access(aclogFilename,rotatedata,vs_err);
    
        // unit_test[1] エラーが発生していないこと
        BOOST_CHECK( vs_err == false );
        // unit_test[2] ファイルがマップに登録されていること
        BOOST_CHECK( implTest.get_logimp_access_map().find(aclogFilename) != implTest.get_logimp_access_map().end() );
        // unit_test[3] アクセスログのインスタンスが生成されていること
        BOOST_CHECK( acinstance != NULL );

        vs_err.setter( false , "" );
        implTest.erase_logger_implement_access(aclogFilename,vs_err);

        // unit_test[3] エラーが発生していないこと
        BOOST_CHECK( vs_err == false );
        // unit_test[4] ファイルがマップに登録されていないこと
        BOOST_CHECK( implTest.get_logimp_access_map().find(aclogFilename) == implTest.get_logimp_access_map().end() );
        
    }

    std::cout << "find_logger_implement_access normal case07 datesize hour" << std::endl;
    {
        std::string aclogFilename = "aclog7";
        accesslog_rotate_default_map_type rotatedata;
        l7vs::error_code vs_err;
    
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "size" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "date" ) );
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "datesize" ) );
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_MAX_BACKUP_INDEX , "1" ) );
        
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_MAX_FILESIZE , "10M" ) );
        
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "year" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "month" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "week" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "date" ) );
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "hour" ) );

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
        
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "55" ) );

        logger_implement_access *acinstance = implTest.find_logger_implement_access(aclogFilename,rotatedata,vs_err);
    
        // unit_test[1] エラーが発生していないこと
        BOOST_CHECK( vs_err == false );
        // unit_test[2] ファイルがマップに登録されていること
        BOOST_CHECK( implTest.get_logimp_access_map().find(aclogFilename) != implTest.get_logimp_access_map().end() );
        // unit_test[3] アクセスログのインスタンスが生成されていること
        BOOST_CHECK( acinstance != NULL );

        vs_err.setter( false , "" );
        implTest.erase_logger_implement_access(aclogFilename,vs_err);

        // unit_test[3] エラーが発生していないこと
        BOOST_CHECK( vs_err == false );
        // unit_test[4] ファイルがマップに登録されていないこと
        BOOST_CHECK( implTest.get_logimp_access_map().find(aclogFilename) == implTest.get_logimp_access_map().end() );
        
    }

    std::cout << "find_logger_implement_access normal case08 access log common name find" << std::endl;
    {
        std::string aclogFilename = "aclog8";
        accesslog_rotate_default_map_type rotatedata;
        l7vs::error_code vs_err;
    
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "size" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "date" ) );
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "datesize" ) );
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_MAX_BACKUP_INDEX , "1" ) );
        
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_MAX_FILESIZE , "10M" ) );
        
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "year" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "month" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "week" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "date" ) );
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "hour" ) );

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
        
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATION_TIMING_VALUE , "55" ) );

        logger_implement_access *acinstance = implTest.find_logger_implement_access(aclogFilename,rotatedata,vs_err);
    
        // unit_test[1] エラーが発生していないこと
        BOOST_CHECK( vs_err == false );
        // unit_test[2] ファイルがマップに登録されていること
        BOOST_CHECK( implTest.get_logimp_access_map().find(aclogFilename) != implTest.get_logimp_access_map().end() );
        // unit_test[3] アクセスログのインスタンスが生成されていること
        BOOST_CHECK( acinstance != NULL );

        logger_implement_access *acinstance2 = implTest.find_logger_implement_access(aclogFilename,rotatedata,vs_err);
    
        // unit_test[1] エラーが発生していないこと
        BOOST_CHECK( vs_err == false );
        // unit_test[2] ファイルがマップに登録されていること
        BOOST_CHECK( implTest.get_logimp_access_map().find(aclogFilename) != implTest.get_logimp_access_map().end() );
        // unit_test[3] 同じインスタンスである事を確認
        BOOST_CHECK( acinstance == acinstance2 );
        
        
        vs_err.setter( false , "" );
        implTest.erase_logger_implement_access(aclogFilename,vs_err);

        // unit_test[3] エラーが発生していないこと
        BOOST_CHECK( vs_err == false );
        // unit_test[4] ファイルがマップに登録されていないこと
        BOOST_CHECK( implTest.get_logimp_access_map().find(aclogFilename) == implTest.get_logimp_access_map().end() );
        
        vs_err.setter( false , "" );
        implTest.erase_logger_implement_access(aclogFilename,vs_err);

        // unit_test[3] エラーが発生していないこと
        BOOST_CHECK( vs_err == false );
        // unit_test[4] ファイルがマップに登録されていないこと
        BOOST_CHECK( implTest.get_logimp_access_map().find(aclogFilename) == implTest.get_logimp_access_map().end() );
        
        
    }

    std::cout << "find_logger_implement_access vs_err case01 init err" << std::endl;
    {
        std::string aclogFilename = "aclog_err1";
        accesslog_rotate_default_map_type rotatedata;
        l7vs::error_code vs_err;
    
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "size" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "date" ) );
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "datesize" ) );
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_MAX_BACKUP_INDEX , "1" ) );
        
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_MAX_FILESIZE , "10M" ) );
        
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "year" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "month" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "week" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "date" ) );
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "hour" ) );

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

        logger_implement_access *acinstance = implTest.find_logger_implement_access(aclogFilename,rotatedata,vs_err);
    
        // unit_test[1] エラーが発生していること
        BOOST_CHECK( vs_err == true );
        // unit_test[2] エラーメッセージが格納されていること
        BOOST_CHECK( vs_err.get_message() == "logger_implement_access initialize err." );
        // unit_test[3] アクセスログのインスタンスが生成されていないこと
        BOOST_CHECK( acinstance == NULL );

        vs_err.setter( false , "" );
        implTest.erase_logger_implement_access(aclogFilename,vs_err);

        // unit_test[3] エラーが発生していること
        BOOST_CHECK( vs_err == true );
        // unit_test[4] ファイルがマップに登録されていないこと
        BOOST_CHECK( vs_err.get_message() == "access log erase instance find err." );
        
    }

    std::cout << "find_logger_implement_access vs_err case02 init err" << std::endl;
    {
        std::string aclogFilename = "aclog_err2";
        accesslog_rotate_default_map_type rotatedata;
        l7vs::error_code vs_err;
    
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "size" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "date" ) );
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_TYPE , "datesize" ) );
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_MAX_BACKUP_INDEX , "1" ) );
        
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_MAX_FILESIZE , "10M" ) );
        
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "year" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "month" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "week" ) );
        //rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "date" ) );
        rotatedata.insert( make_pair( ACCESS_LOG_ROTATE_ROTATION_TIMING , "hour" ) );

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

        logger_implement_access *acinstance = implTest.find_logger_implement_access(aclogFilename,rotatedata,vs_err);
    
        // unit_test[1] エラーが発生していること
        BOOST_CHECK( vs_err == true );
        // unit_test[2] エラーメッセージが格納されていること
        BOOST_CHECK( vs_err.get_message() == "logger_implement_access initialize err." );
        // unit_test[3] アクセスログのインスタンスが生成されていないこと
        BOOST_CHECK( acinstance == NULL );

        vs_err.setter( false , "" );
        implTest.erase_logger_implement_access(aclogFilename,vs_err);

        // unit_test[3] エラーが発生していること
        BOOST_CHECK( vs_err == true );
        // unit_test[4] ファイルがマップに登録されていないこと
        BOOST_CHECK( vs_err.get_message() == "access log erase instance find err." );
        
    }
    
    
    BOOST_MESSAGE( "----- logger_access_manager_find_and_erase_logger_implement_access_test end -----" );
}


test_suite*
init_unit_test_suite(int argc, char* argv[])
{

    test_suite* test = BOOST_TEST_SUITE("logger_access_manager_test");
    //test->add(BOOST_TEST_CASE(logger_access_manager_getInstance_constructor_test));
    //test->add(BOOST_TEST_CASE(logger_access_manager_access_log_rotate_loadConf_test));
    test->add(BOOST_TEST_CASE(logger_access_manager_find_and_erase_logger_implement_access_test));

    
    
    return test;


}

