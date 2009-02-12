#include <vector>
#include <list>
#include <sstream>
#include <algorithm>
#include <boost/thread/pthread/mutex.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/format.hpp>
#include "protocol_module_sessionless.h"

namespace l7vs
{

const std::string protocol_module_sessionless::MODULE_NAME = "sessionless";
const int protocol_module_sessionless::THREAD_DIVISION_UP_STREAM = 0; //上りスレッド
const int protocol_module_sessionless::THREAD_DIVISION_DOWN_STREAM = 1;  //下りスレッド

const int protocol_module_sessionless::END_FLAG_OFF = 0; //終了フラグOFF
const int protocol_module_sessionless::END_FLAG_ON = 1; //終了フラグON

const int protocol_module_sessionless::ACCEPT_END_FLAG_OFF = 0; //ACCEPT完了フラグOFF
const int protocol_module_sessionless::ACCEPT_END_FLAG_ON = 1; //ACCEPT完了フラグON

const int protocol_module_sessionless::SORRY_FLAG_ON = 1; //SORRY状態
const int protocol_module_sessionless::SORRY_FLAG_OFF = 0; //SORRY状態以外

const int protocol_module_sessionless::SORRYSERVER_SWITCH_FLAG_OFF = 0; //sorryserver切替中以外
const int protocol_module_sessionless::SORRYSERVER_SWITCH_FLAG_ON = 1; //sorryserver切替中

const int protocol_module_sessionless::REALSERVER_SWITCH_FLAG_OFF = 0; //realserver切替中以外
const int protocol_module_sessionless::REALSERVER_SWITCH_FLAG_ON = 1; //realserver切替中

const int protocol_module_sessionless::EDIT_DIVISION_NO_EDIT = 0; //編集無し
const int protocol_module_sessionless::EDIT_DIVISION_EDIT = 1; //編集あり

const int protocol_module_sessionless::FORWARDED_FOR_OFF = 0; //forwarded_forフラグOFF
const int protocol_module_sessionless::FORWARDED_FOR_ON = 1;  //forwarded_forフラグON
//! constractor
protocol_module_sessionless::protocol_module_sessionless() :
        http_protocol_module_base(MODULE_NAME), forwarded_for(FORWARDED_FOR_OFF)
{
    //	/*-------- DEBUG LOG --------*/
    //	if (LOG_LV_DEBUG == getloglevel())
    //	{
    //		putLogDebug(010000, "in/out_function: protocol_module_sessionless::"
    //			"protocol_module_sessionless()", __FILE__, __LINE__ );
    //
    //		boost::format formatter("out_function: protocol_module_sessionless::protocol_module_sessionless(): "
    //			"modename=%s");
    //		formatter % MODULE_NAME;
    //		putLogDebug(010001, formatter.str(), __FILE__, __LINE__ );
    //	}
    //	/*------DEBUG LOG END------*/
    sorry_uri.assign('\0');
}
//! destractor
protocol_module_sessionless::~protocol_module_sessionless()
{
//	/*-------- DEBUG LOG --------*/
//	if (LOG_LV_DEBUG == getloglevel())
//	{
//		putLogDebug(010002, "in/out_function: protocol_module_sessionless::"
//			"~protocol_module_sessionless()", __FILE__, __LINE__ );
//	}
//	/*------DEBUG LOG END------*/
}
//! tcp protocol support check
//! @return tcp support is true
//! @return tcp not-support is false
bool protocol_module_sessionless::is_tcp()
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        putLogDebug(10003, "in/out_function : bool protocol_module_sessionless::is_tcp() : "
                    "return_value = true.", __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/
    return true;
}

//! udp protocol support check
//! @return udp support is true
//! @return udp not-support is false
bool protocol_module_sessionless::is_udp()
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        putLogDebug(10004, "in/out_function : bool protocol_module_sessionless::is_udp() : "
                    "return_value = false.", __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/
    return false;
}

//! replication interval interrrupt
//! timer thread call this function. from virtualservice.
void protocol_module_sessionless::replication_interrupt()
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        putLogDebug(10002, "in/out_function : protocol_module_sessionless::"
                    "replication_interrupt().", __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/
}
//! initialize function. called from module control. module loaded call
//! @param[in]	realserver list iterator begin function object type
//!	@param[in]	realserver list iterator end function object type
//! @param[in]	realserver list iterator next function object type
//! @param[in]	realserver list mutex lock function object type.
//! @param[in]	realserver list mutex unlock function object type
void protocol_module_sessionless::initialize(rs_list_itr_func_type	inlist_begin,
        rs_list_itr_func_type	inlist_end,
        rs_list_itr_next_func_type	inlist_next,
        boost::function< void( void ) >	inlist_lock,
        boost::function< void( void ) >	inlist_unlock)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        putLogDebug(10005, "in_function : void protocol_module_sessionless::initialize("
                    "rs_list_itr_func_type inlist_begin, rs_list_itr_func_type inlist_end, "
                    "rs_list_itr_next_func_type inlist_next, boost::function< void( void ) > "
                    "inlist_lock, boost::function< void( void ) > inlist_unlock).", __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    //RealServer list begin function
    rs_list_begin = inlist_begin;
    //RealServer list end function
    rs_list_end = inlist_end;
    //RealServer list next function
    rs_list_next = inlist_next;
    //RealServer list lock funtion
    rs_list_lock = inlist_lock;
    //RealServer list unlock funtion
    rs_list_unlock = inlist_unlock;

    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        putLogDebug(10006, "out_function : void protocol_module_sessionless::initialize("
                    "rs_list_itr_func_type inlist_begin, rs_list_itr_func_type inlist_end, "
                    "rs_list_itr_next_func_type inlist_next, boost::function< void( void ) > "
                    "inlist_lock, boost::function< void( void ) > inlist_unlock).", __FILE__, __LINE__ );
    }
    /*-------- DEBUG LOG --------*/
}

//! finalize called from module control. module unloaded call.
void protocol_module_sessionless::finalize()
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        putLogDebug(10007, "in_function : void protocol_module_sessionless::finalize().", __FILE__,
                    __LINE__ );
    }
    /*------DEBUG LOG END------*/

    //RealServer list functions initializtion
    //RealServer list begin function
    rs_list_begin.clear();
    //RealServer list end function
    rs_list_end.clear();
    //RealServer list next function
    rs_list_next.clear();
    //RealServer list lock function
    rs_list_lock.clear();
    //RealServer list unlock function
    rs_list_unlock.clear();

    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        putLogDebug(10007, "function : void protocol_module_sessionless::finalize() : "
                    "rs_list_begin.clear(), rs_list_end.clear(), rs_list_next.clear(), rs_list_lock.clear(), rs_list_unlock.clear() end.", __FILE__,
                    __LINE__ );
    }
    /*------DEBUG LOG END------*/

    //Replication functions initializtion
    //component memory allcate function
    replication_pay_memory.clear();
    //component memory lock function
    replication_area_lock.clear();
    //component memory unlock function
    replication_area_unlock.clear();

    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        putLogDebug(10007, "function : void protocol_module_sessionless::finalize() : "
                    "replication_pay_memory.clear(), replication_area_lock.clear(), replication_area_unlock.clear() end.", __FILE__,
                    __LINE__ );
    }
    /*------DEBUG LOG END------*/

    //ScheduleModule's functions initializtion
    //Client振り分け処理関数
    schedule_tcp.clear();

    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        putLogDebug(10007, "function : void protocol_module_sessionless::finalize() : "
                    "schedule_tcp.clear(), schedule_udp.clear() end.", __FILE__,
                    __LINE__ );
    }
    /*------DEBUG LOG END------*/

    //Module's option initializtion
    //forwarded_for
    forwarded_for = FORWARDED_FOR_OFF;
    //sorry-uri
    sorry_uri.assign('\0');
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        putLogDebug(10007, "function : void protocol_module_sessionless::finalize() : "
                    "forwarded_for = 0, sorry_uri.assign('\0') end.", __FILE__,
                    __LINE__ );
    }
    /*------DEBUG LOG END------*/

    //logger functions initializtion
    //log level getting function
    getloglevel.clear();
    //logger（Fatal）
    putLogFatal.clear();
    //logger（Error）
    putLogError.clear();
    //logger（Warn）
    putLogWarn.clear();
    //logger（Info）
    putLogInfo.clear();
    //logger（Debug）
    putLogDebug.clear();
}

//! sorry support check
//! @return true sorry mode is supported.
//! @return false sorry mode is unsupported.
bool protocol_module_sessionless::is_use_sorry()
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        putLogDebug(10007, "in/out_function : bool protocol_module_sessionless::is_use_sorry() : "
                    "return_value = true.", __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/
    return true;
}

//! realserver list update event
void protocol_module_sessionless::handle_rslist_update()
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        putLogDebug(10005, "in/out_function : void protocol_module_sessionless::handle_rslist_update().", __FILE__,
                    __LINE__ );
    }
    /*------DEBUG LOG END------*/
}

//! module parameter check.used by l7vsadm
//! @param[in]	module paramter string list
//! @return	result.flag true is parameter is noproblem.
//! @return result.flag false is paramter is problem.
protocol_module_base::check_message_result protocol_module_sessionless::check_parameter(const std::vector<
        std::string>& args)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("in_function : protocol_module_base::check_message_result "
                                "protocol_module_sessionless::check_parameter("
                                "const std::vector<std::string>& args) : args = %s.");
        std::string argsdump;
        for (std::vector<std::string>::const_iterator it = args.begin(); it != args.end(); ++it)
        {
            argsdump += *it;
        }
        formatter % argsdump;
        putLogDebug(10010, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    //set check result to ture
    check_message_result check_result;
    check_result.flag = true;
    bool forward_checked = false;
    bool sorryuri_checked = false;
    const std::string regex_match_str = "(/([0-9a-zA-Z$\\-_.+!*'(),;:@&=]|%[0-9a-fA-F]{2})*)+";
    typedef std::vector<std::string>::const_iterator vec_str_it;

    try
    {
        vec_str_it it = args.begin();
        vec_str_it it_end = args.end();
        //loop option strings
        for (; it != it_end; ++it)
        {
            //option string = "-F"
            if (*it == "-F" || *it == "--forwarded-for")
            {
                //set forward flag to ON
                forward_checked = true;
            }
            //option string = "-S"
            else if (*it == "-S" || *it == "--sorry-uri")
            {
                //set sorryURI flag OFF
                if (!sorryuri_checked)
                {
                    //next item exist
                    if (++it != it_end)
                    {
                        if (!it->empty() && (it->substr(0, 1) == "-" || it->substr(0, 2) == "--"))
                        {
                            //set check flag to FALSE
                            check_result.flag = false;
                            //set check result message
                            check_result.message = "You have to set option value '-S/--sorry-uri'.";
                            putLogError(17000, check_result.message, __FILE__, __LINE__ );
                            //loop break;
                            break;
                        }
                        //next option string's length ＞ 127
                        if (it->size() > MAX_OPTION_SIZE - 1)
                        {
                            std::ostringstream ostr;
                            ostr << "'-S/--sorry-uri' option value '" << *it << "' is too long.";

                            //set check flag to FALSE
                            check_result.flag = false;
                            //set check result message
                            check_result.message = ostr.str();

                            putLogError(17000, check_result.message, __FILE__, __LINE__ );
                            //loop break;
                            break;
                        }
                        //next option string's length ≦ 127
                        else
                        {
                            //regex check
                            //regex is ([^?#]*)(\?([^#]*))?(#(.*))?
                            boost::regex uri(regex_match_str);
                            boost::cmatch what;
                            if (boost::regex_match(it->c_str(), what, uri))
                            {
                                //check OK
                                //set sorryURI flag ON
                                sorryuri_checked = true;
                            }
                            //check NG
                            else
                            {
                                std::ostringstream ostr;
                                ostr << "'-S/--sorry-uri' option value '" << *it << "' is not a valid URI.";

                                //set check flag to FALSE
                                check_result.flag = false;
                                //set check result message
                                check_result.message = ostr.str();
                                putLogError(17000, check_result.message, __FILE__, __LINE__ );
                                //loop break
                                break;
                            }
                        }
                    }
                    //next item is not exist
                    else
                    {
                        //set check flag to FALSE
                        check_result.flag = false;
                        //set check result message
                        check_result.message = "You have to set option value '-S/--sorry-uri'.";
                        putLogError(17000, check_result.message, __FILE__,
                                    __LINE__ );
                        //loop break
                        break;
                    }
                }
                //sorryURI flag = ON
                else
                {
                    //set check flag to FALSE
                    check_result.flag = false;
                    //set check result message
                    check_result.message = "Cannot set multiple option '-S/--sorry-uri'.";
                    putLogError(17000, check_result.message, __FILE__,
                                __LINE__ );
                    //loop break
                    break;
                }
            }
            //other option string
            else
            {
                //set check flag to FALSE
                check_result.flag = false;
                //set check result message
                check_result.message = "Option error.";
                putLogError(17000, check_result.message, __FILE__, __LINE__ );
                //loop break
                break;
            }
        }
    }
    catch (const std::exception& ex)
    {
        check_result.flag = false;
        std::cerr << "check parameter exception: error = " << ex.what() << "." << std::endl;
        boost::format formatter("function : protocol_module_base::check_message_result "
                                "protocol_module_sessionless::check_parameter() exception : "
                                "error = %s.");
        formatter % ex.what();
        putLogError(17000, formatter.str(), __FILE__, __LINE__ );
    }
    catch (...)
    {
        check_result.flag = false;
        std::cerr << "protocol_module_sessionless::check_parameter() : Unknown exception." << std::endl;
        putLogError(17000, "function : protocol_module_base::check_message_result "
                    "protocol_module_sessionless::check_parameter() : "
                    "Unknown exception.", __FILE__, __LINE__ );
    }

    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("out_function : protocol_module_base::check_message_result "
                                "protocol_module_sessionless::check_parameter("
                                "const std::vector<std::string>& args) : return_value = ("
                                "check_message_result.flag = %d, check_message_result.message = %s).");
        formatter % check_result.flag % check_result.message;
        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/
    return check_result;
}

//! parameter set
//! @param[in] module paramter string list
//! @return	result.flag true is parameter is noproblem.
//! @return result.flag false is paramter is problem.
protocol_module_base::check_message_result protocol_module_sessionless::set_parameter(const std::vector<
        std::string>& args)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("in_function : protocol_module_base::check_message_result "
                                "protocol_module_sessionless::set_parameter("
                                "const std::vector<std::string>& args) : args = %s.");
        std::string argsdump;
        for (std::vector<std::string>::const_iterator it = args.begin(); it != args.end(); ++it)
        {
            argsdump += *it;
        }
        formatter % argsdump;
        putLogDebug(10010, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/
    //set check result flag to true
    check_message_result check_result;
    check_result.flag = true;
    bool forward_checked = false;
    bool sorryuri_checked = false;
    const std::string regex_match_str = "(/([0-9a-zA-Z$\\-_.+!*'(),;:@&=]|%[0-9a-fA-F]{2})*)+";

    typedef std::vector<std::string>::const_iterator vec_str_it;

    //set forwarded flag to true
    forwarded_for = 1;

    try
    {
        vec_str_it it = args.begin();
        vec_str_it it_end = args.end();
        for (; it != it_end; ++it)
        {
            //option string = "-F"
            if (*it == "-F" || *it == "--forwarded-for")
            {
                //set forwarded flag ON
                forward_checked = true;
            }
            //option string  = "-S"
            else if (*it == "-S" || *it == "--sorry-uri")
            {
                //sorryURI flag = OFF
                if (!sorryuri_checked)
                {
                    //next item exist
                    if (++it != it_end)
                    {
                        if (!it->empty() && (it->substr(0, 1) == "-" || it->substr(0, 2) == "--"))
                        {
                            //set check flag false
                            check_result.flag = false;
                            //set check result message
                            check_result.message = "You have to set option value '-S/--sorry-uri'.";
                            putLogError(17000, check_result.message, __FILE__,
                                        __LINE__ );
                            //loop break
                            break;
                        }
                        //next option string's length ＞ 127
                        if (it->size() > MAX_OPTION_SIZE - 1)
                        {
                            std::ostringstream ostr;
                            ostr << "'-S/--sorry-uri' option value '" << *it << "' is too long.";

                            //set check flag false
                            check_result.flag = false;
                            //set check result message
                            check_result.message = ostr.str();
                            putLogError(17000, check_result.message, __FILE__,
                                        __LINE__ );
                            //loop break
                            break;
                        }
                        //next option string's length ≦ 127
                        else
                        {
                            //regex check
                            //正規表現は([^?#]*)(\?([^#]*))?(#(.*))?通り
                            boost::regex uri(regex_match_str);
                            boost::cmatch what;
                            //check OK
                            if (boost::regex_match(it->c_str(), what, uri))
                            {
                                sorryuri_checked = true;
                            }
                            //check NG
                            else
                            {
                                std::ostringstream ostr;
                                ostr << "'-S/--sorry-uri' option value '" << *it << "' is not a valid URI.";

                                //set check flag false
                                check_result.flag = false;
                                //set check result message
                                check_result.message = ostr.str();
                                putLogError(17000, check_result.message, __FILE__,
                                            __LINE__ );

                                break;
                            }
                        }
                    }
                    //next item not exist
                    else
                    {
                        //set check flag false
                        check_result.flag = false;
                        //set check result message
                        check_result.message = "You have to set option value '-S/--sorry-uri'.";
                        putLogError(17000, check_result.message, __FILE__,
                                    __LINE__ );

                        break;
                    }
                }
                //sorryURI flag = ON
                else
                {
                    //set check flag false
                    check_result.flag = false;
                    //set check result message
                    check_result.message = "Cannot set multiple option '-S/--sorry-uri'.";
                    putLogError(17000, check_result.message, __FILE__,
                                __LINE__ );

                    break;
                }
            }
            //others
            else
            {
                //set check flag false
                check_result.flag = false;
                //set check result message
                check_result.message = "Option error.";

                putLogError(17000, check_result.message, __FILE__,
                            __LINE__ );

                break;
            }
        }

        if (check_result.flag == true)
        {
            //forward flag = OFF
            if (!forward_checked)
            {
                forwarded_for = 0;
            }
        }

    }
    catch (const std::exception& ex)
    {
        check_result.flag = false;
        std::cerr << "set parameter exception : result=false, error = " << ex.what() << "." << std::endl;
        boost::format formatter("function : protocol_module_base::check_message_result "
                                "protocol_module_sessionless::set_parameter() exception : "
                                "error = %s.");
        formatter % ex.what();
        putLogError(17000, formatter.str(), __FILE__, __LINE__ );
    }
    catch (...)
    {
        check_result.flag = false;
        std::cerr << "protocol_module_sessionless::set_parameter() : Unknown exception." << std::endl;
        putLogError(17000, "function : protocol_module_base::check_message_result "
                    "protocol_module_sessionless::set_parameter() : "
                    "Unknown exception.", __FILE__, __LINE__ );
    }

    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("out_function : protocol_module_base::check_message_result "
                                "protocol_module_sessionless::set_parameter("
                                "const std::vector<std::string>& args) : return_value = ("
                                "check_message_result.flag = %d, check_message_result.message = %s).");
        formatter % check_result.flag % check_result.message;
        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
    }
    /*-------- DEBUG LOG END--------*/

    return check_result;
}

//! parameter add
//! @param[in] module paramter string list
//! @return	result.flag true is parameter is noproblem.
//! @return result.flag false is paramter is problem.
protocol_module_base::check_message_result protocol_module_sessionless::add_parameter(const std::vector<
        std::string>& args)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("in_function : protocol_module_base::check_message_result "
                                "protocol_module_sessionless::add_parameter("
                                "const std::vector<std::string>& args) : args = %s.");
        std::string argsdump;
        for (std::vector<std::string>::const_iterator it = args.begin(); it != args.end(); ++it)
        {
            argsdump += *it;
        }
        formatter % argsdump;
        putLogDebug(10010, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/
    check_message_result check_result;
    //set check result's flag true
    check_result.flag = true;

    //param list is not empty
    if (!args.empty())
    {
        //set check result's flag false
        check_result.flag = false;
        //set check result's message
        check_result.message = "Cannot add option.";
        putLogError(17000, check_result.message, __FILE__, __LINE__ );
    }

    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("out_function : protocol_module_base::check_message_result "
                                "protocol_module_sessionless::add_parameter("
                                "const std::vector<std::string>& args) : return_value = ("
                                "check_message_result.flag = %d, check_message_result.message = %s).");
        formatter % check_result.flag % check_result.message;
        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
    }
    /*-------- DEBUG LOG --------*/

    return check_result;
}



