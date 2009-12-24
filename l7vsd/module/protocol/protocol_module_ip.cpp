/*
 * @file  protocol_module_ip.cpp
 * @brief protocol module of any protocol.
 * @brief this module never keep session persistence.
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

#include <vector>
#include <list>
#include <algorithm>
#include <iostream>
#include <boost/asio/ip/tcp.hpp>
#include <boost/format.hpp>
#include <boost/xpressive/xpressive.hpp>
#include "protocol_module_ip.h"
#include "http_utility.h"
#include "utility.h"

namespace l7vs
{

    const std::string protocol_module_ip::MODULE_NAME = "ip";
    const int protocol_module_ip::THREAD_DIVISION_UP_STREAM = 0;
    const int protocol_module_ip::THREAD_DIVISION_DOWN_STREAM = 1;

    const int protocol_module_ip::END_FLAG_OFF = 0;
    const int protocol_module_ip::END_FLAG_ON = 1;

    const int protocol_module_ip::ACCEPT_END_FLAG_OFF = 0;
    const int protocol_module_ip::ACCEPT_END_FLAG_ON = 1;

    const int protocol_module_ip::SORRY_FLAG_ON = 1;
    const int protocol_module_ip::SORRY_FLAG_OFF = 0;

    const int protocol_module_ip::SWITCH_FLAG_OFF = 0;
    const int protocol_module_ip::SWITCH_FLAG_ON = 1;

    const int protocol_module_ip::FORWARDED_FOR_OFF = 0;
    const int protocol_module_ip::FORWARDED_FOR_ON = 1;

    using namespace boost::xpressive;
    //! constractor
    protocol_module_ip::protocol_module_ip() :
            ip_protocol_module_base(MODULE_NAME), forwarded_for(FORWARDED_FOR_OFF)
    {
        sorry_uri.assign('\0');
        sorry_uri[0] = '/';
    }
    //! destractor
    protocol_module_ip::~protocol_module_ip()
    {
    }
    //! tcp protocol support check
    //! @return tcp support is true
    //! @return tcp not-support is false
    bool protocol_module_ip::is_tcp()
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            putLogDebug(600000, "in/out_function : bool protocol_module_ip::is_tcp() : return_value = true.",
                        __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/
        return true;
    }

    //! udp protocol support check
    //! @return udp support is true
    //! @return udp not-support is false
    bool protocol_module_ip::is_udp()
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            putLogDebug(600001, "in/out_function : bool protocol_module_ip::is_udp() : return_value = false.",
                        __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/
        return false;
    }

    //! replication interval interrrupt
    //! timer thread call this function. from virtualservice.
    void protocol_module_ip::replication_interrupt()
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            putLogDebug(600002, "in_function : protocol_module_ip::replication_interrupt().", __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/
        if (replication_data_processor)
        {
            replication_data_processor->write_replication_area();
            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                putLogDebug(600003, "function : protocol_module_ip::replication_interrupt() : "
                            "write_replication_area() end.", __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/
        }
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            putLogDebug(600004, "out_function : void protocol_module_ip::replication_interrupt().", __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/
    }
    //! initialize function. called from module control. module loaded call
    //! @param[in]    realserver list iterator begin function object type
    //!    @param[in]    realserver list iterator end function object type
    //! @param[in]    realserver list iterator next function object type
    //! @param[in]    realserver list mutex lock function object type.
    //! @param[in]    realserver list mutex unlock function object type
    void protocol_module_ip::initialize(rs_list_itr_func_type    inlist_begin,
                                        rs_list_itr_func_type    inlist_end,
                                        rs_list_itr_next_func_type    inlist_next,
                                        boost::function< void( void ) >    inlist_lock,
                                        boost::function< void( void ) >    inlist_unlock)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            putLogDebug(600005, "in_function : void protocol_module_ip::initialize("
                        "rs_list_itr_func_type inlist_begin, rs_list_itr_func_type inlist_end, "
                        "rs_list_itr_next_func_type inlist_next, boost::function< void(void) > "
                        "inlist_lock, boost::function< void(void) > inlist_unlock).", __FILE__, __LINE__);
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
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            putLogDebug(600006, "out_function : void protocol_module_ip::initialize("
                        "rs_list_itr_func_type inlist_begin, rs_list_itr_func_type inlist_end, "
                        "rs_list_itr_next_func_type inlist_next, boost::function< void(void) > "
                        "inlist_lock, boost::function< void(void) > inlist_unlock).", __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/
    }

    //! finalize called from module control. module unloaded call.
    void protocol_module_ip::finalize()
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            putLogDebug(600007, "in_function : void protocol_module_ip::finalize().", __FILE__, __LINE__);
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

        //Replication functions initializtion
        //component memory allcate function
        replication_pay_memory.clear();
        //component memory lock function
        replication_area_lock.clear();
        //component memory unlock function
        replication_area_unlock.clear();

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            putLogDebug(600008, "function : void protocol_module_ip::finalize() : "
                        "rs_list_begin.clear(), rs_list_end.clear(), rs_list_next.clear(), "
                        "rs_list_lock.clear(), rs_list_unlock.clear() end.", __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/

        //ScheduleModule's functions initializtion
        schedule_tcp.clear();

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            putLogDebug(600009, "function : void protocol_module_ip::finalize() : "
                        "schedule_tcp.clear() end.", __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/

        //Module's option initializtion
        //forwarded_for
        forwarded_for = FORWARDED_FOR_OFF;
        //sorry-uri
        sorry_uri.assign('\0');
        //time_out
        timeout = 0;
        //reschedule flag
        reschedule = 0;

        // replication initialize
        if (replication_data_processor)
        {
            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("delete : address = &(%d).");
                formatter % static_cast<void*>(replication_data_processor);
                putLogDebug(600010, formatter.str(), __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/

            delete replication_data_processor;
            replication_data_processor = NULL;
        }

        // session initialize
        if (ip_data_processor)
        {
            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("delete : address = &(%d).");
                formatter % static_cast<void*>(ip_data_processor);
                putLogDebug(600011, formatter.str(), __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/

            delete ip_data_processor;
            ip_data_processor = NULL;
        }

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            putLogDebug(600012, "out_function : void protocol_module_ip::finalize().", __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/

        //logger functions initializtion
        //log level getting function
        getloglevel.clear();
        //logger(Fatal)
        putLogFatal.clear();
        //logger(Error)
        putLogError.clear();
        //logger(Warn)
        putLogWarn.clear();
        //logger(Info)
        putLogInfo.clear();
        //logger(Debug)
        putLogDebug.clear();
    }

    //! sorry support check
    //! @return true sorry mode is supported.
    //! @return false sorry mode is unsupported.
    bool protocol_module_ip::is_use_sorry()
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            putLogDebug(600013, "in/out_function : bool protocol_module_ip::is_use_sorry() : return_value = true.",
                        __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/
        return true;
    }

    //! realserver list update event
    void protocol_module_ip::handle_rslist_update()
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            putLogDebug(600014, "in/out_function : void protocol_module_ip::handle_rslist_update().",
                        __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/
    }

    //! module parameter check.used by l7vsadm
    //! @param[in]    module paramter string list
    //! @return    result.flag true is parameter is noproblem.
    //! @return result.flag false is paramter is problem.
    protocol_module_base::check_message_result protocol_module_ip::check_parameter(const std::vector<
            std::string>& args)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("in_function : protocol_module_ip::check_message_result "
                                    "protocol_module_ip::check_parameter("
                                    "const std::vector<std::string>& args) : args = %s.");
            std::string argsdump;
            for (std::vector<std::string>::const_iterator it = args.begin(); it != args.end(); ++it)
            {
                argsdump += *it;
            }
            formatter % argsdump;
            putLogDebug(600015, formatter.str(), __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/

        //set check result true
        // set check result flag true
        check_message_result check_result;
        check_result.flag = true;
        bool timeout_flag = false;
        bool reschedule_flag = false;
        bool no_reschedule_flag = false;
        bool forward_checked = false;
        bool sorryuri_checked = false;
        sregex    sorry_uri_regex
        =    +(    '/' >>
                   *(    alpha |
                         digit |
                         ( set = ';', ':', '@', '&', '=' ) |
                         ( set = '$', '-', '_', '.', '+' ) |
                         ( set = '!', '*', '\'', '\(', ')', ',' ) |
                         '%' >> repeat<2>(xdigit)));

        typedef std::vector<std::string>::const_iterator vec_str_it;

        try
        {
            vec_str_it it = args.begin();
            vec_str_it it_end = args.end();
            boost::format formatter;

            //loop option strings
            for (; it != it_end;)
            {
                if (*it == "-T" || *it == "--timeout")
                {
                    // timeout
                    if (!timeout_flag)
                    {
                        // not set timeout option
                        ++it;
                        // next parameter exist check
                        if (it != it_end)
                        {
                            // next parameter exist
                            if ((*it).substr(0, 1) == "-" || (*it).substr(0, 2) == "--")
                            {
                                continue;
                            }

                            try
                            {
                                unsigned long ultimeout = 0;
                                ultimeout = boost::lexical_cast<unsigned long>(*it);
                                // int maxvalue check
                                if (ultimeout > INT_MAX)
                                {
                                    check_result.flag = false;
                                    formatter.parse("'-T/--timeout' option value '%s' is too large.");
                                    formatter % *it;
                                    check_result.message = formatter.str();
                                    putLogError(600000, check_result.message, __FILE__, __LINE__);
                                    break;
                                }
                                else
                                {
                                    timeout_flag = true;
                                    ++it;
                                    continue;
                                }
                            }
                            catch (boost::bad_lexical_cast& e)
                            {
                                // not numeric character
                                check_result.flag = false;
                                formatter.parse("'-T/--timeout' option value '%s' is not numeric character.");
                                formatter % *it;
                                check_result.message = formatter.str();
                                putLogError(600001, check_result.message, __FILE__, __LINE__);
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
                        check_result.flag = false;
                        check_result.message = "Cannot set multiple option '-T/--timeout'.";
                        putLogError(600002, check_result.message, __FILE__, __LINE__);
                        break;
                    }
                }
                else if (*it == "-R" || *it == "--reschedule")
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
                        check_result.flag = false;
                        check_result.message = "You have to choose either of reschedule or no-reschedule.";
                        putLogError(600003, check_result.message, __FILE__, __LINE__);
                        break;
                    }
                }
                else if  (*it == "-N" || *it == "--no-reschedule")
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
                        check_result.flag = false;
                        check_result.message = "You have to choose either of reschedule or no-reschedule.";
                        putLogError(600004, check_result.message, __FILE__, __LINE__);
                        break;
                    }
                }
                //option string = "-F"
                else if (*it == "-F" || *it == "--forwarded-for")
                {
                    //set forward flag  ON
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
                                //set check result flag false
                                check_result.flag = false;
                                //set check result message
                                check_result.message = "You have to set option value '-S/--sorry-uri'.";
                                putLogError(600005, check_result.message, __FILE__, __LINE__ );
                                //loop break;
                                break;
                            }
                            //next option string's length > 127
                            if (it->size() > MAX_OPTION_SIZE - 1)
                            {
                                std::ostringstream ostr;
                                ostr << "'-S/--sorry-uri' option value '" << *it << "' is too long.";

                                //set check result flag false
                                check_result.flag = false;
                                //set check result message
                                check_result.message = ostr.str();
                                putLogError(600006, check_result.message, __FILE__, __LINE__ );
                                //loop break;
                                break;
                            }
                            //next option string's length <= 127
                            else
                            {
                                //regex check
                                if (regex_match(*it, sorry_uri_regex ))
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

                                    //set check result flag false
                                    check_result.flag = false;
                                    //set check result message
                                    check_result.message = ostr.str();
                                    putLogError(600007, check_result.message, __FILE__, __LINE__ );
                                    break;
                                }
                            }
                        }
                        //next item is not exist
                        else
                        {
                            //set check flag false
                            check_result.flag = false;
                            //set check result message
                            check_result.message = "You have to set option value '-S/--sorry-uri'.";
                            putLogError(600008, check_result.message, __FILE__,
                                        __LINE__ );
                            //loop break
                            break;
                        }
                    }
                    //sorryURI flag = ON
                    else
                    {
                        //set check result flag false
                        check_result.flag = false;
                        //set check result message
                        check_result.message = "Cannot set multiple option '-S/--sorry-uri'.";
                        putLogError(600009, check_result.message, __FILE__,
                                    __LINE__ );
                        //loop break
                        break;
                    }
                }
                //other option string
                else
                {
                    //set check result flag false
                    check_result.flag = false;
                    //set check result message
                    check_result.message = "Option error.";
                    putLogError(600010, check_result.message, __FILE__, __LINE__ );
                    //loop break
                    break;
                }

                ++it;
            }
        }
        catch (const std::exception& ex)
        {
            check_result.flag = false;
            std::cerr << "protocol_module_ip::check_parameter() : exception : error = " << ex.what() << "." << std::endl;
            boost::format formatter("function : protocol_module_base::check_message_result "
                                    "protocol_module_ip::check_parameter() exception : "
                                    "error = %s.");
            formatter % ex.what();
            putLogError(600011, formatter.str(), __FILE__, __LINE__ );
        }
        catch (...)
        {
            check_result.flag = false;
            std::cerr << "protocol_module_ip::check_parameter() : Unknown exception." << std::endl;
            putLogError(600012, "function : protocol_module_base::check_message_result "
                        "protocol_module_ip::check_parameter() : "
                        "Unknown exception.", __FILE__, __LINE__ );
        }

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("out_function : protocol_module_base::check_message_result "
                                    "protocol_module_ip::check_parameter("
                                    "const std::vector<std::string>& args) : return_value = ("
                                    "check_message_result.flag = %d, check_message_result.message = %s).");
            formatter % check_result.flag % check_result.message;
            putLogDebug(600016, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        return check_result;
    }

    //! parameter set
    //! @param[in] module paramter string list
    //! @return    result.flag true is parameter is noproblem.
    //! @return result.flag false is paramter is problem.
    protocol_module_base::check_message_result protocol_module_ip::set_parameter(const std::vector<
            std::string>& args)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("in_function : protocol_module_base::check_message_result "
                                    "protocol_module_ip::set_parameter("
                                    "const std::vector<std::string>& args) : args = %s.");
            std::string argsdump;
            for (std::vector<std::string>::const_iterator it = args.begin(); it != args.end(); ++it)
            {
                argsdump += *it;
                argsdump += " ";
            }
            formatter % argsdump;
            putLogDebug(600017, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        //set check result flag true
        check_message_result check_result;
        check_result.flag = true;
        bool timeout_flag = false;
        bool reschedule_flag = false;
        bool no_reschedule_flag = false;
        bool forward_checked = false;
        bool sorryuri_checked = false;
        boost::format formatter;
        sregex    sorry_uri_regex
        =    +(    '/' >>
                   *(    alpha |
                         digit |
                         ( set = ';', ':', '@', '&', '=' ) |
                         ( set = '$', '-', '_', '.', '+' ) |
                         ( set = '!', '*', '\'', '\(', ')', ',' ) |
                         '%' >> repeat<2>(xdigit)));

        typedef std::vector<std::string>::const_iterator vec_str_it;

        //set forwarded flag true
        forwarded_for = 1;

        try
        {
            vec_str_it it = args.begin();
            vec_str_it it_end = args.end();

            for (; it != it_end;)
            {

                if (*it == "-T" || *it == "--timeout")
                {
                    // timeout
                    if (!timeout_flag)
                    {
                        // not set timeout option
                        ++it;
                        // next parameter exist check
                        if (it != it_end)
                        {
                            // next parameter exist
                            if ((*it).substr(0, 1) == "-" || (*it).substr(0, 2) == "--")
                            {
                                continue;
                            }

                            try
                            {
                                unsigned long ultimeout = 0;
                                ultimeout = boost::lexical_cast<unsigned long>(*it);
                                // int maxvalue check
                                if (ultimeout > INT_MAX)
                                {
                                    check_result.flag = false;
                                    formatter.parse("'-T/--timeout' option value '%s' is too large.");
                                    formatter % *it;
                                    check_result.message = formatter.str();
                                    putLogError(600013, check_result.message, __FILE__, __LINE__);
                                    break;
                                }
                                else
                                {
                                    timeout_flag = true;
                                    timeout = ultimeout;
                                    ++it;
                                    continue;
                                }
                            }
                            catch (boost::bad_lexical_cast& e)
                            {
                                // not numeric character
                                check_result.flag = false;
                                formatter.parse("'-T/--timeout' option value '%s' is not numeric character.");
                                formatter % *it;
                                check_result.message = formatter.str();
                                putLogError(600014, check_result.message, __FILE__, __LINE__);
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
                        check_result.flag = false;
                        check_result.message = "Cannot set multiple option '-T/--timeout'.";
                        putLogError(600015, check_result.message, __FILE__, __LINE__);
                        break;

                    }
                }
                else if (*it == "-R" || *it == "--reschedule")
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
                        check_result.flag = false;
                        check_result.message = "You have to choose either of reschedule or no-reschedule.";
                        putLogError(600016, check_result.message, __FILE__, __LINE__);
                        break;
                    }
                }
                else if  (*it == "-N" || *it == "--no-reschedule")
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
                        check_result.flag = false;
                        check_result.message = "You have to choose either of reschedule or no-reschedule.";
                        putLogError(600017, check_result.message, __FILE__, __LINE__);
                        break;
                    }
                }
                //option string = "-F"
                else if (*it == "-F" || *it == "--forwarded-for")
                {
                    //set forwarded flag ON
                    forward_checked = true;
                    forwarded_for = FORWARDED_FOR_ON;
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
                                //set check result flag false
                                check_result.flag = false;
                                //set check result message
                                check_result.message = "You have to set option value '-S/--sorry-uri'.";
                                //loop break
                                break;
                            }
                            //next option string's length > 127
                            if (it->size() > MAX_OPTION_SIZE - 1)
                            {
                                std::ostringstream ostr;
                                ostr << "'-S/--sorry-uri' option value '" << *it << "' is too long.";

                                //set check result flag false
                                check_result.flag = false;
                                //set check result message
                                check_result.message = ostr.str();
                                putLogError(600018, check_result.message, __FILE__,
                                            __LINE__ );
                                //loop break
                                break;
                            }
                            //next option string's length <= 127
                            else
                            {
                                //regex check
                                //check OK
                                if (regex_match(*it, sorry_uri_regex))
                                {
                                    sorryuri_checked = true;
                                    memcpy(sorry_uri.data(), it->c_str(), it->size());
                                }
                                //check NG
                                else
                                {
                                    std::ostringstream ostr;
                                    ostr << "'-S/--sorry-uri' option value '" << *it << "' is not a valid URI.";

                                    //set check result flag false
                                    check_result.flag = false;
                                    //set check result message
                                    check_result.message = ostr.str();
                                    putLogError(600019, check_result.message, __FILE__,
                                                __LINE__ );
                                    break;
                                }
                            }
                        }
                        //next item not exist
                        else
                        {
                            //set check result flag false
                            check_result.flag = false;
                            //set check result message
                            check_result.message = "You have to set option value '-S/--sorry-uri'.";
                            putLogError(600020, check_result.message, __FILE__,
                                        __LINE__ );
                            break;
                        }
                    }
                    //sorryURI flag = ON
                    else
                    {
                        //set check result flag false
                        check_result.flag = false;
                        //set check result message
                        check_result.message = "Cannot set multiple option '-S/--sorry-uri'.";
                        putLogError(600021, check_result.message, __FILE__,
                                    __LINE__ );
                        break;
                    }
                }
                //others
                else
                {
                    //set check result flag false
                    check_result.flag = false;
                    //set check result message
                    check_result.message = "Option error.";
                    putLogError(600022, check_result.message, __FILE__, __LINE__);

                    break;
                }
                ++it;
            }

            // result check
            if (check_result.flag)
            {
                // set timeout's default value
                if (!timeout_flag)
                {
                    timeout = 3600;
                }

                // set reschedule's default value
                if (!reschedule_flag)
                {
                    reschedule = 0;
                }
                //forward flag = OFF
                if (!forward_checked)
                {
                    forwarded_for = 0;
                }

            }
            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("function : protocol_module_ip::check_message_result "
                                        "protocol_module_ip::set_parameter(const std::vector<std::string>& args) : "
                                        "timeout = %d, reschedule = %d.");
                formatter % timeout  % reschedule;
                putLogDebug(600018, formatter.str(), __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/

            unsigned int data_size = 0;
            void* data_addr = NULL;
            data_addr = replication_pay_memory(get_name(), &data_size);

            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                putLogDebug(600019, "function : protocol_module_ip::check_message_result protocol_module_ip::"
                            "set_parameter() : replication_pay_memory() end.", __FILE__, __LINE__);
                boost::format formatter("function : protocol_module_ip::check_message_result protocol_module_ip::"
                                        "set_parameter() : data_addr = &(%d), data_size = %d.");
                formatter % data_addr % data_size;
                putLogDebug(600020, formatter.str(), __FILE__, __LINE__);
            }

            /*------DEBUG LOG END------*/
            if (data_addr == NULL || data_size <= 0)
            {
                // replication area is null
                putLogInfo(600000, "Replication area is null.", __FILE__, __LINE__);

                /*-------- DEBUG LOG --------*/
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    putLogDebug(600021, "function : protocol_module_ip::check_message_result "
                                "protocol_module_ip::set_parameter() : "
                                "Replication area is null.", __FILE__, __LINE__);
                }
                /*------DEBUG LOG END------*/
            }

            // create ip_replication_data_processor
            replication_data_processor = new ip_replication_data_processor(
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
                % sizeof(ip_replication_data_processor);
                putLogDebug(600022, formatter.str(), __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/

            replication_data_processor->register_replication_area_lock(replication_area_lock);

            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                putLogDebug(600023, "function : protocol_module_ip::check_message_result protocol_module_ip::"
                            "set_parameter() : register_replication_area_lock() end.", __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/

            replication_data_processor->register_replication_area_unlock(replication_area_unlock);

            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                putLogDebug(600024, "function : protocol_module_ip::check_message_result protocol_module_ip::"
                            "set_parameter() : register_replication_area_unlock() end.", __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/

            // create ip_session_data_processor
            ip_data_processor = new ip_session_data_processor(
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
                formatter % static_cast<void*>(ip_data_processor)
                % sizeof(ip_session_data_processor);
                putLogDebug(600025, formatter.str(), __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/

            // restore data from replication area
            ip_replication_data* redata = replication_data_processor->get_replication_area();

            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                putLogDebug(600026, "function : protocol_module_ip::check_message_result protocol_module_ip::"
                            "set_parameter() : get_replication_area() end.", __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/

            replication_area_lock();

            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                putLogDebug(600027, "function : protocol_module_ip::check_message_result protocol_module_ip::"
                            "set_parameter() : replication_area_lock() end.", __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/

            ip_data_processor->read_session_data_from_replication_area(redata);

            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                putLogDebug(600028, "function : protocol_module_ip::check_message_result protocol_module_ip::"
                            "set_parameter() : read_session_data_from_replication_area() end.", __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/

            replication_area_unlock();

            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                putLogDebug(600029, "function : protocol_module_ip::check_message_result protocol_module_ip::"
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

            if (ip_data_processor)
            {
                delete ip_data_processor;
                ip_data_processor = NULL;
            }

            std::cerr << "protocol_module_ip::set_parameter() : exception : Could not allocate memory." << std::endl;
            check_result.flag = false;
            check_result.message = "Could not allocate memory.";
            putLogError(600023, check_result.message, __FILE__, __LINE__);
        }
        catch (const std::exception& ex)
        {
            if (replication_data_processor)
            {
                delete replication_data_processor;
                replication_data_processor = NULL;
            }

            if (ip_data_processor)
            {
                delete ip_data_processor;
                ip_data_processor = NULL;
            }

            check_result.flag = false;
            std::cerr << "protocol_module_ip::set_parameter() : exception : error = " << ex.what() << std::endl;
            boost::format formatter("function : protocol_module_ip::check_message_result "
                                    "protocol_module_ip::set_parameter() : exception : error = %s.");
            formatter % ex.what();
            putLogError(600024, formatter.str(), __FILE__, __LINE__);
        }
        catch (...)
        {
            if (replication_data_processor)
            {
                delete replication_data_processor;
                replication_data_processor = NULL;
            }

            if (ip_data_processor)
            {
                delete ip_data_processor;
                ip_data_processor = NULL;
            }

            check_result.flag = false;
            std::cerr << "protocol_module_ip::set_parameter() : Unknown exception." << std::endl;
            putLogError(600025, "function : protocol_module_ip::check_message_result protocol_module_ip::"
                        "set_parameter() : Unknown exception.", __FILE__, __LINE__);
        }
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("out_function : protocol_module_ip::check_message_result "
                                    "protocol_module_ip::set_parameter("
                                    "const std::vector<std::string>& args) : return_value = ("
                                    "check_message_result.flag = %d, check_message_result.message = %s).");
            formatter % check_result.flag % check_result.message;
            putLogDebug(600030, formatter.str(), __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/
        return check_result;
    }

    //! parameter add
    //! @param[in] module paramter string list
    //! @return    result.flag true is parameter is noproblem.
    //! @return result.flag false is paramter is problem.
    protocol_module_base::check_message_result protocol_module_ip::add_parameter(const std::vector<
            std::string>& args)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("in_function : protocol_module_ip::check_message_result protocol_module_ip::"
                                    "add_parameter(const std::vector<std::string>& args) : args = %s.");
            std::string argsdump;
            for (std::vector<std::string>::const_iterator it = args.begin(); it != args.end(); ++it)
            {
                argsdump += *it;
            }
            formatter % argsdump;
            putLogDebug(600031, formatter.str(), __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/
        check_message_result check_result;
        //set check result flag true
        check_result.flag = true;

        //param list is not empty
        if (!args.empty())
        {
            //set check result flag false
            check_result.flag = false;
            //set check result message
            check_result.message = "Cannot add option.";
            putLogError(600026, check_result.message, __FILE__, __LINE__ );
        }

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("out_function : protocol_module_ip::check_message_result "
                                    "protocol_module_ip::add_parameter(const std::vector<std::string>& args) : "
                                    "return_value = (check_message_result.flag = %d, check_message_result.message = %s).");
            formatter % check_result.flag % check_result.message;
            putLogDebug(600032, formatter.str(), __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/
        return check_result;
    }

    //! get option info
    //! @param[out] module paramter string
    void protocol_module_ip::get_option_info(std::string& option)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            putLogDebug(600033, "in_function : void protocol_module_ip::get_option_info("
                        "std::string& option).", __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/

        boost::format option_formatter("--timeout %d%s %s --sorry-uri '%s'");
        option_formatter % timeout % (forwarded_for ? " --forwarded-for" : "") % (reschedule ? "--reschedule" : "--no-reschedule") % sorry_uri.c_array();
        option.assign(option_formatter.str());

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("out_function : void protocol_module_ip::get_option_info("
                                    "std::string& option) : option = %s.");
            formatter % option;
            putLogDebug(600034, formatter.str(), __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/
    }

    //! TCP/IP scheduled function registation.
    //! @param[in] schedule module TCP/IP scheduled function object type
    void protocol_module_ip::register_schedule(tcp_schedule_func_type inschedule)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            putLogDebug(600035, "in_function : void protocol_module_ip::register_schedule("
                        "tcp_schedule_func_type inschedule).", __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/
        schedule_tcp = inschedule;
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            putLogDebug(600036, "out_function : void protocol_module_ip::register_schedule("
                        "tcp_schedule_func_type inschedule).", __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/
    }

    //! UDP scheduled function registation
    //! @param[in] schedule module UDP scheduled funtion object type
    void protocol_module_ip::register_schedule(udp_schedule_func_type inschedule)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            putLogDebug(600037, "in/out_function : void protocol_module_ip::register_schedule("
                        "udp_schedule_func_type inschedule).", __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/
    }

    //! called from session initialzie use in upstream_thread
    //! @param[in]    upstream thread id.
    //! @param[in]    downstream thread id
    //! @return        session use EVENT mode.
    protocol_module_base::EVENT_TAG protocol_module_ip::handle_session_initialize(
        const boost::thread::id up_thread_id, const boost::thread::id down_thread_id,
        const boost::asio::ip::tcp::endpoint& client_endpoint_tcp,
        const boost::asio::ip::udp::endpoint& client_endpoint_udp)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_session_initialize(const boost::thread::id up_thread_id, "
                                    "const boost::thread::id down_thread_id, "
                                    "const boost::asio::ip::tcp::endpoint& client_endpoint_tcp, "
                                    "const boost::asio::ip::udp::endpoint& client_endpoint_udp) : "
                                    "up_thread_id = %d, down_thread_id = %d.");
            formatter % up_thread_id % down_thread_id;
            putLogDebug(600038, formatter.str(), __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/

        EVENT_TAG status = FINALIZE;
        unsigned int ip_hash = 0;

        //session thread initialization
        try
        {
            thread_data_ptr p_up(new session_thread_data_ip);
            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("new : address = &(%d), size = %lu.");
                formatter % static_cast<void*>(p_up.get()) % sizeof(session_thread_data_ip);
                putLogDebug(600039, formatter.str(), __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/

            //calculate ip address's hash
            ip_hash = l7vs_ip_service_calc_hash(client_endpoint_tcp);

            p_up->thread_id                = up_thread_id;
            p_up->thread_division            = THREAD_DIVISION_UP_STREAM;
            p_up->pair_thread_id            = down_thread_id;
            p_up->accept_end_flag            = ACCEPT_END_FLAG_OFF;
            p_up->end_flag                = END_FLAG_OFF;
            p_up->sorry_flag                = SORRY_FLAG_OFF;
            p_up->switch_flag                = SWITCH_FLAG_OFF;
            p_up->last_status                = INITIALIZE;
            p_up->client_endpoint            = client_endpoint_tcp;
            p_up->data_buffer                = new char[MAX_BUFFER_SIZE];
            p_up->data_buffer_size            = MAX_BUFFER_SIZE;
            p_up->data_length                = 0;
            p_up->data_offset                = 0;
            p_up->current_message_rest_size        = 0;
            p_up->data_state                = HTTP_START;
            p_up->ip_hash                = ip_hash;

            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                // data dump
                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                        "handle_session_initialize() : session_thread_data_ip(upthread) : "
                                        "thread_id = %d, thread_division = %d, "
                                        "pair_thread_id = %d, accept_end_flag = %d, end_flag = %d, "
                                        "sorry_flag = %d, switch_flag = %d, last_status = %d, client_endpoint = [%s]:%d, data_buffer = &(%d), "
                                        "data_buffer_size = %d, data_length = %d, data_offset = %d, current_message_rest_size = %d, data_state = %d, "
                                        "ip_hash = %d.");
                formatter % p_up->thread_id
                % p_up->thread_division
                % p_up->pair_thread_id
                % p_up->accept_end_flag
                % p_up->end_flag
                % p_up->sorry_flag
                % p_up->switch_flag
                % p_up->last_status
                % p_up->client_endpoint.address().to_string()
                % p_up->client_endpoint.port()
                % p_up->data_buffer
                % p_up->data_buffer_size
                % p_up->data_length
                % p_up->data_offset
                % p_up->current_message_rest_size
                % p_up->data_state
                % p_up->ip_hash;


                putLogDebug(600040, formatter.str(), __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/

            thread_data_ptr p_down(new session_thread_data_ip);

            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("new : address = &(%d), size = %lu.");
                formatter % static_cast<void*>(p_down.get()) % sizeof(session_thread_data_ip);
                putLogDebug(600041, formatter.str(), __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/

            p_down->thread_id                = down_thread_id;
            p_down->thread_division            = THREAD_DIVISION_DOWN_STREAM;
            p_down->pair_thread_id            = up_thread_id;
            p_down->accept_end_flag            = ACCEPT_END_FLAG_OFF;
            p_down->end_flag                = END_FLAG_OFF;
            p_down->sorry_flag                = SORRY_FLAG_OFF;
            p_down->switch_flag                = SWITCH_FLAG_OFF;
            p_down->last_status                = INITIALIZE;
            p_down->client_endpoint            = client_endpoint_tcp;
            p_down->data_buffer                = new char[MAX_BUFFER_SIZE];
            p_down->data_buffer_size            = MAX_BUFFER_SIZE;
            p_down->data_length                = 0;
            p_down->data_offset                = 0;
            p_down->current_message_rest_size        = 0;
            p_down->data_state                = HTTP_START;
            p_down->ip_hash                = ip_hash;

            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                // data_dump
                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                        "handle_session_initialize() : session_thread_data_ip(downthread) : "
                                        "thread_id = %d, thread_division = %d, "
                                        "pair_thread_id = %d, accept_end_flag = %d, end_flag = %d, "
                                        "sorry_flag = %d, switch_flag = %d, last_status = %d, client_endpoint = [%s]:%d, data_buffer = &(%d), "
                                        "data_buffer_size = %d, data_length = %d, data_offset = %d, current_message_rest_size = %d, data_state = %d, "
                                        "ip_hash = %d.");
                formatter % p_down->thread_id
                % p_down->thread_division
                % p_down->pair_thread_id
                % p_down->accept_end_flag
                % p_down->end_flag
                % p_down->sorry_flag
                % p_down->switch_flag
                % p_down->last_status
                % p_down->client_endpoint.address().to_string()
                % p_down->client_endpoint.port()
                % p_down->data_buffer
                % p_down->data_buffer_size
                % p_down->data_length
                % p_down->data_offset
                % p_down->current_message_rest_size
                % p_down->data_state
                % p_down->ip_hash;
                putLogDebug(600042, formatter.str(), __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/


            boost::mutex::scoped_lock slock(session_thread_data_map_mutex);

            session_thread_data_map[up_thread_id] = p_up;
            session_thread_data_map[down_thread_id] = p_down;

            //set return status
            status = ACCEPT;
            //save last status
            session_thread_data_map[up_thread_id]->last_status = status;
            session_thread_data_map[down_thread_id]->last_status = REALSERVER_RECV;
        }
        catch (const std::bad_alloc&)
        {
            std::cerr << "protocol_module_ip::handle_session_initialize() : exception : Could not allocate memory." << std::endl;
            boost::format formatter("Could not allocate memory. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(600027, formatter.str(), __FILE__, __LINE__);
            status = FINALIZE;
        }
        catch (const std::exception& ex)
        {
            std::cerr << "protocol_module_ip::handle_session_initialize() : exception : error = " << ex.what() << "." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_session_initialize() : exception : error = %s. thread id : %d.");
            formatter % ex.what() % boost::this_thread::get_id();
            putLogError(600028, formatter.str(), __FILE__, __LINE__);
            status = FINALIZE;
        }
        catch (...)
        {
            std::cerr << "protocol_module_ip::handle_session_initialize() : Unknown exception." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_session_initialize() : Unknown exception. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(600029, formatter.str(), __FILE__, __LINE__);
            status = FINALIZE;
        }

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_session_initialize(const boost::thread::id up_thread_id, "
                                    "const boost::thread::id down_thread_id, "
                                    "const boost::asio::ip::tcp::endpoint& client_endpoint_tcp, "
                                    "const boost::asio::ip::udp::endpoint& client_endpoint_udp) : return_value = %d. "
                                    "thread id : %d.");
            formatter % status % boost::this_thread::get_id();
            putLogDebug(600043, formatter.str(), __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/

        return status;
    }
    //! called from session finalize use in upstream thread.
    //! @param[in]    upstream thread id.
    //! @param[in]    downstream thread id
    //! @return        session use EVENT mode.
    protocol_module_base::EVENT_TAG protocol_module_ip::handle_session_finalize(
        const boost::thread::id up_thread_id, const boost::thread::id down_thread_id)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_session_finalize(const boost::thread::id up_thread_id, "
                                    "const boost::thread::id down_thread_id) : "
                                    "up_thread_id = %d, down_thread_id = %d.");
            formatter % up_thread_id % down_thread_id;
            putLogDebug(600044, formatter.str(), __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/
        EVENT_TAG status = STOP;
        thread_data_ptr p_up;
        thread_data_ptr p_down;
        session_thread_data_map_it session_thread_data_it;

        //session thread free
        try
        {
            boost::mutex::scoped_lock slock(session_thread_data_map_mutex);

            session_thread_data_it = session_thread_data_map.find(up_thread_id);
            if (session_thread_data_it != session_thread_data_map.end())
            {
                p_up = session_thread_data_it->second;
                /*-------- DEBUG LOG --------*/
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    boost::format formatter("delete : address = &(%d).");
                    formatter % static_cast<void*>(p_up->data_buffer);
                    putLogDebug(600045, formatter.str(), __FILE__, __LINE__);
                }
                /*------DEBUG LOG END------*/

                delete p_up->data_buffer;
                /*-------- DEBUG LOG --------*/
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    boost::format formatter("delete : address = &(%d).");
                    formatter % static_cast<void*>(p_up.get());
                    putLogDebug(600046, formatter.str(), __FILE__, __LINE__);
                }
                /*------DEBUG LOG END------*/

                session_thread_data_map.erase(up_thread_id);
            }

            session_thread_data_it = session_thread_data_map.find(down_thread_id);
            if (session_thread_data_it != session_thread_data_map.end())
            {

                p_down = session_thread_data_it->second;
                /*-------- DEBUG LOG --------*/
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    boost::format formatter("delete : address = &(%d).");
                    formatter % static_cast<void*>(p_up->data_buffer);
                    putLogDebug(600047, formatter.str(), __FILE__, __LINE__);
                }
                /*------DEBUG LOG END------*/

                delete p_down->data_buffer;
                /*-------- DEBUG LOG --------*/
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    boost::format formatter("delete : address = &(%d).");
                    formatter % static_cast<void*>(p_down.get());
                    putLogDebug(600048, formatter.str(), __FILE__, __LINE__);
                }
                /*------DEBUG LOG END------*/

                session_thread_data_map.erase(down_thread_id);
            }

            //set return status
            status = STOP;
        }
        catch (const std::exception& ex)
        {
            std::cerr << "protocol_module_ip::handle_session_finalize() : exception : error = " << ex.what() << "." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_session_finalize() : exception : error = %s. thread id : %d.");
            formatter % ex.what() % boost::this_thread::get_id();
            putLogError(600030, formatter.str(), __FILE__, __LINE__ );
            status = STOP;
        }
        catch (...)
        {
            std::cerr << "protocol_module_ip::handle_session_finalize() : Unknown exception." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_session_finalize() : "
                                    "Unknown exception. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(600031, formatter.str(), __FILE__, __LINE__ );
            status = STOP;
        }
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_session_finalize(const boost::thread::id up_thread_id, "
                                    "const boost::thread::id down_thread_id) : return_value = %d. thread id : %d.");
            formatter % status % boost::this_thread::get_id();
            putLogDebug(600049, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/
        return status;
    }

    //! called from after session accept.in client socket use in upstream thread.
    //! @param[in]    upstream thread id.
    //! @return        session use EVENT mode.
    protocol_module_base::EVENT_TAG protocol_module_ip::handle_accept(const boost::thread::id thread_id)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_accept(const boost::thread::id thread_id) : thread_id = %d.");
            formatter % thread_id;
            putLogDebug(600050, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        EVENT_TAG status = FINALIZE;
        thread_data_ptr session_data_ptr;
        session_thread_data_map_it session_thread_it;

        try
        {
            {
                boost::mutex::scoped_lock slock(session_thread_data_map_mutex);

                session_thread_it = session_thread_data_map.find(thread_id);
                if (unlikely(session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL))
                {
                    boost::format formatter("Invalid thread id. thread id : %d.");
                    formatter % boost::this_thread::get_id();
                    putLogError(600032, formatter.str(), __FILE__, __LINE__ );
                    throw -1;
                }

                session_data_ptr = session_thread_it->second;
            }

            //set accept end flag ON
            session_data_ptr->accept_end_flag = ACCEPT_END_FLAG_ON;

            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                        "handle_accept(const boost::thread::id thread_id) : ACCEPT_END_FLAG_ON. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogDebug(600051, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/

            //sorry flag on
            if (session_data_ptr->sorry_flag == SORRY_FLAG_ON)
            {
                //set return status
                status = SORRYSERVER_SELECT;
            }
            //sorry flag off
            else
            {
                //set return status
                status = REALSERVER_SELECT;
            }

            //set last status
            session_data_ptr->last_status = status;
        }
        catch (int e)
        {
            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                        "handle_accept() : catch exception e = %d. thread id : %d.");
                formatter % e % boost::this_thread::get_id();
                putLogDebug(600052, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/
            status = FINALIZE;
        }
        catch (const std::exception& ex)
        {
            std::cerr << "protocol_module_ip::handle_accept() : exception : error = " << ex.what() << "." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_accept() : exception : error = %s. thread id : %d.");
            formatter % ex.what() % boost::this_thread::get_id();
            putLogError(600033, formatter.str(), __FILE__, __LINE__ );
            status = FINALIZE;
        }
        catch (...)
        {
            std::cerr << "protocol_module_ip::handle_accept() : Unknown exception." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG "
                                    "protocol_module_ip::handle_accept() : "
                                    "Unknown exception. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(600034, formatter.str(), __FILE__, __LINE__ );
            status = FINALIZE;
        }

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_accept(const boost::thread::id thread_id) : return_value = %d. thread id : %d.");
            formatter % status % boost::this_thread::get_id();
            putLogDebug(600053, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        return status;
    }

    //! called from after session recv in client socket. use in upstream thread.
    //! @param[in]    upstream thread id
    //! @param[in]    recive buffer refarence.
    //! @param[in]    recive length
    //! @return        session use EVENT mode.
    protocol_module_base::EVENT_TAG protocol_module_ip::handle_client_recv(const boost::thread::id thread_id,
            const boost::array<char, MAX_BUFFER_SIZE>& recvbuffer, const size_t recvlen)
    {

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            size_t buffer_size = recvbuffer.size() < recvlen ? recvbuffer.size() : recvlen;
            std::string buffer;
            dump_memory(recvbuffer.data(), buffer_size, buffer);
            boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_client_recv(const boost::thread::id thread_id, "
                                    "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                    "const size_t recvlen) : thread_id = %d, recvbuffer = %s, recvlen = %d.");
            formatter % thread_id % buffer % recvlen;
            putLogDebug(600054, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        EVENT_TAG status                = FINALIZE;
        bool find_ret                    = false;
        size_t http_header_offset            = 0;
        size_t http_header_len                = 0;
        size_t http_header_content_length_offset    = 0;
        size_t http_header_content_length_len        = 0;
        int content_length_value            = 0;
        const size_t CR_LF_LEN                = 2; //length of "\r\n"
        const size_t CR_LF_CR_LF_LEN            = 4; //length of "\r\n\r\n"
        session_thread_data_map_it            session_thread_it;
        thread_data_ptr                    session_data_ptr;
        http_utility::CHECK_RESULT_TAG             check_ret;

        std::string content_length;
        cmatch regex_ret;
        cregex content_length_regex = icase("Content-Length") >> ":" >> *~_d >> (s1 = +_d) >> *~_d;

        //parameter check
        if (unlikely(recvlen > recvbuffer.size()))
        {
            std::cerr << "protocol_module_ip::handle_client_recv() : Data size bigger than buffer size." << std::endl;
            boost::format formatter("Data size bigger than buffer size. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(600035, formatter.str(), __FILE__, __LINE__ );

            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                        "handle_client_recv(const boost::thread::id thread_id, "
                                        "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                        "const size_t recvlen) : return_value = %d. thread id : %d.");
                formatter % FINALIZE % boost::this_thread::get_id();
                putLogDebug(600055, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/
            return status;
        }

        try
        {
            {
                boost::mutex::scoped_lock slock(session_thread_data_map_mutex);

                session_thread_it = session_thread_data_map.find(thread_id);
                if (unlikely(session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL))
                {

                    boost::format formatter("Invalid thread id. thread id : %d.");
                    formatter % boost::this_thread::get_id();
                    putLogError(600036, formatter.str(), __FILE__, __LINE__ );
                    throw -1;
                }

                session_data_ptr = session_thread_it->second;
            }

            //end flag on
            if (session_data_ptr->end_flag == END_FLAG_ON)
            {
                //set return status
                status = CLIENT_RECV;
            }
            //end flag off
            else
            {
                //copy data from recvbuffer
                if (!get_data_from_recvbuffer(session_data_ptr, recvbuffer, recvlen))
                {
                    //copy failed
                    std::cerr << "protocol_module_ip::handle_client_recv() : Data size bigger than buffer size." << std::endl;
                    boost::format formatter("Data size bigger than buffer size. thread id : % id.");
                    formatter % boost::this_thread::get_id();
                    putLogError(600037, formatter.str(), __FILE__, __LINE__);
                    status = FINALIZE;
                }
                else
                {
                    //data state is HTTP_START
                    if (session_data_ptr->data_state == HTTP_START)
                    {
                        //search http header
                        find_ret = http_utility::find_http_header_all(session_data_ptr->data_buffer + session_data_ptr->data_offset,
                                   session_data_ptr->data_length,
                                   http_header_offset,
                                   http_header_len
                                                                     );

                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                                    "handle_client_recv() : call find_http_header_all : "
                                                    "return_value = %d. thread id : %d.");
                            formatter % static_cast<int>(find_ret) % boost::this_thread::get_id();
                            putLogDebug(600056, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/

                        //search http header result is NG
                        if (!find_ret)
                        {
                            //data size bigger than max buffer size
                            if (session_data_ptr->data_length >= MAX_IP_MODULE_BUFFER_SIZE - recvbuffer.size())
                            {
                                //set data state UNKNOWN
                                session_data_ptr->data_state = UNKNOWN;
                                //set current message rest size
                                session_data_ptr->current_message_rest_size = session_data_ptr->data_length;
                            }
                        }
                        //search http header result is OK
                        else
                        {
                            //check http method and version
                            check_ret = http_utility::check_http_method_and_version(session_data_ptr->data_buffer,
                                        session_data_ptr->data_length);
                            /*-------- DEBUG LOG --------*/
                            if (unlikely(LOG_LV_DEBUG == getloglevel()))
                            {
                                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                                        "handle_client_recv() : call check_http_method_and_version : "
                                                        "return_value = %d. thread id : %d.");
                                formatter % check_ret % boost::this_thread::get_id();
                                putLogDebug(600057, formatter.str(), __FILE__, __LINE__ );
                            }
                            /*------DEBUG LOG END------*/

                            //check http method and version result is NG
                            if (check_ret == http_utility::CHECK_NG)
                            {
                                //set data state UNKNOWN
                                session_data_ptr->data_state = UNKNOWN;
                                //set current message rest size
                                session_data_ptr->current_message_rest_size = session_data_ptr->data_length;
                            }
                            //check http method and version result is OK
                            else
                            {
                                //search Content_Length header
                                find_ret = http_utility::find_http_header_content_length(session_data_ptr->data_buffer+session_data_ptr->data_offset,
                                           session_data_ptr->data_length,
                                           http_header_content_length_offset,
                                           http_header_content_length_len);
                                /*-------- DEBUG LOG --------*/
                                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                                {
                                    boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                                            "handle_client_recv() : call find_http_header_content_length : "
                                                            "return_value = %d. thread id : %d.");
                                    formatter % static_cast<int>(find_ret) % boost::this_thread::get_id();
                                    putLogDebug(600058, formatter.str(), __FILE__, __LINE__ );
                                }
                                /*------DEBUG LOG END------*/

                                //search Content_Length result is OK
                                if (find_ret)
                                {
                                    //set content length string
                                    content_length.assign(session_data_ptr->data_buffer+session_data_ptr->data_offset + http_header_content_length_offset,
                                                          http_header_content_length_len );
                                    find_ret = regex_search(content_length.c_str(), regex_ret, content_length_regex);

                                    //"content-length: ddd\r\n"
                                    if ( find_ret)
                                    {
                                        content_length = content_length.substr(
                                                             regex_ret.position(1),
                                                             regex_ret.length(1));

                                        //set content length value
                                        content_length_value = boost::lexical_cast<int>(content_length);
                                    }

                                    //http_header context is "\r\n\r\n" only
                                    if (http_header_len == 0)
                                    {
                                        //set current message rest size
                                        session_data_ptr->current_message_rest_size = http_header_offset + http_header_len + content_length_value + CR_LF_LEN;
                                    }
                                    else
                                    {
                                        //set current message rest size
                                        session_data_ptr->current_message_rest_size = http_header_offset + http_header_len + content_length_value + CR_LF_CR_LF_LEN;
                                    }
                                }
                                //search Content_Length result is NG
                                else
                                {
                                    //http_header context is "\r\n\r\n" only
                                    if (http_header_len == 0)
                                    {
                                        //set current message rest size
                                        session_data_ptr->current_message_rest_size = http_header_offset + http_header_len + CR_LF_LEN;
                                    }
                                    else
                                    {
                                        //set current message rest size
                                        session_data_ptr->current_message_rest_size = http_header_offset + http_header_len + CR_LF_CR_LF_LEN;
                                    }

                                }

                                //set data state HTTP_HEADER
                                session_data_ptr->data_state = HTTP_HEADER;

                            }
                        }
                    }
                    //data state is UNKNOWN
                    else if (session_data_ptr->data_state == UNKNOWN)
                    {
                        //set current message rest size
                        session_data_ptr->current_message_rest_size = session_data_ptr->data_length;
                    }
                    else
                    {
                        //none
                    }

                    //data state is HTTP_START
                    if (session_data_ptr->data_state == HTTP_START )
                    {
                        status = CLIENT_RECV;
                    }
                    //data state is not HTTP_START
                    else
                    {
                        //sorry flag is on
                        if (session_data_ptr->sorry_flag == SORRY_FLAG_ON)
                        {
                            //set return status
                            status = SORRYSERVER_CONNECT;
                        }
                        //sorry flag is off
                        else
                        {
                            //set return status
                            status = REALSERVER_CONNECT;
                        }
                    }
                }
            }

            //set last status
            session_data_ptr->last_status = status;
        }
        catch (int e)
        {
            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                        "handle_client_recv() : catch exception e = %d. thread id : %d.");
                formatter % e % boost::this_thread::get_id();
                putLogDebug(600059, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/
            status = FINALIZE;
        }
        catch (const boost::bad_lexical_cast& )
        {
            std::cerr << "protocol_module_ip::handle_client_recv() : exception : " <<  "Content_Length field's value is invalid." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::handle_client_recv() : "
                                    "Content_Length field's value is invalid. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(600038, formatter.str(), __FILE__, __LINE__ );
            status = FINALIZE;
        }
        catch (const std::exception& ex)
        {
            std::cerr << "protocol_module_ip::handle_client_recv() : exception : error = " << ex.what() << "." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_client_recv() : exception : error = %s. thread id : %d.");
            formatter % ex.what() % boost::this_thread::get_id();
            putLogError(600039, formatter.str(), __FILE__, __LINE__ );
            status = FINALIZE;
        }
        catch (...)
        {
            std::cerr << "protocol_module_ip::handle_client_recv() : Unknown exception." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG "
                                    "protocol_module_ip::handle_client_recv() : "
                                    "Unknown exception. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(600040, formatter.str(), __FILE__, __LINE__ );
            status = FINALIZE;
        }

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_client_recv(const boost::thread::id thread_id, "
                                    "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                    "const size_t recvlen) : return_value = %d. thread id : %d.");
            formatter % status % boost::this_thread::get_id();
            putLogDebug(600060, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        return status;
    }

    //! called from after realserver select.use in upstream thread.
    //! @param[in]    upstream thread id
    //! @param[out]    realserver TCP endpoint
    //! @return        session use EVENT mode.
    protocol_module_base::EVENT_TAG protocol_module_ip::handle_realserver_select(
        const boost::thread::id thread_id, boost::asio::ip::tcp::endpoint & rs_endpoint)
    {

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_realserver_select(const boost::thread::id thread_id, "
                                    "boost::asio::ip::tcp::endpoint& rs_endpoint) : "
                                    "thread_id = %d, rs_endpoint = [%s]:%d.");
            formatter % thread_id % rs_endpoint.address().to_string() % rs_endpoint.port();
            putLogDebug(600061, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        EVENT_TAG status = FINALIZE;
        boost::asio::ip::tcp::endpoint init_endpoint;
        thread_data_ptr session_data_ptr;
        session_thread_data_map_it session_thread_it;
        session_thread_data_map_it session_thread_it_end;
        realserverlist_type::iterator    rs_list_itr;

        if (unlikely(schedule_tcp.empty()))
        {
            std::cerr << "protocol_module_ip::handle_realserver_select() : Schedule_tcp function is empty." << std::endl;
            boost::format formatter("Schedule_tcp function is empty. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(600041, formatter.str(), __FILE__, __LINE__ );

            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                        "handle_realserver_select(const boost::thread::id thread_id, "
                                        "boost::asio::ip::tcp::endpoint& rs_endpoint)"
                                        " : return_value = %d. thread id : %d.");
                formatter % FINALIZE % boost::this_thread::get_id();
                putLogDebug(600062, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/
            return FINALIZE;
        }


        try
        {
            {
                boost::mutex::scoped_lock slock(session_thread_data_map_mutex);

                session_thread_it = session_thread_data_map.find(thread_id);
                if (unlikely(session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL))
                {
                    boost::format formatter("Invalid thread id. thread id : %d.");
                    formatter % boost::this_thread::get_id();
                    putLogError(600042, formatter.str(), __FILE__, __LINE__ );
                    throw -1;
                }

                session_data_ptr = session_thread_it->second;
            }

            //initialize realserver endpoint
            rs_endpoint = init_endpoint;

            boost::mutex::scoped_lock lock(session_data_mutex);

            //get endpoint data from the ip data
            int ret = ip_data_processor->get_endpoint_from_session_data(session_data_ptr->ip_hash,
                      rs_endpoint);

            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("function : protocol_module_base::EVENT_TAG "
                                        "protocol_module_ip::handle_realserver_select() : "
                                        "get_endpoint_from_session_data() end. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogDebug(600063, formatter.str(), __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/

            //endpoint is matched
            if (ret == 0)
            {
                int is_match = 0;
                //find the rs_endpoint in rs_list
                {
                    rs_list_scoped_lock scoped_lock(rs_list_lock, rs_list_unlock);

                    rs_list_itr = rs_list_begin();
                
                    while ( rs_list_itr != rs_list_end())
                    {

                        if ( rs_list_itr->tcp_endpoint == rs_endpoint )
                        {
                            is_match = 1;
                            break;
                        }

                        rs_list_itr = rs_list_next( rs_list_itr );
                    }
                }

                //endpoint is matched in the list
                if (is_match)
                {
                    //data state is HTTP_START
                    if (session_data_ptr->data_state == HTTP_START)
                    {
                        //set return status
                        status = CLIENT_RECV;
                    }
                    //data state is not HTTP_START
                    else
                    {
                        //set return status
                        status = REALSERVER_CONNECT;
                    }
                }
                //endpoint is not matched in the list
                else
                {
                    //if reschedule is on then try multi times connect
                    if (reschedule == 1)
                    {
                        // init rs_endpoint
                        rs_endpoint = init_endpoint;
                        {
                            rs_list_scoped_lock scoped_lock(rs_list_lock, rs_list_unlock);
                            schedule_tcp(thread_id, rs_list_begin, rs_list_end, rs_list_next, rs_endpoint);
                        }

                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                                    "handle_realserver_select() : call schedule_tcp : "
                                                    "rs_endpoint = [%s]:%d. thread id : %d.");
                            formatter % rs_endpoint.address().to_string() % rs_endpoint.port() % boost::this_thread::get_id();
                            putLogDebug(600064, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/

                        //get the endpoint by schedule successfully
                        if (init_endpoint != rs_endpoint)
                        {
                            time_t init_time = 0;

                            //write data to session table
                            ip_data_processor->write_session_data(session_data_ptr->ip_hash, rs_endpoint, init_time);
                            /*-------- DEBUG LOG --------*/
                            if (unlikely(LOG_LV_DEBUG == getloglevel()))
                            {
                                boost::format formatter("function : protocol_module_base::EVENT_TAG "
                                                        "protocol_module_ip::handle_realserver_select() : "
                                                        "write_session_data() end. thread id : %d.");
                                formatter % boost::this_thread::get_id();
                                putLogDebug(600065, formatter.str(), __FILE__, __LINE__);
                            }
                            /*------DEBUG LOG END------*/

                            //data state is HTTP_START
                            if (session_data_ptr->data_state == HTTP_START)
                            {
                                //set return status
                                status = CLIENT_RECV;
                            }
                            //data state is not HTTP_START
                            else
                            {
                                //set return status
                                status = REALSERVER_CONNECT;
                            }
                        }
                        //get the endpoint by schedule unsuccessfully
                        else
                        {
                            session_data_ptr->sorry_flag = SORRY_FLAG_ON;
                            /*-------- DEBUG LOG --------*/
                            if (unlikely(LOG_LV_DEBUG == getloglevel()))
                            {
                                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                                        "handle_realserver_select()"
                                                        " : SORRY_FLAG_ON. thread id : %d.");
                                formatter % boost::this_thread::get_id();
                                putLogDebug(600066, formatter.str(), __FILE__, __LINE__ );
                            }
                            /*------DEBUG LOG END------*/

                            //set return status
                            status = SORRYSERVER_SELECT;
                        }
                    }
                    //reschedule is off
                    else
                    {
                        //set return status
                        status = CLIENT_DISCONNECT;
                    }
                }
            }
            //endpoint is not matched
            else
            {
                // init rs_endpoint
                rs_endpoint = init_endpoint;

                //call schedule_module's schedule function, get realserver endpoint
                {
                    rs_list_scoped_lock scoped_lock(rs_list_lock, rs_list_unlock);
                    schedule_tcp(thread_id, rs_list_begin, rs_list_end, rs_list_next, rs_endpoint);
                }

                /*-------- DEBUG LOG --------*/
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                            "handle_realserver_select() : call schedule_tcp : "
                                            "rs_endpoint = [%s]:%d. thread id : %d.");
                    formatter % rs_endpoint.address().to_string() % rs_endpoint.port() % boost::this_thread::get_id();
                    putLogDebug(600067, formatter.str(), __FILE__, __LINE__ );
                }
                /*------DEBUG LOG END------*/

                //get the endpoint by schedule successfully
                if (init_endpoint != rs_endpoint)
                {
                    time_t init_time = 0;

                    //write data to session table
                    ip_data_processor->write_session_data(session_data_ptr->ip_hash, rs_endpoint, init_time);
                    /*-------- DEBUG LOG --------*/
                    if (unlikely(LOG_LV_DEBUG == getloglevel()))
                    {
                        boost::format formatter("function : protocol_module_base::EVENT_TAG "
                                                "protocol_module_ip::handle_realserver_select() : "
                                                "write_session_data() end. thread id : %d.");
                        formatter % boost::this_thread::get_id();
                        putLogDebug(600068, formatter.str(), __FILE__, __LINE__);
                    }
                    /*------DEBUG LOG END------*/

                    //data state is HTTP_START
                    if (session_data_ptr->data_state == HTTP_START)
                    {
                        //set return status
                        status = CLIENT_RECV;
                    }
                    //data state is not HTTP_START
                    else
                    {
                        //set return status
                        status = REALSERVER_CONNECT;
                    }
                }
                //get the endpoint by schedule unsuccessfully
                else
                {
                    session_data_ptr->sorry_flag = SORRY_FLAG_ON;
                    /*-------- DEBUG LOG --------*/
                    if (unlikely(LOG_LV_DEBUG == getloglevel()))
                    {
                        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                                "handle_realserver_select()"
                                                " : SORRY_FLAG_ON. thread id : %d.");
                        formatter % boost::this_thread::get_id();
                        putLogDebug(600069, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                    //set return status
                    status = SORRYSERVER_SELECT;

                }
            }

            //set last status
            session_data_ptr->last_status = status;
        }
        catch (int e)
        {
            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                        "handle_realserver_select() : catch exception e = %d. thread id : %d.");
                formatter % e % boost::this_thread::get_id();
                putLogDebug(600070, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/
            status = FINALIZE;
        }
        catch (const std::exception& ex)
        {
            std::cerr << "protocol_module_ip::handle_realserver_select() : exception : error = " << ex.what() << "." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_realserver_select() : exception : error = %s. thread id : %d.");
            formatter % ex.what() % boost::this_thread::get_id();
            putLogError(600043, formatter.str(), __FILE__, __LINE__ );
            status = FINALIZE;
        }
        catch (...)
        {
            std::cerr << "protocol_module_ip::handle_realserver_select() : Unknown exception." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG "
                                    "protocol_module_ip::handle_realserver_select() : "
                                    "Unknown exception. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(600044, formatter.str(), __FILE__, __LINE__ );
            status = FINALIZE;
        }

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_realserver_select(const boost::thread::id thread_id, "
                                    "boost::asio::ip::tcp::endpoint& rs_endpoint)"
                                    " : return_value = %d. thread id : %d.");
            formatter % status % boost::this_thread::get_id();
            putLogDebug(600071, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/
        return status;
    }

    //! called from after realserver select
    //! @param[in]    upstream thread id
    //! @param[out]    realserver UDP endpoint
    //! @param[out]    sendbuffer reference
    //! @param[out]    send data length
    //! @return        session use EVENT mode.
    protocol_module_base::EVENT_TAG protocol_module_ip::handle_realserver_select(
        const boost::thread::id thread_id, boost::asio::ip::udp::endpoint& rs_endpoint, boost::array<char,
        MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("in/out_function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_realserver_select(const boost::thread::id thread_id, "
                                    "boost::asio::ip::udp::endpoint& rs_endpoint, boost::array<char,MAX_BUFFER_SIZE>& sendbuffer, "
                                    "size_t& datalen) : "
                                    "return_value = %d. thread id : %d.");
            formatter % STOP % boost::this_thread::get_id();
            putLogDebug(600072, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/
        return STOP;
    }
    //! called from after realserver connect
    //! @param[in]    upstream thread id
    //! @param[out]    sendbuffer reference
    //! @param[out]    send data length
    //! @return        session use EVENT mode.
    protocol_module_base::EVENT_TAG protocol_module_ip::handle_realserver_connect(
        const boost::thread::id thread_id, boost::array<char, MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_realserver_connect(const boost::thread::id thread_id, "
                                    "boost::array<char, MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen) : "
                                    "thread_id = %d.");
            formatter % thread_id;
            putLogDebug(600073, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        EVENT_TAG status            = FINALIZE;
        size_t send_possible_size        = 0;
        size_t x_forwarded_for_insert_pos   = 0;
        thread_data_ptr                session_data_ptr;
        session_thread_data_map_it        session_thread_it;
        std::pair<char*, size_t>        buffer_element;
        std::string                x_forwarded_for_context;

        try
        {
            {
                boost::mutex::scoped_lock slock(session_thread_data_map_mutex);

                //thread id check
                session_thread_it = session_thread_data_map.find(thread_id);
                if (unlikely(session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL))
                {
                    boost::format formatter("Invalid thread id. thread id : %d.");
                    formatter % boost::this_thread::get_id();
                    putLogError(600045, formatter.str(), __FILE__, __LINE__ );
                    throw -1;
                }

                session_data_ptr = session_thread_it->second;
            }

            send_possible_size = std::min(session_data_ptr->current_message_rest_size,
                                          session_data_ptr->data_length
                                         );

            //buffer sequence is empty
            if (session_data_ptr->buffer_sequence.empty())
            {
                //data state is HTTP_HEADER
                if (session_data_ptr->data_state == HTTP_HEADER)
                {
                    //forwarded_for flag is on
                    if (forwarded_for == FORWARDED_FOR_ON)
                    {
                        //search X-Forwarded-For header
                        create_x_forwarded_for(session_data_ptr->client_endpoint.address().to_string(),
                                               session_data_ptr->data_buffer + session_data_ptr->data_offset,
                                               session_data_ptr->data_length,
                                               x_forwarded_for_insert_pos,
                                               x_forwarded_for_context);

                        //put buffer data to buffer_sequence
                        buffer_element.first = session_data_ptr->data_buffer + session_data_ptr->data_offset;
                        buffer_element.second = x_forwarded_for_insert_pos;
                        session_data_ptr->buffer_sequence.push_back(buffer_element);

                        session_data_ptr->forwarded_for_buffer.assign(0);

                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            std::string datadump;
                            dump_memory(x_forwarded_for_context.c_str(),
                                        x_forwarded_for_context.size(), datadump);
                            boost::format formatter(
                                "function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                "handle_realserver_connect() : before memcpy (data dump) : "
                                "data begin = 0, data_size = %d, data = %s");
                            formatter %  x_forwarded_for_context.size() % datadump;
                            putLogDebug(600074, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/

                        memcpy(session_data_ptr->forwarded_for_buffer.data(),
                               x_forwarded_for_context.c_str(),
                               x_forwarded_for_context.size());

                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            std::string datadump;
                            dump_memory(x_forwarded_for_context.c_str(),
                                        x_forwarded_for_context.size(), datadump);
                            boost::format formatter(
                                "function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                "handle_realserver_connect() : after memcpy (data dump) : "
                                "data begin = 0, data_size = %d, data = %s");
                            formatter % x_forwarded_for_context.size() % datadump;
                            putLogDebug(600075, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/

                        //set buffer's position
                        buffer_element.first = session_data_ptr->forwarded_for_buffer.data();
                        buffer_element.second = x_forwarded_for_context.size();

                        //set data to buffer_sequence
                        session_data_ptr->buffer_sequence.push_back(buffer_element);

                        //set buffer's position
                        buffer_element.first = session_data_ptr->data_buffer + session_data_ptr->data_offset + x_forwarded_for_insert_pos;
                        buffer_element.second = send_possible_size - x_forwarded_for_insert_pos;

                        //set data to buffer_sequence
                        session_data_ptr->buffer_sequence.push_back(buffer_element);
                    }
                    //forwarded_for flag is off
                    else
                    {
                        //set buffer's position
                        buffer_element.first = session_data_ptr->data_buffer + session_data_ptr->data_offset;
                        buffer_element.second = send_possible_size;

                        //set data to buffer_sequence
                        session_data_ptr->buffer_sequence.push_back(buffer_element);
                    }
                    session_data_ptr->data_state = HTTP_BODY;
                }
                //data state is not HTTP_HEADER
                else
                {

                    buffer_element.first = session_data_ptr->data_buffer + session_data_ptr->data_offset;
                    buffer_element.second = send_possible_size;

                    //set data to buffer_sequence
                    session_data_ptr->buffer_sequence.push_back(buffer_element);

                }
            }

            //put buffer_sequence data into sendbuffer
            put_data_into_sendbuffer(session_data_ptr, sendbuffer, datalen);

            //set return status
            status = REALSERVER_SEND;

            //set last status
            session_data_ptr->last_status = status;
        }
        catch (int e)
        {
            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                        "handle_realserver_connect() : catch exception e = %d. thread id : %d.");
                formatter % e % boost::this_thread::get_id();
                putLogDebug(600076, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/

            //set return status
            status = FINALIZE;
        }
        catch (std::exception& ex)
        {
            std::cerr << "protocol_module_ip::handle_realserver_connect() : exception : error = " << ex.what() << "." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_realserver_connect() : exception : error = %s. thread id : %d.");
            formatter % ex.what() % boost::this_thread::get_id();
            putLogError(600046, formatter.str(), __FILE__, __LINE__ );

            //set return status
            status = FINALIZE;
        }
        catch (...)
        {
            std::cerr << "protocol_module_ip::handle_realserver_connect() : Unknown exception." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_realserver_connect() : Unknown exception. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(600047, formatter.str(), __FILE__, __LINE__ );

            //set return status
            status = FINALIZE;
        }

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_realserver_connect(const boost::thread::id thread_id, "
                                    "boost::array<char,MAX_BUFFER_SIZE>& sendbuffer, "
                                    "size_t& datalen) : return_value = %d. thread id : %d.");
            formatter % status % boost::this_thread::get_id();
            putLogDebug(600077, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        return status;
    }

    //! called from after realserver connection fail
    //! @param[in]    upstream thread id
    //! @param[in]    fail realserver endpoint reference
    //! @return        session use EVENT mode.
    protocol_module_base::EVENT_TAG protocol_module_ip::handle_realserver_connection_fail(
        const boost::thread::id thread_id, const boost::asio::ip::tcp::endpoint & rs_endpoint)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_realserver_connection_fail(const boost::thread::id thread_id, "
                                    "const boost::asio::ip::tcp::endpoint& rs_endpoint) : "
                                    "thread_id = %d, rs_endpoint = [%s]:%d.");
            formatter % thread_id % rs_endpoint.address().to_string() % rs_endpoint.port();
            putLogDebug(600078, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        EVENT_TAG status = FINALIZE;
        thread_data_ptr session_data_ptr;
        session_thread_data_map_it session_thread_it;

        try
        {
            boost::mutex::scoped_lock slock(session_thread_data_map_mutex);

            session_thread_it = session_thread_data_map.find(thread_id);
            if (unlikely(session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL))
            {
                boost::format formatter("Invalid thread id. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogError(600048, formatter.str(), __FILE__, __LINE__ );
                throw -1;
            }

            session_data_ptr = session_thread_it->second;

            //set return status
            status = CLIENT_DISCONNECT;

            //set last status
            session_data_ptr->last_status = status;
        }
        catch (int e)
        {
            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                        "handle_realserver_connection_fail() : catch exception e = %d. thread id : %d.");
                formatter % e % boost::this_thread::get_id();
                putLogDebug(600079, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/

            //set return status
            status = FINALIZE;
        }
        catch (const std::exception& ex)
        {
            std::cerr << "protocol_module_ip::handle_realserver_connection_fail() : exception : error = " << ex.what() << "." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_realserver_connection_fail() : exception : error = %s. thread id : %d.");
            formatter % ex.what() % boost::this_thread::get_id();
            putLogError(600049, formatter.str(), __FILE__, __LINE__ );

            //set return status
            status = FINALIZE;
        }
        catch (...)
        {
            std::cerr << "protocol_module_ip::handle_realserver_connection_fail() : Unknown exception." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG "
                                    "protocol_module_ip::handle_realserver_connection_fail() : "
                                    "Unknown exception. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(600050, formatter.str(), __FILE__, __LINE__ );

            //set return status
            status = FINALIZE;
        }

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_realserver_connection_fail(const boost::thread::id thread_id, "
                                    "const boost::asio::ip::tcp::endpoint& rs_endpoint) : return_value = %d. thread id : %d.");
            formatter % status % boost::this_thread::get_id();
            putLogDebug(600080, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        return status;
    }
    //! called from after realserver send.
    //! @param[in]    upstream thread id
    //! @return        session use EVENT mode.
    protocol_module_base::EVENT_TAG protocol_module_ip::handle_realserver_send(
        const boost::thread::id thread_id)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_realserver_send(const boost::thread::id thread_id) : thread_id = %d.");
            formatter % thread_id;
            putLogDebug(600081, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        EVENT_TAG status                = FINALIZE;
        size_t http_header_all_offset            = 0;
        size_t http_header_all_len            = 0;
        size_t http_header_content_length_offset    = 0;
        size_t http_header_content_length_len        = 0;
        const size_t CR_LF_LEN                = 2;
        const size_t CR_LF_CR_LF_LEN            = 4;
        int content_length_value            = 0;

        std::string content_length;
        cmatch regex_ret;
        cregex content_length_regex = icase("Content-Length") >> ":" >> *~_d >> (s1 = +_d) >> *~_d;


        bool find_ret = false;
        http_utility::CHECK_RESULT_TAG check_ret;

        thread_data_ptr session_data_ptr;
        session_thread_data_map_it session_thread_it;

        try
        {
            {
                boost::mutex::scoped_lock slock(session_thread_data_map_mutex);

                //thread id check
                session_thread_it = session_thread_data_map.find(thread_id);
                if (unlikely(session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL))
                {
                    boost::format formatter("Invalid thread id. thread id : %d.");
                    formatter % boost::this_thread::get_id();
                    putLogError(600051, formatter.str(), __FILE__, __LINE__ );
                    throw -1;
                }

                session_data_ptr = session_thread_it->second;
            }

            //current_message_rest_size　>　0
            if (session_data_ptr->current_message_rest_size > 0)
            {
                //data size　>　0
                if (session_data_ptr->data_length > 0)
                {
                    //set return status
                    status = REALSERVER_CONNECT;
                }
                //data size is 0
                else
                {
                    //data offset is 0
                    session_data_ptr->data_offset = 0;

                    //set return status
                    status = CLIENT_RECV;
                }
            }
            //current_message_rest_size is 0
            else
            {
                //data size　>　0
                if (session_data_ptr->data_length > 0)
                {
                    //data state is HTTP_BODY
                    if (session_data_ptr->data_state == HTTP_BODY)
                    {
                        //search whole http header, get whole http header's offset and length
                        find_ret = http_utility::find_http_header_all(session_data_ptr->data_buffer + session_data_ptr->data_offset,
                                   session_data_ptr->data_length,
                                   http_header_all_offset,
                                   http_header_all_len
                                                                     );

                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                                    "handle_realserver_send() : call find_http_header_all : "
                                                    "return_value = %d. thread id : %d.");
                            formatter % static_cast<int>(find_ret) % boost::this_thread::get_id();
                            putLogDebug(600082, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/

                        //search http header result is NG
                        if (!find_ret)
                        {
                            //set data state HTTP_START
                            session_data_ptr->data_state = HTTP_START;

                            //set return status
                            status = CLIENT_RECV;
                        }
                        //search http header result is OK
                        else
                        {
                            //check http method and version
                            check_ret = http_utility::check_http_method_and_version(session_data_ptr->data_buffer + session_data_ptr->data_offset,
                                        session_data_ptr->data_length);

                            /*-------- DEBUG LOG --------*/
                            if (unlikely(LOG_LV_DEBUG == getloglevel()))
                            {
                                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                                        "handle_realserver_send() : call check_http_method_and_version : "
                                                        "return_value = %d. thread id : %d.");
                                formatter % check_ret % boost::this_thread::get_id();
                                putLogDebug(600083, formatter.str(), __FILE__, __LINE__ );
                            }
                            /*------DEBUG LOG END------*/

                            //check method and version result is NG
                            if (check_ret == http_utility::CHECK_NG)
                            {
                                //set current message rest size
                                session_data_ptr->current_message_rest_size = session_data_ptr->data_length;

                                //set data state UNKNOWN
                                session_data_ptr->data_state = UNKNOWN;
                            }
                            //check method and version result is OK
                            else
                            {
                                //search Content_Length header
                                find_ret = http_utility::find_http_header_content_length(session_data_ptr->data_buffer+session_data_ptr->data_offset,
                                           session_data_ptr->data_length,
                                           http_header_content_length_offset,
                                           http_header_content_length_len);

                                /*-------- DEBUG LOG --------*/
                                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                                {
                                    boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                                            "handle_realserver_send() : call find_http_header_content_length : "
                                                            "return_value = %d. thread id : %d.");
                                    formatter % static_cast<int>(find_ret) % boost::this_thread::get_id();
                                    putLogDebug(600084, formatter.str(), __FILE__, __LINE__ );
                                }
                                /*------DEBUG LOG END------*/

                                //search Content_Length result is OK
                                if (find_ret)
                                {
                                    content_length.assign(session_data_ptr->data_buffer+session_data_ptr->data_offset + http_header_content_length_offset,
                                                          http_header_content_length_len );
                                    find_ret = regex_search(content_length.c_str(), regex_ret, content_length_regex);

                                    //"content-length: ddd\r\n"
                                    if ( find_ret)
                                    {
                                        content_length = content_length.substr(
                                                             regex_ret.position(1),
                                                             regex_ret.length(1));

                                        //set content length value
                                        content_length_value = boost::lexical_cast<int>(content_length);
                                    }

                                    //http_header context is "\r\n\r\n" only
                                    if (http_header_all_len == 0)
                                    {
                                        //set current message rest size
                                        session_data_ptr->current_message_rest_size = http_header_all_offset + http_header_all_len + content_length_value + CR_LF_LEN;
                                    }
                                    else
                                    {
                                        //set current message rest size
                                        session_data_ptr->current_message_rest_size = http_header_all_offset + http_header_all_len + content_length_value + CR_LF_CR_LF_LEN;
                                    }
                                }
                                //search Content_Length result is OK
                                else
                                {
                                    //http_header context is "\r\n\r\n" only
                                    if (http_header_all_len == 0)
                                    {
                                        //set current message rest size
                                        session_data_ptr->current_message_rest_size = http_header_all_offset + http_header_all_len + CR_LF_LEN;
                                    }
                                    else
                                    {
                                        //set current message rest size
                                        session_data_ptr->current_message_rest_size = http_header_all_offset + http_header_all_len + CR_LF_CR_LF_LEN;
                                    }

                                }

                                //set data state HTTP_HEADER
                                session_data_ptr->data_state = HTTP_HEADER;
                            }

                            //set return status
                            status = REALSERVER_CONNECT;
                        }
                    }
                    //data state is UNKNOWN
                    else if (session_data_ptr->data_state == UNKNOWN)
                    {
                        //set return status
                        status = REALSERVER_CONNECT;
                    }

                }
                //data size is 0
                else
                {
                    //data state is HTTP_BODY
                    if (session_data_ptr->data_state == HTTP_BODY)
                    {
                        //set data state HTTP_START
                        session_data_ptr->data_state = HTTP_START;
                    }

                    //set data offset 0
                    session_data_ptr->data_offset = 0;

                    //set return status
                    status = CLIENT_RECV;
                }
            }
            //set last status
            session_data_ptr->last_status = status;
        }
        catch (int e)
        {
            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                        "handle_realserver_send() : catch exception e = %d. thread id : %d.");
                formatter % e % boost::this_thread::get_id();
                putLogDebug(600085, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/

            //set return status
            status = FINALIZE;
        }
        catch (const boost::bad_lexical_cast& )
        {
            std::cerr << "protocol_module_ip::handle_realserver_send() : exception : " <<  "Content_Length field's value is invalid." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::handle_realserver_send() : "
                                    "Content_Length field's value is invalid. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(600052, formatter.str(), __FILE__, __LINE__ );

            //set return status
            status = FINALIZE;
        }
        catch (const std::exception& ex)
        {
            std::cerr << "protocol_module_ip::handle_realserver_send() : exception : error = " << ex.what() << "." << std::endl;                //set data state HTTP_HEADER
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_realserver_send() : exception : error = %s. thread id : %d.");
            formatter % ex.what() % boost::this_thread::get_id();
            putLogError(600053, formatter.str(), __FILE__, __LINE__ );

            //set return status
            status = FINALIZE;
        }
        catch (...)
        {
            std::cerr << "protocol_module_ip::handle_realserver_send() : Unknown exception." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG "
                                    "protocol_module_ip::handle_realserver_send() : "
                                    "Unknown exception. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(600054, formatter.str(), __FILE__, __LINE__ );

            //set return status
            status = FINALIZE;
        }

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_realserver_send(const boost::thread::id thread_id) : return_value = %d. thread id : %d.");
            formatter % status % boost::this_thread::get_id();
            putLogDebug(600086, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        return status;
    }

    //! called from after sorryserver select
    //! @param[in]    upstream thread id
    //! @param[in]    sorryserver endpiont reference
    //! @return        session use EVENT mode.
    protocol_module_base::EVENT_TAG protocol_module_ip::handle_sorryserver_select(
        const boost::thread::id thread_id, boost::asio::ip::tcp::endpoint& sorry_endpoint)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_sorryserver_select(const boost::thread::id thread_id, "
                                    "boost::asio::ip::tcp::endpoint& sorry_endpoint) : "
                                    "thread_id = %d, sorry_endpoint = [%s]:%d.");
            formatter % thread_id % sorry_endpoint.address().to_string() % sorry_endpoint.port();
            putLogDebug(600087, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        EVENT_TAG status = FINALIZE;
        boost::asio::ip::tcp::endpoint tmp_endpoint;
        thread_data_ptr session_data_ptr;
        session_thread_data_map_it session_thread_it;
        session_thread_data_map_it session_thread_it_end;

        try
        {
            {
                boost::mutex::scoped_lock slock(session_thread_data_map_mutex);

                session_thread_it = session_thread_data_map.find(thread_id);
                if (unlikely(session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL))
                {
                    boost::format formatter("Invalid thread id. thread id : %d.");
                    formatter % boost::this_thread::get_id();
                    putLogError(600055, formatter.str(), __FILE__, __LINE__ );
                    throw -1;
                }

                session_data_ptr = session_thread_it->second;
            }

            //data state is HTTP_START
            if (session_data_ptr->data_state == HTTP_START)
            {
                //set return status
                status = CLIENT_RECV;
            }
            //data state is not HTTP_START
            else
            {
                //set return status
                status = SORRYSERVER_CONNECT;
            }

            //set last status
            session_data_ptr->last_status = status;
        }
        catch (int e)
        {
            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                        "handle_sorryserver_select() : catch exception e = %d. thread id : %d.");
                formatter % e % boost::this_thread::get_id();
                putLogDebug(600088, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/

            //set return status
            status = FINALIZE;
        }
        catch (const std::exception& ex)
        {
            std::cerr << "protocol_module_ip::handle_sorryserver_select() : exception : error = " << ex.what() << "." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_sorryserver_select() : exception : error = %s. thread id : %d.");
            formatter % ex.what() % boost::this_thread::get_id();
            putLogError(600056, formatter.str(), __FILE__, __LINE__ );

            //set return status
            status = FINALIZE;
        }
        catch (...)
        {
            std::cerr << "protocol_module_ip::handle_sorryserver_select() : Unknown exception." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG "
                                    "protocol_module_ip::handle_sorryserver_select() : "
                                    "Unknown exception. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(600057, formatter.str(), __FILE__, __LINE__ );

            //set return status
            status = FINALIZE;
        }

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_sorryserver_select(const boost::thread::id thread_id, "
                                    "boost::asio::ip::tcp::endpoint& sorry_endpoint)"
                                    " : return_value = %d. thread id : %d.");
            formatter % status % boost::this_thread::get_id();
            putLogDebug(600089, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        return status;
    }

    //! called from after sorryserver connect
    //!    @param[in]    upstream thread id
    //! @param[out]    send buffer reference.
    //! @param[out]    send length
    //! @return        session use EVENT mode.
    protocol_module_base::EVENT_TAG protocol_module_ip::handle_sorryserver_connect(
        const boost::thread::id thread_id, boost::array<char, MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_sorryserver_connect(const boost::thread::id thread_id, "
                                    "boost::array<char, MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen) : "
                                    "thread_id = %d.");
            formatter % thread_id;
            putLogDebug(600090, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        EVENT_TAG status            = FINALIZE;
        size_t send_possible_size        = 0;
        size_t uri_offset            = 0;
        size_t uri_len                = 0;
        size_t x_forwarded_for_insert_pos   = 0;
        thread_data_ptr                session_data_ptr;
        session_thread_data_map_it        session_thread_it;
        std::pair<char*, size_t>        buffer_element;
        std::string                x_forwarded_for_context;

        try
        {
            {
                boost::mutex::scoped_lock slock(session_thread_data_map_mutex);

                //thread id check
                session_thread_it = session_thread_data_map.find(thread_id);
                if (unlikely(session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL))
                {
                    boost::format formatter("Invalid thread id. thread id : %d.");
                    formatter % boost::this_thread::get_id();
                    putLogError(600058, formatter.str(), __FILE__, __LINE__ );
                    throw -1;
                }

                session_data_ptr = session_thread_it->second;
            }
            //set send possible data size
            send_possible_size = std::min(session_data_ptr->current_message_rest_size,
                                          session_data_ptr->data_length
                                         );

            if (session_data_ptr->buffer_sequence.empty())
            {
                //data state is HTTP_HEADER
                if (session_data_ptr->data_state == HTTP_HEADER)
                {
                    //search uri
                    http_utility::find_uri(session_data_ptr->data_buffer + session_data_ptr->data_offset,
                                           session_data_ptr->data_length, uri_offset, uri_len);

                    //set buffer's position
                    buffer_element.first = session_data_ptr->data_buffer + session_data_ptr->data_offset;
                    buffer_element.second = uri_offset;

                    //put buffer data to buffer_sequence
                    session_data_ptr->buffer_sequence.push_back(buffer_element);

                    //set buffer's position
                    buffer_element.first = sorry_uri.data();
                    buffer_element.second = strlen(sorry_uri.data());

                    //put buffer data to buffer_sequence
                    session_data_ptr->buffer_sequence.push_back(buffer_element);

                    //forwarded_for flag is on
                    if (forwarded_for == FORWARDED_FOR_ON)
                    {
                        create_x_forwarded_for(session_data_ptr->client_endpoint.address().to_string(),
                                               session_data_ptr->data_buffer + session_data_ptr->data_offset,
                                               session_data_ptr->data_length,
                                               x_forwarded_for_insert_pos,
                                               x_forwarded_for_context);

                        //set buffer's position
                        buffer_element.first = session_data_ptr->data_buffer
                                               + session_data_ptr->data_offset
                                               + uri_offset
                                               + uri_len;
                        buffer_element.second = x_forwarded_for_insert_pos - uri_offset - uri_len;

                        //put buffer data to buffer_sequence
                        session_data_ptr->buffer_sequence.push_back(buffer_element);

                        session_data_ptr->forwarded_for_buffer.assign(0);

                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            std::string datadump;
                            dump_memory(x_forwarded_for_context.c_str(),
                                        x_forwarded_for_context.size(), datadump);
                            boost::format formatter(
                                "function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                "handle_sorryserver_connect() : before memcpy (data dump) : "
                                "data begin = 0, data_size = %d, data = %s");
                            formatter %  x_forwarded_for_context.size() % datadump;
                            putLogDebug(600091, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/

                        memcpy(session_data_ptr->forwarded_for_buffer.data(),
                               x_forwarded_for_context.c_str(),
                               x_forwarded_for_context.size());

                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            std::string datadump;
                            dump_memory(x_forwarded_for_context.c_str(),
                                        x_forwarded_for_context.size(), datadump);
                            boost::format formatter(
                                "function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                "handle_sorryserver_connect() : after memcpy (data dump) : "
                                "data begin = 0, data_size = %d, data = %s");
                            formatter % x_forwarded_for_context.size() % datadump;
                            putLogDebug(600092, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/

                        //set buffer's position
                        buffer_element.first = session_data_ptr->forwarded_for_buffer.data();
                        buffer_element.second = x_forwarded_for_context.size();
                        //put buffer data to buffer_sequence
                        session_data_ptr->buffer_sequence.push_back(buffer_element);

                        //set buffer's position
                        buffer_element.first = session_data_ptr->data_buffer + session_data_ptr->data_offset + x_forwarded_for_insert_pos;
                        buffer_element.second = send_possible_size - x_forwarded_for_insert_pos;
                        //put buffer data to buffer_sequence
                        session_data_ptr->buffer_sequence.push_back(buffer_element);
                    }
                    //forwarded_for flag is on
                    else
                    {
                        //set buffer's position
                        buffer_element.first = session_data_ptr->data_buffer
                                               + session_data_ptr->data_offset
                                               + uri_offset
                                               + uri_len;
                        buffer_element.second = send_possible_size - uri_offset - uri_len;
                        //put buffer data to buffer_sequence
                        session_data_ptr->buffer_sequence.push_back(buffer_element);
                    }

                    //set data state HTTP_BODY
                    session_data_ptr->data_state = HTTP_BODY;
                }
                //data state is not HTTP_HEADER
                else
                {
                    //set buffer's position
                    buffer_element.first = session_data_ptr->data_buffer + session_data_ptr->data_offset;
                    buffer_element.second = send_possible_size;
                    //put buffer data to buffer_sequence
                    session_data_ptr->buffer_sequence.push_back(buffer_element);
                }
            }

            //put buffer_sequence data into sendbuffer
            put_data_into_sendbuffer(session_data_ptr, sendbuffer, datalen);

            //set return status
            status = SORRYSERVER_SEND;

            //set last status
            session_data_ptr->last_status = status;

        }
        catch (int e)
        {
            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                        "handle_sorryserver_connect() : catch exception e = %d. thread id : %d.");
                formatter % e % boost::this_thread::get_id();
                putLogDebug(600093, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/

            //set return status
            status = FINALIZE;
        }
        catch (const std::exception& ex)
        {
            std::cerr << "protocol_module_ip::handle_sorryserver_connect() : exception : error = " << ex.what() << "." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_sorryserver_connect() : exception : error = %s. thread id : %d.");
            formatter % ex.what() % boost::this_thread::get_id();
            putLogError(600059, formatter.str(), __FILE__, __LINE__ );

            //set return status
            status = FINALIZE;
        }
        catch (...)
        {
            std::cerr << "protocol_module_ip::handle_sorryserver_connect() : Unknown exception." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_sorryserver_connect() : Unknown exception. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(600060, formatter.str(), __FILE__, __LINE__ );

            //set return status
            status = FINALIZE;
        }

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_sorryserver_connect(const boost::thread::id thread_id, "
                                    "boost::array<char,MAX_BUFFER_SIZE>& sendbuffer, "
                                    "size_t& datalen) : return_value = %d. thread id : %d.");
            formatter % status % boost::this_thread::get_id();
            putLogDebug(600094, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        return status;
    }
    //! called from after sorryserver connection fail
    //! @param[in]    upstream thread id
    //! @param[in]    sorryserver endpoint reference.
    //! @return        session use EVENT mode.
    protocol_module_base::EVENT_TAG protocol_module_ip::handle_sorryserver_connection_fail(
        const boost::thread::id thread_id, const boost::asio::ip::tcp::endpoint & sorry_endpoint)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_sorryserver_connection_fail(const boost::thread::id thread_id, "
                                    "const boost::asio::ip::tcp::endpoint& sorry_endpoint) : "
                                    "thread_id = %d, sorry_endpoint = [%s]:%d.");
            formatter % thread_id % sorry_endpoint.address().to_string() % sorry_endpoint.port();
            putLogDebug(600095, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        EVENT_TAG status = FINALIZE;
        thread_data_ptr session_data_ptr;
        session_thread_data_map_it session_thread_it;

        try
        {
            boost::mutex::scoped_lock slock(session_thread_data_map_mutex);

            session_thread_it = session_thread_data_map.find(thread_id);
            if (unlikely(session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL))
            {
                boost::format formatter("Invalid thread id. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogError(600061, formatter.str(), __FILE__, __LINE__ );
                throw -1;
            }

            session_data_ptr = session_thread_it->second;

            //set return status
            status = CLIENT_DISCONNECT;

            //set last status
            session_data_ptr->last_status = status;
        }
        catch (int e)
        {
            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                        "handle_sorryserver_connection_fail() : catch exception e = %d. thread id : %d.");
                formatter % e % boost::this_thread::get_id();
                putLogDebug(600096, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/

            //set return status
            status = FINALIZE;
        }
        catch (const std::exception& ex)
        {
            std::cerr << "protocol_module_ip::handle_sorryserver_connection_fail() : exception : error = " << ex.what() << "." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_sorryserver_connection_fail() : exception : error = %s. thread id : %d.");
            formatter % ex.what() % boost::this_thread::get_id();
            putLogError(600062, formatter.str(), __FILE__, __LINE__ );

            //set return status
            status = FINALIZE;
        }
        catch (...)
        {
            std::cerr << "protocol_module_ip::handle_sorryserver_connection_fail() : Unknown exception." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG "
                                    "protocol_module_ip::handle_sorryserver_connection_fail() : "
                                    "Unknown exception. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(600063, formatter.str(), __FILE__, __LINE__ );

            //set return status
            status = FINALIZE;
        }

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_sorryserver_connection_fail(const boost::thread::id thread_id, "
                                    "const boost::asio::ip::tcp::endpoint& sorry_endpoint) : return_value = %d. thread id : %d.");
            formatter % status % boost::this_thread::get_id();
            putLogDebug(600097, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        return status;
    }

    //! called from after sorryserver send
    //! @param[in]    upstream thread id
    //! @return        session use EVENT mode.
    protocol_module_base::EVENT_TAG protocol_module_ip::handle_sorryserver_send(
        const boost::thread::id thread_id)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_sorryserver_send(const boost::thread::id thread_id) : thread_id = %d.");
            formatter % thread_id;
            putLogDebug(600098, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        EVENT_TAG status                = FINALIZE;
        size_t http_header_all_offset            = 0;
        size_t http_header_all_len            = 0;
        size_t http_header_content_length_offset    = 0;
        size_t http_header_content_length_len        = 0;
        const size_t CR_LF_LEN                = 2;
        const size_t CR_LF_CR_LF_LEN            = 4;
        int content_length_value            = 0;

        std::string content_length;
        cmatch regex_ret;
        cregex content_length_regex = icase("Content-Length") >> ":" >> *~_d >> (s1 = +_d) >> *~_d;


        bool find_ret = false;
        http_utility::CHECK_RESULT_TAG check_ret;

        thread_data_ptr session_data_ptr;
        session_thread_data_map_it session_thread_it;

        try
        {
            {
                boost::mutex::scoped_lock slock(session_thread_data_map_mutex);

                //thread id check
                session_thread_it = session_thread_data_map.find(thread_id);
                if (unlikely(session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL))
                {
                    boost::format formatter("Invalid thread id. thread id : %d.");
                    formatter % boost::this_thread::get_id();
                    putLogError(600064, formatter.str(), __FILE__, __LINE__ );
                    throw -1;
                }

                session_data_ptr = session_thread_it->second;
            }

            //current_message_rest_size　>　0
            if (session_data_ptr->current_message_rest_size > 0)
            {
                //data size　>　0
                if (session_data_ptr->data_length > 0)
                {
                    //set return status
                    status = SORRYSERVER_CONNECT;
                }
                //data size is 0
                else
                {
                    //data offset is 0
                    session_data_ptr->data_offset = 0;

                    //set return status
                    status =  CLIENT_RECV;
                }
            }
            //current_message_rest_size is 0
            else
            {
                //data size　>　0
                if (session_data_ptr->data_length> 0)
                {
                    //data state is HTTP_BODY
                    if (session_data_ptr->data_state == HTTP_BODY)
                    {
                        //search whole http header, get whole http header's offset and length
                        find_ret = http_utility::find_http_header_all(session_data_ptr->data_buffer + session_data_ptr->data_offset,
                                   session_data_ptr->data_length,
                                   http_header_all_offset,
                                   http_header_all_len
                                                                     );

                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                                    "handle_sorryserver_send() : call find_http_header_all : "
                                                    "return_value = %d. thread id : %d.");
                            formatter % static_cast<int>(find_ret) % boost::this_thread::get_id();
                            putLogDebug(600099, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/

                        //search http header result is NG
                        if (!find_ret)
                        {
                            //set data state HTTP_START
                            session_data_ptr->data_state = HTTP_START;

                            //set return status
                            status = CLIENT_RECV;
                        }
                        //check method and version result is OK
                        else
                        {
                            //search Content_Length header
                            check_ret = http_utility::check_http_method_and_version(session_data_ptr->data_buffer + session_data_ptr->data_offset,
                                        session_data_ptr->data_length);

                            /*-------- DEBUG LOG --------*/
                            if (unlikely(LOG_LV_DEBUG == getloglevel()))
                            {
                                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                                        "handle_sorryserver_send() : call check_http_method_and_version : "
                                                        "return_value = %d. thread id : %d.");
                                formatter % check_ret % boost::this_thread::get_id();
                                putLogDebug(600100, formatter.str(), __FILE__, __LINE__ );
                            }
                            /*------DEBUG LOG END------*/

                            //check method and version result is NG
                            if (check_ret == http_utility::CHECK_NG)
                            {
                                //set current message rest size
                                session_data_ptr->current_message_rest_size = session_data_ptr->data_length;

                                //set data state UNKNOWN
                                session_data_ptr->data_state = UNKNOWN;
                            }
                            //check method and version result is OK
                            else
                            {
                                //search Content_Length header
                                find_ret = http_utility::find_http_header_content_length(session_data_ptr->data_buffer+session_data_ptr->data_offset,
                                           session_data_ptr->data_length,
                                           http_header_content_length_offset,
                                           http_header_content_length_len);

                                /*-------- DEBUG LOG --------*/
                                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                                {
                                    boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                                            "handle_sorryserver_send() : call find_http_header_content_length : "
                                                            "return_value = %d. thread id : %d.");
                                    formatter % static_cast<int>(find_ret) % boost::this_thread::get_id();
                                    putLogDebug(600101, formatter.str(), __FILE__, __LINE__ );
                                }
                                /*------DEBUG LOG END------*/

                                //search Content_Length result is OK
                                if (find_ret)
                                {
                                    content_length.assign(session_data_ptr->data_buffer+session_data_ptr->data_offset + http_header_content_length_offset,
                                                          http_header_content_length_len );
                                    find_ret = regex_search(content_length.c_str(), regex_ret, content_length_regex);

                                    //"content-length: ddd\r\n"
                                    if ( find_ret)
                                    {
                                        content_length = content_length.substr(
                                                             regex_ret.position(1),
                                                             regex_ret.length(1));

                                        //set content length value
                                        content_length_value = boost::lexical_cast<int>(content_length);
                                    }

                                    //http_header context is "\r\n\r\n" only
                                    if (http_header_all_len == 0)
                                    {
                                        //set current message rest size
                                        session_data_ptr->current_message_rest_size = http_header_all_offset + http_header_all_len + content_length_value + CR_LF_LEN;
                                    }
                                    else
                                    {
                                        //set current message rest size
                                        session_data_ptr->current_message_rest_size = http_header_all_offset + http_header_all_len + content_length_value + CR_LF_CR_LF_LEN;
                                    }
                                }
                                //search Content_Length result is OK
                                else
                                {
                                    //http_header context is "\r\n\r\n" only
                                    if (http_header_all_len == 0)
                                    {
                                        //set current message rest size
                                        session_data_ptr->current_message_rest_size = http_header_all_offset + http_header_all_len + CR_LF_LEN;
                                    }
                                    else
                                    {
                                        //set current message rest size
                                        session_data_ptr->current_message_rest_size = http_header_all_offset + http_header_all_len + CR_LF_CR_LF_LEN;
                                    }

                                }

                                //set data state HTTP_HEADER
                                session_data_ptr->data_state = HTTP_HEADER;

                            }
                            //set return status
                            status = SORRYSERVER_CONNECT;
                        }
                    }
                    //data state is UNKNOWN
                    else if (session_data_ptr->data_state == UNKNOWN)
                    {
                        //set return status
                        status = SORRYSERVER_CONNECT;
                    }

                }
                //data size is 0
                else
                {
                    //data state is HTTP_BODY
                    if (session_data_ptr->data_state == HTTP_BODY)
                    {
                        //set data state HTTP_START
                        session_data_ptr->data_state = HTTP_START;
                    }

                    //set data offset 0
                    session_data_ptr->data_offset = 0;

                    //set return status
                    status = CLIENT_RECV;
                }
            }

            //set last status
            session_data_ptr->last_status = status;
        }
        catch (int e)
        {
            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                        "handle_sorryserver_send() : catch exception e = %d. thread id : %d.");
                formatter % e % boost::this_thread::get_id();
                putLogDebug(600102, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/

            //set return status
            status = FINALIZE;
        }
        catch (const boost::bad_lexical_cast& )
        {
            std::cerr << "protocol_module_ip::handle_sorryserver_send() : exception : " <<  "Content_Length field's value is invalid." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::handle_sorryserver_send() : "
                                    "Content_Length field's value is invalid. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(600065, formatter.str(), __FILE__, __LINE__ );

            //set return status
            status = FINALIZE;
        }
        catch (const std::exception& ex)
        {
            std::cerr << "protocol_module_ip::handle_sorryserver_send() : exception : error = " << ex.what() << "." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_sorryserver_send() : exception : error = %s. thread id : %d.");
            formatter % ex.what() % boost::this_thread::get_id();
            putLogError(600066, formatter.str(), __FILE__, __LINE__ );

            //set return status
            status = FINALIZE;
        }
        catch (...)
        {
            std::cerr << "protocol_module_ip::handle_sorryserver_send() : Unknown exception." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG "
                                    "protocol_module_ip::handle_sorryserver_send() : "
                                    "Unknown exception. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(600067, formatter.str(), __FILE__, __LINE__ );

            //set return status
            status = FINALIZE;
        }

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_sorryserver_send(const boost::thread::id thread_id) : return_value = %d. thread id : %d.");
            formatter % status % boost::this_thread::get_id();
            putLogDebug(600103, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        return status;
    }

    //! called from after realserver recive.for UDP
    //! @param[in]    downstream thread id
    //! @param[in]    realserver UDP endpoint reference
    //! @param[in]    recive from realserver buffer reference
    //! @param[in]    recv data length
    //! @return        session use EVENT mode.
    protocol_module_base::EVENT_TAG protocol_module_ip::handle_realserver_recv(
        const boost::thread::id thread_id, const boost::asio::ip::udp::endpoint& rs_endpoint, const boost::array<char,
        MAX_BUFFER_SIZE>& recvbuffer, const size_t recvlen)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("in/out_function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_realserver_recv(const boost::thread::id thread_id, "
                                    "const boost::asio::ip::udp::endpoint& rs_endpoint, const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                    "const size_t& recvlen) : "
                                    "return_value = %d. thread id : %d.");
            formatter % STOP % boost::this_thread::get_id();
            putLogDebug(600104, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/
        return STOP;
    }

    //! called from after realserver recvive for TCP/IP
    //! @param[in]    downstream thread id
    //! @param[in]    realserver TCP/IP endpoint reference
    //! @param[in]    realserver recive buffer reference.
    //! @param[in]    recv data length
    //! @return        session use EVENT mode.
    protocol_module_base::EVENT_TAG protocol_module_ip::handle_realserver_recv(
        const boost::thread::id thread_id, const boost::asio::ip::tcp::endpoint& rs_endpoint, const boost::array<char,
        MAX_BUFFER_SIZE>& recvbuffer, const size_t recvlen)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            size_t buffer_size = recvbuffer.size() < recvlen ? recvbuffer.size() : recvlen;
            std::string buffer;
            dump_memory(recvbuffer.data(), buffer_size, buffer);
            boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_realserver_recv(const boost::thread::id thread_id, "
                                    "const boost::asio::ip::tcp::endpoint& rs_endpoint, "
                                    "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                    "const size_t recvlen) : thread_id = %d, rs_endpoint = [%s]:%d, recvbuffer = %s, recvlen = %d.");
            formatter % thread_id % rs_endpoint.address().to_string() % rs_endpoint.port()
            % buffer % recvlen;
            putLogDebug(600105, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        EVENT_TAG status                = FINALIZE;

        bool find_ret                    = false;
        size_t http_header_offset            = 0;
        size_t http_header_len                = 0;
        size_t http_header_content_length_offset    = 0;
        size_t http_header_content_length_len        = 0;
        int content_length_value            = 0;
        const size_t CR_LF_LEN                = 2; //length of "\r\n"
        const size_t CR_LF_CR_LF_LEN            = 4; //length of "\r\n\r\n"

        session_thread_data_map_it            session_thread_it;
        thread_data_ptr                    session_data_ptr;
        http_utility::CHECK_RESULT_TAG            check_ret;

        std::string                    content_length;
        cmatch                        regex_ret;
        cregex content_length_regex = icase("Content-Length") >> ":" >> *~_d >> (s1 = +_d) >> *~_d;

        if (unlikely(recvlen > recvbuffer.size()))
        {
            std::cerr << "protocol_module_ip::handle_realserver_recv() : Data size bigger than buffer size." << std::endl;
            boost::format formatter("Data size bigger than buffer size. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(600068, formatter.str(), __FILE__, __LINE__ );

            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                        "handle_realserver_recv(const boost::thread::id thread_id, "
                                        "const boost::asio::ip::tcp::endpoint& rs_endpoint, "
                                        "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                        "const size_t recvlen) : return_value = %d. thread id : %d.");
                formatter % FINALIZE % boost::this_thread::get_id();
                putLogDebug(600106, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/

            return status;
        }

        try
        {
            {
                boost::mutex::scoped_lock slock(session_thread_data_map_mutex);

                session_thread_it = session_thread_data_map.find(thread_id);
                if (unlikely(session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL))
                {
                    boost::format formatter("Invalid thread id. thread id : %d.");
                    formatter % boost::this_thread::get_id();
                    putLogError(600069, formatter.str(), __FILE__, __LINE__ );
                    throw -1;
                }

                session_data_ptr = session_thread_it->second;
            }

            //set switch flag off
            session_data_ptr->switch_flag = SWITCH_FLAG_OFF;

            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                        "handle_realserver_recv(const boost::thread::id thread_id, "
                                        "const boost::asio::ip::tcp::endpoint& rs_endpoint, "
                                        "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                        "const size_t recvlen) : SWITCH_FLAG_OFF. "
                                        "thread_id = %d, rs_endpoint = [%s]:%d.");
                formatter % thread_id % rs_endpoint.address().to_string() % rs_endpoint.port();
                putLogDebug(600107, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/

            //copy data from recvbuffer
            if (!get_data_from_recvbuffer(session_data_ptr, recvbuffer, recvlen))
            {
                //copy failed
                std::cerr << "protocol_module_ip::handle_realserver_recv() : Data size bigger than buffer size." << std::endl;
                boost::format formatter("Data size bigger than buffer size. thread id : % id.");
                formatter % boost::this_thread::get_id();
                putLogError(600070, formatter.str(), __FILE__, __LINE__);
                status = FINALIZE;
            }
            else
            {
                //data state is HTTP_START
                if (session_data_ptr->data_state == HTTP_START)
                {
                    //search http header
                    find_ret = http_utility::find_http_header_all(session_data_ptr->data_buffer + session_data_ptr->data_offset,
                               session_data_ptr->data_length,
                               http_header_offset,
                               http_header_len
                                                                 );

                    /*-------- DEBUG LOG --------*/
                    if (unlikely(LOG_LV_DEBUG == getloglevel()))
                    {
                        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                                "handle_realserver_recv() : call find_http_header_all : "
                                                "return_value = %d. thread id : %d.");
                        formatter % static_cast<int>(find_ret) % boost::this_thread::get_id();
                        putLogDebug(600108, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/

                    //search http header result is NG
                    if (!find_ret)
                    {
                        //data size bigger than max buffer size
                        if (session_data_ptr->data_length >= MAX_IP_MODULE_BUFFER_SIZE - recvbuffer.size())
                        {
                            //set data state UNKNOWN
                            session_data_ptr->data_state = UNKNOWN;
                            //set current message rest size
                            session_data_ptr->current_message_rest_size = session_data_ptr->data_length;
                        }
                    }
                    //search http header result is OK
                    else
                    {
                        //check http version and status code
                        check_ret = http_utility::check_http_version_and_status_code(session_data_ptr->data_buffer,
                                    session_data_ptr->data_length);

                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                                    "handle_realserver_recv() : call check_http_version_and_status_code : "
                                                    "return_value = %d. thread id : %d.");
                            formatter % check_ret % boost::this_thread::get_id();
                            putLogDebug(600109, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/

                        //check http version and status code result is NG
                        if (check_ret == http_utility::CHECK_NG)
                        {
                            //set data state UNKNOWN
                            session_data_ptr->data_state = UNKNOWN;
                            //set current message rest size
                            session_data_ptr->current_message_rest_size = session_data_ptr->data_length;
                        }
                        //check http version and status code result is OK
                        else
                        {
                            //search Content_Length header
                            find_ret = http_utility::find_http_header_content_length(session_data_ptr->data_buffer+session_data_ptr->data_offset,
                                       session_data_ptr->data_length,
                                       http_header_content_length_offset,
                                       http_header_content_length_len);

                            /*-------- DEBUG LOG --------*/
                            if (unlikely(LOG_LV_DEBUG == getloglevel()))
                            {
                                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                                        "handle_realserver_recv() : call find_http_header_content_length : "
                                                        "return_value = %d. thread id : %d.");
                                formatter % static_cast<int>(find_ret) % boost::this_thread::get_id();
                                putLogDebug(600110, formatter.str(), __FILE__, __LINE__ );
                            }
                            /*------DEBUG LOG END------*/

                            //search Content_Length result is OK
                            if (find_ret)
                            {
                                //set content length string
                                content_length.assign(session_data_ptr->data_buffer+session_data_ptr->data_offset + http_header_content_length_offset,
                                                      http_header_content_length_len );
                                find_ret = regex_search(content_length.c_str(), regex_ret, content_length_regex);

                                //"content-length: ddd\r\n"
                                if ( find_ret)
                                {
                                    content_length = content_length.substr(
                                                         regex_ret.position(1),
                                                         regex_ret.length(1));

                                    //set content length value
                                    content_length_value = boost::lexical_cast<int>(content_length);
                                }

                                //http_header context is "\r\n\r\n" only
                                if (http_header_len == 0)
                                {
                                    //set current message rest size
                                    session_data_ptr->current_message_rest_size = http_header_offset + http_header_len + content_length_value + CR_LF_LEN;
                                }
                                else
                                {
                                    //set current message rest size
                                    session_data_ptr->current_message_rest_size = http_header_offset + http_header_len + content_length_value + CR_LF_CR_LF_LEN;
                                }
                            }
                            //search Content_Length result is NG
                            else
                            {
                                //http_header context is "\r\n\r\n" only
                                if (http_header_len == 0)
                                {
                                    //set current message rest size
                                    session_data_ptr->current_message_rest_size = http_header_offset + http_header_len + CR_LF_LEN;
                                }
                                else
                                {
                                    //set current message rest size
                                    session_data_ptr->current_message_rest_size = http_header_offset + http_header_len + CR_LF_CR_LF_LEN;
                                }

                            }

                            //set data state HTTP_HEADER
                            session_data_ptr->data_state = HTTP_HEADER;
                        }
                    }
                }
                //data state is UNKNOWN
                else if (session_data_ptr->data_state == UNKNOWN)
                {
                    //set current message rest size
                    session_data_ptr->current_message_rest_size = session_data_ptr->data_length;
                }
                else
                {
                    //none
                }

                //data state is HTTP_START
                if (session_data_ptr->data_state == HTTP_START)
                {
                    //set return status
                    status = REALSERVER_RECV;
                }
                //data state is not HTTP_START
                else
                {
                    //set return status
                    status = CLIENT_CONNECTION_CHECK;
                }
            }

            //set last status
            session_data_ptr->last_status = status;
        }
        catch (int e)
        {
            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                        "handle_realserver_recv() : catch exception e = %d. thread id : %d.");
                formatter % e % boost::this_thread::get_id();
                putLogDebug(600111, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/

            //set return status
            status = FINALIZE;
        }
        catch (const boost::bad_lexical_cast& )
        {
            std::cerr << "protocol_module_ip::handle_realserver_recv() : exception : " <<  "Content_Length field's value is invalid." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::handle_realserver_recv() : "
                                    "Content_Length field's value is invalid. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(600071, formatter.str(), __FILE__, __LINE__ );

            //set return status
            status = FINALIZE;
        }
        catch (const std::exception& ex)
        {
            std::cerr << "protocol_module_ip::handle_realserver_recv() : exception : error = " << ex.what() << "." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_realserver_recv() : exception : error = %s. thread id : %d.");
            formatter % ex.what() % boost::this_thread::get_id();
            putLogError(600072, formatter.str(), __FILE__, __LINE__ );

            //set return status
            status = FINALIZE;
        }
        catch (...)
        {
            std::cerr << "protocol_module_ip::handle_realserver_recv() : Unknown exception." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG "
                                    "protocol_module_ip::handle_realserver_recv() : "
                                    "Unknown exception. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(600073, formatter.str(), __FILE__, __LINE__ );

            //set return status
            status = FINALIZE;
        }

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_realserver_recv(const boost::thread::id thread_id, "
                                    "const boost::asio::ip::tcp::endpoint& rs_endpoint, "
                                    "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                    "const size_t recvlen) : return_value = %d. thread id : %d.");
            formatter % FINALIZE % boost::this_thread::get_id();
            putLogDebug(600112, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        return status;
    }



    //! called from after sorryserver recive
    //! @param[in]    downstream thread id
    //! @param[in]    sorryserver endpoint reference
    //! @param[in]    recive from realserver buffer reference.
    //! @param[in]    recv data length
    //! @return     session use EVENT mode
    protocol_module_base::EVENT_TAG protocol_module_ip::handle_sorryserver_recv(
        const boost::thread::id thread_id, const boost::asio::ip::tcp::endpoint& sorry_endpoint, const boost::array<
        char, MAX_BUFFER_SIZE>& recvbuffer, const size_t recvlen)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            size_t buffer_size = recvbuffer.size() < recvlen ? recvbuffer.size() : recvlen;
            std::string buffer;
            dump_memory(recvbuffer.data(), buffer_size, buffer);
            boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_sorryserver_recv(const boost::thread::id thread_id, "
                                    "const boost::asio::ip::tcp::endpoint& sorry_endpoint, "
                                    "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                    "const size_t recvlen) : thread_id = %d, sorry_endpoint = [%s]:%d, recvbuffer = %s, recvlen = %d.");
            formatter % thread_id % sorry_endpoint.address().to_string() % sorry_endpoint.port()
            % buffer % recvlen;
            putLogDebug(600113, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        EVENT_TAG status                = FINALIZE;
        bool find_ret                    = false;
        size_t http_header_offset            = 0;
        size_t http_header_len                = 0;
        size_t http_header_content_length_offset    = 0;
        size_t http_header_content_length_len        = 0;
        int content_length_value            = 0;
        const size_t CR_LF_LEN                = 2; //length of "\r\n"
        const size_t CR_LF_CR_LF_LEN            = 4; //length of "\r\n\r\n"

        session_thread_data_map_it            session_thread_it;
        thread_data_ptr                    session_data_ptr;
        http_utility::CHECK_RESULT_TAG            check_ret;
        std::string                    content_length;
        cmatch                        regex_ret;
        cregex content_length_regex = icase("Content-Length") >> ":" >> *~_d >> (s1 = +_d) >> *~_d;

        if (unlikely(recvlen > recvbuffer.size()))
        {
            std::cerr << "protocol_module_ip::handle_sorryserver_recv() : Data size bigger than buffer size." << std::endl;
            boost::format formatter("Data size bigger than buffer size. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(600074, formatter.str(), __FILE__, __LINE__ );

            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                        "handle_sorryserver_recv(const boost::thread::id thread_id, "
                                        "const boost::asio::ip::tcp::endpoint& sorry_endpoint, "
                                        "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                        "const size_t recvlen) : return_value = %d. thread id : %d.");
                formatter % FINALIZE % boost::this_thread::get_id();
                putLogDebug(600114, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/

            return status;
        }

        try
        {
            {
                boost::mutex::scoped_lock slock(session_thread_data_map_mutex);

                session_thread_it = session_thread_data_map.find(thread_id);
                if (unlikely(session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL))
                {
                    boost::format formatter("Invalid thread id. thread id : %d.");
                    formatter % boost::this_thread::get_id();
                    putLogError(600075, formatter.str(), __FILE__, __LINE__ );
                    throw -1;
                }

                session_data_ptr = session_thread_it->second;
            }

            //set switch flag off
            session_data_ptr->switch_flag = SWITCH_FLAG_OFF;

            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                        "handle_sorryserver_recv(const boost::thread::id thread_id, "
                                        "const boost::asio::ip::tcp::endpoint& sorry_endpoint, "
                                        "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                        "const size_t recvlen) : SWITCH_FLAG_OFF. "
                                        "thread_id = %d, rs_endpoint = [%s]:%d.");
                formatter % thread_id % sorry_endpoint.address().to_string() % sorry_endpoint.port();
                putLogDebug(600115, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/

            //copy data from recvbuffer
            if (!get_data_from_recvbuffer(session_data_ptr, recvbuffer, recvlen))
            {
                //copy failed
                std::cerr << "protocol_module_ip::handle_sorryserver_recv() : Data size bigger than buffer size." << std::endl;
                boost::format formatter("Data size bigger than buffer size. thread id : % id.");
                formatter % boost::this_thread::get_id();
                putLogError(600076, formatter.str(), __FILE__, __LINE__);

                status = FINALIZE;
            }
            else
            {
                //data state is HTTP_START
                if (session_data_ptr->data_state == HTTP_START)
                {
                    //search http header
                    find_ret = http_utility::find_http_header_all(session_data_ptr->data_buffer + session_data_ptr->data_offset,
                               session_data_ptr->data_length,
                               http_header_offset,
                               http_header_len
                                                                 );

                    /*-------- DEBUG LOG --------*/
                    if (unlikely(LOG_LV_DEBUG == getloglevel()))
                    {
                        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                                "handle_sorryserver_recv() : call find_http_header_all : "
                                                "return_value = %d. thread id : %d.");
                        formatter % static_cast<int>(find_ret) % boost::this_thread::get_id();
                        putLogDebug(600116, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/

                    //search http header result is NG
                    if (!find_ret)
                    {
                        //data size bigger than max buffer size
                        if (session_data_ptr->data_length >= MAX_IP_MODULE_BUFFER_SIZE - recvbuffer.size())
                        {
                            //set data state UNKNOWN
                            session_data_ptr->data_state = UNKNOWN;
                            //set current message rest size
                            session_data_ptr->current_message_rest_size = session_data_ptr->data_length;
                        }
                    }
                    //search http header result is OK
                    else
                    {
                        //check http version and status code
                        check_ret = http_utility::check_http_version_and_status_code(session_data_ptr->data_buffer,
                                    session_data_ptr->data_length);

                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                                    "handle_sorryserver_recv() : call check_http_version_and_status_code : "
                                                    "return_value = %d. thread id : %d.");
                            formatter % check_ret % boost::this_thread::get_id();
                            putLogDebug(600117, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/

                        //check http version and status code result is NG
                        if (check_ret == http_utility::CHECK_NG)
                        {
                            //set data state UNKNOWN
                            session_data_ptr->data_state = UNKNOWN;
                            //set current message rest size
                            session_data_ptr->current_message_rest_size = session_data_ptr->data_length;
                        }
                        //check http version and status code result is OK
                        else
                        {
                            //search Content_Length header
                            find_ret = http_utility::find_http_header_content_length(session_data_ptr->data_buffer+session_data_ptr->data_offset,
                                       session_data_ptr->data_length,
                                       http_header_content_length_offset,
                                       http_header_content_length_len);

                            /*-------- DEBUG LOG --------*/
                            if (unlikely(LOG_LV_DEBUG == getloglevel()))
                            {
                                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                                        "handle_sorryserver_recv() : call find_http_header_content_length : "
                                                        "return_value = %d. thread id : %d.");
                                formatter % static_cast<int>(find_ret) % boost::this_thread::get_id();
                                putLogDebug(600118, formatter.str(), __FILE__, __LINE__ );
                            }
                            /*------DEBUG LOG END------*/

                            //search Content_Length result is OK
                            if (find_ret)
                            {
                                ////set content length string
                                content_length.assign(session_data_ptr->data_buffer+session_data_ptr->data_offset + http_header_content_length_offset,
                                                      http_header_content_length_len );
                                find_ret = regex_search(content_length.c_str(), regex_ret, content_length_regex);

                                //"content-length: ddd\r\n"
                                if ( find_ret)
                                {
                                    content_length = content_length.substr(
                                                         regex_ret.position(1),
                                                         regex_ret.length(1));

                                    //set content length value
                                    content_length_value = boost::lexical_cast<int>(content_length);
                                }

                                //http_header context is "\r\n\r\n" only
                                if (http_header_len == 0)
                                {
                                    //set current message rest size
                                    session_data_ptr->current_message_rest_size = http_header_offset + http_header_len + content_length_value + CR_LF_LEN;
                                }
                                else
                                {
                                    //set current message rest size
                                    session_data_ptr->current_message_rest_size = http_header_offset + http_header_len + content_length_value + CR_LF_CR_LF_LEN;
                                }
                            }
                            //search Content_Length result is NG
                            else
                            {
                                //http_header context is "\r\n\r\n" only
                                if (http_header_len == 0)
                                {
                                    //set current message rest size
                                    session_data_ptr->current_message_rest_size = http_header_offset + http_header_len + CR_LF_LEN;
                                }
                                else
                                {
                                    //set current message rest size
                                    session_data_ptr->current_message_rest_size = http_header_offset + http_header_len + CR_LF_CR_LF_LEN;
                                }

                            }

                            //set data state HTTP_HEADER
                            session_data_ptr->data_state = HTTP_HEADER;

                        }
                    }
                }
                //data state is UNKNOWN
                else if (session_data_ptr->data_state == UNKNOWN)
                {
                    //set current message rest size
                    session_data_ptr->current_message_rest_size = session_data_ptr->data_length;
                }
                else
                {
                    //none
                }

                //data state is HTTP_START
                if (session_data_ptr->data_state == HTTP_START)
                {
                    //set return status
                    status = SORRYSERVER_RECV;
                }
                //data state is not HTTP_START
                else
                {
                    //set return status
                    status = CLIENT_CONNECTION_CHECK;
                }
            }

            //set last status
            session_data_ptr->last_status = status;
        }
        catch (int e)
        {
            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                        "handle_sorryserver_recv() : catch exception e = %d. thread id : %d.");
                formatter % e % boost::this_thread::get_id();
                putLogDebug(600119, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/

            //set return status
            status = FINALIZE;
        }
        catch (const boost::bad_lexical_cast& )
        {
            std::cerr << "protocol_module_ip::handle_sorryserver_recv() : exception : " <<  "Content_Length field's value is invalid." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::handle_sorryserver_recv() : "
                                    "Content_Length field's value is invalid. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(600077, formatter.str(), __FILE__, __LINE__ );

            //set return status
            status = FINALIZE;
        }
        catch (const std::exception& ex)
        {
            std::cerr << "protocol_module_ip::handle_sorryserver_recv() : exception : error = " << ex.what() << "." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_sorryserver_recv() : exception : error = %s. thread id : %d.");
            formatter % ex.what() % boost::this_thread::get_id();
            putLogError(600078, formatter.str(), __FILE__, __LINE__ );

            //set return status
            status = FINALIZE;
        }
        catch (...)
        {
            std::cerr << "protocol_module_ip::handle_sorryserver_recv() : Unknown exception." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG "
                                    "protocol_module_ip::handle_sorryserver_recv() : "
                                    "Unknown exception. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(600079, formatter.str(), __FILE__, __LINE__ );

            //set return status
            status = FINALIZE;
        }

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_sorryserver_recv(const boost::thread::id thread_id, "
                                    "const boost::asio::ip::tcp::endpoint& sorry_endpoint, "
                                    "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                    "const size_t recvlen) : return_value = %d. thread id : %d.");
            formatter % FINALIZE % boost::this_thread::get_id();
            putLogDebug(600120, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        return status;

    }

    //! called from UPSTEEARM thread. make module original message.
    //! @param[in]    downstream thread id.
    //! @return     session use EVENT mode
    protocol_module_base::EVENT_TAG protocol_module_ip::handle_response_send_inform(
        const boost::thread::id thread_id)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("in/out_function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_response_send_inform(const boost::thread::id thread_id) : "
                                    "return_value = %d. thread id : %d.");
            formatter % STOP % boost::this_thread::get_id();
            putLogDebug(600121, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        return STOP;
    }

    //! called from after client connection check. use TCP/IP only. create client send message.
    //! @param[in]    downstream thread id
    //! @param[out]    send budffer reference
    //! @param[out]    send data length
    //! @return     session use EVENT mode
    protocol_module_base::EVENT_TAG protocol_module_ip::handle_client_connection_check(
        const boost::thread::id thread_id, boost::array<char, MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_client_connection_check(const boost::thread::id thread_id, "
                                    "boost::array<char, MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen) : "
                                    "thread_id = %d.");
            formatter % thread_id;
            putLogDebug(600122, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        EVENT_TAG status = FINALIZE;
        thread_data_ptr session_data_ptr;
        size_t send_possible_size = 0;

        try
        {
            {
                boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);

                session_thread_data_map_it session_thread_it = session_thread_data_map.find(thread_id);
                if (unlikely(session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL))
                {
                    boost::format formatter("Invalid thread id. thread id : %d.");
                    formatter % boost::this_thread::get_id();
                    putLogError(600080, formatter.str(), __FILE__, __LINE__ );
                    throw -1;
                }

                session_data_ptr = session_thread_it->second;
            }

            //data state is HTTP_HEADER
            if (session_data_ptr->data_state == HTTP_HEADER)
            {
                //set data HTTP_BODY
                session_data_ptr->data_state = HTTP_BODY;
            }

            //set send possible data size
            send_possible_size = std::min(std::min(sendbuffer.size(), session_data_ptr->current_message_rest_size),
                                          session_data_ptr->data_length
                                         );
            //set send data size
            datalen = send_possible_size;

            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                std::string datadump;
                dump_memory(session_data_ptr->data_buffer + session_data_ptr->data_offset, send_possible_size, datadump);

                boost::format formatter(
                    "function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                    "handle_client_connection_check() : before memcpy (data dump) : "
                    "data begin = %d, data_size = %d, data = %s");
                formatter % session_data_ptr->data_offset % send_possible_size % datadump;
                putLogDebug(600123, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/

            //copy send possible data to sendbuffer
            memcpy(sendbuffer.data(), session_data_ptr->data_buffer + session_data_ptr->data_offset,
                   send_possible_size);

            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                std::string datadump;
                dump_memory(sendbuffer.data(), send_possible_size, datadump);

                boost::format formatter(
                    "function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                    "handle_client_connection_check() : after memcpy (data dump) : "
                    "data begin = 0, data_size = %d, data = %s");
                formatter % send_possible_size % datadump;
                putLogDebug(600124, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/

            //set current message rest size
            session_data_ptr->current_message_rest_size -= send_possible_size;

            //set buffer's position
            session_data_ptr->data_offset += send_possible_size;
            session_data_ptr->data_length -= send_possible_size;

            //current message rest size is 0
            if (session_data_ptr->current_message_rest_size == 0)
            {
                boost::mutex::scoped_lock lock(session_data_mutex);
                time_t now;
                time(&now);
                boost::asio::ip::tcp::endpoint init_endpoint;

                //write session data to session table
                ip_data_processor->write_session_data(session_data_ptr->ip_hash,
                                                      init_endpoint,
                                                      now);

            }

            //set return status
            status = CLIENT_SEND;

            //set last status
            session_data_ptr->last_status = status;
        }
        catch (int e)
        {
            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                        "handle_client_connection_check() : catch exception e = %d. thread id : %d.");
                formatter % e % boost::this_thread::get_id();
                putLogDebug(600125, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/

            //set return status
            status = FINALIZE;
        }
        catch (const std::exception& ex)
        {
            std::cerr << "protocol_module_ip::handle_client_connection_check() : exception : error = " << ex.what() << "." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_client_connection_check() : exception : error = %s. thread id : %d.");
            formatter % ex.what() % boost::this_thread::get_id();
            putLogError(600081, formatter.str(), __FILE__, __LINE__ );

            //set return status
            status = FINALIZE;
        }
        catch (...)
        {
            std::cerr << "protocol_module_ip::handle_client_connection_check() : Unknown exception." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_client_connection_check() : Unknown exception. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(600082, formatter.str(), __FILE__, __LINE__ );

            //set return status
            status = FINALIZE;
        }

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_client_connection_check(const boost::thread::id thread_id, "
                                    "boost::array<char, MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen) : return_value = %d. thread id : %d.");
            formatter % status % boost::this_thread::get_id();
            putLogDebug(600126, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        return status;
    }

    //! called from after client select. use UDP only
    //! @param[in]    downstream thread id
    //!    @param[in]    client udp endpoint
    //! @param[out]    send buffer reference
    //! @param[out]    send data length
    //! @return     session use EVENT mode
    protocol_module_base::EVENT_TAG protocol_module_ip::handle_client_select(
        const boost::thread::id thread_id, boost::asio::ip::udp::endpoint& cl_endpoint, boost::array<char,
        MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("in/out_function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_client_select(const boost::thread::id thread_id, "
                                    "boost::asio::ip::udp::endpoint& cl_endpoint, boost::array<char,MAX_BUFFER_SIZE>& sendbuffer, "
                                    "const size_t& datalen) : "
                                    "return_value = %d. thread id : %d.");
            formatter % STOP % boost::this_thread::get_id();
            putLogDebug(600127, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/
        return STOP;
    }

    //!    called from after client send
    //!    @param[in]    downstream thread id
    //! @return     session use EVENT mode
    protocol_module_base::EVENT_TAG protocol_module_ip::handle_client_send(
        const boost::thread::id thread_id)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_client_send(const boost::thread::id thread_id) : thread_id = %d.");
            formatter % thread_id;
            putLogDebug(600128, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        EVENT_TAG status = FINALIZE;
        size_t http_header_all_offset        = 0;
        size_t http_header_all_len        = 0;
        size_t http_header_content_length_offset= 0;
        size_t http_header_content_length_len    = 0;
        const size_t CR_LF_LEN             = 2;
        const size_t CR_LF_CR_LF_LEN        = 4;
        int content_length_value        = 0;

        std::string content_length;
        cmatch regex_ret;
        cregex content_length_regex = icase("Content-Length") >> ":" >> *~_d >> (s1 = +_d) >> *~_d;


        bool find_ret = false;
        http_utility::CHECK_RESULT_TAG check_ret;

        thread_data_ptr session_data_ptr;
        session_thread_data_map_it session_thread_it;

        try
        {
            {
                boost::mutex::scoped_lock slock(session_thread_data_map_mutex);

                //thread id check
                session_thread_it = session_thread_data_map.find(thread_id);
                if (unlikely(session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL))
                {
                    boost::format formatter("Invalid thread id. thread id : %d.");
                    formatter % boost::this_thread::get_id();
                    putLogError(600083, formatter.str(), __FILE__, __LINE__ );
                    throw -1;
                }

                session_data_ptr = session_thread_it->second;
            }

            //current_message_rest_size　>　0
            if (session_data_ptr->current_message_rest_size > 0)
            {
                //data size　>　0
                if (session_data_ptr->data_length > 0)
                {
                    //set return status
                    status = CLIENT_CONNECTION_CHECK;
                }
                //data size is 0
                else
                {
                    //data offset is 0
                    session_data_ptr->data_offset = 0;

                    //set return status
                    status = REALSERVER_RECV;
                }
            }
            //current_message_rest_size is 0
            else
            {
                //data size　>　0
                if (session_data_ptr->data_length > 0)
                {
                    //data state is HTTP_BODY
                    if (session_data_ptr->data_state == HTTP_BODY)
                    {
                        //search whole http header, get whole http header's offset and length
                        find_ret = http_utility::find_http_header_all(session_data_ptr->data_buffer + session_data_ptr->data_offset,
                                   session_data_ptr->data_length,
                                   http_header_all_offset,
                                   http_header_all_len
                                                                     );

                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                                    "handle_client_send() : call find_http_header_all : "
                                                    "return_value = %d. thread id : %d.");
                            formatter % static_cast<int>(find_ret) % boost::this_thread::get_id();
                            putLogDebug(600129, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/

                        //search http header result is NG
                        if (!find_ret)
                        {
                            //set data state HTTP_START
                            session_data_ptr->data_state = HTTP_START;

                            //set return status
                            status = REALSERVER_RECV;
                        }
                        //search http header result is OK
                        else
                        {
                            //check http version and status code
                            check_ret = http_utility::check_http_version_and_status_code(session_data_ptr->data_buffer + session_data_ptr->data_offset,
                                        session_data_ptr->data_length);

                            /*-------- DEBUG LOG --------*/
                            if (unlikely(LOG_LV_DEBUG == getloglevel()))
                            {
                                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                                        "handle_client_send() : call check_http_version_and_status_code : "
                                                        "return_value = %d. thread id : %d.");
                                formatter % check_ret % boost::this_thread::get_id();
                                putLogDebug(600130, formatter.str(), __FILE__, __LINE__ );
                            }
                            /*------DEBUG LOG END------*/

                            //check version and status code result is NG
                            if (check_ret == http_utility::CHECK_NG)
                            {
                                //set current message rest size
                                session_data_ptr->current_message_rest_size = session_data_ptr->data_length;

                                //set data state UNKNOWN
                                session_data_ptr->data_state = UNKNOWN;
                            }
                            //check version and status code result is OK
                            else
                            {
                                //search Content_Length header
                                find_ret = http_utility::find_http_header_content_length(session_data_ptr->data_buffer+session_data_ptr->data_offset,
                                           session_data_ptr->data_length,
                                           http_header_content_length_offset,
                                           http_header_content_length_len);

                                /*-------- DEBUG LOG --------*/
                                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                                {
                                    boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                                            "handle_client_send() : call find_http_header_content_length : "
                                                            "return_value = %d. thread id : %d.");
                                    formatter % static_cast<int>(find_ret) % boost::this_thread::get_id();
                                    putLogDebug(600131, formatter.str(), __FILE__, __LINE__ );
                                }
                                /*------DEBUG LOG END------*/

                                //search Content_Length result is OK
                                if (find_ret)
                                {
                                    content_length.assign(session_data_ptr->data_buffer+session_data_ptr->data_offset + http_header_content_length_offset,
                                                          http_header_content_length_len );
                                    find_ret = regex_search(content_length.c_str(), regex_ret, content_length_regex);

                                    //"content-length: ddd\r\n"
                                    if ( find_ret)
                                    {
                                        content_length = content_length.substr(
                                                             regex_ret.position(1),
                                                             regex_ret.length(1));

                                        //set content length value
                                        content_length_value = boost::lexical_cast<int>(content_length);
                                    }

                                    //http_header context is "\r\n\r\n" only
                                    if (http_header_all_len == 0)
                                    {
                                        //set current message rest size
                                        session_data_ptr->current_message_rest_size = http_header_all_offset + http_header_all_len + content_length_value + CR_LF_LEN;
                                    }
                                    else
                                    {
                                        //set current message rest size
                                        session_data_ptr->current_message_rest_size = http_header_all_offset + http_header_all_len + content_length_value + CR_LF_CR_LF_LEN;
                                    }
                                }
                                //search Content_Length result is OK
                                else
                                {
                                    //http_header context is "\r\n\r\n" only
                                    if (http_header_all_len == 0)
                                    {
                                        //set current message rest size
                                        session_data_ptr->current_message_rest_size = http_header_all_offset + http_header_all_len + CR_LF_LEN;
                                    }
                                    else
                                    {
                                        //set current message rest size
                                        session_data_ptr->current_message_rest_size = http_header_all_offset + http_header_all_len + CR_LF_CR_LF_LEN;
                                    }

                                }

                                //set data state HTTP_HEADER
                                session_data_ptr->data_state = HTTP_HEADER;
                            }

                            //set return status
                            status = CLIENT_CONNECTION_CHECK;
                        }
                    }
                    //data state is UNKNOWN
                    else if (session_data_ptr->data_state == UNKNOWN)
                    {
                        //set return status
                        status = CLIENT_CONNECTION_CHECK;
                    }
                }
                //data size is 0
                else
                {
                    //data state is HTTP_BODY
                    if (session_data_ptr->data_state == HTTP_BODY)
                    {
                        //set data state HTTP_START
                        session_data_ptr->data_state = HTTP_START;
                    }

                    //set data offset 0
                    session_data_ptr->data_offset = 0;

                    //set return status
                    status = REALSERVER_RECV;
                }
            }

            //switch flag is on and status is REALSERVER_RECV
            if (session_data_ptr->switch_flag == SWITCH_FLAG_ON
                    && status == REALSERVER_RECV)
            {
                //set return status
                status = CLIENT_DISCONNECT;
            }
            //sorry flag is on and and status is REALSERVER_RECV
            else if (session_data_ptr->sorry_flag == SORRY_FLAG_ON
                     && status == REALSERVER_RECV)
            {
                //set return status
                status = SORRYSERVER_RECV;
            }
            else
            {
                //none
            }

            //set last status
            session_data_ptr->last_status = status;
        }
        catch (int e)
        {
            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                        "handle_client_send() : catch exception e = %d. thread id : %d.");
                formatter % e % boost::this_thread::get_id();
                putLogDebug(600132, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/

            //set last status
            status = FINALIZE;
        }
        catch (const boost::bad_lexical_cast& )
        {
            std::cerr << "protocol_module_ip::handle_client_send() : exception : " <<  "Content_Length field's value is invalid." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::handle_client_send() : "
                                    "Content_Length field's value is invalid. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(600084, formatter.str(), __FILE__, __LINE__ );

            //set last status
            status = FINALIZE;
        }
        catch (const std::exception& ex)
        {
            std::cerr << "protocol_module_ip::handle_client_send() : exception : error = " << ex.what() << "." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_client_send() : exception : error = %s. thread id : %d.");
            formatter % ex.what() % boost::this_thread::get_id();
            putLogError(600085, formatter.str(), __FILE__, __LINE__ );

            //set last status
            status = FINALIZE;
        }
        catch (...)
        {
            std::cerr << "protocol_module_ip::handle_client_send() : Unknown exception." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG "
                                    "protocol_module_ip::handle_client_send() : "
                                    "Unknown exception. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(600086, formatter.str(), __FILE__, __LINE__ );

            //set last status
            status = FINALIZE;
        }

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_client_send(const boost::thread::id thread_id) : return_value = %d. thread id : %d.");
            formatter % status % boost::this_thread::get_id();
            putLogDebug(600133, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        return status;
    }

    //! call from client disconnect event. use upstream thread and downstream thread.
    //! @param[in]    upstream and downstream thread id( check! one thread one event! )
    //! @return     session use EVENT mode
    protocol_module_base::EVENT_TAG protocol_module_ip::handle_client_disconnect(
        const boost::thread::id thread_id)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("in/out_function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_client_disconnect(const boost::thread::id thread_id) : return_value = %d. thread id : %d.");
            formatter % FINALIZE % boost::this_thread::get_id();
            putLogDebug(600134, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/
        return FINALIZE;
    }

    //! call from sorry mode event. use upstream thread and downstream thread
    //! @param[in]    upstream and downstream thread id( check! one thread one event and first time call pattern )
    //! @return     session use EVENT mode
    protocol_module_base::EVENT_TAG protocol_module_ip::handle_sorry_enable(
        const boost::thread::id thread_id)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_sorry_enable(const boost::thread::id thread_id) : thread_id = %d.");
            formatter % boost::this_thread::get_id();
            putLogDebug(600135, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        EVENT_TAG status = FINALIZE;
        thread_data_ptr session_data_ptr;

        try
        {
            {
                boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);

                session_thread_data_map_it session_thread_it = session_thread_data_map.find(thread_id);
                if (unlikely(session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL))
                {
                    boost::format formatter("Invalid thread id. thread id : %d.");
                    formatter % boost::this_thread::get_id();
                    putLogError(600087, formatter.str(), __FILE__, __LINE__ );
                    throw -1;
                }

                session_data_ptr = session_thread_it->second;
            }


            //up thread
            if (session_data_ptr->thread_division == THREAD_DIVISION_UP_STREAM)
            {
                //accept_end_flag is off
                if (session_data_ptr->accept_end_flag == ACCEPT_END_FLAG_OFF)
                {
                    //set return status
                    status = ACCEPT;
                }
                //accept_end_flag is on
                else
                {
                    //sorry flag is on
                    if (session_data_ptr->sorry_flag == SORRY_FLAG_ON)
                    {
                        //data state is HTTP_START or HTTP_HEADER
                        if (session_data_ptr->data_state == HTTP_START
                                || session_data_ptr->data_state ==HTTP_HEADER)
                        {
                            //set switch flag on
                            session_data_ptr->switch_flag = SWITCH_FLAG_ON;

                            /*-------- DEBUG LOG --------*/
                            if (unlikely(LOG_LV_DEBUG == getloglevel()))
                            {
                                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                                        "handle_sorry_enable(const boost::thread::id thread_id) : SWITCH_FLAG_ON. thread id : %d.");
                                formatter % boost::this_thread::get_id();
                                putLogDebug(600136, formatter.str(), __FILE__, __LINE__ );
                            }
                            /*------DEBUG LOG END------*/

                            //set return status
                            status = SORRYSERVER_DISCONNECT;
                        }
                        //data state is HTTP_BODY or UNKNOWN
                        else
                        {
                            //set return status
                            status = session_data_ptr->last_status;
                        }
                    }
                    //sorry flag is off
                    else
                    {
                        //data state is HTTP_START or HTTP_HEADER
                        if (session_data_ptr->data_state == HTTP_START
                                || session_data_ptr->data_state ==HTTP_HEADER)
                        {
                            //set switch flag on
                            session_data_ptr->switch_flag = SWITCH_FLAG_ON;

                            /*-------- DEBUG LOG --------*/
                            if (unlikely(LOG_LV_DEBUG == getloglevel()))
                            {
                                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                                        "handle_sorry_enable(const boost::thread::id thread_id) : SWITCH_FLAG_ON. thread id : %d.");
                                formatter % boost::this_thread::get_id();
                                putLogDebug(600137, formatter.str(), __FILE__, __LINE__ );
                            }
                            /*------DEBUG LOG END------*/
                        }
                        //data state is HTTP_BODY or UNKNOWN
                        else
                        {
                            //set ebd flag on
                            session_data_ptr->end_flag = END_FLAG_ON;

                            /*-------- DEBUG LOG --------*/
                            if (unlikely(LOG_LV_DEBUG == getloglevel()))
                            {
                                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                                        "handle_sorry_enable(const boost::thread::id thread_id) : END_FLAG_ON. thread id : %d.");
                                formatter % boost::this_thread::get_id();
                                putLogDebug(600138, formatter.str(), __FILE__, __LINE__ );
                            }
                            /*------DEBUG LOG END------*/
                        }

                        //set return status
                        status = REALSERVER_DISCONNECT;
                    }
                }
            }
            //down thread
            else
            {
                //sorry flag is on
                if (session_data_ptr->sorry_flag == SORRY_FLAG_ON)
                {
                    //set return status
                    status = session_data_ptr->last_status;
                }
                //sorry flag is off
                else
                {
                    //data state is HTTP_START and data size is 0
                    if (session_data_ptr->data_state == HTTP_START
                            && session_data_ptr->data_length == 0)
                    {
                        //set return status
                        status = SORRYSERVER_RECV;
                    }
                    //data state is HTTP_START and data size > 0
                    else if (session_data_ptr->data_state == HTTP_START
                             && session_data_ptr->data_length > 0)
                    {
                        //set return status
                        status = REALSERVER_DISCONNECT;
                    }
                    //data state is HTTP_HEADER or HTTP_BODY
                    else if (session_data_ptr->data_state == HTTP_HEADER
                             || session_data_ptr->data_state == HTTP_BODY)
                    {
                        //set switch flag on
                        session_data_ptr->switch_flag = SWITCH_FLAG_ON;

                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                                    "handle_sorry_enable(const boost::thread::id thread_id) : SWITCH_FLAG_ON. thread id : %d.");
                            formatter % boost::this_thread::get_id();
                            putLogDebug(600139, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/

                        //set return status
                        status = session_data_ptr->last_status;
                    }
                    //other
                    else
                    {
                        //set return status
                        status = REALSERVER_DISCONNECT;
                    }
                }
            }

            //set sorry flag on
            session_data_ptr->sorry_flag = SORRY_FLAG_ON;

            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                        "handle_sorry_enable(const boost::thread::id thread_id) : SORRY_FLAG_ON. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogDebug(600140, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/

            //set last status
            session_data_ptr->last_status = status;

        }
        catch (int e)
        {
            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                        "handle_sorry_enable() : catch exception e = %d. thread id : %d.");
                formatter % e % boost::this_thread::get_id();
                putLogDebug(600141, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/

            //set return status
            status = FINALIZE;
        }
        catch (std::exception& ex)
        {
            std::cerr << "protocol_module_ip::handle_sorry_enable() : exception : error = " << ex.what() << "." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_sorry_enable() : exception : error = %s. thread id : %d.");
            formatter % ex.what() % boost::this_thread::get_id();
            putLogError(600088, formatter.str(), __FILE__, __LINE__ );

            //set return status
            status = FINALIZE;
        }
        catch (...)
        {
            std::cerr << "protocol_module_ip::handle_sorry_enable() : Unknown exception." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG "
                                    "protocol_module_ip::handle_sorry_enable() : "
                                    "Unknown exception. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(600089, formatter.str(), __FILE__, __LINE__ );

            //set return status
            status = FINALIZE;
        }

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_sorry_enable(const boost::thread::id thread_id) : return_value = %d. thread id : %d.");
            formatter % status % boost::this_thread::get_id();
            putLogDebug(600142, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        return status;
    }

    //! call from sorry mode disable. use upstream thread and downstream thread.
    //! @param[in]    upstream and downstream thread id( check! one thread one event )
    //! @return     session use EVENT mode
    protocol_module_base::EVENT_TAG protocol_module_ip::handle_sorry_disable(
        const boost::thread::id thread_id)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_sorry_disable(const boost::thread::id thread_id) : thread_id = %d.");
            formatter % boost::this_thread::get_id();
            putLogDebug(600143, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        EVENT_TAG status = FINALIZE;
        thread_data_ptr session_data_ptr;

        try
        {
            {
                boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);

                session_thread_data_map_it session_thread_it = session_thread_data_map.find(thread_id);
                if (unlikely(session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL))
                {
                    boost::format formatter("Invalid thread id. thread id : %d.");
                    formatter % boost::this_thread::get_id();
                    putLogError(600090, formatter.str(), __FILE__, __LINE__ );
                    throw -1;
                }

                session_data_ptr = session_thread_it->second;
            }

            //up thread
            if (session_data_ptr->thread_division == THREAD_DIVISION_UP_STREAM)
            {
                //accept_end_flag is off
                if (session_data_ptr->accept_end_flag == ACCEPT_END_FLAG_OFF)
                {
                    //set return status
                    status =ACCEPT;
                }
                //accept_end_flag is on
                else
                {
                    //sorry flag is on
                    if (session_data_ptr->sorry_flag == SORRY_FLAG_ON)
                    {
                        ///data state is HTTP_START or HTTP_HEADER
                        if (session_data_ptr->data_state == HTTP_START
                                || session_data_ptr->data_state ==HTTP_HEADER)
                        {
                            //set switch flag on
                            session_data_ptr->switch_flag = SWITCH_FLAG_ON;

                            /*-------- DEBUG LOG --------*/
                            if (unlikely(LOG_LV_DEBUG == getloglevel()))
                            {
                                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                                        "handle_sorry_disable(const boost::thread::id thread_id) : SWITCH_FLAG_ON. thread id : %d.");
                                formatter % boost::this_thread::get_id();
                                putLogDebug(600144, formatter.str(), __FILE__, __LINE__ );
                            }
                            /*------DEBUG LOG END------*/

                        }
                        //data state is HTTP_BODY or UNKNOWN
                        else
                        {
                            //set end flag on
                            session_data_ptr->end_flag = END_FLAG_ON;

                            /*-------- DEBUG LOG --------*/
                            if (unlikely(LOG_LV_DEBUG == getloglevel()))
                            {
                                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                                        "handle_sorry_disable(const boost::thread::id thread_id) : END_FLAG_ON. thread id : %d.");
                                formatter % boost::this_thread::get_id();
                                putLogDebug(600145, formatter.str(), __FILE__, __LINE__ );
                            }
                            /*------DEBUG LOG END------*/
                        }

                        //set return status
                        status = SORRYSERVER_DISCONNECT;

                    }
                    //sorry flag is off
                    else
                    {
                        //data state is HTTP_START or HTTP_HEADER
                        if (session_data_ptr->data_state == HTTP_START
                                || session_data_ptr->data_state ==HTTP_HEADER)
                        {
                            //set switch flag on
                            session_data_ptr->switch_flag = SWITCH_FLAG_ON;

                            /*-------- DEBUG LOG --------*/
                            if (unlikely(LOG_LV_DEBUG == getloglevel()))
                            {
                                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                                        "handle_sorry_disable(const boost::thread::id thread_id) : SWITCH_FLAG_ON. thread id : %d.");
                                formatter % boost::this_thread::get_id();
                                putLogDebug(600146, formatter.str(), __FILE__, __LINE__ );
                            }
                            /*------DEBUG LOG END------*/

                            //set return status
                            status = REALSERVER_DISCONNECT;
                        }
                        //data state is HTTP_BODY or UNKNOWN
                        else
                        {
                            //set return status
                            status = session_data_ptr->last_status;
                        }
                    }
                }
            }
            //down thread
            else
            {
                //sorry flag is off
                if (session_data_ptr->sorry_flag == SORRY_FLAG_OFF)
                {
                    //set return status
                    status = session_data_ptr->last_status;
                }
                //sorry flag is on
                else
                {
                    //data state is HTTP_START and data size is 0
                    if (session_data_ptr->data_state == HTTP_START
                            && session_data_ptr->data_length == 0)
                    {
                        //set return status
                        status = REALSERVER_RECV;
                    }
                    //data state is HTTP_START and data size > 0
                    else if (session_data_ptr->data_state == HTTP_START
                             && session_data_ptr->data_length > 0)
                    {
                        //set return status
                        status = SORRYSERVER_DISCONNECT;
                    }
                    //data state is HTTP_HEADER or HTTP_BODY
                    else if (session_data_ptr->data_state == HTTP_HEADER
                             || session_data_ptr->data_state == HTTP_BODY)
                    {
                        //set switch flag on
                        session_data_ptr->switch_flag = SWITCH_FLAG_ON;

                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                                    "handle_sorry_disable(const boost::thread::id thread_id) : SWITCH_FLAG_ON. thread id : %d.");
                            formatter % boost::this_thread::get_id();
                            putLogDebug(600147, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/

                        //set return status
                        status = session_data_ptr->last_status;
                    }
                    //other
                    else
                    {
                        //set return status
                        status = SORRYSERVER_DISCONNECT;
                    }
                }
            }

            //set sorry flag off
            session_data_ptr->sorry_flag = SORRY_FLAG_OFF;

            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                        "handle_sorry_disable(const boost::thread::id thread_id) : SORRY_FLAG_OFF. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogDebug(600148, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/

            //set last status
            session_data_ptr->last_status = status;
        }
        catch (int e)
        {
            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                        "handle_sorry_disable() : catch exception e = %d. thread id : %d.");
                formatter % e % boost::this_thread::get_id();
                putLogDebug(600149, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/

            //set last status
            status = FINALIZE;
        }
        catch (std::exception& ex)
        {
            std::cerr << "protocol_module_ip::handle_sorry_disable() : exception : error = " << ex.what() << "." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_sorry_disable() : exception : error = %s. thread id : %d.");
            formatter % ex.what() % boost::this_thread::get_id();
            putLogError(600091, formatter.str(), __FILE__, __LINE__ );

            //set last status
            status = FINALIZE;
        }
        catch (...)
        {
            std::cerr << "protocol_module_ip::handle_sorry_disable() : Unknown exception." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG "
                                    "protocol_module_ip::handle_sorry_disable() : "
                                    "Unknown exception. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(600092, formatter.str(), __FILE__, __LINE__ );

            //set last status
            status = FINALIZE;
        }

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_sorry_disable(const boost::thread::id thread_id) : return_value = %d. thread id : %d.");
            formatter % status % boost::this_thread::get_id();
            putLogDebug(600150, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        return status;
    }

    //! call from realserver disconnect. use upstream thread and downstream thread
    //! @param[in]    upstream and downstream thread id( check! one thread one event )
    //! @param[in]    disconnected realserver endpoint.
    //! @return     session use EVENT mode
    protocol_module_base::EVENT_TAG protocol_module_ip::handle_realserver_disconnect(
        const boost::thread::id thread_id, const boost::asio::ip::tcp::endpoint & rs_endpoint)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_realserver_disconnect(const boost::thread::id thread_id, const boost::asio::ip::tcp::endpoint& rs_endpoint) : "
                                    "thread_id = %d, rs_endpoint = [%s]:%d.");
            formatter % thread_id % rs_endpoint.address().to_string() % rs_endpoint.port();
            putLogDebug(600151, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        EVENT_TAG status = FINALIZE;
        thread_data_ptr session_data_ptr;

        try
        {
            {
                boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);

                session_thread_data_map_it session_thread_it = session_thread_data_map.find(thread_id);
                if (unlikely(session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL))
                {
                    boost::format formatter("Invalid thread id. thread id : %d.");
                    formatter % boost::this_thread::get_id();
                    putLogError(600093, formatter.str(), __FILE__, __LINE__ );
                    throw -1;
                }

                session_data_ptr = session_thread_it->second;
            }


            //up thread
            if (session_data_ptr->thread_division == THREAD_DIVISION_UP_STREAM)
            {
                //end flag is on
                if (session_data_ptr->end_flag == END_FLAG_ON)
                {
                    //set return status
                    status = CLIENT_RECV;
                }
                //end flag is off
                else
                {
                    //switch flag is on
                    if (session_data_ptr->switch_flag == SWITCH_FLAG_ON)
                    {
                        //sorry flag is on
                        if (session_data_ptr->sorry_flag == SORRY_FLAG_ON)
                        {
                            //set return status
                            status = SORRYSERVER_SELECT;
                        }
                        //sorry flag is off
                        else
                        {
                            //set return status
                            status = REALSERVER_SELECT;
                        }

                        //set switch flag off
                        session_data_ptr->switch_flag = SWITCH_FLAG_OFF;

                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                                    "handle_realserver_disconnect(const boost::thread::id thread_id, const boost::asio::ip::tcp::endpoint &"                         "rs_endpoint) : SWITCH_FLAG_OFF. thread id : %d.");
                            formatter % boost::this_thread::get_id();
                            putLogDebug(600152, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                    }
                    //switch flag is off
                    else
                    {
                        //set return status
                        status = CLIENT_RECV;
                    }
                }
            }
            //down thread
            else
            {
                //set return status
                status = CLIENT_DISCONNECT;
            }

            //set last status
            session_data_ptr->last_status = status;
        }
        catch (int e)
        {
            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                        "handle_realserver_disconnect() : catch exception e = %d. thread id : %d.");
                formatter % e % boost::this_thread::get_id();
                putLogDebug(600153, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/

            //set return status
            status = FINALIZE;
        }
        catch (std::exception& ex)
        {
            std::cerr << "protocol_module_ip::handle_realserver_disconnect() : exception : error = " << ex.what() << "." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_realserver_disconnect() : exception : error = %s. thread id : %d.");
            formatter % ex.what() % boost::this_thread::get_id();
            putLogError(600094, formatter.str(), __FILE__, __LINE__ );

            //set return status
            status = FINALIZE;
        }
        catch (...)
        {
            std::cerr << "protocol_module_ip::handle_realserver_disconnect() : Unknown exception." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG "
                                    "protocol_module_ip::handle_realserver_disconnect() : "
                                    "Unknown exception. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(600095, formatter.str(), __FILE__, __LINE__ );

            //set return status
            status = FINALIZE;
        }

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_realserver_disconnect(const boost::thread::id thread_id, "
                                    "const boost::asio::ip::tcp::endpoint& rs_endpoint) : return_value = %d. thread id : %d.");
            formatter % status % boost::this_thread::get_id();
            putLogDebug(600154, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        return status;
    }

    //! call from sorry server disconnect. use upstraem thread and downstream thread
    //! @param[in]    upstream and downstream thread id( check! one thread one event )
    //! @param[in]    disconnect sorryserver endpoint
    //! @return        session use EVENT mode
    //! @return        session use EVENT mode
    protocol_module_base::EVENT_TAG protocol_module_ip::handle_sorryserver_disconnect(
        const boost::thread::id thread_id, const boost::asio::ip::tcp::endpoint & sorry_endpoint)
    {

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_sorryserver_disconnect(const boost::thread::id thread_id, const boost::asio::ip::tcp::endpoint& sorry_endpoint) : "
                                    "thread_id = %d, sorry_endpoint = [%s]:%d.");
            formatter % thread_id % sorry_endpoint.address().to_string() % sorry_endpoint.port();
            putLogDebug(600155, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        EVENT_TAG status = FINALIZE;
        thread_data_ptr session_data_ptr;

        try
        {
            {
                boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);

                session_thread_data_map_it session_thread_it = session_thread_data_map.find(thread_id);
                if (unlikely(session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL))
                {
                    boost::format formatter("Invalid thread id. thread id : %d.");
                    formatter % boost::this_thread::get_id();
                    putLogError(600096, formatter.str(), __FILE__, __LINE__ );
                    throw -1;
                }

                session_data_ptr = session_thread_it->second;
            }

            //up thread
            if (session_data_ptr->thread_division == THREAD_DIVISION_UP_STREAM)
            {
                //end flag is on
                if (session_data_ptr->end_flag == END_FLAG_ON)
                {
                    //set return status
                    status = CLIENT_RECV;
                }
                //end flag is off
                else
                {
                    //switch flag is on
                    if (session_data_ptr->switch_flag == SWITCH_FLAG_ON)
                    {
                        //sorry flag is on
                        if (session_data_ptr->sorry_flag == SORRY_FLAG_ON)
                        {
                            //set return status
                            status = SORRYSERVER_SELECT;
                        }
                        //sorry flag is off
                        else
                        {
                            //set return status
                            status = REALSERVER_SELECT;
                        }

                        //set switch flag off
                        session_data_ptr->switch_flag = SWITCH_FLAG_OFF;

                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                                    "handle_sorryserver_disconnect(const boost::thread::id thread_id, const boost::asio::ip::tcp::endpoint &"                         "sorry_endpoint) : SWITCH_FLAG_OFF. thread id : %d.");
                            formatter % boost::this_thread::get_id();
                            putLogDebug(600156, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/

                    }
                    //switch flag is off
                    else
                    {
                        //set return status
                        status = CLIENT_RECV;
                    }
                }
            }
            //down thread
            else
            {
                //set return status
                status = CLIENT_DISCONNECT;
            }

            //set last status
            session_data_ptr->last_status = status;

        }
        catch (int e)
        {
            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                        "handle_sorryserver_disconnect() : catch exception e = %d. thread id : %d.");
                formatter % e % boost::this_thread::get_id();
                putLogDebug(600157, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/

            //set return status
            status = FINALIZE;
        }

        catch (std::exception& ex)
        {
            std::cerr << "protocol_module_ip::handle_sorryserver_disconnect() : exception : error = " << ex.what() << "." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_sorryserver_disconnect() : exception : error = %s. thread id : %d.");
            formatter % ex.what() % boost::this_thread::get_id();
            putLogError(600097, formatter.str(), __FILE__, __LINE__ );

            //set return status
            status = FINALIZE;
        }
        catch (...)
        {
            std::cerr << "protocol_module_ip::handle_sorryserver_disconnect() : Unknown exception." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG "
                                    "protocol_module_ip::handle_sorryserver_disconnect() : "
                                    "Unknown exception. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(600098, formatter.str(), __FILE__, __LINE__ );

            //set return status
            status = FINALIZE;
        }

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_sorryserver_disconnect(const boost::thread::id thread_id, "
                                    "const boost::asio::ip::tcp::endpoint& sorry_endpoint) : return_value = %d. thread id : %d.");
            formatter % status % boost::this_thread::get_id();
            putLogDebug(600158, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        return status;
    }

    //! call from realserver disconnect. use upstream thread and downstream thread.
    //! @param[in]    upstream and downstream thread id( check! one thread one event )
    //! @param[in]    disconnect realserver endpoint
    //! @return        session use EVENT mode.
    protocol_module_base::EVENT_TAG protocol_module_ip::handle_realserver_close(
        const boost::thread::id thread_id, const boost::asio::ip::udp::endpoint & rs_endpoint)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("in/out_function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                                    "handle_realserver_close(const boost::thread::id thread_id, "
                                    "boost::asio::ip::udp::endpoint& rs_endpoint) : "
                                    "return_value = %d. thread id : %d.");
            formatter % STOP % boost::this_thread::get_id();
            putLogDebug(600159, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        return STOP;
    }


    bool  protocol_module_ip::get_data_from_recvbuffer(
        thread_data_ptr data_ptr, const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, const size_t recvlen)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("in_function : bool protocol_module_ip::get_data_from_recvbuffer("
                                    "thread_data_ptr data_ptr, const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                    "const size_t& recvlen) : thread_id = %d.");
            formatter % boost::this_thread::get_id();
            putLogDebug(600160, formatter.str(), __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/

        char* tmpbuffer = NULL;

        //pointer volidate check
        if (unlikely(data_ptr == NULL || data_ptr->data_buffer == NULL))
        {
            boost::format formatter("Invalid pointer. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(600099, formatter.str(), __FILE__, __LINE__ );

            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("out_function : bool protocol_module_ip::get_data_from_recvbuffer("
                                        "thread_data_ptr data_ptr, const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                        "const size_t& recvlen) : return_value = false. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogDebug(600161, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/

            return false;
        }


        //the new data can append to buffer directly
        if (data_ptr->data_buffer_size - data_ptr->data_length - data_ptr->data_offset >= recvlen)
        {

            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                std::string datadump;
                dump_memory(recvbuffer.data(), recvlen, datadump);
                boost::format formatter(
                    "function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                    "get_data_from_recvbuffer() : before memcpy (data dump) : "
                    "data begin = 0, data_size = %d, data = %s");
                formatter % recvlen % datadump;
                putLogDebug(600162, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/

            memcpy(data_ptr->data_buffer + data_ptr->data_offset + data_ptr->data_length, recvbuffer.data(), recvlen);

            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                std::string datadump;
                dump_memory(data_ptr->data_buffer + data_ptr->data_offset + data_ptr->data_length, recvlen, datadump);
                boost::format formatter(
                    "function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                    "get_data_from_recvbuffer() : after memcpy (data dump) : "
                    "data begin = 0, data_size = %d, data = %s");
                formatter % recvlen % datadump;
                putLogDebug(600163, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/

            data_ptr->data_length += recvlen;

            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("out_function : bool protocol_module_ip::get_data_from_recvbuffer("
                                        "thread_data_ptr data_ptr, const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                        "const size_t& recvlen) : return_value = true. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogDebug(600164, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/
            return true;
        }
        else
        {
            //the new data can append to buffer through moving orignal data
            if (data_ptr->data_buffer_size - data_ptr->data_length >= recvlen)
            {
                memmove(data_ptr->data_buffer, data_ptr->data_buffer + data_ptr->data_offset, data_ptr->data_length);
                /*-------- DEBUG LOG --------*/
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    std::string datadump;
                    dump_memory(recvbuffer.data(), recvlen, datadump);
                    boost::format formatter(
                        "function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                        "get_data_from_recvbuffer() : before memcpy (data dump) : "
                        "data begin = 0, data_size = %d, data = %s");
                    formatter % recvlen % datadump;
                    putLogDebug(600165, formatter.str(), __FILE__, __LINE__ );
                }
                /*------DEBUG LOG END------*/

                memcpy(data_ptr->data_buffer + data_ptr->data_length, recvbuffer.data(), recvlen);

                /*-------- DEBUG LOG --------*/
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    std::string datadump;
                    dump_memory(data_ptr->data_buffer + data_ptr->data_length, recvlen, datadump);
                    boost::format formatter(
                        "function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                        "get_data_from_recvbuffer() : after memcpy (data dump) : "
                        "data begin = 0, data_size = %d, data = %s");
                    formatter % recvlen % datadump;
                    putLogDebug(600166, formatter.str(), __FILE__, __LINE__ );
                }
                /*------DEBUG LOG END------*/

                data_ptr->data_offset = 0;
                data_ptr->data_length += recvlen;

                /*-------- DEBUG LOG --------*/
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    boost::format formatter("out_function : bool protocol_module_ip::get_data_from_recvbuffer("
                                            "thread_data_ptr data_ptr, const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                            "const size_t& recvlen) : return_value = true. thread id : %d.");
                    formatter % boost::this_thread::get_id();
                    putLogDebug(600167, formatter.str(), __FILE__, __LINE__ );
                }
                /*------DEBUG LOG END------*/

                return true;
            }
            else
            {
                //not allowed to realloc new buffer who's size is larger than the max size
                if (data_ptr->data_buffer_size >= MAX_IP_MODULE_BUFFER_SIZE
                        || data_ptr->data_buffer_size + recvlen >= MAX_IP_MODULE_BUFFER_SIZE)
                {
                    boost::format formatter("Buffer size is bigger than Max size. thread id : %d.");
                    formatter % boost::this_thread::get_id();
                    putLogError(600100, formatter.str(), __FILE__, __LINE__ );

                    /*-------- DEBUG LOG --------*/
                    if (unlikely(LOG_LV_DEBUG == getloglevel()))
                    {
                        boost::format formatter("out_function : bool protocol_module_ip::get_data_from_recvbuffer("
                                                "thread_data_ptr data_ptr, const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                                "const size_t& recvlen) : return_value = false. thread id : %d.");
                        formatter % boost::this_thread::get_id();
                        putLogDebug(600168, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/

                    return false;
                }
                else
                {
                    try
                    {
                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            boost::format formatter("new : address = &(%d), size = %lu.");
                            formatter % static_cast<void*>(tmpbuffer) % MAX_IP_MODULE_BUFFER_SIZE;
                            putLogDebug(600169, formatter.str(), __FILE__, __LINE__);
                        }
                        /*------DEBUG LOG END------*/

                        //alloc a new buffer who's size is max
                        tmpbuffer = new char[MAX_IP_MODULE_BUFFER_SIZE];
                    }
                    catch (const std::bad_alloc&) //memory alloc fail
                    {
                        std::cerr << "protocol_module_ip::get_data_from_recvbuffer() : exception : Could not allocate memory." << std::endl;
                        boost::format formatter("Could not allocate memory. thread id : %d.");
                        formatter % boost::this_thread::get_id();
                        putLogError(600101, formatter.str(), __FILE__, __LINE__);

                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            boost::format formatter("out_function : bool protocol_module_ip::get_data_from_recvbuffer("
                                                    "thread_data_ptr data_ptr, const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                                    "const size_t& recvlen) : return_value = false. thread id : %d.");
                            formatter % boost::this_thread::get_id();
                            putLogDebug(600170, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/

                        return false;
                    }

                    memset(tmpbuffer, 0, MAX_IP_MODULE_BUFFER_SIZE);

                    /*-------- DEBUG LOG --------*/
                    if (unlikely(LOG_LV_DEBUG == getloglevel()))
                    {
                        std::string datadump;
                        dump_memory(data_ptr->data_buffer+data_ptr->data_offset, data_ptr->data_length, datadump);
                        boost::format formatter(
                            "function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                            "get_data_from_recvbuffer() : before memcpy (data dump) : "
                            "data begin = 0, data_size = %d, data = %s");
                        formatter % data_ptr->data_length % datadump;
                        putLogDebug(600171, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/

                    //copy old data to new buffer
                    memcpy(tmpbuffer, data_ptr->data_buffer+data_ptr->data_offset, data_ptr->data_length);

                    /*-------- DEBUG LOG --------*/
                    if (unlikely(LOG_LV_DEBUG == getloglevel()))
                    {
                        std::string datadump;
                        dump_memory(tmpbuffer, data_ptr->data_length, datadump);
                        boost::format formatter(
                            "function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                            "get_data_from_recvbuffer() : after memcpy (data dump) : "
                            "data begin = 0, data_size = %d, data = %s");
                        formatter % data_ptr->data_length % datadump;
                        putLogDebug(600172, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/

                    /*-------- DEBUG LOG --------*/
                    if (unlikely(LOG_LV_DEBUG == getloglevel()))
                    {
                        boost::format formatter("delete : address = &(%d).");
                        formatter % static_cast<void*>(data_ptr->data_buffer);
                        putLogDebug(600173, formatter.str(), __FILE__, __LINE__);
                    }
                    /*------DEBUG LOG END------*/

                    //release old memory
                    delete [] data_ptr->data_buffer;
                    data_ptr->data_buffer = tmpbuffer;
                    data_ptr->data_offset = 0;

                    /*-------- DEBUG LOG --------*/
                    if (unlikely(LOG_LV_DEBUG == getloglevel()))
                    {
                        std::string datadump;
                        dump_memory(recvbuffer.data(), recvlen, datadump);
                        boost::format formatter(
                            "function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                            "get_data_from_recvbuffer() : before memcpy (data dump) : "
                            "data begin = 0, data_size = %d, data = %s");
                        formatter % recvlen % datadump;
                        putLogDebug(600174, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/

                    //append new data
                    memcpy(data_ptr->data_buffer+data_ptr->data_length, recvbuffer.data(), recvlen);

                    /*-------- DEBUG LOG --------*/
                    if (unlikely(LOG_LV_DEBUG == getloglevel()))
                    {
                        std::string datadump;
                        dump_memory(data_ptr->data_buffer+data_ptr->data_length, recvlen, datadump);
                        boost::format formatter(
                            "function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                            "get_data_from_recvbuffer() : after memcpy (data dump) : "
                            "data begin = 0, data_size = %d, data = %s");
                        formatter % recvlen % datadump;
                        putLogDebug(600175, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/

                    data_ptr->data_length += recvlen;
                    data_ptr->data_buffer_size = MAX_IP_MODULE_BUFFER_SIZE ;

                    /*-------- DEBUG LOG --------*/
                    if (unlikely(LOG_LV_DEBUG == getloglevel()))
                    {
                        boost::format formatter("out_function : bool protocol_module_ip::get_data_from_recvbuffer("
                                                "thread_data_ptr data_ptr, const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                                "const size_t& recvlen) : return_value = true. thread id : %d.");
                        formatter % boost::this_thread::get_id();
                        putLogDebug(600176, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/

                    return true;

                }
            }
        }
    }

    bool protocol_module_ip::put_data_into_sendbuffer(
        thread_data_ptr data_ptr, boost::array<char, MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("in_function : bool protocol_module_ip::put_data_to_sendbuffer("
                                    "thread_data_ptr data_ptr, boost::array<char,MAX_BUFFER_SIZE>& sendbuffer, "
                                    "size_t& datalen) : thread_id = %d.");
            formatter % boost::this_thread::get_id();
            putLogDebug(600177, formatter.str(), __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/

        size_t sendbuffer_rest_size = 0;
        size_t new_offset        = 0;

        if (unlikely(data_ptr == NULL || data_ptr->data_buffer == NULL || data_ptr->buffer_sequence.empty()))
        {
            boost::format formatter("Invalid pointer. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(600102, formatter.str(), __FILE__, __LINE__ );

            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("out_function : bool protocol_module_ip::put_data_to_sendbuffer("
                                        "thread_data_ptr data_ptr, boost::array<char,MAX_BUFFER_SIZE>& sendbuffer, "
                                        "size_t& datalen) : return_value = false. thread_id = %d.");
                formatter % boost::this_thread::get_id();
                putLogDebug(600178, formatter.str(), __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/

            return false;
        }

        sendbuffer_rest_size = sendbuffer.size();
        datalen = 0;

        //buffer_sequence loop
        //copy data to send buffer until send buffer is full
        while (data_ptr->buffer_sequence.size() > 0)
        {
            std::pair<char*, size_t> buffer_element = data_ptr->buffer_sequence.front();
            //sendbuffer rest size id bigger than copy possible size
            if (buffer_element.second <= sendbuffer_rest_size)
            {
                /*-------- DEBUG LOG --------*/
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    std::string datadump;
                    dump_memory(buffer_element.first, buffer_element.second, datadump);
                    boost::format formatter(
                        "function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                        "put_data_to_sendbuffer() : before memcpy (data dump) : "
                        "data begin = 0, data_size = %d, data = %s");
                    formatter % buffer_element.second % datadump;
                    putLogDebug(600179, formatter.str(), __FILE__, __LINE__ );
                }
                /*------DEBUG LOG END------*/

                //copy data to send buffer
                memcpy(sendbuffer.c_array() + datalen, buffer_element.first, buffer_element.second);

                /*-------- DEBUG LOG --------*/
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    std::string datadump;
                    dump_memory(sendbuffer.c_array() + datalen, buffer_element.second, datadump);
                    boost::format formatter(
                        "function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                        "put_data_to_sendbuffer() : after memcpy (data dump) : "
                        "data begin = 0, data_size = %d, data = %s");
                    formatter % buffer_element.second % datadump;
                    putLogDebug(600180, formatter.str(), __FILE__, __LINE__ );
                }
                /*------DEBUG LOG END------*/

                datalen += buffer_element.second;
                sendbuffer_rest_size -= buffer_element.second;

                //it is the last item of the sequence
                if (data_ptr->buffer_sequence.size() == 1)
                {
                    //set data position
                    new_offset = buffer_element.first + buffer_element.second - data_ptr->data_buffer;
                    data_ptr->current_message_rest_size -= (new_offset - data_ptr->data_offset);
                    data_ptr->data_length -= (new_offset - data_ptr->data_offset);
                    data_ptr->data_offset = new_offset;
                }


                //delete the item
                data_ptr->buffer_sequence.pop_front();
            }
            //sendbuffer rest size is too small
            else
            {
                /*-------- DEBUG LOG --------*/
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    std::string datadump;
                    dump_memory(buffer_element.first, sendbuffer_rest_size, datadump);
                    boost::format formatter(
                        "function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                        "put_data_to_sendbuffer() : before memcpy (data dump) : "
                        "data begin = 0, data_size = %d, data = %s");
                    formatter % sendbuffer_rest_size % datadump;
                    putLogDebug(600181, formatter.str(), __FILE__, __LINE__ );
                }
                /*------DEBUG LOG END------*/

                //copy data to send buffer
                memcpy(sendbuffer.c_array() + datalen, buffer_element.first, sendbuffer_rest_size);

                /*-------- DEBUG LOG --------*/
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    std::string datadump;
                    dump_memory(sendbuffer.c_array() + datalen, sendbuffer_rest_size, datadump);
                    boost::format formatter(
                        "function : protocol_module_base::EVENT_TAG protocol_module_ip::"
                        "put_data_to_sendbuffer() : after memcpy (data dump) : "
                        "data begin = 0, data_size = %d, data = %s");
                    formatter % sendbuffer_rest_size % datadump;
                    putLogDebug(600182, formatter.str(), __FILE__, __LINE__ );
                }
                /*------DEBUG LOG END------*/

                datalen += sendbuffer_rest_size;

                //set item position
                buffer_element.first += sendbuffer_rest_size;
                buffer_element.second -= sendbuffer_rest_size;
                sendbuffer_rest_size = 0;
                break;
            }
        }

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("out_function : bool protocol_module_ip::put_data_to_sendbuffer("
                                    "thread_data_ptr data_ptr, boost::array<char,MAX_BUFFER_SIZE>& sendbuffer, "
                                    "size_t& datalen) : return_value = true. thread_id = %d.");
            formatter % boost::this_thread::get_id();
            putLogDebug(600183, formatter.str(), __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/

        return true;
    }




    //! call from put_data_to_buffer_with_x_forwarded_for(). get x_forwarded_for header's offset and length
    //! if the buffer isn't include x_forwarded_for header, create a new one
    //! @param[in]    buffer
    //! @param[in]    buffer_len
    //! @param[out] x_forwarded_for_insert_pos
    //! @param[out] x_forwarded_for_context
    //! @return  true:create a new x_forwarded_for header
    //!         false: edit old one
    bool protocol_module_ip::create_x_forwarded_for(const std::string& client_endpoint,
            const char* buffer,
            const size_t buffer_len,
            size_t& x_forwarded_for_insert_pos,
            std::string& x_forwarded_for_context)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("in_function : bool protocol_module_ip::create_x_forwarded_for("
                                    "const std::string& client_endpoint, const char* buffer, const size_t buffer_len, "
                                    "size_t& x_forwarded_for_insert_pos, std::string& x_forwarded_for_context) : thread_id = %d.");
            formatter % boost::this_thread::get_id();
            putLogDebug(600184, formatter.str(), __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/

        bool find_ret = false;
        size_t x_forwarded_for_offset   = 0;
        size_t x_forwarded_for_len    = 0;
        size_t http_header_all_offset   = 0;
        size_t http_header_all_len    = 0;
        const char* X_FORWARDED_FOR    = "X-Forwarded-For";

        //search "X-Forwared-For" header
        find_ret = http_utility::find_http_header_x_forwarded_for(buffer, buffer_len,
                   x_forwarded_for_offset, x_forwarded_for_len);

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("function : bool protocol_module_ip::create_x_forwarded_for() : "
                                    "call find_http_header_x_forwarded_for : "
                                    "return_value = %d. thread id : %d.");
            formatter % static_cast<int>(find_ret) % boost::this_thread::get_id();
            putLogDebug(600185, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        //search "X-Forwared-For" result is OK
        if (find_ret)
        {
            //create "X-Forwared-For" string
            x_forwarded_for_insert_pos = x_forwarded_for_offset + x_forwarded_for_len;
            x_forwarded_for_context = ", ";
            x_forwarded_for_context += client_endpoint;

            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("out_function : bool protocol_module_ip::create_x_forwarded_for("
                                        "const std::string& client_endpoint, const char* buffer, const size_t buffer_len, "
                                        "size_t& x_forwarded_for_insert_pos, std::string& x_forwarded_for_context) : return_value = false. thread_id = %d.");
                formatter % boost::this_thread::get_id();
                putLogDebug(600186, formatter.str(), __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/

            return false;
        }

        //create a new one
        //because state is HTTP_HEADER, find_http_header_all must be return true
        http_utility::find_http_header_all(buffer, buffer_len, http_header_all_offset, http_header_all_len);

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("function : bool protocol_module_ip::create_x_forwarded_for() : "
                                    "call find_http_header_all : "
                                    "return_value = true. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogDebug(600187, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        //create "X-Forwared-For" string
        x_forwarded_for_insert_pos = http_header_all_offset;
        x_forwarded_for_context = X_FORWARDED_FOR;
        x_forwarded_for_context += ": ";
        x_forwarded_for_context += client_endpoint;
        x_forwarded_for_context += "\r\n";

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("out_function : bool protocol_module_ip::create_x_forwarded_for("
                                    "const std::string& client_endpoint, const char* buffer, const size_t buffer_len, "
                                    "size_t& x_forwarded_for_insert_pos, std::string& x_forwarded_for_context) : return_value = true. thread_id = %d.");
            formatter % boost::this_thread::get_id();
            putLogDebug(600188, formatter.str(), __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/

        return true;
    }



}//namesapce l7vsd

extern "C" l7vs::protocol_module_base*
    create_module()
{
    return dynamic_cast<l7vs::protocol_module_base*>(new l7vs::protocol_module_ip());
}

extern "C" void
    destroy_module( l7vs::protocol_module_base* in )
{
    delete in;
}
