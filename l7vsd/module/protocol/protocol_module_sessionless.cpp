/*
 * @file  protocol_module_sessionless.cpp
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
#include <boost/xpressive/xpressive.hpp>
#include <vector>
#include <list>
#include <algorithm>
#include <iostream>
#include <boost/asio/ip/tcp.hpp>
#include <boost/format.hpp>
#include "protocol_module_sessionless.h"
#include "utility.h"

namespace l7vs
{

    const std::string protocol_module_sessionless::MODULE_NAME = "sessionless";
    const int protocol_module_sessionless::THREAD_DIVISION_UP_STREAM = 0;
    const int protocol_module_sessionless::THREAD_DIVISION_DOWN_STREAM = 1;

    const int protocol_module_sessionless::END_FLAG_OFF = 0;
    const int protocol_module_sessionless::END_FLAG_ON = 1;

    const int protocol_module_sessionless::ACCEPT_END_FLAG_OFF = 0;
    const int protocol_module_sessionless::ACCEPT_END_FLAG_ON = 1;

    const int protocol_module_sessionless::SORRY_FLAG_ON = 1;
    const int protocol_module_sessionless::SORRY_FLAG_OFF = 0;

    const int protocol_module_sessionless::SORRYSERVER_SWITCH_FLAG_OFF = 0;
    const int protocol_module_sessionless::SORRYSERVER_SWITCH_FLAG_ON = 1;

    const int protocol_module_sessionless::REALSERVER_SWITCH_FLAG_OFF = 0;
    const int protocol_module_sessionless::REALSERVER_SWITCH_FLAG_ON = 1;

    const int protocol_module_sessionless::EDIT_DIVISION_NO_EDIT = 0;
    const int protocol_module_sessionless::EDIT_DIVISION_EDIT = 1;

    const int protocol_module_sessionless::FORWARDED_FOR_OFF = 0;
    const int protocol_module_sessionless::FORWARDED_FOR_ON = 1;
    //! constractor
    protocol_module_sessionless::protocol_module_sessionless() :
            http_protocol_module_base(MODULE_NAME), forwarded_for(FORWARDED_FOR_OFF)
    {
        sorry_uri.assign('\0');
    }
    //! destractor
    protocol_module_sessionless::~protocol_module_sessionless()
    {
    }
    //! tcp protocol support check
    //! @return tcp support is true
    //! @return tcp not-support is false
    bool protocol_module_sessionless::is_tcp()
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            putLogDebug(100000, "in/out_function : bool protocol_module_sessionless::is_tcp() : "
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
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            putLogDebug(100001, "in/out_function : bool protocol_module_sessionless::is_udp() : "
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
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            putLogDebug(100002, "in/out_function : void protocol_module_sessionless::"
                        "replication_interrupt().", __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/
    }
    //! initialize function. called from module control. module loaded call
    //! @param[in]    realserver list iterator begin function object type
    //!    @param[in]    realserver list iterator end function object type
    //! @param[in]    realserver list iterator next function object type
    //! @param[in]    realserver list mutex lock function object type.
    //! @param[in]    realserver list mutex unlock function object type
    void protocol_module_sessionless::initialize(rs_list_itr_func_type    inlist_begin,
            rs_list_itr_func_type    inlist_end,
            rs_list_itr_next_func_type    inlist_next,
            boost::function< void( void ) >    inlist_lock,
            boost::function< void( void ) >    inlist_unlock)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            putLogDebug(100003, "in_function : void protocol_module_sessionless::initialize("
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
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            putLogDebug(100004, "out_function : void protocol_module_sessionless::initialize("
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
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            putLogDebug(100005, "in_function : void protocol_module_sessionless::finalize().", __FILE__,
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
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            putLogDebug(100006, "function : void protocol_module_sessionless::finalize() : "
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
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            putLogDebug(100007, "function : void protocol_module_sessionless::finalize() : "
                        "replication_pay_memory.clear(), replication_area_lock.clear(), replication_area_unlock.clear() end.", __FILE__,
                        __LINE__ );
        }
        /*------DEBUG LOG END------*/

        //ScheduleModule's functions initializtion
        schedule_tcp.clear();

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            putLogDebug(100008, "function : void protocol_module_sessionless::finalize() : "
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
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            putLogDebug(100009, "function : void protocol_module_sessionless::finalize() : "
                        "forwarded_for = 0, sorry_uri.assign('\\0') end.", __FILE__,
                        __LINE__ );
        }
        /*------DEBUG LOG END------*/
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            putLogDebug(100010, "out_function : void protocol_module_sessionless::finalize().", __FILE__,
                        __LINE__ );
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
    bool protocol_module_sessionless::is_use_sorry()
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            putLogDebug(100011, "in/out_function : bool protocol_module_sessionless::is_use_sorry() : "
                        "return_value = true.", __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/
        return true;
    }

    //! realserver list update event
    void protocol_module_sessionless::handle_rslist_update()
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            putLogDebug(100012, "in/out_function : void protocol_module_sessionless::handle_rslist_update().", __FILE__,
                        __LINE__ );
        }
        /*------DEBUG LOG END------*/
    }

    //! module parameter check.used by l7vsadm
    //! @param[in]    module paramter string list
    //! @return    result.flag true is parameter is noproblem.
    //! @return result.flag false is paramter is problem.
    protocol_module_base::check_message_result protocol_module_sessionless::check_parameter(const std::vector<
            std::string>& args)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("in_function : protocol_module_base::check_message_result "
                                    "protocol_module_sessionless::check_parameter("
                                    "const std::vector<std::string>& args) : args = %s.");
            std::string argsdump;
            for (std::vector<std::string>::const_iterator it = args.begin(); it != args.end(); ++it)
            {
                argsdump += *it;
                argsdump += " ";
            }
            formatter % argsdump;
            putLogDebug(100013, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/
        using namespace boost::xpressive;
        //set check result true
        check_message_result check_result;
        check_result.flag = true;
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
            //loop option strings
            for (; it != it_end; ++it)
            {
                //option string = "-F"
                if (*it == "-F" || *it == "--forwarded-for")
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
                                putLogError(100000, check_result.message, __FILE__, __LINE__ );
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

                                putLogError(100001, check_result.message, __FILE__, __LINE__ );
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
                                    putLogError(100002, check_result.message, __FILE__, __LINE__ );
                                    //loop break
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
                            putLogError(100003, check_result.message, __FILE__,
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
                        putLogError(100004, check_result.message, __FILE__,
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
                    putLogError(100005, check_result.message, __FILE__, __LINE__ );
                    //loop break
                    break;
                }
            }
        }
        catch (const std::exception& ex)
        {
            check_result.flag = false;
            std::cerr << "protocol_module_sessionless::check_parameter() : exception : error = " << ex.what() << "." << std::endl;
            boost::format formatter("function : protocol_module_base::check_message_result "
                                    "protocol_module_sessionless::check_parameter() exception : "
                                    "error = %s.");
            formatter % ex.what();
            putLogError(100006, formatter.str(), __FILE__, __LINE__ );
        }
        catch (...)
        {
            check_result.flag = false;
            std::cerr << "protocol_module_sessionless::check_parameter() : Unknown exception." << std::endl;
            putLogError(100007, "function : protocol_module_base::check_message_result "
                        "protocol_module_sessionless::check_parameter() : "
                        "Unknown exception.", __FILE__, __LINE__ );
        }

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("out_function : protocol_module_base::check_message_result "
                                    "protocol_module_sessionless::check_parameter("
                                    "const std::vector<std::string>& args) : return_value = ("
                                    "check_message_result.flag = %d, check_message_result.message = %s).");
            formatter % check_result.flag % check_result.message;
            putLogDebug(100014, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/
        return check_result;
    }

    //! parameter set
    //! @param[in] module paramter string list
    //! @return    result.flag true is parameter is noproblem.
    //! @return result.flag false is paramter is problem.
    protocol_module_base::check_message_result protocol_module_sessionless::set_parameter(const std::vector<
            std::string>& args)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("in_function : protocol_module_base::check_message_result "
                                    "protocol_module_sessionless::set_parameter("
                                    "const std::vector<std::string>& args) : args = %s.");
            std::string argsdump;
            for (std::vector<std::string>::const_iterator it = args.begin(); it != args.end(); ++it)
            {
                argsdump += *it;
                argsdump += " ";
            }
            formatter % argsdump;
            putLogDebug(100015, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/
        using namespace boost::xpressive;
        //set check result flag true
        check_message_result check_result;
        check_result.flag = true;
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

        //set forwarded flag true
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
                                //set check result flag false
                                check_result.flag = false;
                                //set check result message
                                check_result.message = "You have to set option value '-S/--sorry-uri'.";
                                putLogError(100008, check_result.message, __FILE__,
                                            __LINE__ );
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
                                putLogError(100009, check_result.message, __FILE__,
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
                                    strcpy(sorry_uri.data(), it->c_str());
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
                                    putLogError(100010, check_result.message, __FILE__,
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
                            putLogError(100011, check_result.message, __FILE__,
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
                        putLogError(100012, check_result.message, __FILE__,
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

                    putLogError(100013, check_result.message, __FILE__,
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
            std::cerr << "protocol_module_sessionless::set_parameter() : exception : error = " << ex.what() << "." << std::endl;
            boost::format formatter("function : protocol_module_base::check_message_result "
                                    "protocol_module_sessionless::set_parameter() : exception : "
                                    "error = %s.");
            formatter % ex.what();
            putLogError(100014, formatter.str(), __FILE__, __LINE__ );
        }
        catch (...)
        {
            check_result.flag = false;
            std::cerr << "protocol_module_sessionless::set_parameter() : Unknown exception." << std::endl;
            putLogError(100015, "function : protocol_module_base::check_message_result "
                        "protocol_module_sessionless::set_parameter() : "
                        "Unknown exception.", __FILE__, __LINE__ );
        }

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("out_function : protocol_module_base::check_message_result "
                                    "protocol_module_sessionless::set_parameter("
                                    "const std::vector<std::string>& args) : return_value = ("
                                    "check_message_result.flag = %d, check_message_result.message = %s).");
            formatter % check_result.flag % check_result.message;
            putLogDebug(100016, formatter.str(), __FILE__, __LINE__ );
        }
        /*-------- DEBUG LOG END--------*/

        return check_result;
    }

    //! parameter add
    //! @param[in] module paramter string list
    //! @return    result.flag true is parameter is noproblem.
    //! @return result.flag false is paramter is problem.
    protocol_module_base::check_message_result protocol_module_sessionless::add_parameter(const std::vector<
            std::string>& args)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("in_function : protocol_module_base::check_message_result "
                                    "protocol_module_sessionless::add_parameter("
                                    "const std::vector<std::string>& args) : args = %s.");
            std::string argsdump;
            for (std::vector<std::string>::const_iterator it = args.begin(); it != args.end(); ++it)
            {
                argsdump += *it;
                argsdump += " ";
            }
            formatter % argsdump;
            putLogDebug(100017, formatter.str(), __FILE__, __LINE__ );
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
            putLogError(100016, check_result.message, __FILE__, __LINE__ );
        }

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("out_function : protocol_module_base::check_message_result "
                                    "protocol_module_sessionless::add_parameter("
                                    "const std::vector<std::string>& args) : return_value = ("
                                    "check_message_result.flag = %d, check_message_result.message = %s).");
            formatter % check_result.flag % check_result.message;
            putLogDebug(100018, formatter.str(), __FILE__, __LINE__ );
        }
        /*-------- DEBUG LOG --------*/

        return check_result;
    }



    //! TCP/IP scheduled function registation.
    //! @param[in] schedule module TCP/IP scheduled function object type
    void protocol_module_sessionless::register_schedule(tcp_schedule_func_type inschedule)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            putLogDebug(100019, "in_function : void protocol_module_sessionless::register_schedule("
                        "tcp_schedule_func_type inschedule).", __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/
        schedule_tcp = inschedule;
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            putLogDebug(100020, "out_function : void protocol_module_sessionless::register_schedule("
                        "tcp_schedule_func_type inschedule).", __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/
    }

    //! UDP scheduled function registation
    //! @param[in] schedule module UDP scheduled funtion object type
    void protocol_module_sessionless::register_schedule(udp_schedule_func_type inschedule)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            putLogDebug(100021,
                        "in/out_function : void protocol_module_sessionless::register_schedule(udp_schedule_func_type inschedule).",
                        __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/
    }

    //! called from session initialzie use in upstream_thread
    //! @param[in]    upstream thread id.
    //! @param[in]    downstream thread id
    //! @return        session use EVENT mode.
    protocol_module_base::EVENT_TAG protocol_module_sessionless::handle_session_initialize(
        const boost::thread::id up_thread_id, const boost::thread::id down_thread_id,
        const boost::asio::ip::tcp::endpoint& client_endpoint_tcp,
        const boost::asio::ip::udp::endpoint& client_endpoint_udp)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_session_initialize(const boost::thread::id up_thread_id, "
                                    "const boost::thread::id down_thread_id, const boost::asio::ip::tcp::endpoint& client_endpoint_tcp, "
                                    "const boost::asio::ip::udp::endpoint& client_endpoint_udp) : "
                                    "up_thread_id = %d, down_thread_id = %d, client_endpoint_tcp = [%s]:%d.");
            formatter % up_thread_id % down_thread_id % client_endpoint_tcp.address().to_string() % client_endpoint_tcp.port() ;
            putLogDebug(100022, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/
        EVENT_TAG status = FINALIZE;

        //session thread initialization
        try
        {
            thread_data_ptr p_up(new session_thread_data_sessionless);
            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("new : address = &(%d), size = %lu.");
                formatter % static_cast<void*>(p_up.get()) % sizeof(session_thread_data_sessionless);
                putLogDebug(100023, formatter.str(), __FILE__, __LINE__ );
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
            p_up->recive_data_map[client_endpoint_tcp] = recv_data;

            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                // data dump
                boost::format
                formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                          "handle_session_initialize() : session_thread_data_sessionless(up_thread_id) : "
                          "thread_id = %d, thread_division = %d, "
                          "pair_thread_id = %d, accept_end_flag = %d, end_flag = %d, "
                          "sorry_flag = %d, sorryserver_switch_flag = %d, realserver_switch_flag = %d, last_status = %d, client_endpoint_tcp = [%s]:%d.");
                formatter % p_up->thread_id % p_up->thread_division % p_up->pair_thread_id % p_up->accept_end_flag
                % p_up->end_flag % p_up->sorry_flag % p_up->sorryserver_switch_flag % p_up->realserver_switch_flag
                % p_up->last_status % client_endpoint_tcp.address().to_string() % client_endpoint_tcp.port();
                putLogDebug(100024, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/

            thread_data_ptr p_down(new session_thread_data_sessionless);
            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("new : address = &(%d), size = %lu.");
                formatter % static_cast<void*>(p_down.get()) % sizeof(session_thread_data_sessionless);
                putLogDebug(100025, formatter.str(), __FILE__, __LINE__ );
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
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                // data dump
                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                        "handle_session_initialize() : session_thread_data_sessionless(down_thread_id) : "
                                        "thread_id = %d, thread_division = %d, pair_thread_id = %d, accept_end_flag = %d, end_flag = %d, "
                                        "sorry_flag = %d, sorryserver_switch_flag = %d, realserver_switch_flag = %d, "
                                        "last_status = %d, client_endpoint_tcp = [%s]:%d.");
                formatter % p_down->thread_id % p_down->thread_division % p_down->pair_thread_id % p_down->accept_end_flag
                % p_down->end_flag % p_down->sorry_flag % p_down->sorryserver_switch_flag
                % p_down->realserver_switch_flag % p_down->last_status % client_endpoint_tcp.address().to_string() % client_endpoint_tcp.port();
                putLogDebug(100026, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/

            boost::mutex::scoped_lock slock(session_thread_data_map_mutex );

            session_thread_data_map[up_thread_id] = p_up;
            session_thread_data_map[down_thread_id] = p_down;

            status = ACCEPT;
        }
        catch (const std::bad_alloc&)
        {
            std::cerr << "protocol_module_sessionless::handle_session_initialize() : exception : Could not allocate memory." << std::endl;
            boost::format formatter("Could not allocate memory. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(100017, formatter.str(), __FILE__, __LINE__ );
            status = FINALIZE;
        }
        catch (const std::exception& ex)
        {
            std::cerr << "protocol_module_sessionless::handle_session_initialize() : exception : error = " << ex.what() << "." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_session_initialize() : exception : error = %s. thread id : %d.");
            formatter % ex.what() % boost::this_thread::get_id();
            putLogError(100018, formatter.str(), __FILE__, __LINE__ );
            status = FINALIZE;
        }
        catch (...)
        {
            std::cerr << "protocol_module_sessionless::handle_session_initialize() : Unknown exception." << std::endl;
            boost::format formatter("function : protocol_module_base::check_message_result "
                                    "protocol_module_sessionless::handle_session_initialize() : "
                                    "Unknown exception. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(100019, formatter.str(), __FILE__, __LINE__ );

            status = FINALIZE;
        }

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_session_initialize(const boost::thread::id up_thread_id, "
                                    "const boost::thread::id down_thread_id, const boost::asio::ip::tcp::endpoint& client_endpoint_tcp, "
                                    "const boost::asio::ip::udp::endpoint& client_endpoint_udp) : return_value = %d. thread id : %d.");
            formatter % status % boost::this_thread::get_id();
            putLogDebug(100027, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        return status;
    }
    //! called from session finalize use in upstream thread.
    //! @param[in]    upstream thread id.
    //! @param[in]    downstream thread id
    //! @return        session use EVENT mode.
    protocol_module_base::EVENT_TAG protocol_module_sessionless::handle_session_finalize(
        const boost::thread::id up_thread_id, const boost::thread::id down_thread_id)
    {

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_session_finalize(const boost::thread::id up_thread_id, "
                                    "const boost::thread::id down_thread_id) : "
                                    "up_thread_id = %d, down_thread_id = %d.");
            formatter % up_thread_id % down_thread_id;
            putLogDebug(100028, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/
        EVENT_TAG status = STOP;
        thread_data_ptr p_up;
        thread_data_ptr p_down;
        session_thread_data_map_it session_thread_data_it;
        recive_data_map_it recive_data_it;

        //session thread free
        try
        {
            boost::mutex::scoped_lock slock(session_thread_data_map_mutex );

            session_thread_data_it = session_thread_data_map.find(up_thread_id);
            if (session_thread_data_it != session_thread_data_map.end())
            {
                p_up = session_thread_data_it->second;
                /*-------- DEBUG LOG --------*/
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    boost::format formatter("delete : address = &(%d).");
                    formatter % static_cast<void*>(p_up.get());
                    putLogDebug(100029, formatter.str(), __FILE__, __LINE__ );
                }
                /*------DEBUG LOG END------*/
                session_thread_data_map.erase(up_thread_id);
            }

            session_thread_data_it = session_thread_data_map.find(down_thread_id);
            if (session_thread_data_it != session_thread_data_map.end())
            {
                /*-------- DEBUG LOG --------*/
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    boost::format formatter("delete : address = &(%d).");
                    formatter % static_cast<void*>(p_down.get());
                    putLogDebug(100030, formatter.str(), __FILE__,
                                __LINE__ );
                }
                /*------DEBUG LOG END------*/
                session_thread_data_map.erase(down_thread_id);
            }

            status = STOP;
        }
        catch (const std::exception& ex)
        {
            std::cerr << "protocol_module_sessionless::handle_session_finalize() : exception : error = " << ex.what() << "." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_session_finalize() : exception : error = %s. thread id : %d.");
            formatter % ex.what() % boost::this_thread::get_id();
            putLogError(100020, formatter.str(), __FILE__, __LINE__ );
            status = STOP;
        }
        catch (...)
        {
            std::cerr << "protocol_module_sessionless::handle_session_finalize() : Unknown exception." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_session_finalize() : "
                                    "Unknown exception. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(100021, formatter.str(), __FILE__, __LINE__ );
            status = STOP;
        }

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_session_finalize(const boost::thread::id up_thread_id, "
                                    "const boost::thread::id down_thread_id) : return_value = %d. thread id : %d.");
            formatter % status % boost::this_thread::get_id();
            putLogDebug(100031, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/
        return status;
    }

    //! called from after session accept.in client socket use in upstream thread.
    //! @param[in]    upstream thread id.
    //! @return        session use EVENT mode.
    protocol_module_base::EVENT_TAG protocol_module_sessionless::handle_accept(const boost::thread::id thread_id)
    {

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_accept(const boost::thread::id thread_id) : thread_id = %d.");
            formatter % thread_id;
            putLogDebug(100032, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        EVENT_TAG status = FINALIZE;
        thread_data_ptr session_data;
        session_thread_data_map_it session_thread_it;

        try
        {
            boost::mutex::scoped_lock slock(session_thread_data_map_mutex );

            session_thread_it = session_thread_data_map.find(thread_id);
            if (unlikely(session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL))
            {
                boost::format formatter("Invalid thread id. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogError(100022, formatter.str(), __FILE__, __LINE__ );
                throw -1;
            }

            session_data = session_thread_it->second;

            //set accept end flag ON
            session_data->accept_end_flag = ACCEPT_END_FLAG_ON;
            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                        "handle_accept(const boost::thread::id thread_id) : ACCEPT_END_FLAG_ON. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogDebug(100033, formatter.str(), __FILE__, __LINE__ );
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
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                        "handle_accept() : catch exception e = %d. thread id : %d.");
                formatter % e % boost::this_thread::get_id();
                putLogDebug(100034, formatter.str(), __FILE__, __LINE__ );
            }
            status = FINALIZE;
            /*------DEBUG LOG END------*/
        }
        catch (const std::exception& ex)
        {
            std::cerr << "protocol_module_sessionless::handle_accept() : exception : error = " << ex.what() << "." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_accept() : exception : error = %s. thread id : %d.");
            formatter % ex.what() % boost::this_thread::get_id();
            putLogError(100023, formatter.str(), __FILE__, __LINE__ );

            status = FINALIZE;
        }
        catch (...)
        {
            std::cerr << "protocol_module_sessionless::handle_accept() : Unknown exception." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG "
                                    "protocol_module_sessionless::handle_accept() : "
                                    "Unknown exception. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(100024, formatter.str(), __FILE__, __LINE__ );
            status = FINALIZE;
        }

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_accept(const boost::thread::id thread_id) : return_value = %d. thread id : %d.");
            formatter % status % boost::this_thread::get_id();
            putLogDebug(100035, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/
        return status;
    }

    //! called from after session recv in client socket. use in upstream thread.
    //! @param[in]    upstream thread id
    //! @param[in]    recive buffer refarence.
    //! @param[in]    recive length
    //! @return        session use EVENT mode.
    protocol_module_base::EVENT_TAG protocol_module_sessionless::handle_client_recv(const boost::thread::id thread_id,
            const boost::array<char, MAX_BUFFER_SIZE>& recvbuffer, const size_t recvlen)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            size_t buffer_size = recvbuffer.size() < recvlen ? recvbuffer.size() : recvlen;
            std::string buffer;
            dump_memory(recvbuffer.data(), buffer_size, buffer);
            boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_client_recv(const boost::thread::id thread_id, "
                                    "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                    "const size_t recvlen) : thread_id = %d, recvbuffer = %s, recvlen = %d.");
            formatter % thread_id % buffer % recvlen;
            putLogDebug(100036, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        EVENT_TAG status = FINALIZE;
        size_t data_remain_start = 0;
        size_t data_remain_size = 0;
        size_t request_data_remain_size = 0;
        size_t header_offset = 0;
        size_t header_offset_len = 0;
        size_t content_length_header_offset = 0;
        size_t content_length_header_len = 0;
        size_t content_len_value = 0;
        size_t pos = 0;
        size_t buffer_size = 0;
        const size_t cr_lf_len = strlen("\r\n\r\n");
        std::string str_value;
        const std::string http_header = "";
        const std::string content_header = "Content-Length";
        thread_data_ptr session_data;
        char* buffer1 = NULL;
        char* buffer2 = NULL;
        bool bret = false;
        CHECK_RESULT_TAG check_result;
        session_thread_data_map_it session_thread_it;
        recive_data_map_it recive_data_it;

        //parameter check
        if (recvlen > recvbuffer.size())
        {
            std::cerr << "protocol_module_sessionless::handle_client_recv() : Data size bigger than buffer size." << std::endl;
            boost::format formatter("Data size bigger than buffer size. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(100025, formatter.str(), __FILE__, __LINE__ );
            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                        "handle_client_recv(const boost::thread::id thread_id, "
                                        "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                        "const size_t recvlen) : return_value = %d. thread id : %d.");
                formatter % FINALIZE % boost::this_thread::get_id();
                putLogDebug(100037, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/
            return FINALIZE;
        }

        try
        {
            {
                boost::mutex::scoped_lock slock(session_thread_data_map_mutex );

                session_thread_it = session_thread_data_map.find(thread_id);
                if (unlikely(session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL))
                {
                    boost::format formatter("Invalid thread id. thread id : %d.");
                    formatter % boost::this_thread::get_id();
                    putLogError(100026, formatter.str(), __FILE__, __LINE__ );
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
                if (unlikely(recive_data_it == session_data->recive_data_map.end()))
                {
                    boost::format formatter("Invalid endpoint. thread id : %d.");
                    formatter % boost::this_thread::get_id();
                    putLogError(100027, formatter.str(), __FILE__, __LINE__ );
                    throw -1;
                }

                recive_data& recv_data = recive_data_it->second;

                send_status_it it = recv_data.send_status_list.begin();
                send_status_it it_end = recv_data.send_status_list.end();

                //status list check
                it = std::find_if(it, it_end, data_send_ok());
                if (unlikely(it != it_end))
                {
                    boost::format formatter("Sending data is not correct. thread id : %d.");
                    formatter % boost::this_thread::get_id();
                    putLogError(100028, formatter.str(), __FILE__, __LINE__ );
                    throw -1;
                }

                //status list check
                it = recv_data.send_status_list.begin();
                it = std::adjacent_find(it, it_end, data_send_repeated());
                if (unlikely(it != it_end))
                {
                    boost::format formatter("Sending data is not correct. thread id : %d.");
                    formatter % boost::this_thread::get_id();
                    putLogError(100029, formatter.str(), __FILE__, __LINE__ );
                    throw -1;
                }

                /*-------- DEBUG LOG --------*/
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    std::string datadump;
                    boost::format formatter("\nitem[%d] : status = %d, send_end_size = %d, "
                                            "send_rest_size = %d, send_possible_size = %d, "
                                            "send_offset = %d, unsend_size = %d, edit_division = %d.");
                    int i = 0;
                    for (it = recv_data.send_status_list.begin();
                            it != recv_data.send_status_list.end();
                            ++it, ++i)
                    {
                        formatter % i % it->status % it->send_end_size
                        % it->send_rest_size % it->send_possible_size
                        % it->send_offset % it->unsend_size % it->edit_division;
                        datadump += formatter.str();
                    }

                    formatter.parse("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_client_recv() : send status list dump : send status list size = %d.%s");

                    formatter % recv_data.send_status_list.size() % datadump;
                    putLogDebug(100038, formatter.str(), __FILE__, __LINE__ );
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
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    std::string datadump;
                    boost::format formatter("\nitem[%d] : status = %d, send_end_size = %d, "
                                            "send_rest_size = %d, send_possible_size = %d, "
                                            "send_offset = %d, unsend_size = %d, edit_division = %d.");
                    int i = 0;
                    for (it = recv_data.send_status_list.begin();
                            it != recv_data.send_status_list.end();
                            ++it, ++i)
                    {
                        formatter % i % it->status % it->send_end_size
                        % it->send_rest_size % it->send_possible_size
                        % it->send_offset % it->unsend_size % it->edit_division;
                        datadump += formatter.str();
                    }

                    formatter.parse("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_client_recv() : send status list dump : send status list size = %d.%s");

                    formatter % recv_data.send_status_list.size() % datadump;
                    putLogDebug(100039, formatter.str(), __FILE__, __LINE__ );
                }
                /*------DEBUG LOG END------*/

                //recive buffer process
                //buffer rest size < request size
                if (recv_data.recive_buffer_rest_size < recvlen)
                {
                    //buffer max size < remain size + request size
                    //buffer is need reallocate
                    if (recv_data.recive_buffer_max_size < data_remain_size + recvlen)
                    {
                        //the buffer's size that will be allocated is exceed the upper limit value
                        if (MAX_SESSIONLESS_MODULE_BUFFER_SIZE < data_remain_size + recvlen)
                        {
                            std::cerr << "protocol_module_sessionless::handle_client_recv() : the buffer's size that will be allacted is exceed the upper limit value." << std::endl;
                            boost::format formatter("The buffer's size that will be allocated is exceed the upper limit value. thread id : %d.");
                            formatter % boost::this_thread::get_id();
                            putLogError(100030, formatter.str(), __FILE__, __LINE__ );
                            /*-------- DEBUG LOG --------*/
                            if (unlikely(LOG_LV_DEBUG == getloglevel()))
                            {
                                boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                        "handle_client_recv(const boost::thread::id thread_id, "
                                                        "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                                        "const size_t recvlen) : return_value = %d. thread id : %d.");
                                formatter % FINALIZE % boost::this_thread::get_id();
                                putLogDebug(100040, formatter.str(), __FILE__, __LINE__ );
                            }
                            /*------DEBUG LOG END------*/
                            return FINALIZE;
                        }

                        buffer_size = (data_remain_size + recvlen) > MAX_BUFFER_SIZE ? (data_remain_size + recvlen) : MAX_BUFFER_SIZE;
                        //recive_buffer1's memory allocate and initialization
                        buffer1 = new char[buffer_size];
                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            boost::format formatter("new : address = &(%d), size = %lu.");
                            formatter % static_cast<void*>(buffer1) % buffer_size;
                            putLogDebug(100041, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        memset(buffer1, 0, buffer_size);
                        //recive_buffer2's memory allocate and initialization
                        buffer2 = new char[buffer_size];
                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            boost::format formatter("new : address = &(%d), size = %lu.");
                            formatter % static_cast<void*>(buffer2) % buffer_size;
                            putLogDebug(100042, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        memset(buffer2, 0, buffer_size);

                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            std::string datadump;
                            dump_memory(recv_data.recive_buffer + data_remain_start, data_remain_size, datadump);
                            boost::format formatter(
                                "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_client_recv() : before memcpy (data dump) : "
                                "data begin = %d, data_size = %d, data = %s");
                            formatter % data_remain_start % data_remain_size % datadump;
                            putLogDebug(100043, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        //copy data from old buffer to new buffer
                        memcpy(buffer1, recv_data.recive_buffer + data_remain_start, data_remain_size);
                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            std::string datadump;
                            dump_memory(buffer1, data_remain_size, datadump);
                            boost::format formatter(
                                "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_client_recv() : after memcpy (data dump) : "
                                "data begin = 0, data_size = %d, data = %s");
                            formatter % data_remain_size % datadump;
                            putLogDebug(100044, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/

                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            std::string datadump;
                            dump_memory(recvbuffer.data(), recvlen, datadump);
                            boost::format formatter(
                                "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_client_recv() : before memcpy (data dump) : "
                                "data begin = 0, data_size = %d, data = %s");
                            formatter % recvlen % datadump;
                            putLogDebug(100045, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        memcpy(buffer1 + data_remain_size, recvbuffer.data(), recvlen);
                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            std::string datadump;
                            dump_memory(buffer1 + data_remain_size, recvlen, datadump);
                            boost::format formatter(
                                "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_client_recv() : after memcpy (data dump) : "
                                "data begin = %d, data_size = %d, data = %s");
                            formatter % data_remain_size % recvlen % datadump;
                            putLogDebug(100046, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        //free old buffer1 and old buffer2
                        if (recv_data.recive_buffer1 != NULL)
                        {
                            /*-------- DEBUG LOG --------*/
                            if (unlikely(LOG_LV_DEBUG == getloglevel()))
                            {
                                boost::format formatter("delete : address = &(%d).");
                                formatter % static_cast<void*>(recv_data.recive_buffer1);
                                putLogDebug(100047, formatter.str(), __FILE__,
                                            __LINE__ );
                            }
                            /*------DEBUG LOG END------*/
                            delete[] recv_data.recive_buffer1;
                            recv_data.recive_buffer1 = NULL;
                        }

                        if (recv_data.recive_buffer2 != NULL)
                        {
                            /*-------- DEBUG LOG --------*/
                            if (unlikely(LOG_LV_DEBUG == getloglevel()))
                            {
                                boost::format formatter("delete : address = &(%d).");
                                formatter % static_cast<void*>(recv_data.recive_buffer2);
                                putLogDebug(100048, formatter.str(), __FILE__,
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
                        recv_data.recive_buffer_max_size = buffer_size;
                    }
                    //buffer's max size >= remain data size + requst size
                    //buffer isn't need reallocate, but switch
                    else
                    {
                        //pointer valid check
                        if (unlikely(recv_data.recive_buffer1 == NULL || recv_data.recive_buffer2 == NULL))
                        {
                            boost::format formatter("Invalid pointer. thread id : %d.");
                            formatter % boost::this_thread::get_id();
                            putLogError(100031, formatter.str(), __FILE__, __LINE__ );
                            throw -1;
                        }
                        //using buffer is buffer1
                        if (recv_data.recive_buffer == recv_data.recive_buffer1)
                        {
                            //buffer2 initialization
                            memset(recv_data.recive_buffer2, 0, recv_data.recive_buffer_max_size);
                            /*-------- DEBUG LOG --------*/
                            if (unlikely(LOG_LV_DEBUG == getloglevel()))
                            {
                                std::string datadump;
                                dump_memory(recv_data.recive_buffer + data_remain_start, data_remain_size, datadump);
                                boost::format formatter(
                                    "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_client_recv() : before memcpy (data dump) : "
                                    "data begin = %d, data_size = %d, data = %s");
                                formatter % data_remain_start % data_remain_size  % datadump;
                                putLogDebug(100049, formatter.str(), __FILE__, __LINE__ );
                            }
                            /*------DEBUG LOG END------*/
                            //copy data from buffer1 to buffer2
                            memcpy(recv_data.recive_buffer2, recv_data.recive_buffer + data_remain_start, data_remain_size);
                            /*-------- DEBUG LOG --------*/
                            if (unlikely(LOG_LV_DEBUG == getloglevel()))
                            {
                                std::string datadump;
                                dump_memory(recv_data.recive_buffer2, recvlen, datadump);
                                boost::format formatter(
                                    "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_client_recv() : after memcpy (data dump) : "
                                    "data begin = 0, data_size = %d, data = %s");
                                formatter % recvlen % datadump;
                                putLogDebug(100050, formatter.str(), __FILE__, __LINE__ );
                            }
                            /*------DEBUG LOG END------*/
                            /*-------- DEBUG LOG --------*/
                            if (unlikely(LOG_LV_DEBUG == getloglevel()))
                            {
                                std::string datadump;
                                dump_memory(recvbuffer.data(), recvlen, datadump);
                                boost::format formatter(
                                    "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_client_recv() : before memcpy (data dump) : "
                                    "data begin = 0, data_size = %d, data = %s");
                                formatter % recvlen % datadump;
                                putLogDebug(100051, formatter.str(), __FILE__, __LINE__ );
                            }
                            /*------DEBUG LOG END------*/
                            memcpy(recv_data.recive_buffer2 + data_remain_size, recvbuffer.data(), recvlen);
                            /*-------- DEBUG LOG --------*/
                            if (unlikely(LOG_LV_DEBUG == getloglevel()))
                            {
                                std::string datadump;
                                dump_memory(recv_data.recive_buffer2 + data_remain_size, recvlen, datadump);
                                boost::format formatter(
                                    "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_client_recv() : after memcpy (data dump) : "
                                    "data begin = %d, data_size = %d, data = %s");
                                formatter % data_remain_size % recvlen % datadump;
                                putLogDebug(100052, formatter.str(), __FILE__, __LINE__ );
                            }
                            /*------DEBUG LOG END------*/
                            //set buffer2 as using buffer
                            recv_data.recive_buffer = recv_data.recive_buffer2;
                        }
                        //using buffer is buffer2
                        else
                        {
                            //buffer1 initializtion
                            memset(recv_data.recive_buffer1, 0, recv_data.recive_buffer_max_size);
                            /*-------- DEBUG LOG --------*/
                            if (unlikely(LOG_LV_DEBUG == getloglevel()))
                            {
                                std::string datadump;
                                dump_memory(recv_data.recive_buffer + data_remain_start, data_remain_size, datadump);
                                boost::format formatter(
                                    "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_client_recv() : before memcpy (data dump) : "
                                    "data begin = %d, data_size = %d, data = %s");
                                formatter % data_remain_start % data_remain_size % datadump;
                                putLogDebug(100053, formatter.str(), __FILE__, __LINE__ );
                            }
                            /*------DEBUG LOG END------*/
                            //copy data from buffer2 to buffer1
                            memcpy(recv_data.recive_buffer1, recv_data.recive_buffer + data_remain_start, data_remain_size);
                            /*-------- DEBUG LOG --------*/
                            if (unlikely(LOG_LV_DEBUG == getloglevel()))
                            {
                                std::string datadump;
                                dump_memory(recv_data.recive_buffer1, data_remain_size, datadump);
                                boost::format formatter(
                                    "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_client_recv() : after memcpy (data dump) : "
                                    "data begin = 0, data_size = %d, data = %s");
                                formatter % data_remain_size % datadump;
                                putLogDebug(100054, formatter.str(), __FILE__, __LINE__ );
                            }
                            /*------DEBUG LOG END------*/
                            /*-------- DEBUG LOG --------*/
                            if (unlikely(LOG_LV_DEBUG == getloglevel()))
                            {
                                std::string datadump;
                                dump_memory(recvbuffer.data(), recvlen, datadump);
                                boost::format formatter(
                                    "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_client_recv() : before memcpy (data dump) : "
                                    "data begin = 0, data_size = %d, data = %s");
                                formatter % recvlen % datadump;
                                putLogDebug(100055, formatter.str(), __FILE__, __LINE__ );
                            }
                            /*------DEBUG LOG END------*/
                            memcpy(recv_data.recive_buffer1 + data_remain_size, recvbuffer.data(), recvlen);
                            /*-------- DEBUG LOG --------*/
                            if (unlikely(LOG_LV_DEBUG == getloglevel()))
                            {
                                std::string datadump;
                                dump_memory(recv_data.recive_buffer1 + data_remain_size, recvlen, datadump);
                                boost::format formatter(
                                    "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_client_recv() : after memcpy (data dump) : "
                                    "data begin = %d, data_size = %d, data = %s");
                                formatter % data_remain_size % recvlen % datadump;
                                putLogDebug(100056, formatter.str(), __FILE__, __LINE__ );
                            }
                            /*------DEBUG LOG END------*/
                            //set buffer1 as using buffer
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
                        it_begin->send_offset -= data_remain_start;
                    }
                }
                //buffer's rest size >= request size
                else
                {
                    //pointer valid check
                    if (unlikely(recv_data.recive_buffer == NULL))
                    {
                        boost::format formatter("Invalid pointer. thread id : %d.");
                        formatter % boost::this_thread::get_id();
                        putLogError(100032, formatter.str(), __FILE__, __LINE__ );
                        throw -1;
                    }
                    /*-------- DEBUG LOG --------*/
                    if (unlikely(LOG_LV_DEBUG == getloglevel()))
                    {
                        std::string datadump;
                        dump_memory(recvbuffer.data(), recvlen, datadump);
                        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                "handle_client_recv() : before memcpy (data dump) : "
                                                "data begin = 0, data_size = %d, data = %s");
                        formatter % recvlen % datadump;
                        putLogDebug(100057, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                    //copy data from parameter to using buffer
                    memcpy(recv_data.recive_buffer + recv_data.recive_buffer_max_size - recv_data.recive_buffer_rest_size,
                           recvbuffer.data(), recvlen);
                    /*-------- DEBUG LOG --------*/
                    if (unlikely(LOG_LV_DEBUG == getloglevel()))
                    {
                        std::string datadump;
                        dump_memory(recv_data.recive_buffer + recv_data.recive_buffer_max_size - recv_data.recive_buffer_rest_size,
                                    recvlen, datadump);
                        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                "handle_client_recv() : after memcpy (data dump) : "
                                                "data begin = %d, data_size = %d, data = %s");
                        formatter % (recv_data.recive_buffer_max_size - recv_data.recive_buffer_rest_size )
                        % recvlen % datadump;
                        putLogDebug(100058, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                    //buffer's rest size recalc
                    recv_data.recive_buffer_rest_size -= recvlen;
                    //remain data size recalc
                    data_remain_size += recvlen;
                }

                it = recv_data.send_status_list.begin();
                it_end = recv_data.send_status_list.end();

                //set request rest size
                request_data_remain_size = recvlen;

                //original status process
                for (; it != it_end; ++it)
                {
                    //status is SEND_CONTINUE
                    if (it->status == SEND_CONTINUE)
                    {
                        //send rest size > request size
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
                        //send rest size <= request size
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
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                    "handle_client_recv() : call check_http_method : "
                                                    "return_value = %d. thread id : %d.");
                            formatter % check_result % boost::this_thread::get_id();
                            putLogDebug(100059, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        //check http method result is OK
                        if (check_result == CHECK_OK)
                        {
                            //check http version
                            check_result = check_http_version(recv_data.recive_buffer + it->send_offset, data_remain_size);
                            /*-------- DEBUG LOG --------*/
                            if (unlikely(LOG_LV_DEBUG == getloglevel()))
                            {
                                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                        "handle_client_recv() : call check_http_version : "
                                                        "return_value = %d. thread id : %d.");
                                formatter % check_result % boost::this_thread::get_id();
                                putLogDebug(100060, formatter.str(), __FILE__, __LINE__ );
                            }
                            /*------DEBUG LOG END------*/
                        }
                        //check method and version result is OK
                        if (check_result == CHECK_OK)
                        {
                            //search http header
                            bret = find_http_header(recv_data.recive_buffer + it->send_offset, data_remain_size, http_header,
                                                    header_offset, header_offset_len);
                            /*-------- DEBUG LOG --------*/
                            if (unlikely(LOG_LV_DEBUG == getloglevel()))
                            {
                                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                        "handle_client_recv() : call find_http_header : "
                                                        "return_value = %d. thread id : %d.");
                                formatter % static_cast<int>(bret) % boost::this_thread::get_id();
                                putLogDebug(100061, formatter.str(), __FILE__, __LINE__ );
                            }
                            /*------DEBUG LOG END------*/
                            //search http header result is OK
                            if (bret)
                            {
                                //search Content_Length header
                                bret = find_http_header(recv_data.recive_buffer + it->send_offset, data_remain_size,
                                                        content_header, content_length_header_offset, content_length_header_len);
                                /*-------- DEBUG LOG --------*/
                                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                                {
                                    boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                            "handle_client_recv() : call find_http_header : "
                                                            "return_value = %d. thread id : %d.");
                                    formatter % static_cast<int>(bret) % boost::this_thread::get_id();
                                    putLogDebug(100062, formatter.str(), __FILE__, __LINE__ );
                                }
                                /*------DEBUG LOG END------*/
                                //search Content_Length result is OK
                                if (bret)
                                {
                                    //Get Content_Length header's numeric value
                                    for (pos = 0; recv_data.recive_buffer[it->send_offset + content_length_header_offset + pos] != ':' && pos
                                            < content_length_header_len; ++pos)
                                        ;
                                    if (pos == content_length_header_len)
                                    {
                                        throw std::string("Content_Length field's value is invalid.");
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
                                //search Content_Length result is NG
                                else
                                {
                                    //send_rest_size recalc
                                    //set whole http header's length
                                    it->send_rest_size = header_offset + header_offset_len + cr_lf_len;
                                }
                                //set edit_division flag on
                                it->edit_division = EDIT_DIVISION_EDIT;
                            }
                            //search http header result is NG
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
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    std::string datadump;
                    boost::format formatter("\nitem[%d] : status = %d, send_end_size = %d, "
                                            "send_rest_size = %d, send_possible_size = %d, "
                                            "send_offset = %d, unsend_size = %d, edit_division = %d.");
                    int i = 0;
                    for (it = recv_data.send_status_list.begin();
                            it != recv_data.send_status_list.end();
                            ++it, ++i)
                    {
                        formatter % i % it->status % it->send_end_size
                        % it->send_rest_size % it->send_possible_size
                        % it->send_offset % it->unsend_size % it->edit_division;
                        datadump += formatter.str();
                    }

                    formatter.parse("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_client_recv() : send status list dump : send status list size = %d.%s");

                    formatter % recv_data.send_status_list.size() % datadump;
                    putLogDebug(100063, formatter.str(), __FILE__, __LINE__ );
                }
                /*------DEBUG LOG END------*/
                //there are still rest data need to process
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
                    if (unlikely(LOG_LV_DEBUG == getloglevel()))
                    {
                        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                "handle_client_recv() : call check_http_method : "
                                                "return_value = %d. thread id : %d.");
                        formatter % check_result % boost::this_thread::get_id();
                        putLogDebug(100064, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                    //check http method result is OK
                    if (check_result == CHECK_OK)
                    {
                        //check http version
                        check_result = check_http_version(recv_data.recive_buffer + new_send_it->send_offset,
                                                          request_data_remain_size);
                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                    "handle_client_recv() : call check_http_version : "
                                                    "return_value = %d. thread id : %d.");
                            formatter % check_result % boost::this_thread::get_id();
                            putLogDebug(100065, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                    }
                    //check http method and version result is OK
                    if (check_result == CHECK_OK)
                    {
                        //search whole http header, get whole http header's offset and length
                        bret = find_http_header(recv_data.recive_buffer + new_send_it->send_offset, request_data_remain_size,
                                                http_header, header_offset, header_offset_len);
                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                    "handle_client_recv() : call find_http_header : "
                                                    "return_value = %d. thread id : %d.");
                            formatter % check_result % boost::this_thread::get_id();
                            putLogDebug(100066, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        //searched whole http header
                        if (bret)
                        {
                            //search ContentLength http header, get ContentLength header's offset and length
                            bret = find_http_header(recv_data.recive_buffer + new_send_it->send_offset,
                                                    request_data_remain_size, content_header, content_length_header_offset, content_length_header_len);
                            /*-------- DEBUG LOG --------*/
                            if (unlikely(LOG_LV_DEBUG == getloglevel()))
                            {
                                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                        "handle_client_recv() : call find_http_header : "
                                                        "return_value = %d. thread id : %d.");
                                formatter % static_cast<int>(bret) % boost::this_thread::get_id();
                                putLogDebug(100067, formatter.str(), __FILE__, __LINE__ );
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
                                    throw std::string("Content_Length field's value is invalid.");
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
                                    throw std::string("Content_Length field's value is invalid.");
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

                    //check http method or version result is impossible
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
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    std::string datadump;
                    boost::format formatter("\nitem[%d] : status = %d, send_end_size = %d, "
                                            "send_rest_size = %d, send_possible_size = %d, "
                                            "send_offset = %d, unsend_size = %d, edit_division = %d.");
                    int i = 0;
                    for (it = recv_data.send_status_list.begin();
                            it != recv_data.send_status_list.end();
                            ++it, ++i)
                    {
                        formatter % i % it->status % it->send_end_size
                        % it->send_rest_size % it->send_possible_size
                        % it->send_offset % it->unsend_size % it->edit_division;
                        datadump += formatter.str();
                    }

                    formatter.parse("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_client_recv() : send status list dump : send status list size = %d.%s");

                    formatter % recv_data.send_status_list.size() % datadump;
                    putLogDebug(100068, formatter.str(), __FILE__, __LINE__ );
                }
                /*------DEBUG LOG END------*/
                //search for send_possible item in status list
                send_status_it it_find = find_if(recv_data.send_status_list.begin(), recv_data.send_status_list.end(),
                                                 data_send_possible());
                //the data that can be sent possible is exist
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
                //the data that can be sent possible is not exist
                else
                {
                    status = CLIENT_RECV;
                }
            }
        }
        catch (int e)
        {
            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                        "handle_client_recv() : catch exception e = %d. thread id : %d.");
                formatter % e % boost::this_thread::get_id();
                putLogDebug(100069, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/
            status = FINALIZE;
        }
        catch (const std::string& ex)
        {
            std::cerr << "protocol_module_sessionless::handle_client_recv() : exception : " << ex << std::endl;
            boost::format formatter("protocol_module_sessionless::handle_client_recv() : exception :  %s. thread id : %d.");
            formatter % ex.c_str() % boost::this_thread::get_id();
            putLogError(100033, formatter.str(), __FILE__, __LINE__ );
            status = FINALIZE;
        }
        catch (const std::bad_alloc&)
        {
            std::cerr << "protocol_module_sessionless::handle_client_recv() : exception : Could not allocate memory." << std::endl;
            boost::format formatter("Could not allocate memory. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(100034, formatter.str(), __FILE__, __LINE__ );
            status = FINALIZE;
        }
        catch (const std::exception& ex)
        {
            std::cerr << "protocol_module_sessionless::handle_client_recv() : exception : error = " << ex.what() << "." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_client_recv() : exception : error = %s. thread id : %d.");
            formatter % ex.what() % boost::this_thread::get_id();
            putLogError(100035, formatter.str(), __FILE__, __LINE__ );

            status = FINALIZE;
        }
        catch (...)
        {
            std::cerr << "protocol_module_sessionless::handle_client_recv() : Unknown exception." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG "
                                    "protocol_module_sessionless::handle_client_recv() : "
                                    "Unknown exception. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(100036, formatter.str(), __FILE__, __LINE__ );
            status = FINALIZE;
        }

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_client_recv(const boost::thread::id thread_id, "
                                    "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                    "const size_t recvlen) : return_value = %d. thread id : %d.");
            formatter % status % boost::this_thread::get_id();
            putLogDebug(100070, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        return status;
    }

    //! called from after realserver select.use in upstream thread.
    //! @param[in]    upstream thread id
    //! @param[out]    realserver TCP endpoint
    //! @return        session use EVENT mode.
    protocol_module_base::EVENT_TAG protocol_module_sessionless::handle_realserver_select(
        const boost::thread::id thread_id, boost::asio::ip::tcp::endpoint & rs_endpoint)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_realserver_select(const boost::thread::id thread_id, "
                                    "boost::asio::ip::tcp::endpoint & rs_endpoint) : "
                                    "thread_id = %d, rs_endpoint = [%s]:%d.");
            formatter % thread_id % rs_endpoint.address().to_string() % rs_endpoint.port();
            putLogDebug(100071, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/
        EVENT_TAG status = FINALIZE;
        boost::asio::ip::tcp::endpoint tmp_endpoint;
        thread_data_ptr session_data;
        session_thread_data_map_it session_thread_it;
        session_thread_data_map_it session_thread_it_end;
        recive_data_map_it recive_data_it;

        if (schedule_tcp.empty())
        {
            std::cerr << "protocol_module_sessionless::handle_realserver_select() : Schedule_tcp function is empty." << std::endl;
            boost::format formatter("Schedule_tcp function is empty. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(100037, formatter.str(), __FILE__, __LINE__ );
            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                        "handle_realserver_select(const boost::thread::id thread_id, "
                                        "boost::asio::ip::tcp::endpoint & rs_endpoint)"
                                        " : return_value = %d. thread id : %d.");
                formatter % FINALIZE % boost::this_thread::get_id();
                putLogDebug(100072, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/
            return FINALIZE;
        }

        try
        {
            {
                boost::mutex::scoped_lock slock(session_thread_data_map_mutex );

                session_thread_it = session_thread_data_map.find(thread_id);
                if (unlikely(session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL))
                {
                    boost::format formatter("Invalid thread id. thread id : %d.");
                    formatter % boost::this_thread::get_id();
                    putLogError(100038, formatter.str(), __FILE__, __LINE__ );
                    throw -1;
                }

                session_data = session_thread_it->second;
            }

            //call schedule_module's schedule function, get realserver endpoint
            schedule_tcp(thread_id, rs_list_begin, rs_list_end, rs_list_next, rs_endpoint);
            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                        "handle_realserver_select() : call schedule_tcp : "
                                        "rs_endpoint = [%s]:%d. thread id : %d.");
                formatter % rs_endpoint.address().to_string() % rs_endpoint.port() % boost::this_thread::get_id();
                putLogDebug(100073, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/

            //endpoint decide
            if (rs_endpoint != tmp_endpoint)
            {
                //save rs endpoint
                session_data->target_endpoint = rs_endpoint;

                recive_data_it = session_data->recive_data_map.find(session_data->client_endpoint_tcp);
                if (unlikely(recive_data_it == session_data->recive_data_map.end()))
                {
                    boost::format formatter("Invalid endpoint. thread id : %d.");
                    formatter % boost::this_thread::get_id();
                    putLogError(100039, formatter.str(), __FILE__, __LINE__ );
                    throw -1;
                }
                recive_data& recv_data = recive_data_it->second;

                send_status_it it = recv_data.send_status_list.begin();
                send_status_it it_end = recv_data.send_status_list.end();

                it = find_if(it, it_end, data_send_possible());
                if (it != it_end)
                {
                    status = REALSERVER_CONNECT;
                }
                else
                {
                    status = CLIENT_RECV;
                }
            }
            else
            {
                //set end flag on
                session_data->end_flag = END_FLAG_ON;
                /*-------- DEBUG LOG --------*/
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                            "handle_realserver_select() : END_FLAG_ON. thread id : %d.");
                    formatter % boost::this_thread::get_id();
                    putLogDebug(100074, formatter.str(), __FILE__, __LINE__ );
                }
                /*------DEBUG LOG END------*/
                status = CLIENT_DISCONNECT;
            }
        }
        catch (int e)
        {
            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                        "handle_realserver_select() : catch exception e = %d. thread id : %d.");
                formatter % e % boost::this_thread::get_id();
                putLogDebug(100075, formatter.str(), __FILE__, __LINE__ );
            }
            status = FINALIZE;
            /*------DEBUG LOG END------*/
        }
        catch (const std::exception& ex)
        {
            std::cerr << "protocol_module_sessionless::handle_realserver_select() : exception : error = " << ex.what() << "." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_realserver_select() : exception : error = %s. thread id : %d.");
            formatter % ex.what() % boost::this_thread::get_id();
            putLogError(100040, formatter.str(), __FILE__, __LINE__ );
            status = FINALIZE;
        }
        catch (...)
        {
            std::cerr << "protocol_module_sessionless::handle_realserver_select() : Unknown exception." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG "
                                    "protocol_module_sessionless::handle_realserver_select() : "
                                    "Unknown exception. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(100041, formatter.str(), __FILE__, __LINE__ );
            status = FINALIZE;
        }

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_realserver_select(const boost::thread::id thread_id, "
                                    "boost::asio::ip::tcp::endpoint & rs_endpoint)"
                                    " : return_value = %d. thread id : %d.");
            formatter % status % boost::this_thread::get_id();
            putLogDebug(100076, formatter.str(), __FILE__, __LINE__ );
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
    protocol_module_base::EVENT_TAG protocol_module_sessionless::handle_realserver_select(
        const boost::thread::id thread_id, boost::asio::ip::udp::endpoint& rs_endpoint, boost::array<char,
        MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("in/out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_realserver_select(const boost::thread::id thread_id, "
                                    "boost::asio::ip::udp::endpoint& rs_endpoint, boost::array<char,MAX_BUFFER_SIZE>& sendbuffer, "
                                    "size_t& datalen) : "
                                    "return_value = %d. thread id : %d.");
            formatter % STOP % boost::this_thread::get_id();
            putLogDebug(100077, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/
        return STOP;
    }
    //! called from after realserver connect
    //! @param[in]    upstream thread id
    //! @param[out]    sendbuffer reference
    //! @param[out]    send data length
    //! @return        session use EVENT mode.
    protocol_module_base::EVENT_TAG protocol_module_sessionless::handle_realserver_connect(
        const boost::thread::id thread_id, boost::array<char, MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_realserver_connect(const boost::thread::id thread_id, "
                                    "boost::array<char, MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen) : "
                                    "thread_id = %d.");
            formatter % thread_id;
            putLogDebug(100078, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        EVENT_TAG status = FINALIZE;
        bool ret = false;
        size_t header_offset = 0;
        size_t header_offset_len = 0;
        size_t send_buffer_remian_size = 0;
        size_t copy_size = 0;
        const int send_buffer_end_size = sendbuffer.max_size();
        const std::string http_header = "";
        const std::string str_forword_for = "X-Forwarded-For";
        thread_data_ptr session_data;
        session_thread_data_map_it session_thread_it;
        recive_data_map_it recive_data_it;

        try
        {
            {
                boost::mutex::scoped_lock slock(session_thread_data_map_mutex );

                //thread id check
                session_thread_it = session_thread_data_map.find(thread_id);
                if (unlikely(session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL))
                {
                    boost::format formatter("Invalid thread id. thread id : %d.");
                    formatter % boost::this_thread::get_id();
                    putLogError(100042, formatter.str(), __FILE__, __LINE__ );
                    throw -1;
                }

                session_data = session_thread_it->second;
            }

            //endpoint check
            recive_data_it = session_data->recive_data_map.find(session_data->client_endpoint_tcp);
            if (unlikely(recive_data_it == session_data->recive_data_map.end()))
            {
                boost::format formatter("Invalid endpoint. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogError(100043, formatter.str(), __FILE__, __LINE__ );
                throw -1;
            }

            //recive_buffer pointer check
            recive_data& recv_data = recive_data_it->second;
            if (unlikely(recv_data.recive_buffer == NULL))
            {
                boost::format formatter("Invalid pointer. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogError(100044, formatter.str(), __FILE__, __LINE__ );
                throw -1;
            }

            //send list check
            send_status_it it = recv_data.send_status_list.begin();
            send_status_it it_end = recv_data.send_status_list.end();
            it = find_if(it, it_end, data_send_possible());
            if (unlikely(it == it_end))
            {
                boost::format formatter("Sending possible data is not existed. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogError(100045, formatter.str(), __FILE__, __LINE__ );
                throw -1;
            }

            //send buffer rest size initialization
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
                    if (unlikely(LOG_LV_DEBUG == getloglevel()))
                    {
                        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                "handle_realserver_connect() : call find_http_header : "
                                                "return_value = %d. thread id : %d.");
                        formatter % static_cast<int>(ret) % boost::this_thread::get_id();
                        putLogDebug(100079, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                    //search http header result is OK
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
                    //search http header result is NG
                    else
                    {
                        //search whole http header, get whole http header's offset and length
                        ret = find_http_header(recv_data.recive_buffer + it->send_offset, it->send_possible_size, "",
                                               header_offset, header_offset_len);
                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                    "handle_realserver_connect() : call find_http_header : "
                                                    "return_value = %d. thread id : %d.");
                            formatter % static_cast<int>(ret) % boost::this_thread::get_id();
                            putLogDebug(100080, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        if (!ret)
                        {
                            boost::format formatter("find_http_header() function failure. thread id : %d.");
                            formatter % boost::this_thread::get_id();
                            putLogError(100046, formatter.str(), __FILE__, __LINE__ );
                            /*-------- DEBUG LOG --------*/
                            if (unlikely(LOG_LV_DEBUG == getloglevel()))
                            {
                                boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                        "handle_realserver_connect(const boost::thread::id thread_id, "
                                                        "boost::array<char,MAX_BUFFER_SIZE>& sendbuffer, "
                                                        "size_t& datalen) : return_value = %d. thread id : %d.");
                                formatter % FINALIZE % boost::this_thread::get_id();
                                putLogDebug(100081, formatter.str(), __FILE__, __LINE__ );
                            }
                            /*------DEBUG LOG END------*/
                            return FINALIZE;
                        }
                        //create X-Forwarded-For header, put it to edata.data
                        edata.data = str_forword_for;
                        edata.data += ": ";
                        edata.data += session_data->client_endpoint_tcp.address().to_string();
                        edata.data += "\r\n";
                        //save new X-Forwarded-For header offset
                        edata.insert_posission = header_offset;
                        //save new X-Forwarded-For header length
                        edata.data_size = edata.data.size();
                        //save old X-Forwarded-For header length
                        edata.replace_size = 0;
                    }

                    //add to edit_data_list
                    it->edit_data_list.push_back(edata);
                }

                /*-------- DEBUG LOG --------*/
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                            "handle_realserver_connect() : Copy data loop start. thread id : %d.");
                    formatter % boost::this_thread::get_id();
                    putLogDebug(100082, formatter.str(), __FILE__, __LINE__ );
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
                                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                                {
                                    std::string datadump;
                                    dump_memory(recv_data.recive_buffer + it->send_offset + it->send_end_size,
                                                it->send_possible_size, datadump);
                                    boost::format formatter(
                                        "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                        "handle_realserver_connect() : before memcpy (data dump) : "
                                        "data begin = %d, data_size = %d, data = %s");
                                    formatter % (it->send_offset + it->send_end_size)
                                    % copy_size % datadump;
                                    putLogDebug(100083, formatter.str(), __FILE__, __LINE__ );
                                }
                                /*------DEBUG LOG END------*/
                                //copy data from recive_buffer to sendbuffer by sending_possible size
                                memcpy(sendbuffer.data() + send_buffer_end_size - send_buffer_remian_size,
                                       recv_data.recive_buffer + it->send_offset + it->send_end_size,
                                       it->send_possible_size);
                                /*-------- DEBUG LOG --------*/
                                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                                {
                                    std::string datadump;
                                    dump_memory(sendbuffer.data() + send_buffer_end_size - send_buffer_remian_size,
                                                it->send_possible_size, datadump);

                                    boost::format formatter(
                                        "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                        "handle_realserver_connect() : after memcpy (data dump) : "
                                        "data begin = %d, data_size = %d, data = %s");
                                    formatter % (send_buffer_end_size - send_buffer_remian_size)
                                    % copy_size % datadump;
                                    putLogDebug(100084, formatter.str(), __FILE__, __LINE__ );
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
                                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                                {
                                    std::string datadump;
                                    dump_memory(recv_data.recive_buffer + it->send_offset + it->send_end_size,
                                                copy_size, datadump);

                                    boost::format formatter(
                                        "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                        "handle_realserver_connect() : before memcpy (data dump) : "
                                        "data begin = %d, data_size = %d, data = %s");
                                    formatter % (it->send_offset + it->send_end_size)
                                    % copy_size % datadump;
                                    putLogDebug(100085, formatter.str(), __FILE__, __LINE__ );
                                }
                                /*------DEBUG LOG END------*/
                                //copy data from recive_buffer to sendbuffer by send buffer rest size
                                memcpy(sendbuffer.data() + send_buffer_end_size - send_buffer_remian_size,
                                       recv_data.recive_buffer + it->send_offset + it->send_end_size, copy_size);
                                /*-------- DEBUG LOG --------*/
                                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                                {
                                    std::string datadump;
                                    dump_memory(sendbuffer.data() + send_buffer_end_size - send_buffer_remian_size,
                                                copy_size, datadump);

                                    boost::format formatter(
                                        "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                        "handle_realserver_connect() : after memcpy (data dump) : "
                                        "data begin = %d, data_size = %d, data = %s");
                                    formatter % (send_buffer_end_size - send_buffer_remian_size)
                                    % copy_size % datadump;
                                    putLogDebug(100086, formatter.str(), __FILE__, __LINE__ );
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
                        //find the item in the list which has mininum insert_posission
                        std::list<edit_data>::iterator edit_min = std::min_element(it->edit_data_list.begin(),
                                it->edit_data_list.end());
                        //send_buffer_remian_size is larger than data that before X-Forwarded-For
                        if (send_buffer_remian_size >= edit_min->insert_posission - it->send_end_size)
                        {
                            //copy data before X-Forwarded-For
                            copy_size = edit_min->insert_posission - it->send_end_size;
                            /*-------- DEBUG LOG --------*/
                            if (unlikely(LOG_LV_DEBUG == getloglevel()))
                            {
                                std::string datadump;
                                dump_memory(recv_data.recive_buffer + it->send_offset + it->send_end_size,
                                            copy_size, datadump);

                                boost::format formatter(
                                    "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_realserver_connect() : before memcpy (data dump) : "
                                    "data begin = %d, data_size = %d, data = %s");
                                formatter % (it->send_offset + it->send_end_size)
                                % copy_size % datadump;
                                putLogDebug(100087, formatter.str(), __FILE__, __LINE__ );
                            }
                            /*------DEBUG LOG END------*/
                            memcpy(sendbuffer.data() + send_buffer_end_size - send_buffer_remian_size,
                                   recv_data.recive_buffer + it->send_offset + it->send_end_size, copy_size);
                            /*-------- DEBUG LOG --------*/
                            if (unlikely(LOG_LV_DEBUG == getloglevel()))
                            {
                                std::string datadump;
                                dump_memory(sendbuffer.data() + send_buffer_end_size - send_buffer_remian_size,
                                            copy_size, datadump);
                                boost::format formatter(
                                    "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_realserver_connect() : after memcpy (data dump) : "
                                    "data begin = %d, data_size = %d, data = %s");
                                formatter % (send_buffer_end_size - send_buffer_remian_size)
                                % copy_size % datadump;
                                putLogDebug(100088, formatter.str(), __FILE__, __LINE__ );
                            }
                            /*------DEBUG LOG END------*/
                            it->send_end_size += copy_size;
                            it->send_possible_size -= copy_size;
                            send_buffer_remian_size -= copy_size;

                            //there is remain buffer for copy X-Forwarded-For
                            if (send_buffer_remian_size >= edit_min->data_size)
                            {
                                /*-------- DEBUG LOG --------*/
                                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                                {
                                    std::string datadump;
                                    dump_memory(edit_min->data.c_str(),
                                                edit_min->data_size, datadump);

                                    boost::format formatter(
                                        "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                        "handle_realserver_connect() : before memcpy (data dump) : "
                                        "data begin = 0, data_size = %d, data = %s");
                                    formatter % edit_min->data_size % datadump;
                                    putLogDebug(100089, formatter.str(), __FILE__, __LINE__ );
                                }
                                /*------DEBUG LOG END------*/
                                //copy  X-Forwarded-For
                                memcpy(sendbuffer.data() + send_buffer_end_size - send_buffer_remian_size,
                                       edit_min->data.c_str(), edit_min->data_size);
                                /*-------- DEBUG LOG --------*/
                                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                                {
                                    std::string datadump;
                                    dump_memory(sendbuffer.data() + send_buffer_end_size - send_buffer_remian_size,
                                                edit_min->data_size, datadump);
                                    boost::format formatter(
                                        "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                        "handle_realserver_connect() : after memcpy (data dump) : "
                                        "data begin = %d, data_size = %d, data = %s");
                                    formatter % (send_buffer_end_size - send_buffer_remian_size)
                                    % edit_min->data_size % datadump;
                                    putLogDebug(100090, formatter.str(), __FILE__, __LINE__ );
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
                            if (unlikely(LOG_LV_DEBUG == getloglevel()))
                            {
                                std::string datadump;
                                dump_memory(recv_data.recive_buffer + it->send_offset + it->send_end_size,
                                            copy_size, datadump);

                                boost::format formatter(
                                    "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_realserver_connect() : before memcpy (data dump) : "
                                    "data begin = %d, data_size = %d, data = %s");
                                formatter % (it->send_offset + it->send_end_size)
                                % copy_size % datadump;
                                putLogDebug(100091, formatter.str(), __FILE__, __LINE__ );
                            }
                            /*------DEBUG LOG END------*/
                            //copy data as large as possible
                            memcpy(sendbuffer.data() + send_buffer_end_size - send_buffer_remian_size,
                                   recv_data.recive_buffer + it->send_offset + it->send_end_size, copy_size);
                            /*-------- DEBUG LOG --------*/
                            if (unlikely(LOG_LV_DEBUG == getloglevel()))
                            {
                                std::string datadump;
                                dump_memory(sendbuffer.data() + send_buffer_end_size - send_buffer_remian_size,
                                            copy_size, datadump);

                                boost::format formatter(
                                    "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_realserver_connect() : after memcpy (data dump) : "
                                    "data begin = %d, data_size = %d, data = %s");
                                formatter % (send_buffer_end_size - send_buffer_remian_size)
                                % copy_size % datadump;
                                putLogDebug(100092, formatter.str(), __FILE__, __LINE__ );
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
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                            "handle_realserver_connect() : Copy data loop end. thread id : %d.");
                    formatter % boost::this_thread::get_id();
                    putLogDebug(100093, formatter.str(), __FILE__, __LINE__ );
                }
                /*------DEBUG LOG END------*/
            }
            //edit_division flag is off
            else
            {
                //copy data as large as possible
                //send_possible_size is larger
                if (send_buffer_remian_size >= it->send_possible_size)
                {
                    copy_size = it->send_possible_size;
                    /*-------- DEBUG LOG --------*/
                    if (unlikely(LOG_LV_DEBUG == getloglevel()))
                    {
                        std::string datadump;
                        dump_memory(recv_data.recive_buffer + it->send_offset,
                                    copy_size, datadump);
                        boost::format formatter(
                            "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                            "handle_realserver_connect() : before memcpy (data dump) : "
                            "data begin = %d, data_size = %d, data = %s");
                        formatter % it->send_offset
                        % copy_size % datadump;
                        putLogDebug(100094, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                    //copy data by send_possible size
                    memcpy(sendbuffer.data(), recv_data.recive_buffer + it->send_offset, copy_size);
                    /*-------- DEBUG LOG --------*/
                    if (unlikely(LOG_LV_DEBUG == getloglevel()))
                    {
                        std::string datadump;
                        dump_memory(sendbuffer.data(), copy_size, datadump);
                        boost::format formatter(
                            "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                            "handle_realserver_connect() : after memcpy (data dump) : "
                            "data begin = 0, data_size = %d, data = %s");
                        formatter % copy_size % datadump;
                        putLogDebug(100095, formatter.str(), __FILE__, __LINE__ );
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
                    if (unlikely(LOG_LV_DEBUG == getloglevel()))
                    {
                        std::string datadump;
                        dump_memory(recv_data.recive_buffer + it->send_offset, send_buffer_remian_size, datadump);

                        boost::format formatter(
                            "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                            "handle_realserver_connect() : before memcpy (data dump) : "
                            "data begin = %d, data_size = %d, data = %s");
                        formatter % it->send_offset
                        % send_buffer_remian_size % datadump;
                        putLogDebug(100096, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                    //copy data by buffer rest size
                    memcpy(sendbuffer.data(), recv_data.recive_buffer + it->send_offset, send_buffer_remian_size);
                    /*-------- DEBUG LOG --------*/
                    if (unlikely(LOG_LV_DEBUG == getloglevel()))
                    {
                        std::string datadump;
                        dump_memory(sendbuffer.data(), send_buffer_remian_size, datadump);
                        boost::format formatter(
                            "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                            "handle_realserver_connect() : after memcpy (data dump) : "
                            "data begin = 0, data_size = %d, data = %s");
                        formatter % send_buffer_remian_size % datadump;
                        putLogDebug(100097, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                    it->send_end_size = send_buffer_remian_size;
                    it->send_possible_size -= send_buffer_remian_size;
                    send_buffer_remian_size = 0;
                }
            }

            //set copyed data length
            datalen = send_buffer_end_size - send_buffer_remian_size;
            status = REALSERVER_SEND;
        }
        catch (int e)
        {
            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                        "handle_realserver_connect() : catch exception e = %d. thread id : %d.");
                formatter % e % boost::this_thread::get_id();
                putLogDebug(100098, formatter.str(), __FILE__, __LINE__ );
            }
            status = FINALIZE;
            /*------DEBUG LOG END------*/
        }
        catch (const std::exception& ex)
        {
            std::cerr << "protocol_module_sessionless::handle_realserver_connect() : exception : error = " << ex.what() << "." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_realserver_connect() : exception : error = %s. thread id : %d.");
            formatter % ex.what() % boost::this_thread::get_id();
            putLogError(100047, formatter.str(), __FILE__, __LINE__ );
            status = FINALIZE;
        }
        catch (...)
        {
            std::cerr << "protocol_module_sessionless::handle_realserver_connect() : Unknown exception." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_realserver_connect() : Unknown exception. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(100048, formatter.str(), __FILE__, __LINE__ );
            status = FINALIZE;
        }

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_realserver_connect(const boost::thread::id thread_id, "
                                    "boost::array<char,MAX_BUFFER_SIZE>& sendbuffer, "
                                    "size_t& datalen) : return_value = %d. thread id : %d.");
            formatter % status % boost::this_thread::get_id();
            putLogDebug(100099, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        return status;
    }

    //! called from after realserver connection fail
    //! @param[in]    upstream thread id
    //! @param[in]    fail realserver endpoint reference
    //! @return        session use EVENT mode.
    protocol_module_base::EVENT_TAG protocol_module_sessionless::handle_realserver_connection_fail(
        const boost::thread::id thread_id, const boost::asio::ip::tcp::endpoint & rs_endpoint)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_realserver_connection_fail(const boost::thread::id thread_id, "
                                    "const boost::asio::ip::tcp::endpoint & rs_endpoint) : "
                                    "thread_id = %d, rs_endpoint = [%s]:%d.");
            formatter % thread_id % rs_endpoint.address().to_string() % rs_endpoint.port();
            putLogDebug(100100, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        EVENT_TAG status = FINALIZE;
        thread_data_ptr session_data;
        session_thread_data_map_it session_thread_it;

        try
        {
            boost::mutex::scoped_lock slock(session_thread_data_map_mutex );

            session_thread_it = session_thread_data_map.find(thread_id);
            if (unlikely(session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL))
            {
                boost::format formatter("Invalid thread id. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogError(100049, formatter.str(), __FILE__, __LINE__ );
                throw -1;
            }

            session_data = session_thread_it->second;

            //set end flag ON
            session_data->end_flag = END_FLAG_ON;
            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                        "handle_realserver_connection_fail() : END_FLAG_ON. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogDebug(100101, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/
            status = CLIENT_DISCONNECT;
        }
        catch (int e)
        {
            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                        "handle_realserver_connection_fail() : catch exception e = %d. thread id : %d.");
                formatter % e % boost::this_thread::get_id();
                putLogDebug(100102, formatter.str(), __FILE__, __LINE__ );
            }
            status = FINALIZE;
            /*------DEBUG LOG END------*/
        }
        catch (std::exception& ex)
        {
            std::cerr << "protocol_module_sessionless::handle_realserver_connection_fail() : exception : error = " << ex.what() << "." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_realserver_connection_fail() : exception : error = %s. thread id : %d.");
            formatter % ex.what() % boost::this_thread::get_id();
            putLogError(100050, formatter.str(), __FILE__, __LINE__ );
            status = FINALIZE;
        }
        catch (...)
        {
            std::cerr << "protocol_module_sessionless::handle_realserver_connection_fail() : Unknown exception." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_realserver_connection_fail() : Unknown exception. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(100051, formatter.str(), __FILE__, __LINE__ );
            status = FINALIZE;
        }

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_realserver_connection_fail(const boost::thread::id thread_id, "
                                    "const boost::asio::ip::tcp::endpoint & rs_endpoint) : return_value = %d. thread id : %d.");
            formatter % status % boost::this_thread::get_id();
            putLogDebug(100103, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/
        return status;
    }
    //! called from after realserver send.
    //! @param[in]    upstream thread id
    //! @return        session use EVENT mode.
    protocol_module_base::EVENT_TAG protocol_module_sessionless::handle_realserver_send(
        const boost::thread::id thread_id)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_realserver_send(const boost::thread::id thread_id) : "
                                    "thread_id = %d.");
            formatter % thread_id;
            putLogDebug(100104, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/
        EVENT_TAG status = FINALIZE;
        thread_data_ptr session_data;
        session_thread_data_map_it session_thread_it;
        recive_data_map_it recive_data_it;

        try
        {
            {
                boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);

                //thread_id check
                session_thread_it = session_thread_data_map.find(thread_id);
                if (unlikely(session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL))
                {
                    boost::format formatter("Invalid thread id. thread id : %d.");
                    formatter % boost::this_thread::get_id();
                    putLogError(100052, formatter.str(), __FILE__, __LINE__ );
                    throw -1;
                }

                session_data = session_thread_it->second;
            }

            //endpoint check
            recive_data_it = session_data->recive_data_map.find(session_data->client_endpoint_tcp);
            if (unlikely(recive_data_it == session_data->recive_data_map.end()))
            {
                boost::format formatter("Invalid endpoint. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogError(100053, formatter.str(), __FILE__, __LINE__ );
                throw -1;
            }

            recive_data& recv_data = recive_data_it->second;

            send_status_it it = recv_data.send_status_list.begin();
            send_status_it it_end = recv_data.send_status_list.end();

            //status list check
            it = std::adjacent_find(it, it_end, data_send_list_incorrect());
            if (unlikely(it != it_end))
            {
                boost::format formatter("Sending possible data is invalid. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogError(100054, formatter.str(), __FILE__, __LINE__ );
                throw -1;
            }

            //status list check
            it = recv_data.send_status_list.begin();
            it = std::find_if(it, it_end, data_send_ok());
            if (unlikely(it == it_end))
            {
                boost::format formatter("Sending possible data is not existed. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogError(100055, formatter.str(), __FILE__, __LINE__ );
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
                //can recive from client continue
                if (it->send_rest_size > 0)
                {
                    //change status from SEND_OK to SEND_CONTINUE
                    it->status = SEND_CONTINUE;
                }
                //can not recive from client continue
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
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                        "handle_realserver_send() : catch exception e = %d. thread id : %d.");
                formatter % e % boost::this_thread::get_id();
                putLogDebug(100105, formatter.str(), __FILE__, __LINE__ );
            }
            status = FINALIZE;
            /*------DEBUG LOG END------*/
        }
        catch (const std::exception& ex)
        {
            std::cerr << "protocol_module_sessionless::handle_realserver_send() : exception : error = " << ex.what() << "." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_realserver_send() : exception : error = %s. thread id : %d.");
            formatter % ex.what() % boost::this_thread::get_id();
            putLogError(100056, formatter.str(), __FILE__, __LINE__ );

            status = FINALIZE;
        }
        catch (...)
        {
            std::cerr << "protocol_module_sessionless::handle_realserver_send() : Unknown exception." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_realserver_send() : Unknown exception. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(100057, formatter.str(), __FILE__, __LINE__ );
            status = FINALIZE;
        }

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_realserver_send(const boost::thread::id thread_id) : return_value = %d. thread id : %d.");
            formatter % status % boost::this_thread::get_id();
            putLogDebug(100106, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        return status;
    }

    //! called from after sorryserver select
    //! @param[in]    upstream thread id
    //! @param[in]    sorryserver endpiont reference
    //! @return        session use EVENT mode.
    protocol_module_base::EVENT_TAG protocol_module_sessionless::handle_sorryserver_select(
        const boost::thread::id thread_id, boost::asio::ip::tcp::endpoint& sorry_endpoint)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_sorryserver_select(const boost::thread::id thread_id, "
                                    "boost::asio::ip::tcp::endpoint& sorry_endpoint) : "
                                    "thread_id = %d, sorry_endpoint = [%s]:%d.");
            formatter % thread_id % sorry_endpoint.address().to_string() % sorry_endpoint.port();
            putLogDebug(100107, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/
        EVENT_TAG status = FINALIZE;
        boost::asio::ip::tcp::endpoint client_endpoint;

        thread_data_ptr session_data;
        session_thread_data_map_it session_thread_it;
        recive_data_map_it recive_data_it;

        try
        {
            boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);

            session_thread_it = session_thread_data_map.find(thread_id);
            if (unlikely(session_thread_it == session_thread_data_map.end() || session_thread_it->second== NULL))
            {
                boost::format formatter("Invalid thread id. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogError(100058, formatter.str(), __FILE__, __LINE__ );
                throw -1;
            }

            session_data = session_thread_it->second;
            //set sorry_endpoint
            session_data->target_endpoint = sorry_endpoint;

            //endpoint check
            recive_data_it = session_data->recive_data_map.find(session_data->client_endpoint_tcp);
            if (unlikely(recive_data_it == session_data->recive_data_map.end()))
            {
                boost::format formatter("Invalid endpoint. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogError(100059, formatter.str(), __FILE__, __LINE__ );
                throw -1;
            }

            recive_data& recv_data = recive_data_it->second;

            send_status_it it = recv_data.send_status_list.begin();
            send_status_it it_end = recv_data.send_status_list.end();

            it = find_if(it, it_end, data_send_possible());
            if (it != it_end)
            {
                status = SORRYSERVER_CONNECT;
            }
            else
            {
                status = CLIENT_RECV;
            }

        }
        catch (int e)
        {
            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                        "handle_sorryserver_select() : catch exception e = %d. thread id : %d.");
                formatter % e % boost::this_thread::get_id();
                putLogDebug(100108, formatter.str(), __FILE__, __LINE__ );
            }
            status = FINALIZE;
            /*------DEBUG LOG END------*/
        }
        catch (const std::bad_alloc& ex)
        {
            std::cerr << "protocol_module_sessionless::handle_sorryserver_select() : exception : Could not allocate memory." << std::endl;
            boost::format formatter("Could not allocate memory. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(100060, formatter.str(), __FILE__, __LINE__ );
            status = FINALIZE;
        }
        catch (const std::exception& ex)
        {
            std::cerr << "protocol_module_sessionless::handle_sorryserver_select() : exception : error = " << ex.what() << "." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_sorryserver_select() : exception : error = %s. thread id : %d.");
            formatter % ex.what() % boost::this_thread::get_id();
            putLogError(100061, formatter.str(), __FILE__, __LINE__ );
            status = FINALIZE;
        }
        catch (...)
        {
            std::cerr << "protocol_module_sessionless::handle_sorryserver_select() : Unknown exception." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_sorryserver_select() : Unknown exception. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(100062, formatter.str(), __FILE__, __LINE__ );
            status = FINALIZE;
        }

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format
            formatter(
                "out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                "handle_sorryserver_select(const boost::thread::id thread_id, boost::asio::ip::tcp::endpoint& sorry_endpoint) : return_value = %d."
                " thread id : %d.");
            formatter % status % boost::this_thread::get_id();
            putLogDebug(100109, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/
        return status;
    }

    //! called from after sorryserver connect
    //!    @param[in]    upstream thread id
    //! @param[out]    send buffer reference.
    //! @param[out]    send length
    //! @return        session use EVENT mode.
    protocol_module_base::EVENT_TAG protocol_module_sessionless::handle_sorryserver_connect(
        const boost::thread::id thread_id, boost::array<char, MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_sorryserver_connect(const boost::thread::id thread_id, "
                                    "boost::array<char, MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen) : "
                                    "thread_id = %d.");
            formatter % thread_id;
            putLogDebug(100110, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/
        EVENT_TAG status = FINALIZE;
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
        thread_data_ptr session_data;
        session_thread_data_map_it session_thread_it;
        recive_data_map_it recive_data_it;

        try
        {
            {
                boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);

                //thread id check
                session_thread_it = session_thread_data_map.find(thread_id);
                if (unlikely(session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL))
                {
                    boost::format formatter("Invalid thread id. thread id : %d.");
                    formatter % boost::this_thread::get_id();
                    putLogError(100063, formatter.str(), __FILE__, __LINE__ );
                    throw -1;
                }

                session_data = session_thread_it->second;
            }

            //endpoint check
            recive_data_it = session_data->recive_data_map.find(session_data->client_endpoint_tcp);
            if (unlikely(recive_data_it
                         == session_data->recive_data_map.end()))
            {
                boost::format formatter("Invalid endpoint. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogError(100064, formatter.str(), __FILE__, __LINE__ );
                throw -1;
            }

            //recive_buffer pointer check
            recive_data& recv_data = recive_data_it->second;
            if (unlikely(recv_data.recive_buffer == NULL))
            {
                boost::format formatter("Invalid pointer. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogError(100065, formatter.str(), __FILE__, __LINE__ );
                throw -1;
            }

            //send list check
            send_status_it it = recv_data.send_status_list.begin();
            send_status_it it_end = recv_data.send_status_list.end();

            it = find_if(it, it_end, data_send_possible());
            if (unlikely(it == it_end))
            {
                boost::format formatter("Sending possible data is not existed. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogError(100066, formatter.str(), __FILE__, __LINE__ );
                throw -1;
            }

            //send buffer rest size initialization
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
                    if (strlen(sorry_uri.data()) > 0)
                    {
                        ret = find_uri(recv_data.recive_buffer + it->send_offset, it->send_possible_size, url_offset,
                                       url_offset_len);
                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                    "handle_sorryserver_connect() : call find_uri : "
                                                    "return_value = %d. thread id : %d.");
                            formatter % static_cast<int>(ret) % boost::this_thread::get_id();
                            putLogDebug(100111, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        //search http header result is OK
                        if (ret)
                        {
                            //edit sorry_uri, put it to edata.data
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
                    }

                    if (forwarded_for == FORWARDED_FOR_ON)
                    {
                        //search X-Forwarded-For header
                        ret = find_http_header(recv_data.recive_buffer + it->send_offset, it->send_possible_size,
                                               str_forword_for.c_str(), header_offset, header_offset_len);
                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                    "handle_sorryserver_connect() : call find_http_header : "
                                                    "return_value = %d. thread id : %d.");
                            formatter % static_cast<int>(ret) % boost::this_thread::get_id();
                            putLogDebug(100112, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/

                        //search http header result is OK
                        if (ret)
                        {
                            //edit X-Forwarded-For header, put it to edata.data
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
                        //search http header result is NG
                        else
                        {
                            //search whole http header, get whole http header's offset and length
                            ret = find_http_header(recv_data.recive_buffer + it->send_offset, it->send_possible_size, "",
                                                   header_offset, header_offset_len);
                            /*-------- DEBUG LOG --------*/
                            if (unlikely(LOG_LV_DEBUG == getloglevel()))
                            {
                                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                        "handle_sorryserver_connect() : call find_http_header : "
                                                        "return_value = %d. thread id : %d.");
                                formatter % static_cast<int>(ret) % boost::this_thread::get_id();
                                putLogDebug(100113, formatter.str(), __FILE__, __LINE__ );
                            }
                            /*------DEBUG LOG END------*/
                            if (!ret)
                            {
                                boost::format formatter("find_http_header() function failure. thread id : %d.");
                                formatter % boost::this_thread::get_id();
                                putLogError(100067, formatter.str(), __FILE__, __LINE__ );
                                /*-------- DEBUG LOG --------*/
                                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                                {
                                    boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                            "handle_sorryserver_connect(const boost::thread::id thread_id, "
                                                            "boost::array<char,MAX_BUFFER_SIZE>& sendbuffer, "
                                                            "size_t& datalen) : return_value = %d. thread id : %d.");
                                    formatter % FINALIZE % boost::this_thread::get_id();
                                    putLogDebug(100114, formatter.str(), __FILE__, __LINE__ );
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
                            //save new X-Forwarded-For header length
                            edata.data_size = edata.data.size();
                            //save old X-Forwarded-For header length
                            edata.replace_size = 0;
                        }

                        //add to edit_data_list
                        it->edit_data_list.push_back(edata);
                    }
                }

                /*-------- DEBUG LOG --------*/
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                            "handle_sorryserver_connect() : Copy data loop start. thread id : %d.");
                    formatter % boost::this_thread::get_id();
                    putLogDebug(100115, formatter.str(), __FILE__, __LINE__ );
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
                                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                                {
                                    std::string datadump;
                                    dump_memory(recv_data.recive_buffer + it->send_offset + it->send_end_size,
                                                copy_size, datadump);
                                    boost::format formatter(
                                        "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                        "handle_sorryserver_connect() : before memcpy (data dump) : "
                                        "data begin = %d, data_size = %d, data = %s");
                                    formatter % (it->send_offset + it->send_end_size)
                                    % copy_size % datadump;
                                    putLogDebug(100116, formatter.str(), __FILE__, __LINE__ );
                                }
                                /*------DEBUG LOG END------*/
                                memcpy(sendbuffer.data() + send_buffer_end_size - send_buffer_remian_size,
                                       recv_data.recive_buffer + it->send_offset + it->send_end_size, copy_size);
                                /*-------- DEBUG LOG --------*/
                                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                                {
                                    std::string datadump;
                                    dump_memory(sendbuffer.data() + send_buffer_end_size - send_buffer_remian_size,
                                                copy_size, datadump);
                                    boost::format formatter(
                                        "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                        "handle_sorryserver_connect() : after memcpy (data dump) : "
                                        "data begin = %d, data_size = %d, data = %s");
                                    formatter % (send_buffer_end_size - send_buffer_remian_size)
                                    % copy_size % datadump;
                                    putLogDebug(100117, formatter.str(), __FILE__, __LINE__ );
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
                                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                                {
                                    std::string datadump;
                                    dump_memory(recv_data.recive_buffer + it->send_offset + it->send_end_size,
                                                copy_size, datadump);
                                    boost::format formatter(
                                        "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                        "handle_sorryserver_connect() : before memcpy (data dump) : "
                                        "data begin = %d, data_size = %d, data = %s");
                                    formatter % (it->send_offset + it->send_end_size)
                                    % copy_size % datadump;
                                    putLogDebug(100118, formatter.str(), __FILE__, __LINE__ );
                                }
                                /*------DEBUG LOG END------*/
                                //copy data from recive_buffer to sendbuffer by send buffer rest size
                                memcpy(sendbuffer.data() + send_buffer_end_size - send_buffer_remian_size,
                                       recv_data.recive_buffer + it->send_offset + it->send_end_size,
                                       copy_size);
                                /*-------- DEBUG LOG --------*/
                                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                                {
                                    std::string datadump;
                                    dump_memory(sendbuffer.data() + send_buffer_end_size - send_buffer_remian_size,
                                                copy_size, datadump);
                                    boost::format formatter(
                                        "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                        "handle_sorryserver_connect() : after memcpy (data dump) : "
                                        "data begin = %d, data_size = %d, data = %s");
                                    formatter % (send_buffer_end_size - send_buffer_remian_size)
                                    % copy_size % datadump;
                                    putLogDebug(100119, formatter.str(), __FILE__, __LINE__ );
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
                            if (unlikely(LOG_LV_DEBUG == getloglevel()))
                            {
                                std::string datadump;
                                dump_memory(recv_data.recive_buffer + it->send_offset + it->send_end_size,
                                            copy_size, datadump);

                                boost::format formatter(
                                    "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_sorryserver_connect() : before memcpy (data dump) : "
                                    "data begin = %d, data_size = %d, data = %s");
                                formatter % (it->send_offset + it->send_end_size)
                                % copy_size % datadump;
                                putLogDebug(100120, formatter.str(), __FILE__, __LINE__ );
                            }
                            /*------DEBUG LOG END------*/
                            memcpy(sendbuffer.data() + send_buffer_end_size - send_buffer_remian_size,
                                   recv_data.recive_buffer + it->send_offset + it->send_end_size, copy_size);
                            /*-------- DEBUG LOG --------*/
                            if (unlikely(LOG_LV_DEBUG == getloglevel()))
                            {
                                std::string datadump;
                                dump_memory(sendbuffer.data() + send_buffer_end_size - send_buffer_remian_size,
                                            copy_size, datadump);
                                boost::format formatter(
                                    "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_sorryserver_connect() : after memcpy (data dump) : "
                                    "data begin = %d, data_size = %d, data = %s");
                                formatter % (send_buffer_end_size - send_buffer_remian_size)
                                % copy_size % datadump;
                                putLogDebug(100121, formatter.str(), __FILE__, __LINE__ );
                            }
                            /*------DEBUG LOG END------*/
                            it->send_end_size += copy_size;
                            it->send_possible_size -= copy_size;
                            send_buffer_remian_size -= copy_size;

                            //there is remain buffer for copy X-Forwarded-For/url
                            if (send_buffer_remian_size >= edit_min->data_size)
                            {
                                /*-------- DEBUG LOG --------*/
                                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                                {
                                    std::string datadump;
                                    dump_memory(edit_min->data.c_str(),
                                                edit_min->data_size, datadump);
                                    boost::format formatter(
                                        "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                        "handle_sorryserver_connect() : before memcpy (data dump) : "
                                        "data begin = 0, data_size = %d, data = %s");
                                    formatter % edit_min->data_size % datadump;
                                    putLogDebug(100122, formatter.str(), __FILE__, __LINE__ );
                                }
                                /*------DEBUG LOG END------*/
                                //copy X-Forwarded-For/uri
                                memcpy(sendbuffer.data() + send_buffer_end_size - send_buffer_remian_size,
                                       edit_min->data.c_str(), edit_min->data_size);
                                /*-------- DEBUG LOG --------*/
                                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                                {
                                    std::string datadump;
                                    dump_memory(sendbuffer.data() + send_buffer_end_size - send_buffer_remian_size,
                                                edit_min->data_size, datadump);

                                    boost::format formatter(
                                        "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                        "handle_sorryserver_connect() : after memcpy (data dump) : "
                                        "data begin = %d, data_size = %d, data = %s");
                                    formatter % (send_buffer_end_size - send_buffer_remian_size)
                                    % edit_min->data_size % datadump;
                                    putLogDebug(100123, formatter.str(), __FILE__, __LINE__ );
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
                            if (unlikely(LOG_LV_DEBUG == getloglevel()))
                            {
                                std::string datadump;
                                dump_memory(sendbuffer.data() + send_buffer_end_size - send_buffer_remian_size,
                                            copy_size, datadump);

                                boost::format formatter(
                                    "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_sorryserver_connect() : before memcpy (data dump) : "
                                    "data begin = %d, data_size = %d, data = %s");
                                formatter % (it->send_offset + it->send_end_size)
                                % copy_size % datadump;
                                putLogDebug(100124, formatter.str(), __FILE__, __LINE__ );
                            }
                            /*------DEBUG LOG END------*/
                            //copy data as large as possible
                            memcpy(sendbuffer.data() + send_buffer_end_size - send_buffer_remian_size,
                                   recv_data.recive_buffer + it->send_offset + it->send_end_size, send_buffer_remian_size);
                            /*-------- DEBUG LOG --------*/
                            if (unlikely(LOG_LV_DEBUG == getloglevel()))
                            {
                                std::string datadump;
                                dump_memory(sendbuffer.data() + send_buffer_end_size - send_buffer_remian_size,
                                            copy_size, datadump);

                                boost::format formatter(
                                    "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_sorryserver_connect() : after memcpy (data dump) : "
                                    "data begin = %d, data_size = %d, data = %s");
                                formatter % (send_buffer_end_size - send_buffer_remian_size)
                                % copy_size % datadump;
                                putLogDebug(100125, formatter.str(), __FILE__, __LINE__ );
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
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                            "handle_sorryserver_connect() : Copy data loop end. thread id : %d.");
                    formatter % boost::this_thread::get_id();
                    putLogDebug(100126, formatter.str(), __FILE__, __LINE__ );
                }
                /*------DEBUG LOG END------*/
            }
            //edit_division flag is off
            else
            {
                //copy data as large as possible
                //send_possible_size is larger
                if (send_buffer_remian_size >= it->send_possible_size)
                {
                    copy_size = it->send_possible_size;
                    /*-------- DEBUG LOG --------*/
                    if (unlikely(LOG_LV_DEBUG == getloglevel()))
                    {
                        std::string datadump;
                        dump_memory(recv_data.recive_buffer+ it->send_offset, copy_size, datadump);

                        boost::format formatter(
                            "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                            "handle_sorryserver_connect() : before memcpy (data dump) : "
                            "data begin = %d, data_size = %d, data = %s");
                        formatter % it->send_offset
                        % copy_size % datadump;
                        putLogDebug(100127, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                    //copy data by send_possible size
                    memcpy(sendbuffer.data(), recv_data.recive_buffer
                           + it->send_offset, copy_size);
                    /*-------- DEBUG LOG --------*/
                    if (unlikely(LOG_LV_DEBUG == getloglevel()))
                    {
                        std::string datadump;
                        dump_memory(sendbuffer.data(), copy_size, datadump);

                        boost::format formatter(
                            "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                            "handle_sorryserver_connect() : after memcpy (data dump) : "
                            "data begin = 0, data_size = %d, data = %s");
                        formatter % copy_size % datadump;
                        putLogDebug(100128, formatter.str(), __FILE__, __LINE__ );
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
                    if (unlikely(LOG_LV_DEBUG == getloglevel()))
                    {
                        std::string datadump;
                        dump_memory(recv_data.recive_buffer+ it->send_offset, send_buffer_remian_size, datadump);

                        boost::format formatter(
                            "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                            "handle_sorryserver_connect() : before memcpy (data dump) : "
                            "data begin = %d, data_size = %d, data = %s");
                        formatter % it->send_offset
                        % send_buffer_remian_size % datadump;
                        putLogDebug(100129, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                    //copy data by buffer rest size
                    memcpy(sendbuffer.data(), recv_data.recive_buffer
                           + it->send_offset, send_buffer_remian_size);
                    /*-------- DEBUG LOG --------*/
                    if (unlikely(LOG_LV_DEBUG == getloglevel()))
                    {
                        std::string datadump;
                        dump_memory(sendbuffer.data(), send_buffer_remian_size, datadump);

                        boost::format formatter(
                            "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                            "handle_sorryserver_connect() : after memcpy (data dump) : "
                            "data begin = 0, data_size = %d, data = %s");
                        formatter % send_buffer_remian_size % datadump;
                        putLogDebug(100130, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                    it->send_end_size = send_buffer_remian_size;
                    it->send_possible_size -= send_buffer_remian_size;
                    send_buffer_remian_size = 0;
                }
            }

            //set copyed data length
            datalen = send_buffer_end_size - send_buffer_remian_size;

            status = SORRYSERVER_SEND;

        }
        catch (int e)
        {
            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                        "handle_sorryserver_connect() : catch exception e = %d. thread id : %d.");
                formatter % e % boost::this_thread::get_id();
                putLogDebug(100131, formatter.str(), __FILE__, __LINE__ );
            }
            status = FINALIZE;
            /*------DEBUG LOG END------*/
        }
        catch (const std::exception& ex)
        {
            std::cerr << "protocol_module_sessionless::handle_sorryserver_connect() : exception : error = " << ex.what() << "." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_sorryserver_connect() : exception : error = %s. thread id : %d.");
            formatter % ex.what() % boost::this_thread::get_id();
            putLogError(100068, formatter.str(), __FILE__, __LINE__ );
            status = FINALIZE;
        }
        catch (...)
        {
            std::cerr << "protocol_module_sessionless::handle_sorryserver_connect() : Unknown exception." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_sorryserver_connect() : Unknown exception. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(100069, formatter.str(), __FILE__, __LINE__ );
            status = FINALIZE;
        }

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_sorryserver_connect(const boost::thread::id thread_id, "
                                    "boost::array<char,MAX_BUFFER_SIZE>& sendbuffer, "
                                    "size_t& datalen) : return_value = %d. thread id : %d.");
            formatter % status % boost::this_thread::get_id();
            putLogDebug(100132, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        return status;
    }

    //! called from after sorryserver connection fail
    //! @param[in]    upstream thread id
    //! @param[in]    sorryserver endpoint reference.
    //! @return        session use EVENT mode.
    protocol_module_base::EVENT_TAG protocol_module_sessionless::handle_sorryserver_connection_fail(
        const boost::thread::id thread_id, const boost::asio::ip::tcp::endpoint & sorry_endpoint)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_sorryserver_connection_fail(const boost::thread::id thread_id, "
                                    "const boost::asio::ip::tcp::endpoint & sorry_endpoint) : "
                                    "thread_id = %d, sorry_endpoint = [%s]:%d.");
            formatter % thread_id % sorry_endpoint.address().to_string() % sorry_endpoint.port();
            putLogDebug(100133, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        EVENT_TAG status = FINALIZE;
        thread_data_ptr session_data;
        session_thread_data_map_it session_thread_it;

        try
        {
            boost::mutex::scoped_lock slock(session_thread_data_map_mutex );

            session_thread_it = session_thread_data_map.find(thread_id);
            if (unlikely(session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL))
            {
                boost::format formatter("Invalid thread id. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogError(100070, formatter.str(), __FILE__, __LINE__ );
                throw -1;
            }

            session_data = session_thread_it->second;

            //set end flag on
            session_data->end_flag = END_FLAG_ON;
            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                        "handle_sorryserver_connection_fail() : END_FLAG_ON. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogDebug(100134, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/

            status = CLIENT_DISCONNECT;
        }
        catch (int e)
        {
            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                        "handle_sorryserver_connection_fail() : catch exception e = %d. thread id : %d.");
                formatter % e % boost::this_thread::get_id();
                putLogDebug(100135, formatter.str(), __FILE__, __LINE__ );
            }
            status = FINALIZE;
            /*------DEBUG LOG END------*/
        }
        catch (const std::exception& ex)
        {
            std::cerr << "protocol_module_sessionless::handle_sorryserver_connection_fail() : exception : error=" << ex.what() << "." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_sorryserver_connection_fail() : exception : error = %s. thread id : %d.");
            formatter % ex.what() % boost::this_thread::get_id();
            putLogError(100071, formatter.str(), __FILE__, __LINE__ );
            status = FINALIZE;
        }
        catch (...)
        {
            std::cerr << "protocol_module_sessionless::handle_sorryserver_connection_fail() : Unknown exception." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_sorryserver_connection_fail() : Unknown exception. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(100072, formatter.str(), __FILE__, __LINE__ );
            status = FINALIZE;
        }

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format
            formatter(
                "out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                "handle_sorryserver_connection_fail( const boost::thread::id thread_id, const boost::asio::ip::tcp::endpoint & sorry_endpoint) : return_value = %d."
                " thread id : %d.");
            formatter % status % boost::this_thread::get_id();
            putLogDebug(100136, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/
        return status;
    }

    //! called from after sorryserver send
    //! @param[in]    upstream thread id
    //! @return        session use EVENT mode.
    protocol_module_base::EVENT_TAG protocol_module_sessionless::handle_sorryserver_send(
        const boost::thread::id thread_id)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_sorryserver_send(const boost::thread::id thread_id) : "
                                    "thread_id = %d.");
            formatter % thread_id;
            putLogDebug(100137, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/
        EVENT_TAG status = FINALIZE;
        thread_data_ptr session_data;
        session_thread_data_map_it session_thread_it;
        recive_data_map_it recive_data_it;

        try
        {
            {
                boost::mutex::scoped_lock slock(session_thread_data_map_mutex );

                //thread_id check
                session_thread_it = session_thread_data_map.find(thread_id);
                if (unlikely(session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL))
                {
                    boost::format formatter("Invalid thread id. thread id : %d.");
                    formatter % boost::this_thread::get_id();
                    putLogError(100073, formatter.str(), __FILE__, __LINE__ );
                    throw -1;
                }

                session_data = session_thread_it->second;
            }

            //endpoint check
            recive_data_it = session_data->recive_data_map.find(session_data->client_endpoint_tcp);
            if (unlikely(recive_data_it
                         == session_data->recive_data_map.end()))
            {
                boost::format formatter("Invalid endpoint. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogError(100074, formatter.str(), __FILE__, __LINE__ );
                throw -1;
            }

            recive_data& recv_data = recive_data_it->second;

            send_status_it it = recv_data.send_status_list.begin();
            send_status_it it_end = recv_data.send_status_list.end();
            //status list check
            it = std::adjacent_find(it, it_end, data_send_list_incorrect());
            if (unlikely(it != it_end))
            {
                boost::format formatter("Sending possible data is invalid. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogError(100075, formatter.str(), __FILE__, __LINE__ );
                throw -1;
            }

            //status list check
            it = recv_data.send_status_list.begin();
            it = find_if(it, it_end, data_send_ok());
            if (unlikely(it == it_end))
            {
                boost::format formatter("Sending possible data is not existed. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogError(100076, formatter.str(), __FILE__, __LINE__ );
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
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                        "handle_sorryserver_send() : catch exception e = %d. thread id : %d.");
                formatter % e % boost::this_thread::get_id();
                putLogDebug(100138, formatter.str(), __FILE__, __LINE__ );
            }
            status = FINALIZE;
            /*------DEBUG LOG END------*/
        }
        catch (const std::exception& ex)
        {
            std::cerr << "protocol_module_sessionless::handle_sorryserver_send() : exception : error = " << ex.what() << "." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_sorryserver_send() : exception : error = %s. thread id : %d.");
            formatter % ex.what() % boost::this_thread::get_id();
            putLogError(100077, formatter.str(), __FILE__, __LINE__ );

            status = FINALIZE;
        }
        catch (...)
        {
            std::cerr << "protocol_module_sessionless::handle_sorryserver_send() : Unknown exception." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_sorryserver_send() : Unknown exception. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(100078, formatter.str(), __FILE__, __LINE__ );
            status = FINALIZE;
        }

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_sorryserver_send(const boost::thread::id thread_id) : return_value = %d. thread id : %d.");
            formatter % status % boost::this_thread::get_id();
            putLogDebug(100139, formatter.str(), __FILE__, __LINE__ );
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
    protocol_module_base::EVENT_TAG protocol_module_sessionless::handle_realserver_recv(
        const boost::thread::id thread_id, const boost::asio::ip::udp::endpoint& rs_endpoint, const boost::array<char,
        MAX_BUFFER_SIZE>& recvbuffer, const size_t recvlen)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("in/out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_realserver_recv(const boost::thread::id thread_id, "
                                    "const boost::asio::ip::udp::endpoint& rs_endpoint, "
                                    "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                    "const size_t recvlen) : "
                                    "return_value = %d. thread id : %d.");
            formatter % STOP % boost::this_thread::get_id();
            putLogDebug(100140, formatter.str(), __FILE__, __LINE__ );
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
    protocol_module_base::EVENT_TAG protocol_module_sessionless::handle_realserver_recv(
        const boost::thread::id thread_id, const boost::asio::ip::tcp::endpoint& rs_endpoint, const boost::array<char,
        MAX_BUFFER_SIZE>& recvbuffer, const size_t recvlen)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            size_t buffer_size = recvbuffer.size() < recvlen ? recvbuffer.size() : recvlen;
            std::string buffer;
            dump_memory(recvbuffer.data(), buffer_size, buffer);
            boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_realserver_recv(const boost::thread::id thread_id, "
                                    "const boost::asio::ip::tcp::endpoint& rs_endpoint, "
                                    "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                    "const size_t recvlen) : thread_id = %d, rs_endpoint = [%s]:%d, recvbuffer = %s, recvlen = %d.");
            formatter % thread_id % rs_endpoint.address().to_string() % rs_endpoint.port()
            % buffer % recvlen;
            putLogDebug(100141, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        EVENT_TAG status = FINALIZE;
        size_t data_remain_start = 0;
        size_t data_remain_size = 0;
        size_t request_data_remain_size = 0;
        size_t header_offset = 0;
        size_t header_offset_len = 0;
        size_t content_length_header_offset = 0;
        size_t content_length_header_len = 0;
        size_t content_len_value = 0;
        size_t pos = 0;
        size_t buffer_size = 0;
        const size_t cr_lf_len = strlen("\r\n\r\n");
        thread_data_ptr session_data;
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
            std::cerr << "protocol_module_sessionless::handle_realserver_recv() : Data size bigger than buffer size." << std::endl;
            boost::format formatter("Data size bigger than buffer size. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(100079, formatter.str(), __FILE__, __LINE__ );
            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                        "handle_realserver_recv(const boost::thread::id thread_id, "
                                        "const boost::asio::ip::tcp::endpoint& rs_endpoint, "
                                        "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                        "const size_t recvlen) : return_value = %d. thread id : %d.");
                formatter % FINALIZE % boost::this_thread::get_id();
                putLogDebug(100142, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/
            return FINALIZE;
        }

        try
        {
            {
                boost::mutex::scoped_lock slock(session_thread_data_map_mutex );

                session_thread_it = session_thread_data_map.find(thread_id);
                if (unlikely(session_thread_it == session_thread_data_map.end()
                             ||session_thread_it->second == NULL))
                {
                    boost::format formatter("Invalid thread id. thread id : %d.");
                    formatter % boost::this_thread::get_id();
                    putLogError(100080, formatter.str(), __FILE__, __LINE__ );
                    throw -1;
                }

                session_data = session_thread_it->second;
            }

            recive_data_it = session_data->recive_data_map.find(rs_endpoint);
            if (recive_data_it == session_data->recive_data_map.end())
            {
                recive_data recv_data;
                session_data->recive_data_map[rs_endpoint] = recv_data;
            }

            session_data->target_endpoint = rs_endpoint;

            recive_data& recv_data = session_data->recive_data_map[rs_endpoint];

            send_status_it it = recv_data.send_status_list.begin();
            send_status_it it_end = recv_data.send_status_list.end();

            //status list check
            it = std::find_if(it, it_end, data_send_ok());
            if (unlikely(it != it_end))
            {
                boost::format formatter("Sending data is not correct. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogError(100081, formatter.str(), __FILE__, __LINE__ );
                throw -1;
            }

            //status list check
            it = recv_data.send_status_list.begin();
            it = std::adjacent_find(it, it_end, data_send_repeated());
            if (unlikely(it != it_end))
            {
                boost::format formatter("Sending data is not correct. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogError(100082, formatter.str(), __FILE__, __LINE__ );
                throw -1;
            }

            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                std::string datadump;
                boost::format formatter("\nitem[%d] : status = %d, send_end_size = %d, "
                                        "send_rest_size = %d, send_possible_size = %d, "
                                        "send_offset = %d, unsend_size = %d, edit_division = %d.");
                int i = 0;
                for (it = recv_data.send_status_list.begin();
                        it != recv_data.send_status_list.end();
                        ++it, ++i)
                {
                    formatter % i % it->status % it->send_end_size
                    % it->send_rest_size % it->send_possible_size
                    % it->send_offset % it->unsend_size % it->edit_division;
                    datadump += formatter.str();
                }

                formatter.parse("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_realserver_recv() : send status list dump : send status list size = %d.%s");

                formatter % recv_data.send_status_list.size() % datadump;
                putLogDebug(100143, formatter.str(), __FILE__, __LINE__ );
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
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                std::string datadump;
                boost::format formatter("\nitem[%d] : status = %d, send_end_size = %d, "
                                        "send_rest_size = %d, send_possible_size = %d, "
                                        "send_offset = %d, unsend_size = %d, edit_division = %d.");
                int i = 0;
                for (it = recv_data.send_status_list.begin();
                        it != recv_data.send_status_list.end();
                        ++it, ++i)
                {
                    formatter % i % it->status % it->send_end_size
                    % it->send_rest_size % it->send_possible_size
                    % it->send_offset % it->unsend_size % it->edit_division;
                    datadump += formatter.str();
                }

                formatter.parse("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_realserver_recv() : send status list dump : send status list size = %d.%s");

                formatter % recv_data.send_status_list.size() % datadump;
                putLogDebug(100144, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/
            //recive buffer process
            //buffer rest size < request size
            if (recv_data.recive_buffer_rest_size < recvlen)
            {
                //buffer max size < remain size + request size
                //buffer is need reallocate
                if (recv_data.recive_buffer_max_size < data_remain_size + recvlen)
                {
                    //the buffer's size that will be allocated is exceed the upper limit value
                    if (MAX_SESSIONLESS_MODULE_BUFFER_SIZE < data_remain_size + recvlen)
                    {
                        std::cerr << "protocol_module_sessionless::handle_realserver_recv() : the buffer's size that will be allacted is exceed the upper limit value." << std::endl;
                        boost::format formatter("The buffer's size that will be allocated is exceed the upper limit value. thread id : %d.");
                        formatter % boost::this_thread::get_id();
                        putLogError(100083, formatter.str(), __FILE__, __LINE__ );

                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                    "handle_realserver_recv(const boost::thread::id thread_id, "
                                                    "const boost::asio::ip::tcp::endpoint& rs_endpoint, "
                                                    "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                                    "const size_t recvlen) : return_value = %d. thread id : %d.");
                            formatter % FINALIZE % boost::this_thread::get_id();
                            putLogDebug(100145, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/

                        return FINALIZE;
                    }
                    //recive_buffer1's memory allocate and initialization
                    buffer_size = (data_remain_size + recvlen) > MAX_BUFFER_SIZE ? (data_remain_size + recvlen) : MAX_BUFFER_SIZE;
                    buffer1 = new char[buffer_size];
                    /*-------- DEBUG LOG --------*/
                    if (unlikely(LOG_LV_DEBUG == getloglevel()))
                    {
                        boost::format formatter("new : address = &(%d), size = %lu.");
                        formatter % static_cast<void*>(buffer1) % buffer_size;
                        putLogDebug(100146, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*-------- DEBUG LOG --------*/
                    memset(buffer1, 0, buffer_size);
                    //recive_buffer2's memory allocate and initialization
                    buffer2 = new char[buffer_size];
                    /*-------- DEBUG LOG --------*/
                    if (unlikely(LOG_LV_DEBUG == getloglevel()))
                    {
                        boost::format formatter("new : address = &(%d), size = %lu.");
                        formatter % static_cast<void*>(buffer2) % buffer_size;
                        putLogDebug(100147, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*-------- DEBUG LOG END--------*/
                    memset(buffer2, 0, buffer_size);

                    /*-------- DEBUG LOG --------*/
                    if (unlikely(LOG_LV_DEBUG == getloglevel()))
                    {
                        std::string datadump;
                        dump_memory(recv_data.recive_buffer + data_remain_start, data_remain_size, datadump);

                        boost::format formatter(
                            "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                            "handle_realserver_recv() : before memcpy (data dump) : "
                            "data begin = %d, data_size = %d, data = %s");
                        formatter % data_remain_start % data_remain_size % datadump;
                        putLogDebug(100148, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                    //copy data from old buffer to new buffer
                    memcpy(buffer1, recv_data.recive_buffer + data_remain_start, data_remain_size);
                    /*-------- DEBUG LOG --------*/
                    if (unlikely(LOG_LV_DEBUG == getloglevel()))
                    {
                        std::string datadump;
                        dump_memory(buffer1, data_remain_size, datadump);

                        boost::format formatter(
                            "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                            "handle_realserver_recv() : after memcpy (data dump) : "
                            "data begin = 0, data_size = %d, data = %s");
                        formatter % data_remain_size % datadump;
                        putLogDebug(100149, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/

                    /*-------- DEBUG LOG --------*/
                    if (unlikely(LOG_LV_DEBUG == getloglevel()))
                    {
                        std::string datadump;
                        dump_memory(recvbuffer.data(), recvlen, datadump);
                        boost::format formatter(
                            "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                            "handle_realserver_recv() : before memcpy (data dump) : "
                            "data begin = 0, data_size = %d, data = %s");
                        formatter % recvlen % datadump;
                        putLogDebug(100150, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                    memcpy(buffer1 + data_remain_size, recvbuffer.data(), recvlen);
                    /*-------- DEBUG LOG --------*/
                    if (unlikely(LOG_LV_DEBUG == getloglevel()))
                    {
                        std::string datadump;
                        dump_memory(buffer1 + data_remain_size, recvlen, datadump);
                        boost::format formatter(
                            "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                            "handle_realserver_recv() : after memcpy (data dump) : "
                            "data begin = %d, data_size = %d, data = %s");
                        formatter % data_remain_size % recvlen % datadump;
                        putLogDebug(100151, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                    //free old buffer1 and old buffer2
                    if (recv_data.recive_buffer1 != NULL)
                    {
                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            boost::format formatter("delete : address = &(%d).");
                            formatter % static_cast<void*>(recv_data.recive_buffer1);
                            putLogDebug(100152, formatter.str(), __FILE__,
                                        __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        delete[] recv_data.recive_buffer1;
                        recv_data.recive_buffer1 = NULL;
                    }

                    if (recv_data.recive_buffer2 != NULL)
                    {
                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            boost::format formatter("delete : address = &(%d).");
                            formatter % static_cast<void*>(recv_data.recive_buffer2);
                            putLogDebug(100153, formatter.str(), __FILE__,
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
                    recv_data.recive_buffer_max_size = buffer_size;
                }
                //buffer's max size >= remain data size + requst size
                //buffer isn't need reallocate, but switch
                else
                {
                    //pointer valid check
                    if (unlikely(recv_data.recive_buffer1 == NULL || recv_data.recive_buffer2 == NULL))
                    {
                        boost::format formatter("Invalid pointer. thread id : %d.");
                        formatter % boost::this_thread::get_id();
                        putLogError(100084, formatter.str(), __FILE__, __LINE__ );
                        throw -1;
                    }
                    //using buffer is buffer1
                    if (recv_data.recive_buffer == recv_data.recive_buffer1)
                    {
                        //buffer2 initialization
                        memset(recv_data.recive_buffer2, 0, recv_data.recive_buffer_max_size);
                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            std::string datadump;
                            dump_memory(recv_data.recive_buffer + data_remain_start, data_remain_size, datadump);
                            boost::format formatter(
                                "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_realserver_recv() : before memcpy (data dump) : "
                                "data begin = %d, data_size = %d, data = %s");
                            formatter % data_remain_start % data_remain_size % datadump;
                            putLogDebug(100154, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        //copy data from buffer1 to buffer2
                        memcpy(recv_data.recive_buffer2, recv_data.recive_buffer + data_remain_start, data_remain_size);
                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            std::string datadump;
                            dump_memory(recv_data.recive_buffer2, data_remain_size, datadump);
                            boost::format formatter(
                                "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_realserver_recv() : after memcpy (data dump) : "
                                "data begin = 0, data_size = %d, data = %s");
                            formatter % data_remain_size % datadump;
                            putLogDebug(100155, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            std::string datadump;
                            dump_memory(recvbuffer.data(), recvlen, datadump);
                            boost::format formatter(
                                "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_realserver_recv() : before memcpy (data dump) : "
                                "data begin = 0, data_size = %d, data = %s");
                            formatter % data_remain_size % datadump;
                            putLogDebug(100156, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        memcpy(recv_data.recive_buffer2 + data_remain_size, recvbuffer.data(), recvlen);
                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            std::string datadump;
                            dump_memory(recv_data.recive_buffer2 + data_remain_size, recvlen, datadump);

                            boost::format formatter(
                                "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_realserver_recv() : after memcpy (data dump) : "
                                "data begin = %d, data_size = %d, data = %s");
                            formatter % data_remain_size % recvlen % datadump;
                            putLogDebug(100157, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        //set buffer2 as using buffer
                        recv_data.recive_buffer = recv_data.recive_buffer2;
                    }
                    //using buffer is buffer2
                    else
                    {
                        //buffer1 initialization
                        memset(recv_data.recive_buffer1, 0, recv_data.recive_buffer_max_size);
                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            std::string datadump;
                            dump_memory(recv_data.recive_buffer + data_remain_start, data_remain_size, datadump);

                            boost::format formatter(
                                "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_realserver_recv() : before memcpy (data dump) : "
                                "data begin = %d, data_size = %d, data = %s");
                            formatter % data_remain_start % data_remain_size % datadump;
                            putLogDebug(100158, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        //copy data from buffer2 to buffer1
                        memcpy(recv_data.recive_buffer1, recv_data.recive_buffer + data_remain_start, data_remain_size);
                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            std::string datadump;
                            dump_memory(recv_data.recive_buffer1, data_remain_size, datadump);

                            boost::format formatter(
                                "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_realserver_recv() : after memcpy (data dump) : "
                                "data begin = 0, data_size = %d, data = %s");
                            formatter % data_remain_size % datadump;
                            putLogDebug(100159, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            std::string datadump;
                            dump_memory(recvbuffer.data(), recvlen, datadump);

                            boost::format formatter(
                                "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_realserver_recv() : before memcpy (data dump) : "
                                "data begin = 0, data_size = %d, data = %s");
                            formatter % recvlen % datadump;
                            putLogDebug(100160, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        memcpy(recv_data.recive_buffer1 + data_remain_size, recvbuffer.data(), recvlen);
                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            std::string datadump;
                            dump_memory(recv_data.recive_buffer1 + data_remain_size, recvlen, datadump);

                            boost::format formatter(
                                "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_realserver_recv() : after memcpy (data dump) : "
                                "data begin = %d, data_size = %d, data = %s");
                            formatter % data_remain_size % recvlen % datadump;
                            putLogDebug(100161, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        //set buffer1 as using buffer
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
                    it_begin->send_offset -= data_remain_start;
                }
            }
            //buffer's rest size >= request size
            //copy directly
            else
            {
                //pointer valid check
                if (unlikely(recv_data.recive_buffer == NULL))
                {
                    boost::format formatter("Invalid pointer. thread id : %d.");
                    formatter % boost::this_thread::get_id();
                    putLogError(100085, formatter.str(), __FILE__, __LINE__ );
                    throw -1;
                }
                /*-------- DEBUG LOG --------*/
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    std::string datadump;
                    dump_memory(recvbuffer.data(), recvlen, datadump);
                    boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                            "handle_realserver_recv() : before memcpy (data dump) : "
                                            "data begin = 0, data_size = %d, data = %s");
                    formatter % recvlen % datadump;
                    putLogDebug(100162, formatter.str(), __FILE__, __LINE__ );
                }
                /*------DEBUG LOG END------*/

                //copy data from parameter to using buffer
                memcpy(recv_data.recive_buffer + recv_data.recive_buffer_max_size - recv_data.recive_buffer_rest_size,
                       recvbuffer.data(), recvlen);
                /*-------- DEBUG LOG --------*/
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    std::string datadump;
                    dump_memory(recv_data.recive_buffer + recv_data.recive_buffer_max_size - recv_data.recive_buffer_rest_size,
                                recvlen, datadump);
                    boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                            "handle_realserver_recv() : before memcpy (data dump) : "
                                            "data begin = %d, data_size = %d, data = %s");
                    formatter % (recv_data.recive_buffer_max_size - recv_data.recive_buffer_rest_size )
                    % recvlen % datadump;
                    putLogDebug(100163, formatter.str(), __FILE__, __LINE__ );
                }
                /*------DEBUG LOG END------*/
                //buffer's rest size recalc
                recv_data.recive_buffer_rest_size -= recvlen;
                //remain data size recalc
                data_remain_size += recvlen;
            }

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
                    //send rest size > request size
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
                    //send rest size <= request size
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
                    //change status from SEND_CONTINUE to SEND_OK
                    it->status = SEND_OK;
                }
                //status is SEND_NG
                else if (it->status == SEND_NG)
                {
                    //check http method
                    check_result = check_status_code(recv_data.recive_buffer + it->send_offset, data_remain_size);
                    /*-------- DEBUG LOG --------*/
                    if (unlikely(LOG_LV_DEBUG == getloglevel()))
                    {
                        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                "handle_realserver_recv() : call check_http_method : "
                                                "return_value = %d. thread id : %d.");
                        formatter % check_result % boost::this_thread::get_id();
                        putLogDebug(100164, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                    //check http method result is OK
                    if (check_result == CHECK_OK)
                    {
                        //check http version
                        check_result = check_http_version(recv_data.recive_buffer + it->send_offset, data_remain_size);
                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                    "handle_realserver_recv() : call check_http_version : "
                                                    "return_value = %d. thread id : %d.");
                            formatter % check_result % boost::this_thread::get_id();
                            putLogDebug(100165, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                    }
                    //check method and version result is OK
                    if (check_result == CHECK_OK)
                    {
                        //search http header
                        bret = find_http_header(recv_data.recive_buffer + it->send_offset, data_remain_size, http_header,
                                                header_offset, header_offset_len);
                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                    "handle_realserver_recv() : call find_http_header : "
                                                    "return_value = %d. thread id : %d.");
                            formatter % static_cast<int>(bret) % boost::this_thread::get_id();
                            putLogDebug(100166, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        //search http header result is OK
                        if (bret)
                        {
                            //search Content_Length header
                            bret = find_http_header(recv_data.recive_buffer + it->send_offset, data_remain_size,
                                                    content_header, content_length_header_offset, content_length_header_len);
                            /*-------- DEBUG LOG --------*/
                            if (unlikely(LOG_LV_DEBUG == getloglevel()))
                            {
                                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                        "handle_realserver_recv() : call find_http_header : "
                                                        "return_value = %d. thread id : %d.");
                                formatter % static_cast<int>(bret) % boost::this_thread::get_id();
                                putLogDebug(100167, formatter.str(), __FILE__, __LINE__ );
                            }
                            /*------DEBUG LOG END------*/
                            //search Content_Length result is OK
                            if (bret)
                            {
                                //Get Content_Length header's numeric value
                                for (pos = 0; recv_data.recive_buffer[it->send_offset + content_length_header_offset + pos] != ':' && pos
                                        < content_length_header_len; ++pos)
                                    ;
                                if (pos == content_length_header_len)
                                {
                                    throw std::string("Content_Length field's value is invalid.");
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
                                    throw std::string("Content_Length field's value is invalid.");
                                }
                                //send_rest_size recalc
                                //set whole http header's length + Content_Length's value
                                it->send_rest_size = header_offset + header_offset_len + cr_lf_len + content_len_value;
                            }
                            //search Content_Length result is NG
                            else
                            {
                                //send_rest_size recalc
                                //set whole http header's length
                                it->send_rest_size = header_offset + header_offset_len + cr_lf_len;
                            }
                        }
                        //search http header result is NG
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
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                std::string datadump;
                boost::format formatter("\nitem[%d] : status = %d, send_end_size = %d, "
                                        "send_rest_size = %d, send_possible_size = %d, "
                                        "send_offset = %d, unsend_size = %d, edit_division = %d.");
                int i = 0;
                for (it = recv_data.send_status_list.begin();
                        it != recv_data.send_status_list.end();
                        ++it, ++i)
                {
                    formatter % i % it->status % it->send_end_size
                    % it->send_rest_size % it->send_possible_size
                    % it->send_offset % it->unsend_size % it->edit_division;
                    datadump += formatter.str();
                }

                formatter.parse("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_realserver_recv() : send status list dump : send status list size = %d.%s");

                formatter % recv_data.send_status_list.size() % datadump;
                putLogDebug(100168, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/
            //there are still rest data need to process
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
                //calc offset
                new_send_it->send_offset = recv_data.recive_buffer_max_size - recv_data.recive_buffer_rest_size
                                           - request_data_remain_size;

                //check http method
                check_result = check_status_code(recv_data.recive_buffer + new_send_it->send_offset,
                                                 request_data_remain_size);
                /*-------- DEBUG LOG --------*/
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                            "handle_realserver_recv() : call check_http_method : "
                                            "return_value = %d. thread id : %d.");
                    formatter % check_result % boost::this_thread::get_id();
                    putLogDebug(100169, formatter.str(), __FILE__, __LINE__ );
                }
                /*------DEBUG LOG END------*/
                //check http method result is OK
                if (check_result == CHECK_OK)
                {
                    //check http version
                    check_result = check_http_version(recv_data.recive_buffer + new_send_it->send_offset,
                                                      request_data_remain_size);
                    /*-------- DEBUG LOG --------*/
                    if (unlikely(LOG_LV_DEBUG == getloglevel()))
                    {
                        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                "handle_realserver_recv() : call check_http_version : "
                                                "return_value = %d. thread id : %d.");
                        formatter % check_result % boost::this_thread::get_id();
                        putLogDebug(100170, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                }
                //check http method and version result is OK
                if (check_result == CHECK_OK)
                {
                    //search whole http header, get whole http header's offset and length
                    bret = find_http_header(recv_data.recive_buffer + new_send_it->send_offset, request_data_remain_size,
                                            http_header, header_offset, header_offset_len);
                    /*-------- DEBUG LOG --------*/
                    if (unlikely(LOG_LV_DEBUG == getloglevel()))
                    {
                        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                "handle_realserver_recv() : call find_http_header : "
                                                "return_value = %d. thread id : %d.");
                        formatter % static_cast<int>(bret) % boost::this_thread::get_id();
                        putLogDebug(100171, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                    //searched whole http header
                    if (bret)
                    {
                        //search ContentLength http header, get ContentLength header's offset and length
                        bret = find_http_header(recv_data.recive_buffer + new_send_it->send_offset,
                                                request_data_remain_size, content_header, content_length_header_offset, content_length_header_len);
                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                    "handle_realserver_recv() : call find_http_header : "
                                                    "return_value = %d. thread id : %d.");
                            formatter % static_cast<int>(bret) % boost::this_thread::get_id();
                            putLogDebug(100172, formatter.str(), __FILE__, __LINE__ );
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
                                throw std::string("Content_Length field's value is invalid.");
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
                                throw std::string("Content_Length field's value is invalid.");
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
                //check http method or version result is impossible
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
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                std::string datadump;
                boost::format formatter("\nitem[%d] : status = %d, send_end_size = %d, "
                                        "send_rest_size = %d, send_possible_size = %d, "
                                        "send_offset = %d, unsend_size = %d, edit_division = %d.");
                int i = 0;
                for (it = recv_data.send_status_list.begin();
                        it != recv_data.send_status_list.end();
                        ++it, ++i)
                {
                    formatter % i % it->status % it->send_end_size
                    % it->send_rest_size % it->send_possible_size
                    % it->send_offset % it->unsend_size % it->edit_division;
                    datadump += formatter.str();
                }

                formatter.parse("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_realserver_recv() : send status list dump : send status list size = %d.%s");

                formatter % recv_data.send_status_list.size() % datadump;
                putLogDebug(100173, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/

            //search for send_possible item in status list
            send_status_it it_find = find_if(recv_data.send_status_list.begin(), recv_data.send_status_list.end(),
                                             data_send_possible());
            //the data that can be sent possible is exist
            if (it_find != recv_data.send_status_list.end())
            {
                status = CLIENT_CONNECTION_CHECK;
            }
            //the data that can be sent possible is not exist
            else
            {
                status = REALSERVER_RECV;
            }
        }
        catch (int e)
        {
            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                        "handle_realserver_recv() : catch exception e = %d. thread id : %d.");
                formatter % e % boost::this_thread::get_id();
                putLogDebug(100174, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/
            status = FINALIZE;
        }
        catch (const std::string& ex)
        {
            std::cerr << "protocol_module_sessionless::handle_realserver_recv() : exception : " << ex << std::endl;
            boost::format formatter("protocol_module_sessionless::handle_realserver_recv() : exception : %s. thread id : %d.");
            formatter % ex.c_str() % boost::this_thread::get_id();
            putLogError(100086, formatter.str(), __FILE__, __LINE__ );
            status = FINALIZE;
        }
        catch (const std::bad_alloc&)
        {
            std::cerr << "protocol_module_sessionless::handle_realserver_recv() : exception : Could not allocate memory." << std::endl;
            boost::format formatter("Could not allocate memory. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(100087, formatter.str(), __FILE__, __LINE__ );
            status = FINALIZE;
        }
        catch (const std::exception& ex)
        {
            std::cerr << "protocol_module_sessionless::handle_realserver_recv() : exception : error = " << ex.what() << "." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_realserver_recv() : exception : error = %s. thread id : %d.");
            formatter % ex.what() % boost::this_thread::get_id();
            putLogError(100088, formatter.str(), __FILE__, __LINE__ );

            status = FINALIZE;
        }
        catch (...)
        {
            std::cerr << "protocol_module_sessionless::handle_realserver_recv() : Unknown exception." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG "
                                    "protocol_module_sessionless::handle_realserver_recv() : "
                                    "Unknown exception. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(100089, formatter.str(), __FILE__, __LINE__ );
            status = FINALIZE;
        }

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_realserver_recv(const boost::thread::id thread_id, "
                                    "const boost::asio::ip::tcp::endpoint& rs_endpoint, "
                                    "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                    "const size_t recvlen) : return_value = %d. thread id : %d.");
            formatter % status % boost::this_thread::get_id();
            putLogDebug(100175, formatter.str(), __FILE__, __LINE__ );
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
    protocol_module_base::EVENT_TAG protocol_module_sessionless::handle_sorryserver_recv(
        const boost::thread::id thread_id, const boost::asio::ip::tcp::endpoint& sorry_endpoint, const boost::array<
        char, MAX_BUFFER_SIZE>& recvbuffer, const size_t recvlen)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            size_t buffer_size = recvbuffer.size() < recvlen ? recvbuffer.size() : recvlen;
            std::string buffer;
            dump_memory(recvbuffer.data(), buffer_size, buffer);
            boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_sorryserver_recv(const boost::thread::id thread_id, "
                                    "const boost::asio::ip::tcp::endpoint& sorry_endpoint, "
                                    "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                    "const size_t recvlen) : thread_id = %d, sorry_endpoint = [%s]:%d, recvbuffer = %s, recvlen = %d.");
            formatter % thread_id % sorry_endpoint.address().to_string() % sorry_endpoint.port()
            % buffer % recvlen;
            putLogDebug(100176, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        EVENT_TAG status = FINALIZE;
        size_t data_remain_start = 0;
        size_t data_remain_size = 0;
        size_t request_data_remain_size = 0;
        size_t header_offset = 0;
        size_t header_offset_len = 0;
        size_t content_length_header_offset = 0;
        size_t content_length_header_len = 0;
        size_t content_len_value = 0;
        size_t pos = 0;
        size_t buffer_size = 0;
        const size_t cr_lf_len = strlen("\r\n\r\n");
        std::string str_value;
        const std::string http_header = "";
        const std::string content_header = "Content-Length";
        thread_data_ptr session_data;
        char* buffer1 = NULL;
        char* buffer2 = NULL;
        bool bret = false;
        CHECK_RESULT_TAG check_result;
        session_thread_data_map_it session_thread_it;
        recive_data_map_it recive_data_it;

        //parameter check
        if (recvlen > recvbuffer.size())
        {
            std::cerr << "protocol_module_sessionless::handle_sorryserver_recv() : Data size bigger than buffer size." << std::endl;
            boost::format formatter("Data size bigger than buffer size. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(100090, formatter.str(), __FILE__,
                        __LINE__ );
            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                        "handle_sorryserver_recv(const boost::thread::id thread_id, "
                                        "const boost::asio::ip::tcp::endpoint& sorry_endpoint, "
                                        "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                        "const size_t recvlen) : return_value = %d. thread id : %d.");
                formatter % FINALIZE % boost::this_thread::get_id();
                putLogDebug(100177, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/
            return FINALIZE;
        }

        try
        {
            {
                boost::mutex::scoped_lock slock(session_thread_data_map_mutex );

                session_thread_it = session_thread_data_map.find(thread_id);
                if (unlikely(session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL))
                {
                    boost::format formatter("Invalid thread id. thread id : %d.");
                    formatter % boost::this_thread::get_id();
                    putLogError(100091, formatter.str(), __FILE__, __LINE__ );
                    throw -1;
                }

                session_data = session_thread_it->second;
            }

            recive_data_it = session_data->recive_data_map.find(sorry_endpoint);
            if (unlikely(recive_data_it == session_data->recive_data_map.end()))
            {
                recive_data recv_data;
                session_data->recive_data_map[sorry_endpoint] = recv_data;
            }

            session_data->target_endpoint = sorry_endpoint;

            recive_data& recv_data = session_data->recive_data_map[sorry_endpoint];

            //status list check
            send_status_it it = recv_data.send_status_list.begin();
            send_status_it it_end = recv_data.send_status_list.end();
            it = std::find_if(it, it_end, data_send_ok());
            if (unlikely(it != it_end))
            {
                boost::format formatter("Sending data is invalid. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogError(100092, formatter.str(), __FILE__, __LINE__ );
                throw -1;
            }

            //status list check
            it = recv_data.send_status_list.begin();
            it = std::adjacent_find(it, it_end, data_send_repeated());
            if (unlikely(it != it_end))
            {
                boost::format formatter("Sending data is invalid. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogError(100093, formatter.str(), __FILE__, __LINE__ );
                throw -1;
            }

            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                std::string datadump;
                boost::format formatter("\nitem[%d] : status = %d, send_end_size = %d, "
                                        "send_rest_size = %d, send_possible_size = %d, "
                                        "send_offset = %d, unsend_size = %d, edit_division = %d.");
                int i = 0;
                for (it = recv_data.send_status_list.begin();
                        it != recv_data.send_status_list.end();
                        ++it, ++i)
                {
                    formatter % i % it->status % it->send_end_size
                    % it->send_rest_size % it->send_possible_size
                    % it->send_offset % it->unsend_size % it->edit_division;
                    datadump += formatter.str();
                }

                formatter.parse("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_sorryserver_recv() : send status list dump : send status list size = %d.%s");

                formatter % recv_data.send_status_list.size() % datadump;
                putLogDebug(100178, formatter.str(), __FILE__, __LINE__ );
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
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                std::string datadump;
                boost::format formatter("\nitem[%d] : status = %d, send_end_size = %d, "
                                        "send_rest_size = %d, send_possible_size = %d, "
                                        "send_offset = %d, unsend_size = %d, edit_division = %d.");
                int i = 0;
                for (it = recv_data.send_status_list.begin();
                        it != recv_data.send_status_list.end();
                        ++it, ++i)
                {
                    formatter % i % it->status % it->send_end_size
                    % it->send_rest_size % it->send_possible_size
                    % it->send_offset % it->unsend_size % it->edit_division;
                    datadump += formatter.str();
                }

                formatter.parse("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_sorryserver_recv() : send status list dump : send status list size = %d.%s");

                formatter % recv_data.send_status_list.size() % datadump;
                putLogDebug(100179, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/
            //recive buffer process
            //buffer rest size < request size
            if (recv_data.recive_buffer_rest_size < recvlen)
            {
                //buffer max size < remain size + request size
                //buffer is need reallocate
                if (recv_data.recive_buffer_max_size < data_remain_size + recvlen)
                {
                    //the buffer's size that will be allocated is exceed the upper limit value
                    if (MAX_SESSIONLESS_MODULE_BUFFER_SIZE < data_remain_size + recvlen)
                    {
                        std::cerr << "protocol_module_sessionless::handle_sorryserver_recv() : the buffer's size that will be allacted is exceed the upper limit value." << std::endl;
                        boost::format formatter("The buffer's size that will be allocated is exceed the upper limit value. thread id : %d.");
                        formatter % boost::this_thread::get_id();
                        putLogError(100094, formatter.str(), __FILE__, __LINE__ );
                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                    "handle_sorryserver_recv(const boost::thread::id thread_id, "
                                                    "const boost::asio::ip::tcp::endpoint& sorry_endpoint, "
                                                    "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                                    "const size_t recvlen) : return_value = %d. thread id : %d.");
                            formatter % FINALIZE % boost::this_thread::get_id();
                            putLogDebug(100180, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        return FINALIZE;
                    }
                    //recive_buffer1's memory allocate and initialization
                    buffer_size = (data_remain_size + recvlen) > MAX_BUFFER_SIZE ? (data_remain_size + recvlen) : MAX_BUFFER_SIZE;
                    buffer1 = new char[buffer_size];
                    /*-------- DEBUG LOG --------*/
                    if (unlikely(LOG_LV_DEBUG == getloglevel()))
                    {
                        boost::format formatter("new : address = &(%d), size = %lu.");
                        formatter % static_cast<void*>(buffer1) % buffer_size;
                        putLogDebug(100181, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*-------- DEBUG LOG END--------*/
                    memset(buffer1, 0, buffer_size);
                    //recive_buffer2's memory allocate and initialization
                    buffer2 = new char[buffer_size];
                    /*-------- DEBUG LOG --------*/
                    if (unlikely(LOG_LV_DEBUG == getloglevel()))
                    {
                        boost::format formatter("new : address = &(%d), size = %lu.");
                        formatter % static_cast<void*>(buffer2) % buffer_size;
                        putLogDebug(100182, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*-------- DEBUG LOG END--------*/
                    memset(buffer2, 0, buffer_size);

                    /*-------- DEBUG LOG --------*/
                    if (unlikely(LOG_LV_DEBUG == getloglevel()))
                    {
                        std::string datadump;
                        dump_memory(recv_data.recive_buffer + data_remain_start, data_remain_size, datadump);
                        boost::format formatter(
                            "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                            "handle_sorryserver_recv() : before memcpy (data dump) : "
                            "data begin = %d, data_size = %d, data = %s");
                        formatter % data_remain_start % data_remain_size % datadump;
                        putLogDebug(100183, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                    //copy data from old buffer to new buffer
                    memcpy(buffer1, recv_data.recive_buffer + data_remain_start, data_remain_size);
                    /*-------- DEBUG LOG --------*/
                    if (unlikely(LOG_LV_DEBUG == getloglevel()))
                    {
                        std::string datadump;
                        dump_memory(buffer1, data_remain_size, datadump);
                        boost::format formatter(
                            "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                            "handle_sorryserver_recv() : after memcpy (data dump) : "
                            "data begin = 0, data_size = %d, data = %s");
                        formatter % data_remain_size % datadump;
                        putLogDebug(100184, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/

                    /*-------- DEBUG LOG --------*/
                    if (unlikely(LOG_LV_DEBUG == getloglevel()))
                    {
                        std::string datadump;
                        dump_memory(recvbuffer.data(), recvlen, datadump);
                        boost::format formatter(
                            "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                            "handle_sorryserver_recv() : before memcpy (data dump) : "
                            "data begin = 0, data_size = %d, data = %s");
                        formatter % recvlen % datadump;
                        putLogDebug(100185, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                    memcpy(buffer1 + data_remain_size, recvbuffer.data(), recvlen);
                    /*-------- DEBUG LOG --------*/
                    if (unlikely(LOG_LV_DEBUG == getloglevel()))
                    {
                        std::string datadump;
                        dump_memory(buffer1 + data_remain_size, recvlen, datadump);
                        boost::format formatter(
                            "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                            "handle_sorryserver_recv() : after memcpy (data dump) : "
                            "data begin = %d, data_size = %d, data = %s");
                        formatter % data_remain_size % recvlen % datadump;
                        putLogDebug(100186, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                    //free old buffer1 and old buffer2
                    if (recv_data.recive_buffer1 != NULL)
                    {
                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            boost::format formatter("delete : address = &(%d).");
                            formatter % static_cast<void*>(recv_data.recive_buffer1);
                            putLogDebug(100187, formatter.str(), __FILE__,
                                        __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        delete[] recv_data.recive_buffer1;
                        recv_data.recive_buffer1 = NULL;
                    }

                    if (recv_data.recive_buffer2 != NULL)
                    {
                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            boost::format formatter("delete : address = &(%d).");
                            formatter % static_cast<void*>(recv_data.recive_buffer2);
                            putLogDebug(100188, formatter.str(), __FILE__,
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
                    recv_data.recive_buffer_max_size = buffer_size;
                }
                //buffer's max size >= remain data size + requst size
                //buffer isn't need reallocate, but switch
                else
                {
                    //pointer valid check
                    if (unlikely(recv_data.recive_buffer1 == NULL || recv_data.recive_buffer2 == NULL))
                    {
                        boost::format formatter("Invalid pointer. thread id : %d.");
                        formatter % boost::this_thread::get_id();
                        putLogError(100095, formatter.str(), __FILE__, __LINE__ );
                        throw -1;
                    }
                    //using buffer is buffer1
                    if (recv_data.recive_buffer == recv_data.recive_buffer1)
                    {
                        //buffer2 initialization
                        memset(recv_data.recive_buffer2, 0, recv_data.recive_buffer_max_size);
                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            std::string datadump;
                            dump_memory(recv_data.recive_buffer + data_remain_start, data_remain_size, datadump);
                            boost::format formatter(
                                "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_sorryserver_recv() : before memcpy (data dump) : "
                                "data begin = %d, data_size = %d, data = %s");
                            formatter % data_remain_start % data_remain_size % datadump;
                            putLogDebug(100189, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        //copy data from buffer1 to buffer2
                        memcpy(recv_data.recive_buffer2, recv_data.recive_buffer + data_remain_start, data_remain_size);
                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            std::string datadump;
                            dump_memory(recv_data.recive_buffer2, data_remain_size, datadump);
                            boost::format formatter(
                                "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_sorryserver_recv() : after memcpy (data dump) : "
                                "data begin = 0, data_size = %d, data = %s");
                            formatter % data_remain_size % datadump;
                            putLogDebug(100190, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            std::string datadump;
                            dump_memory(recvbuffer.data(), recvlen, datadump);
                            boost::format formatter(
                                "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_sorryserver_recv() : before memcpy (data dump) : "
                                "data begin = 0, data_size = %d, data = %s");
                            formatter % recvlen % datadump;
                            putLogDebug(100191, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        memcpy(recv_data.recive_buffer2 + data_remain_size, recvbuffer.data(), recvlen);
                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            std::string datadump;
                            dump_memory(recv_data.recive_buffer2 + data_remain_size, recvlen, datadump);
                            boost::format formatter(
                                "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_sorryserver_recv() : after memcpy (data dump) : "
                                "data begin = %d, data_size = %d, data = %s");
                            formatter % data_remain_size % recvlen % datadump;
                            putLogDebug(100192, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        //set buffer2 as using buffer
                        recv_data.recive_buffer = recv_data.recive_buffer2;
                    }
                    //using buffer is buffer2
                    else
                    {
                        //buffer1 initializtion
                        memset(recv_data.recive_buffer1, 0, recv_data.recive_buffer_max_size);
                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            std::string datadump;
                            dump_memory(recv_data.recive_buffer + data_remain_start, data_remain_size, datadump);
                            boost::format formatter(
                                "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_sorryserver_recv() : before memcpy (data dump) : "
                                "data begin = %d, data_size = %d, data = %s");
                            formatter % data_remain_start % data_remain_size % datadump;
                            putLogDebug(100193, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        //copy data from buffer2 to buffer1
                        memcpy(recv_data.recive_buffer1, recv_data.recive_buffer + data_remain_start, data_remain_size);
                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            std::string datadump;
                            dump_memory(recv_data.recive_buffer1, data_remain_size, datadump);
                            boost::format formatter(
                                "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_sorryserver_recv() : after memcpy (data dump) : "
                                "data begin = 0, data_size = %d, data = %s");
                            formatter % data_remain_size % datadump;
                            putLogDebug(100194, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/

                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            std::string datadump;
                            dump_memory(recvbuffer.data(), recvlen, datadump);
                            boost::format formatter(
                                "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_sorryserver_recv() : before memcpy (data dump) : "
                                "data begin = 0, data_size = %d, data = %s");
                            formatter % recvlen % datadump;
                            putLogDebug(100195, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        memcpy(recv_data.recive_buffer1 + data_remain_size, recvbuffer.data(), recvlen);
                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            std::string datadump;
                            dump_memory(recv_data.recive_buffer1 + data_remain_size, recvlen, datadump);
                            boost::format formatter(
                                "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_sorryserver_recv() : after memcpy (data dump) : "
                                "data begin = %d, data_size = %d, data = %s");
                            formatter % data_remain_size % recvlen % datadump;
                            putLogDebug(100196, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        //set buffer1 as using buffer
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
                    it_begin->send_offset -= data_remain_start;
                }
            }
            //buffer's rest size >= request size
            //copy directly
            else
            {
                //pointer valid check
                if (unlikely(recv_data.recive_buffer == NULL))
                {
                    boost::format formatter("Invalid pointer. thread id : %d");
                    formatter % boost::this_thread::get_id();
                    putLogError(100096, formatter.str(), __FILE__, __LINE__ );
                    throw -1;
                }
                /*-------- DEBUG LOG --------*/
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    std::string datadump;
                    dump_memory(recvbuffer.data(), recvlen, datadump);
                    boost::format formatter(
                        "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                        "handle_sorryserver_recv() : before memcpy (data dump) : "
                        "data begin = 0, data_size = %d, data = %s");
                    formatter % recvlen % datadump;
                    putLogDebug(100197, formatter.str(), __FILE__, __LINE__ );
                }
                /*------DEBUG LOG END------*/

                //copy data from parameter to using buffer
                memcpy(recv_data.recive_buffer + recv_data.recive_buffer_max_size - recv_data.recive_buffer_rest_size,
                       recvbuffer.data(), recvlen);
                /*-------- DEBUG LOG --------*/
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    std::string datadump;
                    dump_memory(recv_data.recive_buffer + recv_data.recive_buffer_max_size - recv_data.recive_buffer_rest_size, recvlen, datadump);
                    boost::format formatter(
                        "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                        "handle_sorryserver_recv() : after memcpy (data dump) : "
                        "data begin = %d, data_size = %d, data = %s");
                    formatter % (recv_data.recive_buffer_max_size - recv_data.recive_buffer_rest_size) % recvlen % datadump;
                    putLogDebug(100198, formatter.str(), __FILE__, __LINE__ );
                }
                /*------DEBUG LOG END------*/
                //buffer's rest size recalc
                recv_data.recive_buffer_rest_size -= recvlen;
                //remain data size recalc
                data_remain_size += recvlen;
            }

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
                    if (unlikely(LOG_LV_DEBUG == getloglevel()))
                    {
                        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                "handle_sorryserver_recv() : call check_status_code : "
                                                "return_value = %d. thread id : %d.");
                        formatter % check_result % boost::this_thread::get_id();
                        putLogDebug(100199, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                    //check http method result is OK
                    if (check_result == CHECK_OK)
                    {
                        //check http version
                        check_result = check_http_version(recv_data.recive_buffer + it->send_offset, data_remain_size);
                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                    "handle_sorryserver_recv() : call check_http_version : "
                                                    "return_value = %d. thread id : %d.");
                            formatter % check_result % boost::this_thread::get_id();
                            putLogDebug(100200, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                    }
                    //check method and version result is OK
                    if (check_result == CHECK_OK)
                    {
                        //search http header
                        bret = find_http_header(recv_data.recive_buffer + it->send_offset, data_remain_size, http_header,
                                                header_offset, header_offset_len);
                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                    "handle_sorryserver_recv() : call find_http_header : "
                                                    "return_value = %d. thread id : %d.");
                            formatter % static_cast<int>(bret) % boost::this_thread::get_id();
                            putLogDebug(100201, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        //search http header result is OK
                        if (bret)
                        {
                            //search Content_Length header
                            bret = find_http_header(recv_data.recive_buffer + it->send_offset, data_remain_size,
                                                    content_header, content_length_header_offset, content_length_header_len);
                            /*-------- DEBUG LOG --------*/
                            if (unlikely(LOG_LV_DEBUG == getloglevel()))
                            {
                                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                        "handle_sorryserver_recv() : call find_http_header : "
                                                        "return_value = %d. thread id : %d.");
                                formatter % static_cast<int>(bret) %boost::this_thread::get_id();
                                putLogDebug(100202, formatter.str(), __FILE__, __LINE__ );
                            }
                            /*------DEBUG LOG END------*/
                            //search Content_Length result is OK
                            if (bret)
                            {
                                //Get Content_Length header's numeric value
                                for (pos = 0; recv_data.recive_buffer[it->send_offset + content_length_header_offset + pos] != ':' && pos
                                        < content_length_header_len; ++pos)
                                    ;
                                if (pos == content_length_header_len)
                                {
                                    throw std::string("Content_Length field's value is invalid.");
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
                                    throw std::string("Content_Length field's value is invalid.");
                                }
                                //send_rest_size recalc
                                //set whole http header's length + Content_Length's value
                                it->send_rest_size = header_offset + header_offset_len + cr_lf_len + content_len_value;
                            }
                            //search Content_Length result is NG
                            else
                            {
                                //send_rest_size recalc
                                //set whole http header's length
                                it->send_rest_size = header_offset + header_offset_len + cr_lf_len;
                            }
                        }
                        //search http header result is NG
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
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                std::string datadump;
                boost::format formatter("\nitem[%d] : status = %d, send_end_size = %d, "
                                        "send_rest_size = %d, send_possible_size = %d, "
                                        "send_offset = %d, unsend_size = %d, edit_division = %d.");
                int i = 0;
                for (it = recv_data.send_status_list.begin();
                        it != recv_data.send_status_list.end();
                        ++it, ++i)
                {
                    formatter % i % it->status % it->send_end_size
                    % it->send_rest_size % it->send_possible_size
                    % it->send_offset % it->unsend_size % it->edit_division;
                    datadump += formatter.str();
                }

                formatter.parse("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_sorryserver_recv() : send status list dump : send status list size = %d.%s");

                formatter % recv_data.send_status_list.size() % datadump;
                putLogDebug(100203, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/

            //there are still rest data need to process
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
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                            "handle_sorryserver_recv() : call check_status_code : "
                                            "return_value = %d. thread id : %d.");
                    formatter % check_result % boost::this_thread::get_id();
                    putLogDebug(100204, formatter.str(), __FILE__, __LINE__ );
                }
                /*------DEBUG LOG END------*/
                //check http method result is OK
                if (check_result == CHECK_OK)
                {
                    //check http version
                    check_result = check_http_version(recv_data.recive_buffer + new_send_it->send_offset,
                                                      request_data_remain_size);
                    /*-------- DEBUG LOG --------*/
                    if (unlikely(LOG_LV_DEBUG == getloglevel()))
                    {
                        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                "handle_sorryserver_recv() : call check_http_version : "
                                                "return_value = %d. thread id : %d.");
                        formatter % check_result % boost::this_thread::get_id();
                        putLogDebug(100205, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                }
                //check http method and version result is OK
                if (check_result == CHECK_OK)
                {
                    //search whole http header, get whole http header's offset and length
                    bret = find_http_header(recv_data.recive_buffer + new_send_it->send_offset, request_data_remain_size,
                                            http_header, header_offset, header_offset_len);
                    /*-------- DEBUG LOG --------*/
                    if (unlikely(LOG_LV_DEBUG == getloglevel()))
                    {
                        boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                "handle_sorryserver_recv() : call find_http_header : "
                                                "return_value = %d. thread id : %d.");
                        formatter % static_cast<int>(bret) % boost::this_thread::get_id();
                        putLogDebug(100206, formatter.str(), __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/
                    //searched whole http header
                    if (bret)
                    {
                        //search ContentLength http header, get ContentLength header's offset and length
                        bret = find_http_header(recv_data.recive_buffer + new_send_it->send_offset,
                                                request_data_remain_size, content_header, content_length_header_offset, content_length_header_len);
                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                    "handle_sorryserver_recv() : call find_http_header : "
                                                    "return_value = %d. thread id : %d.");
                            formatter % static_cast<int>(bret) % boost::this_thread::get_id();
                            putLogDebug(100207, formatter.str(), __FILE__, __LINE__ );
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
                                throw std::string("Content_Length field's value is invalid.");
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
                                throw std::string("Content_Length field's value is invalid.");
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
                //check http method or version result is impossible
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
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                std::string datadump;
                boost::format formatter("\nitem[%d] : status = %d, send_end_size = %d, "
                                        "send_rest_size = %d, send_possible_size = %d, "
                                        "send_offset = %d, unsend_size = %d, edit_division = %d.");
                int i = 0;
                for (it = recv_data.send_status_list.begin();
                        it != recv_data.send_status_list.end();
                        ++it, ++i)
                {
                    formatter % i % it->status % it->send_end_size
                    % it->send_rest_size % it->send_possible_size
                    % it->send_offset % it->unsend_size % it->edit_division;
                    datadump += formatter.str();
                }

                formatter.parse("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                "handle_sorryserver_recv() : send status list dump : send status list size = %d.%s");

                formatter % recv_data.send_status_list.size() % datadump;
                putLogDebug(100208, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/

            //search for send_possible item in status list
            send_status_it it_find = find_if(recv_data.send_status_list.begin(), recv_data.send_status_list.end(),
                                             data_send_possible());
            //the data that can be sent possible is exist
            if (it_find != recv_data.send_status_list.end())
            {
                status = CLIENT_CONNECTION_CHECK;
            }
            //the data that can be sent possible is not exist
            else
            {
                status = SORRYSERVER_RECV;
            }
        }
        catch (int e)
        {
            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                        "handle_sorryserver_recv() : catch exception e = %d. thread id : %d.");
                formatter % e % boost::this_thread::get_id();
                putLogDebug(100209, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/
            status = FINALIZE;
        }
        catch (const std::string& ex)
        {
            std::cerr << "protocol_module_sessionless::handle_sorryserver_recv() : exception : " << ex << std::endl;
            boost::format formatter("protocol_module_sessionless::handle_sorryserver_recv() : exception : %s. thread id : %d.");
            formatter % ex.c_str() % boost::this_thread::get_id();
            putLogError(100097, formatter.str(), __FILE__, __LINE__ );
            status = FINALIZE;
        }
        catch (const std::bad_alloc&)
        {
            std::cerr << "protocol_module_sessionless::handle_sorryserver_recv() : exception : Could not allocate memory." << std::endl;
            boost::format formatter("Could not allocate memory. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(100098, formatter.str(), __FILE__, __LINE__ );
            status = FINALIZE;
        }
        catch (const std::exception& ex)
        {
            std::cerr << "protocol_module_sessionless::handle_sorryserver_recv() : exception : error = " << ex.what() << "." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_sorryserver_recv() : exception : error = %s. thread id : %d.");
            formatter % ex.what() % boost::this_thread::get_id();
            putLogError(100099, formatter.str(), __FILE__, __LINE__ );

            status = FINALIZE;
        }
        catch (...)
        {
            std::cerr << "protocol_module_sessionless::handle_sorryserver_recv() : Unknown exception." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG "
                                    "protocol_module_sessionless::handle_sorryserver_recv() : "
                                    "Unknown exception. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(100100, formatter.str(), __FILE__, __LINE__ );
            status = FINALIZE;
        }

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_sorryserver_recv(const boost::thread::id thread_id, "
                                    "const boost::asio::ip::tcp::endpoint& sorry_endpoint, "
                                    "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, "
                                    "const size_t recvlen) : return_value = %d. thread id : %d.");
            formatter % status % boost::this_thread::get_id();
            putLogDebug(100210, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        return status;
    }

    //! called from UPSTEEARM thread. make module original message.
    //! @param[in]    downstream thread id.
    //! @return     session use EVENT mode
    protocol_module_base::EVENT_TAG protocol_module_sessionless::handle_response_send_inform(
        const boost::thread::id thread_id)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("in/out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_response_send_inform(const boost::thread::id thread_id) : "
                                    "return_value = %d. thread id : %d.");
            formatter % STOP % boost::this_thread::get_id();
            putLogDebug(100211, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/
        return STOP;
    }

    //! called from after client connection check. use TCP/IP only. create client send message.
    //! @param[in]    downstream thread id
    //! @param[out]    send budffer reference
    //! @param[out]    send data length
    //! @return     session use EVENT mode
    protocol_module_base::EVENT_TAG protocol_module_sessionless::handle_client_connection_check(
        const boost::thread::id thread_id, boost::array<char, MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen)
    {

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_client_connection_check(const boost::thread::id thread_id, "
                                    "boost::array<char, MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen) : "
                                    "thread_id = %d.");
            formatter % thread_id;
            putLogDebug(100212, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        EVENT_TAG status = FINALIZE;
        size_t send_buffer_size = sendbuffer.max_size();
        thread_data_ptr session_data;
        session_thread_data_map_it session_thread_it;
        recive_data_map_it recive_data_it;

        try
        {
            {
                boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);

                session_thread_it = session_thread_data_map.find(thread_id);
                if (unlikely(session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL))
                {
                    boost::format formatter("Invalid thread id. thread id : %d.");
                    formatter % boost::this_thread::get_id();
                    putLogError(100101, formatter.str(), __FILE__, __LINE__ );
                    throw -1;
                }

                session_data = session_thread_it->second;
            }

            recive_data_it = session_data->recive_data_map.find(session_data->target_endpoint);
            if (unlikely(recive_data_it == session_data->recive_data_map.end()))
            {
                boost::format formatter("Invalid endpoint. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogError(100102, formatter.str(), __FILE__, __LINE__ );
                throw -1;
            }

            recive_data& recv_data = recive_data_it->second;

            //get the data that can be sent possible
            send_status_it it = find_if(recv_data.send_status_list.begin(), recv_data.send_status_list.end(),
                                        data_send_possible());
            if (unlikely(it == recv_data.send_status_list.end()))
            {
                boost::format formatter("Sending possible data is not existed. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogError(100103, formatter.str(), __FILE__, __LINE__ );
                throw -1;
            }

            //buffer size >= sending_possible size
            if (send_buffer_size > it->send_possible_size)
            {
                /*-------- DEBUG LOG --------*/
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    std::string datadump;
                    dump_memory(recv_data.recive_buffer + it->send_offset, it->send_possible_size, datadump);

                    boost::format formatter(
                        "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                        "handle_client_connection_check() : before memcpy (data dump) : "
                        "data begin = %d, data_size = %d, data = %s");
                    formatter % it->send_offset % (it->send_possible_size) % datadump;
                    putLogDebug(100213, formatter.str(), __FILE__, __LINE__ );
                }
                /*------DEBUG LOG END------*/
                //copy data from recive_buffer to sendbuffer by sending_possible size
                memcpy(sendbuffer.data(), recv_data.recive_buffer + it->send_offset, it->send_possible_size);
                /*-------- DEBUG LOG --------*/
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    std::string datadump;
                    dump_memory(sendbuffer.data(), it->send_possible_size, datadump);

                    boost::format formatter(
                        "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                        "handle_client_connection_check() : after memcpy (data dump) : "
                        "data begin = 0, data_size = %d, data = %s");
                    formatter % (it->send_possible_size) % datadump;
                    putLogDebug(100214, formatter.str(), __FILE__, __LINE__ );
                }
                /*------DEBUG LOG END------*/
                //send_end_size recalc
                it->send_end_size = it->send_possible_size;
                //set copyed data length
                datalen = it->send_possible_size;
                //sending_possible size recalc
                it->send_possible_size = 0;
            }
            //buffer size < sending_possible size
            else
            {
                /*-------- DEBUG LOG --------*/
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    std::string datadump;
                    dump_memory(recv_data.recive_buffer + it->send_offset, send_buffer_size, datadump);

                    boost::format formatter(
                        "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                        "handle_client_connection_check() : before memcpy (data dump) : "
                        "data begin = %d, data_size = %d, data = %s");
                    formatter % it->send_offset % send_buffer_size % datadump;
                    putLogDebug(100215, formatter.str(), __FILE__, __LINE__ );
                }
                /*------DEBUG LOG END------*/
                //copy data from recive_buffer to sendbuffer by buffer size
                memcpy(sendbuffer.data(), recv_data.recive_buffer + it->send_offset, send_buffer_size);
                /*-------- DEBUG LOG --------*/
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    std::string datadump;
                    dump_memory(sendbuffer.data(), send_buffer_size, datadump);

                    boost::format formatter(
                        "function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                        "handle_client_connection_check() : after memcpy (data dump) : "
                        "data begin = 0, data_size = %d, data = %s");
                    formatter % send_buffer_size % datadump;
                    putLogDebug(100216, formatter.str(), __FILE__, __LINE__ );
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
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                        "handle_client_connection_check() : catch exception e = %d. thread id : %d.");
                formatter % e % boost::this_thread::get_id();
                putLogDebug(100217, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/
            status = FINALIZE;
        }
        catch (const std::exception& ex)
        {
            std::cerr << "protocol_module_sessionless::handle_client_connection_check() : exception : error = " << ex.what() << "." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_client_connection_check() : exception : error = %s. thread id : %d.");
            formatter % ex.what() % boost::this_thread::get_id();
            putLogError(100104, formatter.str(), __FILE__, __LINE__ );
            status = FINALIZE;
        }
        catch (...)
        {
            std::cerr << "protocol_module_sessionless::handle_client_connection_check() : Unknown exception." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_client_connection_check() : Unknown exception. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(100105, formatter.str(), __FILE__, __LINE__ );
            status = FINALIZE;
        }

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_client_connection_check(const boost::thread::id thread_id, "
                                    "boost::array<char, MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen)"
                                    " : return_value = %d. thread id : %d.");
            formatter % status % boost::this_thread::get_id();
            putLogDebug(100218, formatter.str(), __FILE__, __LINE__ );
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
    protocol_module_base::EVENT_TAG protocol_module_sessionless::handle_client_select(
        const boost::thread::id thread_id, boost::asio::ip::udp::endpoint& cl_endpoint, boost::array<char,
        MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("in/out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_client_select(const boost::thread::id thread_id, "
                                    "boost::asio::ip::udp::endpoint& cl_endpoint, "
                                    "boost::array<char,MAX_BUFFER_SIZE>& sendbuffer, "
                                    "size_t& datalen) : "
                                    "return_value = %d. thread id : %d.");
            formatter % STOP % boost::this_thread::get_id();
            putLogDebug(100219, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/
        return STOP;
    }

    //!    called from after client send
    //!    @param[in]    downstream thread id
    //! @return     session use EVENT mode
    protocol_module_base::EVENT_TAG protocol_module_sessionless::handle_client_send(
        const boost::thread::id thread_id)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_client_send(const boost::thread::id thread_id) : "
                                    "thread_id = %d.");
            formatter % thread_id;
            putLogDebug(100220, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/
        EVENT_TAG status = FINALIZE;
        thread_data_ptr session_data;
        session_thread_data_map_it session_thread_it;
        recive_data_map_it recive_data_it;

        try
        {
            {
                boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
                //thread_id check
                session_thread_it = session_thread_data_map.find(thread_id);
                if (unlikely(session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL))
                {
                    boost::format formatter("Invalid thread id. thread id : %d.");
                    formatter % boost::this_thread::get_id();
                    putLogError(100106, formatter.str(), __FILE__, __LINE__ );
                    throw -1;
                }
                session_data = session_thread_it->second;
            }
            //endpoint check
            recive_data_it = session_data->recive_data_map.find(session_data->target_endpoint);
            if (unlikely(recive_data_it == session_data->recive_data_map.end()))
            {
                boost::format formatter("Invalid endpoint. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogError(100107, formatter.str(), __FILE__, __LINE__ );
                throw -1;
            }

            recive_data& recv_data = recive_data_it->second;

            send_status_it it = recv_data.send_status_list.begin();
            send_status_it it_end = recv_data.send_status_list.end();

            //check status list
            it = std::adjacent_find(it, it_end, data_send_list_incorrect());
            if (unlikely(it != it_end))
            {
                boost::format formatter("Sending possible data is invalid. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogError(100108, formatter.str(), __FILE__, __LINE__ );
                throw -1;
            }
            //status list check
            it = recv_data.send_status_list.begin();
            it = find_if(it, it_end, data_send_ok());
            if (unlikely(it == it_end))
            {
                boost::format formatter("Sending possible data is not existed. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogError(100109, formatter.str(), __FILE__, __LINE__ );
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
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                        "handle_client_send() : catch exception e = %d. thread id : %d.");
                formatter % e % boost::this_thread::get_id();
                putLogDebug(100221, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/
            status = FINALIZE;
        }
        catch (const std::exception& ex)
        {
            std::cerr << "protocol_module_sessionless::handle_client_send() : exception : error = " << ex.what() << "." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_client_send() : exception : error = %s. thread id : %d.");
            formatter % ex.what() % boost::this_thread::get_id();
            putLogError(100110, formatter.str(), __FILE__, __LINE__ );
            status = FINALIZE;
        }
        catch (...)
        {
            std::cerr << "protocol_module_sessionless::handle_client_send() : Unknown exception." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_client_send() : Unknown exception. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(100111, formatter.str(), __FILE__, __LINE__ );
            status = FINALIZE;
        }

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_client_send(const boost::thread::id thread_id) : return_value = %d. thread id : %d.");
            formatter % status % boost::this_thread::get_id();
            putLogDebug(100222, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        return status;
    }

    //! call from client disconnect event. use upstream thread and downstream thread.
    //! @param[in]    upstream and downstream thread id( check! one thread one event! )
    //! @return     session use EVENT mode
    protocol_module_base::EVENT_TAG protocol_module_sessionless::handle_client_disconnect(
        const boost::thread::id thread_id)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("in/out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_client_disconnect(const boost::thread::id thread_id) : return_value = %d. thread id : %d.");
            formatter % FINALIZE % boost::this_thread::get_id();
            putLogDebug(100223, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/
        return FINALIZE;
    }

    //! call from sorry mode event. use upstream thread and downstream thread
    //! @param[in]    upstream and downstream thread id( check! one thread one event and first time call pattern )
    //! @return     session use EVENT mode
    protocol_module_base::EVENT_TAG protocol_module_sessionless::handle_sorry_enable(
        const boost::thread::id thread_id)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_sorry_enable(const boost::thread::id thread_id) : "
                                    "thread_id = %d.");
            formatter % thread_id;
            putLogDebug(100224, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        EVENT_TAG status = FINALIZE;
        boost::asio::ip::tcp::endpoint endpoint;
        bool send_possible = false;
        bool send_continue = false;
        bool send_disable = false;
        thread_data_ptr session_data;
        session_thread_data_map_it session_thread_it;
        recive_data_map_it recive_data_it;

        try
        {
            {
                boost::mutex::scoped_lock slock(session_thread_data_map_mutex);
                //check thread_id
                session_thread_it = session_thread_data_map.find(thread_id);
                if (unlikely(session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL))
                {
                    boost::format formatter("Invalid thread id. thread id : %d.");
                    formatter % boost::this_thread::get_id();
                    putLogError(100112, formatter.str(), __FILE__, __LINE__ );
                    throw -1;
                }

                session_data = session_thread_it->second;
            }
            //check endpoint
            endpoint = session_data->thread_division == THREAD_DIVISION_UP_STREAM ? session_data->client_endpoint_tcp
                       : session_data->target_endpoint;
            recive_data_it = session_data->recive_data_map.find(endpoint);
            if (unlikely(recive_data_it == session_data->recive_data_map.end()))
            {
                //must be down thread
                if (unlikely(session_data->thread_division == THREAD_DIVISION_UP_STREAM))
                {
                    boost::format formatter("Invalid endpoint. thread id : %d.");
                    formatter % boost::this_thread::get_id();
                    putLogError(100113, formatter.str(), __FILE__, __LINE__ );
                    throw -1;
                }
                session_data->sorry_flag = SORRY_FLAG_ON;
                /*-------- DEBUG LOG --------*/
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                            "handle_sorry_enable() : SORRY_FLAG_ON. thread id : %d.");
                    formatter % boost::this_thread::get_id();
                    putLogDebug(100225, formatter.str(), __FILE__, __LINE__ );
                }
                /*------DEBUG LOG END------*/
                status = SORRYSERVER_RECV;
            }
            else
            {
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
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                    "handle_sorry_enable() : SORRY_FLAG_ON. thread id : %d.");
                            formatter % boost::this_thread::get_id();
                            putLogDebug(100226, formatter.str(), __FILE__, __LINE__ );
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
                            if (send_possible)
                            {
                                status = SORRYSERVER_CONNECT;
                            }
                            else
                            {
                                status = SORRYSERVER_SELECT;
                            }

                        }
                        //set sorry flag off
                        else
                        {
                            //the data that can be sent continue is exist
                            if (send_continue)
                            {
                                //set end flag on
                                session_data->end_flag = END_FLAG_ON;
                                /*-------- DEBUG LOG --------*/
                                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                                {
                                    boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                            "handle_sorry_enable() : END_FLAG_ON. thread id : %d.");
                                    formatter % boost::this_thread::get_id();
                                    putLogDebug(100227, formatter.str(), __FILE__, __LINE__ );
                                }
                                /*------DEBUG LOG END------*/
                                status = REALSERVER_DISCONNECT;
                            }
                            //the data that can be sent continue is not exist
                            else
                            {
                                //set sorryserver_switch_flag on
                                session_data->sorryserver_switch_flag = SORRYSERVER_SWITCH_FLAG_ON;
                                /*-------- DEBUG LOG --------*/
                                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                                {
                                    boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                            "handle_sorry_enable() : SORRYSERVER_SWITCH_FLAG_ON. thread id : %d.");
                                    formatter % boost::this_thread::get_id();
                                    putLogDebug(100228, formatter.str(), __FILE__, __LINE__ );
                                }
                                /*------DEBUG LOG END------*/
                                //set sorry_flag on
                                session_data->sorry_flag = SORRY_FLAG_ON;
                                /*-------- DEBUG LOG --------*/
                                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                                {
                                    boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                            "handle_sorry_enable() : SORRY_FLAG_ON. thread id : %d.");
                                    formatter % boost::this_thread::get_id();
                                    putLogDebug(100229, formatter.str(), __FILE__, __LINE__ );
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
                        //set sorry_flag on
                        session_data->sorry_flag = SORRY_FLAG_ON;
                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                    "handle_sorry_enable() : SORRY_FLAG_ON. thread id : %d.");
                            formatter % boost::this_thread::get_id();
                            putLogDebug(100230, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        session_data->sorryserver_switch_flag = SORRYSERVER_SWITCH_FLAG_ON;
                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                    "handle_sorry_enable() : SORRYSERVER_SWITCH_FLAG_ON. thread id : %d.");
                            formatter % boost::this_thread::get_id();
                            putLogDebug(100231, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/

                        //sending ng data is exist or send_rest_size > 0
                        if (send_disable)
                        {
                            //set end flag on
                            session_data->end_flag = END_FLAG_ON;
                            /*-------- DEBUG LOG --------*/
                            if (unlikely(LOG_LV_DEBUG == getloglevel()))
                            {
                                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                        "handle_sorry_enable() : END_FLAG_ON. thread id : %d.");
                                formatter % boost::this_thread::get_id();
                                putLogDebug(100232, formatter.str(), __FILE__, __LINE__ );
                            }
                            /*------DEBUG LOG END------*/
                            status = REALSERVER_DISCONNECT;
                        }
                        //
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
                                status = SORRYSERVER_RECV;
                            }
                        }
                    }
                }
            }
        }
        catch (int e)
        {
            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                        "handle_sorry_enable() : catch exception e = %d. thread id : %d.");
                formatter % e % boost::this_thread::get_id();
                putLogDebug(100233, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/
            status = FINALIZE;
        }
        catch (const std::exception& ex)
        {
            std::cerr << "protocol_module_sessionless::handle_sorry_enable() : exception : error = " << ex.what() << "." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_sorry_enable() : exception : error = %s. thread id : %d.");
            formatter % ex.what() % boost::this_thread::get_id();
            putLogError(100114, formatter.str(), __FILE__, __LINE__ );
            status = FINALIZE;
        }
        catch (...)
        {
            std::cerr << "protocol_module_sessionless::handle_sorry_enable() : Unknown exception." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_sorry_enable() : Unknown exception. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(100115, formatter.str(), __FILE__, __LINE__ );
            status = FINALIZE;
        }

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_sorry_enable(const boost::thread::id thread_id) : return_value = %d. thread id : %d.");
            formatter % status % boost::this_thread::get_id();
            putLogDebug(100234, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        return status;
    }

    //! call from sorry mode disable. use upstream thread and downstream thread.
    //! @param[in]    upstream and downstream thread id( check! one thread one event )
    //! @return     session use EVENT mode
    protocol_module_base::EVENT_TAG protocol_module_sessionless::handle_sorry_disable(
        const boost::thread::id thread_id)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_sorry_disable(const boost::thread::id thread_id) : "
                                    "thread_id = %d.");
            formatter % thread_id;
            putLogDebug(100235, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/
        EVENT_TAG status = FINALIZE;
        boost::asio::ip::tcp::endpoint endpoint;
        bool send_possible = false;
        bool send_disable = false;
        bool send_continue = false;
        thread_data_ptr session_data;

        try
        {
            {
                boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
                //check thread_id
                session_thread_data_map_it session_thread_it = session_thread_data_map.find(thread_id);
                if (unlikely(session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL))
                {
                    boost::format formatter("Invalid thread id. thread id : %d.");
                    formatter % boost::this_thread::get_id();
                    putLogError(100116, formatter.str(), __FILE__, __LINE__ );
                    throw -1;
                }
                //check pointer
                session_data = session_thread_it->second;
            }
            //check endpoint
            endpoint = session_data->thread_division == THREAD_DIVISION_UP_STREAM ? session_data->client_endpoint_tcp
                       : session_data->target_endpoint;
            recive_data_map_it recive_data_it = session_data->recive_data_map.find(endpoint);
            if (unlikely(recive_data_it == session_data->recive_data_map.end()))
            {
                //must be down thread
                if (unlikely(session_data->thread_division == THREAD_DIVISION_UP_STREAM))
                {
                    boost::format formatter("Invalid endpoint. thread id : %d.");
                    formatter % boost::this_thread::get_id();
                    putLogError(100117, formatter.str(), __FILE__, __LINE__ );
                    throw -1;
                }

                session_data->sorry_flag = SORRY_FLAG_OFF;
                /*-------- DEBUG LOG --------*/
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                            "handle_sorry_disable() : SORRY_FLAG_OFF. thread id : %d.");
                    formatter % boost::this_thread::get_id();
                    putLogDebug(100236, formatter.str(), __FILE__, __LINE__ );
                }
                /*------DEBUG LOG END------*/
                status = REALSERVER_RECV;
            }
            else
            {
                recive_data& recv_data = recive_data_it->second;

                //get this thread sending possible data
                send_status_it it = find_if(recv_data.send_status_list.begin(), recv_data.send_status_list.end(),
                                            data_send_possible());
                if (it != recv_data.send_status_list.end())
                {
                    send_possible = true;
                }

                //sending ng data is exist or send_rest_size > 0
                it = find_if(recv_data.send_status_list.begin(), recv_data.send_status_list.end(), data_send_disable());
                if (it != recv_data.send_status_list.end())
                {
                    send_disable = true;
                }

                //the data that can be sent continue is exist
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
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                    "handle_sorry_disable() : SORRY_FLAG_OFF. thread id : %d.");
                            formatter % boost::this_thread::get_id();
                            putLogDebug(100237, formatter.str(), __FILE__, __LINE__ );
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
                            //the data that can be sent continue is exist
                            if (send_continue)
                            {
                                //set end flag on
                                session_data->end_flag = END_FLAG_ON;
                                /*-------- DEBUG LOG --------*/
                                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                                {
                                    boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                            "handle_sorry_disable() : END_FLAG_ON. thread id : %d.");
                                    formatter % boost::this_thread::get_id();
                                    putLogDebug(100238, formatter.str(), __FILE__, __LINE__ );
                                }
                                /*------DEBUG LOG END------*/
                                status = SORRYSERVER_DISCONNECT;
                            }
                            //the data that can be sent continue is not exist
                            else
                            {
                                //set realserver_switch_flag on
                                session_data->realserver_switch_flag = REALSERVER_SWITCH_FLAG_ON;
                                /*-------- DEBUG LOG --------*/
                                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                                {
                                    boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                            "handle_sorry_disable() : REALSERVER_SWITCH_FLAG_ON. thread id : %d.");
                                    formatter % boost::this_thread::get_id();
                                    putLogDebug(100239, formatter.str(), __FILE__, __LINE__ );
                                }
                                /*------DEBUG LOG END------*/
                                //set sorry_flag off
                                session_data->sorry_flag = SORRY_FLAG_OFF;
                                /*-------- DEBUG LOG --------*/
                                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                                {
                                    boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                            "handle_sorry_disable() : SORRY_FLAG_OFF. thread id : %d.");
                                    formatter % boost::this_thread::get_id();
                                    putLogDebug(100240, formatter.str(), __FILE__, __LINE__ );
                                }
                                /*------DEBUG LOG END------*/
                                status = SORRYSERVER_DISCONNECT;
                            }
                        }
                        //sorry flag is off
                        else
                        {
                            if (send_possible)
                            {
                                status = REALSERVER_CONNECT;
                            }
                            else
                            {
                                status = REALSERVER_SELECT;
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
                        //set sorry_flag off
                        session_data->sorry_flag = SORRY_FLAG_OFF;
                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                    "handle_sorry_disable() : SORRY_FLAG_OFF. thread id : %d.");
                            formatter % boost::this_thread::get_id();
                            putLogDebug(100241, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        session_data->realserver_switch_flag = REALSERVER_SWITCH_FLAG_ON;
                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                    "handle_sorry_disable() : REALSERVER_SWITCH_FLAG_ON. thread id : %d.");
                            formatter % boost::this_thread::get_id();
                            putLogDebug(100242, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        //sending ng data is exist or send_rest_size > 0
                        if (send_disable)
                        {
                            //set end flag on
                            session_data->end_flag = END_FLAG_ON;
                            /*-------- DEBUG LOG --------*/
                            if (unlikely(LOG_LV_DEBUG == getloglevel()))
                            {
                                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                        "handle_sorry_disable() : END_FLAG_ON. thread id : %d.");
                                formatter % boost::this_thread::get_id();
                                putLogDebug(100243, formatter.str(), __FILE__, __LINE__ );
                            }
                            /*------DEBUG LOG END------*/
                            status = SORRYSERVER_DISCONNECT;
                        }
                        //
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


        }
        catch (int e)
        {
            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                        "handle_sorry_disable() : catch exception e = %d. thread id : %d.");
                formatter % e % boost::this_thread::get_id();
                putLogDebug(100244, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/
            status = FINALIZE;
        }
        catch (std::exception& ex)
        {
            std::cerr << "protocol_module_sessionless::handle_sorry_disable() : exception : error = " << ex.what() << "." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_sorry_disable() : exception : error = %s. thread id : %d.");
            formatter % ex.what() % boost::this_thread::get_id();
            putLogError(100118, formatter.str(), __FILE__, __LINE__ );
            status = FINALIZE;
        }
        catch (...)
        {
            std::cerr << "protocol_module_sessionless::handle_sorry_disable() : Unknown exception." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_sorry_disable() : Unknown exception. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(100119, formatter.str(), __FILE__, __LINE__ );
            status = FINALIZE;
        }

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_sorry_disable(const boost::thread::id thread_id) : return_value = %d. thread id : %d.");
            formatter % status % boost::this_thread::get_id();
            putLogDebug(100245, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        return status;
    }

    //! call from realserver disconnect. use upstream thread and downstream thread
    //! @param[in]    upstream and downstream thread id( check! one thread one event )
    //! @param[in]    disconnected realserver endpoint.
    //! @return     session use EVENT mode
    protocol_module_base::EVENT_TAG protocol_module_sessionless::handle_realserver_disconnect(
        const boost::thread::id thread_id, const boost::asio::ip::tcp::endpoint & rs_endpoint)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("in_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_realserver_disconnect(const boost::thread::id thread_id, const boost::asio::ip::tcp::endpoint & rs_endpoint) : "
                                    "thread_id = %d, rs_endpoint = [%s]:%d.");
            formatter % thread_id % rs_endpoint.address().to_string() % rs_endpoint.port();
            putLogDebug(100246, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/
        EVENT_TAG status = FINALIZE;
        bool possible_flag = false;
        thread_data_ptr session_data;
        boost::asio::ip::tcp::endpoint endpoint;

        try
        {
            {
                boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);

                session_thread_data_map_it session_thread_it = session_thread_data_map.find(thread_id);
                if (unlikely(session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL))
                {
                    boost::format formatter("Invalid thread id. thread id : %d.");
                    formatter % boost::this_thread::get_id();
                    putLogError(100120, formatter.str(), __FILE__, __LINE__ );
                    throw -1;
                }

                session_data = session_thread_it->second;
            }

            endpoint = session_data->thread_division == THREAD_DIVISION_UP_STREAM ? session_data->client_endpoint_tcp
                       : session_data->target_endpoint;
            recive_data_map_it recive_data_it = session_data->recive_data_map.find(endpoint);
            if (unlikely(recive_data_it == session_data->recive_data_map.end()))
            {
                boost::format formatter("Invalid endpoint. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogError(100121, formatter.str(), __FILE__, __LINE__ );
                throw -1;
            }

            recive_data& recv_data = recive_data_it->second;

            //the data that can be sent possible is exist
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
                    //sorryserver_switch_flag is on
                    if (session_data->sorryserver_switch_flag == SORRYSERVER_SWITCH_FLAG_ON)
                    {
                        session_data->sorryserver_switch_flag = SORRYSERVER_SWITCH_FLAG_OFF;
                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                    "handle_realserver_disconnect() : SORRYSERVER_SWITCH_FLAG_OFF. thread id : %d.");
                            formatter % boost::this_thread::get_id();
                            putLogDebug(100247, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/

                        status = SORRYSERVER_SELECT;

                    }
                    //sorryserver_switch_flag is off
                    else
                    {
                        //set end flag on
                        session_data->end_flag = END_FLAG_ON;
                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                    "handle_realserver_disconnect() : END_FLAG_ON. thread id : %d.");
                            formatter % boost::this_thread::get_id();
                            putLogDebug(100248, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        status = CLIENT_RECV;
                    }
                }
            }
            //down thread
            else
            {
                if (session_data->end_flag == END_FLAG_ON)
                {
                    status = CLIENT_DISCONNECT;
                }
                else
                {
                    if (session_data->sorryserver_switch_flag == SORRYSERVER_SWITCH_FLAG_ON)
                    {
                        session_data->sorryserver_switch_flag = SORRYSERVER_SWITCH_FLAG_OFF;
                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                    "handle_realserver_disconnect() : SORRYSERVER_SWITCH_FLAG_OFF. thread id : %d.");
                            formatter % boost::this_thread::get_id();
                            putLogDebug(100249, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        status = SORRYSERVER_RECV;
                    }
                    else
                    {
                        //set end flag on
                        session_data->end_flag = END_FLAG_ON;
                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                    "handle_realserver_disconnect() : END_FLAG_ON. thread id : %d.");
                            formatter % boost::this_thread::get_id();
                            putLogDebug(100250, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        status = CLIENT_DISCONNECT;
                    }
                }

                //the data that can be sent possible is exist
                if (possible_flag)
                {
                    status = CLIENT_CONNECTION_CHECK;
                }
            }
        }
        catch (int e)
        {
            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                        "handle_realserver_disconnect() : catch exception e = %d. thread id : %d.");
                formatter % e % boost::this_thread::get_id();
                putLogDebug(100251, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/
            status = FINALIZE;
        }
        catch (std::exception& ex)
        {
            std::cerr << "protocol_module_sessionless::handle_realserver_disconnect() : exception: error = " << ex.what() << "." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_realserver_disconnect() : exception : error = %s. thread id : %d.");
            formatter % ex.what() % boost::this_thread::get_id();
            putLogError(100122, formatter.str(), __FILE__, __LINE__ );
            status = FINALIZE;
        }
        catch (...)
        {
            std::cerr << "protocol_module_sessionless::handle_realserver_disconnect() : Unknown exception." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_realserver_disconnect() : Unknown exception. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(100123, formatter.str(), __FILE__, __LINE__ );
            status = FINALIZE;
        }

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_realserver_disconnect(const boost::thread::id thread_id, "
                                    "const boost::asio::ip::tcp::endpoint & rs_endpoint) : return_value = %d. thread id : %d.");
            formatter % status % boost::this_thread::get_id();
            putLogDebug(100252, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        return status;
    }

    //! call from sorry server disconnect. use upstraem thread and downstream thread
    //! @param[in]    upstream and downstream thread id( check! one thread one event )
    //! @param[in]    disconnect sorryserver endpoint
    //! @return        session use EVENT mode
    //! @return        session use EVENT mode
    protocol_module_base::EVENT_TAG protocol_module_sessionless::handle_sorryserver_disconnect(
        const boost::thread::id thread_id, const boost::asio::ip::tcp::endpoint & sorry_endpoint)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format
            formatter(
                "in_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                "handle_sorryserver_disconnect(const boost::thread::id thread_id, "
                "const boost::asio::ip::tcp::endpoint & sorry_endpoint) : "
                "thread_id = %d, sorry_endpoint = [%s]:%d.");
            formatter % thread_id % sorry_endpoint.address().to_string() % sorry_endpoint.port() ;
            putLogDebug(100253, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/
        EVENT_TAG status = FINALIZE;
        bool possible_flag = false;
        thread_data_ptr session_data;
        boost::asio::ip::tcp::endpoint endpoint;

        try
        {
            {
                boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);

                session_thread_data_map_it session_thread_it = session_thread_data_map.find(thread_id);
                if (unlikely(session_thread_it == session_thread_data_map.end() || session_thread_it->second == NULL))
                {
                    boost::format formatter("Invalid thread id. thread id : %d.");
                    formatter % boost::this_thread::get_id();
                    putLogError(100124, formatter.str(), __FILE__, __LINE__ );
                    throw -1;
                }

                session_data = session_thread_it->second;
            }

            endpoint = session_data->thread_division == THREAD_DIVISION_UP_STREAM ? session_data->client_endpoint_tcp
                       : session_data->target_endpoint;
            recive_data_map_it recive_data_it = session_data->recive_data_map.find(endpoint);
            if (unlikely(recive_data_it == session_data->recive_data_map.end()))
            {
                boost::format formatter("Invalid endpoint. thread id : %d.");
                formatter % boost::this_thread::get_id();
                putLogError(100125, formatter.str(), __FILE__, __LINE__ );
                throw -1;
            }

            recive_data& recv_data = recive_data_it->second;

            //the data that can be sent possible is exist
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
                    //realserver_switch_flag is on
                    if (session_data->realserver_switch_flag == REALSERVER_SWITCH_FLAG_ON)
                    {
                        session_data->realserver_switch_flag = REALSERVER_SWITCH_FLAG_OFF;
                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                    "handle_sorryserver_disconnect() : REALSERVER_SWITCH_FLAG_OFF. thread id : %d.");
                            formatter % boost::this_thread::get_id();
                            putLogDebug(100254, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        status = REALSERVER_SELECT;
                    }
                    //realserver_switch_flag is off
                    else
                    {
                        //set end flag on
                        session_data->end_flag = END_FLAG_ON;
                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                    "handle_sorryserver_disconnect() : END_FLAG_ON. thread id : %d.");
                            formatter % boost::this_thread::get_id();
                            putLogDebug(100255, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        status = CLIENT_RECV;
                    }
                }
            }
            //down thread
            else
            {
                if (session_data->end_flag == END_FLAG_ON)
                {
                    status = CLIENT_DISCONNECT;
                }
                else
                {
                    if (session_data->realserver_switch_flag == REALSERVER_SWITCH_FLAG_ON)
                    {
                        session_data->realserver_switch_flag = REALSERVER_SWITCH_FLAG_OFF;
                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                    "handle_sorryserver_disconnect() : REALSERVER_SWITCH_FLAG_OFF. thread id : %d.");
                            formatter % boost::this_thread::get_id();
                            putLogDebug(100256, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        status = REALSERVER_RECV;
                    }
                    else
                    {
                        session_data->end_flag = END_FLAG_ON;
                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                                    "handle_sorryserver_disconnect() : END_FLAG_ON. thread id : %d.");
                            formatter % boost::this_thread::get_id();
                            putLogDebug(100257, formatter.str(), __FILE__, __LINE__ );
                        }
                        /*------DEBUG LOG END------*/
                        status = CLIENT_DISCONNECT;
                    }
                }

                //the data that can be sent possible is exist
                if (possible_flag)
                {
                    status = CLIENT_CONNECTION_CHECK;
                }
            }
        }
        catch (int e)
        {
            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                        "handle_sorryserver_disconnect() : catch exception e = %d. thread id : %d.");
                formatter % e % boost::this_thread::get_id();
                putLogDebug(100258, formatter.str(), __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/
            status = FINALIZE;
        }
        catch (const std::exception& ex)
        {
            std::cerr << "protocol_module_sessionless::handle_sorryserver_disconnect() : exception : error = " << ex.what() << "." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_sorryserver_disconnect() : exception : error = %s. thread id : %d.");
            formatter % ex.what() % boost::this_thread::get_id();
            putLogError(100126, formatter.str(), __FILE__, __LINE__ );
            status = FINALIZE;
        }
        catch (...)
        {
            std::cerr << "protocol_module_sessionless::handle_sorryserver_disconnect() : Unknown exception." << std::endl;
            boost::format formatter("function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_sorryserver_disconnect() : Unknown exception. thread id : %d.");
            formatter % boost::this_thread::get_id();
            putLogError(100127, formatter.str(), __FILE__, __LINE__ );
            status = FINALIZE;
        }

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format
            formatter(
                "out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                "handle_sorryserver_disconnect(const boost::thread::id thread_id, "
                "const boost::asio::ip::tcp::endpoint& sorry_endpoint) : return_value = %d. thread id : %d.");
            formatter % status % boost::this_thread::get_id();
            putLogDebug(100259, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        return status;
    }

    //! call from realserver disconnect. use upstream thread and downstream thread.
    //! @param[in]    upstream and downstream thread id( check! one thread one event )
    //! @param[in]    disconnect realserver endpoint
    //! @return        session use EVENT mode.
    protocol_module_base::EVENT_TAG protocol_module_sessionless::handle_realserver_close(
        const boost::thread::id thread_id, const boost::asio::ip::udp::endpoint & rs_endpoint)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("in/out_function : protocol_module_base::EVENT_TAG protocol_module_sessionless::"
                                    "handle_realserver_close(const boost::thread::id thread_id, "
                                    "const boost::asio::ip::udp::endpoint & rs_endpoint) : "
                                    "return_value = %d. thread id : %d.");
            formatter % STOP % boost::this_thread::get_id();
            putLogDebug(100260, formatter.str(), __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/
        return STOP;
    }

}

extern "C" l7vs::protocol_module_base*
    create_module()
{
    return dynamic_cast<l7vs::protocol_module_base*>(new l7vs::protocol_module_sessionless());
}

extern "C" void
    destroy_module( l7vs::protocol_module_base* in )
{
    delete in;
}
