/*
 * @file  ip_session_data_processor.cpp
 * @brief read ip data from replication area and set ip
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

#include "ip_session_data_processor.h"
#include "protocol_module_base.h"
#include "boost/format.hpp"
#include "utility.h"

namespace l7vs
{
//! constructor
ip_session_data_processor::ip_session_data_processor(
    int timeout,
    ip_replication_data_processor* replication_data_processor,
    getloglevel_func_type ingetloglevel, logger_func_type inputLogFatal,
    logger_func_type inputLogError, logger_func_type inputLogWarn,
    logger_func_type inputLogInfo, logger_func_type inputLogDebug)
    :timeout(timeout),
     replication_data_processor(replication_data_processor),
     getloglevel(ingetloglevel), putLogFatal(inputLogFatal),
     putLogError(inputLogError), putLogWarn(inputLogWarn),
     putLogInfo(inputLogInfo), putLogDebug(inputLogDebug)
{
    //ctor
    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("in_function : Constructor ip_session_data_processor::"
                                "ip_session_data_processor( "
                                "int timeout, ip_replication_data_processor* replication_data_processor, "
                                "getloglevel_func_type ingetloglevel, logger_func_type inputLogFatal, "
                                "logger_func_type inputLogError, logger_func_type inputLogWarn, "
                                "logger_func_type inputLogInfo, logger_func_type inputLogDebug) : "
                                "timeout = %d, replication_data_processor = &(%d).");
        formatter % timeout % static_cast<void*>(replication_data_processor);
        putLogDebug(600186, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    // parameter check
    if (unlikely(timeout < 0 || replication_data_processor == NULL))
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            putLogDebug(600187, "out_function : Constructor ip_session_data_processor::"
                                "ip_session_data_processor(int timeout, "
                                "ip_replication_data_processor* replication_data_processor, "
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
        putLogDebug(600188, "out_function : Constructor ip_session_data_processor::"
                    "ip_session_data_processor(int timeout, "
                    "ip_replication_data_processor* replication_data_processor, "
                    "getloglevel_func_type ingetloglevel, logger_func_type inputLogFatal, "
                    "logger_func_type inputLogError, logger_func_type inputLogWarn, "
                    "logger_func_type inputLogInfo, logger_func_type inputLogDebug)."
                    , __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/
}

//! destructor
ip_session_data_processor::~ip_session_data_processor()
{
    //dtor
    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        putLogDebug(600189, "in/out_function : Destructor ip_session_data_processor::"
                    "~ip_session_data_processor().", __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/
}

//! get endpoint from session data
//! @param[in] ip hash
//! @param[out] endpoint refrence
//! @return 0 : success
//! @return 1: failed
//! @return -1:exception
int ip_session_data_processor::get_endpoint_from_session_data(
                                    int ip_hash, 
                                    boost::asio::ip::tcp::endpoint& endpoint)
{
    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("in_function : int ip_session_data_processor::"
                                 "get_endpoint_from_session_data(int ip_hash, "
                                 "boost::asio::ip::tcp::endpoint& endpoint) : "
                                 "ip_hash = %d.");
        formatter % ip_hash;
        putLogDebug(600190, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    int ret = 0;

    try
    {
        boost::asio::ip::tcp::endpoint init_endpoint;

        // ip hash check
        if (unlikely(ip_hash < 0 || ip_hash >= MAX_IP_SESSION_TBL_SIZE))
        {
	    putLogError(600104, "invalid parameter value.", __FILE__, __LINE__);
    	    /*-------- DEBUG LOG --------*/
    	    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    	    {
     	        boost::format formatter("out_function : int ip_session_data_processor::"
                                         "get_endpoint_from_ip_data(int ip_hash, "
                                         "boost::asio::ip::tcp::endpoint& endpoint) : return_value = 1.");
    	        putLogDebug(600191, formatter.str(), __FILE__, __LINE__);
   	    }
    	    /*------DEBUG LOG END------*/
            return 1;
        }

        // session is not exist
        if (session_table[ip_hash].rs_endpoint == init_endpoint)
        {
            ret = 1;
        }
        // session is exist
        else
        {
            // expired time check
            time_t now;
            time(&now);
            if (session_table[ip_hash].last_time == 0 ||
                timeout == 0 ||
                now - session_table[ip_hash].last_time <= timeout)
            {
                // time in
                 endpoint = session_table[ip_hash].rs_endpoint;
            }
            else
            {
                // time out
                ret = 1;
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "ip_session_data_processor::get_endpoint_from_ip_data() : exception: error = " << e.what() << "." << std::endl;
        boost::format formatter("function : int ip_session_data_processor::"
                                "get_endpoint_from_ip_data() : exception : error = %s.");
        formatter % e.what();
        putLogError(600105, formatter.str(), __FILE__, __LINE__);

        ret = -1;
    }

    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("out_function : int ip_session_data_processor::"
                                "get_endpoint_from_ip_data(int ip_hash, "
                                "boost::asio::ip::tcp::endpoint& endpoint) : return_value = %d.");
        formatter % ret;
        putLogDebug(600192, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    return ret;
}

//! write session data to session table
//! @param[in] ip hash
//! @param[in] endpoint refrence
//! @param[in] now time
//! @return 0 : success
//! @return 1: failed
//! @return -1:exception
int ip_session_data_processor::write_session_data(
                                    int ip_hash,
                                    const boost::asio::ip::tcp::endpoint& endpoint,
                                    time_t now_time)
{
    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
     	boost::format formatter("in_function : int ip_session_data_processor::"
                                         "write_session_data(int ip_hash,"
                                         "const boost::asio::ip::tcp::endpoint& endpoint, "
                                         "time_t now_time) : ip_hash = %d, now_time = %d.");
	formatter % ip_hash % now_time;
    	putLogDebug(600193, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    boost::asio::ip::tcp::endpoint init_endpoint;
    ip_replication_temp_data temp_data;

    try
    {
        // ip hash check
        if (unlikely(ip_hash < 0 || ip_hash >= MAX_IP_SESSION_TBL_SIZE))
        {
	    putLogError(600106, "invalid parameter value.", __FILE__, __LINE__);
    	    /*-------- DEBUG LOG --------*/
    	    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    	    {
     	        boost::format formatter("out_function : int ip_session_data_processor::"
                                         "write_session_data(int ip_hash,"
                                         "const boost::asio::ip::tcp::endpoint& endpoint, "
                                         "time_t now_time) : return_value = 1.");
    	        putLogDebug(600194, formatter.str(), __FILE__, __LINE__);
   	    }
    	    /*------DEBUG LOG END------*/
            return 1;
        }

        // write session data to session table
        session_table[ip_hash].rs_endpoint = endpoint;
        session_table[ip_hash].last_time = now_time;

        // set op_code
        if (endpoint == init_endpoint)
        {
            temp_data.op_code = 'U';
        }
        else
        {
            temp_data.op_code = 'A';
        }

        // add the replication list
        temp_data.ip_hash = ip_hash;
        temp_data.last_time = now_time;
        temp_data.rs_endpoint = endpoint;
        replication_data_processor->put_into_temp_list(temp_data);
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            std::string buffer;
                boost::format formatter("function : int ip_session_data_processor::"
                                         "write_session_data() : put_into_temp_list() "
                                        "--add item ip_hash = %d -- end.");
                formatter % ip_hash;
                putLogDebug(600195, formatter.str(), __FILE__, __LINE__);
            }
            /*------DEBUG LOG END------*/
    }
    catch (const std::exception& e)
    {
        std::cerr << "write_session_data exception : error = " << e.what() << "." << std::endl;
        boost::format formatter("function : int ip_session_data_processor::"
                                "write_session_data() : exception : error = %s.");
        formatter % e.what();
        putLogError(600107, formatter.str(), __FILE__, __LINE__);

        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            putLogDebug(600196, "out_function : int ip_session_data_processor::"
                        "write_session_data(int hash, "
                        "const boost::asio::ip::tcp::endpoint& endpoint, time_t now_time) : "
                        "return_value = -1.", __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/

        return -1;
    }

    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        putLogDebug(600197, "out_function : int ip_session_data_processor::"
                    "write_ip_data(const std::string& ip, "
                    "const boost::asio::ip::tcp::endpoint& endpoint, time_t now_time) : "
                    "return_value = 0.", __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    return 0;
}

//! read session data from replication area
//! @param[in] ip_replication_data pointer
//! @return 0 : success
//! @return -1:exception
int ip_session_data_processor::read_session_data_from_replication_area(
                                                    ip_replication_data* replication_area )
{
    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("in_function : int ip_session_data_processor::"
                                "read_session_data_from_replication_area(ip_replication_data* replication_area) : "
                                "replication_area = &(%d).");
        formatter % static_cast<void*>(replication_area);
        putLogDebug(600198, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    // null check
    if (replication_area == NULL)
    {
        putLogError(600108, "Replication area is NULL.", __FILE__, __LINE__);
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            putLogDebug(600199, "out_function : int ip_session_data_processor::"
                                "read_session_data_from_replication_area("
                                "ip_replication_data* replication_area) : return_value = -1.", __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/
        return -1;
    }

    int ret = 0;
    try
    {
        // restore replication data to session table
        for (int i = 0; i < MAX_IP_SESSION_TBL_SIZE; ++i)
        {
            if (replication_area[i].valid == 1)
            {
                boost::asio::ip::tcp::endpoint endpoint(
                                            boost::asio::ip::address::from_string(replication_area[i].realserver_ip),
                                            replication_area[i].realserver_port);
                session_table[i].rs_endpoint = endpoint;
                session_table[i].last_time = replication_area[i].last_time;
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "ip_session_data_processor::read_session_data_from_replication_area() : "
                    "exception : error = " << e.what() << "." << std::endl;
        boost::format formatter("function : int ip_session_data_processor::"
                                "read_session_data_from_replication_area() : exception : error = %s.");
        formatter % e.what();
        putLogError(600109, formatter.str(), __FILE__, __LINE__);

        ret = -1;
    }

    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("out_function : int ip_session_data_processor::"
                                "read_session_data_from_replication_area(ip_replication_data* replication_area) : "
                                "return_value = %d.");
        formatter % ret;
        putLogDebug(600200, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    return ret;
}

}
