#define    TEST_CASE

#include <iostream>
#include <sstream>
#include <iomanip>
#include <limits.h>

#include <log4cxx/logmanager.h>
#include <log4cxx/helpers/loglog.h>
#include <log4cxx/rolling/rollingfileappender.h>
#include <log4cxx/rolling/fixedwindowrollingpolicy.h>
#include <log4cxx/rolling/sizebasedtriggeringpolicy.h>
#include <log4cxx/rolling/timebasedrollingpolicy.h>
#include <log4cxx/consoleappender.h>
#include <errno.h>
#include <stdexcept>
#include <boost/lexical_cast.hpp>

#include <dlfcn.h>

#include <boost/test/included/unit_test.hpp>
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "error_code.h"
#include "logger_enum.h"
#include "logger.h"
#include "logger_wrapper.h"

#include "command_receiver_stub.h"
//#include "realserver_stub.h"
//#include "replication_stub.h"
//#include "snmpbridge_stub.h"
//#include "protocol_module_control_stub.h"
//#include "schedule_module_control_stub.h"
#include "utility.h"
//////////////////////////////////

#include "appender_property.h"
#include "logger_implement_access.h"
#include "logger_logrotate_utility.h"

using namespace boost::unit_test_framework;
//using namespace l7vs::logger_implement_access;

typedef std::map< std::string , std::string > accesslog_rotate_map_type;
typedef std::map< std::string , std::string >::iterator accesslog_rotate_map_type_iterator;

namespace l7vs{

// test class
class log_impl_acs_test    :
  public l7vs::logger_implement_access
{
  private:
    bool testflag;
  public:
    log_impl_acs_test(const std::string &access_log_file_name)
            :l7vs::logger_implement_access(access_log_file_name){};
    virtual ~log_impl_acs_test(){};

    bool init(
            const bool                 rotate_default_flag,
            const appender_property&   access_log_default_property,
            accesslog_rotate_map_type& rotatedata);

    inline void putLog(
                  const std::string& vsinfo,
                  const std::string& cl_con_org,
                  const std::string& rs_con_org,
                  const std::string& rs_con_dest,
                  const std::string& msg = "");

    bool setAcLoggerConf(
                        const appender_property& access_log_default_property,
                        accesslog_rotate_map_type& rotatedata);

    bool other();

    //void    addRef(){ l7vs::logger_implement_access::addRef() };

    //void    releaseRef(){ l7vs::logger_implement_access::releaseRef() };

    //bool    operator<=(const int access_num ){  };

    std::string    getAcLogFileName(){ return( this->access_log_file_name_ ); }

    bool    checkRotateParameterComp(accesslog_rotate_map_type &rotatedata);
    
    //bool    is_rotate_default_flag();
    

};
};


// stub glibc functions
//static int        (*org_daemon)(int nochdir, int noclose) = NULL;
int                daemon_ret = 0;

int    daemon( int nochdir, int noclose ){
    std::cout << "daemon" << std::endl;
    return daemon_ret;
}

// handler_wrapper
void*    handler_wrapper( void* );

// flags
int            call_count_test_handler = 0;    //test_handlerの呼出回数
int            arg_sig_test_handler = 0;        //test_handlerの引数(sig)


//util
template< typename InternetProtocol >
boost::asio::ip::basic_endpoint< InternetProtocol > string_to_endpoint( std::string str ){
    std::string::size_type pos = str.find( ":" );
    std::string    hostname = str.substr( 0, pos );
    std::string    portname = str.substr( pos+1, str.length() );
    boost::asio::io_service        io_service;
    typename InternetProtocol::resolver                resolver(io_service);
    typename InternetProtocol::resolver::query        query( hostname, portname );
    typename InternetProtocol::resolver::iterator    end;
    typename InternetProtocol::resolver::iterator    itr = resolver.resolve( query );
    if( itr == end ) return typename InternetProtocol::endpoint();
    return *itr;
}

//--tests--

bool l7vs::log_impl_acs_test::init(
            const bool                 rotate_default_flag,
            const appender_property&   access_log_default_property,
            accesslog_rotate_map_type& rotatedata)
{
    bool result = false;

    //////////////////////////////////////////////////////////////
    // Unit Test パターン１ 正常確認
    testflag = true;
    result =
      l7vs::logger_implement_access::init(
                                    rotate_default_flag,
                                    access_log_default_property,
				    rotatedata);
    // 実行結果確認
    BOOST_CHECK( result );
    // rotate_default_flag 設定正常 :1
    BOOST_CHECK( rotate_default_flag );
    // 内部起動している
    // setAcLoggerConf は、この試験では、確認しない。通常に動作するだけでOK
    // rotate_default_flag 設定正常
    //////////////////////////////////////////////////////////////
    // Unit Test パターン2 Exception確認
    // 試験準備
    testflag = false;

    result =
        l7vs::logger_implement_access::init(
                                    rotate_default_flag,
                                    access_log_default_property,
				    rotatedata);
    // 実行結果確認
    BOOST_CHECK( !result );
    // 内部起動している
    // setAcLoggerConf は、この試験では、確認しない。通常に動作するだけでOK
    // rotate_default_flag 設定正常
    //////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////
    // Unit Test パターン3 initialized済の時は、即時リターン
    // 試験準備
    initialized = true;

    result =
        l7vs::logger_implement_access::init(
                                    rotate_default_flag,
                                    access_log_default_property,
				    rotatedata);
    // 実行結果確認
    BOOST_CHECK( result );
    // 内部起動している
    // setAcLoggerConf は、この試験では、確認しない。通常に動作するだけでOK
    // rotate_default_flag 設定正常
    //////////////////////////////////////////////////////////////

    return(true);

}          

