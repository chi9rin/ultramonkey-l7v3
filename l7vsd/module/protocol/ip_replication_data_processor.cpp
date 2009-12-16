/*
 * @file  ip_replication_data_processor.cpp
 * @brief the replication of data between ACT and SBY.
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

#include "ip_replication_data_processor.h"
#include "replication.h"
#include "protocol_module_base.h"
#include "boost/format.hpp"
#include "utility.h"

namespace l7vs
{

//! constructor
ip_replication_data_processor::ip_replication_data_processor(
    char* ip_replication_area_begin, int ip_replication_area_size,
    const boost::asio::ip::tcp::endpoint& virtual_service_endpoint,
    getloglevel_func_type ingetloglevel, logger_func_type inputLogFatal,
    logger_func_type inputLogError, logger_func_type inputLogWarn,
    logger_func_type inputLogInfo, logger_func_type inputLogDebug)
    :replication_area(NULL), virtual_service_endpoint(virtual_service_endpoint),
     getloglevel(ingetloglevel), putLogFatal(inputLogFatal),
     putLogError(inputLogError), putLogWarn(inputLogWarn),
     putLogInfo(inputLogInfo), putLogDebug(inputLogDebug)
{
    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("in_function : Constructor ip_replication_data_processor::"
                                 "ip_replication_data_processor("
                                 "char* ip_replication_area_begin, int ip_replication_area_size, "
                                 "const boost::asio::ip::tcp::endpoint& virtual_service_endpoint, "
                                 "getloglevel_func_type ingetloglevel, logger_func_type inputLogFatal, "
                                 "logger_func_type inputLogError, logger_func_type inputLogWarn, "
                                 "logger_func_type inputLogInfo, logger_func_type inputLogDebug) : "
                                 "ip_replication_area_begin = &(%d), "
                                 "ip_replication_area_size = %d, virtual_service_endpoint = [%s]:%d.");
        formatter % static_cast<void*>(ip_replication_area_begin)
        % ip_replication_area_size % virtual_service_endpoint.address().to_string()
        % virtual_service_endpoint.port();
        putLogDebug(600204, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    //ctor
    struct ip_replication_data_header* head = NULL;
    struct ip_replication_data_header* pick = NULL;
    unsigned int used = 0;
    char exist = -1;
    int srv_num = 0;

    // parameter check
    if (unlikely(ip_replication_area_size < 0))
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            putLogDebug(600205, "out_function : Constructor ip_replication_data_processor::"
                         "ip_replication_data_processor("
                         "char* ip_replication_area_begin, int ip_replication_area_size, "
                         "const boost::asio::ip::tcp::endpoint& virtual_service_endpoint, "
                         "getloglevel_func_type ingetloglevel, logger_func_type inputLogFatal, "
                         "logger_func_type inputLogError, logger_func_type inputLogWarn, "
                         "logger_func_type inputLogInfo, logger_func_type inputLogDebug).", __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/

        throw std::logic_error("Invalid parameter value.");
    }

    if (ip_replication_area_begin == NULL)
    {
        // replication area is null
        putLogError(600110, "Replication area is null.", __FILE__, __LINE__);
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            putLogDebug(600206, "function : ip_replication_data_processor::"
                         "ip_replication_data_processor() : Replication area is null.",  __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/
    }
    else
    {
        // check replication area header
        for (srv_num = 0; srv_num < IP_SERVICE_NUMBER; ++srv_num)
        {
            // get the replication data header pointer
            head = reinterpret_cast<ip_replication_data_header*>(ip_replication_area_begin) + srv_num;
            // add used service area size
            used += head->size;

            if (static_cast<size_t>(ip_replication_area_size) * DATA_SIZE <
                    sizeof(ip_replication_data_header) * IP_SERVICE_NUMBER + used)
            {
                // no enough replication area
                putLogError(600111, "Over replication area.", __FILE__, __LINE__);
                /*-------- DEBUG LOG --------*/
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    putLogDebug(600207, "function : ip_replication_data_processor::"
                                 "ip_replication_data_processor() : Over replication area.",
                                 __FILE__, __LINE__);
                    putLogDebug(600208, "out_function : Constructor ip_replication_data_processor::"
                                 "ip_replication_data_processor(int maxlist, "
                                 "char* ip_replication_area_begin, int ip_replication_area_size, "
                                 "const boost::asio::ip::tcp::endpoint& virtual_service_endpoint, "
                                 "getloglevel_func_type ingetloglevel, logger_func_type inputLogFatal, "
                                 "logger_func_type inputLogError, logger_func_type inputLogWarn, "
                                 "logger_func_type inputLogInfo, logger_func_type inputLogDebug).", __FILE__, __LINE__);
                }
                /*------DEBUG LOG END------*/
                return;
            }

            if (pick == NULL && head->size == 0)
            {
                // find the first empty head
                pick = head;
            }

            if (exist >= 0 && pick != NULL)
            {
                // find the lastest same header
                break;
            }

            // match ip and port, this is pre-used ip information
            if (virtual_service_endpoint.address().to_string() == head->virtualserver_ip &&
                    virtual_service_endpoint.port() == head->virtualserver_port)
            {
                exist = srv_num;
            }
        }

        if (exist >= 0)
        {
            head = reinterpret_cast<ip_replication_data_header*>(ip_replication_area_begin) + exist;

            // restore session data from replication direct
            replication_area = reinterpret_cast<ip_replication_data*>(ip_replication_area_begin +
                                head->offset);
        }
        else
        {
            if (pick != NULL)
            {
                // data size check
                if (sizeof(ip_replication_data_header) * IP_SERVICE_NUMBER + used +
                        IP_REPLICATION_MAXLENGTH * sizeof(ip_replication_data) >
                        static_cast<size_t>(ip_replication_area_size) * DATA_SIZE)
                {
                    // replication area is full
                    putLogError(600112, "Replication area is full.", __FILE__, __LINE__);
                    /*-------- DEBUG LOG --------*/
                    if (unlikely(LOG_LV_DEBUG == getloglevel()))
                    {
                        putLogDebug(600209, "function : ip_replication_data_processor::"
                                     "ip_replication_data_processor() : Replication area is full.",
                                     __FILE__, __LINE__);
                    }
                    /*------DEBUG LOG END------*/
                }
                else
                {
                    // initialize replication information
                    replication_area = reinterpret_cast<ip_replication_data*>(ip_replication_area_begin +
                                       sizeof(ip_replication_data_header) * IP_SERVICE_NUMBER +
                                       used);
                    strncpy(pick->virtualserver_ip, virtual_service_endpoint.address().to_string().c_str(),
                            sizeof(pick->virtualserver_ip)-1);
                    pick->virtualserver_ip[sizeof(pick->virtualserver_ip)] = '\0';
                    pick->virtualserver_port = virtual_service_endpoint.port();
                    pick->size = IP_REPLICATION_MAXLENGTH * sizeof(ip_replication_data);
                    pick->offset = sizeof(ip_replication_data_header) * IP_SERVICE_NUMBER +
                                   used;
                    /*-------- DEBUG LOG --------*/
                    if (unlikely(LOG_LV_DEBUG == getloglevel()))
                    {
                        boost::format formatter("Write replication area : head = (ip = %s, port = %d, "
                                                 "size = %d, offset = %d).");
                        formatter % pick->virtualserver_ip % pick->virtualserver_port
                        % pick->size % pick->offset;
                        putLogDebug(600210, formatter.str(), __FILE__, __LINE__);
                    }
                    /*------DEBUG LOG END------*/
                }
            }
            else
            {
                // replication area is full
                putLogError(600113, "Replication area is full.", __FILE__, __LINE__);
                /*-------- DEBUG LOG --------*/
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    putLogDebug(600211, "function : ip_replication_data_processor::"
                                 "ip_replication_data_processor() : Replication area is full.",
                                 __FILE__, __LINE__);
                }
                /*------DEBUG LOG END------*/
            }
        }
    }

    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        putLogDebug(600212, "out_function : Constructor ip_replication_data_processor::"
                     "ip_replication_data_processor("
                     "char* ip_replication_area_begin, int ip_replication_area_size, "
                     "const boost::asio::ip::tcp::endpoint& virtual_service_endpoint, "
                     "getloglevel_func_type ingetloglevel, logger_func_type inputLogFatal, "
                     "logger_func_type inputLogError, logger_func_type inputLogWarn, "
                     "logger_func_type inputLogInfo, logger_func_type inputLogDebug)."
                     , __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/
}

