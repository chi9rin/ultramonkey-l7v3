/*
 * @file  sslid_session_data_processor.cpp
 * @brief read session data from replication area and set session
 * @brief data to replication area.
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

#include "sslid_session_data_processor.h"
#include "protocol_module_base.h"
#include "ssl_protocol_module_base.h"
#include "boost/format.hpp"
#include "utility.h"

namespace l7vs
{
//! constructor
sslid_session_data_processor::sslid_session_data_processor(
    int maxlist, int timeout,
    sslid_replication_data_processor* replication_data_processor,
    getloglevel_func_type ingetloglevel, logger_func_type inputLogFatal,
    logger_func_type inputLogError, logger_func_type inputLogWarn,
    logger_func_type inputLogInfo, logger_func_type inputLogDebug)
    :maxlist(maxlist), timeout(timeout),
     replication_data_processor(replication_data_processor),
     getloglevel(ingetloglevel), putLogFatal(inputLogFatal),
     putLogError(inputLogError), putLogWarn(inputLogWarn),
     putLogInfo(inputLogInfo), putLogDebug(inputLogDebug)
{
    //ctor
    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("in_function : Constructor sslid_session_data_processor::"
                                "sslid_session_data_processor(int maxlist, "
                                "int timeout, sslid_replication_data_processor* replication_data_processor, "
                                "getloglevel_func_type ingetloglevel, logger_func_type inputLogFatal, "
                                "logger_func_type inputLogError, logger_func_type inputLogWarn, "
                                "logger_func_type inputLogInfo, logger_func_type inputLogDebug) : "
                                "maxlist = %d, timeout = %d, replication_data_processor = &(%d).");
        formatter % maxlist % timeout % static_cast<void*>(replication_data_processor);
        putLogDebug(300141, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    // parameter check
    if (unlikely(maxlist < 0 || timeout < 0 || replication_data_processor == NULL))
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            putLogDebug(300142, "out_function : Constructor sslid_session_data_processor::"
                                "sslid_session_data_processor(int maxlist, int timeout, "
                                "sslid_replication_data_processor* replication_data_processor, "
                                "getloglevel_func_type ingetloglevel, logger_func_type inputLogFatal, "
                                "logger_func_type inputLogError, logger_func_type inputLogWarn, "
                                "logger_func_type inputLogInfo, logger_func_type inputLogDebug)."
                            , __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/

        throw std::logic_error("invalid parameter value.");
    }

    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        putLogDebug(300143, "out_function : Constructor sslid_session_data_processor::"
                    "sslid_session_data_processor(int maxlist, int timeout, "
                    "sslid_replication_data_processor* replication_data_processor, "
                    "getloglevel_func_type ingetloglevel, logger_func_type inputLogFatal, "
                    "logger_func_type inputLogError, logger_func_type inputLogWarn, "
                    "logger_func_type inputLogInfo, logger_func_type inputLogDebug)."
                    , __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/
}

//! destructor
sslid_session_data_processor::~sslid_session_data_processor()
{
    //dtor
    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        putLogDebug(300144, "in/out_function : Destructor sslid_session_data_processor::"
                    "~sslid_session_data_processor().", __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/
}

//! get endpoint from session data
//! @param[in] session_id refrence
//! @param[out] endpoint refrence
//! @return 0 : success
//! @return 1: failed
//! @return -1:exception
int sslid_session_data_processor::get_endpoint_from_session_data(
                                    const std::string& session_id,
                                    boost::asio::ip::tcp::endpoint& endpoint)
{
    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        std::string buffer;
        ssl_protocol_module_base::dump_session_id(session_id.c_str(), session_id.size(), buffer);
        boost::format formatter("in_function : int sslid_session_data_processor::"
                                "get_endpoint_from_session_data(const std::string& session_id, "
                                "boost::asio::ip::tcp::endpoint& endpoint) : "
                                "session_id = %s, endpoint = [%s]:%d.");
        formatter % buffer % endpoint.address().to_string() % endpoint.port();
        putLogDebug(300145, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    int ret = 0;

    try
    {
        std::map<std::string, boost::asio::ip::tcp::endpoint>::iterator itendpoint;
        boost::asio::ip::tcp::endpoint temp_endpoint;

        {
            // lock the session map
            boost::mutex::scoped_lock sclock(session_map_mutex);
            itendpoint = session_endpoint_map.find(session_id);
            // can't get the data
            if (itendpoint == session_endpoint_map.end())
            {
                /*-------- DEBUG LOG --------*/
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    putLogDebug(300146, "out_function : int sslid_session_data_processor::"
                                "get_endpoint_from_session_data(const std::string& session_id, "
                                "boost::asio::ip::tcp::endpoint& endpoint) : return_value = 1.",
                                __FILE__, __LINE__);
                }
                /*------DEBUG LOG END------*/

                return 1;
            }

            endpoint = itendpoint->second;
	}

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            boost::format formatter("function : int sslid_session_data_processor::"
                                    "get_endpoint_from_session_data() : endpoint is exist endpoint = [%s]:%d.");
            formatter % endpoint.address().to_string() % endpoint.port();
            putLogDebug(300147, formatter.str(), __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/
    }
    catch(const std::exception& e)
    {
        std::cerr << "sslid_session_data_processor::get_endpoint_from_session_data() : exception: error = " << e.what() << "." << std::endl;
        boost::format formatter("function : int sslid_session_data_processor::"
                                "get_endpoint_from_session_data() : exception : error = %s.");
        formatter % e.what();
        putLogError(300059, formatter.str(), __FILE__, __LINE__);

        ret = -1;
    }

    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("out_function : int sslid_session_data_processor::"
                                "get_endpoint_from_session_data(const std::string& session_id, "
                                "boost::asio::ip::tcp::endpoint& endpoint) : return_value = %d.");
        formatter % ret;
        putLogDebug(300148, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    return ret;
}

