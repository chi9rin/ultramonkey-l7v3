/*

	Logger class Unit Test Program.


*/


#define    TEST_CASE
#include <boost/test/included/unit_test.hpp>
using namespace boost::unit_test_framework;

#include "logger_logrotate_utility.h"

//using namespace l7vs;

extern void* _start;    //開始位置
extern void* _etext;    //終了位置

#define BUF_LEN (4096)

//typedef std::map< LOG_CATEGORY_TAG, LOG_LEVEL_TAG>
//                category_level_map_type;


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
//log4cxx::LayoutPtr fileAppenderCtorArgLayout = 0;
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
//log4cxx::LayoutPtr syslogAppenderCtorArgLayout = 0;
int syslogAppenderCtorArgFacility = 0;

int loggerAddAppenderCalled = 0;
//log4cxx::AppenderPtr loggerAddAppenderArg[2] = {0, 0};

int loggerForcedLogCalled = 0;
//log4cxx::LevelPtr loggerForcedLogArgLevel = 0;

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
//LOG_CATEGORY_TAG snmpGetLevelArgCat;
//LOG_LEVEL_TAG snmpGetLevelReturns;

int appenderSkeletonSetLayoutCalled = 0;
void* appenderSkeletonSetLayoutArgLayout = 0;

int rollingAppenderSkeletonActivateOptionsCalled = 0;

int hierarchySetConfiguredCalled = 0;

//using namespace l7vs;

//enum LOG_ROTATION_TAG{ LOG_ROT_SIZE, LOG_ROT_DATE, LOG_ROT_DATE_SIZE };
//enum LOG_ROTATION_TIMING_TAG{ LOG_TIM_YEAR, LOG_TIM_MONTH, LOG_TIM_WEEK, LG_TIM_DATE, LO_TIM_HOUR };
//# "logger.cpp"

void check_rotate_type_test(){
    BOOST_MESSAGE( "----- check_rotate_type test start -----" );

    l7vs::LOG_ROTATION_TAG res;
    
    // unit_test [1] check_rotate_type test case "size"
    std::cout << "[1] check_rotate_type test" << std::endl;
    res  =  l7vs::logger_logrotate_utility::check_rotate_type("size");
    BOOST_CHECK_EQUAL( l7vs::LOG_ROT_SIZE , res );
    
    // unit_test [2] check_rotate_type test case "date"
    std::cout << "[2] check_rotate_type test" << std::endl;
    res  =  l7vs::logger_logrotate_utility::check_rotate_type("date");
    BOOST_CHECK_EQUAL( l7vs::LOG_ROT_DATE , res );
    
    // unit_test [3] check_rotate_type test case "datesize"
    std::cout << "[3] check_rotate_type test" << std::endl;
    res  =  l7vs::logger_logrotate_utility::check_rotate_type("datesize");
    BOOST_CHECK_EQUAL( l7vs::LOG_ROT_DATESIZE , res );

    // unit_test [4] check_rotate_type test case "date size"
    std::cout << "[4] check_rotate_type test" << std::endl;
    try {
        l7vs::logger_logrotate_utility::check_rotate_type( "date size" );
    } catch ( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Invalid Log Rotation Setting : date size");
    }
       
    // unit_test [5] check_rotate_type test ""
    std::cout << "[5] check_rotate_type test" << std::endl;
    try {
        l7vs::logger_logrotate_utility::check_rotate_type( "" );
    } catch ( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Not Exist Log Rotation Setting.");
    }

    BOOST_MESSAGE( "----- check_rotate_type test end -----" );
}

