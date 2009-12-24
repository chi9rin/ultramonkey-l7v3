/*
 * @file  protocol_module_sslid.cpp
 * @brief protocol module of HTTPS(SSL/TLS).
 * @brief this module provide session persistence by SSL session ID.
 *
 * L7VSD: Linux Virtual Server for Layer7 Load Balancing
 * Copyright (C) 2009  NTT COMWARE Corporation.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 **********************************************************************/

#include "protocol_module_sslid.h"
#include <boost/format.hpp>
#include "utility.h"

namespace l7vs
{
const std::string protocol_module_sslid::MODULE_NAME = "sslid";
const int protocol_module_sslid::THREAD_DIVISION_UP_STREAM = 0;         // up thread
const int protocol_module_sslid::THREAD_DIVISION_DOWN_STREAM = 1;       // down thread
const int protocol_module_sslid::END_FLAG_ON = 1;                       // end flag ON
const int protocol_module_sslid::END_FLAG_OFF = 0;                      // end flag OFF
const int protocol_module_sslid::ACCEPT_END_FLAG_OFF = 0;               // accept flag off
const int protocol_module_sslid::ACCEPT_END_FLAG_ON = 1;                // accept flag on
const int protocol_module_sslid::SORRY_FLAG_ON = 1;                        // sorry flag on
const int protocol_module_sslid::SORRY_FLAG_OFF = 0;                       // sorry flag off

//! constructor
protocol_module_sslid::protocol_module_sslid()
        :ssl_protocol_module_base(MODULE_NAME),
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
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        putLogDebug(300000, "in/out_function : bool protocol_module_sslid::is_tcp() : return_value = true.",
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
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        putLogDebug(300001, "in/out_function : bool protocol_module_sslid::is_udp() : return_value = false.",
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
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        putLogDebug(300002, "in_function : void protocol_module_sslid::initialize("
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
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        putLogDebug(300003, "out_function : void protocol_module_sslid::initialize("
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
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        putLogDebug(300004, "in_function : void protocol_module_sslid::finalize().", __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    // realserver list function object initialize
    rs_list_begin.clear();
    rs_list_end.clear();
    rs_list_next.clear();
    rs_list_lock.clear();
    rs_list_unlock.clear();

    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        putLogDebug(300005, "function : void protocol_module_sslid::finalize() : "
                    "rs_list_begin.clear(), rs_list_end.clear(), rs_list_next.clear(), "
                    "rs_list_lock.clear(), rs_list_unlock.clear() end.", __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    // replication function object initialize
    replication_pay_memory.clear();
    replication_area_lock.clear();
    replication_area_unlock.clear();

    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        putLogDebug(300006, "function : void protocol_module_sslid::finalize() : "
                    "replication_pay_memory.clear(), replication_area_lock.clear(), "
                    "replication_area_unlock.clear() end.", __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    // scheduleModule function object initialize
    schedule_tcp.clear();

    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        putLogDebug(300007, "function : void protocol_module_sslid::finalize() : "
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
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("delete : address = &(%d).");
            formatter % static_cast<void*>(replication_data_processor);
            putLogDebug(300008, formatter.str(), __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/

        delete replication_data_processor;
        replication_data_processor = NULL;
    }

    // session initialize
    if (session_data_processor)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("delete : address = &(%d).");
            formatter % static_cast<void*>(session_data_processor);
            putLogDebug(300009, formatter.str(), __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/

        delete session_data_processor;
        session_data_processor = NULL;
    }

    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        putLogDebug(300010, "out_function : void protocol_module_sslid::finalize().", __FILE__, __LINE__);
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
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        putLogDebug(300011, "in/out_function : bool protocol_module_sslid::is_use_sorry() : return_value = true.",
                    __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/
    return true;
}

//! module parameter check.used by l7vsadm
//! @param[in] module paramter string list
//! @return result.flag true is parameter is noproblem.
//! @return result.flag false is parameter is problem.
protocol_module_sslid::check_message_result protocol_module_sslid::check_parameter(
    const std::vector<std::string>& args)
{
    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
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
        putLogDebug(300012, formatter.str(), __FILE__, __LINE__);
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
                                putLogError(300000, result.message, __FILE__, __LINE__);
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
                            putLogError(300001, result.message, __FILE__, __LINE__);
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
                    putLogError(300002, result.message, __FILE__, __LINE__);
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
                                putLogError(300003, result.message, __FILE__, __LINE__);
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
                            putLogError(300004, result.message, __FILE__, __LINE__);
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
                    putLogError(300005, result.message, __FILE__, __LINE__);
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
                    putLogError(300006, result.message, __FILE__, __LINE__);
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
                    putLogError(300007, result.message, __FILE__, __LINE__);
                    break;
                }
            }
            else
            {
                // set check result flag false
                result.flag = false;
                result.message = "Option error.";
                putLogError(300008, result.message, __FILE__, __LINE__);
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
        putLogError(300009, formatter.str(), __FILE__, __LINE__);
    }
    catch (...)
    {
        result.flag = false;
        std::cerr << "protocol_module_sslid::check_parameter() : Unknown exception." << std::endl;
        putLogError(300010, "function : protocol_module_sslid::check_message_result protocol_module_sslid::"
                    "check_parameter() : Unknown exception.", __FILE__, __LINE__);
    }

    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("out_function : protocol_module_sslid::check_message_result "
                                "protocol_module_sslid::check_parameter("
                                "const std::vector<std::string>& args) : return_value = ("
                                "check_message_result.flag = %d, check_message_result.message = %s).");
        formatter % result.flag % result.message;
        putLogDebug(300013, formatter.str(), __FILE__, __LINE__);
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
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
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
        putLogDebug(300014, formatter.str(), __FILE__, __LINE__);
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
                                putLogError(300011, result.message, __FILE__, __LINE__);
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
                            putLogError(300012, result.message, __FILE__, __LINE__);
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
                    putLogError(300013, result.message, __FILE__, __LINE__);
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
                                putLogError(300014, result.message, __FILE__, __LINE__);
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
                            putLogError(300015, result.message, __FILE__, __LINE__);
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
                    putLogError(300016, result.message, __FILE__, __LINE__);
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
                    putLogError(300017, result.message, __FILE__, __LINE__);
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
                    putLogError(300018, result.message, __FILE__, __LINE__);
                    break;
                }
            }
            else
            {
                // set check result flag false
                result.flag = false;
                result.message = "Option error.";
                putLogError(300019, result.message, __FILE__, __LINE__);
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
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("function : protocol_module_sslid::check_message_result "
                                    "protocol_module_sslid::set_parameter(const std::vector<std::string>& args) : "
                                    "timeout = %d, maxlist = %d, reschedule = %d.");
            formatter % timeout % maxlist % reschedule;
            putLogDebug(300015, formatter.str(), __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/

        unsigned int data_size = 0;
        void* data_addr = NULL;
        data_addr = replication_pay_memory(get_name(), &data_size);

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            putLogDebug(300016, "function : protocol_module_sslid::check_message_result protocol_module_sslid::"
                        "set_parameter() : replication_pay_memory() end.", __FILE__, __LINE__);
            boost::format formatter("function : protocol_module_sslid::check_message_result protocol_module_sslid::"
                        "set_parameter() : data_addr = &(%d), data_size = %d.");
            formatter % data_addr % data_size;
            putLogDebug(300017, formatter.str(), __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/

        if (unlikely(data_addr == NULL || data_size <= 0))
        {
            // replication area is null
            putLogInfo(300000, "Replication area is null.", __FILE__, __LINE__);

            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                putLogDebug(300018, "function : protocol_module_sslid::check_message_result "
                            "protocol_module_sslid::set_parameter() : "
                            "Replication area is null.", __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/
        }

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
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("new : address = &(%d), size = %lu.");
            formatter % static_cast<void*>(replication_data_processor)
            % sizeof(sslid_replication_data_processor);
            putLogDebug(300019, formatter.str(), __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/

        replication_data_processor->register_replication_area_lock(replication_area_lock);
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            putLogDebug(300020, "protocol_module_sslid::check_message_result protocol_module_sslid::"
                        "set_parameter() : register_replication_area_lock() end.", __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/

        replication_data_processor->register_replication_area_unlock(replication_area_unlock);
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            putLogDebug(300021, "protocol_module_sslid::check_message_result protocol_module_sslid::"
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
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("new : address = &(%d), size = %lu.");
            formatter % static_cast<void*>(session_data_processor)
            % sizeof(sslid_session_data_processor);
            putLogDebug(300022, formatter.str(), __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/

        // restore data from replication area
        sslid_replication_data* redata = replication_data_processor->get_replication_area();
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            putLogDebug(300023, "function : protocol_module_sslid::check_message_result protocol_module_sslid::"
                        "set_parameter() : get_replication_area() end.", __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/

        replication_area_lock();
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            putLogDebug(300024, "function : protocol_module_sslid::check_message_result protocol_module_sslid::"
                        "set_parameter() : replication_area_lock() end.", __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/

        session_data_processor->read_session_data_from_replication_area(redata);
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            putLogDebug(300025, "function : protocol_module_sslid::check_message_result protocol_module_sslid::"
                        "set_parameter() : read_session_data_from_replication_area() end.", __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/

        replication_area_unlock();
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            putLogDebug(300026, "function : protocol_module_sslid::check_message_result protocol_module_sslid::"
                        "set_parameter() : replication_area_unlock() end.", __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/
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
        putLogError(300020, result.message, __FILE__, __LINE__);
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
        putLogError(300021, formatter.str(), __FILE__, __LINE__);
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
        putLogError(300022, "function : protocol_module_sslid::check_message_result protocol_module_sslid::"
                    "set_parameter() : Unknown exception.", __FILE__, __LINE__);
    }

    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("out_function : protocol_module_sslid::check_message_result "
                                "protocol_module_sslid::set_parameter("
                                "const std::vector<std::string>& args) : return_value = ("
                                "check_message_result.flag = %d, check_message_result.message = %s).");
        formatter % result.flag % result.message;
        putLogDebug(300027, formatter.str(), __FILE__, __LINE__);
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
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("in_function : protocol_module_sslid::check_message_result protocol_module_sslid::"
                                "add_parameter(const std::vector<std::string>& args) : args = %s.");
        std::string argsdump;
        for (std::vector<std::string>::const_iterator it = args.begin(); it != args.end(); ++it)
        {
            argsdump += *it;
        }
        formatter % argsdump;
        putLogDebug(300028, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    check_message_result result;
    result.flag = true;

    if (!args.empty())
    {
        // option exist
        result.flag = false;
        result.message = "Cannot add option.";
        putLogError(300023, result.message, __FILE__, __LINE__);
    }

    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("out_function : protocol_module_sslid::check_message_result "
                                "protocol_module_sslid::add_parameter(const std::vector<std::string>& args) : "
                                "return_value = (check_message_result.flag = %d, check_message_result.message = %s).");
        formatter % result.flag % result.message;
        putLogDebug(300029, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    return result;
}

//! get option info
//! @param[out] module paramter string
void protocol_module_sslid::get_option_info(std::string& option)
{
    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        putLogDebug(300030, "in_function : void protocol_module_sslid::get_option_info("
                    "std::string& option).", __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    boost::format option_formatter("--timeout %d --maxlist %d %s");
    option_formatter % timeout % maxlist % (reschedule ? "--reschedule" : "--no-reschedule");
    option.assign(option_formatter.str());

    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("out_function : void protocol_module_sslid::get_option_info("
                                "std::string& option) : option = %s.");
        formatter % option;
        putLogDebug(300031, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/
}

//! realserver list update event
void protocol_module_sslid::handle_rslist_update()
{
    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        putLogDebug(300032, "in/out_function : void protocol_module_sslid::handle_rslist_update().",
                    __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/
}

//! TCP/IP scheduled function registation.
//! @param[in] schedule module TCP/IP scheduled function object type
void protocol_module_sslid::register_schedule(tcp_schedule_func_type inschedule)
{
    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        putLogDebug(300033, "in_function : void protocol_module_sslid::register_schedule("
                    "tcp_schedule_func_type inschedule).", __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    schedule_tcp = inschedule;

    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        putLogDebug(300034, "out_function : void protocol_module_sslid::register_schedule("
                    "tcp_schedule_func_type inschedule).", __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/
}

//! UDP scheduled function registation
//! @param[in] schedule module UDP scheduled funtion object type
void protocol_module_sslid::register_schedule(udp_schedule_func_type inschedule)
{
    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        putLogDebug(300035, "in/out_function : void protocol_module_sslid::register_schedule("
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
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_session_initialize(const boost::thread::id up_thread_id, "
                                "const boost::thread::id down_thread_id, "
                                "const boost::asio::ip::tcp::endpoint& client_endpoint_tcp, "
                                "const boost::asio::ip::udp::endpoint& client_endpoint_udp) : "
                                "up_thread_id = %d, down_thread_id = %d.");
        formatter % up_thread_id % down_thread_id;
        putLogDebug(300036, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    EVENT_TAG status = ACCEPT;

    try
    {
        thread_data_ptr threaddata_up(new session_thread_data_sslid);

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("new : address = &(%d), size = %lu.");
            formatter % static_cast<void*>(threaddata_up.get()) % sizeof(session_thread_data_sslid);
            putLogDebug(300037, formatter.str(), __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/

        // initialize the up/down thread data
        threaddata_up->data_begain_offset = 0;
        threaddata_up->data_size = 0;
        threaddata_up->current_record_rest_size = 0;
        threaddata_up->hello_message_flag = false;
        threaddata_up->sorry_flag = SORRY_FLAG_OFF;                     // set sorry_flag OFF
        threaddata_up->end_flag = END_FLAG_OFF;                         // set end_flag OFF
        threaddata_up->accept_flag = ACCEPT_END_FLAG_OFF;               // set accept_flag OFF
        threaddata_up->thread_division = THREAD_DIVISION_UP_STREAM;     // up thread division
        threaddata_up->pair_thread_id = down_thread_id;
        threaddata_up->last_status = ACCEPT;

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            // data dump
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                    "handle_session_initialize() : session_thread_data_sslid(upthread) : "
                                    "data_begain_offset = %d, data_size = %d, current_record_rest_size = %d, "
                                    "hello_message_flag = %d, sorry_flag = %d, end_flag = %d, accept_flag = %d, "
                                    "thread_division = %d, pair_thread_id = %d, last_status = %d.");
            formatter % threaddata_up->data_begain_offset
            % threaddata_up->data_size
            % threaddata_up->current_record_rest_size
            % threaddata_up->hello_message_flag
            % threaddata_up->sorry_flag
            % threaddata_up->end_flag
            % threaddata_up->accept_flag
            % threaddata_up->thread_division
            % threaddata_up->pair_thread_id
            % threaddata_up->last_status;
            putLogDebug(300038, formatter.str(), __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/

        thread_data_ptr threaddata_down(new session_thread_data_sslid);

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("new : address = &(%d), size = %lu.");
            formatter % static_cast<void*>(threaddata_down.get()) % sizeof(session_thread_data_sslid);
            putLogDebug(300039, formatter.str(), __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/

        threaddata_down->data_begain_offset = 0;
        threaddata_down->data_size = 0;
        threaddata_down->current_record_rest_size = 0;
        threaddata_down->hello_message_flag = false;
        threaddata_down->sorry_flag = SORRY_FLAG_OFF;                   // set sorry_flag OFF
        threaddata_down->end_flag = END_FLAG_OFF;                       // set end_flag OFF
        threaddata_down->accept_flag = ACCEPT_END_FLAG_OFF;             // set accept_flag OFF
        threaddata_down->thread_division = THREAD_DIVISION_DOWN_STREAM; // down thread division
        threaddata_down->pair_thread_id = up_thread_id;
        threaddata_down->last_status = REALSERVER_RECV;

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            // data_dump
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                    "handle_session_initialize() : session_thread_data_sslid(downthread) : "
                                    "data_begain_offset = %d, data_size = %d, current_record_rest_size = %d, "
                                    "hello_message_flag = %d, sorry_flag = %d, end_flag = %d, accept_flag = %d, "
                                    "thread_division = %d, pair_thread_id = %d, last_status = %d.");
            formatter % threaddata_down->data_begain_offset
            % threaddata_down->data_size
            % threaddata_down->current_record_rest_size
            % threaddata_down->hello_message_flag
            % threaddata_down->sorry_flag
            % threaddata_down->end_flag
            % threaddata_down->accept_flag
            % threaddata_down->thread_division
            % threaddata_down->pair_thread_id
            % threaddata_down->last_status;
            putLogDebug(300040, formatter.str(), __FILE__, __LINE__);
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
        boost::format formatter("Could not allocate memory. thread id : %d.");
        formatter % boost::this_thread::get_id();
        putLogError(300024, formatter.str(), __FILE__, __LINE__);

        status = FINALIZE;
    }
    catch (const std::exception& e)
    {
        std::cerr << "protocol_module_sslid::handle_session_initialize() : exception : error = " << e.what() << "." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_session_initialize() : exception : error = %s. thread id : %d.");
        formatter % e.what() % boost::this_thread::get_id();
        putLogError(300025, formatter.str(), __FILE__, __LINE__);

        status = FINALIZE;
    }
    catch (...)
    {
        std::cerr << "protocol_module_sslid::handle_session_initialize() : Unknown exception." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_session_initialize() : Unknown exception. thread id : %d.");
        formatter % boost::this_thread::get_id();
        putLogError(300026, formatter.str(), __FILE__, __LINE__);

        status = FINALIZE;
    }

    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_session_initialize(const boost::thread::id up_thread_id, "
                                "const boost::thread::id down_thread_id, "
                                "const boost::asio::ip::tcp::endpoint& client_endpoint_tcp, "
                                "const boost::asio::ip::udp::endpoint& client_endpoint_udp) : return_value = %d. "
                "thread id : %d.");
        formatter % status % boost::this_thread::get_id();
        putLogDebug(300041, formatter.str(), __FILE__, __LINE__);
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
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_session_finalize(const boost::thread::id up_thread_id, "
                                "const boost::thread::id down_thread_id) : "
                                "up_thread_id = %d, down_thread_id = %d.");
        formatter % up_thread_id % down_thread_id;
        putLogDebug(300042, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    try
    {
        // delete the up/down thread data from the map
        boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);

        session_thread_data_map_type::iterator itthread;
        itthread = session_thread_data_map.find(up_thread_id);
        if (likely(itthread != session_thread_data_map.end()))
        {
            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("delete : address = &(%d).");
                formatter % static_cast<void*>(itthread->second.get());
                putLogDebug(300043, formatter.str(), __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/
            session_thread_data_map.erase(itthread);
        }

        itthread = session_thread_data_map.find(down_thread_id);
        if (likely(itthread != session_thread_data_map.end()))
        {
            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("delete : address = &(%d).");
                formatter % static_cast<void*>(itthread->second.get());
                putLogDebug(300044, formatter.str(), __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/
            session_thread_data_map.erase(itthread);
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "protocol_module_sslid::handle_session_finalize() : exception : error = " << e.what() << "." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_session_finalize() : exception : error = %s. thread id : %d.");
        formatter % e.what() % boost::this_thread::get_id();
        putLogError(300027, formatter.str(), __FILE__, __LINE__);
    }
    catch (...)
    {
        std::cerr << "protocol_module_sslid::handle_session_finalize() : Unknown exception." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_session_finalize() : Unknown exception. thread id : %d.");
        formatter % boost::this_thread::get_id();
        putLogError(300028, formatter.str(), __FILE__, __LINE__);
    }

    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_session_finalize(const boost::thread::id up_thread_id, "
                                "const boost::thread::id down_thread_id) : return_value = %d. thread id : %d.");
        formatter % STOP % boost::this_thread::get_id();
        putLogDebug(300045, formatter.str(), __FILE__, __LINE__);
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
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_accept(const boost::thread::id thread_id) : thread_id = %d.");
        formatter % thread_id;
        putLogDebug(300046, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    EVENT_TAG status = FINALIZE;
    thread_data_ptr threaddata;
    session_thread_data_map_type::iterator session_thread_it;

    try
    {
        {
            boost::mutex::scoped_lock slock(session_thread_data_map_mutex);

            session_thread_it = session_thread_data_map.find(thread_id);
            if (unlikely(session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL))
            {
                boost::format formatter("Invalid thread id. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogError(300029, formatter.str(), __FILE__, __LINE__ );
                throw -1;
            }

            threaddata = session_thread_it->second;
        }

        //set accept flag on
        threaddata->accept_flag = ACCEPT_END_FLAG_ON;

        //set return status
        status = CLIENT_RECV;

        //set last status
        threaddata->last_status = status;
    }
    catch (int e)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                    "handle_accept() : catch exception e = %d. thread id : %d.");
            formatter % e % boost::this_thread::get_id();
            putLogDebug(300047, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/
        status = FINALIZE;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "protocol_module_sslid::handle_accept() : exception : error = " << ex.what() << "." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_accept() : exception : error = %s. thread id : %d.");
        formatter % ex.what() % boost::this_thread::get_id();
        putLogError(300030, formatter.str(), __FILE__, __LINE__ );
        status = FINALIZE;
    }
    catch (...)
    {
        std::cerr << "protocol_module_sslid::handle_accept() : Unknown exception." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG "
                                "protocol_module_sslid::handle_accept() : "
                                "Unknown exception. thread id : %d.");
        formatter % boost::this_thread::get_id();
        putLogError(300031, formatter.str(), __FILE__, __LINE__ );
        status = FINALIZE;
    }

    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_accept(const boost::thread::id thread_id) : return_value = %d. thread id : %d.");
        formatter % status % boost::this_thread::get_id();
        putLogDebug(300048, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    return status;
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
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        size_t buffer_size = recvbuffer.size() < recvlen ? recvbuffer.size() : recvlen;
        std::string buffer;
        dump_memory(recvbuffer.data(), buffer_size, buffer);
        boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_client_recv(const boost::thread::id thread_id, "
                                "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                "const size_t recvlen) : thread_id = %d, recvbuffer = %s, recvlen = %d.");
        formatter % thread_id % buffer % recvlen;
        putLogDebug(300049, formatter.str(), __FILE__, __LINE__);
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
            boost::format formatter("Data size bigger than buffer size. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(300032, formatter.str(), __FILE__, __LINE__);

            throw -1;
        }

        {
            boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
            session_thread_data_map_type::iterator it = session_thread_data_map.find(thread_id);
            if (unlikely(it == session_thread_data_map.end() || it->second == NULL))
            {
                boost::format formatter("Invalid thread id. thread id: %d.");
                formatter % boost::this_thread::get_id();
                putLogError(300033, formatter.str(), __FILE__, __LINE__);

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
                boost::format formatter("Data size bigger than buffer size. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogError(300034, formatter.str(), __FILE__, __LINE__);
                threaddata->end_flag = END_FLAG_ON;
                throw -1;
            }

            if (threaddata->data_size > 0 && threaddata->data_begain_offset != 0)
            {
                // data dump
                /*-------- DEBUG LOG --------*/
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    std::string datadump;
                    dump_memory(databuf+threaddata->data_begain_offset, threaddata->data_size, datadump);

                    boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                            "handle_client_recv() : before memmove (data dump) : "
                                            "data begin = %d, data_size = %d, data = %s");
                    formatter % threaddata->data_begain_offset % threaddata->data_size % datadump;
                    putLogDebug(300050, formatter.str(), __FILE__, __LINE__);
                }
                /*------DEBUG LOG END------*/

                // move the data from data start pos to buffer start pos
                memmove(databuf, databuf+threaddata->data_begain_offset, threaddata->data_size);

                /*-------- DEBUG LOG --------*/
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    std::string datadump;
                    dump_memory(databuf, threaddata->data_size, datadump);

                    boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                            "handle_client_recv() : after memmove (data dump) : "
                                            "data begin = 0, data_size = %d, data = %s");
                    formatter % threaddata->data_size % datadump;
                    putLogDebug(300051, formatter.str(), __FILE__, __LINE__);
                }
                /*------DEBUG LOG END------*/
            }

            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                std::string datadump;
                dump_memory(recvbuffer.data(), recvlen, datadump);

                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                        "handle_client_recv() : before memcpy (data dump) : "
                                        "data begin = 0, data_size = %d, data = %s");
                formatter % recvlen % datadump;
                putLogDebug(300052, formatter.str(), __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/

            // copy new recv data to data_buffer
            memcpy(databuf+threaddata->data_size, recvbuffer.data(), recvlen);

            // add data size
            threaddata->data_size += recvlen;
            threaddata->data_begain_offset = 0;

            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                std::string datadump;
                dump_memory(threaddata->data_buffer.data() + threaddata->data_size - recvlen, recvlen, datadump);

                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                        "handle_client_recv() : after memcpy (data dump) : "
                                        "data begin = 0, data_size = %d, data = %s");
                formatter % recvlen % datadump;
                putLogDebug(300053, formatter.str(), __FILE__, __LINE__);
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
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                            "handle_client_recv() : check_ssl_record_sendable() end. thread id : %d.");
                    formatter % boost::this_thread::get_id();
                    putLogDebug(300054, formatter.str(), __FILE__, __LINE__);
                }
                /*------DEBUG LOG END------*/

                if (ret == -1)
                {
                    putLogError(300035, "record data is null.", __FILE__, __LINE__);

                    // anomaly, set the end_flag ON
                    threaddata->end_flag = END_FLAG_ON;
                    status = FINALIZE;
                    /*-------- DEBUG LOG --------*/
                    if (unlikely(LOG_LV_DEBUG == getloglevel()))
                    {
                        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                                "handle_client_recv() : END_FLAG_ON. thread id : %d.");
                        formatter % boost::this_thread::get_id();
                        putLogDebug(300055, formatter.str(), __FILE__, __LINE__);
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

                    // check sorry state
                    if(threaddata->sorry_flag == SORRY_FLAG_ON)
                    {
                        status = SORRYSERVER_SELECT;
                    }
                    else
                    {
                        status = REALSERVER_SELECT;
                    }
                }
            }
            else
            {
                // check sorry state
                if(threaddata->sorry_flag == SORRY_FLAG_ON)
                {
                    status = SORRYSERVER_SELECT;
                }
                else
                {
                    status = REALSERVER_SELECT;
                }
            }
        }

        // set last status
        threaddata->last_status = status;
    }
    catch (int e)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                    "handle_client_recv() : catch exception e = %d. thread id : %d.");
            formatter % e % boost::this_thread::get_id();
            putLogDebug(300056, formatter.str(), __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/
        status = FINALIZE;
    }
    catch (const std::exception& e)
    {
        std::cerr << "protocol_module_sslid::handle_client_recv() : error = " << e.what() << "." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_client_recv() : exception : error = %s. thread id : %d.");
        formatter % e.what() % boost::this_thread::get_id();
        putLogError(300036, formatter.str(), __FILE__, __LINE__);

        status = FINALIZE;
    }
    catch (...)
    {
        std::cerr << "protocol_module_sslid::handle_client_recv() : Unknown exception." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG "
                                "protocol_module_sslid::handle_client_recv() "
                                ": Unknown exception. thread id : %d");
        formatter % boost::this_thread::get_id();
        putLogError(300037, formatter.str(), __FILE__, __LINE__);

        status = FINALIZE;
    }

    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_client_recv(const boost::thread::id thread_id, "
                                "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                "const size_t recvlen) : return_value = %d, end_flag = %d. thread id : %d.");
        formatter % status % (threaddata.get() ? threaddata->end_flag : END_FLAG_ON) % boost::this_thread::get_id();
        putLogDebug(300057, formatter.str(), __FILE__, __LINE__);
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
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_realserver_select(const boost::thread::id thread_id, "
                                "boost::asio::ip::tcp::endpoint& rs_endpoint) : "
                                "thread_id = %d, rs_endpoint = [%s]:%d.");
        formatter % thread_id % rs_endpoint.address().to_string() % rs_endpoint.port();
        putLogDebug(300058, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    // find session_thread_data_sslid's pointer from map
    thread_data_ptr threaddata;
    EVENT_TAG status = FINALIZE;
    realserverlist_type::iterator    rs_list_itr;

    try
    {
        {
            boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
            session_thread_data_map_type::iterator it = session_thread_data_map.find(thread_id);
            if (unlikely(it == session_thread_data_map.end() || it->second == NULL))
            {
                boost::format formatter("Invalid thread id. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogError(300038, formatter.str(), __FILE__, __LINE__);
                /*-------- DEBUG LOG --------*/
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                            "handle_realserver_select(const boost::thread::id thread_id, "
                                            "boost::asio::ip::tcp::endpoint& rs_endpoint) : return_value = %d. thread id : %d.");
                    formatter % FINALIZE % boost::this_thread::get_id();
                    putLogDebug(300059, formatter.str(), __FILE__, __LINE__);
                }
                /*------DEBUG LOG END------*/
                putLogInfo(300001, "Realserver decision failure. ", __FILE__, __LINE__);

                return FINALIZE;
            }

            threaddata = it->second;
        }

        // the first connection or connected successful
        if (realserver_selected(threaddata->selected_realserver))
        {
            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                        "handle_realserver_select() end. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogDebug(300060, formatter.str(), __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/

            // realserver already connected
            rs_endpoint = threaddata->selected_realserver;
            status = REALSERVER_CONNECT;
        }
        else
        {
            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                        "handle_realserver_select() end. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogDebug(300061, formatter.str(), __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/

            // first connection get sessionid from the data buffer
            std::string session_id;
            if (threaddata->hello_message_flag &&
           (get_ssl_session_id(threaddata->data_buffer.data()+threaddata->data_begain_offset,
                                   threaddata->data_size, session_id) == -1))
            {
                /*-------- DEBUG LOG --------*/
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    boost::format formatter("out_function : protocol_module_base::EVENT_TAG "
                                            "protocol_module_sslid::handle_realserver_select("
                                            "const boost::thread::id thread_id, boost::asio::"
                                            "ip::tcp::endpoint& rs_endpoint) : return_value = %d, "
                    "thread id : %d.");
                    formatter % FINALIZE % boost::this_thread::get_id();
                    putLogDebug(300062, formatter.str(), __FILE__, __LINE__);
                }
                /*------DEBUG LOG END------*/

                putLogInfo(300002, "Realserver decision failure.", __FILE__, __LINE__);
            }

            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                std::string buffer;
                dump_session_id(session_id.c_str(), session_id.size(), buffer);
                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                        "handle_realserver_select() : get_ssl_session_id() session_id = %s end. "
                                        "thread id : %d.");
                formatter % buffer % boost::this_thread::get_id();
                putLogDebug(300063, formatter.str(), __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/

            if (session_id.empty())
            {
                // no session id in hellomesseage
                // schedule the endpoint
                boost::asio::ip::tcp::endpoint temp_endpoint;
                boost::asio::ip::tcp::endpoint comp_endpoint;   // for compare the endpoint
                {
                    rs_list_scoped_lock scoped_lock(rs_list_lock, rs_list_unlock);
                    schedule_tcp(thread_id, rs_list_begin, rs_list_end, rs_list_next, temp_endpoint);
                }
                
                if (temp_endpoint != comp_endpoint)
                {
                    // success for get the endpoint by reschedule
                    threaddata->selected_realserver = temp_endpoint;
                    rs_endpoint = temp_endpoint;
                    status = REALSERVER_CONNECT;
                }
                else
                {
                    // failed to get the endpoint by schedule, try to select sorry server
                    // set sorry_flag ON, and set the status SORRYSERVER_SELECT
                    threaddata->sorry_flag = SORRY_FLAG_ON;
                    status = SORRYSERVER_SELECT;
                    /*-------- DEBUG LOG --------*/
                    if (unlikely(LOG_LV_DEBUG == getloglevel()))
                    {
                        boost::format formatter("function : protocol_module_base::EVENT_TAG "
                                    "protocol_module_sslid::handle_realserver_select() : "
                                    "SORRY_FLAG_ON. thread id : %d.");
                        formatter % boost::this_thread::get_id();
                        putLogDebug(300064, formatter.str(), __FILE__, __LINE__);
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
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    boost::format formatter("function : protocol_module_base::EVENT_TAG "
                                            "protocol_module_sslid::handle_realserver_select() : "
                                            "get_endpoint_from_session_data() end. thread id : %d.");
                    formatter % boost::this_thread::get_id();
                    putLogDebug(300065, formatter.str(), __FILE__, __LINE__);
                }
                /*------DEBUG LOG END------*/

                if (ret == 0)
                {
                    int is_match = 0;
                    //find the rs_endpoint in the rs_list
                    {
                        rs_list_scoped_lock scoped_lock(rs_list_lock, rs_list_unlock);

                        rs_list_itr = rs_list_begin();

                        while (rs_list_itr != rs_list_end())
                        {
                            if (rs_list_itr->tcp_endpoint == threaddata->selected_realserver)
                            {
                                is_match = 1;
                                break;
                            }

                            rs_list_itr = rs_list_next(rs_list_itr);
                        }
                    }

                    if (is_match)
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
                            {
                                rs_list_scoped_lock scoped_lock(rs_list_lock, rs_list_unlock);
                                schedule_tcp(thread_id, rs_list_begin, rs_list_end, rs_list_next, temp_endpoint);
                            }

                            if (temp_endpoint != comp_endpoint)
                            {
                            // get the endpoint by reschedule successfully
                            threaddata->selected_realserver = temp_endpoint;
                            rs_endpoint = temp_endpoint;
                            status = REALSERVER_CONNECT;
                            }
                            else
                            {
                                // failed to get the endpoint by reschedule, try to select sorry server
                                // set sorry_flag ON, and set the status SORRYSERVER_SELECT
                                threaddata->sorry_flag = SORRY_FLAG_ON;
                                status = SORRYSERVER_SELECT;
                                /*-------- DEBUG LOG --------*/
                                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                                {
                                    boost::format formatter("function : protocol_module_base::EVENT_TAG "
                                                "protocol_module_sslid::handle_realserver_select() : "
                                                "SORRY_FLAG_ON. thread id : %d.");
                                    formatter % boost::this_thread::get_id();
                                    putLogDebug(300066, formatter.str(), __FILE__, __LINE__);
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
                            if (unlikely(LOG_LV_DEBUG == getloglevel()))
                            {
                            boost::format formatter("function : protocol_module_base::EVENT_TAG "
                                        "protocol_module_sslid::handle_realserver_select() "
                                        ": END_FLAG_ON. thread id : %d.");
                            formatter % boost::this_thread::get_id();
                            putLogDebug(300067, formatter.str(), __FILE__, __LINE__);
                            }
                            /*------DEBUG LOG END------*/
                        }
                    }
                }
                else
                {
                    // failed to get the endpoint
                    if (reschedule == 1)
                    {
                        // reschedule mode
                        boost::asio::ip::tcp::endpoint temp_endpoint;
                        boost::asio::ip::tcp::endpoint comp_endpoint;   // for compare the endpoint
                        {
                            rs_list_scoped_lock scoped_lock(rs_list_lock, rs_list_unlock);
                            schedule_tcp(thread_id, rs_list_begin, rs_list_end, rs_list_next, temp_endpoint);
                        }

                        if (temp_endpoint != comp_endpoint)
                        {
                            // get the endpoint by reschedule successfully
                            threaddata->selected_realserver = temp_endpoint;
                            rs_endpoint = temp_endpoint;
                            status = REALSERVER_CONNECT;
                        }
                        else
                        {
                            // failed to get the endpoint by reschedule, try to select sorry server
                            // set sorry_flag ON, and set the status SORRYSERVER_SELECT
                            threaddata->sorry_flag = SORRY_FLAG_ON;
                            status = SORRYSERVER_SELECT;
                            /*-------- DEBUG LOG --------*/
                            if (unlikely(LOG_LV_DEBUG == getloglevel()))
                            {
                                boost::format formatter("function : protocol_module_base::EVENT_TAG "
                                            "protocol_module_sslid::handle_realserver_select() : "
                                            "SORRY_FLAG_ON. thread id : %d.");
                                formatter % boost::this_thread::get_id();
                                putLogDebug(300068, formatter.str(), __FILE__, __LINE__);
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
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            boost::format formatter("function : protocol_module_base::EVENT_TAG "
                                                    "protocol_module_sslid::handle_realserver_select() "
                                                    ": END_FLAG_ON. thread id : %d.");
                            formatter % boost::this_thread::get_id();
                            putLogDebug(300069, formatter.str(), __FILE__, __LINE__);
                        }
                        /*------DEBUG LOG END------*/
                    }
                }
            }
        }

        // set last status
        threaddata->last_status = status;
    }
    catch (const std::exception& e)
    {
        std::cerr << "protocol_module_sslid::handle_realserver_select() : exception : error = " << e.what() << "." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_realserver_select() : exception : error = %s. thread id : %d.");
        formatter % e.what() % boost::this_thread::get_id();
        putLogError(300039, formatter.str(), __FILE__, __LINE__);

        putLogInfo(300003, "Realserver decision failure.", __FILE__, __LINE__);

        status = FINALIZE;
    }
    catch (...)
    {
        std::cerr << "protocol_module_sslid::handle_realserver_select() : Unknown exception." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                "handle_realserver_select() : Unknown exception. thread id : %d.");
        formatter % boost::this_thread::get_id();
        putLogError(300040, formatter.str(), __FILE__, __LINE__);

        putLogInfo(300004, "Realserver decision failure.", __FILE__, __LINE__);

        status = FINALIZE;
    }

    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_realserver_select(const boost::thread::id thread_id, "
                                "boost::asio::ip::tcp::endpoint& rs_endpoint) : return_value = %d, "
                                "end_flag = %d, rs_endpoint = [%s]:%d. thread id : %d.");
        formatter % status % (threaddata.get() ? threaddata->end_flag : END_FLAG_ON)
                  % rs_endpoint.address().to_string() % rs_endpoint.port() % boost::this_thread::get_id();
        putLogDebug(300070, formatter.str(), __FILE__, __LINE__);
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
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("in/out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_realserver_select(const boost::thread::id thread_id, "
                                "boost::asio::ip::udp::endpoint& rs_endpoint, "
                                "boost::array<char,MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen) : "
                                "return_value = %d. thread id : %d.");
        formatter % STOP % boost::this_thread::get_id();
        putLogDebug(300071, formatter.str(), __FILE__, __LINE__);
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
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_realserver_connect(const boost::thread::id thread_id, "
                                "boost::array<char,MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen) : "
                                "thread_id = %d.");
        formatter % thread_id;
        putLogDebug(300072, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    EVENT_TAG status = REALSERVER_SEND;
    thread_data_ptr threaddata;
    session_thread_data_map_type::iterator it;

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

            it = session_thread_data_map.find(thread_id);
            if (unlikely(it == session_thread_data_map.end()))
            {
                boost::format formatter("Invalid thread id. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogError(300041, formatter.str(), __FILE__, __LINE__);
                status = FINALIZE;
            }

            threaddata = it->second;
        }

        // set last status
        threaddata->last_status = status;
    }
    catch (const std::exception& e)
    {
        std::cerr << "protocol_module_sslid::handle_realserver_connect() : exception : error = " << e.what() << "." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_realserver_connect() exception : error = %s. thread id : %d.");
        formatter % e.what() % boost::this_thread::get_id();
        putLogError(300042, formatter.str(), __FILE__, __LINE__);
        status = FINALIZE;
    }
    catch (...)
    {
        std::cerr << "protocol_module_sslid::handle_realserver_connect() : Unknown exception." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_realserver_connect() : Unknown exception. thread id : %d.");
        formatter % boost::this_thread::get_id();
        putLogError(300043, formatter.str(), __FILE__, __LINE__);

        status = FINALIZE;
    }

    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_realserver_connect(const boost::thread::id thread_id, "
                                "boost::array<char,MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen) : "
                                "return_value = %d. thread id : %d.");
        formatter % status % boost::this_thread::get_id();
        putLogDebug(300073, formatter.str(), __FILE__, __LINE__);
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
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_realserver_connection_fail(const boost::thread::id thread_id, "
                                "const boost::asio::ip::tcp::endpoint& rs_endpoint) : "
                                "thread_id = %d, rs_endpoint = [%s]:%d.");
        formatter % thread_id % rs_endpoint.address().to_string() % rs_endpoint.port();
        putLogDebug(300074, formatter.str(), __FILE__, __LINE__);
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
            if (unlikely(it == session_thread_data_map.end() || it->second == NULL))
            {
                boost::format formatter("Invalid thread id. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogError(300044, formatter.str(), __FILE__, __LINE__);
                /*-------- DEBUG LOG --------*/
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                            "handle_realserver_connection_fail(const boost::thread::id thread_id, "
                                            "const boost::asio::ip::tcp::endpoint& rs_endpoint) : return_value = %d. "
                        "thread id : %d.");
                    formatter % FINALIZE % boost::this_thread::get_id();
                    putLogDebug(300075, formatter.str(), __FILE__, __LINE__);
                }
                /*------DEBUG LOG END------*/
                return FINALIZE;
            }

            threaddata = it->second;
        }

        // set end_flag ON
        threaddata->end_flag = END_FLAG_ON;
        status = CLIENT_DISCONNECT;

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                    "handle_realserver_connection_fail() : END_FLAG_ON. thread id : %d.");
        formatter % boost::this_thread::get_id();
            putLogDebug(300076, formatter.str(), __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/

        // set last status
        threaddata->last_status = status;
    }
    catch (const std::exception& e)
    {
        std::cerr << "protocol_module_sslid::handle_realserver_connection_fail() : exception : error = " << e.what() << "." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_realserver_connection_fail() : exception : error = %s. thread id : %d.");
        formatter % e.what() % boost::this_thread::get_id();
        putLogError(300045, formatter.str(), __FILE__, __LINE__);

        status = FINALIZE;
    }
    catch (...)
    {
        std::cerr << "protocol_module_sslid::handle_realserver_connection_fail() : Unknown exception." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_realserver_connection_fail() : Unknown exception. thread id : %d.");
        formatter % boost::this_thread::get_id();
        putLogError(300046, formatter.str(), __FILE__, __LINE__);

        status = FINALIZE;
    }

    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_realserver_connection_fail(const boost::thread::id thread_id, "
                                "const boost::asio::ip::tcp::endpoint& rs_endpoint) : "
                                "return_value = %d, end_flag = %d. thread id : %d.");
        formatter % status % (threaddata.get() ? threaddata->end_flag : END_FLAG_ON) % boost::this_thread::get_id();
        putLogDebug(300077, formatter.str(), __FILE__, __LINE__);
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
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_realserver_send(const boost::thread::id thread_id) : thread_id = %d.");
        formatter % thread_id;
        putLogDebug(300078, formatter.str(), __FILE__, __LINE__);
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
            if (unlikely(it == session_thread_data_map.end() || it->second == NULL))
            {
                boost::format formatter("Invalid thread id. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogError(300047, formatter.str(), __FILE__, __LINE__);
                /*-------- DEBUG LOG --------*/
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                            "handle_realserver_send(const boost::thread::id thread_id) : "
                                            "return_value = %d. thread id : %d.");
                    formatter % FINALIZE % boost::this_thread::get_id();
                    putLogDebug(300079, formatter.str(), __FILE__, __LINE__);
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
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                            "handle_realserver_send() : check_ssl_record_sendable() end. thread id : %d.");
                    formatter % boost::this_thread::get_id();
                    putLogDebug(300080, formatter.str(), __FILE__, __LINE__);
                }
                /*------DEBUG LOG END------*/

                if (ret == -1)
                {
                    putLogError(300048, "record data is null.", __FILE__, __LINE__);

                    // anomaly, set the end_flag ON
                    threaddata->end_flag = END_FLAG_ON;
                    status = FINALIZE;
                    /*-------- DEBUG LOG --------*/
                    if (unlikely(LOG_LV_DEBUG == getloglevel()))
                    {
                        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                                "handle_realserver_send() : END_FLAG_ON. thread id : %d.");
                        formatter % boost::this_thread::get_id();
                        putLogDebug(300081, formatter.str(), __FILE__, __LINE__);
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

        // set last status
        threaddata->last_status = status;
    }
    catch (const std::exception& e)
    {
        std::cerr << "protocol_module_sslid::handle_realserver_send() : exception : error = " << e.what() << "." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_realserver_send() : exception : error = %s. thread id : %d.");
        formatter % e.what() % boost::this_thread::get_id();
        putLogError(300049, formatter.str(), __FILE__, __LINE__);

        status = FINALIZE;
    }
    catch (...)
    {
        std::cerr << "protocol_module_sslid::handle_realserver_send() : Unknown exception." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_realserver_send() : Unknown exception. thread id : %d.");
        formatter % boost::this_thread::get_id();
        putLogError(300050, formatter.str(), __FILE__, __LINE__);

        status = FINALIZE;
    }

    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_realserver_send(const boost::thread::id thread_id) : "
                                "return_value = %d, end_flag = %d. thread id : %d.");
        formatter % status % (threaddata.get() ? threaddata->end_flag : END_FLAG_ON) % boost::this_thread::get_id();
        putLogDebug(300082, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    return status;
}

//! called from after sorryserver select
//! @param[in] upstream thread id
//! @param[in] sorryserver endppiont reference
//! @return session use EVENT mode.
protocol_module_base::EVENT_TAG protocol_module_sslid::handle_sorryserver_select(
    const boost::thread::id thread_id,
    boost::asio::ip::tcp::endpoint& sorry_endpoint)
{
    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_sorryserver_select(const boost::thread::id thread_id, "
                                "boost::asio::ip::tcp::endpoint& sorry_endpoint) : "
                                "thread id : %d, sorry_endpoint : [%s]:%d.");
        formatter % boost::this_thread::get_id() % sorry_endpoint.address().to_string() % sorry_endpoint.port();
        putLogDebug(300083, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    EVENT_TAG status = FINALIZE;
    thread_data_ptr threaddata;
    session_thread_data_map_type::iterator session_thread_it;

    try
    {
        {
            boost::mutex::scoped_lock slock(session_thread_data_map_mutex);

            session_thread_it = session_thread_data_map.find(thread_id);
            if (unlikely(session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL))
            {
                boost::format formatter("Invalid thread id. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogError(300051, formatter.str(), __FILE__, __LINE__ );
                throw -1;
            }

            threaddata = session_thread_it->second;
        }

        //set return status
        status = SORRYSERVER_CONNECT;

        //set last status
        threaddata->last_status = status;
    }
    catch (int e)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                    "handle_sorryserver_select() : catch exception e = %d. thread id : %d.");
            formatter % e % boost::this_thread::get_id();
            putLogDebug(300084, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/
        status = FINALIZE;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "protocol_module_sslid::handle_sorryserver_select() : exception : error = " << ex.what() << "." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_sorryserver_select() : exception : error = %s. thread id : %d.");
        formatter % ex.what() % boost::this_thread::get_id();
        putLogError(300052, formatter.str(), __FILE__, __LINE__ );
        status = FINALIZE;
    }
    catch (...)
    {
        std::cerr << "protocol_module_sslid::handle_sorryserver_select() : Unknown exception." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG "
                                "protocol_module_sslid::handle_sorryserver_select() : "
                                "Unknown exception. thread id : %d.");
        formatter % boost::this_thread::get_id();
        putLogError(300053, formatter.str(), __FILE__, __LINE__ );
        status = FINALIZE;
    }

    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_sorryserver_select(const boost::thread::id thread_id, "
                                "boost::asio::ip::tcp::endpoint& sorry_endpoint) : thread id : %d, "
                                "sorry_endpoint : [%s]:%d, return_value = %d.");
        formatter % boost::this_thread::get_id()
                    % sorry_endpoint.address().to_string() % sorry_endpoint.port() % status;
        putLogDebug(300085, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    return status;
}

//! called from after sorryserver connect
//! @param[in] upstream thread id
//! @param[out] send buffer reference.
//! @param[out] send length
//! @return session use EVENT mode.
protocol_module_base::EVENT_TAG protocol_module_sslid::handle_sorryserver_connect(
    const boost::thread::id thread_id,
    boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
    size_t& datalen)
{
    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_sorryserver_connect(const boost::thread::id thread_id, "
                                "boost::array<char,MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen) : "
                                "thread_id = %d.");
        formatter % thread_id;
        putLogDebug(300086, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    EVENT_TAG status = SORRYSERVER_SEND;
    thread_data_ptr threaddata;
    session_thread_data_map_type::iterator it;

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

            it = session_thread_data_map.find(thread_id);
            if (unlikely(it == session_thread_data_map.end()))
            {
                boost::format formatter("Invalid thread id. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogError(300054, formatter.str(), __FILE__, __LINE__);
                status = FINALIZE;
            }

            threaddata = it->second;
        }

        // set last status
        threaddata->last_status = status;
    }
    catch (const std::exception& e)
    {
        std::cerr << "protocol_module_sslid::handle_sorryserver_connect() : exception : error = " << e.what() << "." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_sorryserver_connect() exception : error = %s. thread id : %d.");
        formatter % e.what() % boost::this_thread::get_id();
        putLogError(300055, formatter.str(), __FILE__, __LINE__);
        status = FINALIZE;
    }
    catch (...)
    {
        std::cerr << "protocol_module_sslid::handle_sorryserver_connect() : Unknown exception." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_sorryserver_connect() : Unknown exception. thread id : %d.");
        formatter % boost::this_thread::get_id();
        putLogError(300056, formatter.str(), __FILE__, __LINE__);

        status = FINALIZE;
    }

    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_sorryserver_connect(const boost::thread::id thread_id, "
                                "boost::array<char,MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen) : "
                                "return_value = %d. thread id : %d.");
        formatter % status % boost::this_thread::get_id();
        putLogDebug(300087, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    return status;
}

//! called from after sorryserver connection fail
//! @param[in] upstream thread id
//! @param[in] sorryserver endpoint reference.
//! @return session use EVENT mode.
protocol_module_base::EVENT_TAG protocol_module_sslid::handle_sorryserver_connection_fail(
    const boost::thread::id thread_id,
    const boost::asio::ip::tcp::endpoint& sorry_endpoint)
{
    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_sorryserver_connection_fail(const boost::thread::id thread_id, "
                                "const boost::asio::ip::tcp::endpoint& sorry_endpoint) : "
                                "thread_id = %d, sorry_endpoint = [%s]:%d.");
        formatter % thread_id % sorry_endpoint.address().to_string() % sorry_endpoint.port();
        putLogDebug(300088, formatter.str(), __FILE__, __LINE__);
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
            if (unlikely(it == session_thread_data_map.end() || it->second == NULL))
            {
                boost::format formatter("Invalid thread id. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogError(300057, formatter.str(), __FILE__, __LINE__);
                /*-------- DEBUG LOG --------*/
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                            "handle_sorryserver_connection_fail(const boost::thread::id thread_id, "
                                            "const boost::asio::ip::tcp::endpoint& sorry_endpoint) : return_value = %d. "
                        "thread id : %d.");
                    formatter % FINALIZE % boost::this_thread::get_id();
                    putLogDebug(300089, formatter.str(), __FILE__, __LINE__);
                }
                /*------DEBUG LOG END------*/
                return FINALIZE;
            }

            threaddata = it->second;
        }

        // set end_flag ON
        threaddata->end_flag = END_FLAG_ON;
        status = CLIENT_DISCONNECT;

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                    "handle_sorryserver_connection_fail() : END_FLAG_ON. thread id : %d.");
        formatter % boost::this_thread::get_id();
            putLogDebug(300090, formatter.str(), __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/

        // set last status
        threaddata->last_status = status;
    }
    catch (const std::exception& e)
    {
        std::cerr << "protocol_module_sslid::handle_sorryserver_connection_fail() : exception : error = " << e.what() << "." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_sorryserver_connection_fail() : exception : error = %s. thread id : %d.");
        formatter % e.what() % boost::this_thread::get_id();
        putLogError(300058, formatter.str(), __FILE__, __LINE__);

        status = FINALIZE;
    }
    catch (...)
    {
        std::cerr << "protocol_module_sslid::handle_sorryserver_connection_fail() : Unknown exception." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_sorryserver_connection_fail() : Unknown exception. thread id : %d.");
        formatter % boost::this_thread::get_id();
        putLogError(300059, formatter.str(), __FILE__, __LINE__);

        status = FINALIZE;
    }

    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_sorryserver_connection_fail(const boost::thread::id thread_id, "
                                "const boost::asio::ip::tcp::endpoint& sorry_endpoint) : "
                                "return_value = %d, end_flag = %d. thread id : %d.");
        formatter % status % (threaddata.get() ? threaddata->end_flag : END_FLAG_ON) % boost::this_thread::get_id();
        putLogDebug(300091, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    return status;
}

//! called from after sorryserver send
//! @param[in] upstream thread id
//! @return session use EVENT mode.
protocol_module_base::EVENT_TAG protocol_module_sslid::handle_sorryserver_send(
    const boost::thread::id thread_id)
{
    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_sorryserver_send(const boost::thread::id thread_id) : thread_id = %d.");
        formatter % thread_id;
        putLogDebug(300092, formatter.str(), __FILE__, __LINE__);
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
            if (unlikely(it == session_thread_data_map.end() || it->second == NULL))
            {
                boost::format formatter("Invalid thread id. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogError(300060, formatter.str(), __FILE__, __LINE__);
                /*-------- DEBUG LOG --------*/
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                            "handle_sorryserver_send(const boost::thread::id thread_id) : "
                                            "return_value = %d. thread id : %d.");
                    formatter % FINALIZE % boost::this_thread::get_id();
                    putLogDebug(300093, formatter.str(), __FILE__, __LINE__);
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
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                            "handle_sorryserver_send() : check_ssl_record_sendable() end. thread id : %d.");
                    formatter % boost::this_thread::get_id();
                    putLogDebug(300094, formatter.str(), __FILE__, __LINE__);
                }
                /*------DEBUG LOG END------*/

                if (ret == -1)
                {
                    putLogError(300061, "record data is null.", __FILE__, __LINE__);

                    // anomaly, set the end_flag ON
                    threaddata->end_flag = END_FLAG_ON;
                    status = FINALIZE;
                    /*-------- DEBUG LOG --------*/
                    if (unlikely(LOG_LV_DEBUG == getloglevel()))
                    {
                        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                                "handle_sorryserver_send() : END_FLAG_ON. thread id : %d.");
                        formatter % boost::this_thread::get_id();
                        putLogDebug(300095, formatter.str(), __FILE__, __LINE__);
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
                    // the data can be sent, set the status SORRYSERVER_CONNECT
                    // set client helloworld shakehand flag
                    threaddata->hello_message_flag = ishellomessage;
                    threaddata->current_record_rest_size = alllength;
                    status = SORRYSERVER_CONNECT;
                }
            }
            else
            {
                status = SORRYSERVER_CONNECT;
            }
        }

        // set last status
        threaddata->last_status = status;
    }
    catch (const std::exception& e)
    {
        std::cerr << "protocol_module_sslid::handle_sorryserver_send() : exception : error = " << e.what() << "." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_sorryserver_send() : exception : error = %s. thread id : %d.");
        formatter % e.what() % boost::this_thread::get_id();
        putLogError(300062, formatter.str(), __FILE__, __LINE__);

        status = FINALIZE;
    }
    catch (...)
    {
        std::cerr << "protocol_module_sslid::handle_sorryserver_send() : Unknown exception." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_sorryserver_send() : Unknown exception. thread id : %d.");
        formatter % boost::this_thread::get_id();
        putLogError(300063, formatter.str(), __FILE__, __LINE__);

        status = FINALIZE;
    }

    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_sorryserver_send(const boost::thread::id thread_id) : "
                                "return_value = %d, end_flag = %d. thread id : %d.");
        formatter % status % (threaddata.get() ? threaddata->end_flag : END_FLAG_ON) % boost::this_thread::get_id();
        putLogDebug(300096, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    return status;
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

    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        size_t buffer_size = recvbuffer.size() < recvlen ? recvbuffer.size() : recvlen;
        std::string buffer;
        dump_memory(recvbuffer.data(), buffer_size, buffer);
        boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_realserver_recv(const boost::thread::id thread_id, "
                                "const boost::asio::ip::tcp::endpoint& rs_endpoint, "
                                "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                "const size_t recvlen) : thread_id = %d, rs_endpoint = [%s]:%d, "
                                "recvbuffer = %s, recvlen = %d.");
        formatter % thread_id % rs_endpoint.address().to_string() % rs_endpoint.port() %
        buffer % recvlen;
        putLogDebug(300097, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    // parameter check
    if (recvlen > recvbuffer.size())
    {
        std::cerr << "protocol_module_sslid::handle_realserver_recv() : Data size bigger than buffer size." << std::endl;
        boost::format formatter("Data size bigger than buffer size. thread id : %d.");
        formatter % boost::this_thread::get_id();
        putLogError(300064, formatter.str(), __FILE__, __LINE__);
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                    "handle_realserver_recv(const boost::thread::id thread_id, "
                                    "const boost::asio::ip::tcp::endpoint& rs_endpoint, "
                                    "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                    "const size_t recvlen) : return_value = %d. thread id : %d.");
            formatter % FINALIZE % boost::this_thread::get_id();
            putLogDebug(300098, formatter.str(), __FILE__, __LINE__);
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
                boost::format formatter("Invalid thread id. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogError(300065, formatter.str(), __FILE__, __LINE__);
                /*-------- DEBUG LOG --------*/
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                            "handle_realserver_recv(const boost::thread::id thread_id, "
                                            "const boost::asio::ip::tcp::endpoint& rs_endpoint, "
                                            "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                            "const size_t recvlen) : return_value = %d. thread id : %d.");
                    formatter % FINALIZE % boost::this_thread::get_id();
                    putLogDebug(300099, formatter.str(), __FILE__, __LINE__);
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
            boost::format formatter("Data size bigger than buffer size. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(300066, formatter.str(), __FILE__, __LINE__);

            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                        "handle_realserver_recv(const boost::thread::id thread_id, "
                                        "const boost::asio::ip::tcp::endpoint& rs_endpoint, "
                                        "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                        "const size_t recvlen) : return_value = %d. thread id : %d.");
                formatter % FINALIZE % boost::this_thread::get_id();
                putLogDebug(300100, formatter.str(), __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/

            threaddata->end_flag = END_FLAG_ON;
            return FINALIZE;
        }

        if (threaddata->data_size > 0 && threaddata->data_begain_offset != 0)
        {
            // data dump
            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                std::string datadump;
                dump_memory(databuf+threaddata->data_begain_offset, threaddata->data_size, datadump);

                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                        "handle_realserver_recv() : before memmove (data dump) : "
                                        "data begin = %d, data_size = %d, data = %s");
                formatter % threaddata->data_begain_offset % threaddata->data_size % datadump;
                putLogDebug(300101, formatter.str(), __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/

            // move the data from data start pos to buffer start pos
            memmove(databuf, databuf+threaddata->data_begain_offset, threaddata->data_size);

            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                std::string datadump;
                dump_memory(databuf, threaddata->data_size, datadump);

                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                        "handle_realserver_recv() : after memmove (data dump) : "
                                        "data begin = 0, data_size = %d, data = %s");
                formatter % threaddata->data_size % datadump;
                putLogDebug(300102, formatter.str(), __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/
        }

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            std::string datadump;
            dump_memory(recvbuffer.data(), recvlen, datadump);

            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                    "handle_realserver_recv() : before memcpy (data dump) : "
                                    "data begin = 0, data_size = %d, data = %s");
            formatter % recvlen % datadump;
            putLogDebug(300103, formatter.str(), __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/

        // copy new recv data to data_buffer
        memcpy(databuf+threaddata->data_size, recvbuffer.data(), recvlen);

        // add data size
        threaddata->data_size += recvlen;
        threaddata->data_begain_offset = 0;

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            std::string datadump;
            dump_memory(databuf + threaddata->data_size - recvlen, recvlen, datadump);

            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                    "handle_realserver_recv() : after memcpy (data dump) : "
                                    "data begin = 0, data_size = %d, data = %s");
            formatter % recvlen % datadump;
            putLogDebug(300104, formatter.str(), __FILE__, __LINE__);
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
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                        "handle_realserver_recv() : check_ssl_record_sendable() end. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogDebug(300105, formatter.str(), __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/

            if (ret == -1)
            {
                putLogError(300067, "record data is null.", __FILE__, __LINE__);

                // anomaly, set the end_flag ON
                threaddata->end_flag = END_FLAG_ON;
                status = FINALIZE;
                /*-------- DEBUG LOG --------*/
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                            "handle_realserver_recv() : END_FLAG_ON. thread id : %d.");
                    formatter % boost::this_thread::get_id();
                    putLogDebug(300106, formatter.str(), __FILE__, __LINE__);
                }
                /*------DEBUG LOG END------*/
            }
            else if (ret == 1)
            {
                // the data can't be sent, set the status REALSERVER_RECV
                status = REALSERVER_RECV;
            }
            else
            {
                // the data can be sent, set the status CLIENT_CONNECTION_CHECK
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

        // set last status
        threaddata->last_status = status;
    }
    catch (const std::exception& e)
    {
        std::cerr << "protocol_module_sslid::handle_realserver_recv() : exception : error = " << e.what() << "." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_realserver_recv() : exception : error = %s. thread id : %d.");
        formatter % e.what() % boost::this_thread::get_id();
        putLogError(300068, formatter.str(), __FILE__, __LINE__);

        status = FINALIZE;
    }
    catch (...)
    {
        std::cerr << "protocol_module_sslid::handle_realserver_recv() : Unknown exception." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_realserver_recv() : Unknown exception. thread id : %d.");
        formatter % boost::this_thread::get_id();
        putLogError(300069, formatter.str(), __FILE__, __LINE__);

        status = FINALIZE;
    }

    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_realserver_recv(const boost::thread::id thread_id, "
                                "const boost::asio::ip::tcp::endpoint& rs_endpoint, "
                                "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                "const size_t recvlen) : return_value = %d, end_flag = %d. thread id : %d.");
        formatter % status % (threaddata.get() ? threaddata->end_flag : END_FLAG_ON) % boost::this_thread::get_id();
        putLogDebug(300107, formatter.str(), __FILE__, __LINE__);
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
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("in/out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_realserver_recv(const boost::thread::id thread_id, "
                                "const boost::asio::ip::udp::endpoint& rs_endpoint, "
                                "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                "const size_t recvlen) : return_value = %d. thread id : %d.");
        formatter % STOP % boost::this_thread::get_id();
        putLogDebug(300108, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    return STOP;
}

//! called from after sorryserver recive
//! @param[in] downstream thread id
//! @param[in] sorryserver endpoint reference
//! @param[in] recive from realserver buffer reference.
//! @param[in] recv data length
//! @return     session use EVENT mode.
protocol_module_base::EVENT_TAG protocol_module_sslid::handle_sorryserver_recv(
    const boost::thread::id thread_id,
    const boost::asio::ip::tcp::endpoint& sorry_endpoint,
    const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,
    const size_t recvlen)
{
    /*-------- DEBUG LOG --------*/

    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        size_t buffer_size = recvbuffer.size() < recvlen ? recvbuffer.size() : recvlen;
        std::string buffer;
        dump_memory(recvbuffer.data(), buffer_size, buffer);
        boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_sorryserver_recv(const boost::thread::id thread_id, "
                                "const boost::asio::ip::tcp::endpoint& sorry_endpoint, "
                                "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                "const size_t recvlen) : thread_id = %d, sorry_endpoint = [%s]:%d, "
                                "recvbuffer = %s, recvlen = %d.");
        formatter % thread_id % sorry_endpoint.address().to_string() % sorry_endpoint.port() %
        buffer % recvlen;
        putLogDebug(300109, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    // parameter check
    if (recvlen > recvbuffer.size())
    {
        std::cerr << "protocol_module_sslid::handle_sorryserver_recv() : Data size bigger than buffer size." << std::endl;
        boost::format formatter("Data size bigger than buffer size. thread id : %d.");
        formatter % boost::this_thread::get_id();
        putLogError(300070, formatter.str(), __FILE__, __LINE__);
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                    "handle_sorryserver_recv(const boost::thread::id thread_id, "
                                    "const boost::asio::ip::tcp::endpoint& sorry_endpoint, "
                                    "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                    "const size_t recvlen) : return_value = %d. thread id : %d.");
            formatter % FINALIZE % boost::this_thread::get_id();
            putLogDebug(300110, formatter.str(), __FILE__, __LINE__);
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
                boost::format formatter("Invalid thread id. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogError(300071, formatter.str(), __FILE__, __LINE__);
                /*-------- DEBUG LOG --------*/
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                            "handle_sorryserver_recv(const boost::thread::id thread_id, "
                                            "const boost::asio::ip::tcp::endpoint& sorry_endpoint, "
                                            "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                            "const size_t recvlen) : return_value = %d. thread id : %d.");
                    formatter % FINALIZE % boost::this_thread::get_id();
                    putLogDebug(300111, formatter.str(), __FILE__, __LINE__);
                }
                /*------DEBUG LOG END------*/

                return FINALIZE;
            }

            threaddata = it->second;
        }

        // get the c-style pointer from data buffer
        char* databuf = threaddata->data_buffer.c_array();
        // data length check
        if (threaddata->data_size + recvlen > threaddata->data_buffer.size())
        {
            std::cerr << "protocol_module_sslid::handle_sorryserver_recv() : Data size bigger than buffer size." << std::endl;
            boost::format formatter("Data size bigger than buffer size. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(300072, formatter.str(), __FILE__, __LINE__);

            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                        "handle_sorryserver_recv(const boost::thread::id thread_id, "
                                        "const boost::asio::ip::tcp::endpoint& sorry_endpoint, "
                                        "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                        "const size_t recvlen) : return_value = %d. thread id : %d.");
                formatter % FINALIZE % boost::this_thread::get_id();
                putLogDebug(300112, formatter.str(), __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/

            threaddata->end_flag = END_FLAG_ON;
            return FINALIZE;
        }

        if (threaddata->data_size > 0 && threaddata->data_begain_offset != 0)
        {
            // data dump
            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                std::string datadump;
                dump_memory(databuf+threaddata->data_begain_offset, threaddata->data_size, datadump);

                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                        "handle_sorryserver_recv() : before memmove (data dump) : "
                                        "data begin = %d, data_size = %d, data = %s");
                formatter % threaddata->data_begain_offset % threaddata->data_size % datadump;
                putLogDebug(300113, formatter.str(), __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/

            // move the data from data start pos to buffer start pos
            memmove(databuf, databuf+threaddata->data_begain_offset, threaddata->data_size);

            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                std::string datadump;
                dump_memory(databuf, threaddata->data_size, datadump);

                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                        "handle_sorryserver_recv() : after memmove (data dump) : "
                                        "data begin = 0, data_size = %d, data = %s");
                formatter % threaddata->data_size % datadump;
                putLogDebug(300114, formatter.str(), __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/
        }

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            std::string datadump;
            dump_memory(recvbuffer.data(), recvlen, datadump);

            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                    "handle_sorryserver_recv() : before memcpy (data dump) : "
                                    "data begin = 0, data_size = %d, data = %s");
            formatter % recvlen % datadump;
            putLogDebug(300115, formatter.str(), __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/

        // copy new recv data to data_buffer
        memcpy(databuf+threaddata->data_size, recvbuffer.data(), recvlen);

        // add data size
        threaddata->data_size += recvlen;
        threaddata->data_begain_offset = 0;

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            std::string datadump;
            dump_memory(databuf + threaddata->data_size - recvlen, recvlen, datadump);

            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                    "handle_sorryserver_recv() : after memcpy (data dump) : "
                                    "data begin = 0, data_size = %d, data = %s");
            formatter % recvlen % datadump;
            putLogDebug(300116, formatter.str(), __FILE__, __LINE__);
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
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                        "handle_sorryserver_recv() : check_ssl_record_sendable() end. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogDebug(300117, formatter.str(), __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/

            if (ret == -1)
            {
                putLogError(300073, "record data is null.", __FILE__, __LINE__);

                // anomaly, set the end_flag ON
                threaddata->end_flag = END_FLAG_ON;
                status = FINALIZE;
                /*-------- DEBUG LOG --------*/
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                            "handle_sorryserver_recv() : END_FLAG_ON. thread id : %d.");
                    formatter % boost::this_thread::get_id();
                    putLogDebug(300118, formatter.str(), __FILE__, __LINE__);
                }
                /*------DEBUG LOG END------*/
            }
            else if (ret == 1)
            {
                // the data can't be sent, set the status SORRYSERVER_RECV
                status = SORRYSERVER_RECV;
            }
            else
            {
                // the data can be sent, set the status CLIENT_CONNECTION_CHECK
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

        // set last status
        threaddata->last_status = status;
    }
    catch (const std::exception& e)
    {
        std::cerr << "protocol_module_sslid::handle_sorryserver_recv() : exception : error = " << e.what() << "." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_sorryserver_recv() : exception : error = %s. thread id : %d.");
        formatter % e.what() % boost::this_thread::get_id();
        putLogError(300074, formatter.str(), __FILE__, __LINE__);

        status = FINALIZE;
    }
    catch (...)
    {
        std::cerr << "protocol_module_sslid::handle_sorryserver_recv() : Unknown exception." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_sorryserver_recv() : Unknown exception. thread id : %d.");
        formatter % boost::this_thread::get_id();
        putLogError(300075, formatter.str(), __FILE__, __LINE__);

        status = FINALIZE;
    }

    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_sorryserver_recv(const boost::thread::id thread_id, "
                                "const boost::asio::ip::tcp::endpoint& sorry_endpoint, "
                                "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                "const size_t recvlen) : return_value = %d, end_flag = %d. thread id : %d.");
        formatter % status % (threaddata.get() ? threaddata->end_flag : END_FLAG_ON) % boost::this_thread::get_id();
        putLogDebug(300119, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    return status;
}

//! called from UPSTEEARM thread. make module original message.
//! @param[in] downstream thread id.
//! @return session use EVENT mode(STOP).
protocol_module_base::EVENT_TAG protocol_module_sslid::handle_response_send_inform(
    const boost::thread::id thread_id)
{
    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("in/out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_response_send_inform(const boost::thread::id thread_id) : return_value = %d. "
                "thread id : %d.");
        formatter % STOP % boost::this_thread::get_id();
        putLogDebug(300120, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    return STOP;
}

//! called from after client connection check. use TCP/IP only. create client send message.
//! @param[in]    downstream thread id
//! @param[out]    send budffer reference
//! @param[out]    send data length
//! @return     session use EVENT mode(CLIENT_SEND, FINALIZE).
protocol_module_base::EVENT_TAG protocol_module_sslid::handle_client_connection_check(
    const boost::thread::id thread_id,
    boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
    size_t& datalen)
{
    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_client_connection_check(const boost::thread::id thread_id, "
                                "boost::array<char,MAX_BUFFER_SIZE>& sendbuffer, "
                                "size_t& datalen) : thread_id = %d.");
        formatter % thread_id;
        putLogDebug(300121, formatter.str(), __FILE__, __LINE__);
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
            if (unlikely(it == session_thread_data_map.end() || it->second == NULL))
            {
                boost::format formatter("Invalid thread id. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogError(300076, formatter.str(), __FILE__, __LINE__);

                /*-------- DEBUG LOG --------*/
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                            "handle_client_connection_check(const boost::thread::id thread_id, "
                                            "boost::array<char,MAX_BUFFER_SIZE>& sendbuffer, "
                                            "size_t& datalen) : return_value = %d. thread id : %d.");
                    formatter % FINALIZE % boost::this_thread::get_id();
                    putLogDebug(300122, formatter.str(), __FILE__, __LINE__);
                }
                /*------DEBUG LOG END------*/

                return FINALIZE;
            }

            threaddata = it->second;
        }

        // check sorry state
        if(threaddata->sorry_flag == SORRY_FLAG_OFF)
        {
            // serverhello message
            if (threaddata->hello_message_flag)
            {
                threaddata->hello_message_flag = false;
                // it's server hello message
                // get sessionid from the data buffer
                std::string session_id;
                if (get_ssl_session_id(threaddata->data_buffer.data()+threaddata->data_begain_offset,
                                       threaddata->data_size, session_id) == -1)
                {
                    /*-------- DEBUG LOG --------*/
                    if (unlikely(LOG_LV_DEBUG == getloglevel()))
                    {
                        boost::format formatter("out_function : protocol_module_base::EVENT_TAG "
                                                "protocol_module_sslid::handle_client_connection_check("
                                                "const boost::thread::id thread_id, "
                                                "boost::array<char,MAX_BUFFER_SIZE>& sendbuffer, "
                                                "size_t& datalen) : return_value = %d. thread id : %d.");
                        formatter % FINALIZE % boost::this_thread::get_id();
                        putLogDebug(300123, formatter.str(), __FILE__, __LINE__);
                    }
                    /*------DEBUG LOG END------*/
                }

                /*-------- DEBUG LOG --------*/
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                            "handle_client_connection_check() : get_ssl_session_id() end. thread id : %d.");
                    formatter % boost::this_thread::get_id();
                    putLogDebug(300124, formatter.str(), __FILE__, __LINE__);
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
                    if (unlikely(LOG_LV_DEBUG == getloglevel()))
                    {
                        std::string buffer;
                        dump_session_id(session_id.c_str(), session_id.size(), buffer);
                        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                                "handle_client_connection_check() : write_session_data() end."
                                                "session_id = %s. thread id : %d.");
                        formatter % buffer % boost::this_thread::get_id();
                        putLogDebug(300125, formatter.str(), __FILE__, __LINE__);
                    }
                    /*------DEBUG LOG END------*/
                }
            }
        }

        // put data to send
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
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                    "handle_client_connection_check() : put_data_to_sendbuffer() end. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogDebug(300126, formatter.str(), __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/

        // set last status
        threaddata->last_status = status;
    }
    catch (const std::exception& e)
    {
        std::cerr << "protocol_module_sslid::handle_client_connection_check() : exception : error = " << e.what() << "." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_client_connection_check() : exception : error = %s. thread id : %d.");
        formatter % e.what() % boost::this_thread::get_id();
        putLogError(300077, formatter.str(), __FILE__, __LINE__);

        status = FINALIZE;
    }
    catch (...)
    {
        std::cerr << "protocol_module_sslid::handle_client_connection_check() : Unknown exception." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_client_connection_check() : Unknown exception. thread id : %d.");
        formatter % boost::this_thread::get_id();
        putLogError(300078, formatter.str(), __FILE__, __LINE__);

        status = FINALIZE;
    }

    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_client_connection_check(const boost::thread::id thread_id, "
                                "boost::array<char,MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen) : "
                                "return_value = %d. thread id : %d.");
        formatter % status % boost::this_thread::get_id();
        putLogDebug(300127, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    return status;
}

//! called from after client select. use UDP only
//! @param[in] downstream thread id
//! @param[in] client udp endpoint
//! @param[out]    send buffer reference
//! @param[out]    send data length
//! @return     session use EVENT mode(STOP).
protocol_module_base::EVENT_TAG protocol_module_sslid::handle_client_select(
    const boost::thread::id thread_id,
    boost::asio::ip::udp::endpoint& cl_endpoint,
    boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
    size_t& datalen)
{
    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("in/out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_client_select(const boost::thread::id thread_id, "
                                "boost::asio::ip::udp::endpoint& cl_endpoint, "
                                "boost::array<char,MAX_BUFFER_SIZE>& sendbuffer, "
                                "size_t& datalen) : return_value = %d. thread id : %d.");
        formatter % STOP % boost::this_thread::get_id();
        putLogDebug(300128, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    return STOP;
}

//! called from after client send
//! @param[in] downstream thread id
//! @return     session use EVENT mode(CLIENT_DISCONNECT,
//!              REALSERVER_RECV, CLIENT_CONNECTION_CHECK, FINALIZE).
protocol_module_base::EVENT_TAG protocol_module_sslid::handle_client_send(
    const boost::thread::id thread_id)
{
    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_client_send(const boost::thread::id thread_id) : thread_id = %d.");
        formatter % thread_id;
        putLogDebug(300129, formatter.str(), __FILE__, __LINE__);
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
            if (unlikely(it == session_thread_data_map.end() || it->second == NULL))
            {
                boost::format formatter("Invalid thread id. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogError(300079, formatter.str(), __FILE__, __LINE__);
                /*-------- DEBUG LOG --------*/
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                            "handle_client_send(const boost::thread::id thread_id) : return_value = %d. thread id : %d.");
                    formatter % FINALIZE % boost::this_thread::get_id();
                    putLogDebug(300130, formatter.str(), __FILE__, __LINE__);
                }
                /*------DEBUG LOG END------*/

                return FINALIZE;
            }

            threaddata = it->second;
        }

        if (threaddata->data_size <= 0)
        {
            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                        "handle_client_send() : data_size = %d, end_flag = %d. thread id : %d.");
                formatter % threaddata->data_size % threaddata->end_flag % boost::this_thread::get_id();
                putLogDebug(300131, formatter.str(), __FILE__, __LINE__);
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
                // end_flag is OFF, check sorry state
                if (threaddata->sorry_flag == SORRY_FLAG_ON)
                {
                    // sorry flag is on
                    status = SORRYSERVER_RECV;
                }
                else
                {
                    // sorry flag is off
                    status = REALSERVER_RECV;
                }
            }
        }
        else
        {
            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                        "handle_client_send() : data_size = %d, current_record_rest_size = %d. thread id : %d.");
                formatter % threaddata->data_size % threaddata->current_record_rest_size % boost::this_thread::get_id();
                putLogDebug(300132, formatter.str(), __FILE__, __LINE__);
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
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                            "handle_client_send() : check_ssl_record_sendable() end. "
                                            "return_value = %d. thread id : %d.");
                    formatter % ret % boost::this_thread::get_id();
                    putLogDebug(300133, formatter.str(), __FILE__, __LINE__);
                }
                /*------DEBUG LOG END------*/

                if (ret == -1)
                {
                    putLogError(300080, "record data is null.", __FILE__, __LINE__);

                    // anomaly, set the end_flag ON
                    threaddata->end_flag = END_FLAG_ON;
                    /*-------- DEBUG LOG --------*/
                    if (unlikely(LOG_LV_DEBUG == getloglevel()))
                    {
                        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                                "handle_client_send() : END_FLAG_ON. thread id : %d.");
                        formatter % boost::this_thread::get_id();
                        putLogDebug(300134, formatter.str(), __FILE__, __LINE__);
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
                        // end_flag is OFF, check sorry state
                        if (threaddata->sorry_flag == SORRY_FLAG_ON)
                        {
                            // sorry flag is on
                            status = SORRYSERVER_RECV;
                        }
                        else
                        {
                            // sorry flag is off
                            status = REALSERVER_RECV;
                        }
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

        // set last status
        threaddata->last_status = status;
    }
    catch (const std::exception& e)
    {
        std::cerr << "protocol_module_sslid::handle_client_send() : exception : error = " << e.what() << "." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_client_send() : exception : error = %s. thread id : %d.");
        formatter % e.what() % boost::this_thread::get_id();
        putLogError(300081, formatter.str(), __FILE__, __LINE__);

        status = FINALIZE;
    }
    catch (...)
    {
        std::cerr << "protocol_module_sslid::handle_client_send() : Unknown exception." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_client_send() : Unknown exception. thread id : %d.");
        formatter % boost::this_thread::get_id();
        putLogError(300082, formatter.str(), __FILE__, __LINE__);

        status = FINALIZE;
    }

    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_client_send(const boost::thread::id thread_id) : "
                                "return_value = %d, end_flag = %d. thread id : %d.");
        formatter % status % (threaddata.get() ? threaddata->end_flag : END_FLAG_ON) % boost::this_thread::get_id();
        putLogDebug(300135, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    return status;
}