//! write session data to map
//! @param[in] session_id refrence
//! @param[in] endpoint refrence
//! @param[in] now time
//! @return 0 : success
//! @return 1: failed
//! @return -1:exception
int sslid_session_data_processor::write_session_data(
                                    const std::string& session_id,
                                    const boost::asio::ip::tcp::endpoint& endpoint,
                                    time_t now_time)
{
    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        std::string buffer;
        ssl_protocol_module_base::dump_session_id(session_id.c_str(), session_id.size(), buffer);
        boost::format formatter("in_function : int sslid_session_data_processor::"
                                "write_session_data(const std::string& session_id, "
                                "const boost::asio::ip::tcp::endpoint& endpoint, "
                                "time_t now_time) : session_id = %s, "
                                "endpoint = [%s]:%d, now_time = %lu.");
        formatter % buffer % endpoint.address().to_string() % endpoint.port() % now_time;
        putLogDebug(300149, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

     // lock the session map
    boost::mutex::scoped_lock sclock(session_map_mutex);
    try
    {
        // map size check
        if (maxlist <= 0)
        {
             /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                putLogDebug(300150, "out_function : int sslid_session_data_processor::"
                            "write_session_data(const std::string& session_id, "
                            "const boost::asio::ip::tcp::endpoint& endpoint, time_t now_time) : "
                            "return_value = 0.", __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/
            return 0;
        }

        std::map<std::string, boost::asio::ip::tcp::endpoint>::iterator itendpoint;
        itendpoint = session_endpoint_map.find(session_id);

        if (itendpoint != session_endpoint_map.end())
        {
            // endpoint exist
            time_t oldtime = session_lasttime_map[session_id];
            session_endpoint_map[session_id] = endpoint;
            session_lasttime_map[session_id] = now_time;
            // delete session from the map
            std::multimap<time_t, std::string>::iterator itbegin;
            std::multimap<time_t, std::string>::iterator itend;
            itbegin = lasttime_session_map.lower_bound(oldtime);
            itend = lasttime_session_map.upper_bound(oldtime);

            while (itbegin != itend)
            {
                if (itbegin->first == oldtime && itbegin->second == session_id)
                {
                    lasttime_session_map.erase(itbegin);
                    break;
                }
                ++itbegin;
            }
            lasttime_session_map.insert(std::make_pair(now_time, session_id));

            sclock.unlock();

            // update the replication list
            sslid_replication_temp_data temp_data;
            temp_data.op_code = 'U';
            temp_data.session_id = session_id;
            temp_data.realserver_addr = endpoint;
            temp_data.last_time = now_time;
            replication_data_processor->put_into_temp_list(temp_data);
            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                std::string buffer;
                ssl_protocol_module_base::dump_session_id(session_id.c_str(), session_id.size(), buffer);
                boost::format formatter("function : int sslid_session_data_processor::"
                                        "write_session_data() : put_into_temp_list() "
                                        "--update item session_id = %s -- end.");
                formatter % buffer;
                putLogDebug(300151, formatter.str(), __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/
        }
        else
        {
            // endpoint not exist
            if (session_endpoint_map.size() >= static_cast<size_t>(maxlist))
            {
                // the map is full
                if (clear_expired_session_data() == 1)
                {
                    /*-------- DEBUG LOG --------*/
                    if (unlikely(LOG_LV_DEBUG == getloglevel()))
                    {
                        putLogDebug(300152, "out_function : int sslid_session_data_processor::"
                                    "write_session_data(const std::string& session_id, "
                                    "const boost::asio::ip::tcp::endpoint& endpoint, time_t now_time) : "
                                    "return_value = 0.", __FILE__, __LINE__);
                    }
                    /*------DEBUG LOG END------*/

                    return 0;
                }

                /*-------- DEBUG LOG --------*/
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    putLogDebug(300153, "function : int sslid_session_data_processor::"
                                "write_session_data() : clear_expired_session_data() end.",
                                __FILE__, __LINE__);
                }
                /*------DEBUG LOG END------*/
            }

            session_endpoint_map[session_id] = endpoint;
            session_lasttime_map[session_id] = now_time;
            lasttime_session_map.insert(std::make_pair(now_time, session_id));

            sclock.unlock();

            // add the replication list
            sslid_replication_temp_data temp_data;
            temp_data.op_code = 'A';
            temp_data.session_id = session_id;
            temp_data.realserver_addr = endpoint;
            temp_data.last_time = now_time;
            replication_data_processor->put_into_temp_list(temp_data);
            /*-------- DEBUG LOG --------*/
            if (unlikely(LOG_LV_DEBUG == getloglevel()))
            {
                std::string buffer;
                ssl_protocol_module_base::dump_session_id(session_id.c_str(), session_id.size(), buffer);
                boost::format formatter("function : int sslid_session_data_processor::"
                                        "write_session_data() : put_into_temp_list() "
                                        "--add item session_id = %s -- end.");
                formatter % buffer;
                putLogDebug(300154, formatter.str(), __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "write_session_data exception : error = " << e.what() << "." << std::endl;
        boost::format formatter("function : int sslid_session_data_processor::"
                                "write_session_data() : exception : error = %s.");
        formatter % e.what();
        putLogError(300060, formatter.str(), __FILE__, __LINE__);

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            putLogDebug(300155, "out_function : int sslid_session_data_processor::"
                        "write_session_data(const std::string& session_id, "
                        "const boost::asio::ip::tcp::endpoint& endpoint, time_t now_time) : "
                        "return_value = -1.", __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/

        return -1;
    }

    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        putLogDebug(300156, "out_function : int sslid_session_data_processor::"
                    "write_session_data(const std::string& session_id, "
                    "const boost::asio::ip::tcp::endpoint& endpoint, time_t now_time) : "
                    "return_value = 0.", __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    return 0;
}

//! read session data from replication area
//! @param[in] sslid_replication_data pointer
//! @return 0 : success
//! @return -1:exception
int sslid_session_data_processor::read_session_data_from_replication_area(
                                                    sslid_replication_data* replication_area )
{
    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("in_function : int sslid_session_data_processor::"
                                "read_session_data_from_replication_area(sslid_replication_data* replication_area) : "
                                "replication_area = &(%d).");
        formatter % static_cast<void*>(replication_area);
        putLogDebug(300157, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    // null check
    if (unlikely(replication_area == NULL))
    {
        putLogError(300061, "Replication area is NULL.", __FILE__, __LINE__);
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            putLogDebug(300158, "out_function : int sslid_session_data_processor::"
                                "read_session_data_from_replication_area("
                                "sslid_replication_data* replication_area) : return_value = -1.", __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/
        return -1;
    }

    int ret = 0;
    try
    {
        // restore replication data to map
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            char* pdata = reinterpret_cast<char*>(replication_area);
            std::string datadump;
            protocol_module_base::dump_memory(pdata, maxlist, datadump);

            boost::format formatter("function : int sslid_session_data_processor::"
                                    "read_session_data_from_replication_area() : "
                                    "data begin = 0, data_size = %d, data = %s");
            formatter % maxlist % datadump;
            putLogDebug(300159, formatter.str(), __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/
        for (int i = 0; i < maxlist; ++i)
        {
            if (replication_area[i].valid == 1)
            {
                // valid session data
                char sessionid_temp[SSLID_LENGTH+1] = {0};
                memcpy(sessionid_temp, replication_area[i].session_id, SSLID_LENGTH);
                boost::asio::ip::tcp::endpoint endpoint(
                                            boost::asio::ip::address::from_string(replication_area[i].realserver_ip),
                                            replication_area[i].realserver_port);
                session_endpoint_map[sessionid_temp] = endpoint;
                session_lasttime_map[sessionid_temp] = replication_area[i].last_time;
                lasttime_session_map.insert(std::make_pair(replication_area[i].last_time, sessionid_temp));
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "sslid_session_data_processor::read_session_data_from_replication_area() : "
                    "exception : error = " << e.what() << "." << std::endl;
        boost::format formatter("function : int sslid_session_data_processor::"
                                "read_session_data_from_replication_area() : exception : error = %s.");
        formatter % e.what();
        putLogError(300062, formatter.str(), __FILE__, __LINE__);

        ret = -1;
    }

    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("out_function : int sslid_session_data_processor::"
                                "read_session_data_from_replication_area(slid_replication_data* replication_area) : "
                                "return_value = %d.");
        formatter % ret;
        putLogDebug(300160, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    return ret;
}

//! clear expired session data
//! @return 0 : success 1 : map size is 0
int sslid_session_data_processor::clear_expired_session_data()
{
    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        putLogDebug(300161, "in_function : int sslid_session_data_processor::"
                    "clear_expired_session_data().", __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    // map must be already locked
    int ret = 0;

    if (session_endpoint_map.size() == 0)
    {
         /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            putLogDebug(300162, "out_function : int sslid_session_data_processor::"
                        "clear_expired_session_data() : return_value = 1.",  __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/

        return 1;
    }

    sslid_replication_temp_data temp_data;
    std::string session_id = lasttime_session_map.begin()->second;
    lasttime_session_map.erase(lasttime_session_map.begin());
    session_endpoint_map.erase(session_id);
    session_lasttime_map.erase(session_id);
    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        std::string buffer;
        ssl_protocol_module_base::dump_session_id(session_id.c_str(), session_id.size(), buffer);
        boost::format formatter("function : int sslid_session_data_processor::"
                    "clear_expired_session_data() : "
                    "--delete oldest time item form the map session_id = %s -- end.");
        formatter % buffer;
        putLogDebug(300163, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    // put into replication temp list
    temp_data.op_code = 'D';
    temp_data.session_id = session_id;
    replication_data_processor->put_into_temp_list(temp_data);
    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        std::string buffer;
        ssl_protocol_module_base::dump_session_id(session_id.c_str(), session_id.size(), buffer);
        boost::format formatter("function : int sslid_session_data_processor::"
                    "clear_expired_session_data() : put_into_temp_list() "
                    "--delete oldest time item session_id = %s -- end.");
        formatter % buffer;
        putLogDebug(300164, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("out_function : int sslid_session_data_processor::"
                                "clear_expired_session_data() : return_value = %d.");
        formatter % ret;
        putLogDebug(300165, formatter.str(),  __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    return ret;
}

}