void check_max_backup_index_test(){
    BOOST_MESSAGE( "----- check_max_backup_index test start -----" );

    unsigned int res;
    
    // unit_test [1] check_max_backup_index test case "1"
    std::cout << "[1] check_max_backup_index test" << std::endl;
    res  =  l7vs::logger_logrotate_utility::check_max_backup_index("1");
    BOOST_CHECK_EQUAL( (unsigned int)1 , res );

    // unit_test [2] check_max_backup_index test case "12"
    std::cout << "[2] check_max_backup_index test" << std::endl;
    res  =  l7vs::logger_logrotate_utility::check_max_backup_index("12");
    BOOST_CHECK_EQUAL( (unsigned int)12 , res );

    // unit_test [3] check_max_backup_index test case "0"
    std::cout << "[3] check_max_backup_index test" << std::endl;
    try {
        l7vs::logger_logrotate_utility::check_max_backup_index( "0" );
    } catch ( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Max Backup Index must at least 1.");
    }

    // unit_test [4] check_max_backup_index test case "13"
    std::cout << "[4] check_max_backup_index test" << std::endl;
    try {
        l7vs::logger_logrotate_utility::check_max_backup_index( "13" );
    } catch ( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Max Backup Index must at most 12.");
    }

    // unit_test [5] check_max_backup_index test case "ABC"
    std::cout << "[5] check_max_backup_index test" << std::endl;
    try {
        l7vs::logger_logrotate_utility::check_max_backup_index( "ABC" );
    } catch ( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Invalid MaxBackupIndex Value : ABC.");
    }

    // unit_test [6] check_max_backup_index test ""
    std::cout << "[6] check_max_backup_index test" << std::endl;
    try {
        l7vs::logger_logrotate_utility::check_max_backup_index( "" );
    } catch ( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Not Exist Log MaxBackupIndex Setting.");
    }

    BOOST_MESSAGE( "----- check_max_backup_index test end -----" );
}

void check_max_file_size_test(){
    BOOST_MESSAGE( "----- check_max_file_size test start -----" );

    unsigned long long res;

    // unit_test [1] check_max_file_size test case "65535"
    std::cout << "[1] check_max_file_size test" << std::endl;
    res  =  l7vs::logger_logrotate_utility::check_max_file_size("65535");
    BOOST_CHECK_EQUAL( (unsigned long long)65535 , res );
    
    // unit_test [2] check_max_file_size test case "4294967295"
    std::cout << "[2] check_max_file_size test" << std::endl;
    res  =  l7vs::logger_logrotate_utility::check_max_file_size("4294967295");
    BOOST_CHECK_EQUAL( (unsigned long long)4294967295 , res );

    // unit_test [3] check_max_file_size test case "64K"
    std::cout << "[3] check_max_file_size test" << std::endl;
    res  =  l7vs::logger_logrotate_utility::check_max_file_size("64K");
    BOOST_CHECK_EQUAL( (unsigned long long)65536 , res );

    // unit_test [4] check_max_file_size test case "4194303K"
    std::cout << "[4] check_max_file_size test" << std::endl;
    res  =  l7vs::logger_logrotate_utility::check_max_file_size("4194303K");
    BOOST_CHECK_EQUAL( (unsigned long long)4294966272 , res );

    // unit_test [5] check_max_file_size test case "1M"
    std::cout << "[5] check_max_file_size test" << std::endl;
    res  =  l7vs::logger_logrotate_utility::check_max_file_size("1M");
    BOOST_CHECK_EQUAL( (unsigned long long)1048576 , res );

    // unit_test [6] check_max_file_size test case "4095M"
    std::cout << "[6] check_max_file_size test" << std::endl;
    res  =  l7vs::logger_logrotate_utility::check_max_file_size("4095M");
    BOOST_CHECK_EQUAL( (unsigned long long)4293918720 , res );

    // unit_test [7] check_max_file_size test case "1G"
    std::cout << "[7] check_max_file_size test" << std::endl;
    res  =  l7vs::logger_logrotate_utility::check_max_file_size("1G");
    BOOST_CHECK_EQUAL( (unsigned long long)1073741824 , res );

    // unit_test [8] check_max_file_size test case "3G"
    std::cout << "[8] check_max_file_size test" << std::endl;
    res  =  l7vs::logger_logrotate_utility::check_max_file_size("3G");
    BOOST_CHECK_EQUAL( (unsigned long long)3221225472 , res );

    // unit_test [9] check_max_file_size test case "65534"
    std::cout << "[9] check_max_file_size test" << std::endl;
    try {
        l7vs::logger_logrotate_utility::check_max_file_size( "65534" );
    } catch ( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "FileSize must at least 65535 bytes.");
    }
    // unit_test [10] check_max_file_size test case "63K"
    std::cout << "[10] check_max_file_size test" << std::endl;
    try {
        l7vs::logger_logrotate_utility::check_max_file_size( "63K" );
    } catch ( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "FileSize must at least 65535 bytes.");
    }
    // unit_test [11] check_max_file_size test case "0M"
    std::cout << "[11] check_max_file_size test" << std::endl;
    try {
        l7vs::logger_logrotate_utility::check_max_file_size( "0M" );
    } catch ( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "FileSize must at least 65535 bytes.");
    }
    // unit_test [12] check_max_file_size test case "0G"
    std::cout << "[12] check_max_file_size test" << std::endl;
    try {
        l7vs::logger_logrotate_utility::check_max_file_size( "0G" );
    } catch ( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "FileSize must at least 65535 bytes.");
    }

    // unit_test [13] check_max_file_size test case "4294967296"
    std::cout << "[13] check_max_file_size test" << std::endl;
    try {
        l7vs::logger_logrotate_utility::check_max_file_size( "4294967296" );
    } catch ( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Invalid FileSize Value.");
    }

    // unit_test [14] check_max_file_size test case "4194304K"
    std::cout << "[14] check_max_file_size test" << std::endl;
    try {
        l7vs::logger_logrotate_utility::check_max_file_size( "4194304K" );
    } catch ( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Invalid FileSize Value.");
    }

    // unit_test [15] check_max_file_size test case "4096M"
    std::cout << "[15] check_max_file_size test" << std::endl;
    try {
        l7vs::logger_logrotate_utility::check_max_file_size( "4096M" );
    } catch ( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Invalid FileSize Value.");
    }

    // unit_test [16] check_max_file_size test case "4G"
    std::cout << "[16] check_max_file_size test" << std::endl;
    try {
        l7vs::logger_logrotate_utility::check_max_file_size( "4G" );
    } catch ( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Invalid FileSize Value.");
    }

    // unit_test [17] check_max_file_size test case "1T"
    std::cout << "[17] check_max_file_size test" << std::endl;
    try {
        l7vs::logger_logrotate_utility::check_max_file_size( "1T" );
    } catch ( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Invalid FileSize Value.");
    }

    // unit_test [18] check_max_file_size test case "ABC"
    std::cout << "[18] check_max_file_size test" << std::endl;
    try {
        l7vs::logger_logrotate_utility::check_max_file_size( "ABC" );
    } catch ( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Invalid FileSize Value.");
    }

    // unit_test [19] check_max_file_size test case "ABCK"
    std::cout << "[19] check_max_file_size test" << std::endl;
    try {
        l7vs::logger_logrotate_utility::check_max_file_size( "ABCK" );
    } catch ( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Invalid FileSize Value.");
    }

    // unit_test [20] check_max_file_size test case "ABCM"
    std::cout << "[20] check_max_file_size test" << std::endl;
    try {
        l7vs::logger_logrotate_utility::check_max_file_size( "ABCM" );
    } catch ( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Invalid FileSize Value.");
    }

    // unit_test [22] check_max_file_size test case ""
    std::cout << "[22] check_max_file_size test" << std::endl;
    try {
        l7vs::logger_logrotate_utility::check_max_file_size( "" );
    } catch ( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Not Exist Log MaxFileSize Setting.");
    }



    BOOST_MESSAGE( "----- check_max_file_size test end -----" );
}