bool l7vs::log_impl_acs_test::setAcLoggerConf(
			const appender_property& access_log_default_property,
                    	accesslog_rotate_map_type& rotatedata)
{
    // 本関数の試験は、２項目でよい。
    // 処理の関係上「rotate_default_flag」の設定値 ture/falseの２パターン
    // で、全てのルートを確認可能
    bool            result = false;
    std::exception e;

    // 試験用に機能追加
    if( !testflag ){
        // 試験用に強制的にexceptionを発生させる。
        testflag = true;
        throw e;
    }
    testflag = true;

    
    //////////////////////////////////////////////////////////////
    // Unit Test パターン4 正常確認
    // rotate_default_flag = true
    rotate_default_flag = true;
    // set_appender実行

    result =
      l7vs::logger_implement_access::setAcLoggerConf(
                                           access_log_default_property,
                                           rotatedata);
    // 実行結果確認
    BOOST_CHECK( result );
    //////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////
    // Unit Test パターン5 正常確認
    // rotate_default_flag = false
    rotate_default_flag = false;
    // acccess_log_LogrotateParamCheck 実行結果 false にする
    // set_appender未実行
    result =
      l7vs::logger_implement_access::setAcLoggerConf(
                                           access_log_default_property,
                                           rotatedata);
    // 実行結果確認
    BOOST_CHECK( !result );
    //////////////////////////////////////////////////////////////

    return(true);

}


bool l7vs::log_impl_acs_test::other()
{
    
    //////////////////////////////////////////////////////////////
    // Unit Test パターン6 正常確認
    // access_cnt = 1
    l7vs::logger_implement_access::addRef();
    // 実行結果確認
    BOOST_CHECK( 2 == access_cnt );
    //////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////
    // Unit Test パターン7 正常確認
    // access_cnt = 2
    l7vs::logger_implement_access::releaseRef();
    // 実行結果確認
    BOOST_CHECK( 1 == access_cnt );
    //////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////
    // Unit Test パターン8 正常確認
    // access_cnt = 2
    std::string name =
                l7vs::logger_implement_access::getAcLogFileName();
std::cout << "filename:" << name << std::endl;
    // 実行結果確認
    BOOST_CHECK_EQUAL( name, "/tmp/dat" );

    return(true);

}

bool l7vs::log_impl_acs_test::checkRotateParameterComp(
                                       accesslog_rotate_map_type &rotatedata)
{
    bool result =false;
    //////////////////////////////////////////////////////////////
    // Unit Test パターン9 正常確認
    rotatedata.insert( std::make_pair("num1", "arg1"));
    rotatedata.insert( std::make_pair("num2", "arg2"));
    rotatedata.insert( std::make_pair("num3", "arg3"));
    rotatedata.insert( std::make_pair("num4", "arg4"));
    rotatedata.insert( std::make_pair("num5", "arg5"));

    aclog_args.insert( std::make_pair("num1", "arg1"));
    aclog_args.insert( std::make_pair("num2", "arg2"));
    aclog_args.insert( std::make_pair("num3", "arg3"));
    aclog_args.insert( std::make_pair("num4", "arg4"));
    aclog_args.insert( std::make_pair("num5", "arg5"));
    result = 
      l7vs::logger_implement_access::checkRotateParameterComp(
                                                   rotatedata);
    // 実行結果確認
    BOOST_CHECK( true == result );
    rotatedata.clear();
    aclog_args.clear();
    //////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////
    // Unit Test パターン10 正常確認
    rotatedata.insert( std::make_pair("num1", "arg1"));
    rotatedata.insert( std::make_pair("num2", "argERR"));
    rotatedata.insert( std::make_pair("num3", "arg3"));
    rotatedata.insert( std::make_pair("num4", "arg4"));
    rotatedata.insert( std::make_pair("num5", "arg5"));

    aclog_args.insert( std::make_pair("num1", "arg1"));
    aclog_args.insert( std::make_pair("num2", "arg2"));
    aclog_args.insert( std::make_pair("num3", "arg3"));
    aclog_args.insert( std::make_pair("num4", "arg4"));
    aclog_args.insert( std::make_pair("num5", "arg5"));
    result = 
      l7vs::logger_implement_access::checkRotateParameterComp(
                                                   rotatedata);
    // 実行結果確認
    BOOST_CHECK( false == result );
    rotatedata.clear();
    aclog_args.clear();
    //////////////////////////////////////////////////////////////
    // Unit Test パターン11 正常確認
    rotatedata.insert( std::make_pair("num1", "arg1"));
    rotatedata.insert( std::make_pair("num2", "arg2"));
    rotatedata.insert( std::make_pair("num3", "arg3"));
    rotatedata.insert( std::make_pair("num4", "arg4"));
    rotatedata.insert( std::make_pair("num5", "arg5"));

    result = 
      l7vs::logger_implement_access::checkRotateParameterComp(
                                                   rotatedata);
    // 実行結果確認
    BOOST_CHECK( false == result );
    rotatedata.clear();
    aclog_args.clear();

    return(true);

}