//! destructor
ip_replication_data_processor::~ip_replication_data_processor()
{
    //dtor
    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        putLogDebug(600213, "in/out_function : Destructor ip_replication_data_processor::"
                     "~ip_replication_data_processor().", __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/
}

//! put data into temp list
//! @param[in] ip_replication_temp_data refrence
void ip_replication_data_processor::put_into_temp_list(
    const ip_replication_temp_data& data)
{
    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("in_function : void ip_replication_data_processor::put_into_temp_list("
                                 "const ip_replication_temp_data& data) : data.op_code = %c, "
                                 "data.ip_hash = %d, data.last_time = %lu, data.rs_endpoint = [%s]:%d.");
        formatter % data.op_code % data.ip_hash % data.last_time % data.rs_endpoint.address().to_string()
        % data.rs_endpoint.port();
        putLogDebug(600214, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    if (replication_area)
    {
        boost::mutex::scoped_lock sclock(temp_list_mutex);
        while (temp_list.size() >= static_cast<size_t>(MAX_IP_SESSION_TEMPLIST_SIZE))
        {
            // wait for empty list item
            temp_list_condition.wait(sclock);
        }

        temp_list.push_back(data);
        temp_list_condition.notify_one();
    }

    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        putLogDebug(600215, "out_function : void ip_replication_data_processor::"
                     "put_into_temp_list(const ip_replication_temp_data& data).", __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/
}

//! write replication area, called by protocol_module_ip::replication_interrupt
void ip_replication_data_processor::write_replication_area()
{
    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        putLogDebug(600216, "in_function : void ip_replication_data_processor::"
                     "write_replicaion_area().", __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    // replication_area check
    if (replication_area == NULL)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            putLogDebug(600217, "out_function : void ip_replication_data_processor::"
                         "write_replicaion_area().", __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/
        return;
    }

    bool blocked = false;
    try
    {
        ip_replication_temp_data temp_data;

        while (true)
        {
            if (get_from_temp_list(temp_data) == -1)
            {
                break;
            }
            blocked = false;
            replication_area_lock();
            blocked = true;

            // ip data process
            switch (temp_data.op_code)
            {
            case 'A':
                // data add
                replication_area[temp_data.ip_hash].last_time = temp_data.last_time;
                strncpy(replication_area[temp_data.ip_hash].realserver_ip,
                         temp_data.rs_endpoint.address().to_string().c_str(),
                         sizeof(replication_area[temp_data.ip_hash].realserver_ip)-1);
                replication_area[temp_data.ip_hash].realserver_port = temp_data.rs_endpoint.port();

                /*-------- DEBUG LOG --------*/
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    boost::format formatter("function : void ip_replication_data_processor::"
                                             "write_replicaion_area() : 'A' : realserver_ip = %s, "
                                             "realserver_prot = %d, last_time = %d.");
                    formatter % replication_area[temp_data.ip_hash].realserver_ip % replication_area[temp_data.ip_hash].realserver_port %
                     replication_area[temp_data.ip_hash].last_time;
                    putLogDebug(600218, formatter.str(), __FILE__, __LINE__);
                }
                /*------DEBUG LOG END------*/

                replication_area[temp_data.ip_hash].valid = 1;
                break;
            case 'U':
                // data update
                replication_area[temp_data.ip_hash].last_time = temp_data.last_time;

                /*-------- DEBUG LOG --------*/
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    boost::format formatter("function : void ip_replication_data_processor::"
                                             "write_replicaion_area() : 'U' : last_time = %d.");
                    formatter % replication_area[temp_data.ip_hash].last_time;
                    putLogDebug(600219, formatter.str(), __FILE__, __LINE__);
                }
                /*------DEBUG LOG END------*/

                replication_area[temp_data.ip_hash].valid = 1;
                break;
            default:
                break;
            }

            replication_area_unlock();
        }
    }
    catch (const std::exception& e)
    {
        if (blocked)
        {
            replication_area_unlock();
        }
        std::cerr << "ip_replication_data_processor::write_replicaion_area() : exception : error " << e.what() << "." << std::endl;
        boost::format formatter("function : void ip_replication_data_processor::write_replicaion_area() : "
                                 "exception : error = %s.");
        formatter % e.what();
        putLogError(600114, formatter.str(), __FILE__, __LINE__);
    }
    catch (boost::thread_interrupted&)
    {
        if (blocked)
        {
            replication_area_unlock();
        }
        std::cerr << "ip_replication_data_processor::write_replicaion_area() : exception." << std::endl;
        putLogError(600115, "function : void ip_replication_data_processor::write_replicaion_area() : "
                     "Thread_interrupted exception.", __FILE__, __LINE__);
    }
    catch (...)
    {
        if (blocked)
        {
            replication_area_unlock();
        }
        std::cerr << "ip_replication_data_processor::write_replicaion_area() : Unknown exception." << std::endl;
        putLogError(600116, "function : void ip_replication_data_processor::write_replicaion_area() : "
                     "Unknown exception.", __FILE__, __LINE__);
    }

    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        putLogDebug(600220, "out_function : void ip_replication_data_processor::"
                     "write_replicaion_area().",  __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/
}

//! return the replication area pointer
//! @return ip_replication_data pointer
ip_replication_data* ip_replication_data_processor::get_replication_area()
{
    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("in/out_function : ip_replication_data* "
                                 "ip_replication_data_processor::"
                                 "get_replication_area() : return_value = &(%d).");
        formatter % static_cast<void*>(replication_area);
        putLogDebug(600221, formatter.str(),  __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    return replication_area;
}

//! register replication area lock function
//! @param[in] lock function object
void ip_replication_data_processor::register_replication_area_lock(
    boost::function<void(void)> intable_lock)
{
    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        putLogDebug(600222, "in_function : void ip_replication_data_processor::"
                     "register_replication_area_lock(boost::function<void(void)> intable_lock).",
                     __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    replication_area_lock = intable_lock;

    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        putLogDebug(600223, "out_function : void ip_replication_data_processor::"
                     "register_replication_area_lock(boost::function<void(void)> intable_lock).",
                     __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/
}

//! register replication area unlock function
//! @param[in] unlock function object
void ip_replication_data_processor::register_replication_area_unlock(
    boost::function<void(void)> intable_unlock)
{
    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        putLogDebug(600224, "in_function : void ip_replication_data_processor::"
                     "register_replication_area_unlock(boost::function<void(void)> intable_unlock).",
                     __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    replication_area_unlock = intable_unlock;

    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        putLogDebug(600225, "out_function : void ip_replication_data_processor::"
                     "register_replication_area_unlock(boost::function<void(void)> intable_unlock).",
                     __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/
}

//! get data from temp list
//! @param[out] ip_replication_temp_data refrence
//! @return  0 : success for get data
//! @return  -1 : faild to get data
int ip_replication_data_processor::get_from_temp_list(
    ip_replication_temp_data& data)
{
    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        putLogDebug(600226, "in_function : void ip_replication_data_processor::"
                     "get_from_temp_list(ip_replication_temp_data& data).", __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    int ret = 0;
    boost::mutex::scoped_lock sclock(temp_list_mutex);
    if (temp_list.size() <= 0)
    {
        ret = -1;
    }
    else
    {
        data = temp_list[0];
        temp_list.pop_front();

        temp_list_condition.notify_one();
    }

    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("out_function : void ip_replication_data_processor::"
                                "get_from_temp_list(ip_replication_temp_data& data) : "
                                "return_value = %d, data.op_code = %c, data.ip_hash = %d, "
                                "data.last_time = %lu, data.rs_endpoint = [%s]:%d.");
        formatter % ret % data.op_code % data.ip_hash % data.last_time %
        data.rs_endpoint.address().to_string() % data.rs_endpoint.port();
        putLogDebug(600227, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    return ret;
}

}