//! TCP/IP scheduled function registation.
//! @param[in] schedule module TCP/IP scheduled function object type
void protocol_module_sessionless::register_schedule(tcp_schedule_func_type inschedule)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        putLogDebug(10005, "in_function : void protocol_module_sessionless::register_schedule("
                    "tcp_schedule_func_type inschedule).", __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/
    schedule_tcp = inschedule;
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        putLogDebug(10005, "out_function : void protocol_module_sessionless::register_schedule("
                    "tcp_schedule_func_type inschedule).", __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/
}

//! UDP scheduled function registation
//! @param[in] schedule module UDP scheduled funtion object type
void protocol_module_sessionless::register_schedule(udp_schedule_func_type inschedule)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        putLogDebug(10002,
                    "in/out_function : void protocol_module_sessionless::register_schedule(udp_schedule_func_type inschedule).",
                    __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/
}

//! called from session initialzie use in upstream_thread
//! @param[in]	upstream thread id.
//! @param[in]	downstream thread id
//! @return		session use EVENT mode.
protocol_module_base::EVENT_TAG protocol_module_sessionless::handle_session_initialize(
    const boost::thread::id up_thread_id, const boost::thread::id down_thread_id,
    const boost::asio::ip::tcp::endpoint& client_endpoint_tcp,
    const boost::asio::ip::udp::endpoint& client_endpoint_udp)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_session_initialize(const boost::thread::id up_thread_id, "
                                "const boost::thread::id down_thread_id, const boost::asio::ip::tcp::endpoint& client_endpoint_tcp, "
                                "const boost::asio::ip::udp::endpoint& client_endpoint_udp) : "
                                "up_thread_id = %d, down_thread_id = %d, client_endpoint_tcp = [%s]:%d.");
        formatter % up_thread_id % down_thread_id % client_endpoint_tcp.address().to_string() % client_endpoint_tcp.port() ;
        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/
    EVENT_TAG status;
    session_thread_data_sessionless* p_up = NULL;
    session_thread_data_sessionless* p_down = NULL;

    //session thread initialization
    try
    {
        p_up = new session_thread_data_sessionless;
        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == getloglevel())
        {
            boost::format formatter("new : address = &(%d), size = %lu.");
            formatter % static_cast<void*>(p_up) % sizeof(session_thread_data_sessionless);
            putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/
        p_up->thread_id = up_thread_id;
        p_up->thread_division = THREAD_DIVISION_UP_STREAM;
        p_up->pair_thread_id = down_thread_id;
        p_up->accept_end_flag = ACCEPT_END_FLAG_OFF;
        p_up->end_flag = END_FLAG_OFF;
        p_up->sorry_flag = SORRY_FLAG_OFF;
        p_up->sorryserver_switch_flag = SORRYSERVER_SWITCH_FLAG_OFF;
        p_up->realserver_switch_flag = REALSERVER_SWITCH_FLAG_OFF;
        p_up->last_status = INITIALIZE;
        p_up->client_endpoint_tcp = client_endpoint_tcp;

        recive_data recv_data;
        recv_data.recive_buffer = NULL;
        recv_data.recive_buffer1 = NULL;
        recv_data.recive_buffer2 = NULL;
        recv_data.recive_buffer_max_size = 0;
        recv_data.recive_buffer_rest_size = 0;

        p_up->recive_data_map[client_endpoint_tcp] = recv_data;

        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == getloglevel())
        {
            // data dump
            boost::format
            formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                      "handle_session_initialize() : session_thread_data_sessionless(upthread) : "
                      "thread_id = %d, thread_division = %d, "
                      "pair_thread_id = %d, accept_end_flag = %d, end_flag = %d, "
                      "sorry_flag = %d, sorryserver_switch_flag = %d, realserver_switch_flag = %d, last_status = %d, client_endpoint_tcp = [%s]:%d.");
            formatter % p_up->thread_id % p_up->thread_division % p_up->pair_thread_id % p_up->accept_end_flag
            % p_up->end_flag % p_up->sorry_flag % p_up->sorryserver_switch_flag % p_up->realserver_switch_flag
            % p_up->last_status % client_endpoint_tcp.address().to_string() % client_endpoint_tcp.port();
            putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        p_down = new session_thread_data_sessionless;
        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == getloglevel())
        {
            boost::format formatter("new : address = &(%d), size = %lu.");
            formatter % static_cast<void*>(p_down) % sizeof(session_thread_data_sessionless);
            putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/
        p_down->thread_id = down_thread_id;
        p_down->thread_division = THREAD_DIVISION_DOWN_STREAM;
        p_down->pair_thread_id = up_thread_id;
        p_down->accept_end_flag = ACCEPT_END_FLAG_OFF;
        p_down->end_flag = END_FLAG_OFF;
        p_down->sorry_flag = SORRY_FLAG_OFF;
        p_down->sorryserver_switch_flag = SORRYSERVER_SWITCH_FLAG_OFF;
        p_down->realserver_switch_flag = REALSERVER_SWITCH_FLAG_OFF;
        p_down->last_status = INITIALIZE;
        p_down->client_endpoint_tcp = client_endpoint_tcp;
        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == getloglevel())
        {
            // data dump
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_session_initialize() : session_thread_data_sessionless(down_thread_id) : "
                                    "accept_end_flag = %d, end_flag = %d, "
                                    "pair_thread_id = %d, realserver_switch_flag = %d, sorry_flag = %d, "
                                    "thread_division= %d, thread_id = %d, last_status = %d, client_endpoint_tcp = [%s]:%d.");
            formatter % p_down->thread_id % p_down->thread_division % p_down->pair_thread_id % p_down->accept_end_flag
            % p_down->end_flag % p_down->sorry_flag % p_down->sorryserver_switch_flag
            % p_down->realserver_switch_flag % p_down->last_status % client_endpoint_tcp.address().to_string() % client_endpoint_tcp.port();
            putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        boost::mutex::scoped_lock slock(session_thread_data_map_mutex);

        session_thread_data_map[up_thread_id] = p_up;
        session_thread_data_map[down_thread_id] = p_down;

        status = ACCEPT;
    }
    catch (const std::bad_alloc&)
    {
        std::cerr << "handle_session_initialize exception : Could not allocate memory." << std::endl;
        putLogError(17000, "Could not allocate memory.", __FILE__, __LINE__ );
        status = FINALIZE;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "handle_session_initialize exception : error = " << ex.what() << "." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_session_initialize() : exception : error = %s.");
        formatter % ex.what();
        putLogError(17000, formatter.str(), __FILE__, __LINE__ );
        status = FINALIZE;
    }
    catch (...)
    {
        std::cerr << "rotocol_module_sessionless::handle_session_initialize() : Unknown exception." << std::endl;
        putLogError(17000, "function : protocol_module_base::check_message_result "
                    "protocol_module_sessionless::handle_session_initialize() : "
                    "Unknown exception.", __FILE__, __LINE__ );

        status = FINALIZE;
    }

    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_session_initialize(const boost::thread::id up_thread_id, "
                                "const boost::thread::id down_thread_id, const boost::asio::ip::tcp::endpoint& client_endpoint_tcp, "
                                "const boost::asio::ip::udp::endpoint& client_endpoint_udp) : return_value = %d.");
        formatter % status;
        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    return status;
}
//! called from session finalize use in upstream thread.
//! @param[in]	upstream thread id.
//! @param[in]	downstream thread id
//! @return		session use EVENT mode.
protocol_module_base::EVENT_TAG protocol_module_sessionless::handle_session_finalize(
    const boost::thread::id up_thread_id, const boost::thread::id down_thread_id)
{

    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_session_finalize(const boost::thread::id up_thread_id, "
                                "const boost::thread::id down_thread_id): "
                                "up_thread_id = %d, down_thread_id = %d.");
        formatter % up_thread_id % down_thread_id;
        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/
    EVENT_TAG status;
    session_thread_data_sessionless* p_up = NULL;
    session_thread_data_sessionless* p_down = NULL;
    session_thread_data_map_it session_thread_data_it;
    recive_data_map_it recive_data_it;

    //session thread free
    try
    {
        boost::mutex::scoped_lock slock(session_thread_data_map_mutex);

        session_thread_data_it = session_thread_data_map.find(up_thread_id);
        if (session_thread_data_it != session_thread_data_map.end())
        {
            p_up = session_thread_data_it->second;
            if (p_up != NULL)
            {
                recive_data_it = p_up->recive_data_map.find(p_up->client_endpoint_tcp);
                if (recive_data_it != p_up->recive_data_map.end())
                {
                    recive_data& recv_data = recive_data_it->second;
                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        boost::format formatter("delete : address = &(%d).");
                        formatter % static_cast<void*>(recv_data.recive_buffer2);
                        putLogDebug(10005, formatter.str(), __FILE__,
                                    __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                    delete[] recv_data.recive_buffer2;
                    recv_data.recive_buffer2 = NULL;

                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        boost::format formatter("delete : address = &(%d).");
                        formatter % static_cast<void*>(recv_data.recive_buffer1);
                        putLogDebug(10005, formatter.str(), __FILE__,
                                    __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                    delete[] recv_data.recive_buffer1;
                    recv_data.recive_buffer1 = NULL;

                    recv_data.recive_buffer = NULL;
                }
                /*-------- DEBUG LOG --------*/
                if (LOG_LV_DEBUG == getloglevel())
                {
                    boost::format formatter("delete : address = &(%d).");
                    formatter % static_cast<void*>(p_up);
                    putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                }
                /*------DEBUG LOG END------*/
                delete p_up;
            }
            session_thread_data_map.erase(up_thread_id);
        }

        session_thread_data_it = session_thread_data_map.find(down_thread_id);
        if (session_thread_data_it != session_thread_data_map.end())
        {
            p_down = session_thread_data_it->second;
            if (p_down != NULL)
            {
                recive_data_it = p_down->recive_data_map.find(p_down->target_endpoint);
                if (recive_data_it != p_down->recive_data_map.end())
                {
                    recive_data& recv_data = recive_data_it->second;
                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        boost::format formatter("delete : address = &(%d).");
                        formatter % static_cast<void*>(recv_data.recive_buffer2);
                        putLogDebug(10005, formatter.str(), __FILE__,
                                    __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                    delete[] recv_data.recive_buffer1;
                    recv_data.recive_buffer1 = NULL;


                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        boost::format formatter("delete : address = &(%d).");
                        formatter % static_cast<void*>(recv_data.recive_buffer2);
                        putLogDebug(10005, formatter.str(), __FILE__,
                                    __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                    delete[] recv_data.recive_buffer2;
                    recv_data.recive_buffer = NULL;
                }
                /*-------- DEBUG LOG --------*/
                if (LOG_LV_DEBUG == getloglevel())
                {
                    boost::format formatter("delete : address = &(%d).");
                    formatter % static_cast<void*>(p_down);
                    putLogDebug(10005, formatter.str(), __FILE__,
                                __LINE__ );
                }
                /*------DEBUG LOG END------*/
                delete p_down;
            }
            session_thread_data_map.erase(down_thread_id);
        }

        status = STOP;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "handle_session_finalize exception : error = " << ex.what() << "." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_session_finalize() : exception : error = %s.");
        formatter % STOP % ex.what();
        putLogError(17000, formatter.str(), __FILE__, __LINE__ );
        status = STOP;
    }
    catch (...)
    {
        std::cerr << "Unknown exception." << std::endl;
        putLogError(17000, "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                    "handle_session_finalize() : "
                    "Unknown exception.", __FILE__, __LINE__ );
        status = STOP;
    }

    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_session_finalize(const boost::thread::id up_thread_id, "
                                "const boost::thread::id down_thread_id) : return_value = %d.");
        formatter % status;
        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/
    return status;
}

//! called from after session accept.in client socket use in upstream thread.
//! @param[in]	upstream thread id.
//! @return		session use EVENT mode.
protocol_module_base::EVENT_TAG protocol_module_sessionless::handle_accept(const boost::thread::id thread_id)
{

    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_accept(const boost::thread::id thread_id) : thread_id = %d.");
        formatter % thread_id;
        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    EVENT_TAG status;
    session_thread_data_sessionless* session_data = NULL;
    session_thread_data_map_it session_thread_it;

    try
    {
        boost::mutex::scoped_lock slock(session_thread_data_map_mutex);

        session_thread_it = session_thread_data_map.find(thread_id);
        if (session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL)
        {
            putLogError(17000, "Invalid thread id.", __FILE__, __LINE__ );
            throw -1;
        }

        session_data = session_thread_it->second;

        //set accept end flag ON
        session_data->accept_end_flag = ACCEPT_END_FLAG_ON;
        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == getloglevel())
        {
            putLogDebug(10005, "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                        "handle_accept(const boost::thread::id thread_id) : ACCEPT_END_FLAG_ON.", __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        //sorry flag on
        if (session_data->sorry_flag == SORRY_FLAG_ON)
        {
            //set return status
            status = SORRYSERVER_SELECT;
        }
        //sorry flag on
        else
        {
            //set return status
            status = REALSERVER_SELECT;
        }
    }
    catch (int e)
    {
        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == getloglevel())
        {
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_accept() : catch exception e = %d.");
            formatter % e;
            putLogError(17000, formatter.str(), __FILE__, __LINE__ );
        }
        status = FINALIZE;
        /*------DEBUG LOG END------*/
    }
    catch (const std::exception& ex)
    {
        std::cerr << "handle_accept exception : error = " << ex.what() << "." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_accept() : exception : error = %s.");
        formatter % FINALIZE % ex.what();
        putLogError(17000, formatter.str(), __FILE__, __LINE__ );

        status = FINALIZE;
    }
    catch (...)
    {
        std::cerr << "Unknown exception." << std::endl;
        putLogError(17000, "function : protocol_module_base::check_message_result "
                    "protocol_module_sessionless::handle_accept() : "
                    "Unknown exception.", __FILE__, __LINE__ );
        status = FINALIZE;
    }

    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_accept(const boost::thread::id thread_id) : return_value = %d.");
        formatter % status;
        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/
    return status;
}

//! called from after session recv in client socket. use in upstream thread.
//! @param[in]	upstream thread id
//! @param[in]	recive buffer refarence.
//! @param[in]	recive length
//! @return		session use EVENT mode.
protocol_module_base::EVENT_TAG protocol_module_sessionless::handle_client_recv(const boost::thread::id thread_id,
        const boost::array<char, MAX_BUFFER_SIZE>& recvbuffer, const size_t recvlen)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_client_recv(const boost::thread::id thread_id, "
                                "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                "const size_t recvlen) : thread_id = %d, recvbuffer = %s, recvlen = %d.");
        formatter % thread_id % recvbuffer.data() % recvlen;
        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    EVENT_TAG status;
    size_t data_remain_start = 0;
    size_t data_remain_size = 0;
    size_t request_data_remain_size = 0;
    size_t header_offset = 0;
    size_t header_offset_len = 0;
    size_t content_length_header_offset = 0;
    size_t content_length_header_len = 0;
    size_t content_len_value = 0;
    size_t pos = 0;
    const size_t cr_lf_len = strlen("\r\n\r\n");
    std::string str_value;
    const std::string http_header = "";
    const std::string content_header = "Content-Length";
    session_thread_data_sessionless* session_data = NULL;
    char* buffer1 = NULL;
    char* buffer2 = NULL;
    bool bret = false;
    CHECK_RESULT_TAG check_result;
    session_thread_data_map_it session_thread_it;
    recive_data_map_it recive_data_it;

    //parameter check
    if (recvlen > recvbuffer.size())
    {
        std::cerr << "handle_client_recv() : Data size bigger than buffer size." << std::endl;
        putLogError(17000, "Data size bigger than buffer size.", __FILE__,
                    __LINE__ );
        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == getloglevel())
        {
            boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_client_recv(const boost::thread::id thread_id, "
                                    "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                    "const size_t recvlen) : return_value = %d.");
            formatter % FINALIZE;
            putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/
        return FINALIZE;
    }

    try
    {
        {
            boost::mutex::scoped_lock slock(session_thread_data_map_mutex);

            session_thread_it = session_thread_data_map.find(thread_id);
            if (session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL)
            {
                putLogError(17000, "Invalid thread id.", __FILE__, __LINE__ );
                throw -1;
            }

            session_data = session_thread_it->second;
        }

        //end flag on
        if (session_data->end_flag == END_FLAG_ON)
        {
            status =  CLIENT_RECV;
        }
        //end flag off
        else
        {
            recive_data_it = session_data->recive_data_map.find(session_data->client_endpoint_tcp);
            if (recive_data_it == session_data->recive_data_map.end())
            {
                putLogError(17000, "Invalid endpoint.", __FILE__, __LINE__ );
                throw -1;
            }

            recive_data& recv_data = recive_data_it->second;

            send_status_it it = recv_data.send_status_list.begin();
            send_status_it it_end = recv_data.send_status_list.end();

            //status list check
            it = std::find_if(it, it_end, data_send_ok());
            if (it != it_end)
            {
                putLogError(17000, "Sending data is not correct.", __FILE__, __LINE__ );
                throw -1;
            }

            //status list check
            it = recv_data.send_status_list.begin();
            it = std::adjacent_find(it, it_end, data_send_repeated());
            if (it != it_end)
            {
                putLogError(17000, "Sending data is not correct.", __FILE__, __LINE__ );
                throw -1;
            }

            /*-------- DEBUG LOG --------*/
            if (LOG_LV_DEBUG == getloglevel())
            {
                putLogDebug(10005, "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                            "handle_client_recv() : step1--send status list loop start.", __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/

            it = recv_data.send_status_list.begin();
            //get original status info
            while (it != it_end)
            {
                //item status is SEND_END
                if (it->status == SEND_END)
                {
                    //erase from list
                    recv_data.send_status_list.erase(it++);
                    continue;
                }
                //item status is SEND_CONTINUE
                else if (it->status == SEND_CONTINUE)
                {
                    it->send_offset += it->send_end_size;
                    data_remain_start = it->send_offset;
                    //繰り返し処理を終了する
                    break;
                }
                //item status is SEND_NG
                else
                {
                    data_remain_start = it->send_offset;
                    data_remain_size = it->unsend_size;
                    break;
                }

                ++it;
            }
            /*-------- DEBUG LOG --------*/
            if (LOG_LV_DEBUG == getloglevel())
            {
                putLogDebug(10005, "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                            "handle_client_recv() : step1--send status list loop end.", __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/

            /*-------- DEBUG LOG --------*/
            if (LOG_LV_DEBUG == getloglevel())
            {
                putLogDebug(10005, "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                            "handle_client_recv() : step2--data buffer size process start.", __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/

            //recive buffer process
            //buffer rest size　＜　request size
            if (recv_data.recive_buffer_rest_size < recvlen)
            {
                //buffer max size　＜　remain size　＋　request size
                //buffer is need reallcate
                if (recv_data.recive_buffer_max_size < data_remain_size + recvlen)
                {
                    //first recive
                    if (recv_data.recive_buffer == NULL)
                    {
                        //recive_buffer1's memery allocate and initialization
                        recv_data.recive_buffer1 = new char[MAX_BUFFER_SIZE];
                        /*-------- DEBUG LOG --------*/
                        if (LOG_LV_DEBUG == getloglevel())
                        {
                            boost::format formatter("new : address = &(%d), size = %lu.");
                            formatter % static_cast<void*>(recv_data.recive_buffer1) % (MAX_BUFFER_SIZE);
                            putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*-------- DEBUG LOG END--------*/
                        memset(recv_data.recive_buffer1, 0, MAX_BUFFER_SIZE);
                        //recive_buffer2's memery allocate and initialization
                        recv_data.recive_buffer2 = new char[MAX_BUFFER_SIZE];
                        /*-------- DEBUG LOG --------*/
                        if (LOG_LV_DEBUG == getloglevel())
                        {
                            boost::format formatter("new : address = &(%d), size = %lu.");
                            formatter % static_cast<void*>(recv_data.recive_buffer2) % (MAX_BUFFER_SIZE);
                            putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*-------- DEBUG LOG END--------*/
                        memset(recv_data.recive_buffer2, 0, MAX_BUFFER_SIZE);
                        //set uisng buffer recive_buffer1 for defalut
                        recv_data.recive_buffer = recv_data.recive_buffer1;
                        //set max buffer size MAX_BUFFER_SIZE
                        recv_data.recive_buffer_max_size = MAX_BUFFER_SIZE;
                    }
                    //not first recive
                    else
                    {
                        //recive_buffer1's memery allocate and initialization
                        buffer1 = new char[data_remain_size + recvlen];
                        /*-------- DEBUG LOG --------*/
                        if (LOG_LV_DEBUG == getloglevel())
                        {
                            boost::format formatter("new : address = &(%d), size = %lu.");
                            formatter % static_cast<void*>(buffer1) % (data_remain_size + recvlen);
                            putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        memset(buffer1, 0, data_remain_size + recvlen);
                        //recive_buffer2's memery allocate and initialization
                        buffer2 = new char[data_remain_size + recvlen];
                        /*-------- DEBUG LOG --------*/
                        if (LOG_LV_DEBUG == getloglevel())
                        {
                            boost::format formatter("new : address = &(%d), size = %lu.");
                            formatter % static_cast<void*>(buffer2) % (data_remain_size + recvlen);
                            putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        memset(buffer2, 0, data_remain_size + recvlen);

                        /*-------- DEBUG LOG --------*/
                        if (LOG_LV_DEBUG == getloglevel())
                        {
                            std::string datadump;
                            dump_memory(recv_data.recive_buffer + data_remain_start, data_remain_size, datadump);
                            boost::format formatter(
                                "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_client_recv() : before memcpy (data dump) : "
                                "data begin = %d, data_size = %d, data = %s.");
                            formatter % data_remain_start % (data_remain_size) % datadump;
                            putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        //copy data from old buffer to new buffer
                        memcpy(buffer1, recv_data.recive_buffer + data_remain_start, data_remain_size);
                        /*-------- DEBUG LOG --------*/
                        if (LOG_LV_DEBUG == getloglevel())
                        {
                            std::string datadump;
                            dump_memory(buffer1, data_remain_size, datadump);
                            boost::format formatter(
                                "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_client_recv() : after memcpy (data dump) : "
                                "data begin = 0, data_size = %d, data = %s.");
                            formatter % (data_remain_size) % datadump;
                            putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/

                        /*-------- DEBUG LOG --------*/
                        if (LOG_LV_DEBUG == getloglevel())
                        {
                            std::string datadump;
                            dump_memory(recvbuffer.data(), recvlen, datadump);
                            boost::format formatter(
                                "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_client_recv() : before memcpy (data dump) : "
                                "data begin = %0, data_size = %d, data = %s.");
                            formatter % recvlen % datadump;
                            putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        memcpy(buffer1 + data_remain_size, recvbuffer.data(), recvlen);
                        /*-------- DEBUG LOG --------*/
                        if (LOG_LV_DEBUG == getloglevel())
                        {
                            std::string datadump;
                            dump_memory(buffer1 + data_remain_size, recvlen, datadump);
                            boost::format formatter(
                                "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_client_recv() : after memcpy (data dump) :  "
                                "data begin = %d, data_size = %d, data = %s.");
                            formatter % data_remain_size % recvlen % datadump;
                            putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        //free old buffer1 and old buffer2
                        if (recv_data.recive_buffer1 != NULL)
                        {
                            /*-------- DEBUG LOG --------*/
                            if (LOG_LV_DEBUG == getloglevel())
                            {
                                boost::format formatter("delete : address = &(%d).");
                                formatter % static_cast<void*>(recv_data.recive_buffer1);
                                putLogDebug(10005, formatter.str(), __FILE__,
                                            __LINE__ );
                            }
                            /*------DEBUG LOG END------*/
                            delete[] recv_data.recive_buffer1;
                            recv_data.recive_buffer1 = NULL;
                        }

                        if (recv_data.recive_buffer2 != NULL)
                        {
                            /*-------- DEBUG LOG --------*/
                            if (LOG_LV_DEBUG == getloglevel())
                            {
                                boost::format formatter("delete : address = &(%d).");
                                formatter % static_cast<void*>(recv_data.recive_buffer2);
                                putLogDebug(10005, formatter.str(), __FILE__,
                                            __LINE__ );
                            }
                            /*------DEBUG LOG END------*/
                            delete[] recv_data.recive_buffer2;
                            recv_data.recive_buffer2 = NULL;
                        }

                        //set new buffer pointer
                        recv_data.recive_buffer1 = buffer1;
                        recv_data.recive_buffer2 = buffer2;
                        recv_data.recive_buffer = recv_data.recive_buffer1;
                        //set new buffer's max size
                        recv_data.recive_buffer_max_size = data_remain_size + recvlen;
                    }
                }
                //buffer's max size　≧　remain data size　＋　requst size
                //buffer isn't need reallcate, but switch
                else
                {
                    //pointer valid check
                    if (recv_data.recive_buffer1 == NULL || recv_data.recive_buffer2 == NULL)
                    {
                        putLogError(17000, "Invalid pointer.", __FILE__, __LINE__ );
                        throw -1;
                    }
                    //using buffer is buffer1
                    if (recv_data.recive_buffer == recv_data.recive_buffer1)
                    {
                        //buffer2 initialization
                        memset(recv_data.recive_buffer2, 0, recv_data.recive_buffer_max_size);
                        /*-------- DEBUG LOG --------*/
                        if (LOG_LV_DEBUG == getloglevel())
                        {
                            std::string datadump;
                            dump_memory(recv_data.recive_buffer + data_remain_start, data_remain_size, datadump);
                            boost::format formatter(
                                "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_client_recv() : before memcpy (data dump) : "
                                "data begin = %d, data_size = %d, data = %s.");
                            formatter % data_remain_start % data_remain_size  % datadump;
                            putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        //copy data from buffer1 to buffer2
                        memcpy(recv_data.recive_buffer2, recv_data.recive_buffer + data_remain_start, data_remain_size);
                        /*-------- DEBUG LOG --------*/
                        if (LOG_LV_DEBUG == getloglevel())
                        {
                            std::string datadump;
                            dump_memory(recv_data.recive_buffer2, recvlen, datadump);
                            boost::format formatter(
                                "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_client_recv() : after memcpy (data dump) :  "
                                "data begin = 0, data_size = %d, data = %s.");
                            formatter % recvlen % datadump;
                            putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        memcpy(recv_data.recive_buffer2 + data_remain_size, recvbuffer.data(), recvlen);
                        /*-------- DEBUG LOG --------*/
                        if (LOG_LV_DEBUG == getloglevel())
                        {
                            std::string datadump;
                            dump_memory(recv_data.recive_buffer2 + data_remain_size, recvlen, datadump);
                            boost::format formatter(
                                "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_client_recv() : after memcpy (data dump) : "
                                "data begin = %d, data_size = %d, data = %s.");
                            formatter % data_remain_size % recvlen % datadump;
                            putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        //set buffer2 for using buffer
                        recv_data.recive_buffer = recv_data.recive_buffer2;
                    }
                    //using buffer is buffer2
                    else
                    {
                        //buffer1 initializtion
                        memset(recv_data.recive_buffer1, 0, recv_data.recive_buffer_max_size);
                        /*-------- DEBUG LOG --------*/
                        if (LOG_LV_DEBUG == getloglevel())
                        {
                            std::string datadump;
                            dump_memory(recv_data.recive_buffer + data_remain_start, data_remain_size, datadump);
                            boost::format formatter(
                                "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_client_recv() : before memcpy (data dump) :  "
                                "data begin = %d, data_size = %d, data = %s.");
                            formatter % data_remain_start % data_remain_size % datadump;
                            putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        //copy data from buffer2 to buffer1
                        memcpy(recv_data.recive_buffer1, recv_data.recive_buffer + data_remain_start, data_remain_size);
                        /*-------- DEBUG LOG --------*/
                        if (LOG_LV_DEBUG == getloglevel())
                        {
                            std::string datadump;
                            dump_memory(recv_data.recive_buffer1, data_remain_size, datadump);
                            boost::format formatter(
                                "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_client_recv() : after memcpy (data dump) :  "
                                "data begin = 0, data_size = %d, data = %s.");
                            formatter % data_remain_size % datadump;
                            putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        /*-------- DEBUG LOG --------*/
                        if (LOG_LV_DEBUG == getloglevel())
                        {
                            std::string datadump;
                            dump_memory(recvbuffer.data(), recvlen, datadump);
                            boost::format formatter(
                                "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_client_recv() : before memcpy (data dump) :  "
                                "data begin = 0, data_size = %d, data = %s.");
                            formatter % recvlen % datadump;
                            putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        memcpy(recv_data.recive_buffer1 + data_remain_size, recvbuffer.data(), recvlen);
                        /*-------- DEBUG LOG --------*/
                        if (LOG_LV_DEBUG == getloglevel())
                        {
                            std::string datadump;
                            dump_memory(recv_data.recive_buffer1 + data_remain_size, recvlen, datadump);
                            boost::format formatter(
                                "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_client_recv() : after memcpy (data dump) :  "
                                "data begin = %d, data_size = %d, data = %s.");
                            formatter % data_remain_size % recvlen % datadump;
                            putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        //set buffer1 for using buffer
                        recv_data.recive_buffer = recv_data.recive_buffer1;
                    }
                }

                //set buffer's rest size
                recv_data.recive_buffer_rest_size = recv_data.recive_buffer_max_size - data_remain_size - recvlen;

                //remain_size recalc
                data_remain_size += recvlen;

                send_status_it it_begin = recv_data.send_status_list.begin();
                send_status_it it_end = recv_data.send_status_list.end();

                //offset recalc
                for (; it_begin != it_end; ++it_begin)
                {
                    it->send_offset -= data_remain_start;
                }
            }
            //buffer's rest size　≧　request size
            else
            {
                //pointer valid check
                if (recv_data.recive_buffer == NULL)
                {
                    putLogError(17000, "Invalid pointer.", __FILE__, __LINE__ );
                    throw -1;
                }
                /*-------- DEBUG LOG --------*/
                if (LOG_LV_DEBUG == getloglevel())
                {
                    std::string datadump;
                    dump_memory(recvbuffer.data(), recvlen, datadump);
                    boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                            "handle_client_recv() : before memcpy (data dump) : "
                                            "data begin = 0, data_size = %d, data = %s.");
                    formatter % recvlen % datadump;
                    putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                }
                /*------DEBUG LOG END------*/
                //copy data from paramenter to using buffer
                memcpy(recv_data.recive_buffer + recv_data.recive_buffer_max_size - recv_data.recive_buffer_rest_size,
                       recvbuffer.data(), recvlen);
                /*-------- DEBUG LOG --------*/
                if (LOG_LV_DEBUG == getloglevel())
                {
                    std::string datadump;
                    dump_memory(recv_data.recive_buffer + recv_data.recive_buffer_max_size - recv_data.recive_buffer_rest_size,
                                recvlen, datadump);
                    boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                            "handle_client_recv() : after memcpy (data dump) : "
                                            "data begin = %d, data_size = %d, data = %s.");
                    formatter % (recv_data.recive_buffer_max_size - recv_data.recive_buffer_rest_size )
                    % recvlen % datadump;
                    putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                }
                /*------DEBUG LOG END------*/
                //buffer's rest size recalc
                recv_data.recive_buffer_rest_size -= recvlen;
                //remain data size recalc
                data_remain_size += recvlen;
            }
            /*-------- DEBUG LOG --------*/
            if (LOG_LV_DEBUG == getloglevel())
            {
                putLogDebug(10005, "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                            "handle_client_recv() : step2--data buffer size process end.", __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/

            it = recv_data.send_status_list.begin();
            it_end = recv_data.send_status_list.end();

            //set request rest size
            request_data_remain_size = recvlen;
            /*-------- DEBUG LOG --------*/
            if (LOG_LV_DEBUG == getloglevel())
            {
                putLogDebug(10005, "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                            "handle_client_recv() : step3--send status list loop start.", __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/
            //original status process
            for (; it != it_end; ++it)
            {
                //status is SEND_CONTINUE
                if (it->status == SEND_CONTINUE)
                {
                    //send rest size　>　request size
                    if (it->send_rest_size > request_data_remain_size)
                    {
                        //send possible size recalc
                        it->send_possible_size = request_data_remain_size;
                        //send rest size recalc
                        it->send_rest_size -= request_data_remain_size;
                        //send end size recalc
                        it->send_end_size = 0;
                        //request size recalc
                        request_data_remain_size = 0;
                    }
                    //send rest size　<=　request size
                    else
                    {
                        //send possible size recalc
                        it->send_possible_size = it->send_rest_size;
                        //send rest size recalc
                        request_data_remain_size -= it->send_rest_size;
                        //send end size recalc
                        it->send_end_size = 0;
                        //request size recalc
                        it->send_rest_size = 0;
                    }
                    //set edit_division flag off
                    it->edit_division = EDIT_DIVISION_NO_EDIT;
                    //set status SEND_OK
                    it->status = SEND_OK;
                }
                //status is SEND_NG
                else if (it->status == SEND_NG)
                {
                    //check http method
                    check_result = check_http_method(recv_data.recive_buffer + it->send_offset, data_remain_size);
                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                "handle_client_recv() : call check_http_method : "
                                                "return_value = %d.");
                        formatter % check_result;
                        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                    //check http method result is OK
                    if (check_result == CHECK_OK)
                    {
                        //check http version
                        check_result = check_http_version(recv_data.recive_buffer + it->send_offset, data_remain_size);
                        /*-------- DEBUG LOG --------*/
                        if (LOG_LV_DEBUG == getloglevel())
                        {
                            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                    "handle_client_recv() : call check_http_version : "
                                                    "return_value = %d.");
                            formatter % check_result;
                            putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                    }
                    //check method and version result is OK
                    if (check_result == CHECK_OK)
                    {
                        //serch http header
                        bret = find_http_header(recv_data.recive_buffer + it->send_offset, data_remain_size, http_header,
                                                header_offset, header_offset_len);
                        /*-------- DEBUG LOG --------*/
                        if (LOG_LV_DEBUG == getloglevel())
                        {
                            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                    "handle_client_recv() : call find_http_header : "
                                                    "return_value = %d.");
                            formatter % static_cast<int>(bret);;
                            putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        //serch http header result is OK
                        if (bret)
                        {
                            //serch Content_Length header
                            bret = find_http_header(recv_data.recive_buffer + it->send_offset, data_remain_size,
                                                    content_header, content_length_header_offset, content_length_header_len);
                            /*-------- DEBUG LOG --------*/
                            if (LOG_LV_DEBUG == getloglevel())
                            {
                                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                        "handle_client_recv() : call find_http_header : "
                                                        "return_value = %d.");
                                formatter % formatter % static_cast<int>(bret);;
                                putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                            }
                            /*------DEBUG LOG END------*/
                            //serch Content_Length result is OK
                            if (bret)
                            {
                                //Get Content_Length header's numeric value
                                for (pos = 0; recv_data.recive_buffer[it->send_offset + content_length_header_offset + pos] != ':' && pos
                                        < content_length_header_len; ++pos)
                                    ;
                                if (pos == content_length_header_len)
                                {
                                    throw std::string("Contend_Lenght field's value is invalid.");
                                }

                                ++pos;

                                str_value.assign(recv_data.recive_buffer + it->send_offset + content_length_header_offset + pos,
                                                 content_length_header_len - pos);

                                size_t pos_end = str_value.find_last_of('\r');
                                if (pos_end != std::string::npos)
                                {
                                    str_value = str_value.erase(pos_end);
                                }

                                for (pos = 0; !isgraph(str_value[pos]) && str_value[pos] != '\0'; ++pos);

                                str_value = str_value.substr(pos);

                                try
                                {
                                    content_len_value = boost::lexical_cast<size_t>(str_value.c_str());
                                }
                                catch (const boost::bad_lexical_cast& ex)
                                {
                                    throw std::string("Content_Length field's value is invalid.");
                                }

                                //send_rest_size recalc
                                //set whole http header's length + Content_Length's value
                                it->send_rest_size = header_offset + header_offset_len + cr_lf_len + content_len_value;
                            }
                            //serch Content_Length result is NG
                            else
                            {
                                //send_rest_size recalc
                                //set whole http header's length
                                it->send_rest_size = header_offset + header_offset_len + cr_lf_len;
                            }
                            //set edit_division flag on
                            it->edit_division = EDIT_DIVISION_EDIT;
                            /*-------- DEBUG LOG --------*/
                            if (LOG_LV_DEBUG == getloglevel())
                            {
                                putLogDebug(10005, "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                            "handle_client_recv() : EDIT_DIVISION_EDIT.", __FILE__, __LINE__ );
                            }
                            /*------DEBUG LOG END------*/
                        }
                        //serch http header result is NG
                        else
                        {
                            //unsend_size recalc
                            it->unsend_size += request_data_remain_size;
                            //request data rest size recalc
                            request_data_remain_size = 0;
                            break;
                        }
                    }
                    //check method and version result is NG
                    else if (check_result == CHECK_NG)
                    {
                        //set edit_division flag off
                        it->edit_division = EDIT_DIVISION_NO_EDIT;
                        //send_rest_size recalc
                        it->send_rest_size = it->unsend_size + request_data_remain_size;
                    }
                    //check method and version result is CHECK_INPOSSIBLE
                    else
                    {
                        //unsend_size recalc
                        it->unsend_size += request_data_remain_size;
                        //request data rest size recalc
                        request_data_remain_size = 0;
                        break;
                    }

                    //recalc fields value according to send_rest_size and request rest size
                    if (it->send_rest_size > it->unsend_size + request_data_remain_size)
                    {
                        it->send_possible_size = it->unsend_size + request_data_remain_size;
                        it->send_rest_size -= (it->unsend_size + request_data_remain_size);
                        it->send_end_size = 0;
                        it->unsend_size = 0;
                        request_data_remain_size = 0;
                    }
                    else
                    {
                        it->send_possible_size = it->send_rest_size;
                        request_data_remain_size = it->unsend_size + request_data_remain_size - it->send_rest_size;
                        it->send_end_size = 0;
                        it->unsend_size = 0;
                        it->send_rest_size = 0;
                    }

                    //change status from SEND_NG to SEND_OK
                    it->status = SEND_OK;
                }
                //no request rest data to process
                if (request_data_remain_size <= 0)
                {
                    break;
                }
            }
            /*-------- DEBUG LOG --------*/
            if (LOG_LV_DEBUG == getloglevel())
            {
                putLogDebug(10005, "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                            "handle_client_recv() : step3--send status list loop end.", __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/
            /*-------- DEBUG LOG --------*/
            if (LOG_LV_DEBUG == getloglevel())
            {
                putLogDebug(10005, "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                            "handle_client_recv() : step4--new send status create procession start.", __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/
            //there are still rest data need to prosess
            //new status created and add to status list
            while (request_data_remain_size > 0)
            {
                //new status created
                send_status new_send_state;

                new_send_state.edit_division = EDIT_DIVISION_NO_EDIT;
                new_send_state.send_end_size = 0;
                new_send_state.send_offset = 0;
                new_send_state.send_possible_size = 0;
                new_send_state.unsend_size = 0;
                new_send_state.send_rest_size = 0;
                //status initialize to SEND_NG
                new_send_state.status = SEND_NG;
                //add new status to status_list
                recv_data.send_status_list.push_back(new_send_state);
                std::list<send_status>::reverse_iterator new_send_it = recv_data.send_status_list.rbegin();
                //cacl offset
                new_send_it->send_offset = recv_data.recive_buffer_max_size - recv_data.recive_buffer_rest_size
                                           - request_data_remain_size;

                //check http method
                check_result = check_http_method(recv_data.recive_buffer + new_send_it->send_offset,
                                                 request_data_remain_size);
                /*-------- DEBUG LOG --------*/
                if (LOG_LV_DEBUG == getloglevel())
                {
                    boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                            "handle_client_recv() : call check_http_method : "
                                            "return_value = %d.");
                    formatter % check_result;
                    putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                }
                /*------DEBUG LOG END------*/
                //check http method resilt is OK
                if (check_result == CHECK_OK)
                {
                    //check http version
                    check_result = check_http_version(recv_data.recive_buffer + new_send_it->send_offset,
                                                      request_data_remain_size);
                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                "handle_client_recv() : call check_http_version : "
                                                "return_value = %d.");
                        formatter % check_result;
                        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                }
                //check http method and version resilt is OK
                if (check_result == CHECK_OK)
                {
                    //search whole http header, get whole http header's offset and length
                    bret = find_http_header(recv_data.recive_buffer + new_send_it->send_offset, request_data_remain_size,
                                            http_header, header_offset, header_offset_len);
                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                "handle_client_recv() : call find_http_header : "
                                                "return_value = %d.");
                        formatter % check_result;
                        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                    //searched whole http header
                    if (bret)
                    {
                        //search ContentLength http header, get ContentLength header's offset and length
                        bret = find_http_header(recv_data.recive_buffer + new_send_it->send_offset,
                                                request_data_remain_size, content_header, content_length_header_offset, content_length_header_len);
                        /*-------- DEBUG LOG --------*/
                        if (LOG_LV_DEBUG == getloglevel())
                        {
                            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                    "handle_client_recv() : call find_http_header : "
                                                    "return_value = %d.");
                            formatter % static_cast<int>(bret);
                            putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/

                        //searched ContentLength http header
                        if (bret)
                        {
                            //Get Content_Length header's numeric value
                            for (pos = 0;
                                    recv_data.recive_buffer[new_send_it->send_offset + content_length_header_offset + pos] != ':'
                                    && pos < content_length_header_len;
                                    ++pos);
                            if (pos == content_length_header_len)
                            {
                                throw std::string("Contend_Lenght field's value is invalid.");
                            }

                            ++pos;

                            str_value.assign(recv_data.recive_buffer + new_send_it->send_offset + content_length_header_offset + pos,
                                             content_length_header_len - pos);

                            size_t pos_end = str_value.find_last_of('\r');
                            if (pos_end != std::string::npos)
                            {
                                str_value = str_value.erase(pos_end);
                            }

                            for (pos = 0; !isgraph(str_value[pos]) && str_value[pos] != '\0'; ++pos);

                            str_value = str_value.substr(pos);
                            try
                            {
                                content_len_value = boost::lexical_cast<size_t>(str_value.c_str());
                            }
                            catch (const boost::bad_lexical_cast& ex)
                            {
                                throw std::string("Contend_Lenght field's value is invalid.");
                            }
                            //send_rest_size recalc
                            //set whole http header's  + whole http header's length + Content_Length's value
                            new_send_it->send_rest_size = header_offset + header_offset_len + cr_lf_len + content_len_value;
                        }
                        //not searched ContentLength http header
                        else
                        {
                            //send_rest_size recalc
                            //set whole http header's  + whole http header's length
                            new_send_it->send_rest_size = header_offset + header_offset_len + cr_lf_len;

                        }
                        //set edit_division flag on
                        new_send_it->edit_division = EDIT_DIVISION_EDIT;
                        /*-------- DEBUG LOG --------*/
                        if (LOG_LV_DEBUG == getloglevel())
                        {
                            putLogDebug(10005, "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                        "handle_client_recv() : EDIT_DIVISION_EDIT.", __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                    }
                    //not searched whole http header
                    else
                    {
                        new_send_it->unsend_size = request_data_remain_size;
                        request_data_remain_size = 0;
                        break;
                    }
                }
                //check http method or version result is NG
                else if (check_result == CHECK_NG)
                {
                    new_send_it->edit_division = EDIT_DIVISION_NO_EDIT;
                    new_send_it->send_rest_size = request_data_remain_size;
                }

                //check http method or version resilt is NG
                else
                {
                    new_send_it->unsend_size = request_data_remain_size;
                    request_data_remain_size = 0;
                    break;
                }

                //recalc fields value according to send_rest_size and request rest size
                if (new_send_it->send_rest_size > request_data_remain_size)
                {
                    new_send_it->send_possible_size = request_data_remain_size;
                    new_send_it->send_rest_size -= request_data_remain_size;
                    new_send_it->send_end_size = 0;
                    request_data_remain_size = 0;
                }
                else
                {
                    new_send_it->send_possible_size = new_send_it->send_rest_size;
                    request_data_remain_size -= new_send_it->send_rest_size;
                    new_send_it->send_end_size = 0;
                    new_send_it->send_rest_size = 0;
                }

                //change status from SEND_NG to SEND_OK
                new_send_it->status = SEND_OK;
            }

            /*-------- DEBUG LOG --------*/
            if (LOG_LV_DEBUG == getloglevel())
            {
                putLogDebug(10005, "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                            "handle_client_recv() : step4--new send status create procession end.", __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/
            //search for send_possbile item in status list
            send_status_it it_find = find_if(recv_data.send_status_list.begin(), recv_data.send_status_list.end(),
                                             data_send_possible());
            //send_possbile item is exist
            if (it_find != recv_data.send_status_list.end())
            {
                //sorry flag is on
                if (session_data->sorry_flag == SORRY_FLAG_ON)
                {
                    status = SORRYSERVER_CONNECT;
                }
                //sorry flag is off
                else
                {
                    status = REALSERVER_CONNECT;
                }
            }
            //send_possbile item is not exist
            else
            {
                status = CLIENT_RECV;
            }
        }
    }
    catch (int e)
    {
        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == getloglevel())
        {
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_client_recv() : catch exception e = %d.");
            formatter % e;
            putLogError(17000, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/
        status = FINALIZE;
    }
    catch (const std::string& ex)
    {
        std::cerr << "handle_client_recv exception : " << ex << std::endl;
        putLogError(17000, ex.c_str(), __FILE__, __LINE__ );
        status = FINALIZE;
    }
    catch (const std::bad_alloc&)
    {
        std::cerr << "handle_client_recv exception : Could not allocate memory." << std::endl;
        putLogError(17000, "Could not allocate memory.", __FILE__, __LINE__ );
        status = FINALIZE;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "handle_client_recv exception : error =" << ex.what() << "." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_client_recv() : exception : error = %s.");
        formatter % ex.what();
        putLogError(17000, formatter.str(), __FILE__, __LINE__ );

        status = FINALIZE;
    }
    catch (...)
    {
        std::cerr << "protocol_module_sessionless::handle_client_recv() : Unknown exception." << std::endl;
        putLogError(17000, "function : protocol_module_base::check_message_result "
                    "protocol_module_sessionless::handle_client_recv() : "
                    "Unknown exception.", __FILE__, __LINE__ );
        status = FINALIZE;
    }

    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_client_recv(const boost::thread::id thread_id, "
                                "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                "const size_t recvlen) : return_value = %d.");
        formatter % status;
        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    return status;
}

//! called from after realserver select.use in upstream thread.
//! @param[in]	upstream thread id
//! @param[out]	realserver TCP endpoint
//! @return		session use EVENT mode.
protocol_module_base::EVENT_TAG protocol_module_sessionless::handle_realserver_select(
    const boost::thread::id thread_id, boost::asio::ip::tcp::endpoint & rs_endpoint)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_realserver_select(const boost::thread::id thread_id, "
                                "boost::asio::ip::tcp::endpoint & rs_endpoint) : "
                                "thread_id = %d, rs_endpoint = [%s]:%d.");
        formatter % thread_id % rs_endpoint.address().to_string() % rs_endpoint.port();
        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/
    EVENT_TAG status;
    boost::asio::ip::tcp::endpoint tmp_endpoint;
    session_thread_data_sessionless* session_data = NULL;
    session_thread_data_sessionless* session_down_thread_data = NULL;
    session_thread_data_map_it session_thread_it;

    if (schedule_tcp.empty())
    {
        std::cerr << "handle_realserver_select() : Schedule_tcp function is empty." << std::endl;
        putLogError(17000, "Schedule_tcp function is empty.", __FILE__, __LINE__ );
        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == getloglevel())
        {
            boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_realserver_select(const boost::thread::id thread_id, "
                                    "boost::asio::ip::tcp::endpoint & rs_endpoint)"
                                    " : return_value = %d.");
            formatter % FINALIZE;
            putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/
        return FINALIZE;
    }

    try
    {
        {
            boost::mutex::scoped_lock slock(session_thread_data_map_mutex);

            session_thread_it = session_thread_data_map.find(thread_id);
            if (session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL)
            {
                putLogError(17000, "Invalid thread id.", __FILE__, __LINE__ );
                throw -1;
            }

            session_data = session_thread_it->second;
        }

        //call schedule_module's schedule function, get realserver endpoint
        schedule_tcp(thread_id, rs_list_begin, rs_list_end, rs_list_next, rs_endpoint);
        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == getloglevel())
        {
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_realserver_select() : call schedule_tcp : "
                                    "rs_endpoint = [%s]:%d.");
            formatter % rs_endpoint.address().to_string() % rs_endpoint.port();
            putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        //endpoint decide
        if (rs_endpoint != tmp_endpoint)
        {
            recive_data recv_data;
            recv_data.recive_buffer = NULL;
            recv_data.recive_buffer1 = NULL;
            recv_data.recive_buffer2 = NULL;
            recv_data.recive_buffer_max_size = 0;
            recv_data.recive_buffer_rest_size = 0;
            std::map<boost::asio::ip::tcp::endpoint, recive_data> recive_map;
            recive_map[rs_endpoint] = recv_data;

            //thread_id's memory is not allocated
            session_thread_it = session_thread_data_map.find(session_data->pair_thread_id);
            if (session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL)
            {
                session_down_thread_data = new session_thread_data_sessionless;
                /*-------- DEBUG LOG --------*/
                if (LOG_LV_DEBUG == getloglevel())
                {
                    boost::format formatter("new : address = &(%d), size = %lu.");
                    formatter % static_cast<void*>(session_down_thread_data) % sizeof(session_thread_data_sessionless);
                    putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                }
                /*------DEBUG LOG END------*/
                session_down_thread_data->thread_id = session_data->pair_thread_id;
                session_down_thread_data->thread_division = THREAD_DIVISION_DOWN_STREAM;
                session_down_thread_data->pair_thread_id = thread_id;
                session_down_thread_data->accept_end_flag = ACCEPT_END_FLAG_OFF;
                session_down_thread_data->end_flag = END_FLAG_OFF;
                session_down_thread_data->sorry_flag = SORRY_FLAG_OFF;
                session_down_thread_data->sorryserver_switch_flag = SORRYSERVER_SWITCH_FLAG_OFF;
                session_down_thread_data->realserver_switch_flag = REALSERVER_SWITCH_FLAG_OFF;
                session_down_thread_data->recive_data_map = recive_map;

                session_thread_data_map[session_data->pair_thread_id] = session_down_thread_data;
                /*-------- DEBUG LOG --------*/
                if (LOG_LV_DEBUG == getloglevel())
                {
                    // data dump
                    boost::format
                    formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                              "handle_realserver_select() : session_thread_data_sessionless(down_thread) : "
                              "thread_id = %d, thread_division = %d, "
                              "pair_thread_id = %d, accept_end_flag = %d, end_flag = %d, "
                              "sorry_flag = %d, sorryserver_switch_flag = %d, realserver_switch_flag = %d, client_endpoint_tcp = [%s]:%d.");
                    formatter % session_down_thread_data->thread_id % session_down_thread_data->thread_division % session_down_thread_data->pair_thread_id % session_down_thread_data->accept_end_flag
                    % session_down_thread_data->end_flag % session_down_thread_data->sorry_flag % session_down_thread_data->sorryserver_switch_flag % session_down_thread_data->realserver_switch_flag
                    % session_down_thread_data->client_endpoint_tcp.address().to_string() % session_down_thread_data->client_endpoint_tcp.port();
                    putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                }
                /*------DEBUG LOG END------*/
            }
            else
            {
                //thread_id's memory is allocated, check endpoint's memory
                session_down_thread_data = session_thread_it->second;
                session_down_thread_data->thread_id = session_data->pair_thread_id;
                session_down_thread_data->thread_division = THREAD_DIVISION_DOWN_STREAM;
                session_down_thread_data->pair_thread_id = thread_id;
                session_down_thread_data->accept_end_flag = ACCEPT_END_FLAG_OFF;
                session_down_thread_data->end_flag = END_FLAG_OFF;
                session_down_thread_data->sorry_flag = SORRY_FLAG_OFF;
                session_down_thread_data->sorryserver_switch_flag = SORRYSERVER_SWITCH_FLAG_OFF;
                session_down_thread_data->realserver_switch_flag = REALSERVER_SWITCH_FLAG_OFF;

                /*-------- DEBUG LOG --------*/
                if (LOG_LV_DEBUG == getloglevel())
                {
                    // data dump
                    boost::format
                    formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                              "handle_realserver_select() : session_thread_data_sessionless(down_thread) : "
                              "thread_id = %d, thread_division = %d, "
                              "pair_thread_id = %d, accept_end_flag = %d, end_flag = %d, "
                              "sorry_flag = %d, sorryserver_switch_flag = %d, realserver_switch_flag = %d, client_endpoint_tcp = [%s]:%d.");
                    formatter % session_down_thread_data->thread_id % session_down_thread_data->thread_division % session_down_thread_data->pair_thread_id % session_down_thread_data->accept_end_flag
                    % session_down_thread_data->end_flag % session_down_thread_data->sorry_flag % session_down_thread_data->sorryserver_switch_flag % session_down_thread_data->realserver_switch_flag
                    % session_down_thread_data->client_endpoint_tcp.address().to_string() % session_down_thread_data->client_endpoint_tcp.port();
                    putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                }
                /*------DEBUG LOG END------*/

                if (session_down_thread_data->recive_data_map.find(rs_endpoint)
                        != session_down_thread_data->recive_data_map.end())
                {
                    //
                    recive_data & recv_data = session_down_thread_data->recive_data_map[rs_endpoint];
                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        boost::format formatter("delete : address = &(%d).");
                        formatter % static_cast<void*>(recv_data.recive_buffer1);
                        putLogDebug(10005, formatter.str(), __FILE__,
                                    __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                    delete[] recv_data.recive_buffer1;
                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        boost::format formatter("delete : address = &(%d).");
                        formatter % static_cast<void*>(recv_data.recive_buffer2);
                        putLogDebug(10005, formatter.str(), __FILE__,
                                    __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                    delete[] recv_data.recive_buffer2;
                    recv_data.recive_buffer1 = NULL;
                    recv_data.recive_buffer2 = NULL;
                    recv_data.recive_buffer = NULL;
                    recv_data.recive_buffer_max_size = 0;
                    recv_data.recive_buffer_rest_size = 0;
                    recv_data.send_status_list.clear();
                }
                else
                {
                    session_down_thread_data->recive_data_map[rs_endpoint] = recv_data;
                }

            }

            status = CLIENT_RECV;

        }
        else
        {
            //set end flag on
            session_data->end_flag = END_FLAG_ON;
            /*-------- DEBUG LOG --------*/
            if (LOG_LV_DEBUG == getloglevel())
            {
                putLogDebug(10005, "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                            "handle_realserver_select() : END_FLAG_ON.", __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/

            status = CLIENT_DISCONNECT;
        }
    }
    catch (int e)
    {
        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == getloglevel())
        {
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_realserver_select() : catch exception e = %d.");
            formatter % e;
            putLogError(17000, formatter.str(), __FILE__, __LINE__ );
        }
        status = FINALIZE;
        /*------DEBUG LOG END------*/
    }
    catch (const std::exception& ex)
    {
        std::cerr << "handle_realserver_select exception : error = " << ex.what() << "." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_realserver_select() : exception : error = %s.");
        formatter % ex.what();
        putLogError(17000, formatter.str(), __FILE__, __LINE__ );
        status = FINALIZE;
    }
    catch (...)
    {
        std::cerr << "protocol_module_sessionless::handle_realserver_select() : Unknown exception." << std::endl;
        putLogError(17000, "function : protocol_module_base::EVENT_TAG "
                    "protocol_module_sessionless::handle_realserver_select() : "
                    "Unknown exception.", __FILE__, __LINE__ );
        status = FINALIZE;
    }

    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_realserver_select(const boost::thread::id thread_id, "
                                "boost::asio::ip::tcp::endpoint & rs_endpoint)"
                                " : return_value = %d.");
        formatter % status;
        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    return status;
}

//! called from after realserver select
//! @param[in]	upstream thread id
//! @param[out]	realserver UDP endpoint
//! @param[out]	sendbudffer reference
//! @param[out]	send data length
//! @return		session use EVENT mode.
protocol_module_base::EVENT_TAG protocol_module_sessionless::handle_realserver_select(
    const boost::thread::id thread_id, boost::asio::ip::udp::endpoint& rs_endpoint, boost::array<char,
    MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("in/out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_realserver_select(const boost::thread::id thread_id, "
                                "boost::asio::ip::udp::endpoint& rs_endpoint, boost::array<char,MAX_BUFFER_SIZE>& sendbuffer, "
                                "size_t& datalen) : "
                                "return_value = %d.");
        formatter % STOP;
        putLogDebug(10000, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/
    return STOP;
}
//! called from after realserver connect
//! @param[in]	upstream thread id
//! @param[out]	sendbuffer reference
//! @param[out]	send data length
//! @return		session use EVENT mode.
protocol_module_base::EVENT_TAG protocol_module_sessionless::handle_realserver_connect(
    const boost::thread::id thread_id, boost::array<char, MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_realserver_connect(const boost::thread::id up_thread_id, "
                                "boost::array<char, MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen) : "
                                "thread_id = %d, sendbuffer = %s, datalen = %d.");
        formatter % thread_id % sendbuffer.data() % datalen;
        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    EVENT_TAG status;
    bool ret = false;
    size_t header_offset = 0;
    size_t header_offset_len = 0;
    size_t send_buffer_remian_size = 0;
    size_t copy_size = 0;
    const int send_buffer_end_size = sendbuffer.max_size();
    const std::string http_header = "";
    const std::string str_forword_for = "X-Forwarded-For";
    session_thread_data_sessionless* session_data = NULL;
    session_thread_data_map_it session_thread_it;
    recive_data_map_it recive_data_it;

    try
    {
        {
            boost::mutex::scoped_lock slock(session_thread_data_map_mutex);

            //thread id check
            session_thread_it = session_thread_data_map.find(thread_id);
            if (session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL)
            {
                putLogError(17000, "Invalid thread id.", __FILE__, __LINE__ );
                throw -1;
            }

            session_data = session_thread_it->second;
        }

        //endpoint check
        recive_data_it = session_data->recive_data_map.find(session_data->target_endpoint);
        if (recive_data_it == session_data->recive_data_map.end())
        {
            putLogError(17000, "Invalid endpoint.", __FILE__, __LINE__ );
            throw -1;
        }

        //recive_buffer pointer check
        recive_data& recv_data = recive_data_it->second;
        if (recv_data.recive_buffer == NULL)
        {
            putLogError(17000, "Invalid pointer.", __FILE__, __LINE__ );
            throw -1;
        }

        //send list check
        send_status_it it = recv_data.send_status_list.begin();
        send_status_it it_end = recv_data.send_status_list.end();
        it = find_if(it, it_end, data_send_possible());
        if (it == it_end)
        {
            putLogError(17000, "Sending possible data is not existed.", __FILE__, __LINE__ );
            throw -1;
        }

        //send buffer rest size initliazation
        send_buffer_remian_size = send_buffer_end_size;

        //edit_division flag on
        if (it->edit_division == EDIT_DIVISION_EDIT  && forwarded_for == FORWARDED_FOR_ON)
        {
            //edit list is empty
            if (it->edit_data_list.empty())
            {
                //edit data create
                edit_data edata;
                edata.data_size = 0;
                edata.insert_posission = 0;
                edata.replace_size = 0;
                //search X-Forwarded-For header
                ret = find_http_header(recv_data.recive_buffer + it->send_offset, it->send_possible_size,
                                       str_forword_for, header_offset, header_offset_len);
                /*-------- DEBUG LOG --------*/
                if (LOG_LV_DEBUG == getloglevel())
                {
                    boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                            "handle_realserver_connect() : call find_http_header : "
                                            "return_value = %d.");
                    formatter % static_cast<int>(ret);
                    putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                }
                /*------DEBUG LOG END------*/
                //serch http header result is OK
                if (ret)
                {
                    //edit X-Forwarded-For header, set it to edata.data
                    edata.data.assign(recv_data.recive_buffer + it->send_offset + header_offset, header_offset_len);
                    edata.data += ", ";
                    edata.data += session_data->client_endpoint_tcp.address().to_string();
                    //save new X-Forwarded-For header offset
                    edata.insert_posission = header_offset;
                    //save new X-Forwarded-For header lenght
                    edata.data_size = edata.data.size();
                    //save old X-Forwarded-For header lenght
                    edata.replace_size = header_offset_len;
                }
                //serch http header result is NG
                else
                {
                    //search whole http header, get whole http header's offset and length
                    ret = find_http_header(recv_data.recive_buffer + it->send_offset, it->send_possible_size, "",
                                           header_offset, header_offset_len);
                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                "handle_realserver_connect() : call find_http_header : "
                                                "return_value = %d.");
                        formatter % static_cast<int>(ret);
                        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                    if (!ret)
                    {
                        putLogError(17000, "find_http_header function failure.", __FILE__, __LINE__ );
                        /*-------- DEBUG LOG --------*/
                        if (LOG_LV_DEBUG == getloglevel())
                        {
                            boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                    "handle_realserver_connect(const boost::thread::id thread_id, "
                                                    "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                                    "const size_t recvlen) : return_value = %d.");
                            formatter % FINALIZE;
                            putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        return FINALIZE;
                    }
                    //create X-Forwarded-For header, set it to edata.data
                    edata.data = str_forword_for;
                    edata.data += ": ";
                    edata.data += session_data->client_endpoint_tcp.address().to_string();
                    edata.data += "\r\n";
                    //save new X-Forwarded-For header offset
                    edata.insert_posission = header_offset;
                    //save new X-Forwarded-For header lenght
                    edata.data_size = edata.data.size();
                    //save old X-Forwarded-For header lenght
                    edata.replace_size = 0;
                }

                //add to edit_data_list
                it->edit_data_list.push_back(edata);
            }

            /*-------- DEBUG LOG --------*/
            if (LOG_LV_DEBUG == getloglevel())
            {
                putLogDebug(10005, "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                            "handle_realserver_connect() : Copy data loop start.", __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/
            while (true)
            {
                //edit_data_list is empty
                if (it->edit_data_list.empty())
                {
                    //set edit_division flag on
                    it->edit_division = EDIT_DIVISION_NO_EDIT;

                    if (send_buffer_remian_size > 0 && it->send_possible_size > 0)
                    {
                        //send_buffer_remian_size is larger
                        if (send_buffer_remian_size >= it->send_possible_size)
                        {
                            copy_size = it->send_possible_size;
                            /*-------- DEBUG LOG --------*/
                            if (LOG_LV_DEBUG == getloglevel())
                            {
                                std::string datadump;
                                dump_memory(recv_data.recive_buffer + it->send_offset + it->send_end_size,
                                            it->send_possible_size, datadump);
                                boost::format formatter(
                                    "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_realserver_connect() : before memcpy (data dump) : "
                                    "data begin = %d, data_size = %d, data = %s.");
                                formatter % (it->send_offset + it->send_end_size)
                                % copy_size % datadump;
                                putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                            }
                            /*------DEBUG LOG END------*/
                            //copy data from recive_buffer to sendbuffer by sending_possible size
                            memcpy(sendbuffer.data() + send_buffer_end_size - send_buffer_remian_size,
                                   recv_data.recive_buffer + it->send_offset + it->send_end_size,
                                   it->send_possible_size);
                            /*-------- DEBUG LOG --------*/
                            if (LOG_LV_DEBUG == getloglevel())
                            {
                                std::string datadump;
                                dump_memory(sendbuffer.data() + send_buffer_end_size - send_buffer_remian_size,
                                            it->send_possible_size, datadump);

                                boost::format formatter(
                                    "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_realserver_connect() : after memcpy (data dump) : "
                                    "data begin = %d, data_size = %d, data = %s.");
                                formatter % (send_buffer_end_size - send_buffer_remian_size)
                                % copy_size % datadump;
                                putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                            }
                            /*------DEBUG LOG END------*/

                            it->send_end_size += copy_size;
                            it->send_possible_size = 0;
                            send_buffer_remian_size -= copy_size;
                        }
                        //send_possible_size is larger
                        else
                        {
                            copy_size = send_buffer_remian_size;
                            /*-------- DEBUG LOG --------*/
                            if (LOG_LV_DEBUG == getloglevel())
                            {
                                std::string datadump;
                                dump_memory(recv_data.recive_buffer + it->send_offset + it->send_end_size,
                                            copy_size, datadump);

                                boost::format formatter(
                                    "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_realserver_connect() : before memcpy (data dump) : "
                                    "data begin = %d, data_size = %d, data = %s.");
                                formatter % (it->send_offset + it->send_end_size)
                                % copy_size % datadump;
                                putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                            }
                            /*------DEBUG LOG END------*/
                            //copy data from recive_buffer to sendbuffer by send buffer rest size
                            memcpy(sendbuffer.data() + send_buffer_end_size - send_buffer_remian_size,
                                   recv_data.recive_buffer + it->send_offset + it->send_end_size, copy_size);
                            /*-------- DEBUG LOG --------*/
                            if (LOG_LV_DEBUG == getloglevel())
                            {
                                std::string datadump;
                                dump_memory(sendbuffer.data() + send_buffer_end_size - send_buffer_remian_size,
                                            copy_size, datadump);

                                boost::format formatter(
                                    "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_realserver_connect() : after memcpy (data dump) : "
                                    "data begin = %d, data_size = %d, data = %s.");
                                formatter % (send_buffer_end_size - send_buffer_remian_size)
                                % copy_size % datadump;
                                putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                            }
                            /*------DEBUG LOG END------*/
                            it->send_end_size += copy_size;
                            it->send_possible_size -= copy_size;
                            send_buffer_remian_size = 0;
                        }
                    }
                    break;
                }
                //edit_data_list is not empty
                else
                {
                    //search item while insert_posission is mininum
                    std::list<edit_data>::iterator edit_min = std::min_element(it->edit_data_list.begin(),
                            it->edit_data_list.end());
                    //send_buffer_remian_size is larger than data that before X-Forwarded-For
                    if (send_buffer_remian_size >= edit_min->insert_posission - it->send_end_size)
                    {
                        //copy data before X-Forwarded-For
                        copy_size = edit_min->insert_posission - it->send_end_size;
                        /*-------- DEBUG LOG --------*/
                        if (LOG_LV_DEBUG == getloglevel())
                        {
                            std::string datadump;
                            dump_memory(recv_data.recive_buffer + it->send_offset + it->send_end_size,
                                        copy_size, datadump);

                            boost::format formatter(
                                "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_realserver_connect() : before memcpy (data dump) : "
                                "data begin = %d, data_size = %d, data = %s.");
                            formatter % (it->send_offset + it->send_end_size)
                            % copy_size % datadump;
                            putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        memcpy(sendbuffer.data() + send_buffer_end_size - send_buffer_remian_size,
                               recv_data.recive_buffer + it->send_offset + it->send_end_size, copy_size);
                        /*-------- DEBUG LOG --------*/
                        if (LOG_LV_DEBUG == getloglevel())
                        {
                            std::string datadump;
                            dump_memory(sendbuffer.data() + send_buffer_end_size - send_buffer_remian_size,
                                        copy_size, datadump);
                            boost::format formatter(
                                "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_realserver_connect() : after memcpy (data dump) : "
                                "data begin = %d, data_size = %d, data = %s.");
                            formatter % (send_buffer_end_size - send_buffer_remian_size)
                            % copy_size % datadump;
                            putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        it->send_end_size += copy_size;
                        it->send_possible_size -= copy_size;
                        send_buffer_remian_size -= copy_size;

                        //there is remain buffer for copy X-Forwarded-For
                        if (send_buffer_remian_size >= edit_min->data_size)
                        {
                            /*-------- DEBUG LOG --------*/
                            if (LOG_LV_DEBUG == getloglevel())
                            {
                                std::string datadump;
                                dump_memory(edit_min->data.c_str(),
                                            edit_min->data_size, datadump);

                                boost::format formatter(
                                    "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_realserver_connect() : before memcpy (data dump) : "
                                    "data begin = 0, data_size = %d, data = %s.");
                                formatter % edit_min->data_size % datadump;
                                putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                            }
                            /*------DEBUG LOG END------*/
                            //copy  X-Forwarded-For
                            memcpy(sendbuffer.data() + send_buffer_end_size - send_buffer_remian_size,
                                   edit_min->data.c_str(), edit_min->data_size);
                            /*-------- DEBUG LOG --------*/
                            if (LOG_LV_DEBUG == getloglevel())
                            {
                                boost::format formatter("%02x ");
                                std::string datadump;
                                dump_memory(sendbuffer.data() + send_buffer_end_size - send_buffer_remian_size,
                                            edit_min->data_size, datadump);
                                formatter.parse(
                                    "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_realserver_connect() : after memcpy (data dump) : "
                                    "data begin = %d, data_size = %d, data = %s.");
                                formatter % (send_buffer_end_size - send_buffer_remian_size)
                                % edit_min->data_size % datadump;
                                putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                            }
                            /*------DEBUG LOG END------*/
                            it->send_end_size += edit_min->replace_size;
                            it->send_possible_size -= edit_min->replace_size;
                            send_buffer_remian_size -= edit_min->data_size;
                            it->edit_data_list.erase(edit_min);
                        }
                        //
                        else
                        {
                            break;
                        }
                    }
                    //data that before X-Forwarded-For is larger than send_buffer_remian_size
                    else
                    {
                        copy_size = send_buffer_remian_size;
                        /*-------- DEBUG LOG --------*/
                        if (LOG_LV_DEBUG == getloglevel())
                        {
                            std::string datadump;
                            dump_memory(recv_data.recive_buffer + it->send_offset + it->send_end_size,
                                        copy_size, datadump);

                            boost::format formatter(
                                "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_realserver_connect() : before memcpy (data dump) : "
                                "data begin = %d, data_size = %d, data = %s.");
                            formatter % (it->send_offset + it->send_end_size)
                            % copy_size % datadump;
                            putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        //copy data as large as possible
                        memcpy(sendbuffer.data() + send_buffer_end_size - send_buffer_remian_size,
                               recv_data.recive_buffer + it->send_offset + it->send_end_size, copy_size);
                        /*-------- DEBUG LOG --------*/
                        if (LOG_LV_DEBUG == getloglevel())
                        {
                            std::string datadump;
                            dump_memory(sendbuffer.data() + send_buffer_end_size - send_buffer_remian_size,
                                        copy_size, datadump);

                            boost::format formatter(
                                "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_realserver_connect() : after memcpy (data dump) : "
                                "data begin = %d, data_size = %d, data = %s.");
                            formatter % (send_buffer_end_size - send_buffer_remian_size)
                            % copy_size % datadump;
                            putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/

                        it->send_end_size += copy_size;
                        it->send_possible_size -= copy_size;
                        send_buffer_remian_size -= copy_size;
                        break;
                    }
                }
            }
            /*-------- DEBUG LOG --------*/
            if (LOG_LV_DEBUG == getloglevel())
            {
                putLogDebug(10005, "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                            "handle_realserver_connect() : Copy data loop end.", __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/
        }
        //edit_division flag is off
        else
        {
            //copy data as large as possbile
            //send_possible_size is larger
            if (send_buffer_remian_size >= it->send_possible_size)
            {
                copy_size = it->send_possible_size;
                /*-------- DEBUG LOG --------*/
                if (LOG_LV_DEBUG == getloglevel())
                {
                    std::string datadump;
                    dump_memory(recv_data.recive_buffer + it->send_offset,
                                copy_size, datadump);
                    boost::format formatter(
                        "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                        "handle_realserver_connect() : before memcpy (data dump) : "
                        "data begin = %d, data_size = %d, data = %s.");
                    formatter % it->send_offset
                    % copy_size % datadump;
                    putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                }
                /*------DEBUG LOG END------*/
                //copy date by send_possible size
                memcpy(sendbuffer.data(), recv_data.recive_buffer + it->send_offset, copy_size);
                /*-------- DEBUG LOG --------*/
                if (LOG_LV_DEBUG == getloglevel())
                {
                    std::string datadump;
                    dump_memory(sendbuffer.data(), copy_size, datadump);
                    boost::format formatter(
                        "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                        "handle_realserver_connect() : after memcpy (data dump) : "
                        "data begin = 0, data_size = %d, data = %s.");
                    formatter % copy_size % datadump;
                    putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                }
                /*------DEBUG LOG END------*/
                it->send_end_size = copy_size;
                it->send_possible_size = 0;
                send_buffer_remian_size -= copy_size;
            }
            //buffer rest size is larger
            else
            {
                /*-------- DEBUG LOG --------*/
                if (LOG_LV_DEBUG == getloglevel())
                {
                    std::string datadump;
                    dump_memory(recv_data.recive_buffer + it->send_offset, copy_size, datadump);

                    boost::format formatter(
                        "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                        "handle_realserver_connect() : before memcpy (data dump) : "
                        "data begin = %d, data_size = %d, data = %s.");
                    formatter % it->send_offset
                    % send_buffer_remian_size % datadump;
                    putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                }
                /*------DEBUG LOG END------*/
                //copy date by buffer rest size
                memcpy(sendbuffer.data(), recv_data.recive_buffer + it->send_offset, send_buffer_remian_size);
                /*-------- DEBUG LOG --------*/
                if (LOG_LV_DEBUG == getloglevel())
                {
                    std::string datadump;
                    dump_memory(sendbuffer.data(), copy_size, datadump);
                    boost::format formatter(
                        "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                        "handle_realserver_connect() : after memcpy (data dump) : "
                        "data begin = 0, data_size = %d, data = %s.");
                    formatter % send_buffer_remian_size % datadump;
                    putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                }
                /*------DEBUG LOG END------*/
                it->send_end_size = send_buffer_remian_size;
                it->send_possible_size -= send_buffer_remian_size;
                send_buffer_remian_size = 0;
            }
        }

        //set copyed date lenght
        datalen = send_buffer_end_size - send_buffer_remian_size;
        status = REALSERVER_SEND;
    }
    catch (int e)
    {
        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == getloglevel())
        {
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_realserver_connect() : catch exception e = %d.");
            formatter % e;
            putLogError(17000, formatter.str(), __FILE__, __LINE__ );
        }
        status = FINALIZE;
        /*------DEBUG LOG END------*/
    }
    catch (const std::exception& ex)
    {
        std::cerr << "handle_realserver_connect exception : error = " << ex.what() << "." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_realserver_connect() : exception : error = %s.");
        formatter % ex.what();
        putLogError(17000, formatter.str(), __FILE__, __LINE__ );
        status = FINALIZE;
    }
    catch (...)
    {
        std::cerr << "protocol_module_sessionless::handle_realserver_connect() : Unknown exception." << std::endl;
        putLogError(17000, "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                    "handle_realserver_connect() : Unknown exception.", __FILE__, __LINE__ );
        status = FINALIZE;
    }

    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_realserver_connect(const boost::thread::id thread_id, "
                                "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                "const size_t recvlen) : return_value = %d.");
        formatter % status;
        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    return status;
}

//! called from after realserver connection fail
//! @param[in]	upstream thread id
//! @param[in]	fail realserver endpoint reference
//! @return		session use EVENT mode.
protocol_module_base::EVENT_TAG protocol_module_sessionless::handle_realserver_connection_fail(
    const boost::thread::id thread_id, const boost::asio::ip::tcp::endpoint & rs_endpoint)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_realserver_connection_fail(const boost::thread::id thread_id, "
                                "const boost::asio::ip::tcp::endpoint & rs_endpoint) : "
                                "thread_id = %d, rs_endpoint = [%s]:%d.");
        formatter % thread_id % rs_endpoint.address().to_string() % rs_endpoint.port();
        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    EVENT_TAG status;
    session_thread_data_sessionless* session_data = NULL;
    session_thread_data_map_it session_thread_it;

    try
    {
        boost::mutex::scoped_lock slock(session_thread_data_map_mutex);

        session_thread_it = session_thread_data_map.find(thread_id);
        if (session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL)
        {
            putLogError(17000, "Invalid thread id.", __FILE__, __LINE__ );
            throw -1;
        }

        session_data = session_thread_it->second;

        //終了フラグをON
        session_data->end_flag = END_FLAG_ON;
        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == getloglevel())
        {
            putLogDebug(10005, "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                        "handle_realserver_connection_fail() : END_FLAG_ON.", __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/
        //遷移先ステータスを設定する
        status = CLIENT_DISCONNECT;
    }
    catch (int e)
    {
        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == getloglevel())
        {
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_realserver_connection_fail() : catch exception e = %d.");
            formatter % e;
            putLogError(17000, formatter.str(), __FILE__, __LINE__ );
        }
        status = FINALIZE;
        /*------DEBUG LOG END------*/
    }
    catch (std::exception& ex)
    {
        std::cerr << "handle_realserver_connection_fail exception : error = " << ex.what() << "." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_realserver_connection_fail() : exception : result = %d, error = %s.");
        formatter % FINALIZE % ex.what();
        putLogError(17000, formatter.str(), __FILE__, __LINE__ );
        status = FINALIZE;
    }
    catch (...)
    {
        std::cerr << "Unknown exception." << std::endl;
        putLogError(17000, "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                    "handle_realserver_connection_fail() : exception : result = %d.", __FILE__, __LINE__ );
        status = FINALIZE;
    }

    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_realserver_connection_fail(const boost::thread::id thread_id, "
                                "const boost::asio::ip::tcp::endpoint & rs_endpoint) : return_value = %d.");
        formatter % status;
        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/
    return status;
}
//! called from after realserver send.
//! @param[in]	upstream thread id
//! @return		session use EVENT mode.
protocol_module_base::EVENT_TAG protocol_module_sessionless::handle_realserver_send(
    const boost::thread::id thread_id)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_realserver_send(const boost::thread::id thread_id) : "
                                "thread_id = %d.");
        formatter % thread_id;
        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/
    EVENT_TAG status;
    session_thread_data_sessionless* session_data = NULL;
    session_thread_data_map_it session_thread_it;
    recive_data_map_it recive_data_it;

    try
    {
        {
            boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);

            //thread_id check
            session_thread_it = session_thread_data_map.find(thread_id);
            if (session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL)
            {
                putLogError(17000, "Invalid thread id.", __FILE__, __LINE__ );
                throw -1;
            }

            session_data = session_thread_it->second;
        }

        //endpoint check
        recive_data_it = session_data->recive_data_map.find(session_data->client_endpoint_tcp);
        if (recive_data_it == session_data->recive_data_map.end())
        {
            putLogError(17000, "Invalid endpoint.", __FILE__, __LINE__ );
            throw -1;
        }

        recive_data& recv_data = recive_data_it->second;

        send_status_it it = recv_data.send_status_list.begin();
        send_status_it it_end = recv_data.send_status_list.end();

        //status list check
        it = std::adjacent_find(it, it_end, data_send_list_incorrect());
        if (it != it_end)
        {
            putLogError(17000, "Sending possible data is invalid.", __FILE__, __LINE__ );
            throw -1;
        }

        //status list check
        it = recv_data.send_status_list.begin();
        it = std::find_if(it, it_end, data_send_ok());
        if (it == it_end)
        {
            putLogError(17000, "Sending possible data is not existed.", __FILE__, __LINE__ );
            throw -1;
        }

        //sending possible data is exist
        if (it->send_possible_size > 0)
        {
            //status remain SEND_OK
            it->status = SEND_OK;
            //offset recalc
            it->send_offset += it->send_end_size;

            //insert_posission recalc
            for (std::list<edit_data>::iterator list_it = it->edit_data_list.begin(); list_it
                    != it->edit_data_list.end(); ++list_it)
            {
                list_it->insert_posission -= it->send_end_size;
            }

            //send_end_size recalc
            it->send_end_size = 0;
        }
        //sending possible data is not exist
        else
        {
            //can recive from clent continue
            if (it->send_rest_size > 0)
            {
                //change status from SEND_OK to SEND_CONTINUE
                it->status = SEND_CONTINUE;
            }
            //can not recive from clent continue
            else
            {
                //change status from SEND_OK to SEND_END
                it->status = SEND_END;
            }
        }

        it = recv_data.send_status_list.begin();
        it = find_if(it, it_end, data_send_ok());
        //send_ok item is exist
        if (it != it_end)
        {
            status = REALSERVER_CONNECT;
        }
        //send_ok item is exist
        else
        {
            status = CLIENT_RECV;
        }

    }
    catch (int e)
    {
        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == getloglevel())
        {
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_realserver_send() : catch exception e = %d.");
            formatter % e;
            putLogError(17000, formatter.str(), __FILE__, __LINE__ );
        }
        status = FINALIZE;
        /*------DEBUG LOG END------*/
    }
    catch (const std::exception& ex)
    {
        std::cerr << "handle_realserver_send exception : error = " << ex.what() << "." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_realserver_send() : exception : error = %s.");
        formatter % ex.what();
        putLogError(17000, formatter.str(), __FILE__, __LINE__ );

        status = FINALIZE;
    }
    catch (...)
    {
        std::cerr << "protocol_module_sessionless::handle_realserver_send() : Unknown exception." << std::endl;
        putLogError(17000, "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                    "handle_realserver_send() : exception : Unknown exception.", __FILE__, __LINE__ );
        status = FINALIZE;
    }

    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_realserver_send(const boost::thread::id thread_id) : return_value = %d.");
        formatter % status;
        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    return status;
}

//! called from after sorryserver select
//! @param[in]	upstream thread id
//! @param[in]	sorryserver endppiont reference
//! @return		session use EVENT mode.
protocol_module_base::EVENT_TAG protocol_module_sessionless::handle_sorryserver_select(
    const boost::thread::id thread_id, boost::asio::ip::tcp::endpoint& sorry_endpoint)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_sorryserver_select(const boost::thread::id thread_id, "
                                "boost::asio::ip::tcp::endpoint& sorry_endpoint) : "
                                "thread_id = %d, sorry_endpoint = [%s]:%d.");
        formatter % thread_id % sorry_endpoint.address().to_string() % sorry_endpoint.port();
        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/
    EVENT_TAG status;
    boost::asio::ip::tcp::endpoint client_endpoint;

    session_thread_data_sessionless* session_data = NULL;
    session_thread_data_sessionless* session_down_thread_data = NULL;
    session_thread_data_map_it session_thread_it;
    recive_data_map_it recive_data_it;

    try
    {
        boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);

        session_thread_it = session_thread_data_map.find(thread_id);
        if (session_thread_it == session_thread_data_map.end() || session_thread_it->second== NULL)
        {
            putLogError(17000, "Invalid thread id.", __FILE__, __LINE__ );
            throw -1;
        }

        session_data = session_thread_it->second;
        //送信先endpointにendpointを設定する
        session_data->target_endpoint = sorry_endpoint;

        recive_data recv_data;
        recv_data.recive_buffer = NULL;
        recv_data.recive_buffer1 = NULL;
        recv_data.recive_buffer2 = NULL;
        recv_data.recive_buffer_max_size = 0;
        recv_data.recive_buffer_rest_size = 0;
        std::map<boost::asio::ip::tcp::endpoint, recive_data> recive_map;
        recive_map[sorry_endpoint] = recv_data;
        //allcate memory for down thread's reciving buffer
        session_thread_it = session_thread_data_map.find(session_data->pair_thread_id);
        if (session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL)
        {
            session_down_thread_data = new session_thread_data_sessionless;
            /*-------- DEBUG LOG --------*/
            if (LOG_LV_DEBUG == getloglevel())
            {
                boost::format formatter("new : address = &(%d), size = %lu.");
                formatter % static_cast<void*>(session_down_thread_data) % sizeof(session_thread_data_sessionless);
                putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/
            session_down_thread_data->thread_id = session_data->pair_thread_id;
            session_down_thread_data->thread_division = THREAD_DIVISION_DOWN_STREAM;
            session_down_thread_data->pair_thread_id = thread_id;
            session_down_thread_data->accept_end_flag = ACCEPT_END_FLAG_OFF;
            session_down_thread_data->end_flag = END_FLAG_OFF;
            session_down_thread_data->sorry_flag = SORRY_FLAG_OFF;
            session_down_thread_data->sorryserver_switch_flag = SORRYSERVER_SWITCH_FLAG_OFF;
            session_down_thread_data->realserver_switch_flag = REALSERVER_SWITCH_FLAG_OFF;
            session_down_thread_data->target_endpoint = sorry_endpoint;
            session_down_thread_data->client_endpoint_tcp = session_data->client_endpoint_tcp;
            session_down_thread_data->recive_data_map = recive_map;

            /*-------- DEBUG LOG --------*/
            if (LOG_LV_DEBUG == getloglevel())
            {
                // data dump
                boost::format
                formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                          "handle_sorryserver_select() : session_thread_data_sessionless(down_thread) : "
                          "thread_id = %d, thread_division = %d, "
                          "pair_thread_id = %d, accept_end_flag = %d, end_flag = %d, "
                          "sorry_flag = %d, sorryserver_switch_flag = %d, realserver_switch_flag = %d, target_endpoint = [%s]:%d, client_endpoint_tcp = [%s]:%d.");
                formatter % session_down_thread_data->thread_id % session_down_thread_data->thread_division % session_down_thread_data->pair_thread_id % session_down_thread_data->accept_end_flag
                % session_down_thread_data->end_flag % session_down_thread_data->sorry_flag % session_down_thread_data->sorryserver_switch_flag % session_down_thread_data->realserver_switch_flag
                % session_down_thread_data->target_endpoint.address().to_string() % session_down_thread_data->target_endpoint.port()
                % session_down_thread_data->client_endpoint_tcp.address().to_string() % session_down_thread_data->client_endpoint_tcp.port();
                putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/

            session_thread_data_map[session_data->pair_thread_id] = session_down_thread_data;
        }
        else
        {
            session_down_thread_data = session_thread_it->second;
            //thread_id's memory is allocated, check endpoint's memory
            session_down_thread_data->thread_id = session_data->pair_thread_id;
            session_down_thread_data->thread_division = THREAD_DIVISION_DOWN_STREAM;
            session_down_thread_data->pair_thread_id = thread_id;
            session_down_thread_data->accept_end_flag = ACCEPT_END_FLAG_OFF;
            session_down_thread_data->end_flag = END_FLAG_OFF;
            session_down_thread_data->sorry_flag = SORRY_FLAG_OFF;
            session_down_thread_data->sorryserver_switch_flag = SORRYSERVER_SWITCH_FLAG_OFF;
            session_down_thread_data->realserver_switch_flag = REALSERVER_SWITCH_FLAG_OFF;
            session_down_thread_data->client_endpoint_tcp = session_data->client_endpoint_tcp;

            /*-------- DEBUG LOG --------*/
            if (LOG_LV_DEBUG == getloglevel())
            {
                // data dump
                boost::format
                formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                          "handle_sorryserver_select() : session_thread_data_sessionless(down_thread) : "
                          "thread_id = %d, thread_division = %d, "
                          "pair_thread_id = %d, accept_end_flag = %d, end_flag = %d, "
                          "sorry_flag = %d, sorryserver_switch_flag = %d, realserver_switch_flag = %d, client_endpoint_tcp = [%s]:%d.");
                formatter % session_down_thread_data->thread_id % session_down_thread_data->thread_division % session_down_thread_data->pair_thread_id % session_down_thread_data->accept_end_flag
                % session_down_thread_data->end_flag % session_down_thread_data->sorry_flag % session_down_thread_data->sorryserver_switch_flag % session_down_thread_data->realserver_switch_flag
                % session_down_thread_data->client_endpoint_tcp.address().to_string() % session_down_thread_data->client_endpoint_tcp.port();
                putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/

            recive_data_it = session_down_thread_data->recive_data_map.find(sorry_endpoint);
            if (recive_data_it
                    != session_down_thread_data->recive_data_map.end())
            {
                //
                recive_data & recv_data = recive_data_it->second;
                /*-------- DEBUG LOG --------*/
                if (LOG_LV_DEBUG == getloglevel())
                {
                    boost::format formatter("delete : address = &(%d).");
                    formatter % static_cast<void*>(recv_data.recive_buffer1);
                    putLogDebug(10005, formatter.str(), __FILE__,
                                __LINE__ );
                }
                /*------DEBUG LOG END------*/
                delete[] recv_data.recive_buffer1;
                /*-------- DEBUG LOG --------*/
                if (LOG_LV_DEBUG == getloglevel())
                {
                    boost::format formatter("delete : address = &(%d).");
                    formatter % static_cast<void*>(recv_data.recive_buffer2);
                    putLogDebug(10005, formatter.str(), __FILE__,
                                __LINE__ );
                }
                /*------DEBUG LOG END------*/
                delete[] recv_data.recive_buffer2;
                recv_data.recive_buffer1 = NULL;
                recv_data.recive_buffer2 = NULL;
                recv_data.recive_buffer = NULL;
                recv_data.recive_buffer_max_size = 0;
                recv_data.recive_buffer_rest_size = 0;
                recv_data.send_status_list.clear();
            }
            else
            {
                session_down_thread_data->recive_data_map[sorry_endpoint] = recv_data;
            }
        }

        //遷移先ステータスを設定する
        status = SORRYSERVER_CONNECT;
    }
    catch (int e)
    {
        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == getloglevel())
        {
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_realserver_send() : catch exception e = %d.");
            formatter % e;
            putLogError(17000, formatter.str(), __FILE__, __LINE__ );
        }
        status = FINALIZE;
        /*------DEBUG LOG END------*/
    }
    catch (const std::bad_alloc& ex)
    {
        std::cerr << "handle_sorryserver_select exception : Could not allocate memory." << std::endl;
        putLogError(17000, "Could not allocate memory.", __FILE__, __LINE__ );
        status = FINALIZE;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "handle_sorryserver_select exception : error = " << ex.what() << "." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_sorryserver_select() : exception : error = %s.");
        formatter % ex.what();
        putLogError(17000, formatter.str(), __FILE__, __LINE__ );
        status = FINALIZE;
    }
    catch (...)
    {
        std::cerr << "protocol_module_sessionless::handle_sorryserver_select() : Unknown exception." << std::endl;
        putLogError(17000, "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                    "handle_sorryserver_select() : exception : Unknown exception.", __FILE__, __LINE__ );
        status = FINALIZE;
    }

    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format
        formatter(
            "out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
            "handle_sorryserver_select(const boost::thread::id thread_id, boost::asio::ip::tcp::endpoint& sorry_endpoint) : return_value = %d.");
        formatter % status;
        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/
    return status;
}

//! called from after sorryserver connect
//!	@param[in]	upstream thread id
//! @param[out]	send buffer reference.
//! @param[out]	send length
//! @return		session use EVENT mode.
protocol_module_base::EVENT_TAG protocol_module_sessionless::handle_sorryserver_connect(
    const boost::thread::id thread_id, boost::array<char, MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_sorryserver_connect(const boost::thread::id thread_id, "
                                "boost::array<char, MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen) : "
                                "thread_id = %d, sendbuffer = %s, datalen = %d.");
        formatter % thread_id % sendbuffer.data() % datalen;
        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/
    EVENT_TAG status;
    bool ret = false;
    size_t header_offset = 0;
    size_t header_offset_len = 0;
    size_t url_offset = 0;
    size_t url_offset_len = 0;
    size_t send_buffer_remian_size = 0;
    size_t copy_size = 0;
    const int send_buffer_end_size = sendbuffer.max_size();
    const std::string http_header = "";
    const std::string str_forword_for = "X-Forwarded-For";
    session_thread_data_sessionless* session_data = NULL;
    session_thread_data_map_it session_thread_it;
    recive_data_map_it recive_data_it;

    try
    {
        {
            boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);

            //thread id check
            session_thread_it = session_thread_data_map.find(thread_id);
            if (session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL)
            {
                putLogError(17000, "Invalid thread id.", __FILE__, __LINE__ );
                throw -1;
            }

            session_data = session_thread_it->second;
        }

        //endpoint check
        recive_data_it = session_data->recive_data_map.find(session_data->target_endpoint);
        if (recive_data_it
                == session_data->recive_data_map.end())
        {
            putLogError(17000, "Invalid endpoint.", __FILE__, __LINE__ );
            throw -1;
        }

        //recive_buffer pointer check
        recive_data& recv_data = recive_data_it->second;
        if (recv_data.recive_buffer == NULL)
        {
            putLogError(17000, "Invalid recive_buffer.", __FILE__, __LINE__ );
            throw -1;
        }

        //send list check
        send_status_it it = recv_data.send_status_list.begin();
        send_status_it it_end = recv_data.send_status_list.end();

        it = find_if(it, it_end, data_send_possible());
        if (it == it_end)
        {
            putLogError(17000, "Sending possible is not existed.", __FILE__, __LINE__ );
            throw -1;
        }

        //send buffer rest size initliazation
        send_buffer_remian_size = send_buffer_end_size;

        //edit_division flag on
        if (it->edit_division == EDIT_DIVISION_EDIT)
        {
            //edit list is empty
            if (it->edit_data_list.empty())
            {
                //edit data create
                edit_data edata;
                edata.data_size = 0;
                edata.insert_posission = 0;
                edata.replace_size = 0;
                //search uri
                ret = find_uri(recv_data.recive_buffer + it->send_offset, it->send_possible_size, url_offset,
                               url_offset_len);
                /*-------- DEBUG LOG --------*/
                if (LOG_LV_DEBUG == getloglevel())
                {
                    boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                            "handle_sorryserver_connect() : call find_uri : "
                                            "return_value = %d.");
                    formatter % static_cast<int>(ret);
                    putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                }
                /*------DEBUG LOG END------*/
                //serch http header result is OK
                if (ret)
                {
                    //edit sorry_uri, set it to edata.data
                    edata.data = sorry_uri.data();
                    //save new uri offset
                    edata.insert_posission = url_offset;
                    //save new uri length
                    edata.data_size = edata.data.size();
                    //save old uri length
                    edata.replace_size = url_offset_len;
                    //add to edit_data_list
                    it->edit_data_list.push_back(edata);
                }
                //
                else
                {
                }

                if (forwarded_for == FORWARDED_FOR_ON)
                {
                    //search X-Forwarded-For header
                    ret = find_http_header(recv_data.recive_buffer + it->send_offset, it->send_possible_size,
                                           str_forword_for.c_str(), header_offset, header_offset_len);
                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                "handle_sorryserver_connect() : call find_http_header : "
                                                "return_value = %d.");
                        formatter % static_cast<int>(ret);
                        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/

                    //serch http header result is OK
                    if (ret)
                    {
                        //edit X-Forwarded-For header, set it to edata.data
                        edata.data.assign(recv_data.recive_buffer + it->send_offset + header_offset, header_offset_len);
                        edata.data += ", ";
                        edata.data += session_data->client_endpoint_tcp.address().to_string();
                        //save new X-Forwarded-For header offset
                        edata.insert_posission = header_offset;
                        //save new X-Forwarded-For header length
                        edata.data_size = edata.data.size();
                        //save old X-Forwarded-For header length
                        edata.replace_size = header_offset_len;
                    }
                    //serch http header result is NG
                    else
                    {
                        //search whole http header, get whole http header's offset and length
                        ret = find_http_header(recv_data.recive_buffer + it->send_offset, it->send_possible_size, "",
                                               header_offset, header_offset_len);
                        /*-------- DEBUG LOG --------*/
                        if (LOG_LV_DEBUG == getloglevel())
                        {
                            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                    "handle_sorryserver_connect() : call find_http_header : "
                                                    "return_value = %d.");
                            formatter % static_cast<int>(ret);
                            putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        if (!ret)
                        {
                            putLogError(17000, "find_http_header function failure.", __FILE__, __LINE__ );
                            /*-------- DEBUG LOG --------*/
                            if (LOG_LV_DEBUG == getloglevel())
                            {
                                boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                        "handle_sorryserver_connect(const boost::thread::id thread_id, "
                                                        "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                                        "const size_t recvlen) : return_value = %d.");
                                formatter % FINALIZE;
                                putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                            }
                            /*------DEBUG LOG END------*/
                            return FINALIZE;
                        }
                        //create X-Forwarded-For header, set it to edata.data
                        edata.data = str_forword_for;
                        edata.data += ": ";
                        edata.data += session_data->client_endpoint_tcp.address().to_string();
                        edata.data += "\r\n";
                        //save new X-Forwarded-For header offset
                        edata.insert_posission = header_offset;
                        //save new X-Forwarded-For header lenght
                        edata.data_size = edata.data.size();
                        //save old X-Forwarded-For header lenght
                        edata.replace_size = 0;
                    }

                    //add to edit_data_list
                    it->edit_data_list.push_back(edata);
                }
            }

            /*-------- DEBUG LOG --------*/
            if (LOG_LV_DEBUG == getloglevel())
            {
                putLogDebug(10005, "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                            "handle_sorryserver_connect() : Copy data loop start.", __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/
            while (true)
            {
                //edit_data_list is empty
                if (it->edit_data_list.empty())
                {
                    //set edit_division flag off
                    it->edit_division = EDIT_DIVISION_NO_EDIT;

                    if (send_buffer_remian_size > 0 && it->send_possible_size > 0)
                    {
                        //send_buffer_remian_size is larger
                        if (send_buffer_remian_size > it->send_possible_size)
                        {
                            //copy data from recive_buffer to sendbuffer by sending_possible size
                            copy_size = it->send_possible_size;
                            /*-------- DEBUG LOG --------*/
                            if (LOG_LV_DEBUG == getloglevel())
                            {
                                std::string datadump;
                                dump_memory(recv_data.recive_buffer + it->send_offset + it->send_end_size,
                                            copy_size, datadump);
                                boost::format formatter(
                                    "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_sorryserver_connect() : before memcpy (data dump) : "
                                    "data begin = %d, data_size = %d, data = %s.");
                                formatter % (it->send_offset + it->send_end_size)
                                % copy_size % datadump;
                                putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                            }
                            /*------DEBUG LOG END------*/
                            memcpy(sendbuffer.data() + send_buffer_end_size - send_buffer_remian_size,
                                   recv_data.recive_buffer + it->send_offset + it->send_end_size, copy_size);
                            /*-------- DEBUG LOG --------*/
                            if (LOG_LV_DEBUG == getloglevel())
                            {
                                std::string datadump;
                                dump_memory(sendbuffer.data() + send_buffer_end_size - send_buffer_remian_size,
                                            copy_size, datadump);
                                boost::format formatter(
                                    "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_sorryserver_connect() : after memcpy (data dump) : "
                                    "data begin = %d, data_size = %d, data = %s.");
                                formatter % (send_buffer_end_size - send_buffer_remian_size)
                                % copy_size % datadump;
                                putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                            }
                            /*------DEBUG LOG END------*/
                            it->send_end_size += copy_size;
                            it->send_possible_size = 0;
                            send_buffer_remian_size -= copy_size;
                        }
                        //send_possible_size is larger
                        else
                        {
                            copy_size = send_buffer_remian_size;
                            /*-------- DEBUG LOG --------*/
                            if (LOG_LV_DEBUG == getloglevel())
                            {
                                std::string datadump;
                                dump_memory(recv_data.recive_buffer + it->send_offset + it->send_end_size,
                                            copy_size, datadump);
                                boost::format formatter(
                                    "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_sorryserver_connect() : before memcpy (data dump) : "
                                    "data begin = %d, data_size = %d, data = %s.");
                                formatter % (it->send_offset + it->send_end_size)
                                % copy_size % datadump;
                                putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                            }
                            /*------DEBUG LOG END------*/
                            //copy data from recive_buffer to sendbuffer by send buffer rest size
                            memcpy(sendbuffer.data() + send_buffer_end_size - send_buffer_remian_size,
                                   recv_data.recive_buffer + it->send_offset + it->send_end_size,
                                   copy_size);
                            /*-------- DEBUG LOG --------*/
                            if (LOG_LV_DEBUG == getloglevel())
                            {
                                std::string datadump;
                                dump_memory(sendbuffer.data() + send_buffer_end_size - send_buffer_remian_size,
                                            copy_size, datadump);
                                boost::format formatter(
                                    "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_sorryserver_connect() : after memcpy (data dump) : "
                                    "data begin = %d, data_size = %d, data = %s.");
                                formatter % (send_buffer_end_size - send_buffer_remian_size)
                                % copy_size % datadump;
                                putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                            }
                            /*------DEBUG LOG END------*/
                            it->send_end_size += copy_size;
                            it->send_possible_size -= copy_size;
                            send_buffer_remian_size = 0;
                        }
                    }

                    break;
                }
                //edit_data_list is not empty
                else
                {
                    //search item which insert_posission is mininum
                    std::list<edit_data>::iterator edit_min = std::min_element(it->edit_data_list.begin(),
                            it->edit_data_list.end());
                    //send_buffer_remian_size is larger than data that before X-Forwarded-For/uri
                    if (send_buffer_remian_size >= edit_min->insert_posission - it->send_end_size)
                    {
                        //copy data before X-Forwarded-For/url
                        copy_size = edit_min->insert_posission - it->send_end_size;
                        /*-------- DEBUG LOG --------*/
                        if (LOG_LV_DEBUG == getloglevel())
                        {
                            std::string datadump;
                            dump_memory(recv_data.recive_buffer + it->send_offset + it->send_end_size,
                                        copy_size, datadump);

                            boost::format formatter(
                                "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_sorryserver_connect() : before memcpy (data dump) : "
                                "data begin = %d, data_size = %d, data = %s.");
                            formatter % (it->send_offset + it->send_end_size)
                            % copy_size % datadump;
                            putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        memcpy(sendbuffer.data() + send_buffer_end_size - send_buffer_remian_size,
                               recv_data.recive_buffer + it->send_offset + it->send_end_size, copy_size);
                        /*-------- DEBUG LOG --------*/
                        if (LOG_LV_DEBUG == getloglevel())
                        {
                            std::string datadump;
                            dump_memory(sendbuffer.data() + send_buffer_end_size - send_buffer_remian_size,
                                        copy_size, datadump);
                            boost::format formatter(
                                "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_sorryserver_connect() : after memcpy (data dump) : "
                                "data begin = %d, data_size = %d, data = %s.");
                            formatter % (send_buffer_end_size - send_buffer_remian_size)
                            % copy_size % datadump;
                            putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        it->send_end_size += copy_size;
                        it->send_possible_size -= copy_size;
                        send_buffer_remian_size -= copy_size;

                        //there is remain buffer for copy X-Forwarded-For/url
                        if (send_buffer_remian_size >= edit_min->data_size)
                        {
                            /*-------- DEBUG LOG --------*/
                            if (LOG_LV_DEBUG == getloglevel())
                            {
                                std::string datadump;
                                dump_memory(edit_min->data.c_str(),
                                            edit_min->data_size, datadump);
                                boost::format formatter(
                                    "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_sorryserver_connect() : before memcpy (data dump) : "
                                    "data begin = 0, data_size = %d, data = %s.");
                                formatter % edit_min->data_size % datadump;
                                putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                            }
                            /*------DEBUG LOG END------*/
                            //copy X-Forwarded-For/uri
                            memcpy(sendbuffer.data() + send_buffer_end_size - send_buffer_remian_size,
                                   edit_min->data.c_str(), edit_min->data_size);
                            /*-------- DEBUG LOG --------*/
                            if (LOG_LV_DEBUG == getloglevel())
                            {
                                std::string datadump;
                                dump_memory(sendbuffer.data() + send_buffer_end_size - send_buffer_remian_size,
                                            edit_min->data_size, datadump);

                                boost::format formatter(
                                    "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_sorryserver_connect() : after memcpy (data dump) : "
                                    "data begin = %d, data_size = %d, data = %s.");
                                formatter % (send_buffer_end_size - send_buffer_remian_size)
                                % edit_min->data_size % datadump;
                                putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                            }
                            /*------DEBUG LOG END------*/
                            it->send_end_size += edit_min->replace_size;
                            it->send_possible_size -= edit_min->replace_size;
                            send_buffer_remian_size -= edit_min->data_size;
                            it->edit_data_list.erase(edit_min);
                        }
                        //
                        else
                        {
                            break;
                        }
                    }
                    //data that before X-Forwarded-For/uri is larger than send_buffer_remian_size
                    else
                    {
                        copy_size = send_buffer_remian_size;
                        /*-------- DEBUG LOG --------*/
                        if (LOG_LV_DEBUG == getloglevel())
                        {
                            std::string datadump;
                            dump_memory(sendbuffer.data() + send_buffer_end_size - send_buffer_remian_size,
                                        copy_size, datadump);

                            boost::format formatter(
                                "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_sorryserver_connect() : before memcpy (data dump) : "
                                "data begin = %d, data_size = %d, data = %s.");
                            formatter % (it->send_offset + it->send_end_size)
                            % copy_size % datadump;
                            putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        //copy data as large as possible
                        memcpy(sendbuffer.data() + send_buffer_end_size - send_buffer_remian_size,
                               recv_data.recive_buffer + it->send_offset + it->send_end_size, send_buffer_remian_size);
                        /*-------- DEBUG LOG --------*/
                        if (LOG_LV_DEBUG == getloglevel())
                        {
                            std::string datadump;
                            dump_memory(sendbuffer.data() + send_buffer_end_size - send_buffer_remian_size,
                                        copy_size, datadump);

                            boost::format formatter(
                                "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_sorryserver_connect() : after memcpy (data dump) : "
                                "data begin = %d, data_size = %d, data = %s.");
                            formatter % (send_buffer_end_size - send_buffer_remian_size)
                            % copy_size % datadump;
                            putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        it->send_end_size += copy_size;
                        it->send_possible_size -= copy_size;
                        send_buffer_remian_size -= copy_size;
                        break;
                    }
                }
            }
            /*-------- DEBUG LOG --------*/
            if (LOG_LV_DEBUG == getloglevel())
            {
                putLogDebug(10005, "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                            "handle_sorryserver_connect() : Copy data loop end.", __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/
        }
        //edit_division flag is off
        else
        {
            //copy data as large as possbile
            //send_possible_size is larger
            if (send_buffer_remian_size >= it->send_possible_size)
            {
                copy_size = it->send_possible_size;
                /*-------- DEBUG LOG --------*/
                if (LOG_LV_DEBUG == getloglevel())
                {
                    std::string datadump;
                    dump_memory(recv_data.recive_buffer+ it->send_offset, copy_size, datadump);

                    boost::format formatter(
                        "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                        "handle_sorryserver_connect() : before memcpy (data dump) : "
                        "data begin = %d, data_size = %d, data = %s.");
                    formatter % it->send_offset
                    % copy_size % datadump;
                    putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                }
                /*------DEBUG LOG END------*/
                //copy date by send_possible size
                memcpy(sendbuffer.data(), recv_data.recive_buffer
                       + it->send_offset, copy_size);
                /*-------- DEBUG LOG --------*/
                if (LOG_LV_DEBUG == getloglevel())
                {
                    std::string datadump;
                    dump_memory(sendbuffer.data(), copy_size, datadump);

                    boost::format formatter(
                        "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                        "handle_sorryserver_connect() : after memcpy (data dump) : "
                        "data begin = 0, data_size = %d, data = %s.");
                    formatter % copy_size % datadump;
                    putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                }
                /*------DEBUG LOG END------*/
                it->send_end_size = it->send_possible_size;
                it->send_possible_size = 0;
                send_buffer_remian_size -= copy_size;
            }
            //buffer rest size is larger
            else
            {
                /*-------- DEBUG LOG --------*/
                if (LOG_LV_DEBUG == getloglevel())
                {
                    std::string datadump;
                    dump_memory(recv_data.recive_buffer+ it->send_offset, send_buffer_remian_size, datadump);

                    boost::format formatter(
                        "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                        "handle_sorryserver_connect() : before memcpy (data dump) : "
                        "data begin = %d, data_size = %d, data = %s.");
                    formatter % it->send_offset
                    % send_buffer_remian_size % datadump;
                    putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                }
                /*------DEBUG LOG END------*/
                //copy date by buffer rest size
                memcpy(sendbuffer.data(), recv_data.recive_buffer
                       + it->send_offset, send_buffer_remian_size);
                /*-------- DEBUG LOG --------*/
                if (LOG_LV_DEBUG == getloglevel())
                {
                    std::string datadump;
                    dump_memory(sendbuffer.data(), send_buffer_remian_size, datadump);

                    boost::format formatter(
                        "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                        "handle_sorryserver_connect() : after memcpy (data dump) : "
                        "data begin = 0, data_size = %d, data = %s.");
                    formatter % send_buffer_remian_size % datadump;
                    putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                }
                /*------DEBUG LOG END------*/
                it->send_end_size = send_buffer_remian_size;
                it->send_possible_size -= send_buffer_remian_size;
                send_buffer_remian_size = 0;
            }
        }

        //set copyed date lenght
        datalen = send_buffer_end_size - send_buffer_remian_size;

        status = SORRYSERVER_SEND;

    }
    catch (int e)
    {
        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == getloglevel())
        {
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_sorryserver_connect() : catch exception e = %d.");
            formatter % e;
            putLogError(17000, formatter.str(), __FILE__, __LINE__ );
        }
        status = FINALIZE;
        /*------DEBUG LOG END------*/
    }
    catch (const std::exception& ex)
    {
        std::cerr << "handle_sorryserver_connect exception: error = " << ex.what() << "." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_sorryserver_connect() : exception : error = %s.");
        formatter % ex.what();
        putLogError(17000, formatter.str(), __FILE__, __LINE__ );
        status = FINALIZE;
    }
    catch (...)
    {
        std::cerr << "protocol_module_sessionless::handle_sorryserver_connect() : Unknown exception." << std::endl;
        putLogError(17000, "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                    "handle_sorryserver_connect() : Unknown exception.", __FILE__, __LINE__ );
        status = FINALIZE;
    }

    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_sorryserver_connect(const boost::thread::id thread_id, "
                                "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                "const size_t recvlen) : return_value = %d.");
        formatter % status;
        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    return status;
}

//! called from after sorryserver connection fail
//! @param[in]	upstream thread id
//! @param[in]	sorryserver endpoint reference.
//! @return		session use EVENT mode.
protocol_module_base::EVENT_TAG protocol_module_sessionless::handle_sorryserver_connection_fail(
    const boost::thread::id thread_id, const boost::asio::ip::tcp::endpoint & sorry_endpoint)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_sorryserver_connection_fail(const boost::thread::id thread_id, "
                                "const boost::asio::ip::tcp::endpoint & sorry_endpoint) : "
                                "thread_id = %d, sorry_endpoint = %s.");
        formatter % thread_id % sorry_endpoint.address().to_string();
        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    EVENT_TAG status;
    session_thread_data_sessionless* session_data = NULL;
    session_thread_data_map_it session_thread_it;

    try
    {

        boost::mutex::scoped_lock slock(session_thread_data_map_mutex);

        session_thread_it = session_thread_data_map.find(thread_id);
        if (session_thread_it == session_thread_data_map.end())
        {
            putLogError(17000, "Invalid thread id.", __FILE__, __LINE__ );
            return FINALIZE;
        }

        session_data = session_thread_it->second;
        if (session_data == NULL)
        {
            putLogError(17000, "Invalid pointer.", __FILE__, __LINE__ );
            return FINALIZE;
        }

        //set end flag on
        session_data->end_flag = END_FLAG_ON;

        status = CLIENT_DISCONNECT;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "handle_sorryserver_connection_fail exception : error=" << ex.what() << "." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_sorryserver_connection_fail() : exception : result = %d, error = %s.");
        formatter % FINALIZE % ex.what();
        putLogError(17000, formatter.str(), __FILE__, __LINE__ );
        status = FINALIZE;
    }
    catch (...)
    {
        std::cerr << "Unknown exception." << std::endl;
        putLogError(17000, "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                    "handle_sorryserver_connection_fail() : Unknown exception.", __FILE__, __LINE__ );
        status = FINALIZE;
    }

    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format
        formatter(
            "out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
            "handle_sorryserver_connection_fail( const boost::thread::id thread_id, const boost::asio::ip::tcp::endpoint & sorry_endpoint) : return_value = %d.");
        formatter % status;
        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/
    return status;
}

//! called from after sorryserver send
//! @param[in]	upstream thread id
//! @return		session use EVENT mode.
protocol_module_base::EVENT_TAG protocol_module_sessionless::handle_sorryserver_send(
    const boost::thread::id thread_id)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_sorryserver_send(const boost::thread::id thread_id) : "
                                "thread_id = %d.");
        formatter % thread_id;
        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/
    EVENT_TAG status;
    session_thread_data_sessionless* session_data = NULL;
    session_thread_data_map_it session_thread_it;
    recive_data_map_it recive_data_it;

    try
    {
        {
            boost::mutex::scoped_lock slock(session_thread_data_map_mutex);

            //thread_id check
            session_thread_it = session_thread_data_map.find(thread_id);
            if (session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL)
            {
                putLogError(17000, "Invalid thread id.", __FILE__, __LINE__ );
                throw -1;
            }

            session_data = session_thread_it->second;
        }

        //endpoint check
        recive_data_it = session_data->recive_data_map.find(session_data->client_endpoint_tcp);
        if (recive_data_it
                == session_data->recive_data_map.end())
        {
            putLogError(17000, "Invalid endpoint.", __FILE__, __LINE__ );
            throw -1;
        }

        recive_data& recv_data = recive_data_it->second;

        send_status_it it = recv_data.send_status_list.begin();
        send_status_it it_end = recv_data.send_status_list.end();
        //status list check
        it = std::adjacent_find(it, it_end, data_send_list_incorrect());
        if (it != it_end)
        {
            putLogError(17000, "Sending possible data is invalid.", __FILE__, __LINE__ );
            throw -1;
        }

        //status list check
        it = recv_data.send_status_list.begin();
        it = find_if(it, it_end, data_send_ok());
        if (it == it_end)
        {
            putLogError(17000, "Sending possible data is not existed.", __FILE__, __LINE__ );
            throw -1;
        }

        //sending possible data is exist
        if (it->send_possible_size > 0)
        {
            //status remain SEND_OK
            it->status = SEND_OK;
            //offset recalc
            it->send_offset += it->send_end_size;

            //insert_posission recalc
            for (std::list<edit_data>::iterator list_it = it->edit_data_list.begin(); list_it
                    != it->edit_data_list.end(); ++list_it)
            {
                list_it->insert_posission -= it->send_end_size;
            }

            //send_end_size recalc
            it->send_end_size = 0;
        }
        //sending possible data is not exist
        else
        {
            //can recive from clent continue
            if (it->send_rest_size > 0)
            {
                //change status from SEND_OK to SEND_CONTINUE
                it->status = SEND_CONTINUE;
            }
            //can not recive from clent continue
            else
            {
                //change status from SEND_OK to SEND_END
                it->status = SEND_END;
            }
        }

        it = recv_data.send_status_list.begin();
        it = find_if(it, it_end, data_send_ok());
        //send_ok item is exist
        if (it != it_end)
        {
            status = SORRYSERVER_CONNECT;
        }
        //send_ok item is exist
        else
        {
            status = CLIENT_RECV;
        }

    }
    catch (int e)
    {
        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == getloglevel())
        {
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_sorryserver_send() : catch exception e = %d.");
            formatter % e;
            putLogError(17000, formatter.str(), __FILE__, __LINE__ );
        }
        status = FINALIZE;
        /*------DEBUG LOG END------*/
    }
    catch (const std::exception& ex)
    {
        std::cerr << "handle_sorryserver_send exception : error = " << ex.what() << "." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_sorryserver_send() : exception : error = %s.");
        formatter % ex.what();
        putLogError(17000, formatter.str(), __FILE__, __LINE__ );

        status = FINALIZE;
    }
    catch (...)
    {
        std::cerr << "protocol_module_sessionless::handle_sorryserver_send() : Unknown exception." << std::endl;
        putLogError(17000, "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                    "handle_sorryserver_send() : exception : Unknown exception.", __FILE__, __LINE__ );
        status = FINALIZE;
    }

    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_sorryserver_send(const boost::thread::id thread_id) : return_value = %d.");
        formatter % status;
        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    return status;
}

//! called from after realserver recive.for UDP
//! @param[in]	downstream thread id
//! @param[in]	realserver UDP endpoint reference
//! @param[in]	recive from realserver buffer reference
//! @param[in]	recv data length
//! @return		session use EVENT mode.
protocol_module_base::EVENT_TAG protocol_module_sessionless::handle_realserver_recv(
    const boost::thread::id thread_id, const boost::asio::ip::udp::endpoint& rs_endpoint, const boost::array<char,
    MAX_BUFFER_SIZE>& recvbuffer, const size_t recvlen)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("in/out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_realserver_recv(const boost::thread::id thread_id, "
                                "const boost::asio::ip::udp::endpoint& rs_endpoint, "
                                "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                "const size_t recvlen) : "
                                "return_value = %d.");
        formatter % STOP;
        putLogDebug(10000, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/
    return STOP;
}

//! called from after realserver recvive for TCP/IP
//! @param[in]	downstream thread id
//! @param[in]	realserver TCP/IP endpoint reference
//! @param[in]	realserver recive buffer reference.
//! @param[in]	recv data length
//! @return		session use EVENT mode.
protocol_module_base::EVENT_TAG protocol_module_sessionless::handle_realserver_recv(
    const boost::thread::id thread_id, const boost::asio::ip::tcp::endpoint& rs_endpoint, const boost::array<char,
    MAX_BUFFER_SIZE>& recvbuffer, const size_t recvlen)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_realserver_recv(const boost::thread::id thread_id, "
                                "const boost::asio::ip::tcp::endpoint& rs_endpoint, "
                                "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                "const size_t recvlen) : thread_id = %d, rs_endpoint = [%s]:%d, recvbuffer = %s, recvlen = %d.");
        formatter % thread_id % rs_endpoint.address().to_string() % rs_endpoint.port()
        % recvbuffer.data() % recvlen;
        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    EVENT_TAG status;
    size_t data_remain_start = 0;
    size_t data_remain_size = 0;
    size_t request_data_remain_size = 0;
    size_t header_offset = 0;
    size_t header_offset_len = 0;
    size_t content_length_header_offset = 0;
    size_t content_length_header_len = 0;
    size_t content_len_value = 0;
    size_t pos = 0;
    const size_t cr_lf_len = strlen("\r\n\r\n");
    session_thread_data_sessionless* session_data = NULL;
    char* buffer1 = NULL;
    char* buffer2 = NULL;
    std::string str_value;
    const std::string http_header = "";
    const std::string content_header = "Content-Length";
    bool bret = false;
    CHECK_RESULT_TAG check_result;
    session_thread_data_map_it session_thread_it;
    recive_data_map_it recive_data_it;

    //parameter check
    if (recvlen > recvbuffer.size())
    {
        std::cerr << "handle_realserver_recv() : Data size bigger than buffer size." << std::endl;
        putLogError(17000, "Data size bigger than buffer size.", __FILE__,
                    __LINE__ );
        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == getloglevel())
        {
            boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_realserver_recv(const boost::thread::id thread_id, "
                                    "const boost::asio::ip::tcp::endpoint& rs_endpoint, "
                                    "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                    "const size_t recvlen) : return_value = %d.");
            formatter % FINALIZE;
            putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/
        return FINALIZE;
    }

    try
    {
        {
            boost::mutex::scoped_lock slock(session_thread_data_map_mutex);

            session_thread_it = session_thread_data_map.find(thread_id);
            if (session_thread_it == session_thread_data_map.end()
                    ||session_thread_it->second == NULL)
            {
                putLogError(17000, "Invalid thread id.", __FILE__, __LINE__ );
                throw -1;
            }

            session_data = session_thread_it->second;
        }

        recive_data_it = session_data->recive_data_map.find(session_data->target_endpoint);
        if (recive_data_it == session_data->recive_data_map.end())
        {
            putLogError(17000, "Invalid endpoint.", __FILE__, __LINE__ );
            throw -1;
        }

        recive_data& recv_data = recive_data_it->second;

        send_status_it it = recv_data.send_status_list.begin();
        send_status_it it_end = recv_data.send_status_list.end();

        //status list check
        it = std::find_if(it, it_end, data_send_ok());
        if (it != it_end)
        {
            putLogError(17000, "Sending data is not correct.", __FILE__, __LINE__ );
            throw -1;
        }

        //status list check
        it = recv_data.send_status_list.begin();
        it = std::adjacent_find(it, it_end, data_send_repeated());
        if (it != it_end)
        {
            putLogError(17000, "Sending data is not correct.", __FILE__, __LINE__ );
            throw -1;
        }

        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == getloglevel())
        {
            putLogDebug(10005, "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                        "handle_realserver_recv() : step1--send status list loop start.", __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/
        it = recv_data.send_status_list.begin();
        //get original status info
        while (it != it_end)
        {
            //item status is SEND_END
            if (it->status == SEND_END)
            {
                //erase from list
                recv_data.send_status_list.erase(it++);
                continue;
            }
            //item status is SEND_CONTINUE
            else if (it->status == SEND_CONTINUE)
            {
                it->send_offset += it->send_end_size;
                data_remain_start = it->send_offset;
                break;
            }
            //item status is SEND_NG
            else
            {
                data_remain_start = it->send_offset;
                data_remain_size = it->unsend_size;
                break;
            }

            ++it;
        }
        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == getloglevel())
        {
            putLogDebug(10005, "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                        "handle_realserver_recv() : step1--send status list loop end.", __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        //recive buffer process
        //buffer rest size　＜　request size
        if (recv_data.recive_buffer_rest_size < recvlen)
        {
            //buffer max size　＜　remain size　＋　request size
            //buffer is need reallcate
            if (recv_data.recive_buffer_max_size < data_remain_size + recvlen)
            {
                //first recive
                if (recv_data.recive_buffer == NULL)
                {
                    //recive_buffer1's memery allocate and initialization
                    recv_data.recive_buffer1 = new char[MAX_BUFFER_SIZE];
                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        boost::format formatter("new : address = &(%d), size = %lu.");
                        formatter % static_cast<void*>(recv_data.recive_buffer1) % (MAX_BUFFER_SIZE);
                        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*-------- DEBUG LOG END--------*/
                    memset(recv_data.recive_buffer1, 0, MAX_BUFFER_SIZE);
                    //recive_buffer2's memery allocate and initialization
                    recv_data.recive_buffer2 = new char[MAX_BUFFER_SIZE];
                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        boost::format formatter("new : address = &(%d), size = %lu.");
                        formatter % static_cast<void*>(recv_data.recive_buffer2) % (MAX_BUFFER_SIZE);
                        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*-------- DEBUG LOG END--------*/
                    memset(recv_data.recive_buffer2, 0, MAX_BUFFER_SIZE);
                    //set uisng buffer recive_buffer1 for defalut
                    recv_data.recive_buffer = recv_data.recive_buffer1;
                    //set max buffer size MAX_BUFFER_SIZE
                    recv_data.recive_buffer_max_size = MAX_BUFFER_SIZE;
                }
                //not first recive
                else
                {
                    //recive_buffer1's memery allocate and initialization
                    buffer1 = new char[data_remain_size + recvlen];
                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        boost::format formatter("new : address = &(%d), size = %lu.");
                        formatter % static_cast<void*>(buffer1) % (data_remain_size + recvlen);
                        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*-------- DEBUG LOG --------*/
                    memset(buffer1, 0, data_remain_size + recvlen);
                    //recive_buffer2's memery allocate and initialization
                    buffer2 = new char[data_remain_size + recvlen];
                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        boost::format formatter("new : address = &(%d), size = %lu.");
                        formatter % static_cast<void*>(buffer2) % (data_remain_size + recvlen);
                        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*-------- DEBUG LOG END--------*/
                    memset(buffer2, 0, data_remain_size + recvlen);

                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        std::string datadump;
                        dump_memory(recv_data.recive_buffer + data_remain_start, data_remain_size, datadump);

                        boost::format formatter(
                            "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                            "handle_realserver_recv() : before memcpy (data dump) : "
                            "data begin = %d, data_size = %d, data = %s.");
                        formatter % data_remain_start % data_remain_size % datadump;
                        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                    //copy data from old buffer to new buffer
                    memcpy(buffer1, recv_data.recive_buffer + data_remain_start, data_remain_size);
                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        std::string datadump;
                        dump_memory(buffer1, data_remain_size, datadump);

                        boost::format formatter(
                            "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                            "handle_realserver_recv() : after memcpy (data dump) : "
                            "data begin = 0, data_size = %d, data = %s.");
                        formatter % data_remain_size % datadump;
                        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/

                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        std::string datadump;
                        dump_memory(recvbuffer.data(), recvlen, datadump);
                        boost::format formatter(
                            "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                            "handle_realserver_recv() : before memcpy (data dump) : "
                            "data begin = 0, data_size = %d, data = %s.");
                        formatter % recvlen % datadump;
                        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                    memcpy(buffer1 + data_remain_size, recvbuffer.data(), recvlen);
                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        std::string datadump;
                        dump_memory(buffer1 + data_remain_size, recvlen, datadump);
                        boost::format formatter(
                            "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                            "handle_realserver_recv() : after memcpy (data dump) :  "
                            "data begin = %d, data_size = %d, data = %s.");
                        formatter % data_remain_size % recvlen % datadump;
                        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                    //free old buffer1 and old buffer2
                    if (recv_data.recive_buffer1 != NULL)
                    {
                        /*-------- DEBUG LOG --------*/
                        if (LOG_LV_DEBUG == getloglevel())
                        {
                            boost::format formatter("delete : address = &(%d).");
                            formatter % static_cast<void*>(recv_data.recive_buffer1);
                            putLogDebug(10005, formatter.str(), __FILE__,
                                        __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        delete[] recv_data.recive_buffer1;
                        recv_data.recive_buffer1 = NULL;
                    }

                    if (recv_data.recive_buffer2 != NULL)
                    {
                        /*-------- DEBUG LOG --------*/
                        if (LOG_LV_DEBUG == getloglevel())
                        {
                            boost::format formatter("delete : address = &(%d).");
                            formatter % static_cast<void*>(recv_data.recive_buffer2);
                            putLogDebug(10005, formatter.str(), __FILE__,
                                        __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        delete[] recv_data.recive_buffer2;
                        recv_data.recive_buffer2 = NULL;
                    }

                    //set new buffer pointer
                    recv_data.recive_buffer1 = buffer1;
                    recv_data.recive_buffer2 = buffer2;
                    recv_data.recive_buffer = recv_data.recive_buffer1;
                    //set new buffer's max size
                    recv_data.recive_buffer_max_size = data_remain_size + recvlen;
                }
            }
            //buffer's max size　≧　remain data size　＋　requst size
            //buffer isn't need reallcate, but switch
            else
            {
                //pointer valid check
                if (recv_data.recive_buffer1 == NULL || recv_data.recive_buffer2 == NULL)
                {
                    putLogError(17000, "Invalid pointer.", __FILE__, __LINE__ );
                    throw -1;
                }
                //using buffer is buffer1
                if (recv_data.recive_buffer == recv_data.recive_buffer1)
                {
                    //buffer2 initialization
                    memset(recv_data.recive_buffer2, 0, recv_data.recive_buffer_max_size);
                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        std::string datadump;
                        dump_memory(recv_data.recive_buffer + data_remain_start, data_remain_size, datadump);
                        boost::format formatter(
                            "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                            "handle_realserver_recv() : before memcpy (data dump) : "
                            "data begin = %d, data_size = %d, data = %s.");
                        formatter % data_remain_start % data_remain_size % datadump;
                        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                    //copy data from buffer1 to buffer2
                    memcpy(recv_data.recive_buffer2, recv_data.recive_buffer + data_remain_start, data_remain_size);
                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        std::string datadump;
                        dump_memory(recv_data.recive_buffer2, data_remain_size, datadump);
                        boost::format formatter(
                            "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                            "handle_realserver_recv() : after memcpy (data dump) : "
                            "data begin = 0, data_size = %d, data = %s.");
                        formatter % data_remain_size % datadump;
                        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        std::string datadump;
                        dump_memory(recvbuffer.data(), recvlen, datadump);
                        boost::format formatter(
                            "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                            "handle_realserver_recv() : before memcpy (data dump) : "
                            "data begin = 0, data_size = %d, data = %s.");
                        formatter % data_remain_size % datadump;
                        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                    memcpy(recv_data.recive_buffer2 + data_remain_size, recvbuffer.data(), recvlen);
                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        std::string datadump;
                        dump_memory(recv_data.recive_buffer2 + data_remain_size, recvlen, datadump);

                        boost::format formatter(
                            "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                            "handle_realserver_recv() : after memcpy (data dump) : "
                            "data begin = %d, data_size = %d, data = %s.");
                        formatter % data_remain_size % recvlen % datadump;
                        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                    //set buffer2 for using buffer
                    recv_data.recive_buffer = recv_data.recive_buffer2;
                }
                //using buffer is buffer2
                else
                {
                    //buffer1 initializtion
                    memset(recv_data.recive_buffer1, 0, recv_data.recive_buffer_max_size);
                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        std::string datadump;
                        dump_memory(recv_data.recive_buffer + data_remain_start, data_remain_size, datadump);

                        boost::format formatter(
                            "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                            "handle_realserver_recv() : before memcpy (data dump) : "
                            "data begin = %d, data_size = %d, data = %s.");
                        formatter % data_remain_start % (data_remain_size+recvlen) % datadump;
                        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                    //copy data from buffer2 to buffer1
                    memcpy(recv_data.recive_buffer1, recv_data.recive_buffer + data_remain_start, data_remain_size);
                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        std::string datadump;
                        dump_memory(recv_data.recive_buffer1, data_remain_size, datadump);

                        boost::format formatter(
                            "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                            "handle_realserver_recv() : after memcpy (data dump) : "
                            "data begin = 0, data_size = %d, data = %s.");
                        formatter % data_remain_size % datadump;
                        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        std::string datadump;
                        dump_memory(recvbuffer.data(), recvlen, datadump);

                        boost::format formatter(
                            "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                            "handle_realserver_recv() : before memcpy (data dump) : "
                            "data begin = 0, data_size = %d, data = %s.");
                        formatter % recvlen % datadump;
                        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                    memcpy(recv_data.recive_buffer1 + data_remain_size, recvbuffer.data(), recvlen);
                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        std::string datadump;
                        dump_memory(recv_data.recive_buffer1 + data_remain_size, recvlen, datadump);

                        boost::format formatter(
                            "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                            "handle_realserver_recv() : after memcpy (data dump) : "
                            "data begin = %d, data_size = %d, data = %s.");
                        formatter % data_remain_size % recvlen % datadump;
                        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                    //set buffer1 for using buffer
                    recv_data.recive_buffer = recv_data.recive_buffer1;
                }
            }

            //set buffer's rest size
            recv_data.recive_buffer_rest_size = recv_data.recive_buffer_max_size - data_remain_size - recvlen;

            //remain_size recalc
            data_remain_size += recvlen;

            send_status_it it_begin = recv_data.send_status_list.begin();
            send_status_it it_end = recv_data.send_status_list.end();
            //offset recalc
            for (; it_begin != it_end; ++it_begin)
            {
                it->send_offset -= data_remain_start;
            }
        }
        //buffer's rest size　≧　request size
        //copy directly
        else
        {
            //pointer valid check
            if (recv_data.recive_buffer == NULL)
            {
                putLogError(17000, "Invalid pointer.", __FILE__, __LINE__ );
                throw -1;
            }
            /*-------- DEBUG LOG --------*/
            if (LOG_LV_DEBUG == getloglevel())
            {
                std::string datadump;
                dump_memory(recvbuffer.data(), recvlen, datadump);
                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                        "handle_realserver_recv() : before memcpy (data dump) : "
                                        "data begin = 0, data_size = %d, data = %s.");
                formatter % recvlen % datadump;
                putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/

            //copy data from paramenter to using buffer
            memcpy(recv_data.recive_buffer + recv_data.recive_buffer_max_size - recv_data.recive_buffer_rest_size,
                   recvbuffer.data(), recvlen);
            /*-------- DEBUG LOG --------*/
            if (LOG_LV_DEBUG == getloglevel())
            {
                std::string datadump;
                dump_memory(recv_data.recive_buffer + recv_data.recive_buffer_max_size - recv_data.recive_buffer_rest_size,
                            recvlen, datadump);
                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                        "handle_realserver_recv() : before memcpy (data dump) : "
                                        "data begin = %d, data_size = %d, data = %s.");
                formatter % (recv_data.recive_buffer_max_size - recv_data.recive_buffer_rest_size )
                % recvlen % datadump;
                putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/
            //buffer's rest size recalc
            recv_data.recive_buffer_rest_size -= recvlen;
            //remain data size recalc
            data_remain_size += recvlen;
        }
        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == getloglevel())
        {
            putLogDebug(10005, "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                        "handle_realserver_recv() : step2--data buffer size process end.", __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == getloglevel())
        {
            putLogDebug(10005, "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                        "handle_realserver_recv() : step3--send status list loop start.", __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        it = recv_data.send_status_list.begin();
        it_end = recv_data.send_status_list.end();
        //request rest size initialization
        request_data_remain_size = recvlen;
        //original status process
        for (; it != it_end; ++it)
        {
            //status is SEND_CONTINUE
            if (it->status == SEND_CONTINUE)
            {
                //send rest size　>　request size
                if (it->send_rest_size > request_data_remain_size)
                {
                    //send possible size recalc
                    it->send_possible_size = request_data_remain_size;
                    //send rest size recalc
                    it->send_rest_size -= request_data_remain_size;
                    //send end size recalc
                    it->send_end_size = 0;
                    //request size recalc
                    request_data_remain_size = 0;
                }
                //send rest size　<=　request size
                else
                {
                    //send possible size recalc
                    it->send_possible_size = it->send_rest_size;
                    //send rest size recalc
                    request_data_remain_size -= it->send_rest_size;
                    //send end size recalc
                    it->send_end_size = 0;
                    //request size recalc
                    it->send_rest_size = 0;
                }
//				//編集区分に編集なしを設定する
//				it->edit_division = EDIT_DIVISION_NO_EDIT;
                //change status from SEND_CONTINUE to SEND_OK
                it->status = SEND_OK;
            }
            //status is SEND_NG
            else if (it->status == SEND_NG)
            {
                //check http method
                check_result = check_status_code(recv_data.recive_buffer + it->send_offset, data_remain_size);
                /*-------- DEBUG LOG --------*/
                if (LOG_LV_DEBUG == getloglevel())
                {
                    boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                            "handle_realserver_recv() : call check_http_method : "
                                            "return_value = %d.");
                    formatter % check_result;
                    putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                }
                /*------DEBUG LOG END------*/
                //check http method result is OK
                if (check_result == CHECK_OK)
                {
                    //check http version
                    check_result = check_http_version(recv_data.recive_buffer + it->send_offset, data_remain_size);
                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                "handle_realserver_recv() : call check_http_version : "
                                                "return_value = %d.");
                        formatter % check_result;
                        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                }
                //check method and version result is OK
                if (check_result == CHECK_OK)
                {
                    //serch http header
                    bret = find_http_header(recv_data.recive_buffer + it->send_offset, data_remain_size, http_header,
                                            header_offset, header_offset_len);
                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                "handle_realserver_recv() : call find_http_header : "
                                                "return_value = %d.");
                        formatter % static_cast<int>(bret);
                        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                    //serch http header result is OK
                    if (bret)
                    {
                        //serch Content_Length header
                        bret = find_http_header(recv_data.recive_buffer + it->send_offset, data_remain_size,
                                                content_header, content_length_header_offset, content_length_header_len);
                        /*-------- DEBUG LOG --------*/
                        if (LOG_LV_DEBUG == getloglevel())
                        {
                            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                    "handle_realserver_recv() : call find_http_header : "
                                                    "return_value = %d.");
                            formatter % formatter % static_cast<int>(bret);
                            putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        //serch Content_Length result is OK
                        if (bret)
                        {
                            //Get Content_Length header's numeric value
                            for (pos = 0; recv_data.recive_buffer[it->send_offset + content_length_header_offset + pos] != ':' && pos
                                    < content_length_header_len; ++pos)
                                ;
                            if (pos == content_length_header_len)
                            {
                                throw std::string("Contend_Lenght field's value is invalid.");
                            }

                            ++pos;

                            str_value.assign(recv_data.recive_buffer + it->send_offset + content_length_header_offset + pos,
                                             content_length_header_len - pos);

                            size_t pos_end = str_value.find_last_of('\r');
                            if (pos_end != std::string::npos)
                            {
                                str_value = str_value.erase(pos_end);
                            }

                            for (pos = 0; !isgraph(str_value[pos]) && str_value[pos] != '\0'; ++pos)
                                ;

                            str_value = str_value.substr(pos);

                            try
                            {
                                content_len_value = boost::lexical_cast<size_t>(str_value.c_str());
                            }
                            catch (const boost::bad_lexical_cast& ex)
                            {
                                throw std::string("Contend_Lenght field's value is invalid.");
                            }
                            //send_rest_size recalc
                            //set whole http header's length + Content_Length's value
                            it->send_rest_size = header_offset + header_offset_len + cr_lf_len + content_len_value;
                        }
                        //serch Content_Length result is NG
                        else
                        {
                            //send_rest_size recalc
                            //set whole http header's length
                            it->send_rest_size = header_offset + header_offset_len + cr_lf_len;
                        }
                    }
                    //serch http header result is NG
                    else
                    {
                        it->unsend_size += request_data_remain_size;
                        request_data_remain_size = 0;
                        break;
                    }
                }
                //check method and version result is NG
                else if (check_result == CHECK_NG)
                {
                    //send_rest_size recalc
                    it->send_rest_size = it->unsend_size + request_data_remain_size;
                }
                //check method and version result is CHECK_INPOSSIBLE
                else
                {
                    it->unsend_size += request_data_remain_size;
                    request_data_remain_size = 0;
                    break;
                }

                //recalc fields value according to send_rest_size and request rest size
                if (it->send_rest_size > it->unsend_size + request_data_remain_size)
                {
                    it->send_possible_size = it->unsend_size+ request_data_remain_size;
                    it->send_rest_size -= (it->unsend_size + request_data_remain_size);
                    it->send_end_size = 0;
                    it->unsend_size = 0;
                    request_data_remain_size = 0;
                }
                else
                {
                    it->send_possible_size = it->send_rest_size;
                    request_data_remain_size = it->unsend_size + request_data_remain_size - it->send_rest_size;
                    it->send_end_size = 0;
                    it->unsend_size = 0;
                    it->send_rest_size = 0;
                }

                //change status from SEND_NG to SEND_OK
                it->status = SEND_OK;
            }
            //no request rest data to process
            if (request_data_remain_size <= 0)
            {
                break;
            }
        }
        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == getloglevel())
        {
            putLogDebug(10005, "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                        "handle_realserver_recv() : step3--send status list loop end.", __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/
        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == getloglevel())
        {
            putLogDebug(10005, "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                        "handle_realserver_recv() : step4--new send status create procession start.", __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/
        //there are still rest data need to prosess
        //new status created and add to status list
        while (request_data_remain_size > 0)
        {
            //new status created
            send_status new_send_state;
            new_send_state.edit_division = EDIT_DIVISION_NO_EDIT;
            new_send_state.send_end_size = 0;
            new_send_state.send_offset = 0;
            new_send_state.send_possible_size = 0;
            new_send_state.unsend_size = 0;
            new_send_state.send_rest_size = 0;
            //status initialize to SEND_NG
            new_send_state.status = SEND_NG;
            //add new status to status_list
            recv_data.send_status_list.push_back(new_send_state);
            std::list<send_status>::reverse_iterator new_send_it = recv_data.send_status_list.rbegin();
            //cacl offset
            new_send_it->send_offset = recv_data.recive_buffer_max_size - recv_data.recive_buffer_rest_size
                                       - request_data_remain_size;

            //check http method
            check_result = check_status_code(recv_data.recive_buffer + new_send_it->send_offset,
                                             request_data_remain_size);
            /*-------- DEBUG LOG --------*/
            if (LOG_LV_DEBUG == getloglevel())
            {
                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                        "handle_realserver_recv() : call check_http_method : "
                                        "return_value = %d.");
                formatter % check_result;
                putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/
            //check http method resilt is OK
            if (check_result == CHECK_OK)
            {
                //check http version
                check_result = check_http_version(recv_data.recive_buffer + new_send_it->send_offset,
                                                  request_data_remain_size);
                /*-------- DEBUG LOG --------*/
                if (LOG_LV_DEBUG == getloglevel())
                {
                    boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                            "handle_realserver_recv() : call check_http_version : "
                                            "return_value = %d.");
                    formatter % check_result;
                    putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                }
                /*------DEBUG LOG END------*/
            }
            //check http method and version resilt is OK
            if (check_result == CHECK_OK)
            {
                //search whole http header, get whole http header's offset and length
                bret = find_http_header(recv_data.recive_buffer + new_send_it->send_offset, request_data_remain_size,
                                        http_header, header_offset, header_offset_len);
                /*-------- DEBUG LOG --------*/
                if (LOG_LV_DEBUG == getloglevel())
                {
                    boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                            "handle_realserver_recv() : call find_http_header : "
                                            "return_value = %d.");
                    formatter % static_cast<int>(bret);
                    putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                }
                /*------DEBUG LOG END------*/
                //searched whole http header
                if (bret)
                {
                    //search ContentLength http header, get ContentLength header's offset and length
                    bret = find_http_header(recv_data.recive_buffer + new_send_it->send_offset,
                                            request_data_remain_size, content_header, content_length_header_offset, content_length_header_len);
                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                "handle_realserver_recv() : call find_http_header : "
                                                "return_value = %d.");
                        formatter % static_cast<int>(bret);
                        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/

                    //searched ContentLength http header
                    if (bret)
                    {
                        //Get Content_Length header's numeric value
                        for (pos = 0; recv_data.recive_buffer[new_send_it->send_offset + content_length_header_offset + pos] != ':'
                                && pos < content_length_header_len; ++pos)
                            ;
                        if (pos == content_length_header_len)
                        {
                            throw std::string("Contend_Lenght field's value is invalid.");
                        }

                        ++pos;

                        str_value.assign(recv_data.recive_buffer + new_send_it->send_offset + content_length_header_offset + pos,
                                         content_length_header_len - pos);

                        size_t pos_end = str_value.find_last_of('\r');
                        if (pos_end != std::string::npos)
                        {
                            str_value = str_value.erase(pos_end);
                        }

                        for (pos = 0; !isgraph(str_value[pos]) && str_value[pos] != '\0'; ++pos)
                            ;

                        str_value = str_value.substr(pos);
                        try
                        {
                            content_len_value = boost::lexical_cast<size_t>(str_value.c_str());
                        }
                        catch (const boost::bad_lexical_cast& ex)
                        {
                            throw std::string("Contend_Lenght field's value is invalid.");
                        }
                        //send_rest_size recalc
                        //set whole http header's  + whole http header's length + Content_Length's value
                        new_send_it->send_rest_size = header_offset + header_offset_len + cr_lf_len + content_len_value;
                    }
                    //not searched ContentLength http header
                    else
                    {
                        //send_rest_size recalc
                        //set whole http header's  + whole http header's length
                        new_send_it->send_rest_size = header_offset + header_offset_len + cr_lf_len;
                    }
                }
                //not searched whole http header
                else
                {
                    new_send_it->unsend_size = request_data_remain_size;
                    request_data_remain_size = 0;
                    break;
                }
            }
            //check http method or version result is NG
            else if (check_result == CHECK_NG)
            {
                //送信データ残サイズにリクエストデータ残サイズを設定する
                new_send_it->send_rest_size = request_data_remain_size;
            }
            //check http method or version result is NG
            else
            {
                new_send_it->unsend_size = request_data_remain_size;
                request_data_remain_size = 0;
                break;
            }

            //recalc fields value according to send_rest_size and request rest size
            if (new_send_it->send_rest_size > request_data_remain_size)
            {
                new_send_it->send_possible_size = request_data_remain_size;
                new_send_it->send_rest_size -= request_data_remain_size;
                new_send_it->send_end_size = 0;
                new_send_it->send_end_size = 0;
                request_data_remain_size = 0;
            }
            else
            {
                new_send_it->send_possible_size = new_send_it->send_rest_size;
                request_data_remain_size -= new_send_it->send_rest_size;
                new_send_it->send_end_size = 0;
                new_send_it->send_rest_size = 0;
            }

            //change status from SEND_NG to SEND_OK
            new_send_it->status = SEND_OK;
        }
        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == getloglevel())
        {
            putLogDebug(10005, "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                        "handle_realserver_recv() : step4--new send status create procession end.", __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        //search for send_possbile item in status list
        send_status_it it_find = find_if(recv_data.send_status_list.begin(), recv_data.send_status_list.end(),
                                         data_send_possible());
        //send_possbile item is exist
        if (it_find != recv_data.send_status_list.end())
        {
            status = CLIENT_CONNECTION_CHECK;
        }
        //send_possbile item is not exist
        else
        {
            status = REALSERVER_RECV;
        }
    }
    catch (int e)
    {
        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == getloglevel())
        {
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_realserver_recv() : catch exception e = %d.");
            formatter % e;
            putLogError(17000, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/
        status = FINALIZE;
    }
    catch (const std::string& ex)
    {
        std::cerr << "handle_realserver_recv exception : " << ex << std::endl;
        putLogError(17000, ex.c_str(), __FILE__, __LINE__ );
        status = FINALIZE;
    }
    catch (const std::bad_alloc&)
    {
        std::cerr << "handle_realserver_recv exception : Could not allocate memory." << std::endl;
        putLogError(17000, "Could not allocate memory.", __FILE__, __LINE__ );
        status = FINALIZE;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "handle_realserver_recv exception : error =" << ex.what() << "." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_realserver_recv() : exception : error = %s.");
        formatter % ex.what();
        putLogError(17000, formatter.str(), __FILE__, __LINE__ );

        status = FINALIZE;
    }
    catch (...)
    {
        std::cerr << "protocol_module_sessionless::handle_realserver_recv() : Unknown exception." << std::endl;
        putLogError(17000, "function : protocol_module_base::check_message_result "
                    "protocol_module_sessionless::handle_realserver_recv() : "
                    "Unknown exception.", __FILE__, __LINE__ );
        status = FINALIZE;
    }

    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_realserver_recv(const boost::thread::id thread_id, "
                                "const boost::asio::ip::tcp::endpoint& rs_endpoint, "
                                "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                "const size_t recvlen) : return_value = %d.");
        formatter % status;
        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    return status;
}



//! called from after sorryserver recive
//! @param[in]	downstream thread id
//! @param[in]	sorryserver endpoint reference
//! @param[in]	recive from realserver buffer reference.
//! @param[in]	recv data length
//! @return 	session use EVENT mode
protocol_module_base::EVENT_TAG protocol_module_sessionless::handle_sorryserver_recv(
    const boost::thread::id thread_id, const boost::asio::ip::tcp::endpoint& sorry_endpoint, const boost::array<
    char, MAX_BUFFER_SIZE>& recvbuffer, const size_t recvlen)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_sorryserver_recv(const boost::thread::id thread_id, "
                                "const boost::asio::ip::tcp::endpoint& sorry_endpoint, "
                                "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                "const size_t recvlen) : thread_id = %d, sorry_endpoint = [%s]:%d, recvbuffer = %s, recvlen = %d.");
        formatter % thread_id % sorry_endpoint.address().to_string() % sorry_endpoint.port()
        % recvbuffer.data() % recvlen;
        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    EVENT_TAG status;
    size_t data_remain_start = 0;
    size_t data_remain_size = 0;
    size_t request_data_remain_size = 0;
    size_t header_offset = 0;
    size_t header_offset_len = 0;
    size_t content_length_header_offset = 0;
    size_t content_length_header_len = 0;
    size_t content_len_value = 0;
    size_t pos = 0;
    const size_t cr_lf_len = strlen("\r\n\r\n");
    std::string str_value;
    const std::string http_header = "";
    const std::string content_header = "Content-Length";
    session_thread_data_sessionless* session_data = NULL;
    char* buffer1 = NULL;
    char* buffer2 = NULL;
    bool bret = false;
    CHECK_RESULT_TAG check_result;
    session_thread_data_map_it session_thread_it;
    recive_data_map_it recive_data_it;

    //parameter check
    if (recvlen > recvbuffer.size())
    {
        std::cerr << "handle_sorryserver_recv() : Data size bigger than buffer size." << std::endl;
        putLogError(17000, "Data size bigger than buffer size.", __FILE__,
                    __LINE__ );
        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == getloglevel())
        {
            boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_sorryserver_recv(const boost::thread::id thread_id, "
                                    "const boost::asio::ip::tcp::endpoint& rs_endpoint, "
                                    "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                    "const size_t recvlen) : return_value = %d.");
            formatter % FINALIZE;
            putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/
        return FINALIZE;
    }

    try
    {
        {
            boost::mutex::scoped_lock slock(session_thread_data_map_mutex);

            session_thread_it = session_thread_data_map.find(thread_id);
            if (session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL)
            {
                putLogError(17000, "Invalid thread id.", __FILE__, __LINE__ );
                throw -1;
            }

            session_data = session_thread_it->second;
        }

        recive_data_it = session_data->recive_data_map.find(session_data->target_endpoint);
        if (recive_data_it == session_data->recive_data_map.end())
        {
            putLogError(17000, "Invalid endpoint.", __FILE__, __LINE__ );
            throw -1;
        }

        recive_data& recv_data = session_data->recive_data_map[session_data->target_endpoint];

        //status list check
        send_status_it it = recv_data.send_status_list.begin();
        send_status_it it_end = recv_data.send_status_list.end();
        it = std::find_if(it, it_end, data_send_ok());
        if (it != it_end)
        {
            putLogError(17000, "Sending data is invalid.", __FILE__, __LINE__ );
            throw -1;
        }

        //status list check
        it = recv_data.send_status_list.begin();
        it = std::adjacent_find(it, it_end, data_send_repeated());
        if (it != it_end)
        {
            putLogError(17000, "Sending data is invalid.", __FILE__, __LINE__ );
            throw -1;
        }

        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == getloglevel())
        {
            putLogDebug(10005, "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                        "handle_sorryserver_recv() : step1--send status list loop start.", __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        it = recv_data.send_status_list.begin();
        //get original status info
        while (it != it_end)
        {
            //item status is SEND_END
            if (it->status == SEND_END)
            {
                //erase from list
                recv_data.send_status_list.erase(it++);
                continue;
            }
            //item status is SEND_CONTINUE
            else if (it->status == SEND_CONTINUE)
            {
                it->send_offset += it->send_end_size;
                data_remain_start = it->send_offset;
                break;
            }
            //item status is SEND_NG
            else
            {
                data_remain_start = it->send_offset;
                data_remain_size = it->unsend_size;
                break;
            }

            ++it;
        }
        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == getloglevel())
        {
            putLogDebug(10005, "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                        "handle_sorryserver_recv() : step1--send status list loop end.", __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == getloglevel())
        {
            putLogDebug(10005, "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                        "handle_realserver_recv() : step2--data buffer size process start.", __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/
        //recive buffer process
        //buffer rest size　＜　request size
        if (recv_data.recive_buffer_rest_size < recvlen)
        {
            //buffer max size　＜　remain size　＋　request size
            //buffer is need reallcate
            if (recv_data.recive_buffer_max_size < data_remain_size + recvlen)
            {
                //first recive
                if (recv_data.recive_buffer == NULL)
                {
                    //recive_buffer1's memery allocate and initialization
                    recv_data.recive_buffer1 = new char[MAX_BUFFER_SIZE];
                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        boost::format formatter("new : address = &(%d), size = %lu.");
                        formatter % static_cast<void*>(recv_data.recive_buffer1) % (MAX_BUFFER_SIZE);
                        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*-------- DEBUG LOG END--------*/
                    memset(recv_data.recive_buffer1, 0, MAX_BUFFER_SIZE);
                    //recive_buffer2's memery allocate and initialization
                    recv_data.recive_buffer2 = new char[MAX_BUFFER_SIZE];
                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        boost::format formatter("new : address = &(%d), size = %lu.");
                        formatter % static_cast<void*>(recv_data.recive_buffer2) % (MAX_BUFFER_SIZE);
                        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*-------- DEBUG LOG END--------*/
                    memset(recv_data.recive_buffer2, 0, MAX_BUFFER_SIZE);
                    //set uisng buffer recive_buffer1 for defalut
                    recv_data.recive_buffer = recv_data.recive_buffer1;
                    //set max buffer size MAX_BUFFER_SIZE
                    recv_data.recive_buffer_max_size = MAX_BUFFER_SIZE;
                }
                //not first recive
                else
                {
                    //recive_buffer1's memery allocate and initialization
                    buffer1 = new char[data_remain_size + recvlen];
                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        boost::format formatter("new : address = &(%d), size = %lu.");
                        formatter % static_cast<void*>(buffer1) % (data_remain_size + recvlen);
                        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*-------- DEBUG LOG --------*/
                    memset(buffer1, 0, data_remain_size + recvlen);
                    //recive_buffer2's memery allocate and initialization
                    buffer2 = new char[data_remain_size + recvlen];
                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        boost::format formatter("new : address = &(%d), size = %lu.");
                        formatter % static_cast<void*>(buffer2) % (data_remain_size + recvlen);
                        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*-------- DEBUG LOG END--------*/
                    memset(buffer2, 0, data_remain_size + recvlen);

                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        std::string datadump;
                        dump_memory(recv_data.recive_buffer + data_remain_start, data_remain_size, datadump);
                        boost::format formatter(
                            "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                            "handle_realserver_recv() : before memcpy (data dump) : "
                            "data begin = %d, data_size = %d, data = %s.");
                        formatter % data_remain_start % data_remain_size % datadump;
                        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                    //copy data from old buffer to new buffer
                    memcpy(buffer1, recv_data.recive_buffer + data_remain_start, data_remain_size);
                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        std::string datadump;
                        dump_memory(buffer1, data_remain_size, datadump);
                        boost::format formatter(
                            "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                            "handle_realserver_recv() : after memcpy (data dump) :  "
                            "data begin = 0, data_size = %d, data = %s.");
                        formatter % recvlen % datadump;
                        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/

                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        std::string datadump;
                        dump_memory(recvbuffer.data(), recvlen, datadump);
                        boost::format formatter(
                            "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                            "handle_realserver_recv() : before memcpy (data dump) :  "
                            "data begin = 0, data_size = %d, data = %s.");
                        formatter % recvlen % datadump;
                        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                    memcpy(buffer1 + data_remain_size, recvbuffer.data(), recvlen);
                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        std::string datadump;
                        dump_memory(buffer1 + data_remain_size, data_remain_size, datadump);
                        boost::format formatter(
                            "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                            "handle_realserver_recv() : after memcpy (data dump) :  "
                            "data begin = %d, data_size = %d, data = %s.");
                        formatter % data_remain_size % recvlen % datadump;
                        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                    //free old buffer1 and old buffer2
                    if (recv_data.recive_buffer1 != NULL)
                    {
                        /*-------- DEBUG LOG --------*/
                        if (LOG_LV_DEBUG == getloglevel())
                        {
                            boost::format formatter("delete : address = &(%d).");
                            formatter % static_cast<void*>(recv_data.recive_buffer1);
                            putLogDebug(10005, formatter.str(), __FILE__,
                                        __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        delete[] recv_data.recive_buffer1;
                        recv_data.recive_buffer1 = NULL;
                    }

                    if (recv_data.recive_buffer2 != NULL)
                    {
                        /*-------- DEBUG LOG --------*/
                        if (LOG_LV_DEBUG == getloglevel())
                        {
                            boost::format formatter("delete : address = &(%d).");
                            formatter % static_cast<void*>(recv_data.recive_buffer2);
                            putLogDebug(10005, formatter.str(), __FILE__,
                                        __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        delete[] recv_data.recive_buffer2;
                        recv_data.recive_buffer2 = NULL;
                    }

                    //set new buffer pointer
                    recv_data.recive_buffer1 = buffer1;
                    recv_data.recive_buffer2 = buffer2;
                    recv_data.recive_buffer = recv_data.recive_buffer1;
                    //set new buffer's max size
                    recv_data.recive_buffer_max_size = data_remain_size + recvlen;
                }
            }
            //buffer's max size　≧　remain data size　＋　requst size
            //buffer isn't need reallcate, but switch
            else
            {
                //pointer valid check
                if (recv_data.recive_buffer1 == NULL || recv_data.recive_buffer2 == NULL)
                {
                    putLogError(17000, "Invalid pointer.", __FILE__, __LINE__ );
                    throw -1;
                }
                //using buffer is buffer1
                if (recv_data.recive_buffer == recv_data.recive_buffer1)
                {
                    //buffer2 initialization
                    memset(recv_data.recive_buffer2, 0, recv_data.recive_buffer_max_size);
                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        std::string datadump;
                        dump_memory(recv_data.recive_buffer + data_remain_start, data_remain_size, datadump);
                        boost::format formatter(
                            "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                            "handle_realserver_recv() : before memcpy (data dump) : "
                            "data begin = %d, data_size = %d, data = %s.");
                        formatter % data_remain_start % data_remain_size % datadump;
                        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                    //copy data from buffer1 to buffer2
                    memcpy(recv_data.recive_buffer2, recv_data.recive_buffer + data_remain_start, data_remain_size);
                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        std::string datadump;
                        dump_memory(recv_data.recive_buffer2, data_remain_size, datadump);
                        boost::format formatter(
                            "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                            "handle_realserver_recv() : after memcpy (data dump) :  "
                            "data begin = 0, data_size = %d, data = %s.");
                        formatter % recvlen % datadump;
                        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        std::string datadump;
                        dump_memory(recvbuffer.data(), recvlen, datadump);
                        boost::format formatter(
                            "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                            "handle_realserver_recv() : before memcpy (data dump) :  "
                            "data begin = 0, data_size = %d, data = %s.");
                        formatter % recvlen % datadump;
                        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                    memcpy(recv_data.recive_buffer2 + data_remain_size, recvbuffer.data(), recvlen);
                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        std::string datadump;
                        dump_memory(recv_data.recive_buffer2 + data_remain_size, data_remain_size, datadump);
                        boost::format formatter(
                            "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                            "handle_realserver_recv() : after memcpy (data dump) :  "
                            "data begin = %d, data_size = %d, data = %s.");
                        formatter % data_remain_size % recvlen % datadump;
                        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                    //set buffer2 for using buffer
                    recv_data.recive_buffer = recv_data.recive_buffer2;
                }
                //using buffer is buffer2
                else
                {
                    //buffer1 initializtion
                    memset(recv_data.recive_buffer1, 0, recv_data.recive_buffer_max_size);
                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        std::string datadump;
                        dump_memory(recv_data.recive_buffer + data_remain_start, data_remain_size, datadump);
                        boost::format formatter(
                            "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                            "handle_realserver_recv() : before memcpy (data dump) : "
                            "data begin = %d, data_size = %d, data = %s.");
                        formatter % data_remain_start % data_remain_size % datadump;
                        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                    //copy data from buffer2 to buffer1
                    memcpy(recv_data.recive_buffer1, recv_data.recive_buffer + data_remain_start, data_remain_size);
                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        std::string datadump;
                        dump_memory(recv_data.recive_buffer1, data_remain_size, datadump);
                        boost::format formatter(
                            "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                            "handle_realserver_recv() : after memcpy (data dump) :  "
                            "data begin = 0, data_size = %d, data = %s.");
                        formatter % recvlen % datadump;
                        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/

                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        std::string datadump;
                        dump_memory(recvbuffer.data(), recvlen, datadump);
                        boost::format formatter(
                            "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                            "handle_realserver_recv() : before memcpy (data dump) :  "
                            "data begin = 0, data_size = %d, data = %s.");
                        formatter % recvlen % datadump;
                        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                    memcpy(recv_data.recive_buffer1 + data_remain_size, recvbuffer.data(), recvlen);
                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        std::string datadump;
                        dump_memory(recv_data.recive_buffer1 + data_remain_size, data_remain_size, datadump);
                        boost::format formatter(
                            "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                            "handle_realserver_recv() : after memcpy (data dump) :  "
                            "data begin = %d, data_size = %d, data = %s.");
                        formatter % data_remain_size % recvlen % datadump;
                        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                    //set buffer1 for using buffer
                    recv_data.recive_buffer = recv_data.recive_buffer1;
                }
            }

            //set buffer's rest size
            recv_data.recive_buffer_rest_size = recv_data.recive_buffer_max_size - data_remain_size - recvlen;

            //remain_size recalc
            data_remain_size += recvlen;

            send_status_it it_begin = recv_data.send_status_list.begin();
            send_status_it it_end = recv_data.send_status_list.end();
            //offset recalc
            for (; it_begin != it_end; ++it_begin)
            {
                it->send_offset -= data_remain_start;
            }
        }
        //buffer's rest size　≧　request size
        //copy directly
        else
        {
            //pointer valid check
            if (recv_data.recive_buffer == NULL)
            {
                putLogError(17000, "Invalid pointer.", __FILE__, __LINE__ );
                throw -1;
            }
            /*-------- DEBUG LOG --------*/
            if (LOG_LV_DEBUG == getloglevel())
            {
                std::string datadump;
                dump_memory(recvbuffer.data(), recvlen, datadump);
                boost::format formatter(
                    "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                    "handle_realserver_recv() : before memcpy (data dump) :  "
                    "data begin = 0, data_size = %d, data = %s.");
                formatter % recvlen % datadump;
                putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/

            //copy data from paramenter to using buffer
            memcpy(recv_data.recive_buffer + recv_data.recive_buffer_max_size - recv_data.recive_buffer_rest_size,
                   recvbuffer.data(), recvlen);
            /*-------- DEBUG LOG --------*/
            if (LOG_LV_DEBUG == getloglevel())
            {
                std::string datadump;
                dump_memory(recv_data.recive_buffer + recv_data.recive_buffer_max_size - recv_data.recive_buffer_rest_size, data_remain_size, datadump);
                boost::format formatter(
                    "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                    "handle_realserver_recv() : after memcpy (data dump) :  "
                    "data begin = %d, data_size = %d, data = %s.");
                formatter % (recv_data.recive_buffer_max_size - recv_data.recive_buffer_rest_size) % recvlen % datadump;
                putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/
            //buffer's rest size recalc
            recv_data.recive_buffer_rest_size -= recvlen;
            //remain data size recalc
            data_remain_size += recvlen;
        }
        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == getloglevel())
        {
            putLogDebug(10005, "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                        "handle_realserver_recv() : step2--data buffer size process end.", __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/
        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == getloglevel())
        {
            putLogDebug(10005, "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                        "handle_realserver_recv() : step3--send status list loop start.", __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/
        it = recv_data.send_status_list.begin();
        it_end = recv_data.send_status_list.end();
        //request rest size initialization
        request_data_remain_size = recvlen;
        //original status process
        for (; it != it_end; ++it)
        {
            //status is SEND_CONTINUE
            if (it->status == SEND_CONTINUE)
            {
                //
                if (it->send_rest_size > request_data_remain_size)
                {
                    it->send_possible_size = request_data_remain_size;
                    it->send_rest_size -= request_data_remain_size;
                    it->send_end_size = 0;
                    request_data_remain_size = 0;
                }
                else
                {
                    it->send_possible_size = it->send_rest_size;
                    request_data_remain_size -= it->send_rest_size;
                    it->send_end_size = 0;
                    it->send_rest_size = 0;
                }

                //change status from SEND_CONTINUE to SEND_OK
                it->status = SEND_OK;
            }
            //status is SEND_NG
            else if (it->status == SEND_NG)
            {
                //check http method
                check_result = check_status_code(recv_data.recive_buffer + it->send_offset, data_remain_size);
                /*-------- DEBUG LOG --------*/
                if (LOG_LV_DEBUG == getloglevel())
                {
                    boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                            "handle_realserver_recv() : call check_status_code : "
                                            "return_value = %d.");
                    formatter % check_result;
                    putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                }
                /*------DEBUG LOG END------*/
                //check http method result is OK
                if (check_result == CHECK_OK)
                {
                    //check http version
                    check_result = check_http_version(recv_data.recive_buffer + it->send_offset, data_remain_size);
                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                "handle_realserver_recv() : call check_http_version : "
                                                "return_value = %d.");
                        formatter % check_result;
                        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                }
                //check method and version result is OK
                if (check_result == CHECK_OK)
                {
                    //serch http header
                    bret = find_http_header(recv_data.recive_buffer + it->send_offset, data_remain_size, http_header,
                                            header_offset, header_offset_len);
                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                "handle_realserver_recv() : call find_http_header : "
                                                "return_value = %d.");
                        formatter % static_cast<int>(bret);
                        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                    //serch http header result is OK
                    if (bret)
                    {
                        //serch Content_Length header
                        bret = find_http_header(recv_data.recive_buffer + it->send_offset, data_remain_size,
                                                content_header, content_length_header_offset, content_length_header_len);
                        /*-------- DEBUG LOG --------*/
                        if (LOG_LV_DEBUG == getloglevel())
                        {
                            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                    "handle_realserver_recv() : call find_http_header : "
                                                    "return_value = %d.");
                            formatter % static_cast<int>(bret);;
                            putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        //serch Content_Length result is OK
                        if (bret)
                        {
                            //Get Content_Length header's numeric value
                            for (pos = 0; recv_data.recive_buffer[it->send_offset + content_length_header_offset + pos] != ':' && pos
                                    < content_length_header_len; ++pos)
                                ;
                            if (pos == content_length_header_len)
                            {
                                throw std::string("Contend_Lenght field's value is invalid.");
                            }

                            ++pos;

                            str_value.assign(recv_data.recive_buffer + it->send_offset + content_length_header_offset + pos,
                                             content_length_header_len - pos);

                            size_t pos_end = str_value.find_last_of('\r');
                            if (pos_end != std::string::npos)
                            {
                                str_value = str_value.erase(pos_end);
                            }

                            for (pos = 0; !isgraph(str_value[pos]) && str_value[pos] != '\0'; ++pos)
                                ;

                            str_value = str_value.substr(pos);

                            try
                            {
                                content_len_value = boost::lexical_cast<size_t>(str_value.c_str());
                            }
                            catch (const boost::bad_lexical_cast& ex)
                            {
                                throw std::string("Contend_Lenght field's value is invalid.");
                            }
                            //send_rest_size recalc
                            //set whole http header's length + Content_Length's value
                            it->send_rest_size = header_offset + header_offset_len + cr_lf_len + content_len_value;
                        }
                        //serch Content_Length result is NG
                        else
                        {
                            //send_rest_size recalc
                            //set whole http header's length
                            it->send_rest_size = header_offset + header_offset_len + cr_lf_len;
                        }
                    }
                    //serch http header result is NG
                    else
                    {
                        it->unsend_size += request_data_remain_size;
                        request_data_remain_size = 0;
                        break;
                    }
                }
                //check method or version result is NG
                else if (check_result == CHECK_NG)
                {
                    it->send_rest_size = it->unsend_size+request_data_remain_size;
                }
                //check method and version result is CHECK_INPOSSIBLE
                else
                {
                    it->unsend_size += request_data_remain_size;
                    request_data_remain_size = 0;
                    break;
                }

                //recalc fields value according to send_rest_size and request rest size
                if (it->send_rest_size > it->unsend_size + request_data_remain_size)
                {
                    it->send_possible_size = it->unsend_size + request_data_remain_size;
                    it->send_rest_size -= (it->unsend_size + request_data_remain_size);
                    it->send_end_size = 0;
                    it->unsend_size = 0;
                    request_data_remain_size = 0;
                }
                else
                {
                    it->send_possible_size = it->send_rest_size;
                    request_data_remain_size = it->unsend_size + request_data_remain_size - it->send_rest_size;
                    it->send_end_size = 0;
                    it->unsend_size = 0;
                    it->send_rest_size = 0;
                }

                //change status from SEND_NG to SEND_OK
                it->status = SEND_OK;
            }
            //no request rest data to process
            if (request_data_remain_size <= 0)
            {
                break;
            }
        }
        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == getloglevel())
        {
            putLogDebug(10005, "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                        "handle_realserver_recv() : step3--send status list loop end.", __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/
        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == getloglevel())
        {
            putLogDebug(10005, "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                        "handle_realserver_recv() : step4--new send status create procession start.", __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        //there are still rest data need to prosess
        //new status created and add to status list
        while (request_data_remain_size > 0)
        {
            //new status created
            send_status new_send_state;
            new_send_state.edit_division = EDIT_DIVISION_NO_EDIT;
            new_send_state.send_end_size = 0;
            new_send_state.send_offset = 0;
            new_send_state.send_possible_size = 0;
            new_send_state.unsend_size = 0;
            new_send_state.send_rest_size = 0;
            //status initialize to SEND_NG
            new_send_state.status = SEND_NG;
            //add new status to status_list
            recv_data.send_status_list.push_back(new_send_state);
            std::list<send_status>::reverse_iterator new_send_it = recv_data.send_status_list.rbegin();
            //cacl offset
            new_send_it->send_offset = recv_data.recive_buffer_max_size - recv_data.recive_buffer_rest_size
                                       - request_data_remain_size;

            //check http method
            check_result = check_status_code(recv_data.recive_buffer + new_send_it->send_offset,
                                             request_data_remain_size);
            /*-------- DEBUG LOG --------*/
            if (LOG_LV_DEBUG == getloglevel())
            {
                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                        "handle_realserver_recv() : call check_status_code : "
                                        "return_value = %d.");
                formatter % check_result;
                putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/
            //check http method resilt is OK
            if (check_result == CHECK_OK)
            {
                //check http version
                check_result = check_http_version(recv_data.recive_buffer + new_send_it->send_offset,
                                                  request_data_remain_size);
                /*-------- DEBUG LOG --------*/
                if (LOG_LV_DEBUG == getloglevel())
                {
                    boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                            "handle_realserver_recv() : call check_http_version : "
                                            "return_value = %d.");
                    formatter % check_result;
                    putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                }
                /*------DEBUG LOG END------*/
            }
            //check http method and version resilt is OK
            if (check_result == CHECK_OK)
            {
                //search whole http header, get whole http header's offset and length
                bret = find_http_header(recv_data.recive_buffer + new_send_it->send_offset, request_data_remain_size,
                                        http_header, header_offset, header_offset_len);
                /*-------- DEBUG LOG --------*/
                if (LOG_LV_DEBUG == getloglevel())
                {
                    boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                            "handle_realserver_recv() : call find_http_header : "
                                            "return_value = %d.");
                    formatter % static_cast<int>(bret);
                    putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                }
                /*------DEBUG LOG END------*/
                //searched whole http header
                if (bret)
                {
                    //search ContentLength http header, get ContentLength header's offset and length
                    bret = find_http_header(recv_data.recive_buffer + new_send_it->send_offset,
                                            request_data_remain_size, content_header, content_length_header_offset, content_length_header_len);
                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                "handle_realserver_recv() : call find_http_header : "
                                                "return_value = %d.");
                        formatter % static_cast<int>(bret);
                        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/

                    //searched ContentLength http header
                    if (bret)
                    {
                        //Get Content_Length header's numeric value
                        for (pos = 0; recv_data.recive_buffer[new_send_it->send_offset + content_length_header_offset + pos] != ':'
                                && pos < content_length_header_len; ++pos)
                            ;
                        if (pos == content_length_header_len)
                        {
                            throw std::string("Contend_Lenght field's value is invalid.");
                        }
                        ++pos;

                        str_value.assign(recv_data.recive_buffer + new_send_it->send_offset + content_length_header_offset + pos,
                                         content_length_header_len - pos);

                        size_t pos_end = str_value.find_last_of('\r');
                        if (pos_end != std::string::npos)
                        {
                            str_value = str_value.erase(pos_end);
                        }

                        for (pos = 0; !isgraph(str_value[pos]) && str_value[pos] != '\0'; ++pos)
                            ;

                        str_value = str_value.substr(pos);
                        try
                        {
                            content_len_value = boost::lexical_cast<size_t>(str_value.c_str());
                        }
                        catch (const boost::bad_lexical_cast& ex)
                        {
                            throw std::string("Contend_Lenght field's value is invalid.");
                        }
                        //send_rest_size recalc
                        //set whole http header's  + whole http header's length + Content_Length's value
                        new_send_it->send_rest_size = header_offset + header_offset_len + cr_lf_len + content_len_value;
                    }
                    //not searched ContentLength http header
                    else
                    {
                        //send_rest_size recalc
                        //set whole http header's  + whole http header's length
                        new_send_it->send_rest_size = header_offset + header_offset_len + cr_lf_len;
                    }
                }
                //not searched whole http header
                else
                {
                    new_send_it->unsend_size = request_data_remain_size;
                    request_data_remain_size = 0;
                    break;
                }
            }
            //check http method or version result is NG
            else if (check_result == CHECK_NG)
            {
                new_send_it->send_rest_size = request_data_remain_size;
            }
            //check http method or version resilt is NG
            else
            {
                new_send_it->unsend_size = request_data_remain_size;
                request_data_remain_size = 0;
                break;
            }

            //recalc fields value according to send_rest_size and request rest size
            if (new_send_it->send_rest_size > request_data_remain_size)
            {
                new_send_it->send_possible_size = request_data_remain_size;
                new_send_it->send_rest_size -= request_data_remain_size;
                new_send_it->send_end_size = 0;
                request_data_remain_size = 0;
            }
            else
            {
                new_send_it->send_possible_size = new_send_it->send_rest_size;
                request_data_remain_size -= new_send_it->send_rest_size;
                new_send_it->send_end_size = 0;
                new_send_it->send_rest_size = 0;
            }

            //change status from SEND_NG to SEND_OK
            new_send_it->status = SEND_OK;
        }
        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == getloglevel())
        {
            putLogDebug(10005, "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                        "handle_realserver_recv() : step4--new send status create procession end.", __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        //search for send_possbile item in status list
        send_status_it it_find = find_if(recv_data.send_status_list.begin(), recv_data.send_status_list.end(),
                                         data_send_possible());
        //send_possbile item is exist
        if (it_find != recv_data.send_status_list.end())
        {
            status = CLIENT_CONNECTION_CHECK;
        }
        //send_possbile item is not exist
        else
        {
            status = SORRYSERVER_RECV;
        }
    }
    catch (int e)
    {
        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == getloglevel())
        {
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_realserver_recv() : catch exception e = %d.");
            formatter % e;
            putLogError(17000, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/
        status = FINALIZE;
    }
    catch (const std::string& ex)
    {
        std::cerr << "handle_sorryserver_recv exception : " << ex << std::endl;
        putLogError(17000, ex.c_str(), __FILE__, __LINE__ );
        status = FINALIZE;
    }
    catch (const std::bad_alloc&)
    {
        std::cerr << "handle_sorryserver_recv exception : Could not allocate memory." << std::endl;
        putLogError(17000, "Could not allocate memory.", __FILE__, __LINE__ );
        status = FINALIZE;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "handle_realserver_recv exception : error =" << ex.what() << "." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_realserver_recv() : exception : error = %s.");
        formatter % ex.what();
        putLogError(17000, formatter.str(), __FILE__, __LINE__ );

        status = FINALIZE;
    }
    catch (...)
    {
        std::cerr << "protocol_module_sessionless::handle_realserver_recv() : Unknown exception." << std::endl;
        putLogError(17000, "function : protocol_module_base::check_message_result "
                    "protocol_module_sessionless::handle_realserver_recv() : "
                    "Unknown exception.", __FILE__, __LINE__ );
        status = FINALIZE;
    }

    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_sorryserver_recv(const boost::thread::id thread_id, "
                                "const boost::asio::ip::tcp::endpoint& rs_endpoint, "
                                "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                "const size_t recvlen) : return_value = %d.");
        formatter % status;
        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    return status;
}

//! called from UPSTEEARM thread. make module original message.
//! @param[in]	downstream thread id.
//! @return 		session use EVENT mode
protocol_module_base::EVENT_TAG protocol_module_sessionless::handle_response_send_inform(
    const boost::thread::id thread_id)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("in/out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_response_send_inform(const boost::thread::id thread_id) : "
                                "return_value = %d.");
        formatter % STOP;
        putLogDebug(10000, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/
    return STOP;
}

//! called from after client connection check. use TCP/IP only. create client send message.
//! @param[in]	downstream thread id
//! @param[out]	send budffer reference
//! @param[out]	send data length
//! @return 	session use EVENT mode
protocol_module_base::EVENT_TAG protocol_module_sessionless::handle_client_connection_check(
    const boost::thread::id thread_id, boost::array<char, MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen)
{

    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_client_connection_check(const boost::thread::id thread_id, "
                                "boost::array<char, MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen) : "
                                "thread_id = %d, sendbuffer = %s, datalen = %d.");
        formatter % thread_id % sendbuffer.data() % datalen;
        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    EVENT_TAG status;
    size_t send_buffer_size = sendbuffer.max_size();
    session_thread_data_sessionless* session_data = NULL;
    session_thread_data_map_it session_thread_it;
    recive_data_map_it recive_data_it;

    try
    {
        {
            boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);

            session_thread_it = session_thread_data_map.find(thread_id);
            if (session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL)
            {
                putLogError(17000, "Invalid thread id.", __FILE__, __LINE__ );
                throw -1;
            }

            session_data = session_thread_it->second;
        }

        recive_data_it = session_data->recive_data_map.find(session_data->target_endpoint);
        if (recive_data_it == session_data->recive_data_map.end())
        {
            putLogError(17000, "Invalid endpoint.", __FILE__, __LINE__ );
            throw -1;
        }

        recive_data& recv_data = recive_data_it->second;

        //get send_possible data
        send_status_it it = find_if(recv_data.send_status_list.begin(), recv_data.send_status_list.end(),
                                    data_send_possible());
        if (it == recv_data.send_status_list.end())
        {
            putLogError(17000, "Sending possible data is not existed.", __FILE__, __LINE__ );
            throw -1;
        }

        //buffer size　≧　sending_possible size
        if (send_buffer_size > it->send_possible_size)
        {
            /*-------- DEBUG LOG --------*/
            if (LOG_LV_DEBUG == getloglevel())
            {
                std::string datadump;
                dump_memory(recv_data.recive_buffer + it->send_offset, it->send_possible_size, datadump);

                boost::format formatter(
                    "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                    "handle_client_connection_check() : before memcpy (data dump) : "
                    "data begin = %d, data_size = %d, data = %s.");
                formatter % it->send_offset % (it->send_possible_size) % datadump;
                putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/
            //copy data from recive_buffer to sendbuffer by sending_possible size
            memcpy(sendbuffer.data(), recv_data.recive_buffer + it->send_offset, it->send_possible_size);
            /*-------- DEBUG LOG --------*/
            if (LOG_LV_DEBUG == getloglevel())
            {
                std::string datadump;
                dump_memory(sendbuffer.data(), it->send_possible_size, datadump);

                boost::format formatter(
                    "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                    "handle_client_connection_check() : after memcpy (data dump) : "
                    "data begin = 0, data_size = %d, data = %s.");
                formatter % (it->send_possible_size) % datadump;
                putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/
            //send_end_size recalc
            it->send_end_size = it->send_possible_size;
            //sending_possible size recalc
            it->send_possible_size = 0;
            //set copyed data length
            datalen = it->send_possible_size;
        }
        //buffer size <　sending_possible size
        else
        {
            /*-------- DEBUG LOG --------*/
            if (LOG_LV_DEBUG == getloglevel())
            {
                std::string datadump;
                dump_memory(recv_data.recive_buffer + it->send_offset, send_buffer_size, datadump);

                boost::format formatter(
                    "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                    "handle_client_connection_check() : before memcpy (data dump) : "
                    "data begin = %d, data_size = %d, data = %s.");
                formatter % it->send_offset % send_buffer_size % datadump;
                putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/
            //copy data from recive_buffer to sendbuffer by buffer size
            memcpy(sendbuffer.data(), recv_data.recive_buffer + it->send_offset, send_buffer_size);
            /*-------- DEBUG LOG --------*/
            if (LOG_LV_DEBUG == getloglevel())
            {
                std::string datadump;
                dump_memory(sendbuffer.data(), send_buffer_size, datadump);

                boost::format formatter(
                    "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                    "handle_client_connection_check() : after memcpy (data dump) : "
                    "data begin = 0, data_size = %d, data = %s.");
                formatter % send_buffer_size % datadump;
                putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/
            //send_end_size recalc
            it->send_end_size = send_buffer_size;
            //sending_possible size recalc
            it->send_possible_size -= send_buffer_size;
            //set copyed data length
            datalen = send_buffer_size;
        }

        status = CLIENT_SEND;
    }
    catch (int e)
    {
        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == getloglevel())
        {
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_client_connection_check() : catch exception e = %d.");
            formatter % e;
            putLogError(17000, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/
        status = FINALIZE;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "handle_client_connection_check exception: error = " << ex.what() << "." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_client_connection_check() : exception : error = %s.");
        formatter % ex.what();
        putLogError(17000, formatter.str(), __FILE__, __LINE__ );
        status = FINALIZE;
    }
    catch (...)
    {
        std::cerr << "protocol_module_sessionless::handle_client_connection_check() : Unknown exception." << std::endl;
        putLogError(17000, "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                    "handle_client_connection_check() : exception : Unknown exception.", __FILE__, __LINE__ );
        status = FINALIZE;
    }

    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_client_connection_check(const boost::thread::id thread_id, "
                                "boost::array<char, MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen)"
                                " : return_value = %d.");
        formatter % status;
        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/
    return status;
}

//! called from after client select. use UDP only
//! @param[in]	downstream thread id
//!	@param[in]	client udp endpoint
//! @param[out]	send buffer reference
//! @param[out]	send data length
//! @return 	session use EVENT mode
protocol_module_base::EVENT_TAG protocol_module_sessionless::handle_client_select(
    const boost::thread::id thread_id, boost::asio::ip::udp::endpoint& cl_endpoint, boost::array<char,
    MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("in/out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_client_select(const boost::thread::id thread_id, "
                                "boost::asio::ip::udp::endpoint& cl_endpoint, "
                                "boost::array<char,MAX_BUFFER_SIZE>& sendbuffer, "
                                "size_t& datalen) : "
                                "return_value = %d.");
        formatter % STOP;
        putLogDebug(10000, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/
    return STOP;
}

//!	called from after client send
//!	@param[in]	downstream thread id
//! @return 	session use EVENT mode
protocol_module_base::EVENT_TAG protocol_module_sessionless::handle_client_send(
    const boost::thread::id thread_id)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_client_send(const boost::thread::id thread_id) : "
                                "thread_id = %d.");
        formatter % thread_id;
        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/
    EVENT_TAG status;
    session_thread_data_sessionless* session_data = NULL;
    session_thread_data_map_it session_thread_it;
    recive_data_map_it recive_data_it;

    try
    {
        {
            boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
            //thread_id check
            session_thread_it = session_thread_data_map.find(thread_id);
            if (session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL)
            {
                putLogError(17000, "Invalid thread id.", __FILE__, __LINE__ );
                throw -1;
            }
            session_data = session_thread_it->second;
        }
        //endpoint check
        recive_data_it = session_data->recive_data_map.find(session_data->target_endpoint);
        if (recive_data_it == session_data->recive_data_map.end())
        {
            putLogError(17000, "Invalid endpoint.", __FILE__, __LINE__ );
            throw -1;
        }

        recive_data& recv_data = recive_data_it->second;

        send_status_it it = recv_data.send_status_list.begin();
        send_status_it it_end = recv_data.send_status_list.end();

        //check status list
        it = std::adjacent_find(it, it_end, data_send_list_incorrect());
        if (it != it_end)
        {
            putLogError(17000, "Sending possible data is invalid.", __FILE__, __LINE__ );
            throw -1;
        }
        //status list check
        it = recv_data.send_status_list.begin();
        it = find_if(it, it_end, data_send_ok());
        if (it == it_end)
        {
            putLogError(17000, "Sending possible data is not existed.", __FILE__, __LINE__ );
            throw -1;
        }

        //sending possible data is exist
        if (it->send_possible_size > 0)
        {
            //status remain SEND_OK
            it->status = SEND_OK;
            //offset recalc
            it->send_offset += it->send_end_size;
            //send_end_size recalc
            it->send_end_size = 0;
        }
        //sending possible data is not exist
        else
        {
            //can recive from clent continue
            if (it->send_rest_size > 0)
            {
                //change status from SEND_OK to SEND_CONTINUE
                it->status = SEND_CONTINUE;
            }
            //can not recive from clent continue
            else
            {
                //change status from SEND_OK to SEND_END
                it->status = SEND_END;
            }
        }

        it = recv_data.send_status_list.begin();
        it = find_if(it, it_end, data_send_ok());
        //send_ok item is exist
        if (it != it_end)
        {
            status = CLIENT_CONNECTION_CHECK;
        }
        //send_ok item is not exist
        else
        {
            //end flag is on
            if (session_data->end_flag == END_FLAG_ON)
            {
                status = CLIENT_DISCONNECT;
            }
            //end flag is off
            else
            {
                //sorry flag is on
                if (session_data->sorry_flag == SORRY_FLAG_ON)
                {
                    status = SORRYSERVER_RECV;
                }
                //sorry flag is off
                else
                {
                    status = REALSERVER_RECV;
                }
            }
        }
    }
    catch (int e)
    {
        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == getloglevel())
        {
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_client_send() : catch exception e = %d.");
            formatter % e;
            putLogError(17000, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/
        status = FINALIZE;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "handle_client_send exception : error = " << ex.what() << "." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_client_send() : exception : error = %s.");
        formatter % ex.what();
        putLogError(17000, formatter.str(), __FILE__, __LINE__ );
        status = FINALIZE;
    }
    catch (...)
    {
        std::cerr << "protocol_module_sessionless::handle_client_send() : Unknown exception." << std::endl;
        putLogError(17000, "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                    "handle_client_send() : exception : Unknown exception.", __FILE__, __LINE__ );
        status = FINALIZE;
    }

    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_client_send(const boost::thread::id thread_id) : return_value = %d.");
        formatter % status;
        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    return status;
}

//! call from client disconnect event. use upstream thread and downstream thread.
//! @param[in]	upstream and downstream thread id( check! one thread one event! )
//! @return 	session use EVENT mode
protocol_module_base::EVENT_TAG protocol_module_sessionless::handle_client_disconnect(
    const boost::thread::id thread_id)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("in/out_function : protocol_module_sessionless::"
                                "handle_client_disconnect(const boost::thread::id thread_id) : return_value = %d.");
        formatter % FINALIZE;
        putLogDebug(10002, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/
    return FINALIZE;
}

//! call from sorry mode event. use upstream thread and downstream thread
//! @param[in]	upstream and downstream thread id( check! one thread one event and first time call pattern )
//! @return 	session use EVENT mode
protocol_module_base::EVENT_TAG protocol_module_sessionless::handle_sorry_enable(
    const boost::thread::id thread_id)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_sorry_enable(const boost::thread::id thread_id) : "
                                "thread_id = %d.");
        formatter % thread_id;
        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    EVENT_TAG status;
    boost::asio::ip::tcp::endpoint endpoint;
    bool send_possible = false;
    bool send_continue = false;
    bool send_disable = false;
    session_thread_data_sessionless* session_data = NULL;
    session_thread_data_map_it session_thread_it;
    recive_data_map_it recive_data_it;

    try
    {
        {
            boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
            //check thread_id
            session_thread_it = session_thread_data_map.find(thread_id);
            if (session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL)
            {
                putLogError(17000, "Invalid thread id.", __FILE__, __LINE__ );
                throw -1;
            }

            session_data = session_thread_it->second;
        }
        //check endpoint
        endpoint = session_data->thread_division == THREAD_DIVISION_UP_STREAM ? session_data->client_endpoint_tcp
                   : session_data->target_endpoint;
        recive_data_it = session_data->recive_data_map.find(endpoint);
        if (recive_data_it == session_data->recive_data_map.end())
        {
            putLogError(17000, "Invalid endpoint.", __FILE__, __LINE__ );
            throw -1;
        }

        recive_data& recv_data = recive_data_it->second;

        //get this thread sending possible data
        send_status_it it = find_if(recv_data.send_status_list.begin(), recv_data.send_status_list.end(),
                                    data_send_possible());
        if (it != recv_data.send_status_list.end())
        {
            send_possible = true;
        }

        it = find_if(recv_data.send_status_list.begin(), recv_data.send_status_list.end(), data_send_continue());
        if (it != recv_data.send_status_list.end())
        {
            send_continue = true;
        }

        it = find_if(recv_data.send_status_list.begin(), recv_data.send_status_list.end(), data_send_disable());
        if (it != recv_data.send_status_list.end())
        {
            send_disable = true;
        }

        //up thread
        if (session_data->thread_division == THREAD_DIVISION_UP_STREAM)
        {
            //accept_end_flag is off
            if (session_data->accept_end_flag == ACCEPT_END_FLAG_OFF)
            {
                //set sorry flag on
                session_data->sorry_flag = SORRY_FLAG_ON;
                /*-------- DEBUG LOG --------*/
                if (LOG_LV_DEBUG == getloglevel())
                {
                    putLogDebug(10005, "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_sorry_enable() : SORRY_FLAG_ON.", __FILE__, __LINE__ );
                }
                /*------DEBUG LOG END------*/
                status = ACCEPT;
            }
            //accept_end_flag is on
            else
            {
                //set sorry flag on
                if (session_data->sorry_flag == SORRY_FLAG_ON)
                {
                    //sending possible data is exist
                    if (send_possible)
                    {
                        status = SORRYSERVER_SELECT;
                    }
                    //sending possible data is not exist
                    else
                    {
                        status = CLIENT_RECV;
                    }
                }
                //set sorry flag off
                else
                {
                    //sending continue data is exist
                    if (send_continue)
                    {
                        //set end flag on
                        session_data->end_flag = END_FLAG_ON;
                        /*-------- DEBUG LOG --------*/
                        if (LOG_LV_DEBUG == getloglevel())
                        {
                            putLogDebug(10005, "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                        "handle_sorry_enable() : END_FLAG_ON.", __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        status = REALSERVER_DISCONNECT;
                    }
                    //sending continue data is not exist
                    else
                    {
                        //set sorryserver_switch_flag on
                        session_data->sorryserver_switch_flag = SORRYSERVER_SWITCH_FLAG_ON;
                        /*-------- DEBUG LOG --------*/
                        if (LOG_LV_DEBUG == getloglevel())
                        {
                            putLogDebug(10005, "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                        "handle_sorry_enable() : SORRYSERVER_SWITCH_FLAG_ON.", __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        //set sorry_flag on
                        session_data->sorry_flag = SORRY_FLAG_ON;
                        /*-------- DEBUG LOG --------*/
                        if (LOG_LV_DEBUG == getloglevel())
                        {
                            putLogDebug(10005, "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                        "handle_sorry_enable() : SORRY_FLAG_ON.", __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        status = REALSERVER_DISCONNECT;
                    }
                }
            }
        }
        //down thread
        else
        {
            //sorry_flag is on
            if (session_data->sorry_flag == SORRY_FLAG_ON)
            {
                //sending possible data is exist
                if (send_possible)
                {
                    status = CLIENT_CONNECTION_CHECK;
                }
                //sending possible data is not exist
                else
                {
                    status = SORRYSERVER_RECV;
                }
            }
            //sorry_flag is off
            else
            {
                //sending possible data is exist or send_rest_size ＞ ０
                if (send_disable)
                {
                    //set end flag on
                    session_data->end_flag = END_FLAG_ON;
                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        putLogDebug(10005, "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_sorry_enable() : END_FLAG_ON.", __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                    status = REALSERVER_DISCONNECT;
                }
                //
                else
                {
                    //set sorry_flag on
                    session_data->sorry_flag = SORRY_FLAG_ON;
                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        putLogDebug(10005, "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_sorry_enable() : SORRY_FLAG_ON.", __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                    //sending possible data is exist
                    if (send_possible)
                    {
                        status = CLIENT_CONNECTION_CHECK;
                    }
                    //sending possible data is not exist
                    else
                    {
                        status = SORRYSERVER_RECV;
                    }
                }
            }
        }
    }
    catch (int e)
    {
        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == getloglevel())
        {
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_sorry_enable() : catch exception e = %d.");
            formatter % e;
            putLogError(17000, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/
        status = FINALIZE;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "handle_sorry_enable exception: error = " << ex.what() << "." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_sorry_enable() : exception : error = %s.");
        formatter % ex.what();
        putLogError(17000, formatter.str(), __FILE__, __LINE__ );
        status = FINALIZE;
    }
    catch (...)
    {
        std::cerr << "protocol_module_sessionless::handle_sorry_enable() Unknown exception." << std::endl;
        putLogError(17000, "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                    "handle_sorry_enable() : exception : Unknown exception.", __FILE__, __LINE__ );
        status = FINALIZE;
    }

    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_sorry_enable(const boost::thread::id thread_id) : return_value = %d.");
        formatter % status;
        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    return status;
}

//! call from sorry mode disable. use upstream thread and downstream thread.
//! @param[in]	upstream and downstream thread id( check! one thread one event )
//! @return 	session use EVENT mode
protocol_module_base::EVENT_TAG protocol_module_sessionless::handle_sorry_disable(
    const boost::thread::id thread_id)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_sorry_disable(const boost::thread::id thread_id) : "
                                "thread_id = %d.");
        formatter % thread_id;
        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/
    EVENT_TAG status;
    boost::asio::ip::tcp::endpoint endpoint;
    bool send_possible = false;
    bool send_disable = false;
    bool send_continue = false;
    session_thread_data_sessionless* session_data = NULL;

    try
    {
        {
            boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
            //check thread_id
            session_thread_data_map_it session_thread_it = session_thread_data_map.find(thread_id);
            if (session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL)
            {
                putLogError(17000, "Invalid thread id.", __FILE__, __LINE__ );
                throw -1;
            }
            //check pointer
            session_data = session_thread_it->second;
        }
        //check endpoint
        endpoint = session_data->thread_division == THREAD_DIVISION_UP_STREAM ? session_data->client_endpoint_tcp
                   : session_data->target_endpoint;
        recive_data_map_it recive_data_it = session_data->recive_data_map.find(endpoint);
        if (recive_data_it == session_data->recive_data_map.end())
        {
            putLogError(17000, "Invalid endpoint.", __FILE__, __LINE__ );
            throw -1;
        }

        recive_data& recv_data = recive_data_it->second;

        //get this thread sending possible data
        send_status_it it = find_if(recv_data.send_status_list.begin(), recv_data.send_status_list.end(),
                                    data_send_possible());
        if (it != recv_data.send_status_list.end())
        {
            send_possible = true;
        }

        //sending possible data is exist or send_rest_size ＞ ０
        it = find_if(recv_data.send_status_list.begin(), recv_data.send_status_list.end(), data_send_disable());
        if (it != recv_data.send_status_list.end())
        {
            send_disable = true;
        }

        //sending continue data is exist
        it = find_if(recv_data.send_status_list.begin(), recv_data.send_status_list.end(), data_send_continue());
        if (it != recv_data.send_status_list.end())
        {
            send_continue = true;
        }

        //up thread
        if (session_data->thread_division == THREAD_DIVISION_UP_STREAM)
        {
            //accept_end_flag is off
            if (session_data->accept_end_flag == ACCEPT_END_FLAG_OFF)
            {
                //set sorry flag off
                session_data->sorry_flag = SORRY_FLAG_OFF;
                /*-------- DEBUG LOG --------*/
                if (LOG_LV_DEBUG == getloglevel())
                {
                    putLogDebug(10005, "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_sorry_disable() : SORRY_FLAG_OFF.", __FILE__, __LINE__ );
                }
                /*------DEBUG LOG END------*/

                status = ACCEPT;
            }
            //accept_end_flag is on
            else
            {
                //sorry flag is on
                if (session_data->sorry_flag == SORRY_FLAG_ON)
                {
                    //sending continue data is exist
                    if (send_continue)
                    {
                        //set end flag on
                        session_data->end_flag = END_FLAG_ON;
                        /*-------- DEBUG LOG --------*/
                        if (LOG_LV_DEBUG == getloglevel())
                        {
                            putLogDebug(10005, "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                        "handle_sorry_disable() : END_FLAG_ON.", __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        status = SORRYSERVER_DISCONNECT;
                    }
                    //sending continue data is not exist
                    else
                    {
                        //set realserver_switch_flag on
                        session_data->realserver_switch_flag = REALSERVER_SWITCH_FLAG_ON;
                        /*-------- DEBUG LOG --------*/
                        if (LOG_LV_DEBUG == getloglevel())
                        {
                            putLogDebug(10005, "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                        "handle_sorry_disable() : REALSERVER_SWITCH_FLAG_ON.", __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        //set sorry_flag off
                        session_data->sorry_flag = SORRY_FLAG_OFF;
                        /*-------- DEBUG LOG --------*/
                        if (LOG_LV_DEBUG == getloglevel())
                        {
                            putLogDebug(10005, "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                        "handle_sorry_disable() : SORRY_FLAG_OFF.", __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        status = SORRYSERVER_DISCONNECT;
                    }
                }
                //sorry flag is off
                else
                {
                    //sending possble data is exsit
                    if (send_possible)
                    {
                        status = REALSERVER_SELECT;
                    }
                    //sending possble data is not exsit
                    else
                    {
                        status = CLIENT_RECV;
                    }
                }
            }
        }
        //down thread
        else
        {
            //sorry_flag is on
            if (session_data->sorry_flag == SORRY_FLAG_ON)
            {
                //sending possible data is exist or send_rest_size ＞ ０
                if (send_disable)
                {
                    //set end flag on
                    session_data->end_flag = END_FLAG_ON;
                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        putLogDebug(10005, "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_sorry_disable() : END_FLAG_ON.", __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                    status = SORRYSERVER_DISCONNECT;
                }
                //
                else
                {
                    //set sorry_flag off
                    session_data->sorry_flag = SORRY_FLAG_OFF;
                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        putLogDebug(10005, "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_sorry_disable() : SORRY_FLAG_OFF.", __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                    //sending possible data is exist
                    if (send_possible)
                    {
                        status = CLIENT_CONNECTION_CHECK;
                    }
                    //sending possible data is not exist
                    else
                    {
                        status = REALSERVER_RECV;
                    }
                }
            }
            //sorry_flag is off
            else
            {
                //sending possible data is exist
                if (send_possible)
                {
                    status = CLIENT_CONNECTION_CHECK;
                }
                //sending possible data is not exist
                else
                {
                    status = REALSERVER_RECV;
                }
            }
        }
    }
    catch (int e)
    {
        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == getloglevel())
        {
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_sorry_disable() : catch exception e = %d.");
            formatter % e;
            putLogError(17000, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/
        status = FINALIZE;
    }
    catch (std::exception& ex)
    {
        std::cerr << "handle_sorry_disable exception : error = " << ex.what() << "." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_sorry_disable() : exception : error = %s.");
        formatter % ex.what();
        putLogError(17000, formatter.str(), __FILE__, __LINE__ );
        status = FINALIZE;
    }
    catch (...)
    {
        std::cerr << "protocol_module_sessionless::handle_sorry_disable() : Unknown exception." << std::endl;
        putLogError(17000, "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                    "handle_sorry_disable() : exception : Unknown exception.", __FILE__, __LINE__ );
        status = FINALIZE;
    }

    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_sorry_disable(const boost::thread::id thread_id) : return_value = %d.");
        formatter % status;
        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    return status;
}

//! call from realserver disconnect. use upstream thread and downstream thread
//! @param[in]	upstream and downstream thread id( check! one thread one event )
//! @param[in]	disconnected realserver endpoint.
//! @return 	session use EVENT mode
protocol_module_base::EVENT_TAG protocol_module_sessionless::handle_realserver_disconnect(
    const boost::thread::id thread_id, const boost::asio::ip::tcp::endpoint & rs_endpoint)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_realserver_disconnect(const boost::thread::id thread_id, const boost::asio::ip::tcp::endpoint & rs_endpoint) : "
                                "thread_id = %d, rs_endpoint = [%s]:%d.");
        formatter % thread_id % rs_endpoint.address().to_string() % rs_endpoint.port();
        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/
    EVENT_TAG status;
    bool possible_flag = false;
    boost::asio::ip::tcp::endpoint endpoint;
    session_thread_data_sessionless* session_data = NULL;

    try
    {
        {
            boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);

            session_thread_data_map_it session_thread_it = session_thread_data_map.find(thread_id);
            if (session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL)
            {
                putLogError(17000, "Invalid thread id.", __FILE__, __LINE__ );
                throw -1;
            }

            session_data = session_thread_it->second;
        }

        recive_data_map_it recive_data_it = session_data->recive_data_map.find(endpoint);
        if (recive_data_it == session_data->recive_data_map.end())
        {
            putLogError(17000, "Invalid endpoint.", __FILE__, __LINE__ );
            throw -1;
        }

        recive_data& recv_data = recive_data_it->second;

        //send_possible item is exist
        send_status_it it = find_if(recv_data.send_status_list.begin(), recv_data.send_status_list.end(),
                                    data_send_possible());
        if (it != recv_data.send_status_list.end())
        {
            possible_flag = true;
        }

        //up thread
        if (session_data->thread_division == THREAD_DIVISION_UP_STREAM)
        {
            //end flag is on
            if (session_data->end_flag == END_FLAG_ON)
            {
                status = CLIENT_RECV;
            }
            //end flag is off
            else
            {
                //sorryserver flag is on
                if (session_data->sorryserver_switch_flag == SORRYSERVER_SWITCH_FLAG_ON)
                {
                    //send_possible data is exist
                    if (possible_flag)
                    {
                        status = SORRYSERVER_SELECT;
                    }
                    //send_possible data is not exist
                    else
                    {
                        status = CLIENT_RECV;
                    }
                }
                //sorryserver flag is off
                else
                {
                    //set end flag on
                    session_data->end_flag = END_FLAG_ON;
                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        putLogDebug(10005, "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_realserver_disconnect() : END_FLAG_ON.", __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                    status = CLIENT_RECV;
                }
            }
        }
        //down thread
        else
        {
            //set end flag on
            session_data->end_flag = END_FLAG_ON;
            /*-------- DEBUG LOG --------*/
            if (LOG_LV_DEBUG == getloglevel())
            {
                putLogDebug(10005, "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                            "handle_realserver_disconnect() : END_FLAG_ON.", __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/
            //send_possible data is exist
            if (possible_flag)
            {
                status = CLIENT_CONNECTION_CHECK;
            }
            //send_possible data is not exist
            else
            {
                status = CLIENT_DISCONNECT;
            }
        }
    }
    catch (int e)
    {
        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == getloglevel())
        {
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_realserver_disconnect() : catch exception e = %d.");
            formatter % e;
            putLogError(17000, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/
        status = FINALIZE;
    }
    catch (std::exception& ex)
    {
        std::cerr << "handle_realserver_disconnect exception: error = " << ex.what() << "." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_realserver_disconnect() : exception : error = %s.");
        formatter % ex.what();
        putLogError(17000, formatter.str(), __FILE__, __LINE__ );
        status = FINALIZE;
    }
    catch (...)
    {
        std::cerr << "protocol_module_sessionless::handle_realserver_disconnect() : Unknown exception." << std::endl;
        putLogError(17000, "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                    "handle_realserver_disconnect() : exception : Unknown exception.", __FILE__, __LINE__ );
        status = FINALIZE;
    }

    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_realserver_disconnect(const boost::thread::id thread_id, "
                                "const boost::asio::ip::tcp::endpoint & rs_endpoint) : return_value = %d.");
        formatter % status;
        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    return status;
}

//! call from sorry server disconnect. use upstraem thread and downstream thread
//! @param[in]	upstream and downstream thread id( check! one thread one event )
//! @param[in]	disconnect sorryserver endpoint
//! @return		session use EVENT mode
//! @return		session use EVENT mode
protocol_module_base::EVENT_TAG protocol_module_sessionless::handle_sorryserver_disconnect(
    const boost::thread::id thread_id, const boost::asio::ip::tcp::endpoint & sorry_endpoint)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format
        formatter(
            "in_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
            "handle_sorryserver_disconnect(const boost::thread::id thread_id, "
            "const boost::asio::ip::tcp::endpoint & sorry_endpoint) : "
            "thread_id = %d, sorry_endpoint = [%s]:%d.");
        formatter % thread_id % sorry_endpoint.address().to_string() % sorry_endpoint.port() ;
        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/
    EVENT_TAG status;
    bool possible_flag = false;
    session_thread_data_sessionless* session_data = NULL;

    try
    {
        {
            boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);

            session_thread_data_map_it session_thread_it = session_thread_data_map.find(thread_id);
            if (session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL)
            {
                putLogError(17000, "Invalid thread id.", __FILE__, __LINE__ );
                throw -1;
            }

            session_data = session_thread_it->second;
        }

        recive_data_map_it recive_data_it = session_data->recive_data_map.find(session_data->target_endpoint);
        if (session_data->recive_data_map.find(session_data->target_endpoint) == session_data->recive_data_map.end())
        {
            putLogError(17000, "Invalid endpoint.", __FILE__, __LINE__ );
            throw -1;
        }

        recive_data& recv_data = recive_data_it->second;

        //send_possible item is exist
        send_status_it it = find_if(recv_data.send_status_list.begin(), recv_data.send_status_list.end(),
                                    data_send_possible());
        if (it != recv_data.send_status_list.end())
        {
            possible_flag = true;
        }

        //up thread
        if (session_data->thread_division == THREAD_DIVISION_UP_STREAM)
        {
            //end flag is on
            if (session_data->end_flag == END_FLAG_ON)
            {
                status = CLIENT_RECV;
            }
            //end flag is off
            else
            {
                //realserver switch flag is on
                if (session_data->realserver_switch_flag == REALSERVER_SWITCH_FLAG_ON)
                {
                    //send_possible data is exist
                    if (possible_flag)
                    {
                        status = REALSERVER_SELECT;
                    }
                    //send_possible data is not exist
                    else
                    {
                        status = CLIENT_RECV;
                    }
                }
                //realserver switch flag is off
                else
                {
                    //set end flag on
                    session_data->end_flag = END_FLAG_ON;
                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        putLogDebug(10005, "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_sorryserver_disconnect() : END_FLAG_ON.", __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                    status = CLIENT_RECV;
                }
            }
        }
        //down thread
        else
        {
            //set end flag on
            session_data->end_flag = END_FLAG_ON;
            /*-------- DEBUG LOG --------*/
            if (LOG_LV_DEBUG == getloglevel())
            {
                putLogDebug(10005, "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                            "handle_sorryserver_disconnect() : END_FLAG_ON.", __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/
            //send_possible data is exist
            if (possible_flag)
            {
                status = CLIENT_CONNECTION_CHECK;
            }
            //send_possible data is not exist
            else
            {
                status = CLIENT_DISCONNECT;
            }
        }
    }
    catch (int e)
    {
        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == getloglevel())
        {
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_sorryserver_disconnect() : catch exception e = %d.");
            formatter % e;
            putLogError(17000, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/
        status = FINALIZE;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "handle_sorryserver_disconnect exception : error = " << ex.what() << "." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_sorryserver_disconnect() : exception : error = %s.");
        formatter % ex.what();
        putLogError(17000, formatter.str(), __FILE__, __LINE__ );
        status = FINALIZE;
    }
    catch (...)
    {
        std::cerr << "protocol_module_sessionless::handle_sorryserver_disconnect() : Unknown exception." << std::endl;
        putLogError(17000, "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                    "handle_sorryserver_disconnect() : exception : Unknown exception.", __FILE__, __LINE__ );
        status = FINALIZE;
    }

    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format
        formatter(
            "out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
            "handle_sorryserver_disconnect(const boost::thread::id thread_id, "
            "const boost::asio::ip::tcp::endpoint& sorry_endpoint) : return_value = %d.");
        formatter % status;
        putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    return status;
}

//! call from realserver disconnect. use upstream thread and downstream thread.
//! @param[in]	upstream and downstream thread id( check! one thread one event )
//! @param[in]	disconnect realserver endpoint
//! @return		session use EVENT mode.
protocol_module_base::EVENT_TAG protocol_module_sessionless::handle_realserver_close(
    const boost::thread::id thread_id, const boost::asio::ip::udp::endpoint & rs_endpoint)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("in/out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_realserver_close(const boost::thread::id thread_id, "
                                "const boost::asio::ip::udp::endpoint & rs_endpoint) : "
                                "return_value = %d.");
        formatter % STOP;
        putLogDebug(10000, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/
    return STOP;
}

}
