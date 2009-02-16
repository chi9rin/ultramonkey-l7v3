#include "protocol_module_sslid.h"
#include <boost/format.hpp>

namespace l7vs
{
const std::string protocol_module_sslid::MODULE_NAME = "sslid";
const int protocol_module_sslid::REALSERVER_CONNECT_FAILED_MAX_COUNT = 3;
const int protocol_module_sslid::THREAD_DIVISION_UP_STREAM = 0;         // up thread
const int protocol_module_sslid::THREAD_DIVISION_DOWN_STREAM = 1;       // down thread
const int protocol_module_sslid::END_FLAG_ON = 1;                       // end flag ON
const int protocol_module_sslid::END_FLAG_OFF = 0;                      // end flag OFF

//! constructor
protocol_module_sslid::protocol_module_sslid()
        :ssl_protocol_module_base(MODULE_NAME),
        realserver_connect_failed_max_count(REALSERVER_CONNECT_FAILED_MAX_COUNT),
        replication_data_processor(NULL),
        session_data_processor(NULL)
{

}

//! destructor
protocol_module_sslid::~protocol_module_sslid()
{
    //dtor
}

//! tcp protocol support check
//! @return tcp support is true
//! @return tcp not-support is false
bool protocol_module_sslid::is_tcp()
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        putLogDebug(30000, "in/out_function : bool protocol_module_sslid::is_tcp() : return_value = true.",
                    __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    return true;
}

//! udp protocol support check
//! @return udp support is true
	//! @return udp not-support is false
bool protocol_module_sslid::is_udp()
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        putLogDebug(30001, "in/out_function : bool protocol_module_sslid::is_udp() : return_value = false.",
                    __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    return false;
}

//! initialize function. called from module control. module loaded call
//! @param[in] realserver list iterator begin function object type
//! @param[in] realserver list iterator end function object type
//! @param[in] realserver list iterator next function object type
//! @param[in] realserver list mutex lock function object type
//! @param[in] realserver list mutex unlock function object type
void protocol_module_sslid::initialize(
    rs_list_itr_func_type inlist_begin,
    rs_list_itr_func_type inlist_end,
    rs_list_itr_next_func_type inlist_next,
    boost::function< void(void) > inlist_lock,
    boost::function< void(void) > inlist_unlock)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        putLogDebug(30002, "in_function : void protocol_module_sslid::initialize("
                    "rs_list_itr_func_type inlist_begin, rs_list_itr_func_type inlist_end, "
                    "rs_list_itr_next_func_type inlist_next, boost::function< void(void) > "
                    "inlist_lock, boost::function< void(void) > inlist_unlock).", __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    rs_list_begin = inlist_begin;
    rs_list_end = inlist_end;
    rs_list_next = inlist_next;
    rs_list_lock = inlist_lock;
    rs_list_unlock = inlist_unlock;

    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        putLogDebug(30003, "out_function : void protocol_module_sslid::initialize("
                    "rs_list_itr_func_type inlist_begin, rs_list_itr_func_type inlist_end, "
                    "rs_list_itr_next_func_type inlist_next, boost::function< void(void) > "
                    "inlist_lock, boost::function< void(void) > inlist_unlock).", __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/
}

//! finalize called from module control. module unloaded call.
void protocol_module_sslid::finalize()
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        putLogDebug(30004, "in_function : void protocol_module_sslid::finalize().", __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    // realserver list function object initialize
    rs_list_begin.clear();
    rs_list_end.clear();
    rs_list_next.clear();
    rs_list_lock.clear();
    rs_list_unlock.clear();

    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        putLogDebug(30005, "function : void protocol_module_sslid::finalize() : "
                    "rs_list_begin.clear(), rs_list_end.clear(), rs_list_next.clear(), "
                    "rs_list_lock.clear(), rs_list_unlock.clear() end.", __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    // replication function object initialize
    replication_pay_memory.clear();
    replication_area_lock.clear();
    replication_area_unlock.clear();

    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        putLogDebug(30006, "function : void protocol_module_sslid::finalize() : "
                    "replication_pay_memory.clear(), replication_area_lock.clear(), "
                    "replication_area_unlock.clear() end.", __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    // scheduleModule function object initialize
    schedule_tcp.clear();

    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        putLogDebug(30007, "function : void protocol_module_sslid::finalize() : "
                           "schedule_tcp.clear() end.", __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    // module option initialize
    timeout = 0;
    maxlist = 0;
    reschedule = 0;

    // replication initialize
    if (replication_data_processor)
    {
        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == getloglevel())
        {
            boost::format formatter("delete : address = &(%d).");
            formatter % static_cast<void*>(replication_data_processor);
            putLogDebug(30008, formatter.str(), __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/

        delete replication_data_processor;
        replication_data_processor = NULL;
    }

    // session initialize
    if (session_data_processor)
    {
        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == getloglevel())
        {
            boost::format formatter("delete : address = &(%d).");
            formatter % static_cast<void*>(session_data_processor);
            putLogDebug(30009, formatter.str(), __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/

        delete session_data_processor;
        session_data_processor = NULL;
    }

    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        putLogDebug(30010, "out_function : void protocol_module_sslid::finalize().", __FILE__, __LINE__);
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

//! sorry support check
//! @return false sorry mode is unsupported.
//! @return true sorry mode is supported.
bool protocol_module_sslid::is_use_sorry()
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        putLogDebug(30011, "in/out_function : bool protocol_module_sslid::is_use_sorry() : return_value = false.",
                    __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/
    return false;
}

//! module parameter check.used by l7vsadm
//! @param[in] module paramter string list
//! @return result.flag true is parameter is noproblem.
//! @return result.flag false is parameter is problem.
protocol_module_sslid::check_message_result protocol_module_sslid::check_parameter(
    const std::vector<std::string>& args)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("in_function : protocol_module_sslid::check_message_result "
                                "protocol_module_sslid::check_parameter("
                                "const std::vector<std::string>& args) : args = %s.");
        std::string argsdump;
        for (std::vector<std::string>::const_iterator it = args.begin(); it != args.end(); ++it)
        {
            argsdump += *it;
        }
        formatter % argsdump;
        putLogDebug(30012, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    check_message_result result;
    // set check result flag true
    result.flag = true;
    bool timeout_flag = false;
    bool maxlist_flag = false;
    bool reschedule_flag = false;
    bool no_reschedule_flag = false;

    try
    {
        std::vector<std::string>::const_iterator itparam;
        boost::format formatter;

        for (itparam = args.begin(); itparam != args.end();)
        {
            if (*itparam == "-T" || *itparam == "--timeout")
            {
                // timeout
                if (!timeout_flag)
                {
                    // not set timeout option
                    ++itparam;
                    // next parameter exist check
                    if (itparam != args.end())
                    {
                        // next parameter exist
                        if ((*itparam).substr(0, 1) == "-" || (*itparam).substr(0, 2) == "--")
                        {
                            continue;
                        }

                        try
                        {
                            unsigned long ultimeout = 0;
                            ultimeout = boost::lexical_cast<unsigned long>(*itparam);
                            // int maxvalue check
                            if (ultimeout > INT_MAX)
                            {
                                result.flag = false;
                                formatter.parse("'-T/--timeout' option value '%s' is too large.");
                                formatter % *itparam;
                                result.message = formatter.str();
                                putLogError(37000, result.message, __FILE__, __LINE__);
                                break;
                            }
                            else
                            {
                                timeout_flag = true;
                                ++itparam;
                                continue;
                            }
                        }
                        catch (boost::bad_lexical_cast& e)
                        {
                            // not numeric character
                            result.flag = false;
                            formatter.parse("'-T/--timeout' option value '%s' is not numeric character.");
                            formatter % *itparam;
                            result.message = formatter.str();
                            putLogError(37001, result.message, __FILE__, __LINE__);
                            break;
                        }
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    // already set timeout
                    result.flag = false;
                    result.message = "Cannot set multiple option '-T/--timeout'.";
                    putLogError(37002, result.message, __FILE__, __LINE__);
                    break;
                }
            }
            else if (*itparam == "-M" || *itparam == "--maxlist")
            {
                // maxlist
                if (!maxlist_flag)
                {
                    // not set maxlist option
                    ++itparam;
                    // next parameter exist check
                    if (itparam != args.end())
                    {
                        // next parameter exist
                        if ((*itparam).substr(0, 1) == "-" || (*itparam).substr(0, 2) == "--")
                        {
                            continue;
                        }

                        try
                        {
                            unsigned long ulmaxlist = 0;
                            ulmaxlist = boost::lexical_cast<unsigned long>(*itparam);
                            // int maxvalue check
                            if (ulmaxlist > INT_MAX)
                            {
                                result.flag = false;
                                formatter.parse("'-M/--maxlist' option value '%s' is too large.");
                                formatter % *itparam;
                                result.message = formatter.str();
                                putLogError(37003, result.message, __FILE__, __LINE__);
                                break;
                            }
                            else
                            {
                                maxlist_flag = true;
                                ++itparam;
                                continue;
                            }
                        }
                        catch (boost::bad_lexical_cast& e)
                        {
                            // not numeric character
                            result.flag = false;
                            formatter.parse("'-M/--maxlist' option value '%s' is not numeric character.");
                            formatter % *itparam;
                            result.message = formatter.str();
                            putLogError(37004, result.message, __FILE__, __LINE__);
                            break;
                        }
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    // already set maxlist
                    result.flag = false;
                    result.message = "Cannot set multiple option '-M/--maxlist'.";
                    putLogError(37005, result.message, __FILE__, __LINE__);
                    break;
                }
            }
            else if (*itparam == "-R" || *itparam == "--reschedule")
            {
                // reschedule
                if (!no_reschedule_flag)
                {
                    // not set no-reschedule flag
                    reschedule_flag = true;
                }
                else
                {
                    // already set no-reschedule flag
                    result.flag = false;
                    result.message = "You have to choose either of reschedule or no-reschedule.";
                    putLogError(37006, result.message, __FILE__, __LINE__);
                    break;
                }
            }
            else if  (*itparam == "-N" || *itparam == "--no-reschedule")
            {
                // no-reschedule
                if (!reschedule_flag)
                {
                    // not set reschedule flag
                    no_reschedule_flag = true;
                }
                else
                {
                    // already set reshcedule flag
                    result.flag = false;
                    result.message = "You have to choose either of reschedule or no-reschedule.";
                    putLogError(37007, result.message, __FILE__, __LINE__);
                    break;
                }
            }
            else
            {
                // set check result flag false
                result.flag = false;
                result.message = "Option error.";
                putLogError(37008, result.message, __FILE__, __LINE__);
                break;
            }
            ++itparam;
        }
    }
    catch (const std::exception& e)
    {
        result.flag = false;
        std::cerr << "protocol_module_sslid::check_parameter() : exception : error = " << e.what() << "." << std::endl;
        boost::format formatter("function : protocol_module_sslid::check_message_result "
                                "protocol_module_sslid::check_parameter() : exception : error = %s.");
        formatter % e.what();
        putLogError(37009, formatter.str(), __FILE__, __LINE__);
    }
    catch (...)
    {
        result.flag = false;
        std::cerr << "protocol_module_sslid::check_parameter() : Unknown exception." << std::endl;
        putLogError(37010, "function : protocol_module_sslid::check_message_result protocol_module_sslid::"
                    "check_parameter() : Unknown exception.", __FILE__, __LINE__);
    }

    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("out_function : protocol_module_sslid::check_message_result "
                                "protocol_module_sslid::check_parameter("
                                "const std::vector<std::string>& args) : return_value = ("
                                "check_message_result.flag = %d, check_message_result.message = %s).");
        formatter % result.flag % result.message;
        putLogDebug(30013, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    return result;
}

//! parameter set
//! @param[in] module paramter string list
//! @return result.flag true is parameter is noproblem.
//! @return result.flag false is paramter is problem.
protocol_module_sslid::check_message_result protocol_module_sslid::set_parameter(
    const std::vector<std::string>& args)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("in_function : protocol_module_sslid::check_message_result "
                                "protocol_module_sslid::set_parameter("
                                "const std::vector<std::string>& args) : args = %s.");
        std::string argsdump;
        for (std::vector<std::string>::const_iterator it = args.begin(); it != args.end(); ++it)
        {
            argsdump += *it;
        }
        formatter % argsdump;
        putLogDebug(30014, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    check_message_result result;
    // set check result flag true
    result.flag = true;
    bool timeout_flag = false;
    bool maxlist_flag = false;
    bool reschedule_flag = false;
    bool no_reschedule_flag = false;

    try
    {
        std::vector<std::string>::const_iterator itparam;
        boost::format formatter;

        for (itparam = args.begin(); itparam != args.end();)
        {
            if (*itparam == "-T" || *itparam == "--timeout")
            {
                // timeout
                if (!timeout_flag)
                {
                    // not set timeout option
                    ++itparam;
                    // next parameter exist check
                    if (itparam != args.end())
                    {
                        // next parameter exist
                        if ((*itparam).substr(0, 1) == "-" || (*itparam).substr(0, 2) == "--")
                        {
                            continue;
                        }

                        try
                        {
                            unsigned long ultimeout = 0;
                            ultimeout = boost::lexical_cast<unsigned long>(*itparam);
                            // int maxvalue check
                            if (ultimeout > INT_MAX)
                            {
                                result.flag = false;
                                formatter.parse("'-T/--timeout' option value '%s' is too large.");
                                formatter % *itparam;
                                result.message = formatter.str();
                                putLogError(37011, result.message, __FILE__, __LINE__);
                                break;
                            }
                            else
                            {
                                timeout = ultimeout;
                                timeout_flag = true;
                                ++itparam;
                                continue;
                            }
                        }
                        catch (boost::bad_lexical_cast& e)
                        {
                            // not numeric character
                            result.flag = false;
                            formatter.parse("'-T/--timeout' option value '%s' is not numeric character.");
                            formatter % *itparam;
                            result.message = formatter.str();
                            putLogError(37012, result.message, __FILE__, __LINE__);
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
                    result.message = "Cannot set multiple option '-T/--timeout'.";
                    putLogError(37013, result.message, __FILE__, __LINE__);
                    break;
                }
            }
            else if (*itparam == "-M" || *itparam == "--maxlist")
            {
                // maxlist
                if (!maxlist_flag)
                {
                    // not set maxlist option
                    ++itparam;
                    // next parameter exist check
                    if (itparam != args.end())
                    {
                        // next parameter exist
                        if ((*itparam).substr(0, 1) == "-" || (*itparam).substr(0, 2) == "--")
                        {
                            continue;
                        }

                        try
                        {
                            unsigned long ulmaxlist = 0;
                            ulmaxlist = boost::lexical_cast<unsigned long>(*itparam);
                            // int maxvalue check
                            if (ulmaxlist > INT_MAX)
                            {
                                result.flag = false;
                                formatter.parse("'-M/--maxlist' option value '%s' is too large.");
                                formatter % *itparam;
                                result.message = formatter.str();
                                putLogError(37014, result.message, __FILE__, __LINE__);
                                break;
                            }
                            else
                            {
                                maxlist = ulmaxlist;
                                maxlist_flag = true;
                                ++itparam;
                                continue;
                            }
                        }
                        catch (boost::bad_lexical_cast& e)
                        {
                            // not numeric character
                            result.flag = false;
                            formatter.parse("'-M/--maxlist' option value '%s' is not numeric character.");
                            formatter % *itparam;
                            result.message = formatter.str();
                            putLogError(37015, result.message, __FILE__, __LINE__);
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
                    putLogError(37016, result.message, __FILE__, __LINE__);
                    break;
                }
            }
            else if (*itparam == "-R" || *itparam == "--reschedule")
            {
                // reschedule
                if (!no_reschedule_flag)
                {
                    // not set no-reschedule flag
                    reschedule_flag = true;
                    reschedule = 1;
                }
                else
                {
                    // already set no-reschedule flag
                    result.flag = false;
                    result.message = "You have to choose either of reschedule or no-reschedule.";
                    putLogError(37017, result.message, __FILE__, __LINE__);
                    break;
                }
            }
            else if  (*itparam == "-N" || *itparam == "--no-reschedule")
            {
                // no-reschedule
                if (!reschedule_flag)
                {
                    // not set reschedule flag
                    no_reschedule_flag = true;
                    reschedule = 0;
                }
                else
                {
                    // already set reshcedule flag
                    result.flag = false;
                    result.message = "You have to choose either of reschedule or no-reschedule.";
                    putLogError(37018, result.message, __FILE__, __LINE__);
                    break;
                }
            }
            else
            {
                // set check result flag false
                result.flag = false;
                result.message = "Option error.";
                putLogError(37019, result.message, __FILE__, __LINE__);
                break;
            }

            ++itparam;
        }

        // result check
        if (result.flag)
        {
            // set timeout's default value
            if (!timeout_flag)
            {
                timeout = 3600;
            }

            // set maxlist's default value
            if (!maxlist_flag)
            {
                maxlist = 1024;
            }

            // set reschedule's default value
            if (!reschedule_flag)
            {
                reschedule = 0;
            }
        }

        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == getloglevel())
        {
            boost::format formatter("function : protocol_module_sslid::check_message_result "
                                    "protocol_module_sslid::set_parameter(const std::vector<std::string>& args) : "
                                    "timeout = %d, maxlist = %d, reschedule = %d.");
            formatter % timeout % maxlist % reschedule;
            putLogDebug(30015, formatter.str(), __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/

        unsigned int data_size = 0;
        void* data_addr = NULL;
        data_addr = replication_pay_memory(get_name(), &data_size);

        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == getloglevel())
        {
            putLogDebug(30016, "function : protocol_module_sslid::check_message_result protocol_module_sslid::"
                        "set_parameter() : replication_pay_memory() end.", __FILE__, __LINE__);
            boost::format formatter("function : protocol_module_sslid::check_message_result protocol_module_sslid::"
                        "set_parameter() : data_addr = &(%d), data_size = %d.");
            formatter % data_addr % data_size;
            putLogDebug(30017, formatter.str(), __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/

        if (data_addr == NULL || data_size <= 0)
        {
            // replication area is null
            putLogError(37020, "Replication area is null.", __FILE__, __LINE__);

            /*-------- DEBUG LOG --------*/
            if (LOG_LV_DEBUG == getloglevel())
            {
                boost::format formatter("out_function : protocol_module_sslid::check_message_result "
                                        "protocol_module_sslid::set_parameter("
                                        "const std::vector<std::string>& args) : return_value = ("
                                        "check_message_result.flag = %d, check_message_result.message = %s).");
                formatter % result.flag % result.message;
                putLogDebug(30018, formatter.str(), __FILE__, __LINE__);
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
                putLogDebug );

            /*-------- DEBUG LOG --------*/
            if (LOG_LV_DEBUG == getloglevel())
            {
                boost::format formatter("new : address = &(%d), size = %lu.");
                formatter % static_cast<void*>(replication_data_processor)
                % sizeof(sslid_session_data_processor);
                putLogDebug(30019, formatter.str(), __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/

            replication_data_processor->register_replication_area_lock(replication_area_lock);
            /*-------- DEBUG LOG --------*/
            if (LOG_LV_DEBUG == getloglevel())
            {
                putLogDebug(30020, "protocol_module_sslid::check_message_result protocol_module_sslid::"
                            "set_parameter() : register_replication_area_lock() end.", __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/

            replication_data_processor->register_replication_area_unlock(replication_area_unlock);
            /*-------- DEBUG LOG --------*/
            if (LOG_LV_DEBUG == getloglevel())
            {
                putLogDebug(30021, "protocol_module_sslid::check_message_result protocol_module_sslid::"
                            "set_parameter() : register_replication_area_unlock() end.", __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/

            // create sslid_session_data_processor
            session_data_processor = new sslid_session_data_processor(maxlist,
                    timeout,
                    replication_data_processor,
                    getloglevel,
                    putLogFatal,
                    putLogError,
                    putLogWarn,
                    putLogInfo,
                    putLogDebug);

            /*-------- DEBUG LOG --------*/
            if (LOG_LV_DEBUG == getloglevel())
            {
                boost::format formatter("new : address = &(%d), size = %lu.");
                formatter % static_cast<void*>(session_data_processor)
                % sizeof(sslid_session_data_processor);
                putLogDebug(30022, formatter.str(), __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/

            // restore data from replication area
            sslid_replication_data* redata = replication_data_processor->get_replication_area();
            /*-------- DEBUG LOG --------*/
            if (LOG_LV_DEBUG == getloglevel())
            {
                putLogDebug(30023, "function : protocol_module_sslid::check_message_result protocol_module_sslid::"
                            "set_parameter() : get_replication_area() end.", __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/

            replication_area_lock();
            /*-------- DEBUG LOG --------*/
            if (LOG_LV_DEBUG == getloglevel())
            {
                putLogDebug(30024, "function : protocol_module_sslid::check_message_result protocol_module_sslid::"
                            "set_parameter() : replication_area_lock() end.", __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/

            session_data_processor->read_session_data_from_replication_area(redata);
            /*-------- DEBUG LOG --------*/
            if (LOG_LV_DEBUG == getloglevel())
            {
                putLogDebug(30025, "function : protocol_module_sslid::check_message_result protocol_module_sslid::"
                            "set_parameter() : read_session_data_from_replication_area() end.", __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/

            replication_area_unlock();
            /*-------- DEBUG LOG --------*/
            if (LOG_LV_DEBUG == getloglevel())
            {
                putLogDebug(30026, "function : protocol_module_sslid::check_message_result protocol_module_sslid::"
                            "set_parameter() : replication_area_unlock() end.", __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/
        }
    }
    catch (const std::bad_alloc& ba)
    {
        if (replication_data_processor)
        {
            delete replication_data_processor;
            replication_data_processor = NULL;
        }

        if (session_data_processor)
        {
            delete session_data_processor;
            session_data_processor = NULL;
        }

        std::cerr << "protocol_module_sslid::set_parameter() : exception : Could not allocate memory." << std::endl;
        result.flag = false;
        result.message = "Could not allocate memory.";
        putLogError(37021, result.message, __FILE__, __LINE__);
    }
    catch (const std::exception& e)
    {
        if (replication_data_processor)
        {
            delete replication_data_processor;
            replication_data_processor = NULL;
        }

        if (session_data_processor)
        {
            delete session_data_processor;
            session_data_processor = NULL;
        }

        result.flag = false;
        std::cerr << "protocol_module_sslid::set_parameter() : exception : error = " << e.what() << std::endl;
        boost::format formatter("function : protocol_module_sslid::check_message_result "
                                "protocol_module_sslid::set_parameter() : exception : error = %s.");
        formatter % e.what();
        putLogError(37022, formatter.str(), __FILE__, __LINE__);
    }
    catch (...)
    {
        if (replication_data_processor)
        {
            delete replication_data_processor;
            replication_data_processor = NULL;
        }

        if (session_data_processor)
        {
            delete session_data_processor;
            session_data_processor = NULL;
        }

        result.flag = false;
        std::cerr << "protocol_module_sslid::set_parameter() : Unknown exception." << std::endl;
        putLogError(37023, "function : protocol_module_sslid::check_message_result protocol_module_sslid::"
                    "set_parameter() : Unknown exception.", __FILE__, __LINE__);
    }

    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("out_function : protocol_module_sslid::check_message_result "
                                "protocol_module_sslid::set_parameter("
                                "const std::vector<std::string>& args) : return_value = ("
                                "check_message_result.flag = %d, check_message_result.message = %s).");
        formatter % result.flag % result.message;
        putLogDebug(30027, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    return result;
}

//! parameter add
//! @param[in] module paramter string list
//! @return result.flag true is parameter is noproblem.
//! @return result.flag false is paramter is problem.
protocol_module_sslid::check_message_result protocol_module_sslid::add_parameter(
    const std::vector<std::string>& args)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("in_function : protocol_module_sslid::check_message_result protocol_module_sslid::"
                                "add_parameter(const std::vector<std::string>& args) : args = %s.");
        std::string argsdump;
        for (std::vector<std::string>::const_iterator it = args.begin(); it != args.end(); ++it)
        {
            argsdump += *it;
        }
        formatter % argsdump;
        putLogDebug(30028, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    check_message_result result;
    result.flag = true;

    if (!args.empty())
    {
        // option exist
        result.flag = false;
        result.message = "Cannot add option.";
        putLogError(37024, result.message, __FILE__, __LINE__);
    }

    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("out_function : protocol_module_sslid::check_message_result "
                                "protocol_module_sslid::add_parameter(const std::vector<std::string>& args) : "
                                "return_value = (check_message_result.flag = %d, check_message_result.message = %s).");
        formatter % result.flag % result.message;
        putLogDebug(30029, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    return result;
}

//! realserver list update event
void protocol_module_sslid::handle_rslist_update()
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        putLogDebug(30030, "in/out_function : void protocol_module_sslid::handle_rslist_update().",
                    __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/
}

//! TCP/IP scheduled function registation.
//! @param[in] schedule module TCP/IP scheduled function object type
void protocol_module_sslid::register_schedule(tcp_schedule_func_type inschedule)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        putLogDebug(30031, "in_function : void protocol_module_sslid::register_schedule("
                    "tcp_schedule_func_type inschedule).", __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    schedule_tcp = inschedule;

    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        putLogDebug(30032, "out_function : void protocol_module_sslid::register_schedule("
                    "tcp_schedule_func_type inschedule).", __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/
}

//! UDP scheduled function registation
//! @param[in] schedule module UDP scheduled funtion object type
void protocol_module_sslid::register_schedule(udp_schedule_func_type inschedule)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        putLogDebug(30033, "in/out_function : void protocol_module_sslid::register_schedule("
                    "udp_schedule_func_type inschedule).", __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/
}

//! called from session initialzie use in upstream_thread
//! @param[in] upstream thread id.
//! @param[in] downstream thread id
//! @return session use EVENT mode(ACCEPT, FINALIZE).
protocol_module_base::EVENT_TAG protocol_module_sslid::handle_session_initialize(
    const boost::thread::id up_thread_id,
    const boost::thread::id down_thread_id,
    const boost::asio::ip::tcp::endpoint& client_endpoint_tcp,
    const boost::asio::ip::udp::endpoint& client_endpoint_udp)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_session_initialize(const boost::thread::id up_thread_id, "
                                "const boost::thread::id down_thread_id, "
                                "const boost::asio::ip::tcp::endpoint& client_endpoint_tcp, "
                                "const boost::asio::ip::udp::endpoint& client_endpoint_udp) : "
                                "up_thread_id = %d, down_thread_id = %d.");
        formatter % up_thread_id % down_thread_id;
        putLogDebug(30034, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    EVENT_TAG status = ACCEPT;

    try
    {
        thread_data_ptr threaddata_up(new session_thread_data_sslid);

        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == getloglevel())
        {
            boost::format formatter("new : address = &(%d), size = %lu.");
            formatter % static_cast<void*>(threaddata_up.get()) % sizeof(session_thread_data_sslid);
            putLogDebug(30035, formatter.str(), __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/

        // initialize the up/down thread data
        threaddata_up->realserver_connect_failed_count = 0;
        threaddata_up->data_begain_offset = 0;
        threaddata_up->data_size = 0;
        threaddata_up->current_record_rest_size = 0;
        threaddata_up->hello_message_flag = false;
        threaddata_up->end_flag = END_FLAG_OFF;                         // set end_flag OFF
        threaddata_up->thread_division = THREAD_DIVISION_UP_STREAM;     // up thread division
        threaddata_up->pair_thread_id = down_thread_id;

        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == getloglevel())
        {
            // data dump
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                    "handle_session_initialize() : session_thread_data_sslid(upthread) : "
                                    "realserver_connect_failed_count = %d, data_begain_offset = %d, "
                                    "data_size = %d, current_record_rest_size = %d, hello_message_flag = %d, "
                                    "end_flag = %d, thread_division = %d, pair_thread_id = %d.");
            formatter % threaddata_up->realserver_connect_failed_count
            % threaddata_up->data_begain_offset
            % threaddata_up->data_size
            % threaddata_up->current_record_rest_size
            % threaddata_up->hello_message_flag
            % threaddata_up->end_flag
            % threaddata_up->thread_division
            % threaddata_up->pair_thread_id;
            putLogDebug(30036, formatter.str(), __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/

        thread_data_ptr threaddata_down(new session_thread_data_sslid);

        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == getloglevel())
        {
            boost::format formatter("new : address = &(%d), size = %lu.");
            formatter % static_cast<void*>(threaddata_down.get()) % sizeof(session_thread_data_sslid);
            putLogDebug(30037, formatter.str(), __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/

        threaddata_down->realserver_connect_failed_count = 0;
        threaddata_down->data_begain_offset = 0;
        threaddata_down->data_size = 0;
        threaddata_down->current_record_rest_size = 0;
        threaddata_down->hello_message_flag = false;
        threaddata_down->end_flag = END_FLAG_OFF;                       // set end_flag OFF
        threaddata_down->thread_division = THREAD_DIVISION_DOWN_STREAM; // down thread division
        threaddata_down->pair_thread_id = up_thread_id;

        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == getloglevel())
        {
            // data_dump
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                    "handle_session_initialize() : session_thread_data_sslid(downthread) : "
                                    "realserver_connect_failed_count = %d, data_begain_offset = %d, "
                                    "data_size = %d, current_record_rest_size = %d, hello_message_flag = %d, "
                                    "end_flag = %d, thread_division = %d, pair_thread_id = %d.");
            formatter % threaddata_down->realserver_connect_failed_count
            % threaddata_down->data_begain_offset
            % threaddata_down->data_size
            % threaddata_down->current_record_rest_size
            % threaddata_down->hello_message_flag
            % threaddata_down->end_flag
            % threaddata_down->thread_division
            % threaddata_down->pair_thread_id;
            putLogDebug(30038, formatter.str(), __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/

        {
            // insert the up/down thread data into the map
            boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
            session_thread_data_map[up_thread_id] = threaddata_up;
            session_thread_data_map[down_thread_id] = threaddata_down;
        }
    }
    catch (const std::bad_alloc& ba)
    {
        std::cerr << "protocol_module_sslid::handle_session_initialize() : exception : Could not allocate memory." << std::endl;
        putLogError(37025, "Could not allocate memory.", __FILE__, __LINE__);

        status = FINALIZE;
    }
    catch (const std::exception& e)
    {
        std::cerr << "protocol_module_sslid::handle_session_initialize() : exception : error = " << e.what() << "." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_session_initialize() : exception : error = %s.");
        formatter % e.what();
        putLogError(37026, formatter.str(), __FILE__, __LINE__);

        status = FINALIZE;
    }
    catch (...)
    {
        std::cerr << "protocol_module_sslid::handle_session_initialize() : Unknown exception." << std::endl;
        putLogError(37027, "function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                    "handle_session_initialize() : Unknown exception.", __FILE__, __LINE__);

        status = FINALIZE;
    }

    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_session_initialize(const boost::thread::id up_thread_id, "
                                "const boost::thread::id down_thread_id, "
                                "const boost::asio::ip::tcp::endpoint& client_endpoint_tcp, "
                                "const boost::asio::ip::udp::endpoint& client_endpoint_udp) : return_value = %d.");
        formatter % status;
        putLogDebug(30039, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    return status;
}

//! called from session finalize use in upstream thread.
//! @param[in] upstream thread id.
//! @param[in] downstream thread id
//! @return session use EVENT mode(STOP).
protocol_module_base::EVENT_TAG protocol_module_sslid::handle_session_finalize(
    const boost::thread::id up_thread_id,
    const boost::thread::id down_thread_id)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_session_finalize(const boost::thread::id up_thread_id, "
                                "const boost::thread::id down_thread_id) : "
                                "up_thread_id = %d, down_thread_id = %d.");
        formatter % up_thread_id % down_thread_id;
        putLogDebug(30040, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    try
    {
        // delete the up/down thread data from the map
        boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);

        session_thread_data_map_type::iterator itthread;
        itthread = session_thread_data_map.find(up_thread_id);
        if (itthread != session_thread_data_map.end())
        {
            /*-------- DEBUG LOG --------*/
            if (LOG_LV_DEBUG == getloglevel())
            {
                boost::format formatter("delete : address = &(%d).");
                formatter % static_cast<void*>(itthread->second.get());
                putLogDebug(30041, formatter.str(), __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/
            session_thread_data_map.erase(itthread);
        }

        itthread = session_thread_data_map.find(down_thread_id);
        if (itthread != session_thread_data_map.end())
        {
            /*-------- DEBUG LOG --------*/
            if (LOG_LV_DEBUG == getloglevel())
            {
                boost::format formatter("delete : address = &(%d).");
                formatter % static_cast<void*>(itthread->second.get());
                putLogDebug(30042, formatter.str(), __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/
            session_thread_data_map.erase(itthread);
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "protocol_module_sslid::handle_session_finalize() : exception : error = " << e.what() << "." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_session_finalize() : exception : error = %s.");
        formatter % e.what();
        putLogError(37028, formatter.str(), __FILE__, __LINE__);
    }
    catch (...)
    {
        std::cerr << "protocol_module_sslid::handle_session_finalize() : Unknown exception." << std::endl;
        putLogError(37029, "function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                    "handle_session_finalize() : Unknown exception.", __FILE__, __LINE__);
    }

    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_session_finalize(const boost::thread::id up_thread_id, "
                                "const boost::thread::id down_thread_id) : return_value = %d.");
        formatter % STOP;
        putLogDebug(30043, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    return STOP;
}

//! called from after session accept.in client socket use in upstream thread.
//! @param[in] upstream thread id.
//! @return session use EVENT mode(CLIENT_RECV).
protocol_module_base::EVENT_TAG protocol_module_sslid::handle_accept(
    const boost::thread::id thread_id)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("in/out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_accept(const boost::thread::id thread_id) : thread_id = %d, return_value = %d.");
        formatter % thread_id % CLIENT_RECV;
        putLogDebug(30044, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    return CLIENT_RECV;
}

//! called from after session recv in client socket. use in upstream thread.
//! @param[in] upstream thread id
//! @param[in] recive buffer refarence.
//! @param[in] recive length
//! @return session use EVENT mode(CLIENT_RECV, REALSERVER_SELECT, FINALIZE).
protocol_module_base::EVENT_TAG protocol_module_sslid::handle_client_recv(
    const boost::thread::id thread_id,
    const boost::array<char, MAX_BUFFER_SIZE>& recvbuffer,
    const size_t recvlen)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        size_t buffer_size = recvbuffer.size() < recvlen ? recvbuffer.size() : recvlen;
        std::string buffer(recvbuffer.data(), buffer_size);
        boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_client_recv(const boost::thread::id thread_id, "
                                "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                "const size_t recvlen) : thread_id = %d, recvbuffer = %s, recvlen = %d.");
        formatter % thread_id % buffer % recvlen;
        putLogDebug(30045, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    // find session_thread_data_sslid's pointer from map
    thread_data_ptr threaddata;
    EVENT_TAG status = FINALIZE;
    try
    {
        // parameter check
        if (recvlen > recvbuffer.size())
        {
            std::cerr << "protocol_module_sslid::handle_client_recv() : Data size bigger than buffer size." << std::endl;
            putLogError(37030, "Data size bigger than buffer size.", __FILE__, __LINE__);
            throw -1;
        }

        {
            boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
            session_thread_data_map_type::iterator it = session_thread_data_map.find(thread_id);
            if (it == session_thread_data_map.end() || it->second == NULL)
            {
                putLogError(37031, "Invalid thread id.", __FILE__, __LINE__);

                throw -1;
            }

            threaddata = it->second;
        }

        if (threaddata->end_flag == END_FLAG_ON)
        {
            // end_flag is ON, realserver is disconnect
            status = CLIENT_RECV;
        }
        else
        {
            // end_flag is OFF
            // get the c-style pointer from data buffer
            char* databuf = threaddata->data_buffer.c_array();
            // data length check
            if (threaddata->data_size + recvlen > threaddata->data_buffer.size())
            {
                std::cerr << "protocol_module_sslid::handle_client_recv() : Data size bigger than buffer size." << std::endl;
                putLogError(37032, "Data size bigger than buffer size.", __FILE__, __LINE__);
                threaddata->end_flag = END_FLAG_ON;
                throw -1;
            }

            if (threaddata->data_size > 0 && threaddata->data_begain_offset != 0)
            {
                // data dump
                /*-------- DEBUG LOG --------*/
                if (LOG_LV_DEBUG == getloglevel())
                {
                    std::string datadump;
                    dump_memory(threaddata->data_buffer.data()+threaddata->data_begain_offset,
                                threaddata->data_size, datadump);

                    boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                            "handle_client_recv() : before memmove (data dump) : "
                                            "data begin = %d, data_size = %d, data = %s");
                    formatter % threaddata->data_begain_offset % threaddata->data_size % datadump;
                    putLogDebug(30046, formatter.str(), __FILE__, __LINE__);
                }
                /*------DEBUG LOG END------*/

                // move the data from data start pos to buffer start pos
                memmove(databuf, databuf+threaddata->data_begain_offset, threaddata->data_size);

                /*-------- DEBUG LOG --------*/
                if (LOG_LV_DEBUG == getloglevel())
                {
                    std::string datadump;
                    dump_memory(threaddata->data_buffer.data(),
                                threaddata->data_size, datadump);

                    boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                            "handle_client_recv() : after memmove (data dump) : "
                                            "data begin = 0, data_size = %d, data = %s");
                    formatter % threaddata->data_size % datadump;
                    putLogDebug(30047, formatter.str(), __FILE__, __LINE__);
                }
                /*------DEBUG LOG END------*/
            }

            /*-------- DEBUG LOG --------*/
            if (LOG_LV_DEBUG == getloglevel())
            {
                std::string datadump;
                dump_memory(threaddata->data_buffer.data(),
                            threaddata->data_size+recvlen, datadump);

                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                        "handle_client_recv() : before memcpy (data dump) : "
                                        "data begin = 0, data_size = %d, data = %s");
                formatter % threaddata->data_size % datadump;
                putLogDebug(30048, formatter.str(), __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/

            // copy new recv data to data_buffer
            memcpy(databuf+threaddata->data_size, recvbuffer.data(), recvlen);

            // add data size
            threaddata->data_size += recvlen;
            threaddata->data_begain_offset = 0;

            /*-------- DEBUG LOG --------*/
            if (LOG_LV_DEBUG == getloglevel())
            {
                std::string datadump;
                dump_memory(threaddata->data_buffer.data(),
                            threaddata->data_size+recvlen, datadump);

                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                        "handle_client_recv() : after memcpy (data dump) : "
                                        "data begin = 0, data_size = %d, data = %s");
                formatter % threaddata->data_size % datadump;
                putLogDebug(30049, formatter.str(), __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/

            if (threaddata->current_record_rest_size == 0)
            {
                // new SSL Record start
                size_t alllength = 0;
                bool ishellomessage = false;
                int ret = check_ssl_record_sendable(true,
                                                    threaddata->data_buffer.data()+threaddata->data_begain_offset,
                                                    threaddata->data_size,
                                                    alllength,
                                                    ishellomessage);
                /*-------- DEBUG LOG --------*/
                if (LOG_LV_DEBUG == getloglevel())
                {
                    putLogDebug(30050, "function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_client_recv() : check_ssl_record_sendable() end.", __FILE__, __LINE__);
                }
                /*------DEBUG LOG END------*/

                if (ret == -1)
                {
                    // anomaly, set the end_flag ON
                    threaddata->end_flag = END_FLAG_ON;
                    status = FINALIZE;
                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        putLogDebug(30051, "function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                    "handle_client_recv() : END_FLAG_ON.", __FILE__, __LINE__);
                    }
                    /*------DEBUG LOG END------*/
                }
                else if (ret == 1)
                {
                    // the data can't be sent, set the status CLIENT_RECV
                    status = CLIENT_RECV;
                }
                else
                {
                    // the data can be sent, set the status REALSERVER_SELECT
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
    catch (int e)
    {
        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == getloglevel())
        {
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                    "handle_client_recv() : catch exception e = %d.");
            formatter % e;
            putLogDebug(30052, formatter.str(), __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/
        status = FINALIZE;
    }
    catch (const std::exception& e)
    {
        std::cerr << "protocol_module_sslid::handle_client_recv() : error = " << e.what() << "." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_client_recv() : exception : error = %s.");
        formatter % e.what();
        putLogError(37033, formatter.str(), __FILE__, __LINE__);

        status = FINALIZE;
    }
    catch (...)
    {
        std::cerr << "protocol_module_sslid::handle_client_recv() : Unknown exception." << std::endl;
        putLogError(37034, "function : protocol_module_base::EVENT_TAG "
                    "protocol_module_sslid::handle_client_recv() : Unknown exception.", __FILE__, __LINE__);

        status = FINALIZE;
    }

    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_client_recv(const boost::thread::id thread_id, "
                                "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                "const size_t recvlen) : return_value = %d, end_flag = %d.");
        formatter % status % (threaddata.get() ? threaddata->end_flag : END_FLAG_ON);
        putLogDebug(30053, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    return status;
}

//! called from after realserver select.use in upstream thread.
//! @param[in] upstream thread id
//! @param[out] realserver TCP endpoint
//! @return session use EVENT mode(REALSERVER_CONNECT, CLIENT_DISCONNECT, FINALIZE).
protocol_module_base::EVENT_TAG protocol_module_sslid::handle_realserver_select(
    const boost::thread::id thread_id,
    boost::asio::ip::tcp::endpoint& rs_endpoint)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_realserver_select(const boost::thread::id thread_id, "
                                "boost::asio::ip::tcp::endpoint& rs_endpoint) : "
                                "thread_id = %d, rs_endpoint = [%s]:%d.");
        formatter % thread_id % rs_endpoint.address().to_string() % rs_endpoint.port();
        putLogDebug(30054, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    // find session_thread_data_sslid's pointer from map
    thread_data_ptr threaddata;
    EVENT_TAG status = FINALIZE;

    try
    {
        {
            boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
            session_thread_data_map_type::iterator it = session_thread_data_map.find(thread_id);
            if (it == session_thread_data_map.end() || it->second == NULL)
            {
                putLogError(37035, "Invalid thread id.", __FILE__, __LINE__);
                /*-------- DEBUG LOG --------*/
                if (LOG_LV_DEBUG == getloglevel())
                {
                    boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                            "handle_realserver_select(const boost::thread::id thread_id, "
                                            "boost::asio::ip::tcp::endpoint& rs_endpoint) : return_value = %d.");
                    formatter % FINALIZE;
                    putLogDebug(30055, formatter.str(), __FILE__, __LINE__);
                }
                /*------DEBUG LOG END------*/
                putLogInfo(35000, "Realserver decision failure.", __FILE__, __LINE__);

                return FINALIZE;
            }

            threaddata = it->second;
        }

        if (threaddata->realserver_connect_failed_count <= 0)
        {
            // the first connection or connected successful
            if (realserver_selected(threaddata->selected_realserver))
            {
                /*-------- DEBUG LOG --------*/
                if (LOG_LV_DEBUG == getloglevel())
                {
                    putLogDebug(30056, "function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_realserver_select() end.", __FILE__, __LINE__);
                }
                /*------DEBUG LOG END------*/

                // realserver already connected
                rs_endpoint = threaddata->selected_realserver;
                status = REALSERVER_CONNECT;
            }
            else
            {
                /*-------- DEBUG LOG --------*/
                if (LOG_LV_DEBUG == getloglevel())
                {
                    putLogDebug(30057, "function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_realserver_select() end.", __FILE__, __LINE__);
                }
                /*------DEBUG LOG END------*/

                // first connection
                if (!threaddata->hello_message_flag)
                {
                    // the first connection, the theaddata must be hellomessage
                    threaddata->end_flag = END_FLAG_ON;
                    status = FINALIZE;
                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        putLogDebug(30058, "function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                    "handle_realserver_select() : END_FLAG_ON.", __FILE__, __LINE__);
                    }
                    /*------DEBUG LOG END------*/
                }
                else
                {
                    // get sessionid from the data buffer
                    std::string session_id;
                    if (get_ssl_session_id(threaddata->data_buffer.data()+threaddata->data_begain_offset,
                                           threaddata->data_size, session_id) == -1)
                    {
                        /*-------- DEBUG LOG --------*/
                        if (LOG_LV_DEBUG == getloglevel())
                        {
                            boost::format formatter("out_function : protocol_module_base::EVENT_TAG "
                                                    "protocol_module_sslid::handle_realserver_select("
                                                    "const boost::thread::id thread_id, boost::asio::"
                                                    "ip::tcp::endpoint& rs_endpoint) : return_value = %d.");
                            formatter % FINALIZE;
                            putLogDebug(30059, formatter.str(), __FILE__, __LINE__);
                        }
                        /*------DEBUG LOG END------*/

                        putLogInfo(35001, "Realserver decision failure.", __FILE__, __LINE__);

                        threaddata->end_flag = END_FLAG_ON;
                        return FINALIZE;
                    }

                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        putLogDebug(30060, "function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                    "handle_realserver_select() : get_ssl_session_id() end.", __FILE__, __LINE__);
                    }
                    /*------DEBUG LOG END------*/

                    if (session_id.empty())
                    {
                        // no session id in hellomesseage
                        // schedule the endpoint
                        boost::asio::ip::tcp::endpoint temp_endpoint;
                        boost::asio::ip::tcp::endpoint comp_endpoint;   // for compare the endpoint
                        schedule_tcp(thread_id, rs_list_begin, rs_list_end, rs_list_next, temp_endpoint);
                        if (temp_endpoint != comp_endpoint)
                        {
                            // success for get the endpoint by reschedule
                            threaddata->selected_realserver = temp_endpoint;
                            rs_endpoint = temp_endpoint;
                            status = REALSERVER_CONNECT;
                        }
                        else
                        {
                            // failed to get the endpoint by reschedule
                            // set end_flag ON, and turn the status to CLIENT_DISCONNECT
                            threaddata->end_flag = END_FLAG_ON;
                            status = CLIENT_DISCONNECT;
                            /*-------- DEBUG LOG --------*/
                            if (LOG_LV_DEBUG == getloglevel())
                            {
                                putLogDebug(30061, "function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                            "handle_realserver_select() : END_FLAG_ON.", __FILE__, __LINE__);
                            }
                            /*------DEBUG LOG END------*/
                        }
                    }
                    else
                    {
                        // the session id is in the hellomessage
                        // try to get the endpoint from session data by session id
                        int ret = session_data_processor->get_endpoint_from_session_data(
                                      session_id,
                                      threaddata->selected_realserver);
                        /*-------- DEBUG LOG --------*/
                        if (LOG_LV_DEBUG == getloglevel())
                        {
                            putLogDebug(30062, "function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                        "handle_realserver_select() : get_endpoint_from_session_data() end.",
                                        __FILE__, __LINE__);
                        }
                        /*------DEBUG LOG END------*/

                        if (ret == 0)
                        {
                            // get the endpoint successfully
                            rs_endpoint = threaddata->selected_realserver;
                            status = REALSERVER_CONNECT;
                        }
                        else
                        {
                            // failed to get the endpoint
                            if (reschedule == 1)
                            {
                                // reschedule mode
                                boost::asio::ip::tcp::endpoint temp_endpoint;
                                boost::asio::ip::tcp::endpoint comp_endpoint;   // for compare the endpoint
                                schedule_tcp(thread_id, rs_list_begin, rs_list_end, rs_list_next, temp_endpoint);
                                if (temp_endpoint != comp_endpoint)
                                {
                                    // get the endpoint by reschedule successfully
                                    threaddata->selected_realserver = temp_endpoint;
                                    rs_endpoint = temp_endpoint;
                                    status = REALSERVER_CONNECT;
                                }
                                else
                                {
                                    // failed to get the endpoint by reschedule
                                    // set end_flag ON, and set the status CLIENT_DISCONNECT
                                    threaddata->end_flag = END_FLAG_ON;
                                    status = CLIENT_DISCONNECT;
                                    /*-------- DEBUG LOG --------*/
                                    if (LOG_LV_DEBUG == getloglevel())
                                    {
                                        putLogDebug(30063, "function : protocol_module_base::EVENT_TAG "
                                                    "protocol_module_sslid::handle_realserver_select() : "
                                                    "END_FLAG_ON.", __FILE__, __LINE__);
                                    }
                                    /*------DEBUG LOG END------*/
                                }
                            }
                            else
                            {
                                // no reschedule mode
                                // set end_flag ON and disconnect the client
                                threaddata->end_flag = END_FLAG_ON;
                                status = CLIENT_DISCONNECT;
                                /*-------- DEBUG LOG --------*/
                                if (LOG_LV_DEBUG == getloglevel())
                                {
                                    putLogDebug(30064, "function : protocol_module_base::EVENT_TAG "
                                                "protocol_module_sslid::handle_realserver_select() : END_FLAG_ON.",
                                                __FILE__, __LINE__);
                                }
                                /*------DEBUG LOG END------*/
                            }
                        }
                    }
                }
            }
        }
        else if (threaddata->realserver_connect_failed_count < realserver_connect_failed_max_count)
        {
            // try multi times connect
            if (reschedule == 1)
            {
                // reschedule mode
                boost::asio::ip::tcp::endpoint temp_endpoint;
                boost::asio::ip::tcp::endpoint comp_endpoint;   // for compare the endpoint
                schedule_tcp(thread_id, rs_list_begin, rs_list_end, rs_list_next, temp_endpoint);
                if (temp_endpoint != comp_endpoint)
                {
                    // get the endpoint by reschedule successfully
                    threaddata->selected_realserver = temp_endpoint;
                    rs_endpoint = temp_endpoint;
                    status = REALSERVER_CONNECT;
                }
                else
                {
                    // failed to get the endpoint by reschedule
                    // set end_flag ON, and set the status CLIENT_DISCONNECT
                    threaddata->end_flag = END_FLAG_ON;
                    status = CLIENT_DISCONNECT;
                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        putLogDebug(30065, "function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                    "handle_realserver_select() : END_FLAG_ON.", __FILE__, __LINE__);
                    }
                    /*------DEBUG LOG END------*/
                }
            }
            else
            {
                // no reschedule mode
                // set end_flag ON and disconnect the client
                threaddata->end_flag = END_FLAG_ON;
                status = CLIENT_DISCONNECT;
                /*-------- DEBUG LOG --------*/
                if (LOG_LV_DEBUG == getloglevel())
                {
                    putLogDebug(30066, "function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_realserver_select() : END_FLAG_ON.", __FILE__, __LINE__);
                }
                /*------DEBUG LOG END------*/
            }
        }
        else
        {
            // connect failed
            // set end_flag ON and disconnect the client
            threaddata->end_flag = END_FLAG_ON;
            status = CLIENT_DISCONNECT;
            /*-------- DEBUG LOG --------*/
            if (LOG_LV_DEBUG == getloglevel())
            {
                putLogDebug(30067, "function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                            "handle_realserver_select() : END_FLAG_ON.", __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "protocol_module_sslid::handle_realserver_select() : exception : error = " << e.what() << "." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_realserver_select() : exception : error = %s.");
        formatter % e.what();
        putLogError(37036, formatter.str(), __FILE__, __LINE__);

        putLogInfo(35002, "Realserver decision failure.", __FILE__, __LINE__);

        status = FINALIZE;
    }
    catch (...)
    {
        std::cerr << "protocol_module_sslid::handle_realserver_select() : Unknown exception." << std::endl;
        putLogError(37037, "function : protocol_module_base::EVENT_TAG "
                    "protocol_module_sslid::handle_realserver_select() : Unknown exception.", __FILE__, __LINE__);

        putLogInfo(35003, "Realserver decision failure.", __FILE__, __LINE__);

        status = FINALIZE;
    }

    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_realserver_select(const boost::thread::id thread_id, "
                                "boost::asio::ip::tcp::endpoint& rs_endpoint) : return_value = %d, "
                                "end_flag = %d, rs_endpoint = [%s]:%d.");
        formatter % status % (threaddata.get() ? threaddata->end_flag : END_FLAG_ON)
	 % rs_endpoint.address().to_string() % rs_endpoint.port();
        putLogDebug(30068, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    return status;
}

//! called from after realserver select
//! @param[in] upstream thread id
//! @param[out] realserver UDP endpoint
//! @param[out] sendbudffer reference
//! @param[out] send data length
//! @return session use EVENT mode(STOP).
protocol_module_base::EVENT_TAG protocol_module_sslid::handle_realserver_select(
    const boost::thread::id thread_id,
    boost::asio::ip::udp::endpoint& rs_endpoint,
    boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
    size_t& datalen)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("in/out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_realserver_select(const boost::thread::id thread_id, "
                                "boost::asio::ip::udp::endpoint& rs_endpoint, "
                                "boost::array<char,MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen) : "
                                "return_value = %d.");
        formatter % STOP;
        putLogDebug(30069, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/
    return STOP;
}

//! called from after realserver connect
//! @param[in] upstream thread id
//! @param[out] sendbuffer reference
//! @param[out] send data length
//! @return session use EVENT mode(REALSERVER_SEND, FINALIZE).
protocol_module_base::EVENT_TAG protocol_module_sslid::handle_realserver_connect(
    const boost::thread::id thread_id,
    boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
    size_t& datalen)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_realserver_connect(const boost::thread::id thread_id, "
                                "boost::array<char,MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen) : "
                                "thread_id = %d.");
        formatter % thread_id;
        putLogDebug(30070, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    EVENT_TAG status = REALSERVER_SEND;
    try
    {
        if (put_data_to_sendbuffer(thread_id, sendbuffer, datalen) == -1)
        {
            // can't find the thread from the thread map
            status = FINALIZE;
        }
        else
        {
            // find session_thread_data_sslid's pointer from map
            boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);

            session_thread_data_map_type::iterator it = session_thread_data_map.find(thread_id);
            if (it == session_thread_data_map.end())
            {
                putLogError(37038, "Invalid thread id.", __FILE__, __LINE__);
                status = FINALIZE;
            }
            else
            {
                it->second->realserver_connect_failed_count = 0;
            }
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "protocol_module_sslid::handle_realserver_connect() : exception : error = " << e.what() << "." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_realserver_connect() exception : error = %s.");
        formatter % e.what();
        putLogError(37039, formatter.str(), __FILE__, __LINE__);
        status = FINALIZE;
    }
    catch (...)
    {
        std::cerr << "protocol_module_sslid::handle_realserver_connect() : Unknown exception." << std::endl;
        putLogError(37040, "function protocol_module_base::EVENT_TAG "
                    "protocol_module_sslid::handle_realserver_connect() : Unknown exception.", __FILE__, __LINE__);

        status = FINALIZE;
    }

    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_realserver_connect(const boost::thread::id thread_id, "
                                "boost::array<char,MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen) : "
                                "return_value = %d.");
        formatter % status;
        putLogDebug(30071, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    return status;
}

//! called from after realserver connection fail
//! @param[in] upstream thread id
//! @param[in] fail realserver endpoint reference
//! @return session use EVENT mode(REALSERVER_SELECT, CLIENT_DISCONNECT, FINALIZE).
protocol_module_base::EVENT_TAG protocol_module_sslid::handle_realserver_connection_fail(
    const boost::thread::id thread_id,
    const boost::asio::ip::tcp::endpoint& rs_endpoint)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_realserver_connection_fail(const boost::thread::id thread_id, "
                                "const boost::asio::ip::tcp::endpoint& rs_endpoint) : "
                                "thread_id = %d, rs_endpoint = [%s]:%d.");
        formatter % thread_id % rs_endpoint.address().to_string() % rs_endpoint.port();
        putLogDebug(30072, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    // find session_thread_data_sslid's pointer from map
    thread_data_ptr threaddata;
    EVENT_TAG status = FINALIZE;

    try
    {
        {
            boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
            session_thread_data_map_type::iterator it = session_thread_data_map.find(thread_id);
            if (it == session_thread_data_map.end() || it->second == NULL)
            {
                putLogError(37041, "Invalid thread id.", __FILE__, __LINE__);
                /*-------- DEBUG LOG --------*/
                if (LOG_LV_DEBUG == getloglevel())
                {
                    boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                            "handle_realserver_connection_fail(const boost::thread::id thread_id, "
                                            "const boost::asio::ip::tcp::endpoint& rs_endpoint) : return_value = %d.");
                    formatter % FINALIZE;
                    putLogDebug(30073, formatter.str(), __FILE__, __LINE__);
                }
                /*------DEBUG LOG END------*/
                return FINALIZE;
            }

            threaddata = it->second;
        }

        if (reschedule == 1)
        {
            // reschedule mode
            ++threaddata->realserver_connect_failed_count;
            status = REALSERVER_SELECT;
        }
        else
        {
            // no reschedule mode
            // set end_flag ON
            threaddata->end_flag = END_FLAG_ON;
            status = CLIENT_DISCONNECT;

            /*-------- DEBUG LOG --------*/
            if (LOG_LV_DEBUG == getloglevel())
            {
                putLogDebug(30074, "function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                            "handle_realserver_connection_fail() : END_FLAG_ON.", __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "protocol_module_sslid::handle_realserver_connection_fail() : exception : error = " << e.what() << "." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_realserver_connection_fail() : exception : error = %s.");
        formatter % e.what();
        putLogError(37042, formatter.str(), __FILE__, __LINE__);

        status = FINALIZE;
    }
    catch (...)
    {
        std::cerr << "protocol_module_sslid::handle_realserver_connection_fail() : Unknown exception." << std::endl;
        putLogError(37043, "function : protocol_module_base::EVENT_TAG "
                    "protocol_module_sslid::handle_realserver_connection_fail() : Unknown exception.",
                    __FILE__, __LINE__);

        status = FINALIZE;
    }

    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_realserver_connection_fail(const boost::thread::id thread_id, "
                                "const boost::asio::ip::tcp::endpoint& rs_endpoint) : "
                                "return_value = %d, end_flag = %d.");
        formatter % status % (threaddata.get() ? threaddata->end_flag : END_FLAG_ON);
        putLogDebug(30075, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    return status;
}

//! called from after realserver send.
//! @param[in] upstream thread id
//! @return session use EVENT mode(CLIENT_RECV, REALSERVER_CONNECT, FINALIZE).
protocol_module_base::EVENT_TAG protocol_module_sslid::handle_realserver_send(
    const boost::thread::id thread_id)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_realserver_send(const boost::thread::id thread_id) : thread_id = %d.");
        formatter % thread_id;
        putLogDebug(30076, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    // find session_thread_data_sslid's pointer from map
    thread_data_ptr threaddata;
    EVENT_TAG status = FINALIZE;

    try
    {
        {
            boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
            session_thread_data_map_type::iterator it = session_thread_data_map.find(thread_id);
            if (it == session_thread_data_map.end() || it->second == NULL)
            {
                putLogError(37044, "Invalid thread id.", __FILE__, __LINE__);
                /*-------- DEBUG LOG --------*/
                if (LOG_LV_DEBUG == getloglevel())
                {
                    boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                            "handle_realserver_send(const boost::thread::id thread_id) : "
                                            "return_value = %d.");
                    formatter % FINALIZE;
                    putLogDebug(30077, formatter.str(), __FILE__, __LINE__);
                }
                /*------DEBUG LOG END------*/

                return FINALIZE;
            }

            threaddata = it->second;
        }

        if (threaddata->data_size <= 0)
        {
            // data buffer is empty
            status = CLIENT_RECV;
        }
        else
        {
            // send the remaining data
            if (threaddata->current_record_rest_size == 0)
            {
                // new SSL Record start
                size_t alllength = 0;
                bool ishellomessage = false;
                int ret = check_ssl_record_sendable(true,
                                                    threaddata->data_buffer.data() + threaddata->data_begain_offset,
                                                    threaddata->data_size,
                                                    alllength,
                                                    ishellomessage);
                /*-------- DEBUG LOG --------*/
                if (LOG_LV_DEBUG == getloglevel())
                {
                    putLogDebug(30078, "function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_realserver_send() : check_ssl_record_sendable() end.", __FILE__, __LINE__);
                }
                /*------DEBUG LOG END------*/

                if (ret == -1)
                {
                    // anomaly, set the end_flag ON
                    threaddata->end_flag = END_FLAG_ON;
                    status = FINALIZE;
                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        putLogDebug(30079, "function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                    "handle_realserver_send() : END_FLAG_ON.", __FILE__, __LINE__);
                    }
                    /*------DEBUG LOG END------*/
                }
                else if (ret == 1)
                {
                    // the data can't be sent, set the status CLIENT_RECV
                    status = CLIENT_RECV;
                }
                else
                {
                    // the data can be sent, set the status REALSERVER_SELECT
                    // set client helloworld shakehand flag
                    threaddata->hello_message_flag = ishellomessage;
                    threaddata->current_record_rest_size = alllength;
                    status = REALSERVER_CONNECT;
                }
            }
            else
            {
                status = REALSERVER_CONNECT;
            }
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "protocol_module_sslid::handle_realserver_send() : exception : error = " << e.what() << "." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_realserver_send() : exception : error = %s.");
        formatter % e.what();
        putLogError(37045, formatter.str(), __FILE__, __LINE__);

        status = FINALIZE;
    }
    catch (...)
    {
        std::cerr << "protocol_module_sslid::handle_realserver_send() : Unknown exception." << std::endl;
        putLogError(37046, "function : protocol_module_base::EVENT_TAG "
                    "protocol_module_sslid::handle_realserver_send() : Unknown exception.", __FILE__, __LINE__);

        status = FINALIZE;
    }

    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_realserver_send(const boost::thread::id thread_id) : "
                                "return_value = %d, end_flag = %d.");
        formatter % status % (threaddata.get() ? threaddata->end_flag : END_FLAG_ON);
        putLogDebug(30080, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    return status;
}

//! called from after sorryserver select
//! @param[in] upstream thread id
//! @param[in] sorryserver endppiont reference
//! @return session use EVENT mode(STOP).
protocol_module_base::EVENT_TAG protocol_module_sslid::handle_sorryserver_select(
    const boost::thread::id thread_id,
    boost::asio::ip::tcp::endpoint& sorry_endpoint)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("in/out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_sorryserver_select(const boost::thread::id thread_id, "
                                "boost::asio::ip::tcp::endpoint& sorry_endpoint) : return_value = %d.");
        formatter % STOP;
        putLogDebug(30081, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    return STOP;
}

//! called from after sorryserver connect
//! @param[in] upstream thread id
//! @param[out] send buffer reference.
//! @param[out] send length
//! @return session use EVENT mode(STOP).
protocol_module_base::EVENT_TAG protocol_module_sslid::handle_sorryserver_connect(
    const boost::thread::id thread_id,
    boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
    size_t& datalen)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("in/out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_sorryserver_connect(const boost::thread::id thread_id, "
                                "boost::array<char,MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen) : "
                                "return_value = %d.");
        formatter % STOP;
        putLogDebug(30082, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    return STOP;
}

//! called from after sorryserver connection fail
//! @param[in] upstream thread id
//! @param[in] sorryserver endpoint reference.
//! @return session use EVENT mode(STOP).
protocol_module_base::EVENT_TAG protocol_module_sslid::handle_sorryserver_connection_fail(
    const boost::thread::id thread_id,
    const boost::asio::ip::tcp::endpoint& sorry_endpoint)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("in/out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_sorryserver_connection_fail(const boost::thread::id thread_id, "
                                "const boost::asio::ip::tcp::endpoint& sorry_endpoint) : return_value = %d.");
        formatter % STOP;
        putLogDebug(30083, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    return STOP;
}

//! called from after sorryserver send
//! @param[in] upstream thread id
//! @return session use EVENT mode(STOP).
protocol_module_base::EVENT_TAG protocol_module_sslid::handle_sorryserver_send(
    const boost::thread::id thread_id)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("in/out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_sorryserver_send(const boost::thread::id thread_id) : return_value = %d.");
        formatter % STOP;
        putLogDebug(30084, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    return STOP;
}

//! called from after realserver recvive for TCP/IP
//! @param[in] downstream thread id
//! @param[in] realserver TCP/IP endpoint reference
//! @param[in] realserver recive buffer reference.
//! @param[in] recv data length
//! @return session use EVENT mode(REALSERVER_RECV, CLIENT_CONNECTION_CHECK, FINALIZE).
protocol_module_base::EVENT_TAG protocol_module_sslid::handle_realserver_recv(
    const boost::thread::id thread_id,
    const boost::asio::ip::tcp::endpoint& rs_endpoint,
    const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,
    const size_t recvlen)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        size_t buffer_size = recvbuffer.size() < recvlen ? recvbuffer.size() : recvlen;
        std::string buffer(recvbuffer.data(), buffer_size);
        boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_realserver_recv(const boost::thread::id thread_id, "
                                "const boost::asio::ip::tcp::endpoint& rs_endpoint, "
                                "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                "const size_t recvlen) : thread_id = %d, rs_endpoint = [%s]:%d, "
                                "recvbuffer = %s, recvlen = %d.");
        formatter % thread_id % rs_endpoint.address().to_string() % rs_endpoint.port() %
        buffer % recvlen;
        putLogDebug(30085, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    // parameter check
    if (recvlen > recvbuffer.size())
    {
        std::cerr << "protocol_module_sslid::handle_realserver_recv() : Data size bigger than buffer size." << std::endl;
        putLogError(37047, "Data size bigger than buffer size.", __FILE__, __LINE__);
        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == getloglevel())
        {
            boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                    "handle_realserver_recv(const boost::thread::id thread_id, "
                                    "const boost::asio::ip::tcp::endpoint& rs_endpoint, "
                                    "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                    "const size_t recvlen) : return_value = %d.");
            formatter % FINALIZE;
            putLogDebug(30086, formatter.str(), __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/

        return FINALIZE;
    }

    // find session_thread_data_sslid's pointer from map
    thread_data_ptr threaddata;
    EVENT_TAG status = FINALIZE;

    try
    {
        {
            boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
            session_thread_data_map_type::iterator it = session_thread_data_map.find(thread_id);
            if (it == session_thread_data_map.end() || it->second == NULL)
            {
                putLogError(37048, "Invalid thread id.", __FILE__, __LINE__);
                /*-------- DEBUG LOG --------*/
                if (LOG_LV_DEBUG == getloglevel())
                {
                    boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                            "handle_realserver_recv(const boost::thread::id thread_id, "
                                            "const boost::asio::ip::tcp::endpoint& rs_endpoint, "
                                            "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                            "const size_t recvlen) : return_value = %d.");
                    formatter % FINALIZE;
                    putLogDebug(30087, formatter.str(), __FILE__, __LINE__);
                }
                /*------DEBUG LOG END------*/

                return FINALIZE;
            }

            threaddata = it->second;
            // realserver endpoint set
            threaddata->selected_realserver = rs_endpoint;
        }

        // get the c-style pointer from data buffer
        char* databuf = threaddata->data_buffer.c_array();
        // data length check
        if (threaddata->data_size + recvlen > threaddata->data_buffer.size())
        {
            std::cerr << "protocol_module_sslid::handle_realserver_recv() : Data size bigger than buffer size." << std::endl;
            putLogError(37049, "Data size bigger than buffer size.", __FILE__, __LINE__);

            /*-------- DEBUG LOG --------*/
            if (LOG_LV_DEBUG == getloglevel())
            {
                boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                        "handle_realserver_recv(const boost::thread::id thread_id, "
                                        "const boost::asio::ip::tcp::endpoint& rs_endpoint, "
                                        "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                        "const size_t recvlen) : return_value = %d.");
                formatter % FINALIZE;
                putLogDebug(30088, formatter.str(), __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/

            threaddata->end_flag = END_FLAG_ON;
            return FINALIZE;
        }

        if (threaddata->data_size > 0 && threaddata->data_begain_offset != 0)
        {
            // data dump
            /*-------- DEBUG LOG --------*/
            if (LOG_LV_DEBUG == getloglevel())
            {
                std::string datadump;
                dump_memory(threaddata->data_buffer.data()+threaddata->data_begain_offset,
                            threaddata->data_size, datadump);

                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                        "handle_realserver_recv() : before memmove (data dump) : "
                                        "data begin = %d, data_size = %d, data = %s");
                formatter % threaddata->data_begain_offset % threaddata->data_size % datadump;
                putLogDebug(30089, formatter.str(), __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/

            // move the data from data start pos to buffer start pos
            memmove(databuf, databuf+threaddata->data_begain_offset, threaddata->data_size);

            /*-------- DEBUG LOG --------*/
            if (LOG_LV_DEBUG == getloglevel())
            {
                std::string datadump;
                dump_memory(threaddata->data_buffer.data(),
                            threaddata->data_size, datadump);

                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                        "handle_realserver_recv() : after memmove (data dump) : "
                                        "data begin = 0, data_size = %d, data = %s");
                formatter % threaddata->data_size % datadump;
                putLogDebug(30090, formatter.str(), __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/
        }

        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == getloglevel())
        {
            std::string datadump;
            dump_memory(threaddata->data_buffer.data(),
                        threaddata->data_size+recvlen, datadump);

            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                    "handle_realserver_recv() : before memcpy (data dump) : "
                                    "data begin = 0, data_size = %d, data = %s");
            formatter % threaddata->data_size % datadump;
            putLogDebug(30091, formatter.str(), __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/

        // copy new recv data to data_buffer
        memcpy(databuf+threaddata->data_size, recvbuffer.data(), recvlen);

        // add data size
        threaddata->data_size += recvlen;
        threaddata->data_begain_offset = 0;

        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == getloglevel())
        {
            std::string datadump;
            dump_memory(threaddata->data_buffer.data(),
                        threaddata->data_size+recvlen, datadump);

            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                    "handle_realserver_recv() : after memcpy (data dump) : "
                                    "data begin = 0, data_size = %d, data = %s");
            formatter % threaddata->data_size % datadump;
            putLogDebug(30092, formatter.str(), __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/

        if (threaddata->current_record_rest_size == 0)
        {
            // new SSL Record start
            size_t alllength = 0;
            bool ishellomessage = false;
            int ret = check_ssl_record_sendable(false,
                                                threaddata->data_buffer.data()+threaddata->data_begain_offset,
                                                threaddata->data_size,
                                                alllength,
                                                ishellomessage);
            /*-------- DEBUG LOG --------*/
            if (LOG_LV_DEBUG == getloglevel())
            {
                putLogDebug(30093, "function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                            "handle_realserver_recv() : check_ssl_record_sendable() END.", __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/

            if (ret == -1)
            {
                // anomaly, set the end_flag ON
                threaddata->end_flag = END_FLAG_ON;
                status = FINALIZE;
                /*-------- DEBUG LOG --------*/
                if (LOG_LV_DEBUG == getloglevel())
                {
                    putLogDebug(30094, "function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_realserver_recv() : END_FLAG_ON.", __FILE__, __LINE__);
                }
                /*------DEBUG LOG END------*/
            }
            else if (ret == 1)
            {
                // the data can't be sent, set the status CLIENT_RECV
                status = REALSERVER_RECV;
            }
            else
            {
                // the data can be sent, set the status REALSERVER_SELECT
                // set client helloworld shakehand flag
                threaddata->hello_message_flag = ishellomessage;
                threaddata->current_record_rest_size = alllength;
                status = CLIENT_CONNECTION_CHECK;
            }
        }
        else
        {
            status = CLIENT_CONNECTION_CHECK;
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "protocol_module_sslid::handle_realserver_recv() : exception : error = " << e.what() << "." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_realserver_recv() : exception : error = %s.");
        formatter % e.what();
        putLogError(37050, formatter.str(), __FILE__, __LINE__);

        status = FINALIZE;
    }
    catch (...)
    {
        std::cerr << "protocol_module_sslid::handle_realserver_recv() : Unknown exception." << std::endl;
        putLogError(37051, "function : protocol_module_base::EVENT_TAG "
                    "protocol_module_sslid::handle_realserver_recv() : Unknown exception.", __FILE__, __LINE__);

        status = FINALIZE;
    }

    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_realserver_recv(const boost::thread::id thread_id, "
                                "const boost::asio::ip::tcp::endpoint& rs_endpoint, "
                                "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                "const size_t recvlen) : return_value = %d, end_flag = %d.");
        formatter % status % (threaddata.get() ? threaddata->end_flag : END_FLAG_ON);
        putLogDebug(30095, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    return status;
}

//! called from after realserver recive.for UDP
//! @param[in] downstream thread id
//! @param[in] realserver UDP endpoint reference
//! @param[in] recive from realserver buffer reference
//! @param[in] recv data length
//! @return session use EVENT mode(STOP).
protocol_module_base::EVENT_TAG protocol_module_sslid::handle_realserver_recv(
    const boost::thread::id thread_id,
    const boost::asio::ip::udp::endpoint& rs_endpoint,
    const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,
    const size_t recvlen)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("in/out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_realserver_recv(const boost::thread::id thread_id, "
                                "const boost::asio::ip::udp::endpoint& rs_endpoint, "
                                "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                "const size_t recvlen) : return_value = %d.");
        formatter % STOP;
        putLogDebug(30096, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    return STOP;
}

//! called from after sorryserver recive
//! @param[in] downstream thread id
//! @param[in] sorryserver endpoint reference
//! @param[in] recive from realserver buffer reference.
//! @param[in] recv data length
//! @return 	session use EVENT mode(STOP).
protocol_module_base::EVENT_TAG protocol_module_sslid::handle_sorryserver_recv(
    const boost::thread::id thread_id,
    const boost::asio::ip::tcp::endpoint& sorry_endpoint,
    const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,
    const size_t recvlen)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("in/out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_sorryserver_recv(const boost::thread::id thread_id, "
                                "const boost::asio::ip::tcp::endpoint& sorry_endpoint, "
                                "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                "const size_t recvlen) : return_value = %d.");
        formatter % STOP;
        putLogDebug(30097, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    return STOP;
}

//! called from UPSTEEARM thread. make module original message.
//! @param[in] downstream thread id.
//! @return session use EVENT mode(STOP).
protocol_module_base::EVENT_TAG protocol_module_sslid::handle_response_send_inform(
    const boost::thread::id thread_id)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("in/out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_response_send_inform(const boost::thread::id thread_id) : return_value = %d.");
        formatter % STOP;
        putLogDebug(30098, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    return STOP;
}

//! called from after client connection check. use TCP/IP only. create client send message.
//! @param[in]	downstream thread id
//! @param[out]	send budffer reference
//! @param[out]	send data length
//! @return 	session use EVENT mode(CLIENT_SEND, FINALIZE).
protocol_module_base::EVENT_TAG protocol_module_sslid::handle_client_connection_check(
    const boost::thread::id thread_id,
    boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
    size_t& datalen)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_client_connection_check(const boost::thread::id thread_id, "
                                "boost::array<char,MAX_BUFFER_SIZE>& sendbuffer, "
                                "size_t& datalen) : thread_id = %d.");
        formatter % thread_id;
        putLogDebug(30099, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    // find session_thread_data_sslid's pointer from map
    thread_data_ptr threaddata;
    EVENT_TAG status = FINALIZE;

    try
    {
        {
            boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
            session_thread_data_map_type::iterator it = session_thread_data_map.find(thread_id);
            if (it == session_thread_data_map.end() || it->second == NULL)
            {
                putLogError(37052, "Invalid thread id.", __FILE__, __LINE__);

                /*-------- DEBUG LOG --------*/
                if (LOG_LV_DEBUG == getloglevel())
                {
                    boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                            "handle_client_connection_check(const boost::thread::id thread_id, "
                                            "boost::array<char,MAX_BUFFER_SIZE>& sendbuffer, "
                                            "size_t& datalen) : return_value = %d.");
                    formatter % FINALIZE;
                    putLogDebug(30100, formatter.str(), __FILE__, __LINE__);
                }
                /*------DEBUG LOG END------*/

                return FINALIZE;
            }

            threaddata = it->second;
        }

        if (threaddata->current_record_rest_size == 0)
        {
            // new SSL Record
            if (threaddata->hello_message_flag)
            {
                // it's server hello message
                // get sessionid from the data buffer
                std::string session_id;
                if (get_ssl_session_id(threaddata->data_buffer.data()+threaddata->data_begain_offset,
                                       threaddata->data_size, session_id) == -1)
                {
                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        boost::format formatter("out_function : protocol_module_base::EVENT_TAG "
                                                "protocol_module_sslid::handle_client_connection_check("
                                                "const boost::thread::id thread_id, "
                                                "boost::array<char,MAX_BUFFER_SIZE>& sendbuffer, "
                                                "size_t& datalen) : return_value = %d.");
                        formatter % FINALIZE;
                        putLogDebug(30101, formatter.str(), __FILE__, __LINE__);
                    }
                    /*------DEBUG LOG END------*/

                    threaddata->end_flag = END_FLAG_ON;
                    return FINALIZE;
                }

                /*-------- DEBUG LOG --------*/
                if (LOG_LV_DEBUG == getloglevel())
                {
                    putLogDebug(30102, "function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_client_connection_check() : get_ssl_session_id() end.", __FILE__, __LINE__);
                }
                /*------DEBUG LOG END------*/

                if (!session_id.empty())
                {
                    // session id exist
                    time_t now;
                    time(&now);
                    session_data_processor->write_session_data(session_id,
                            threaddata->selected_realserver,
                            now);

                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        putLogDebug(30103, "function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                    "handle_client_connection_check() : write_session_data() end.",
                                    __FILE__, __LINE__);
                    }
                    /*------DEBUG LOG END------*/
                }
            }
        }

        // put datao to send
        if (put_data_to_sendbuffer(thread_id, sendbuffer, datalen) == -1)
        {
            // can't find the thread from the thread map
            threaddata->end_flag = END_FLAG_ON;
            status = FINALIZE;
        }
        else
        {
            status = CLIENT_SEND;
        }

        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == getloglevel())
        {
            putLogDebug(30104, "function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                        "handle_client_connection_check() : put_data_to_sendbuffer() end.",
                        __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/
    }
    catch (const std::exception& e)
    {
        std::cerr << "protocol_module_sslid::handle_client_connection_check() : exception : error = " << e.what() << "." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_client_connection_check() : exception : error = %s.");
        formatter % e.what();
        putLogError(37053, formatter.str(), __FILE__, __LINE__);

        status = FINALIZE;
    }
    catch (...)
    {
        std::cerr << "protocol_module_sslid::handle_client_connection_check() : Unknown exception." << std::endl;
        putLogError(37054, "function : protocol_module_base::EVENT_TAG "
                    "protocol_module_sslid::handle_client_connection_check() : Unknown exception.", __FILE__, __LINE__);

        status = FINALIZE;
    }

    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_client_connection_check(const boost::thread::id thread_id, "
                                "boost::array<char,MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen) : "
                                "return_value = %d.");
        formatter % status;
        putLogDebug(30105, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    return status;
}

//! called from after client select. use UDP only
//! @param[in] downstream thread id
//! @param[in] client udp endpoint
//! @param[out]	send buffer reference
//! @param[out]	send data length
//! @return 	session use EVENT mode(STOP).
protocol_module_base::EVENT_TAG protocol_module_sslid::handle_client_select(
    const boost::thread::id thread_id,
    boost::asio::ip::udp::endpoint& cl_endpoint,
    boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
    size_t& datalen)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("in/out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_client_select(const boost::thread::id thread_id, "
                                "boost::asio::ip::udp::endpoint& cl_endpoint, "
                                "boost::array<char,MAX_BUFFER_SIZE>& sendbuffer, "
                                "size_t& datalen) : return_value = %d.");
        formatter % STOP;
        putLogDebug(30106, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    return STOP;
}

//! called from after client send
//! @param[in] downstream thread id
//! @return 	session use EVENT mode(CLIENT_DISCONNECT,
//!              REALSERVER_RECV, CLIENT_CONNECTION_CHECK, FINALIZE).
protocol_module_base::EVENT_TAG protocol_module_sslid::handle_client_send(
    const boost::thread::id thread_id)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_client_send(const boost::thread::id thread_id) : thread_id = %d.");
        formatter % thread_id;
        putLogDebug(30107, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    // find session_thread_data_sslid's pointer from map
    thread_data_ptr threaddata;
    EVENT_TAG status = FINALIZE;

    try
    {
        {
            boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
            session_thread_data_map_type::iterator it = session_thread_data_map.find(thread_id);
            if (it == session_thread_data_map.end() || it->second == NULL)
            {
                putLogError(37055, "Invalid thread id.", __FILE__, __LINE__);
                /*-------- DEBUG LOG --------*/
                if (LOG_LV_DEBUG == getloglevel())
                {
                    boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                            "handle_client_send(const boost::thread::id thread_id) : return_value = %d.");
                    formatter % FINALIZE;
                    putLogDebug(30108, formatter.str(), __FILE__, __LINE__);
                }
                /*------DEBUG LOG END------*/

                return FINALIZE;
            }

            threaddata = it->second;
        }

        if (threaddata->data_size <= 0)
        {
            /*-------- DEBUG LOG --------*/
            if (LOG_LV_DEBUG == getloglevel())
            {
                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                        "handle_client_send() : data_size = %d, end_flag = %d.");
                formatter % threaddata->data_size % threaddata->end_flag;
                putLogDebug(30109, formatter.str(), __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/

            // all data already send to the client
            if (threaddata->end_flag == END_FLAG_ON)
            {
                // end_flag is ON
                status = CLIENT_DISCONNECT;
            }
            else
            {
                // end_flag is OFF
                status = REALSERVER_RECV;
            }
        }
        else
        {
            /*-------- DEBUG LOG --------*/
            if (LOG_LV_DEBUG == getloglevel())
            {
                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                        "handle_client_send() : data_size = %d, current_record_rest_size = %d.");
                formatter % threaddata->data_size % threaddata->current_record_rest_size;
                putLogDebug(30110, formatter.str(), __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/

            // remain data to send to the client
            if (threaddata->current_record_rest_size == 0)
            {
                // new SSL Record start
                size_t alllength = 0;
                bool ishellomessage = false;
                int ret = check_ssl_record_sendable(false,
                                                    threaddata->data_buffer.data() + threaddata->data_begain_offset,
                                                    threaddata->data_size,
                                                    alllength,
                                                    ishellomessage);
                /*-------- DEBUG LOG --------*/
                if (LOG_LV_DEBUG == getloglevel())
                {
                    boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                            "handle_client_send() : check_ssl_record_sendable() end. "
                                            "return_value = %d.");
                    formatter % ret;
                    putLogDebug(30111, formatter.str(), __FILE__, __LINE__);
                }
                /*------DEBUG LOG END------*/

                if (ret == -1)
                {
                    // anomaly, set the end_flag ON
                    threaddata->end_flag = END_FLAG_ON;
                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        putLogDebug(30112, "function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                    "handle_client_send() : END_FLAG_ON.", __FILE__, __LINE__);
                    }
                    /*------DEBUG LOG END------*/

                    status = FINALIZE;
                }
                else if (ret == 1)
                {
                    // the data is unsendable
                    if (threaddata->end_flag == END_FLAG_ON)
                    {
                        // end_flag is ON
                        status = CLIENT_DISCONNECT;
                    }
                    else
                    {
                        // end_flag is OFF
                        status = REALSERVER_RECV;
                    }
                }
                else
                {
                    // the data is sendable, set the status CLIENT_CONNECTION_CHECK
                    // set client helloworld shakehand flag
                    threaddata->hello_message_flag = ishellomessage;
                    threaddata->current_record_rest_size = alllength;
                    status = CLIENT_CONNECTION_CHECK;
                }
            }
            else
            {
                // other data
                status = CLIENT_CONNECTION_CHECK;
            }
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "protocol_module_sslid::handle_client_send() : exception : error = " << e.what() << "." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_client_send() : exception : error = %s.");
        formatter % e.what();
        putLogError(37056, formatter.str(), __FILE__, __LINE__);

        status = FINALIZE;
    }
    catch (...)
    {
        std::cerr << "protocol_module_sslid::handle_client_send() : Unknown exception." << std::endl;
        putLogError(37057, "function : protocol_module_base::EVENT_TAG "
                    "protocol_module_sslid::handle_client_send() : Unknown exception.", __FILE__, __LINE__);

        status = FINALIZE;
    }

    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_client_send(const boost::thread::id thread_id) : "
                                "return_value = %d, end_flag = %d.");
        formatter % status % (threaddata.get() ? threaddata->end_flag : END_FLAG_ON);
        putLogDebug(30113, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    return status;
}

//! call from client disconnect event. use upstream thread and downstream thread.
//! @param[in] upstream and downstream thread id(check! one thread one event!)
//! @return 	session use EVENT mode(FINALIZE).
protocol_module_base::EVENT_TAG protocol_module_sslid::handle_client_disconnect(
    const boost::thread::id thread_id)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("in/out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_client_disconnect(const boost::thread::id thread_id) : "
                                "thread_id = %d, return_value = %d.");
        formatter % thread_id % FINALIZE;
        putLogDebug(30114, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    return FINALIZE;
}

//! call from sorry mode event. use upstream thread and downstream thread
//! @param[in] upstream and downstream thread id(check! one thread one event and first time call pattern)
//! @return 	session use EVENT mode(STOP).
protocol_module_base::EVENT_TAG protocol_module_sslid::handle_sorry_enable(
    const boost::thread::id thread_id)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("in/out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_sorry_enable(const boost::thread::id thread_id) : return_value = %d.");
        formatter % STOP;
        putLogDebug(30115, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    return STOP;
}

//! call from sorry mode disable. use upstream thread and downstream thread.
//! @param[in] upstream and downstream thread id(check! one thread one event)
//! @return 	session use EVENT mode(STOP).
protocol_module_base::EVENT_TAG protocol_module_sslid::handle_sorry_disable(
    const boost::thread::id thread_id)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("in/out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_sorry_disable(const boost::thread::id thread_id) : return_value = %d.");
        formatter % STOP;
        putLogDebug(30116, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    return STOP;
}

//! call from realserver disconnect. use upstream thread and downstream thread
//! @param[in] upstream and downstream thread id(check! one thread one event)
//! @param[in] disconnected realserver endpoint.
//! @return 	session use EVENT mode(STOP).
protocol_module_base::EVENT_TAG protocol_module_sslid::handle_realserver_disconnect(
    const boost::thread::id thread_id,
    const boost::asio::ip::tcp::endpoint& rs_endpoint)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("in/out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_realserver_disconnect(const boost::thread::id thread_id, "
                                "const boost::asio::ip::tcp::endpoint& rs_endpoint) : return_value = %d.");
        formatter % STOP;
        putLogDebug(30117, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    return STOP;
}

//! call from sorry server disconnect. use upstraem thread and downstream thread
//! @param[in] upstream and downstream thread id(check! one thread one event)
//! @param[in] disconnect sorryserver endpoint
//! @return session use EVENT mode(STOP).
protocol_module_base::EVENT_TAG protocol_module_sslid::handle_sorryserver_disconnect(
    const boost::thread::id thread_id,
    const boost::asio::ip::tcp::endpoint& sorry_endpoint)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("in/out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_sorryserver_disconnect(const boost::thread::id thread_id, "
                                "const boost::asio::ip::tcp::endpoint& sorry_endpoint) : return_value = %d.");
        formatter % STOP;
        putLogDebug(30118, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    return STOP;
}

//! call from realserver disconnect. use upstream thread and downstream thread.
//! @param[in] upstream and downstream thread id(check! one thread one event)
//! @param[in] disconnect realserver endpoint
//! @return session use EVENT mode(STOP).
protocol_module_base::EVENT_TAG protocol_module_sslid::handle_realserver_close(
    const boost::thread::id thread_id,
    const boost::asio::ip::udp::endpoint& rs_endpoint)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("in/out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_realserver_close(const boost::thread::id thread_id, "
                                "const boost::asio::ip::udp::endpoint& rs_endpoint) : return_value = %d.");
        formatter % STOP;
        putLogDebug(30119, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    return STOP;
}

//! replication interval interrrupt
//! timer thread call this function. from virtualservice.
void protocol_module_sslid::replication_interrupt()
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        putLogDebug(30120, "in_function : void protocol_module_sslid::replication_interrupt().",
                    __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/
    replication_data_processor->write_replicaion_area();
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        putLogDebug(30121, "function : void protocol_module_sslid::replication_interrupt() : "
                    "write_replication_area() end.", __FILE__, __LINE__);
        putLogDebug(30122, "out_function : void protocol_module_sslid::replication_interrupt().",
                    __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/
}

//! put data into send buffer function
//! @param[in] upstream and downstream thread id
//! @param[out] sendbudffer reference
//! @param[out] send data length
//! @return 0 : success
//! @return -1: failed
int protocol_module_sslid::put_data_to_sendbuffer(
    const boost::thread::id& thread_id,
    boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
    size_t& datalen)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("in_function : int protocol_module_sslid::put_data_to_sendbuffer("
                                "const boost::thread::id& thread_id, "
                                "boost::array<char,MAX_BUFFER_SIZE>& sendbuffer, "
                                "size_t& datalen) : thread_id = %d.");
        formatter % thread_id;
        putLogDebug(30123, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    // find session_thread_data_sslid's pointer from map
    thread_data_ptr threaddata;
    datalen = 0;

    {
        boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
        session_thread_data_map_type::iterator it = session_thread_data_map.find(thread_id);
        if (it == session_thread_data_map.end() || it->second == NULL)
        {
            putLogError(37058, "Invalid thread id.", __FILE__, __LINE__);

            /*-------- DEBUG LOG --------*/
            if (LOG_LV_DEBUG == getloglevel())
            {
                putLogDebug(30124, "out_function : int protocol_module_sslid::put_data_to_sendbuffer("
                            "const boost::thread::id& thread_id, "
                            "boost::array<char,MAX_BUFFER_SIZE>& sendbuffer, "
                            "size_t& datalen) : return_value = -1.", __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/

            return -1;
        }

        threaddata = it->second;
    }

    size_t sendbufsize = sendbuffer.size();
    const char* databegin = threaddata->data_buffer.data();
    char* senddatabegin = sendbuffer.c_array();

    if (threaddata->current_record_rest_size >= threaddata->data_size)
    {
        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == getloglevel())
        {
            putLogDebug(30125, "function : int protocol_module_sslid::"
                        "put_data_to_sendbuffer() : current_record_rest_size >= data_size.",
                        __FILE__, __LINE__);
        }

        // remain data size greater than all data size
        if (threaddata->data_size <= sendbufsize)
        {
            // dump data
            /*-------- DEBUG LOG --------*/
            if (LOG_LV_DEBUG == getloglevel())
            {
                std::string datadump;
                dump_memory(threaddata->data_buffer.data(),
                            threaddata->data_size, datadump);

                boost::format formatter("function : int protocol_module_sslid::"
                                        "put_data_to_sendbuffer() : data_size <= sendbuffer size. "
                                        "before memcpy (data dump) : data_begin_offset = %d, "
                                        "current_record_rest_size = %d, dump data begin = 0, "
                                        "dump data_size = %d, dump data = %s");
                formatter % threaddata->data_begain_offset % threaddata->current_record_rest_size
                % threaddata->data_size % datadump;
                putLogDebug(30126, formatter.str(), __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/

            // all data size is less than or equals with the send data buffer size
            // put the data to send data buffer
            memcpy(senddatabegin, databegin+threaddata->data_begain_offset, threaddata->data_size);

            // begin offset init
            threaddata->data_begain_offset = 0;
            // remain data size minus
            threaddata->current_record_rest_size -= threaddata->data_size;
            // return data length
            datalen = threaddata->data_size;
            // all data size init
            threaddata->data_size = 0;

            /*-------- DEBUG LOG --------*/
            if (LOG_LV_DEBUG == getloglevel())
            {
                std::string datadump;
                dump_memory(threaddata->data_buffer.data(),
                            threaddata->data_size, datadump);

                boost::format formatter("function : int protocol_module_sslid::"
                                        "put_data_to_sendbuffer() : data_size <= sendbuffer size. "
                                        "after memcpy (data dump) : data_begin_offset = %d, "
                                        "current_record_rest_size = %d, dump data begin = 0, "
                                        "dump data_size = %d, dump data = %s");
                formatter % threaddata->data_begain_offset % threaddata->current_record_rest_size
                % threaddata->data_size % datadump;
                putLogDebug(30127, formatter.str(), __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/
        }
        else
        {
            // dump data
            /*-------- DEBUG LOG --------*/
            if (LOG_LV_DEBUG == getloglevel())
            {
                std::string datadump;
                dump_memory(senddatabegin, sendbufsize, datadump);

                boost::format formatter("function : int protocol_module_sslid::"
                                        "put_data_to_sendbuffer() : data_size > sendbuffer size. "
                                        "before memcpy (data dump) : data_begin_offset = %d, "
                                        "current_record_rest_size = %d, dump data begin = 0, "
                                        "dump data_size = %d, dump data = %s");
                formatter % threaddata->data_begain_offset % threaddata->current_record_rest_size
                % threaddata->data_size % datadump;
                putLogDebug(30128, formatter.str(), __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/

            // all data size greater than the send data buffer size
            // put the data to send data buffer
            memcpy(senddatabegin, databegin+threaddata->data_begain_offset, sendbufsize);

            // begin offset add
            threaddata->data_begain_offset += sendbufsize;
            // remian data size minus
            threaddata->current_record_rest_size -= sendbufsize;
            // return data length
            datalen = sendbufsize;
            // all data size minus
            threaddata->data_size -= sendbufsize;

            /*-------- DEBUG LOG --------*/
            if (LOG_LV_DEBUG == getloglevel())
            {
                std::string datadump;
                dump_memory(senddatabegin, sendbufsize, datadump);

                boost::format formatter("function : int protocol_module_sslid::"
                                        "put_data_to_sendbuffer() : data_size > sendbuffer size. "
                                        "after memcpy (data dump) : data_begin_offset = %d, "
                                        "current_record_rest_size = %d, dump data begin = 0, "
                                        "dump data_size = %d, dump data = %s");
                formatter % threaddata->data_begain_offset % threaddata->current_record_rest_size
                % threaddata->data_size % datadump;
                putLogDebug(30129, formatter.str(), __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/
        }
    }
    else
    {
        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == getloglevel())
        {
            putLogDebug(30130, "function : int protocol_module_sslid::"
                        "put_data_to_sendbuffer() : current_record_rest_size < data_size.",
                        __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/

        // remain data size less than all data size
        if  (threaddata->current_record_rest_size <= sendbufsize)
        {
            // dump data
            /*-------- DEBUG LOG --------*/
            if (LOG_LV_DEBUG == getloglevel())
            {
                std::string datadump;
                dump_memory(senddatabegin, threaddata->current_record_rest_size, datadump);

                boost::format formatter("function : int protocol_module_sslid::"
                                        "put_data_to_sendbuffer() : current_record_rest_size <= sendbuffer size. "
                                        "before memcpy (data dump) : data_begin_offset = %d, "
                                        "current_record_rest_size = %d, dump data begin = 0, "
                                        "dump data_size = %d, dump data = %s");
                formatter % threaddata->data_begain_offset % threaddata->current_record_rest_size
                % threaddata->data_size % datadump;
                putLogDebug(30131, formatter.str(), __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/

            // remain data size less than or equals with the send data buffer size
            // put the data to send data buffer
            memcpy(senddatabegin, databegin+threaddata->data_begain_offset, threaddata->current_record_rest_size);

            // begin offset add
            threaddata->data_begain_offset += threaddata->current_record_rest_size;
            // all data size minus
            threaddata->data_size -= threaddata->current_record_rest_size;
            // return data length
            datalen = threaddata->current_record_rest_size;
            // begin offset init
            threaddata->current_record_rest_size = 0;

            /*-------- DEBUG LOG --------*/
            if (LOG_LV_DEBUG == getloglevel())
            {
                std::string datadump;
                dump_memory(senddatabegin, threaddata->current_record_rest_size, datadump);

                boost::format formatter("function : int protocol_module_sslid::"
                                        "put_data_to_sendbuffer() : current_record_rest_size <= sendbuffer size. "
                                        "after memcpy (data dump) : data_begin_offset = %d, "
                                        "current_record_rest_size = %d, dump data begin = 0, "
                                        "dump data_size = %d, dump data = %s");
                formatter % threaddata->data_begain_offset % threaddata->current_record_rest_size
                % threaddata->data_size % datadump;
                putLogDebug(30132, formatter.str(), __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/
        }
        else
        {
            // data dump
            /*-------- DEBUG LOG --------*/
            if (LOG_LV_DEBUG == getloglevel())
            {
                std::string datadump;
                dump_memory(senddatabegin, sendbufsize, datadump);

                boost::format formatter("function : int protocol_module_sslid::"
                                        "put_data_to_sendbuffer() : current_record_rest_size > sendbuffer size. "
                                        "before memcpy (data dump) : data_begin_offset = %d, "
                                        "current_record_rest_size = %d, dump data begin = 0, "
                                        "dump data_size = %d, dump data = %s");
                formatter % threaddata->data_begain_offset % threaddata->current_record_rest_size
                % threaddata->data_size % datadump;
                putLogDebug(30133, formatter.str(), __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/

            // remain data size greater than send data buffersize
            // put the data to send data buffer
            memcpy(senddatabegin, databegin+threaddata->data_begain_offset, sendbufsize);

            // offset add
            threaddata->data_begain_offset += sendbufsize;
            // all data size minus
            threaddata->data_size -= sendbufsize;
            // return data length
            datalen = sendbufsize;
            // remain data size minus
            threaddata->current_record_rest_size -= sendbufsize;

            /*-------- DEBUG LOG --------*/
            if (LOG_LV_DEBUG == getloglevel())
            {
                std::string datadump;
                dump_memory(senddatabegin, sendbufsize, datadump);

                boost::format formatter("function : int protocol_module_sslid::"
                                        "put_data_to_sendbuffer() : current_record_rest_size > sendbuffer size. "
                                        "after memcpy (data dump) : data_begin_offset = %d, "
                                        "current_record_rest_size = %d, dump data begin = 0, "
                                        "dump data_size = %d, dump data = %s");
                formatter % threaddata->data_begain_offset % threaddata->current_record_rest_size
                % threaddata->data_size % datadump;
                putLogDebug(30134, formatter.str(), __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/
        }
    }

    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        putLogDebug(30135, "out_function : int protocol_module_sslid::put_data_to_sendbuffer("
                    "const boost::thread::id& thread_id, "
                    "boost::array<char,MAX_BUFFER_SIZE>& sendbuffer, "
                    "size_t& datalen) : return_value = 0.", __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    return 0;
}

//! judge pointed endpoint whether selected
//! @param[in] endpoint refrence
//! @return false if endpoint is not selected
//! @return true if endpoint is selected
bool protocol_module_sslid::realserver_selected(const boost::asio::ip::tcp::endpoint& rs_endpoint)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("in_function : bool protocol_module_sslid::realserver_selected("
                                "const boost::asio::ip::tcp::endpoint& rs_endpoint) : rs_endpoint = [%s]:%d.");
        formatter % rs_endpoint.address().to_string() % rs_endpoint.port();
        putLogDebug(30136, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    boost::asio::ip::tcp::endpoint temp_endpoint;
    if (temp_endpoint ==  rs_endpoint)
    {
        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == getloglevel())
        {
            putLogDebug(30137, "out_function : bool protocol_module_sslid::realserver_selected("
                        "const boost::asio::ip::tcp::endpoint& rs_endpoint) : return_value = false.",
                        __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/

        // equals to default endpoint
        return false;
    }
    else
    {
        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == getloglevel())
        {
            putLogDebug(30138, "out_function : bool protocol_module_sslid::realserver_selected("
                        "const boost::asio::ip::tcp::endpoint& rs_endpoint) : return_value = true.",
                        __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/
        // already connected
        return true;
    }
}

}