test_suite* init_unit_test_suite( int argc, char* argv[] ){

   test_suite* test = BOOST_TEST_SUITE( "logger rotate utility class test" );

//    static LOG_ROTATION_TAG check_rotate_type(
//                                const std::string &rotation_type);
   test->add( BOOST_TEST_CASE( &check_rotate_type_test ) );

//    static unsigned int check_max_backup_index(
//                                const std::string &max_backup_index);
   test->add( BOOST_TEST_CASE( &check_max_backup_index_test ) );


//    static unsigned long long check_max_file_size(
//                                const std::string &max_file_size);
   test->add( BOOST_TEST_CASE( &check_max_file_size_test ) );

//    static LOG_ROTATION_TIMING_TAG check_rotate_timing(
//                                const std::string &rotation_timing);
 

//    static std::string check_rotate_timing_value(
//                                const std::string rotation_timing_value_key,
//                                const LOG_ROTATION_TIMING_TAG rotation_timing);


//    static void set_appender(
//                                const appender_property& log_property,
//                                const char *logger_layout,
//                                const std::string &log_category);

//    static bool acccess_log_LogrotateParamCheck(
//                                accesslog_rotate_map_type& rotatedata,
//                                appender_property &access_log_property);

//    static void loglotation_utility_logic_error(
//                                const unsigned int logno, 
//                                const std::string& str, 
//                                const char* file , 
//                                const unsigned int line);


    
    framework::master_test_suite().add( test );
    return NULL;
}