//! call from client disconnect event. use upstream thread and downstream thread.
//! @param[in] upstream and downstream thread id(check! one thread one event!)
//! @return     session use EVENT mode(FINALIZE).
protocol_module_base::EVENT_TAG protocol_module_sslid::handle_client_disconnect(
    const boost::thread::id thread_id)
{
    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_client_disconnect(const boost::thread::id thread_id) : "
                                "thread_id = %d.");
        formatter % thread_id;
        putLogDebug(300136, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    thread_data_ptr threaddata;
    session_thread_data_map_type::iterator session_thread_it;

    try
    {
        {
            boost::mutex::scoped_lock slock(session_thread_data_map_mutex);

            session_thread_it = session_thread_data_map.find(thread_id);
            if (unlikely(session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL))
            {
                boost::format formatter("Invalid thread id. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogError(300083, formatter.str(), __FILE__, __LINE__ );
                throw -1;
            }

            threaddata = session_thread_it->second;
        }

        //set last status
        threaddata->last_status = FINALIZE;
    }
    catch (int e)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                    "handle_client_disconnect() : catch exception e = %d. thread id : %d.");
            formatter % e % boost::this_thread::get_id();
            putLogDebug(300137, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/
    }
    catch (const std::exception& ex)
    {
        std::cerr << "protocol_module_sslid::handle_client_disconnect() : exception : error = " << ex.what() << "." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_client_disconnect() : exception : error = %s. thread id : %d.");
        formatter % ex.what() % boost::this_thread::get_id();
        putLogError(300084, formatter.str(), __FILE__, __LINE__ );
    }
    catch (...)
    {
        std::cerr << "protocol_module_sslid::handle_client_disconnect() : Unknown exception." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG "
                                "protocol_module_sslid::handle_client_disconnect() : "
                                "Unknown exception. thread id : %d.");
        formatter % boost::this_thread::get_id();
        putLogError(300085, formatter.str(), __FILE__, __LINE__ );
    }

    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_client_disconnect(const boost::thread::id thread_id) : "
                                "thread_id = %d, return_value = %d.");
        formatter % thread_id % FINALIZE;
        putLogDebug(300138, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    return FINALIZE;
}