inline void l7vs::log_impl_acs_test::putLog(
                            const std::string& vsinfo,
                            const std::string& cl_con_org,
                            const std::string& rs_con_org,
                            const std::string& rs_con_dest,
                            const std::string& msg)

{
//#define LOGGER_LAYOUT "%d{%Y/%m/%d %H:%M:%S} [%p] %c %m %t %F:%L%n"
#define LOGGER_LAYOUT "%d{%Y/%m/%d %H:%M:%S} [%p] %m%n"

    log4cxx::LoggerPtr root   = log4cxx::Logger::getRootLogger();
    log4cxx::LayoutPtr layout = new log4cxx::PatternLayout(LOGGER_LAYOUT);
    // create console appender from layout
    log4cxx::WriterAppender* consoleAppender
                 = new log4cxx::ConsoleAppender(
                                            layout,
                                            log4cxx::ConsoleAppender::getSystemErr() );
    root->addAppender(consoleAppender);
    //////////////////////////////////////////////////////////////
    // Unit Test パターン12 正常確認
    l7vs::logger_implement_access::putLog(
                                      vsinfo,
                                      cl_con_org,
                                      rs_con_org,
                                      rs_con_dest,
                                      msg);
    // ログフォーマット:
    //   2008/12/07 20:08:31 [INFO] [[AccessLog] (CL)192.168.2.1 --> 192.168.2.2 --UM-- 192.168.1.101:37259 --> (RS-DST)192.168.1.106:80 ]
    //     (CL)192.168.2.1 
    //         192.168.2.2 
    //         192.168.1.101:37259
    // (RS-DST)192.168.1.106:80 ]
    std::cout << "ログフォーマット 目で確認" << std::endl;
    std::cout << 
       "2008/12/07 20:08:31 [INFO] [ [AccessLog] (CL)192.168.2.1:9898 --> 192.168.2.2:80 --UM-- 192.168.1.101:37259 --> (RS-DST)192.168.1.106:80 msghogehotge-piiiiii]"
          << std::endl;
    //////////////////////////////////////////////////////////////
    // Unit Test パターン13 Exception確認
    l7vs::logger_implement_access::putLog(
				      vsinfo,
                                      cl_con_org,
                                      rs_con_org,
                                      rs_con_dest,
                                      msg);
    // ログフォーマット:
    std::cout << "実行不可" << std::endl;

}
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
void logger_main()
{
  std::string   fname = "/tmp/dat";
  // new
  l7vs::log_impl_acs_test    log_inst(fname);
  l7vs::appender_property    access_log_default_property;
  accesslog_rotate_map_type  rotatedata;

  // test 1,2
  log_inst.init(
		true,
		access_log_default_property,
		rotatedata);

  // test 3,4
  log_inst.setAcLoggerConf(
			   access_log_default_property,
			   rotatedata);

  // test 5,6,7
  log_inst.other();

  // test 8,9,10
  rotatedata.clear();

  log_inst.checkRotateParameterComp(rotatedata);

  log_inst.putLog(
               "192.168.2.2:80",
               "192.168.2.1:9898",
               "192.168.1.101:37259",
               "192.168.1.106:80",
               "msghogehotge-piiiiii");

  // delete log_inst;
}
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
test_suite*    init_unit_test_suite( int argc, char* argv[] ){

  //  org_daemon = (int (*)(int, int)) dlsym(RTLD_NEXT, "daemon");

  test_suite* ts = BOOST_TEST_SUITE( "l7vsd class test" );

  //  l7vs::log_impl_acs_test log_impl_acs_test_instance("/tmp/work");

  //logger_instance.loadConf();

  ts->add( BOOST_TEST_CASE( &logger_main ) );

  return ts;

}
////////////////////////////////////////////////////////////////////
