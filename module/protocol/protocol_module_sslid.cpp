#include "protocol_module_sslid.h"
#include <boost/format.hpp>

#define REALSERVER_CONNECT_FAILED_MAX_COUNT (3)
#define THREAD_DIVISION_UP_STREAM (0)                          // up thread
#define THREAD_DIVISION_DOWN_STREAM (1)                    // down thread
#define END_FLAG_ON (1)                                                 // end flag ON
#define END_FLAG_OFF (0)                                                // end flag OFF

namespace l7vs
{

const std::string protocol_module_sslid::MODULE_NAME = "sslid";

protocol_module_sslid::protocol_module_sslid():
  ssl_protocol_module_base( MODULE_NAME ),
    realserver_connect_failed_max_count( REALSERVER_CONNECT_FAILED_MAX_COUNT ),
  replication_data_processor( NULL ),
  session_data_processor( NULL )
{
    //ctor
    /*-------- DEBUG LOG --------*/
    if ( LOG_LV_DEBUG == getloglevel() )
    {
        putLogDebug( 030000, "in_function: Constructor protocol_module_sslid::"
                            "protocol_module_sslid()", __FILE__, __LINE__ );

        boost::format formatter( "out_function: protocol_module_sslid::protocol_module_sslid(): "
                                            "modename=%s, realserver_connect_failed_max_count=%d" );
        formatter % MODULE_NAME % realserver_connect_failed_max_count;
        putLogDebug( 030001, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/
}

protocol_module_sslid::~protocol_module_sslid()
{
    //dtor
    /*-------- DEBUG LOG --------*/
    if ( LOG_LV_DEBUG == getloglevel() )
    {
        putLogDebug( 030002, "in/out_function: Destructor protocol_module_sslid::"
                            "~protocol_module_sslid()", __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/
}

bool protocol_module_sslid::is_tcp()
{
    /*-------- DEBUG LOG --------*/
    if ( LOG_LV_DEBUG == getloglevel() )
    {
        putLogDebug( 030002, "in/out_function: bool protocol_module_sslid::is_tcp(): "
                            "return_value=true", __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    return true;
}

bool protocol_module_sslid::is_udp()
{
    /*-------- DEBUG LOG --------*/
    if ( LOG_LV_DEBUG == getloglevel() )
    {
        putLogDebug( 030002, "in/out_function: bool protocol_module_sslid::is_udp(): "
                            "return_value=false", __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    return false;
}

void protocol_module_sslid::initialize(
                                rs_list_itr_func_type inlist_begin,
                                rs_list_itr_func_type inlist_end,
                                rs_list_itr_func_type inlist_next,
                                boost::function< void( void ) >	inlist_lock,
                                boost::function< void( void ) >	inlist_unlock )
{
    /*-------- DEBUG LOG --------*/
    if ( LOG_LV_DEBUG == getloglevel() )
    {
        putLogDebug( 030003, "in_function: void protocol_module_sslid::initialize("
                            "rs_list_itr_func_type inlist_begin, rs_list_itr_func_type inlist_end,"
                            "rs_list_itr_func_type inlist_next,boost::function< void( void ) >	"
                            "inlist_lock,boost::function< void( void ) >	inlist_unlock )" ,
                            __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    rs_list_begin = inlist_begin;
    rs_list_end = inlist_end;
    rs_list_next = inlist_next;
    rs_list_lock = inlist_lock;
    rs_list_unlock = inlist_unlock;

    /*-------- DEBUG LOG --------*/
    if ( LOG_LV_DEBUG == getloglevel() )
    {
        putLogDebug( 030004, "out_function: void protocol_module_sslid::initialize("
                            "rs_list_itr_func_type inlist_begin,rs_list_itr_func_type inlist_end,"
                            "rs_list_itr_func_type inlist_next,boost::function< void( void ) >	"
                            "inlist_lock,boost::function< void( void ) >	inlist_unlock ) ",
                            __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/
}

void protocol_module_sslid::finalize()
{
    /*-------- DEBUG LOG --------*/
    if ( LOG_LV_DEBUG == getloglevel() )
    {
        putLogDebug( 030000, "int_function: void protocol_module_sslid::finalize()",
                            __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

	// realserver list function object initialize
	rs_list_begin.clear();
	rs_list_end.clear();
	rs_list_next.clear();
	rs_list_lock.clear();
	rs_list_unlock.clear();

	/*-------- DEBUG LOG --------*/
    if ( LOG_LV_DEBUG == getloglevel() )
    {
        putLogDebug( 030000, "function: rs_list_begin.clear(), rs_list_end.clear(), "
                            "rs_list_next.clear(), rs_list_lock.clear(), rs_list_unlock.clear() END" ,
                            __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

	// replication function object initialize
	replication_pay_memory.clear();
    replication_area_lock.clear();
    replication_area_unlock.clear();

    /*-------- DEBUG LOG --------*/
    if ( LOG_LV_DEBUG == getloglevel() )
    {
        putLogDebug( 030000, "function: replication_pay_memory.clear(), "
                            "replication_area_lock.clear(), replication_area_unlock.clear() END" ,
                            __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    // scheduleModule function object initialize
    schedule_tcp.clear();

    /*-------- DEBUG LOG --------*/
    if ( LOG_LV_DEBUG == getloglevel() )
    {
        putLogDebug( 030000, "function: schedule_tcp.clear() END", __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    // module option initialize
    timeout = 0;
    maxlist = 0;
    reschedule = 0;

    // replication initialize
    if ( replication_data_processor )
    {
        /*-------- DEBUG LOG --------*/
        if ( LOG_LV_DEBUG == getloglevel() )
        {
            boost::format formatter("delete: address=&(%d)" );
            formatter % &replication_data_processor;
            putLogDebug( 030000, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        delete replication_data_processor;
        replication_data_processor = NULL;
    }

    // session initialize
    if ( session_data_processor )
    {
        /*-------- DEBUG LOG --------*/
        if ( LOG_LV_DEBUG == getloglevel() )
        {
            boost::format formatter("delete: address=&(%d)" );
            formatter % &session_data_processor;
            putLogDebug( 030000, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        delete session_data_processor;
        session_data_processor = NULL;
    }

    /*-------- DEBUG LOG --------*/
    if ( LOG_LV_DEBUG == getloglevel() )
    {
        putLogDebug( 030000, "out_function: void protocol_module_sslid::finalize()",
                            __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    // logger function object initialize
    getloglevel.clear();
	putLogFatal.clear();
    putLogError.clear();
	putLogWarn.clear();
	putLogInfo.clear();
	putLogDebug.clear();
}

bool protocol_module_sslid::is_use_sorry()
{
    /*-------- DEBUG LOG --------*/
    if ( LOG_LV_DEBUG == getloglevel() )
    {
        putLogDebug( 030002, "in/out_function: bool protocol_module_sslid::is_use_sorry(): "
                            "return_value=false", __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    return false;
}

protocol_module_sslid::check_message_result protocol_module_sslid::check_parameter(
                                                const std::vector<std::string>& args )
{
    /*-------- DEBUG LOG --------*/
    if ( LOG_LV_DEBUG == getloglevel() )
    {
        boost::format formatter( "int_function: protocol_module_sslid::check_message_result "
                                            "protocol_module_sslid::check_parameter("
                                            "const std::vector<std::string>& args ): args=%s" );
        std::string argsdump;
        for ( std::vector<std::string>::const_iterator it = args.begin(); it != args.end(); ++it )
        {
            argsdump += *it;
        }
        formatter % argsdump;
        putLogDebug( 030000, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    check_message_result result;
    // set check flag to true
    result.flag = true;
    bool timeout_flag = false;               // timeout set flag
    bool maxlist_flag = false;               // maxlist set flag
    bool reschedule_flag = false;         // reschedule set flag
    bool no_reschedule_flag = false;    // no-reschedule set flag

    try
    {
        std::vector<std::string>::const_iterator itparam;
        boost::format formatter;

        for ( itparam = args.begin(); itparam != args.end(); ++itparam )
        {
            if ( *itparam == "-T" || *itparam == "--timeout" )
            {
                // timeout
                if ( !timeout_flag )
                {
                    // not set timeout option
                    ++itparam;
                    // next parameter exist check
                    if ( itparam != args.end() )
                    {
                        // next parameter exist
                        try
                        {
                            unsigned long ultimeout = 0;
                            ultimeout = boost::lexical_cast<unsigned long>( *itparam );
                            // int maxvalue check
                            if ( ultimeout > INT_MAX )
                            {
                                result.flag = false;
                                formatter.parse( "'-T/--timeout' option value '%s' is too large." );
                                formatter % *itparam;
                                result.message = formatter.str();
                                putLogError( 037000, result.message, __FILE__, __LINE__ );
                                break;
                            }
                            else
                            {
                                timeout_flag = true;
                                continue;
                            }
                        }
                        catch ( boost::bad_lexical_cast& e )
                        {
                            // not numeric character
                            result.flag = false;
                            formatter.parse( "'-T/--timeout' option value '%s' is not numeric character." );
                            formatter % *itparam;
                            result.message = formatter.str();
                            putLogError( 037000, result.message, __FILE__, __LINE__ );
                            break;
                        }
                    }
                    else
                    {
                        // next parameter not exist
                        break;
                    }
                }
                else
                {
                    // already set timeout
                    result.flag = false;
                    result.message = "Cannot set multiple option '--timeout/-T'.";
                    putLogError( 037000, result.message, __FILE__, __LINE__ );
                    break;
                }
            }
            else if ( *itparam == "-M" || *itparam == "--maxlist" )
            {
                // maxlist
                if ( !maxlist_flag )
                {
                    // not set maxlist option
                    ++itparam;
                    // next parameter exist check
                    if ( itparam != args.end() )
                    {
                        // next parameter exist
                        try
                        {
                            unsigned long ulmaxlist = 0;
                            ulmaxlist = boost::lexical_cast<unsigned long>( *itparam );
                            // int maxvalue check
                            if ( ulmaxlist > INT_MAX )
                            {
                                result.flag = false;
                                formatter.parse( "'-M/--maxlist' option value '%s' is too large." );
                                formatter % *itparam;
                                result.message = formatter.str();
                                putLogError( 037000, result.message, __FILE__, __LINE__);
                                break;
                            }
                            else
                            {
                                maxlist_flag = true;
                                continue;
                            }
                        }
                        catch ( boost::bad_lexical_cast& e )
                        {
                            // not numeric character
                            result.flag = false;
                            formatter.parse("'-M/--maxlist' option value '%s' is not numeric character.");
                            formatter % *itparam;
                            result.message = formatter.str();
                            putLogError( 037000, result.message, __FILE__, __LINE__);
                            break;
                        }
                    }
                    else
                    {
                        // next parameter not exist
                        break;
                    }
                }
                else
                {
                    // already set timeout
                    result.flag = false;
                    result.message = "Cannot set multiple option '-M/--maxlist'.";
                    putLogError( 037000, result.message, __FILE__, __LINE__);
                    break;
                }
            }
            else if ( *itparam == "-R" || *itparam == "--reschedule" )
            {
                // reschedule
                if ( !no_reschedule_flag )
                {
                    // not set no-reschedule flag
                    reschedule_flag = true;
                }
                else
                {
                    // already set no-reschedule flag
                    result.flag = false;
                    result.message = "You have to choose either of reschdule or no-reschedule.";
                    putLogError( 037000, result.message, __FILE__, __LINE__);
                    break;
                }
            }
            else if  ( *itparam == "-N" || *itparam == "--no-reschedule" )
            {
                // no-reschedule
                if ( !reschedule_flag )
                {
                    // not set reschedule flag
                    no_reschedule_flag = true;
                }
                else
                {
                    // already set reshcedule flag
                    result.flag = false;
                    result.message = "You have to choose either of reschdule or no-reschedule.";
                    putLogError( 037000, result.message, __FILE__, __LINE__);
                    break;
                }
            }
            else
            {
                // check result is false
                result.flag = false;
                result.message = "Option error.";
                putLogError( 037000, result.message, __FILE__, __LINE__);
                break;
            }
        }
    }
    catch ( const std::exception& e )
    {
        result.flag = false;
        std::cerr << "check parameter exception: result=false, error=" << e.what() << "." << std::endl;
        boost::format formatter( "function protocol_module_sslid::check_message_result "
                                            "protocol_module_sslid::check_parameter() exception: "
                                            "result=false, error=%s." );
        formatter % e.what();
        putLogError( 037000, formatter.str(), __FILE__, __LINE__ );
    }
    catch( ... )
    {
        result.flag = false;
        std::cerr << "Unkown exception." << std::endl;
        putLogError( 037000, "function protocol_module_sslid::check_message_result "
                        "protocol_module_sslid::check_parameter() :"
                        "Unkown exception.", __FILE__, __LINE__ );
    }

    /*-------- DEBUG LOG --------*/
    if ( LOG_LV_DEBUG == getloglevel() )
    {
        boost::format formatter( "out_function: protocol_module_sslid::check_message_result "
                                            "protocol_module_sslid::check_parameter("
                                            "const std::vector<std::string>& args ): return_value=("
                                            "check_message_result.flag=%d, check_message_result.message=%s" );
        formatter % result.flag % result.message;
        putLogDebug( 030000, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    return result;
}

protocol_module_sslid::check_message_result protocol_module_sslid::set_parameter(
                                                const std::vector<std::string>& args )
{
    /*-------- DEBUG LOG --------*/
    if ( LOG_LV_DEBUG == getloglevel() )
    {
        boost::format formatter( "int_function: protocol_module_sslid::check_message_result "
                                            "protocol_module_sslid::set_parameter("
                                            "const std::vector<std::string>& args ): args=%s" );
        std::string argsdump;
        for ( std::vector<std::string>::const_iterator it = args.begin(); it != args.end(); ++it )
        {
            argsdump += *it;
        }
        formatter % argsdump;
        putLogDebug( 030000, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    check_message_result result;
    // set check flag to true
    result.flag = true;
    bool timeout_flag = false;               // timeout set flag
    bool maxlist_flag = false;               // maxlist set flag
    bool reschedule_flag = false;         // reschedule set flag
    bool no_reschedule_flag = false;    // no-reschedule set flag

    try
    {
        std::vector<std::string>::const_iterator itparam;
        boost::format formatter;

        for ( itparam = args.begin(); itparam != args.end(); ++itparam )
        {
            if ( *itparam == "-T" || *itparam == "--timeout" )
            {
                // timeout
                if ( !timeout_flag )
                {
                    // not set timeout option
                    ++itparam;
                    // next parameter exist check
                    if ( itparam != args.end() )
                    {
                        // next parameter exist
                        try
                        {
                            unsigned long ultimeout = 0;
                            ultimeout = boost::lexical_cast<unsigned long>( *itparam );
                            // int maxvalue check
                            if ( ultimeout > INT_MAX )
                            {
                                result.flag = false;
                                formatter.parse( "'-T/--timeout' option value '%s' is too large." );
                                formatter % *itparam;
                                result.message = formatter.str();
                                putLogError( 037000, result.message, __FILE__, __LINE__ );
                                break;
                            }
                            else
                            {
                                timeout = ultimeout;
                                timeout_flag = true;
                                continue;
                            }
                        }
                        catch ( boost::bad_lexical_cast& e )
                        {
                            // not numeric character
                            result.flag = false;
                            formatter.parse( "'-T/--timeout' option value '%s' is not numeric character." );
                            formatter % *itparam;
                            result.message = formatter.str();
                            putLogError( 037000, result.message, __FILE__, __LINE__ );
                            break;
                        }
                    }
                    else
                    {
                        // next parameter not exist
                        break;
                    }
                }
                else
                {
                    // already set timeout
                    result.flag = false;
                    result.message = "Cannot set multiple option '--timeout/-T'.";
                    putLogError( 037000, result.message, __FILE__, __LINE__ );
                    break;
                }
            }
            else if ( *itparam == "-M" || *itparam == "--maxlist" )
            {
                // maxlist
                if ( !maxlist_flag )
                {
                    // not set maxlist option
                    ++itparam;
                    // next parameter exist check
                    if ( itparam != args.end() )
                    {
                        // next parameter exist
                        try
                        {
                            unsigned long ulmaxlist = 0;
                            ulmaxlist = boost::lexical_cast<unsigned long>( *itparam );
                            // int maxvalue check
                            if ( ulmaxlist > INT_MAX )
                            {
                                result.flag = false;
                                formatter.parse( "'-M/--maxlist' option value '%s' is too large." );
                                formatter % *itparam;
                                result.message = formatter.str();
                                putLogError( 037000, result.message, __FILE__, __LINE__ );
                                break;
                            }
                            else
                            {
                                maxlist = ulmaxlist;
                                maxlist_flag = true;
                                continue;
                            }
                        }
                        catch ( boost::bad_lexical_cast& e )
                        {
                            // not numeric character
                            result.flag = false;
                            formatter.parse("'-M/--maxlist' option value '%s' is not numeric character.");
                            formatter % *itparam;
                            result.message = formatter.str();
                            putLogError( 037000, result.message, __FILE__, __LINE__ );
                            break;
                        }
                    }
                    else
                    {
                        // next parameter not exist
                        break;
                    }
                }
                else
                {
                    // already set timeout
                    result.flag = false;
                    result.message = "Cannot set multiple option '-M/--maxlist'.";
                    putLogError( 037000, result.message, __FILE__, __LINE__ );
                    break;
                }
            }
            else if ( *itparam == "-R" || *itparam == "--reschedule" )
            {
                // reschedule
                if ( !no_reschedule_flag )
                {
                    // not set no-reschedule flag
                    reschedule_flag = true;
                    reschedule = 1;
                }
                else
                {
                    // already set no-reschedule flag
                    result.flag = false;
                    result.message = "You have to choose either of reschdule or no-reschedule.";
                    putLogError( 037000, result.message, __FILE__, __LINE__ );
                    break;
                }
            }
            else if  ( *itparam == "-N" || *itparam == "--no-reschedule" )
            {
                // no-reschedule
                if ( !reschedule_flag )
                {
                    // not set reschedule flag
                    no_reschedule_flag = true;
                    reschedule = 0;
                }
                else
                {
                    // already set reshcedule flag
                    result.flag = false;
                    result.message = "You have to choose either of reschdule or no-reschedule.";
                    putLogError( 037000, result.message, __FILE__, __LINE__ );
                    break;
                }
            }
            else
            {
                // check result is false
                result.flag = false;
                result.message = "Option error.";
                putLogError( 037000, result.message, __FILE__, __LINE__ );
                break;
            }
        }

        // result check
        if ( result.flag )
        {
            // timeout default set
            if ( !timeout_flag )
            {
                timeout = 3600;
            }

            // maxlist default set
            if ( !maxlist_flag )
            {
                maxlist = 1024;
            }

            // reschedule default set
            if ( !reschedule_flag )
            {
                reschedule = 0;
            }
        }

        /*-------- DEBUG LOG --------*/
        if ( LOG_LV_DEBUG == getloglevel() )
        {
            boost::format formatter( "function: protocol_module_sslid::check_message_result "
                                                "protocol_module_sslid::set_parameter("
                                                "const std::vector<std::string>& args ): timeout=%d,"
                                                "maxlist=%d, reschedule=%d" );
            formatter % timeout % maxlist % reschedule;
            putLogDebug( 030000, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        unsigned int data_size = 0;
        void* data_addr = NULL;
        data_addr = replication_pay_memory( get_name(), &data_size );

        /*-------- DEBUG LOG --------*/
        if ( LOG_LV_DEBUG == getloglevel() )
        {
            putLogDebug( 030000, "function: protocol_module_sslid::check_message_result "
                                 "protocol_module_sslid::set_parameter(): "
                                 "replication_pay_memory() END.", __FILE__, __LINE__ );
            boost::format formatter( "function: protocol_module_sslid::check_message_result "
                                                "protocol_module_sslid::set_parameter(): "
                                                "data_addr=&(%d), data_size=%d." );
            formatter % &data_addr % data_size;
            putLogDebug( 030000, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        if ( data_addr == NULL || data_size <= 0 )
        {
            // replication area is full
            putLogError( 037000, "Replication area is full.", __FILE__, __LINE__);

            /*-------- DEBUG LOG --------*/
            if ( LOG_LV_DEBUG == getloglevel() )
            {
                boost::format formatter( "out_function: protocol_module_sslid::check_message_result "
                                                    "protocol_module_sslid::set_parameter("
                                                    "const std::vector<std::string>& args ): return_value="
                                                    "check_message_result.flag=%d, check_message_result.message=%s" );
                formatter % result.flag % result.message;
                putLogDebug( 030000, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/

            return result;
        }
        else
        {
                // create sslid_replication_data_processor
                replication_data_processor = new sslid_replication_data_processor(
                                                                                    maxlist,
                                                                                    static_cast<char*>(data_addr),
                                                                                    data_size,
                                                                                    virtual_service_endpoint_tcp,
                                                                                    getloglevel,
                                                                                    putLogFatal,
                                                                                    putLogError,
                                                                                    putLogWarn,
                                                                                    putLogInfo,
                                                                                    putLogDebug  );

            /*-------- DEBUG LOG --------*/
            if ( LOG_LV_DEBUG == getloglevel() )
            {
                boost::format formatter("new: address=&(%d), size=%lu." );
                formatter % &replication_data_processor % sizeof( sslid_session_data_processor );
                putLogDebug( 030000, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/

            replication_data_processor->register_replication_area_lock( replication_area_lock );
            /*-------- DEBUG LOG --------*/
            if ( LOG_LV_DEBUG == getloglevel() )
            {
                putLogDebug( 030000, "protocol_module_sslid::check_message_result "
                                    "protocol_module_sslid::set_parameter(): register_replication_area_lock() END.",
                                    __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/

            replication_data_processor->register_replication_area_unlock( replication_area_unlock );
            /*-------- DEBUG LOG --------*/
            if ( LOG_LV_DEBUG == getloglevel() )
            {
                putLogDebug( 030000, "protocol_module_sslid::check_message_result "
                                    "protocol_module_sslid::set_parameter(): register_replication_area_unlock() END.",
                                    __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/

            // create sslid_session_data_processor
            session_data_processor = new sslid_session_data_processor( maxlist,
                                                                                                       timeout,
                                                                                                       replication_data_processor,
                                                                                                       getloglevel,
                                                                                                       putLogFatal,
                                                                                                       putLogError,
                                                                                                       putLogWarn,
                                                                                                       putLogInfo,
                                                                                                       putLogDebug );

            /*-------- DEBUG LOG --------*/
            if ( LOG_LV_DEBUG == getloglevel() )
            {
                boost::format formatter("new: address=&(%d), size=%lu." );
                formatter % &session_data_processor % sizeof( sslid_session_data_processor );
                putLogDebug( 030000, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/

            // restore data from replication area
            sslid_replication_data* redata = replication_data_processor->get_replication_area();
            /*-------- DEBUG LOG --------*/
            if ( LOG_LV_DEBUG == getloglevel() )
            {
                putLogDebug( 030000, "protocol_module_sslid::check_message_result "
                                    "protocol_module_sslid::set_parameter(): get_replication_area() END.",
                                    __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/

            replication_area_lock();
            /*-------- DEBUG LOG --------*/
            if ( LOG_LV_DEBUG == getloglevel() )
            {
                putLogDebug( 030000, "protocol_module_sslid::check_message_result "
                                    "protocol_module_sslid::set_parameter(): replication_area_lock() END.",
                                    __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/

            session_data_processor->read_session_data_from_replication_area( redata );
            /*-------- DEBUG LOG --------*/
            if ( LOG_LV_DEBUG == getloglevel() )
            {
                putLogDebug( 030000, "protocol_module_sslid::check_message_result "
                                    "protocol_module_sslid::set_parameter(): "
                                    "read_session_data_from_replication_area() END.",
                                    __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/

            replication_area_unlock();
            /*-------- DEBUG LOG --------*/
            if ( LOG_LV_DEBUG == getloglevel() )
            {
                putLogDebug( 030000, "protocol_module_sslid::check_message_result "
                                    "protocol_module_sslid::set_parameter(): replication_area_unlock() END.",
                                    __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/
        }
    }
    catch ( const std::bad_alloc& ba )
    {
        if ( replication_data_processor )
        {
            delete replication_data_processor;
            replication_data_processor = NULL;
        }

        if ( session_data_processor )
        {
            delete session_data_processor;
            session_data_processor = NULL;
        }

        std::cerr << "set parameter exception: Could not allocate memory." << std::endl;
        result.flag = false;
        result.message = "Could not allocate memory.";
        putLogError( 037000, result.message, __FILE__, __LINE__ );
    }
    catch ( const std::exception& e )
    {
        if ( replication_data_processor )
        {
            delete replication_data_processor;
            replication_data_processor = NULL;
        }

        if ( session_data_processor )
        {
            delete session_data_processor;
            session_data_processor = NULL;
        }

        result.flag = false;
        std::cerr << "set parameter exception: result=false, error=" << e.what() << std::endl;
        boost::format formatter( "function protocol_module_sslid::check_message_result "
                                            "protocol_module_sslid::set_parameter() exception: "
                                            "result=false, error=%s." );
        formatter % e.what();
        putLogError( 037000, formatter.str(), __FILE__, __LINE__ );
    }
    catch( ... )
    {
        if ( replication_data_processor )
        {
            delete replication_data_processor;
            replication_data_processor = NULL;
        }

        if ( session_data_processor )
        {
            delete session_data_processor;
            session_data_processor = NULL;
        }

        result.flag = false;
        std::cerr << "Unkown exception." << std::endl;
        putLogError( 037000, "function protocol_module_sslid::check_message_result "
                        "protocol_module_sslid::check_parameter() : "
                        "Unkown exception.", __FILE__, __LINE__ );
    }

    /*-------- DEBUG LOG --------*/
    if ( LOG_LV_DEBUG == getloglevel() )
    {
        boost::format formatter( "out_function: protocol_module_sslid::check_message_result "
                                            "protocol_module_sslid::set_parameter("
                                            "const std::vector<std::string>& args ): return_value="
                                            "check_message_result.flag=%d, check_message_result.message=%s" );
        formatter % result.flag % result.message;
        putLogDebug( 030000, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    return result;
}

protocol_module_sslid::check_message_result protocol_module_sslid::add_parameter(
                                                const std::vector<std::string>& args )
{
    /*-------- DEBUG LOG --------*/
    if ( LOG_LV_DEBUG == getloglevel() )
    {
        boost::format formatter( "int_function: protocol_module_sslid::check_message_result "
                                            "protocol_module_sslid::add_parameter("
                                            "const std::vector<std::string>& args ): args=%s" );
        std::string argsdump;
        for ( std::vector<std::string>::const_iterator it = args.begin(); it != args.end(); ++it )
        {
            argsdump += *it;
        }
        formatter % argsdump;
        putLogDebug( 030000, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    check_message_result result;
    result.flag = true;

    if ( args.size() > 0 )
    {
        // option exist
        result.flag = false;
        result.message = "Cannot add option.";
        putLogError( 037000, result.message, __FILE__, __LINE__ );
    }

    /*-------- DEBUG LOG --------*/
    if ( LOG_LV_DEBUG == getloglevel() )
    {
        boost::format formatter( "out_function: protocol_module_sslid::check_message_result "
                                            "protocol_module_sslid::add_parameter("
                                            "const std::vector<std::string>& args ): return_value="
                                            "check_message_result.flag=%d, check_message_result.message=%s" );
        formatter % result.flag % result.message;
        putLogDebug( 030000, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    return result;
}

void protocol_module_sslid::handle_rslist_update()
{
    /*-------- DEBUG LOG --------*/
    if ( LOG_LV_DEBUG == getloglevel() )
    {
        putLogDebug( 030002, "in/out_function:void protocol_module_sslid::handle_rslist_update(): "
                            "return_value=void", __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/
}

void protocol_module_sslid::register_schedule( tcp_schedule_func_type inschedule )
{
    /*-------- DEBUG LOG --------*/
    if ( LOG_LV_DEBUG == getloglevel() )
    {
        putLogDebug( 030000, "in_function: void protocol_module_sslid::register_schedule( "
                            "tcp_schedule_func_type inschedule )", __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    schedule_tcp = inschedule;

    /*-------- DEBUG LOG --------*/
    if ( LOG_LV_DEBUG == getloglevel() )
    {
        putLogDebug( 030000, "out_function: void protocol_module_sslid::register_schedule( "
                            "tcp_schedule_func_type inschedule )", __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/
}

void protocol_module_sslid::register_schedule( udp_schedule_func_type inschedule )
{
    /*-------- DEBUG LOG --------*/
    if ( LOG_LV_DEBUG == getloglevel() )
    {
        putLogDebug( 030002, "in/out_function:void protocol_module_sslid::register_schedule(): "
                            "return_value=void", __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/
}

protocol_module_base::EVENT_TAG protocol_module_sslid::handle_session_initialize(
                                                const boost::thread::id up_thread_id,
                                                const boost::thread::id down_thread_id )
{
    /*-------- DEBUG LOG --------*/
    if ( LOG_LV_DEBUG == getloglevel() )
    {
        boost::format formatter( "in_function: protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                            "handle_session_initialize( const boost::thread::id up_thread_id, "
                                            "const boost::thread::id down_thread_id ): "
                                            "up_thread_id=%d, down_thread_id=%d" );
        formatter % up_thread_id % down_thread_id;
        putLogDebug( 030000, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    EVENT_TAG status = ACCEPT;
    session_thread_data_sslid* threaddata_up = NULL;
    session_thread_data_sslid* threaddata_down = NULL;

    try
    {
        // initialize the up/down thread data
        threaddata_up = new session_thread_data_sslid;

        /*-------- DEBUG LOG --------*/
        if ( LOG_LV_DEBUG == getloglevel() )
        {
            boost::format formatter("new: address=&(%d), size=%lu" );
            formatter % &threaddata_up % sizeof( session_thread_data_sslid );
            putLogDebug( 030000, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        threaddata_up->realserver_connect_failed_count = 0;
        threaddata_up->data_begain_offset = 0;
        threaddata_up->data_size = 0;
        threaddata_up->current_record_rest_size = 0;
        threaddata_up->hello_message_flag = false;
        threaddata_up->end_flag = END_FLAG_OFF;                                     // set end_flag to OFF
        threaddata_up->thread_division = THREAD_DIVISION_UP_STREAM;     // up thread division
        threaddata_up->pair_thread_id = down_thread_id;

        /*-------- DEBUG LOG --------*/
        if ( LOG_LV_DEBUG == getloglevel() )
        {
            // data dump
            boost::format formatter("function: protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                    "handle_session_initialize() : session_thread_data_sslid(upthread) : "
                                    "realserver_connect_failed_count=%d, data_begain_offset=%d, "
                                    "data_size=%d, current_record_rest_size=%d, hello_message_flag=%d,"
                                    "end_flag=%d, thread_division=%d, pair_thread_id=%d.");
            formatter % threaddata_up->realserver_connect_failed_count
                          % threaddata_up->data_begain_offset
                          % threaddata_up->data_size
                          % threaddata_up->current_record_rest_size
                          % threaddata_up->hello_message_flag
                          % threaddata_up->end_flag
                          % threaddata_up->thread_division
                          % threaddata_up->pair_thread_id;
            putLogDebug( 030000, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        threaddata_down = new session_thread_data_sslid;

        /*-------- DEBUG LOG --------*/
        if ( LOG_LV_DEBUG == getloglevel() )
        {
            boost::format formatter("new: address=&(%d), size=%lu" );
            formatter % &threaddata_down % sizeof( session_thread_data_sslid );
            putLogDebug( 030000, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        threaddata_down->realserver_connect_failed_count = 0;
        threaddata_down->data_begain_offset = 0;
        threaddata_down->data_size = 0;
        threaddata_down->current_record_rest_size = 0;
        threaddata_down->hello_message_flag = false;
        threaddata_down->end_flag = END_FLAG_OFF;                                       // set end_flag to OFF
        threaddata_down->thread_division = THREAD_DIVISION_DOWN_STREAM; // down thread division
        threaddata_down->pair_thread_id = up_thread_id;

        /*-------- DEBUG LOG --------*/
        if ( LOG_LV_DEBUG == getloglevel() )
        {
            // data_dump
            boost::format formatter( "function: protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                                "handle_session_initialize() : session_thread_data_sslid(downthread) : "
                                                "realserver_connect_failed_count=%d, data_begain_offset=%d, "
                                                "data_size=%d, current_record_rest_size=%d, hello_message_flag=%d,"
                                                "end_flag=%d, thread_division=%d, pair_thread_id=%d.");
            formatter % threaddata_down->realserver_connect_failed_count
                          % threaddata_down->data_begain_offset
                          % threaddata_down->data_size
                          % threaddata_down->current_record_rest_size
                          % threaddata_down->hello_message_flag
                          % threaddata_down->end_flag
                          % threaddata_down->thread_division
                          % threaddata_down->pair_thread_id;
            putLogDebug( 030000, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        {
            // insert the up/down thread data into the map
            boost::mutex::scoped_lock sclock( session_thread_data_map_mutex );
            session_thread_data_map[up_thread_id] = threaddata_up;
            session_thread_data_map[down_thread_id] = threaddata_down;
        }
    }
    catch ( const std::bad_alloc& ba )
    {
        if ( threaddata_up )
        {
            delete threaddata_up;
            threaddata_up = NULL;
        }

        if ( threaddata_down )
        {
            delete threaddata_down;
            threaddata_down = NULL;
        }

        std::cerr << "handle_session_initialize exception: Could not allocate memory." << std::endl;
        putLogError( 037000, "Could not allocate memory.", __FILE__, __LINE__ );

        status = FINALIZE;
    }
    catch ( const std::exception& e )
    {
        if ( threaddata_up )
        {
            delete threaddata_up;
            threaddata_up = NULL;
        }

        if ( threaddata_down )
        {
            delete threaddata_down;
            threaddata_down = NULL;
        }

        std::cerr << "handle_session_initialize exception: error=%s" << e.what() << "." << std::endl;
        boost::format formatter( "function: protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                            "handle_session_initialize() exception: result=%d, error=%s." );
        formatter % e.what();
        putLogError( 037000, formatter.str(), __FILE__, __LINE__ );

        status = FINALIZE;
    }
    catch( ... )
    {
        if ( threaddata_up )
        {
            delete threaddata_up;
            threaddata_up = NULL;
        }

        if ( threaddata_down )
        {
            delete threaddata_down;
            threaddata_down = NULL;
        }

        std::cerr << "Unkown exception." << std::endl;
        putLogError( 037000, "function protocol_module_sslid::check_message_result "
                        "protocol_module_sslid::handle_session_initialize() : "
                        "Unkown exception.", __FILE__, __LINE__ );

        status = FINALIZE;
    }

    /*-------- DEBUG LOG --------*/
    if ( LOG_LV_DEBUG == getloglevel() )
    {
        boost::format formatter( "out_function: protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                            "handle_session_initialize( const boost::thread::id up_thread_id,"
                                            "const boost::thread::id down_thread_id ): return_value=%d" );
        formatter % status;
        putLogDebug( 030000, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    return status;
}

protocol_module_base::EVENT_TAG protocol_module_sslid::handle_session_finalize(
                                                const boost::thread::id up_thread_id,
                                                const boost::thread::id down_thread_id )
{
    /*-------- DEBUG LOG --------*/
    if ( LOG_LV_DEBUG == getloglevel() )
    {
        boost::format formatter( "in_function: protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                            "handle_session_finalize( const boost::thread::id up_thread_id, "
                                            "const boost::thread::id down_thread_id ): "
                                            "up_thread_id=%d, down_thread_id=%d" );
        formatter % up_thread_id % down_thread_id;
        putLogDebug( 030000, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    try
    {
        // delete the up/down thread data from the map
        boost::mutex::scoped_lock sclock( session_thread_data_map_mutex );

        session_thread_data_map_type::iterator itthread;
        itthread = session_thread_data_map.find( up_thread_id );
        if ( itthread != session_thread_data_map.end() )
        {
            /*-------- DEBUG LOG --------*/
            if ( LOG_LV_DEBUG == getloglevel() )
            {
                boost::format formatter("delete: address=&(%d)" );
                formatter % &itthread->second;
                putLogDebug( 030000, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/

            delete itthread->second;
            itthread->second = NULL;
            session_thread_data_map.erase( itthread );
        }

        itthread = session_thread_data_map.find( down_thread_id );
        if ( itthread != session_thread_data_map.end() )
        {
            /*-------- DEBUG LOG --------*/
            if ( LOG_LV_DEBUG == getloglevel() )
            {
                boost::format formatter("delete: address=&(%d)" );
                formatter % &itthread->second;
                putLogDebug( 030000, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/

            delete itthread->second;
            itthread->second = NULL;
            session_thread_data_map.erase( itthread );
        }
    }
    catch ( const std::exception& e )
    {
        std::cerr << "handle_session_finalize exception: error=%s" << e.what() << "." << std::endl;
        boost::format formatter( "function: protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                            "handle_session_finalize() exception: result=%d, error=%s." );
        formatter % STOP % e.what();
        putLogError( 037000, formatter.str(), __FILE__, __LINE__ );
    }
    catch( ... )
    {
        std::cerr << "Unkown exception." << std::endl;
        putLogError( 037000, "function protocol_module_sslid::check_message_result "
                        "protocol_module_sslid::handle_session_finalize() : "
                        "Unkown exception.", __FILE__, __LINE__ );
    }

    /*-------- DEBUG LOG --------*/
    if ( LOG_LV_DEBUG == getloglevel() )
    {
        boost::format formatter( "out_function: protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                            "handle_session_finalize( const boost::thread::id up_thread_id,"
                                            "const boost::thread::id down_thread_id ): return_value=%d" );
        formatter % STOP;
        putLogDebug( 030000, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    return STOP;
}

protocol_module_base::EVENT_TAG protocol_module_sslid::handle_accept(
                                                const boost::thread::id thread_id )
{
    /*-------- DEBUG LOG --------*/
    if ( LOG_LV_DEBUG == getloglevel() )
    {
        boost::format formatter( "in_function: protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                            "handle_accept( const boost::thread::id thread_id ): thread_id=%d" );
        formatter % thread_id;
        putLogDebug( 030000, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    EVENT_TAG status = STOP;
    // find thread id from map
    boost::mutex::scoped_lock sclock( session_thread_data_map_mutex );
    try
    {
        session_thread_data_map_type::iterator it = session_thread_data_map.find( thread_id );
        if ( it == session_thread_data_map.end() )
        {
            // waiting for jp response.....??????????????????
            putLogError(037000, "Invalid thread id.", __FILE__, __LINE__ );
            status = FINALIZE;
        }
        else
        {
            status = CLIENT_RECV;
        }
    }
    catch ( const std::exception& e )
    {
        std::cerr << "handle_accept exception: error=%s" << e.what() << "." << std::endl;
        boost::format formatter( "function: protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                            "handle_accept() exception: result=%d, error=%s." );
        formatter % FINALIZE % e.what();
        putLogError( 037000, formatter.str(), __FILE__, __LINE__ );

        status = FINALIZE;
    }
    catch( ... )
    {
        std::cerr << "Unkown exception." << std::endl;
        putLogError( 037000, "function protocol_module_sslid::check_message_result "
                        "protocol_module_sslid::handle_accept() : "
                        "Unkown exception.", __FILE__, __LINE__ );

        status = FINALIZE;
    }

    /*-------- DEBUG LOG --------*/
    if ( LOG_LV_DEBUG == getloglevel() )
    {
        boost::format formatter("out_function: protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                            "handle_accept( const boost::thread::id thread_id ): return_value=%d.");
        formatter % status;
        putLogDebug( 030000, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    return status;
}

protocol_module_base::EVENT_TAG protocol_module_sslid::handle_client_recv(
                                                const boost::thread::id thread_id,
                                                const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,
                                                const size_t recvlen )
{
    /*-------- DEBUG LOG --------*/
    if ( LOG_LV_DEBUG == getloglevel() )
    {
        boost::format formatter( "in_function: protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                            "handle_client_recv( const boost::thread::id thread_id,"
                                            "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,"
                                            "const size_t recvlen ): thread_id=%d, recvbuffer=%s, recvlen=%d" );
        formatter % thread_id % recvbuffer.data() % recvlen;
        putLogDebug( 030000, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    // parameter check
    if (recvlen > recvbuffer.size() )
    {
        // waiting for jp response...??????????????????
        std::cerr << "handle_client_recv(): Data size bigger than buffer size." << std::endl;
        putLogError(037000, "Data size bigger than buffer size.", __FILE__, __LINE__ );
        /*-------- DEBUG LOG --------*/
        if ( LOG_LV_DEBUG == getloglevel() )
        {
            boost::format formatter( "out_function: protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                                "handle_client_recv( const boost::thread::id thread_id,"
                                                "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,"
                                                "const size_t recvlen ): return_value=%d" );
            formatter % FINALIZE;
            putLogDebug( 030000, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        return FINALIZE;
    }

    // find session_thread_data_sslid's pointer from map
    session_thread_data_sslid* threaddata = NULL;
    EVENT_TAG status = STOP;

    try
    {
        {
            boost::mutex::scoped_lock sclock( session_thread_data_map_mutex );
            session_thread_data_map_type::iterator it = session_thread_data_map.find( thread_id );
            if ( it == session_thread_data_map.end() )
            {
                // waiting for jp response.....??????????????????
                putLogError(037000, "Invalid thread id.", __FILE__, __LINE__ );

                /*-------- DEBUG LOG --------*/
                if ( LOG_LV_DEBUG == getloglevel() )
                {
                    boost::format formatter( "out_function: protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                                        "handle_client_recv( const boost::thread::id thread_id,"
                                                        "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,"
                                                        "const size_t recvlen ): return_value=%d" );
                    formatter % FINALIZE;
                    putLogDebug( 030000, formatter.str(), __FILE__, __LINE__ );
                }
                /*------DEBUG LOG END------*/

                return FINALIZE;
            }

            threaddata = it->second;
        }

        if ( threaddata->end_flag == END_FLAG_ON )
        {
            // end_flag is ON, realserver is disconnect
             status = CLIENT_RECV;
        }
        else
        {
            // end_flag is OFF
            // get the c-style pointer from data buffer
            char* databuf = threaddata->data_buffer.c_array();

            if ( threaddata->data_size > 0 )
            {
                // data length check
                if ( threaddata->data_size + recvlen > threaddata->data_buffer.size() )
                {
                    // waiting for jp response...??????????????????
                    std::cerr << "handle_client_recv(): Data size bigger than buffer size." << std::endl;
                    putLogError(037000, "Data size bigger than buffer size.", __FILE__, __LINE__ );

                    /*-------- DEBUG LOG --------*/
                    if ( LOG_LV_DEBUG == getloglevel() )
                    {
                        boost::format formatter( "out_function: protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                                            "handle_client_recv( const boost::thread::id thread_id,"
                                                            "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,"
                                                            "const size_t recvlen ): return_value=%d" );
                        formatter % FINALIZE;
                        putLogDebug( 030000, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/

                    return FINALIZE;
                }

                // data dump
                /*-------- DEBUG LOG --------*/
                if ( LOG_LV_DEBUG == getloglevel() )
                {
                    boost::format formatter( "%02x" );
                    for ( int i = 0; i < threaddata->data_size; ++i )
                    {
                        formatter % static_cast<unsigned short>(
                                                static_cast<unsigned char>(
                                                threaddata->data_buffer[threaddata->data_begain_offset+i]));
                    }

                    std::string datadump = formatter.str();
                    formatter.parse( "function: protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                            "handle_client_recv( ): before memmove (data dump):  "
                                            "data begin(%d), data_size=%d,data=%s.");
                    formatter % threaddata->data_begain_offset % threaddata->data_size % datadump;
                    putLogDebug( 030000, formatter.str(), __FILE__, __LINE__ );
                }
                /*------DEBUG LOG END------*/

                // move the data from data start pos to buffer start pos
                memmove( databuf,
                                 &databuf[threaddata->data_begain_offset],
                                 threaddata->data_size );

                 /*-------- DEBUG LOG --------*/
                if ( LOG_LV_DEBUG == getloglevel() )
                {
                    boost::format formatter( "%02x" );
                    for ( int i = 0; i < threaddata->data_size; ++i )
                    {
                        formatter % static_cast<unsigned short>(
                                                static_cast<unsigned char>(threaddata->data_buffer[i]));
                    }

                    std::string datadump = formatter.str();
                    formatter.parse( "function: protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                            "handle_client_recv( ): after memmove (data dump):  "
                                            "data begin(0), data_size=%d,data=%s.");
                    formatter % threaddata->data_size % datadump;
                    putLogDebug( 030000, formatter.str(), __FILE__, __LINE__ );
                }
                /*------DEBUG LOG END------*/
            }

            /*-------- DEBUG LOG --------*/
            if ( LOG_LV_DEBUG == getloglevel() )
            {
                boost::format formatter( "%02x" );
                for ( size_t i = 0; i < threaddata->data_size+recvlen; ++i )
                {
                    formatter % static_cast<unsigned short>(
                                            static_cast<unsigned char>(threaddata->data_buffer[i]));
                }

                std::string datadump = formatter.str();
                formatter.parse( "function: protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                        "handle_client_recv( ): before memcpy (data dump):  "
                                        "data begin(0), data_size=%d,data=%s.");
                formatter % threaddata->data_size % datadump;
                putLogDebug( 030000, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/

            // copy new recv data to data_buffer
            memcpy( &databuf[threaddata->data_size], recvbuffer.data(), recvlen );

            // add data size
            threaddata->data_size += recvlen;
            threaddata->data_begain_offset = 0;

            /*-------- DEBUG LOG --------*/
            if ( LOG_LV_DEBUG == getloglevel() )
            {
                boost::format formatter( "%02x" );
                for ( size_t i = 0; i < threaddata->data_size+recvlen; ++i )
                {
                    formatter % static_cast<unsigned short>(
                                            static_cast<unsigned char>(threaddata->data_buffer[i]));
                }

                std::string datadump = formatter.str();
                formatter.parse( "function: protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                        "handle_client_recv( ): after memcpy (data dump):  "
                                        "data begin(0), data_size=%d,data=%s.");
                formatter % threaddata->data_size % datadump;
                putLogDebug( 030000, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/

            if ( threaddata->current_record_rest_size == 0 )
            {
                // new SSL Record start
                int alllength = 0;
                bool ishellomessage = false;
                int ret = check_ssl_record_sendable( true,
                                                                       threaddata->data_buffer.data(),
                                                                       threaddata->data_size,
                                                                       alllength,
                                                                       ishellomessage );
                /*-------- DEBUG LOG --------*/
                if ( LOG_LV_DEBUG == getloglevel() )
                {
                    putLogDebug( 030000, "function:protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                        "handle_client_recv() : check_ssl_record_sendable() END.", __FILE__, __LINE__ );
                }
                /*------DEBUG LOG END------*/

                if ( ret == -1 )
                {
                    // anomaly, set the end_flag to ON
                    threaddata->end_flag = END_FLAG_ON;
                    status = FINALIZE;
                    /*-------- DEBUG LOG --------*/
                    if ( LOG_LV_DEBUG == getloglevel() )
                    {
                        putLogDebug( 030000, "function: protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                            "handle_client_recv(): END_FLAG_ON.", __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                }
                else if ( ret == 1 )
                {
                    // disable send the data, set the status to CLIENT_RECV
                     status = CLIENT_RECV;
                }
                else
                {
                    // enable send the data, set the status to REALSERVER_SELECT
                    // set client helloworld shakehand flag
                    threaddata->hello_message_flag = ishellomessage;
                    threaddata->current_record_rest_size = alllength;
                    status = REALSERVER_SELECT;
                }
            }
            else
            {
                // other data
                 status = REALSERVER_SELECT;
            }
        }
    }
    catch ( const std::exception& e )
    {
        std::cerr << "handle_client_recv exception: error=%s" << e.what() << "." << std::endl;
        boost::format formatter( "function: protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                            "handle_client_recv() exception: result=%d, error=%s." );
        formatter % FINALIZE % e.what();
        putLogError( 037000, formatter.str(), __FILE__, __LINE__ );

        status = FINALIZE;
    }
    catch( ... )
    {
        std::cerr << "Unkown exception." << std::endl;
        putLogError( 037000, "function protocol_module_sslid::check_message_result "
                        "protocol_module_sslid::handle_client_recv() : "
                        "Unkown exception.", __FILE__, __LINE__ );

        status = FINALIZE;
    }

    /*-------- DEBUG LOG --------*/
    if ( LOG_LV_DEBUG == getloglevel() )
    {
        boost::format formatter( "out_function: protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                            "handle_client_recv( const boost::thread::id thread_id,"
                                            "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,"
                                            "const size_t recvlen ): return_value=%d,end_flag=%d." );
        formatter % status % threaddata->end_flag;
        putLogDebug( 030000, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    return status;
}


protocol_module_base::EVENT_TAG protocol_module_sslid::handle_realserver_select(
                                                const boost::thread::id thread_id,
                                                boost::asio::ip::tcp::endpoint& rs_endpoint )
{
    /*-------- DEBUG LOG --------*/
    if ( LOG_LV_DEBUG == getloglevel() )
    {
        boost::format formatter( "in_function: protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                            "handle_realserver_select( const boost::thread::id thread_id, "
                                            "boost::asio::ip::tcp::endpoint& rs_endpoint ): "
                                            "thread_id=%d,rs_endpoint=[%s]:%d" );
        formatter % thread_id % rs_endpoint.address().to_string() % rs_endpoint.port();
        putLogDebug( 030000, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    // find session_thread_data_sslid's pointer from map
    session_thread_data_sslid* threaddata = NULL;
    EVENT_TAG status = STOP;

    try
    {
        {
            boost::mutex::scoped_lock sclock( session_thread_data_map_mutex );
            session_thread_data_map_type::iterator it = session_thread_data_map.find(thread_id);
            if ( it == session_thread_data_map.end() )
            {
                 // waiting for jp response.....??????????????????
                putLogError(037000, "Invalid thread id.", __FILE__, __LINE__ );
                /*-------- DEBUG LOG --------*/
                if ( LOG_LV_DEBUG == getloglevel() )
                {
                    boost::format formatter( "out_function: protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                                        "handle_realserver_select( const boost::thread::id thread_id,"
                                                        "boost::asio::ip::tcp::endpoint& rs_endpoint ): return_value=%d" );
                    formatter % FINALIZE;
                    putLogDebug( 030000, formatter.str(), __FILE__, __LINE__ );
                }
                /*------DEBUG LOG END------*/
                // waiting for jp response Could not finalize protomod. (Realserver decision failure)??????
                putLogInfo( 035000, "Realserver decision failure", __FILE__, __LINE__ );

                return FINALIZE;
            }

            threaddata = it->second;
        }

        if ( threaddata->realserver_connect_failed_count <= 0 )
        {
            // the first connect or connected successful
            if ( realserver_selected( threaddata->selected_realserver ) )
            {
                 /*-------- DEBUG LOG --------*/
                if ( LOG_LV_DEBUG == getloglevel() )
                {
                    putLogDebug( 030000, "function: protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                        "realserver_selected() END.", __FILE__, __LINE__ );
                }
                /*------DEBUG LOG END------*/

                // realserver already connected
                rs_endpoint = threaddata->selected_realserver;
                status = REALSERVER_CONNECT;
            }
            else
            {
                 /*-------- DEBUG LOG --------*/
                if ( LOG_LV_DEBUG == getloglevel() )
                {
                    putLogDebug( 030000, "function: protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                        "realserver_selected(): END.", __FILE__, __LINE__ );
                }
                /*------DEBUG LOG END------*/

                // first connect
                if ( !threaddata->hello_message_flag )
                {
                    // when the first connect, must be hellomessage
                    threaddata->end_flag = END_FLAG_ON;
                    status = FINALIZE;
                     /*-------- DEBUG LOG --------*/
                    if ( LOG_LV_DEBUG == getloglevel() )
                    {
                        putLogDebug( 030000, "function: protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                            "handle_realserver_select(): END_FLAG_ON.", __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                }
                else
                {
                    // get sessionid from the data buffer
                    std::string session_id;
                    if ( get_ssl_session_id( threaddata->data_buffer.data(), threaddata->data_size,
                                                    session_id ) == -1 )
                    {
                        /*-------- DEBUG LOG --------*/
                        if ( LOG_LV_DEBUG == getloglevel() )
                        {
                            boost::format formatter( "out_function: protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                                                "handle_realserver_select( const boost::thread::id thread_id,"
                                                                "boost::asio::ip::tcp::endpoint& rs_endpoint ): return_value=%d" );
                            formatter % FINALIZE;
                            putLogDebug( 030000, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/

                        // waiting for jp response Could not finalize protomod. (Realserver decision failure)??????
                        putLogInfo( 035000, "Realserver decision failure", __FILE__, __LINE__ );

                        return FINALIZE;
                    }

                    /*-------- DEBUG LOG --------*/
                    if ( LOG_LV_DEBUG == getloglevel() )
                    {
                        putLogDebug( 030000, "function: protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                             "handle_realserver_select( ): get_ssl_session_id() END.", __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/

                    if ( session_id.size() == 0 )
                    {
                        // no session id in hellomesseage
                        // schedule the endpoint
                        boost::asio::ip::tcp::endpoint temp_endpoint;
                        boost::asio::ip::tcp::endpoint comp_endpoint;   // for compare the endpoint
                        schedule_tcp(thread_id, rs_list_begin, rs_list_end, rs_list_next, temp_endpoint );
                        if ( temp_endpoint != comp_endpoint )
                        {
                            // success for get the endpoint by reschedule
                            threaddata->selected_realserver = temp_endpoint;
                            rs_endpoint = temp_endpoint;
                            status = REALSERVER_CONNECT;
                        }
                        else
                        {
                             // failed to get the endpoint by reschedule
                            // set end_flag to ON, and turn the status to CLIENT_DISCONNECT
                            threaddata->end_flag = END_FLAG_ON;
                            status = CLIENT_DISCONNECT;
                            /*-------- DEBUG LOG --------*/
                            if ( LOG_LV_DEBUG == getloglevel() )
                            {
                                putLogDebug( 030000, "function: protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                                    "handle_realserver_select(): END_FLAG_ON.", __FILE__, __LINE__ );
                            }
                            /*------DEBUG LOG END------*/
                        }
                    }
                    else
                    {
                        // has session id in hellomessage
                        // try to get the endpoint from session data by session id
                        int ret = session_data_processor->get_endpoint_from_session_data(
                                                                                                session_id,
                                                                                                threaddata->selected_realserver );
                        /*-------- DEBUG LOG --------*/
                        if ( LOG_LV_DEBUG == getloglevel() )
                        {
                            putLogDebug( 030000, "function: protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                                "handle_realserver_select(): get_endpoint_from_session_data() END.",
                                                __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/

                        if ( ret == 0 )
                        {
                            // success for get the endpoint
                            rs_endpoint = threaddata->selected_realserver;
                            status = REALSERVER_CONNECT;
                        }
                        else
                        {
                            // failed to get the endpoint
                            if ( reschedule == 1 )
                            {
                                // reschedule mode
                                boost::asio::ip::tcp::endpoint temp_endpoint;
                                boost::asio::ip::tcp::endpoint comp_endpoint;   // for compare the endpoint
                                schedule_tcp(thread_id, rs_list_begin, rs_list_end, rs_list_next, temp_endpoint );
                                if ( temp_endpoint != comp_endpoint )
                                {
                                    // success for get the endpoint by reschedule
                                    threaddata->selected_realserver = temp_endpoint;
                                    rs_endpoint = temp_endpoint;
                                    status = REALSERVER_CONNECT;
                                }
                                else
                                {
                                    // failed to get the endpoint by reschedule
                                    // set end_flag to ON, and turn the status to CLIENT_DISCONNECT
                                    threaddata->end_flag = END_FLAG_ON;
                                    status = CLIENT_DISCONNECT;
                                     /*-------- DEBUG LOG --------*/
                                    if ( LOG_LV_DEBUG == getloglevel() )
                                    {
                                        putLogDebug( 030000, "function: protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                                            "handle_realserver_select(): END_FLAG_ON.", __FILE__, __LINE__ );
                                    }
                                    /*------DEBUG LOG END------*/
                                }
                            }
                            else
                            {
                                // no reschedule mode
                                // set end_flag to ON and disconnect the client
                                threaddata->end_flag = END_FLAG_ON;
                                status = CLIENT_DISCONNECT;
                                 /*-------- DEBUG LOG --------*/
                                if ( LOG_LV_DEBUG == getloglevel() )
                                {
                                    putLogDebug( 030000, "function: protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                                        "handle_realserver_select(): END_FLAG_ON.", __FILE__, __LINE__ );
                                }
                                /*------DEBUG LOG END------*/
                            }
                        }
                    }
                }
            }
        }
        else if ( threaddata->realserver_connect_failed_count < realserver_connect_failed_max_count )
        {
            // try multi times connect
            if ( reschedule == 1 )
            {
                // reschedule mode
                boost::asio::ip::tcp::endpoint temp_endpoint;
                boost::asio::ip::tcp::endpoint comp_endpoint;   // for compare the endpoint
                schedule_tcp(thread_id, rs_list_begin, rs_list_end, rs_list_next, temp_endpoint );
                if ( temp_endpoint != comp_endpoint )
                {
                    // success for get the endpoint by reschedule
                    threaddata->selected_realserver = temp_endpoint;
                    rs_endpoint = temp_endpoint;
                    status = REALSERVER_CONNECT;
                }
                else
                {
                    // failed to get the endpoint by reschedule
                    // set end_flag to ON, and turn the status to CLIENT_DISCONNECT
                    threaddata->end_flag = END_FLAG_ON;
                    status = CLIENT_DISCONNECT;
                     /*-------- DEBUG LOG --------*/
                    if ( LOG_LV_DEBUG == getloglevel() )
                    {
                        putLogDebug( 030000, "function: protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                            "handle_realserver_select(): END_FLAG_ON.", __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                }
            }
            else
            {
                // no reschedule mode
                // set end_flag to ON and disconnect the client
                threaddata->end_flag = END_FLAG_ON;
                status = CLIENT_DISCONNECT;
                 /*-------- DEBUG LOG --------*/
                if ( LOG_LV_DEBUG == getloglevel() )
                {
                    putLogDebug( 030000, "function: protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                        "handle_realserver_select(): END_FLAG_ON.", __FILE__, __LINE__ );
                }
                /*------DEBUG LOG END------*/
            }
        }
        else
        {
            // connect failed
            // set end_flag to ON and disconnect the client
            threaddata->end_flag = END_FLAG_ON;
            status = CLIENT_DISCONNECT;
             /*-------- DEBUG LOG --------*/
            if ( LOG_LV_DEBUG == getloglevel() )
            {
                putLogDebug( 030000, "function: protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                    "handle_realserver_select(): END_FLAG_ON.", __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/
        }
    }
    catch ( const std::exception& e )
    {
        std::cerr << "handle_realserver_select exception: error=%s" << e.what() << "." << std::endl;
        boost::format formatter( "function: protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                            "handle_realserver_select() exception: result=%d, error=%s." );
        formatter % FINALIZE % e.what();
        putLogError( 037000, formatter.str(), __FILE__, __LINE__ );

        // waiting for jp response Could not finalize protomod. (Realserver decision failure)??????
        putLogInfo( 035000, "Realserver decision failure", __FILE__, __LINE__ );

        status = FINALIZE;
    }
    catch( ... )
    {
        std::cerr << "Unkown exception." << std::endl;
        putLogError( 037000, "function protocol_module_sslid::check_message_result "
                        "protocol_module_sslid::handle_realserver_select() : "
                        "Unkown exception.", __FILE__, __LINE__ );

        // waiting for jp response Could not finalize protomod. (Realserver decision failure)??????
        putLogInfo( 035000, "Realserver decision failure", __FILE__, __LINE__ );

        status = FINALIZE;
    }

    /*-------- DEBUG LOG --------*/
    if ( LOG_LV_DEBUG == getloglevel() )
    {
        boost::format formatter( "out_function: protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                            "handle_realserver_select( const boost::thread::id thread_id,"
                                            "boost::asio::ip::tcp::endpoint& rs_endpoint ): return_value=%d,"
                                            "end_flag=%d, rs_endpoint=[%s]:%d." );
        formatter % status % threaddata->end_flag % rs_endpoint.address().to_string() % rs_endpoint.port();
        putLogDebug( 030000, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    return status;
}

protocol_module_base::EVENT_TAG protocol_module_sslid::handle_realserver_select(
                                                const boost::thread::id thread_id,
                                                boost::asio::ip::udp::endpoint& rs_endpoint,
                                                boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
                                                size_t& datalen )
{
    /*-------- DEBUG LOG --------*/
    if ( LOG_LV_DEBUG == getloglevel() )
    {
        boost::format formatter( "in/out_function: protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                            "handle_realserver_select( const boost::thread::id thread_id,"
                                            "boost::asio::ip::tcp::endpoint& rs_endpoint, "
                                            "boost::array<char,MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen ): "
                                            "return_value=%d." );
        formatter % STOP;
        putLogDebug( 030000, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/
    return STOP;
}

protocol_module_base::EVENT_TAG protocol_module_sslid::handle_realserver_connect(
                                                const boost::thread::id thread_id,
                                                boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
                                                size_t& datalen )
{
    /*-------- DEBUG LOG --------*/
    if ( LOG_LV_DEBUG == getloglevel() )
    {
        boost::format formatter( "in_function: protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                            "handle_realserver_connect( const boost::thread::id thread_id,"
                                            "boost::array<char,MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen ): "
                                            "thread_id=%d, sendbuffer=%s, datalen=%d" );
        formatter % thread_id % sendbuffer.data() % datalen;
        putLogDebug( 030000, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    EVENT_TAG status = REALSERVER_SEND;
    try
    {
        if ( put_data_to_sendbuffer( thread_id, sendbuffer, datalen ) == -1 )
        {
            // can't find the thread from the thread map
            status = FINALIZE;
        }
        else
        {
            // find session_thread_data_sslid's pointer from map
            boost::mutex::scoped_lock sclock( session_thread_data_map_mutex );

            session_thread_data_map_type::iterator it = session_thread_data_map.find(thread_id);
            if ( it == session_thread_data_map.end() )
            {
                 // waiting for jp response.....??????????????????
                putLogError(037000, "Invalid thread id.", __FILE__, __LINE__ );
                status = FINALIZE;
            }
            else
            {
                session_thread_data_sslid* threaddata = it->second;
                threaddata->realserver_connect_failed_count = 0;
            }
        }
    }
    catch ( const std::exception& e )
    {
        std::cerr << "handle_realserver_connect exception: error=%s" << e.what() << "." << std::endl;
        boost::format formatter( "function: protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                            "handle_realserver_connect() exception: result=%d, error=%s." );
        formatter % FINALIZE % e.what();
        putLogError( 037000, formatter.str(), __FILE__, __LINE__ );
        status = FINALIZE;
    }
    catch( ... )
    {
        std::cerr << "Unkown exception." << std::endl;
        putLogError( 037000, "function protocol_module_sslid::check_message_result "
                        "protocol_module_sslid::handle_realserver_connect() : "
                        "Unkown exception.", __FILE__, __LINE__ );

        status = FINALIZE;
    }

    /*-------- DEBUG LOG --------*/
    if ( LOG_LV_DEBUG == getloglevel() )
    {
        boost::format formatter( "out_function: protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                            "handle_realserver_connect( const boost::thread::id thread_id,"
                                            "boost::array<char,MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen ): "
                                            "return_value=%d" );
        formatter % status;
        putLogDebug( 030000, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    return status;
}

protocol_module_base::EVENT_TAG protocol_module_sslid::handle_realserver_connection_fail(
                                                const boost::thread::id thread_id,
                                                const boost::asio::ip::tcp::endpoint& rs_endpoint )
{
    /*-------- DEBUG LOG --------*/
    if ( LOG_LV_DEBUG == getloglevel() )
    {
        boost::format formatter( "in_function: protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                            "handle_realserver_connection_fail( const boost::thread::id thread_id,"
                                            "const boost::asio::ip::tcp::endpoint& rs_endpoint ): "
                                            "thread_id=%d,rs_endpoint=[%s]:%d" );
        formatter % thread_id % rs_endpoint.address().to_string() % rs_endpoint.port();
        putLogDebug( 030000, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

     // find session_thread_data_sslid's pointer from map
    session_thread_data_sslid* threaddata = NULL;
    EVENT_TAG status = STOP;

    try
    {
        {
            boost::mutex::scoped_lock sclock( session_thread_data_map_mutex );
            session_thread_data_map_type::iterator it = session_thread_data_map.find(thread_id);
            if ( it == session_thread_data_map.end() )
            {
                // waiting for jp response.....??????????????????
                putLogError(037000, "Invalid thread id.", __FILE__, __LINE__ );
                /*-------- DEBUG LOG --------*/
                if ( LOG_LV_DEBUG == getloglevel() )
                {
                    boost::format formatter( "out_function: protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                                        "handle_realserver_connection_fail( const boost::thread::id thread_id,"
                                                        "const boost::asio::ip::tcp::endpoint& rs_endpoint ): return_value=%d" );
                    formatter % FINALIZE;
                    putLogDebug( 030000, formatter.str(), __FILE__, __LINE__ );
                }
                /*------DEBUG LOG END------*/
                return FINALIZE;
            }

            threaddata = it->second;
        }

        if ( reschedule == 1 )
        {
            // reschedule mode
            ++threaddata->realserver_connect_failed_count;
            status = REALSERVER_SELECT;
        }
        else
        {
            // no reschedule mode
            // set end_flag to ON
            threaddata->end_flag = END_FLAG_ON;
            status = CLIENT_DISCONNECT;

            /*-------- DEBUG LOG --------*/
            if ( LOG_LV_DEBUG == getloglevel() )
            {
                putLogDebug( 030000, "function: protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                    "handle_realserver_connection_fail(): END_FLAG_ON.", __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/
        }
    }
    catch ( const std::exception& e )
    {
        std::cerr << "handle_realserver_connection_fail exception: error=%s" << e.what() << "." << std::endl;
        boost::format formatter( "function: protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                            "handle_realserver_connection_fail() exception: result=%d, error=%s." );
        formatter % FINALIZE % e.what();
        putLogError( 037000, formatter.str(), __FILE__, __LINE__ );

        status = FINALIZE;
    }
    catch( ... )
    {
        std::cerr << "Unkown exception." << std::endl;
        putLogError( 037000, "function protocol_module_sslid::check_message_result "
                        "protocol_module_sslid::handle_realserver_connection_fail() : "
                        "Unkown exception.", __FILE__, __LINE__ );

        status = FINALIZE;
    }

    /*-------- DEBUG LOG --------*/
    if ( LOG_LV_DEBUG == getloglevel() )
    {
        boost::format formatter( "out_function: protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                            "handle_realserver_connection_fail( const boost::thread::id thread_id,"
                                            "const boost::asio::ip::tcp::endpoint& rs_endpoint ): return_value=%d,"
                                            "end_flag=%d" );
        formatter % status % threaddata->end_flag;
        putLogDebug( 030000, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    return status;
}

protocol_module_base::EVENT_TAG protocol_module_sslid::handle_realserver_send(
                                                const boost::thread::id thread_id )
{

   return STOP;
}

protocol_module_base::EVENT_TAG protocol_module_sslid::handle_sorryserver_select(
                                const boost::thread::id thread_id,
								boost::asio::ip::tcp::endpoint& sorry_endpoint )
{
    /*-------- DEBUG LOG --------*/
    if ( LOG_LV_DEBUG == getloglevel() )
    {
        boost::format formatter( "in/out_function: protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                            "handle_sorryserver_select( const boost::thread::id thread_id,"
                                            "boost::asio::ip::tcp::endpoint& rs_endpoint ): "
                                            "return_value=%d." );
        formatter % STOP;
        putLogDebug( 030000, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    return STOP;
}

protocol_module_base::EVENT_TAG protocol_module_sslid::handle_sorryserver_connect(
								const boost::thread::id thread_id,
								boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
								size_t& datalen )
{
    /*-------- DEBUG LOG --------*/
    if ( LOG_LV_DEBUG == getloglevel() )
    {
        boost::format formatter( "in/out_function: protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                            "handle_sorryserver_connect( const boost::thread::id thread_id,"
                                            "boost::asio::ip::tcp::endpoint& rs_endpoint, size_t& datalen ): "
                                            "return_value=%d." );
        formatter % STOP;
        putLogDebug( 030000, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    return STOP;
}

protocol_module_base::EVENT_TAG protocol_module_sslid::handle_sorryserver_connection_fail(
								const boost::thread::id thread_id,
								const boost::asio::ip::tcp::endpoint& sorry_endpoint )
{
    /*-------- DEBUG LOG --------*/
    if ( LOG_LV_DEBUG == getloglevel() )
    {
        boost::format formatter( "in/out_function: protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                            "handle_sorryserver_connection_fail( const boost::thread::id thread_id,"
                                            "boost::asio::ip::tcp::endpoint& rs_endpoint ): "
                                            "return_value=%d." );
        formatter % STOP;
        putLogDebug( 030000, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    return STOP;
}

protocol_module_base::EVENT_TAG protocol_module_sslid::handle_sorryserver_send(
                                                const boost::thread::id thread_id )
{
    /*-------- DEBUG LOG --------*/
    if ( LOG_LV_DEBUG == getloglevel() )
    {
        boost::format formatter( "in/out_function: protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                            "handle_sorryserver_send( const boost::thread::id thread_id) :"
                                            "return_value=%d." );
        formatter % STOP;
        putLogDebug( 030000, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    return STOP;
}

protocol_module_base::EVENT_TAG protocol_module_sslid::handle_realserver_recv(
                                                const boost::thread::id thread_id,
                                                const boost::asio::ip::tcp::endpoint& rs_endpoint,
                                                const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,
                                                const size_t recvlen )
{
   return STOP;
}

protocol_module_base::EVENT_TAG protocol_module_sslid::handle_realserver_recv(
                                                const boost::thread::id thread_id,
                                                const boost::asio::ip::udp::endpoint& rs_endpoint,
                                                const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,
                                                const size_t recvlen )
{
    return STOP;
}

protocol_module_base::EVENT_TAG protocol_module_sslid::handle_sorryserver_recv(
                                                const boost::thread::id thread_id,
                                                const boost::asio::ip::tcp::endpoint& sorry_endpoint,
                                                const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,
                                                const size_t recvlen )
{
    return STOP;
}

protocol_module_base::EVENT_TAG protocol_module_sslid::handle_response_send_inform(
                                                const boost::thread::id thread_id )
{
    return STOP;
}

protocol_module_base::EVENT_TAG protocol_module_sslid::handle_client_connection_check(
                                                const boost::thread::id thread_id,
                                                boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
                                                size_t& datalen )
{
    return STOP;
}

protocol_module_base::EVENT_TAG protocol_module_sslid::handle_client_select(
                                                const boost::thread::id thread_id,
                                                boost::asio::ip::udp::endpoint& cl_endpoint,
                                                boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
                                                size_t& datalen )
{
   return STOP;
}

protocol_module_base::EVENT_TAG protocol_module_sslid::handle_client_send(
                                                const boost::thread::id thread_id )
{
    return STOP;
}

protocol_module_base::EVENT_TAG protocol_module_sslid::handle_client_disconnect(
                                                const boost::thread::id thread_id )
{
    return FINALIZE;
}

protocol_module_base::EVENT_TAG protocol_module_sslid::handle_sorry_enable(
                                                const boost::thread::id thread_id )
{
    return STOP;
}

protocol_module_base::EVENT_TAG protocol_module_sslid::handle_sorry_disable(
                                                const boost::thread::id thread_id )
{
    return STOP;
}

protocol_module_base::EVENT_TAG protocol_module_sslid::handle_realserver_disconnect(
                                                const boost::thread::id thread_id,
                                                const boost::asio::ip::tcp::endpoint& rs_endpoint )
{
   return STOP;
}

protocol_module_base::EVENT_TAG protocol_module_sslid::handle_sorryserver_disconnect(
                                                const boost::thread::id thread_id,
                                                const boost::asio::ip::tcp::endpoint& sorry_endpoint )
{
   return STOP;
}

protocol_module_base::EVENT_TAG protocol_module_sslid::handle_realserver_close(
                                                const boost::thread::id thread_id,
                                                const boost::asio::ip::udp::endpoint& rs_endpoint )
{
   return STOP;
}

void protocol_module_sslid::replication_interrupt()
{

}

//! put data into send buffer function
//! @param const boost::thread::id& thread_id
//! @param boost::array<char,MAX_BUFFER_SIZE>& sendbuffer
//! @param size_t& datalen
//! @return 0 : success -1: failed
int protocol_module_sslid::put_data_to_sendbuffer(
                                const boost::thread::id& thread_id,
                                boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
                                size_t& datalen)
{
    return 0;
}

bool protocol_module_sslid::realserver_selected(const boost::asio::ip::tcp::endpoint& rs_endpoint)
{
    return true;
}

}