//! call from sorry mode event. use upstream thread and downstream thread
//! @param[in] upstream and downstream thread id(check! one thread one event and first time call pattern)
//! @return     session use EVENT mode.
protocol_module_base::EVENT_TAG protocol_module_sslid::handle_sorry_enable(
    const boost::thread::id thread_id)
{
    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_sorry_enable(const boost::thread::id thread_id) : thread id : %d.");
        formatter % boost::this_thread::get_id();
        putLogDebug(300139, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    EVENT_TAG status = ACCEPT;
    thread_data_ptr threaddata;
    session_thread_data_map_type::iterator session_thread_it;

    try
    {
        {
            boost::mutex::scoped_lock slock(session_thread_data_map_mutex);

            session_thread_it = session_thread_data_map.find(thread_id);
            if (unlikely(session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL))
            {
                boost::format formatter("Invalid thread id. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogError(300086, formatter.str(), __FILE__, __LINE__ );
                throw -1;
            }

            threaddata = session_thread_it->second;
        }

        //up thread
        if (threaddata->thread_division == THREAD_DIVISION_UP_STREAM)
        {
            //accept_flag is off
            if (threaddata->accept_flag == ACCEPT_END_FLAG_OFF)
            {
                //set return status
                status = ACCEPT;
            }
            //accept_flag is on
            else
            {
                if (threaddata->sorry_flag == SORRY_FLAG_ON)
                {
                    // set return status
                    status = threaddata->last_status;
                }
                else
                {
                    // set end flag on
                    threaddata->end_flag = END_FLAG_ON;

                    /*-------- DEBUG LOG --------*/
                    if (unlikely(LOG_LV_DEBUG == getloglevel()))
                    {
                    boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                            "handle_sorry_enable(const boost::thread::id thread_id) : END_FLAG_ON. thread id : %d.");
                    formatter % boost::this_thread::get_id();
                    putLogDebug(300140, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/

                    // set return status
                    status = CLIENT_DISCONNECT;
                }
            }
        }
        //down thread
        else
        {
            threaddata->last_status = threaddata->last_status == REALSERVER_RECV ? SORRYSERVER_RECV : threaddata->last_status;
            // set return status
            status = threaddata->last_status;
        }

        // set sorry flag on
        threaddata->sorry_flag = SORRY_FLAG_ON;

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                "handle_sorry_enable(const boost::thread::id thread_id) : SORRY_FLAG_ON. thread id : %d.");
        formatter % boost::this_thread::get_id();
        putLogDebug(300141, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        // set last status
        threaddata->last_status = status;
    }
    catch (int e)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                    "handle_sorry_enable() : catch exception e = %d. thread id : %d.");
            formatter % e % boost::this_thread::get_id();
            putLogDebug(300142, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        // set return status
    status = FINALIZE;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "protocol_module_sslid::handle_sorry_enable() : exception : error = " << ex.what() << "." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_sorry_enable() : exception : error = %s. thread id : %d.");
        formatter % ex.what() % boost::this_thread::get_id();
        putLogError(300087, formatter.str(), __FILE__, __LINE__ );

        // set return status
    status = FINALIZE;
    }
    catch (...)
    {
        std::cerr << "protocol_module_sslid::handle_sorry_enable() : Unknown exception." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG "
                                "protocol_module_sslid::handle_sorry_enable() : "
                                "Unknown exception. thread id : %d.");
        formatter % boost::this_thread::get_id();
        putLogError(300088, formatter.str(), __FILE__, __LINE__ );

        // set return status
    status = FINALIZE;
    }

    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_sorry_enable(const boost::thread::id thread_id) : "
                                "thread_id = %d, return_value = %d.");
        formatter % thread_id % status;
        putLogDebug(300143, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    return status;
}

//! call from sorry mode disable. use upstream thread and downstream thread.
//! @param[in] upstream and downstream thread id(check! one thread one event)
//! @return     session use EVENT mode.
protocol_module_base::EVENT_TAG protocol_module_sslid::handle_sorry_disable(
    const boost::thread::id thread_id)
{
    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("in/out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_sorry_disable(const boost::thread::id thread_id) : thread id : %d.");
        formatter % boost::this_thread::get_id();
        putLogDebug(300144, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    EVENT_TAG status = ACCEPT;
    thread_data_ptr threaddata;
    session_thread_data_map_type::iterator session_thread_it;

    try
    {
        {
            boost::mutex::scoped_lock slock(session_thread_data_map_mutex);

            session_thread_it = session_thread_data_map.find(thread_id);
            if (unlikely(session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL))
            {
                boost::format formatter("Invalid thread id. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogError(300089, formatter.str(), __FILE__, __LINE__ );
                throw -1;
            }

            threaddata = session_thread_it->second;
        }

        //up thread
        if (threaddata->thread_division == THREAD_DIVISION_UP_STREAM)
        {
            //accept_flag is off
            if (threaddata->accept_flag == ACCEPT_END_FLAG_OFF)
            {
                //set return status
                status = ACCEPT;
            }
            //accept_flag is on
            else
            {
                if (threaddata->sorry_flag == SORRY_FLAG_ON)
                {
                    // set end flag on
                    threaddata->end_flag = END_FLAG_ON;

                    /*-------- DEBUG LOG --------*/
                    if (unlikely(LOG_LV_DEBUG == getloglevel()))
                    {
                    boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                            "handle_sorry_disable(const boost::thread::id thread_id) : END_FLAG_ON. thread id : %d.");
                    formatter % boost::this_thread::get_id();
                    putLogDebug(300145, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/

                    // set return status
                    status = CLIENT_DISCONNECT;
                }
                else
                {
                    threaddata->last_status = threaddata->last_status == SORRYSERVER_RECV ? REALSERVER_RECV : threaddata->last_status;
                    // set return status
                    status = threaddata->last_status;
                }
            }
        }
        //down thread
        else
        {
            // set return status
            status = threaddata->last_status;
        }

        // set sorry flag off
        threaddata->sorry_flag = SORRY_FLAG_OFF;

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                "handle_sorry_disable(const boost::thread::id thread_id) : SORRY_FLAG_OFF. thread id : %d.");
        formatter % boost::this_thread::get_id();
        putLogDebug(300146, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        // set last status
        threaddata->last_status = status;
    }
    catch (int e)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                    "handle_sorry_disable() : catch exception e = %d. thread id : %d.");
            formatter % e % boost::this_thread::get_id();
            putLogDebug(300147, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        // set return status
    status = FINALIZE;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "protocol_module_sslid::handle_sorry_disable() : exception : error = " << ex.what() << "." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_sorry_disable() : exception : error = %s. thread id : %d.");
        formatter % ex.what() % boost::this_thread::get_id();
        putLogError(300090, formatter.str(), __FILE__, __LINE__ );

        // set return status
    status = FINALIZE;
    }
    catch (...)
    {
        std::cerr << "protocol_module_sslid::handle_sorry_disable() : Unknown exception." << std::endl;
        boost::format formatter("function : protocol_module_base::EVENT_TAG "
                                "protocol_module_sslid::handle_sorry_disable() : "
                                "Unknown exception. thread id : %d.");
        formatter % boost::this_thread::get_id();
        putLogError(300091, formatter.str(), __FILE__, __LINE__ );

        // set return status
    status = FINALIZE;
    }

    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_sorry_disable(const boost::thread::id thread_id) : "
                                "thread_id = %d, return_value = %d.");
        formatter % thread_id % status;
        putLogDebug(300148, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    return status;
}

//! call from realserver disconnect. use upstream thread and downstream thread
//! @param[in] upstream and downstream thread id(check! one thread one event)
//! @param[in] disconnected realserver endpoint.
//! @return     session use EVENT mode(FINALIZE).
protocol_module_base::EVENT_TAG protocol_module_sslid::handle_realserver_disconnect(
    const boost::thread::id thread_id,
    const boost::asio::ip::tcp::endpoint& sorry_endpoint)
{
    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("in/out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_realserver_disconnect(const boost::thread::id thread_id, "
                                "const boost::asio::ip::tcp::endpoint& rs_endpoint) : return_value = %d. thread id : %d.");
        formatter % FINALIZE % boost::this_thread::get_id();
        putLogDebug(300149, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    return FINALIZE;
}

//! call from sorry server disconnect. use upstraem thread and downstream thread
//! @param[in] upstream and downstream thread id(check! one thread one event)
//! @param[in] disconnect sorryserver endpoint
//! @return session use EVENT mode(FINALIZE).
protocol_module_base::EVENT_TAG protocol_module_sslid::handle_sorryserver_disconnect(
    const boost::thread::id thread_id,
    const boost::asio::ip::tcp::endpoint& sorry_endpoint)
{
    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("in/out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_sorryserver_disconnect(const boost::thread::id thread_id, "
                                "const boost::asio::ip::tcp::endpoint& sorry_endpoint) : return_value = %d. thread id : %d.");
        formatter % FINALIZE % boost::this_thread::get_id();
        putLogDebug(300150, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    return FINALIZE;
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
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("in/out_function : protocol_module_base::EVENT_TAG protocol_module_sslid::"
                                "handle_realserver_close(const boost::thread::id thread_id, "
                                "const boost::asio::ip::udp::endpoint& rs_endpoint) : return_value = %d. thread id : %d.");
        formatter % STOP % boost::this_thread::get_id();
        putLogDebug(300151, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    return STOP;
}

//! replication interval interrrupt
//! timer thread call this function. from virtualservice.
void protocol_module_sslid::replication_interrupt()
{
    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        putLogDebug(300152, "in_function : void protocol_module_sslid::replication_interrupt().", __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/
    if (replication_data_processor)
    {
        replication_data_processor->write_replicaion_area();
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            putLogDebug(300153, "function : void protocol_module_sslid::replication_interrupt() : "
                                "write_replication_area() end.", __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/
    }
    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        putLogDebug(300154, "out_function : void protocol_module_sslid::replication_interrupt().", __FILE__, __LINE__);
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
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("in_function : int protocol_module_sslid::put_data_to_sendbuffer("
                                "const boost::thread::id& thread_id, "
                                "boost::array<char,MAX_BUFFER_SIZE>& sendbuffer, "
                                "size_t& datalen) : thread_id = %d.");
        formatter % thread_id;
        putLogDebug(300155, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    // find session_thread_data_sslid's pointer from map
    thread_data_ptr threaddata;
    datalen = 0;

    {
        boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
        session_thread_data_map_type::iterator it = session_thread_data_map.find(thread_id);
        if (unlikely(it == session_thread_data_map.end() || it->second == NULL))
        {
            boost::format formatter("Invalid thread id. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(300092, formatter.str(), __FILE__, __LINE__);

            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("out_function : int protocol_module_sslid::put_data_to_sendbuffer("
                                        "const boost::thread::id& thread_id, "
                                        "boost::array<char,MAX_BUFFER_SIZE>& sendbuffer, "
                                        "size_t& datalen) : return_value = -1. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogDebug(300156, formatter.str(), __FILE__, __LINE__);
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
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("function : int protocol_module_sslid::put_data_to_sendbuffer() "
                                    ": current_record_rest_size >= data_size. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogDebug(300157, formatter.str(), __FILE__, __LINE__);
        }

        // remain data size greater than all data size
        if (threaddata->data_size <= sendbufsize)
        {
            // dump data
            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                std::string datadump;
                dump_memory(databegin+threaddata->data_begain_offset,
                            threaddata->data_size, datadump);

                boost::format formatter("function : int protocol_module_sslid::"
                                        "put_data_to_sendbuffer() : data_size <= sendbuffer size. "
                                        "before memcpy (data dump) : data_begin_offset = %d, "
                                        "current_record_rest_size = %d, dump data begin = 0, "
                                        "dump data_size = %d, dump data = %s");
                formatter % threaddata->data_begain_offset % threaddata->current_record_rest_size
                % threaddata->data_size % datadump;
                putLogDebug(300158, formatter.str(), __FILE__, __LINE__);
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
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                std::string datadump;
                dump_memory(senddatabegin, datalen, datadump);

                boost::format formatter("function : int protocol_module_sslid::"
                                        "put_data_to_sendbuffer() : data_size <= sendbuffer size. "
                                        "after memcpy (data dump) : data_begin_offset = %d, "
                                        "current_record_rest_size = %d, dump data begin = 0, "
                                        "dump data_size = %d, dump data = %s");
                formatter % threaddata->data_begain_offset % threaddata->current_record_rest_size
                % datalen % datadump;
                putLogDebug(300159, formatter.str(), __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/
        }
        else
        {
            // dump data
            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                std::string datadump;
                dump_memory(databegin+threaddata->data_begain_offset, sendbufsize, datadump);

                boost::format formatter("function : int protocol_module_sslid::"
                                        "put_data_to_sendbuffer() : data_size > sendbuffer size. "
                                        "before memcpy (data dump) : data_begin_offset = %d, "
                                        "current_record_rest_size = %d, dump data begin = 0, "
                                        "dump data_size = %d, dump data = %s");
                formatter % threaddata->data_begain_offset % threaddata->current_record_rest_size
                % sendbufsize % datadump;
                putLogDebug(300160, formatter.str(), __FILE__, __LINE__);
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
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                std::string datadump;
                dump_memory(senddatabegin, datalen, datadump);

                boost::format formatter("function : int protocol_module_sslid::"
                                        "put_data_to_sendbuffer() : data_size > sendbuffer size. "
                                        "after memcpy (data dump) : data_begin_offset = %d, "
                                        "current_record_rest_size = %d, dump data begin = 0, "
                                        "dump data_size = %d, dump data = %s");
                formatter % threaddata->data_begain_offset % threaddata->current_record_rest_size
                % datalen % datadump;
                putLogDebug(300161, formatter.str(), __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/
        }
    }
    else
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("function : int protocol_module_sslid::put_data_to_sendbuffer() "
                                    ": current_record_rest_size < data_size. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogDebug(300162, formatter.str(), __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/

        // remain data size less than all data size
        if  (threaddata->current_record_rest_size <= sendbufsize)
        {
            // dump data
            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                std::string datadump;
                dump_memory(databegin+threaddata->data_begain_offset, threaddata->current_record_rest_size, datadump);

                boost::format formatter("function : int protocol_module_sslid::"
                                        "put_data_to_sendbuffer() : current_record_rest_size <= sendbuffer size. "
                                        "before memcpy (data dump) : data_begin_offset = %d, "
                                        "current_record_rest_size = %d, dump data begin = 0, "
                                        "dump data_size = %d, dump data = %s");
                formatter % threaddata->data_begain_offset % threaddata->current_record_rest_size
                % threaddata->current_record_rest_size % datadump;
                putLogDebug(300163, formatter.str(), __FILE__, __LINE__);
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
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                std::string datadump;
                dump_memory(senddatabegin, datalen, datadump);

                boost::format formatter("function : int protocol_module_sslid::"
                                        "put_data_to_sendbuffer() : current_record_rest_size <= sendbuffer size. "
                                        "after memcpy (data dump) : data_begin_offset = %d, "
                                        "current_record_rest_size = %d, dump data begin = 0, "
                                        "dump data_size = %d, dump data = %s");
                formatter % threaddata->data_begain_offset % threaddata->current_record_rest_size
                % datalen % datadump;
                putLogDebug(300164, formatter.str(), __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/
        }
        else
        {
            // data dump
            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                std::string datadump;
                dump_memory(databegin+threaddata->data_begain_offset, sendbufsize, datadump);

                boost::format formatter("function : int protocol_module_sslid::"
                                        "put_data_to_sendbuffer() : current_record_rest_size > sendbuffer size. "
                                        "before memcpy (data dump) : data_begin_offset = %d, "
                                        "current_record_rest_size = %d, dump data begin = 0, "
                                        "dump data_size = %d, dump data = %s");
                formatter % threaddata->data_begain_offset % threaddata->current_record_rest_size
                % sendbufsize % datadump;
                putLogDebug(300165, formatter.str(), __FILE__, __LINE__);
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
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                std::string datadump;
                dump_memory(senddatabegin, datalen, datadump);

                boost::format formatter("function : int protocol_module_sslid::"
                                        "put_data_to_sendbuffer() : current_record_rest_size > sendbuffer size. "
                                        "after memcpy (data dump) : data_begin_offset = %d, "
                                        "current_record_rest_size = %d, dump data begin = 0, "
                                        "dump data_size = %d, dump data = %s");
                formatter % threaddata->data_begain_offset % threaddata->current_record_rest_size
                % datalen % datadump;
                putLogDebug(300166, formatter.str(), __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/
        }
    }

    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("out_function : int protocol_module_sslid::put_data_to_sendbuffer("
                                "const boost::thread::id& thread_id, "
                                "boost::array<char,MAX_BUFFER_SIZE>& sendbuffer, "
                                "size_t& datalen) : return_value = 0. thread id : %d.");
        formatter % boost::this_thread::get_id();
        putLogDebug(300167, formatter.str(), __FILE__, __LINE__);
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
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("in_function : bool protocol_module_sslid::realserver_selected("
                                "const boost::asio::ip::tcp::endpoint& rs_endpoint) : rs_endpoint = [%s]:%d.");
        formatter % rs_endpoint.address().to_string() % rs_endpoint.port();
        putLogDebug(300168, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    boost::asio::ip::tcp::endpoint temp_endpoint;
    if (temp_endpoint ==  rs_endpoint)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            putLogDebug(300169, "out_function : bool protocol_module_sslid::realserver_selected("
                                "const boost::asio::ip::tcp::endpoint& rs_endpoint) : return_value = false."
                                , __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/

        // equals to default endpoint
        return false;
    }
    else
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            putLogDebug(300170, "out_function : bool protocol_module_sslid::realserver_selected("
                                "const boost::asio::ip::tcp::endpoint& rs_endpoint) : return_value = true."
                                , __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/
        // already connected
        return true;
    }
}

}

extern "C" l7vs::protocol_module_base*
create_module(){
    return dynamic_cast<l7vs::protocol_module_base*>(new l7vs::protocol_module_sslid());
}

extern "C" void
destroy_module( l7vs::protocol_module_base* in ){
    delete in;
}
