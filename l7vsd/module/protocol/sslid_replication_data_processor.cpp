/*
 * @file  sslid_replication_data_processor.cpp
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

#include "sslid_replication_data_processor.h"
#include "replication.h"
#include "protocol_module_base.h"
#include "ssl_protocol_module_base.h"
#include "boost/format.hpp"
#include "utility.h"

#define SSLID_SERVICE_NUMBER (128)

namespace l7vs
{

//! constructor
sslid_replication_data_processor::sslid_replication_data_processor(
    int maxlist, char* sslid_replication_area_begain, int sslid_replication_area_size,
    const boost::asio::ip::tcp::endpoint& virtual_service_endpoint,
    getloglevel_func_type ingetloglevel, logger_func_type inputLogFatal,
    logger_func_type inputLogError, logger_func_type inputLogWarn,
    logger_func_type inputLogInfo, logger_func_type inputLogDebug)
    :maxlist(maxlist), max_temp_list(2 * maxlist),
     replication_area(NULL), virtual_service_endpoint(virtual_service_endpoint),
     getloglevel(ingetloglevel), putLogFatal(inputLogFatal),
     putLogError(inputLogError), putLogWarn(inputLogWarn),
     putLogInfo(inputLogInfo), putLogDebug(inputLogDebug)
{
    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("in_function : Constructor sslid_replication_data_processor::"
                                 "sslid_replication_data_processor(int maxlist, "
                                 "char* sslid_replication_area_begain, int sslid_replication_area_size, "
                                 "const boost::asio::ip::tcp::endpoint& virtual_service_endpoint, "
                                 "getloglevel_func_type ingetloglevel, logger_func_type inputLogFatal, "
                                 "logger_func_type inputLogError, logger_func_type inputLogWarn, "
                                 "logger_func_type inputLogInfo, logger_func_type inputLogDebug) : "
                                 "maxlist = %d, sslid_replication_area_begain = &(%d), "
                                 "sslid_replication_area_size = %d, virtual_service_endpoint = [%s]:%d.");
        formatter % maxlist % static_cast<void*>(sslid_replication_area_begain)
        % sslid_replication_area_size % virtual_service_endpoint.address().to_string()
        % virtual_service_endpoint.port();
        putLogDebug(300197, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    //ctor
    struct sslid_replication_data_header* head = NULL;
    struct sslid_replication_data_header* pick = NULL;
    unsigned int used = 0;
    char exist = -1;
    int srv_num = 0;

    // parameter check
    if (unlikely(maxlist < 0 || sslid_replication_area_size < 0))
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            putLogDebug(300198, "out_function : Constructor sslid_replication_data_processor::"
                         "sslid_replication_data_processor(int maxlist, "
                         "char* sslid_replication_area_begain, int sslid_replication_area_size, "
                         "const boost::asio::ip::tcp::endpoint& virtual_service_endpoint, "
                         "getloglevel_func_type ingetloglevel, logger_func_type inputLogFatal, "
                         "logger_func_type inputLogError, logger_func_type inputLogWarn, "
                         "logger_func_type inputLogInfo, logger_func_type inputLogDebug).", __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/

        throw std::logic_error("Invalid parameter value.");
    }

    if (unlikely(sslid_replication_area_begain == NULL))
    {
        // replication area is null
        putLogInfo(300006, "Replication area is null.", __FILE__, __LINE__);
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            putLogDebug(300199, "function : sslid_replication_data_processor::"
                         "sslid_replication_data_processor() : Replication area is null.",  __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/
    }
    else
    {
        // check replication area header
        for (srv_num = 0; srv_num < SSLID_SERVICE_NUMBER; ++srv_num)
        {
            // get the replication data header pointer
            head = reinterpret_cast<sslid_replication_data_header*>(sslid_replication_area_begain) + srv_num;
            // add used service area size
            used += head->size;

            if (static_cast<size_t>(sslid_replication_area_size) * DATA_SIZE <
                    sizeof(sslid_replication_data_header) * SSLID_SERVICE_NUMBER + used)
            {
                // no enough replication area
                putLogError(300096, "Over replication area.", __FILE__, __LINE__);
                /*-------- DEBUG LOG --------*/
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    putLogDebug(300200, "function : sslid_replication_data_processor::"
                                 "sslid_replication_data_processor() : Over replication area.",
                                 __FILE__, __LINE__);
                    putLogDebug(300201, "out_function : Constructor sslid_replication_data_processor::"
                                 "sslid_replication_data_processor(int maxlist, "
                                 "char* sslid_replication_area_begain, int sslid_replication_area_size, "
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

            // match ip and port, this is pre-used session information
            if (virtual_service_endpoint.address().to_string() == head->virtualserver_ip &&
                    virtual_service_endpoint.port() == head->virtualserver_port)
            {
                exist = srv_num;
            }
        }

        if (exist >= 0)
        {
            head = reinterpret_cast<sslid_replication_data_header*>(sslid_replication_area_begain) + exist;
            // restore session information
            if (maxlist * sizeof(sslid_replication_data) > head->size)
            {
                // resize if maxlist set bigger than old
                if (pick != NULL)
                {
                    // area size check
                    if (sizeof(sslid_replication_data_header) * SSLID_SERVICE_NUMBER + used +
                            maxlist * sizeof(sslid_replication_data) >
                            static_cast<size_t>(sslid_replication_area_size) * DATA_SIZE)
                    {
                        // replication area is full
                        putLogError(300097, "Replication area is full.", __FILE__, __LINE__);
                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            putLogDebug(300202, "function : sslid_replication_data_processor::"
                                        "sslid_replication_data_processor() : Replication area is full.",
                                        __FILE__, __LINE__);
                        }
                        /*------DEBUG LOG END------*/
                    }
                    else
                    {
                        replication_area = reinterpret_cast<sslid_replication_data*>(sslid_replication_area_begain +
                                           sizeof(sslid_replication_data_header) * SSLID_SERVICE_NUMBER +
                                           used);
                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            std::string datadump;
                            protocol_module_base::dump_memory(sslid_replication_area_begain+head->offset, head->size, datadump);

                            boost::format formatter("function : sslid_replication_data_processor::"
                                                    "sslid_replication_data_processor() : before memcpy (data dump) : "
                                                    "data begin = %d, data_size = %d, data = %s");
                            formatter % head->offset % head->size % datadump;
                            putLogDebug(300203, formatter.str(), __FILE__, __LINE__);
                        }
                        /*------DEBUG LOG END------*/
                        memcpy(replication_area, sslid_replication_area_begain + head->offset, head->size);
                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            char* pdata = reinterpret_cast<char*>(replication_area);
                            std::string datadump;
                            protocol_module_base::dump_memory(pdata, head->size, datadump);

                            boost::format formatter("function : sslid_replication_data_processor::"
                                                    "sslid_replication_data_processor() : after memcpy (data dump) : "
                                                    "data begin = 0, data_size = %d, data = %s");
                            formatter % head->size % datadump;
                            putLogDebug(300204, formatter.str(), __FILE__, __LINE__);
                        }
                        /*------DEBUG LOG END------*/
                        memset(sslid_replication_area_begain + head->offset, 0, head->size);
                        strncpy(pick->virtualserver_ip, virtual_service_endpoint.address().to_string().c_str(),
                                sizeof(pick->virtualserver_ip)-1);
                        pick->virtualserver_ip[sizeof(pick->virtualserver_ip)-1] = '\0';
                        pick->virtualserver_port = virtual_service_endpoint.port();
                        pick->size = maxlist * sizeof(sslid_replication_data);
                        pick->offset = sizeof(sslid_replication_data_header) * SSLID_SERVICE_NUMBER +
                                       used;

                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            boost::format formatter("Write replication area : head = (ip = %s, port = %d, "
                                                    "size = %d, offset = %d).");
                            formatter % pick->virtualserver_ip % pick->virtualserver_port
                            % pick->size % pick->offset;
                            putLogDebug(300205, formatter.str(), __FILE__, __LINE__);
                        }
                        /*------DEBUG LOG END------*/
                    }
                }
                else
                {
                    // replication area is full
                    putLogError(300098, "Replication area is full.", __FILE__, __LINE__);
                    /*-------- DEBUG LOG --------*/
                    if (unlikely(LOG_LV_DEBUG == getloglevel()))
                    {
                        putLogDebug(300206, "function : sslid_replication_data_processor::"
                                     "sslid_replication_data_processor() : Replication area is full.",
                                     __FILE__, __LINE__);
                    }
                    /*------DEBUG LOG END------*/
                }
            }
            else
            {
                // restore session data from replication direct
                head->size = maxlist * sizeof(sslid_replication_data);
                /*-------- DEBUG LOG --------*/
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    boost::format formatter("Write replication area : head = (ip = %s, port = %d, "
                                             "size = %d, offset = %d).");
                    formatter % pick->virtualserver_ip % pick->virtualserver_port
                    % pick->size % pick->offset;
                    putLogDebug(300207, formatter.str(), __FILE__, __LINE__);
                }
                /*------DEBUG LOG END------*/
                replication_area = reinterpret_cast<sslid_replication_data*>(sslid_replication_area_begain +
                                    head->offset);
            }
        }

        if (exist == -1)
        {
            if (pick != NULL)
            {
                // data size check
                if (sizeof(sslid_replication_data_header) * SSLID_SERVICE_NUMBER + used +
                        maxlist * sizeof(sslid_replication_data) >
                        static_cast<size_t>(sslid_replication_area_size) * DATA_SIZE)
                {
                    // replication area is full
                    putLogError(300099, "Replication area is full.", __FILE__, __LINE__);
                    /*-------- DEBUG LOG --------*/
                    if (unlikely(LOG_LV_DEBUG == getloglevel()))
                    {
                        putLogDebug(300208, "function : sslid_replication_data_processor::"
                                     "sslid_replication_data_processor() : Replication area is full.",
                                     __FILE__, __LINE__);
                    }
                    /*------DEBUG LOG END------*/
                }
                else
                {
                    // initialize replication information
                    replication_area = reinterpret_cast<sslid_replication_data*>(sslid_replication_area_begain +
                                       sizeof(sslid_replication_data_header) * SSLID_SERVICE_NUMBER +
                                       used);
                    strncpy(pick->virtualserver_ip, virtual_service_endpoint.address().to_string().c_str(),
                            sizeof(pick->virtualserver_ip)-1);
                    pick->virtualserver_ip[sizeof(pick->virtualserver_ip)-1] = '\0';
                    pick->virtualserver_port = virtual_service_endpoint.port();
                    pick->size = maxlist * sizeof(sslid_replication_data);
                    pick->offset = sizeof(sslid_replication_data_header) * SSLID_SERVICE_NUMBER +
                                   used;
                    /*-------- DEBUG LOG --------*/
                    if (unlikely(LOG_LV_DEBUG == getloglevel()))
                    {
                        boost::format formatter("Write replication area : head = (ip = %s, port = %d, "
                                                 "size = %d, offset = %d).");
                        formatter % pick->virtualserver_ip % pick->virtualserver_port
                        % pick->size % pick->offset;
                        putLogDebug(300209, formatter.str(), __FILE__, __LINE__);
                    }
                    /*------DEBUG LOG END------*/
                }
            }
            else
            {
                // replication area is full
                putLogError(300100, "Replication area is full.", __FILE__, __LINE__);
                /*-------- DEBUG LOG --------*/
                if (unlikely(LOG_LV_DEBUG == getloglevel()))
                {
                    putLogDebug(300210, "function : sslid_replication_data_processor::"
                                 "sslid_replication_data_processor() : Replication area is full.",
                                 __FILE__, __LINE__);
                }
                /*------DEBUG LOG END------*/
            }
        }
    }

    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        putLogDebug(300211, "out_function : Constructor sslid_replication_data_processor::"
                     "sslid_replication_data_processor(int maxlist, "
                     "char* sslid_replication_area_begain, int sslid_replication_area_size, "
                     "const boost::asio::ip::tcp::endpoint& virtual_service_endpoint, "
                     "getloglevel_func_type ingetloglevel, logger_func_type inputLogFatal, "
                     "logger_func_type inputLogError, logger_func_type inputLogWarn, "
                     "logger_func_type inputLogInfo, logger_func_type inputLogDebug)."
                     , __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/
}

//! destructor
sslid_replication_data_processor::~sslid_replication_data_processor()
{
    //dtor
    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        putLogDebug(300212, "in/out_function : Destructor sslid_replication_data_processor::"
                     "~sslid_replication_data_processor().", __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/
}

//! put data into temp list
//! @param[in] sslid_replication_temp_data refrence
void sslid_replication_data_processor::put_into_temp_list(
    const sslid_replication_temp_data& data)
{
    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        std::string buffer;
        ssl_protocol_module_base::dump_session_id(data.session_id.c_str(), data.session_id.size(), buffer);
        boost::format formatter("in_function : void sslid_replication_data_processor::put_into_temp_list("
                                 "const sslid_replication_temp_data& data) : data.op_code = %c, "
                                 "data.session_id = %s, data.last_time = %lu, data.endpoint = [%s]:%d.");
        formatter % data.op_code % buffer % data.last_time % data.realserver_addr.address().to_string()
        % data.realserver_addr.port();
        putLogDebug(300213, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    if (replication_area)
    {
        boost::mutex::scoped_lock sclock(temp_list_mutex);
        while (temp_list.size() >= static_cast<size_t>(max_temp_list))
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
        putLogDebug(300214, "out_function : void sslid_replication_data_processor::"
                     "put_into_temp_list(const sslid_replication_temp_data& data).", __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/
}

//! write replication area, called by protocol_module_sslid::replication_interrupt
void sslid_replication_data_processor::write_replicaion_area()
{
    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        putLogDebug(300215, "in_function : void sslid_replication_data_processor::"
                     "write_replicaion_area().", __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    // maxlist and replication_area check
    if (maxlist <= 0 || replication_area == NULL)
    {
        /*-------- DEBUG LOG --------*/
        if (unlikely(LOG_LV_DEBUG == getloglevel()))
        {
            putLogDebug(300216, "out_function : void sslid_replication_data_processor::"
                         "write_replicaion_area().", __FILE__, __LINE__);
        }
        /*------DEBUG LOG END------*/
        return;
    }

    bool blocked = false;
    try
    {
        sslid_replication_temp_data temp_data;
        int i = 0;

        while (true)
        {
            if (get_from_temp_list(temp_data) == -1)
            {
                break;
            }
            blocked = false;
            replication_area_lock();
            blocked = true;

            // session data process
            switch (temp_data.op_code)
            {
            case 'A':
                // data add
                for (i = 0; i < maxlist; ++i)
                {
                    if (replication_area[i].valid == 0)
                    {
                        // empty item
                        memset(&replication_area[i], 0, sizeof(replication_area[i]));
                        memcpy(replication_area[i].session_id, temp_data.session_id.c_str(), SSLID_LENGTH);

                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            char session_id_temp[SSLID_LENGTH+1] = {0};
                            memcpy(session_id_temp, replication_area[i].session_id, SSLID_LENGTH);
                            std::string buffer;
                            ssl_protocol_module_base::dump_session_id(session_id_temp, SSLID_LENGTH, buffer);
                            boost::format formatter("function : void sslid_replication_data_processor::"
                                                    "write_replicaion_area() : 'A' : session_id = %s.");
                            formatter % buffer;
                            putLogDebug(300217, formatter.str(), __FILE__, __LINE__);
                        }
                        /*------DEBUG LOG END------*/

                        replication_area[i].last_time = temp_data.last_time;
                        strncpy(replication_area[i].realserver_ip,
                                 temp_data.realserver_addr.address().to_string().c_str(),
                                 sizeof(replication_area[i].realserver_ip)-1);
                        replication_area[i].realserver_port = temp_data.realserver_addr.port();

                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            boost::format formatter("function : void sslid_replication_data_processor::"
                                                     "write_replicaion_area() : 'A' : realserver_ip = %s, "
                                                     "realserver_prot = %d.");
                            formatter % replication_area[i].realserver_ip % replication_area[i].realserver_port;
                            putLogDebug(300218, formatter.str(), __FILE__, __LINE__);
                        }
                        /*------DEBUG LOG END------*/

                        replication_area[i].valid = 1;
                        break;
                    }
                }
                break;
            case 'U':
                // data update
                for (i = 0; i < maxlist; ++i)
                {
                    if (replication_area[i].valid == 1 &&
                        memcmp(replication_area[i].session_id, temp_data.session_id.c_str(), SSLID_LENGTH) == 0)
                    {
                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            boost::format formatter("function : void sslid_replication_data_processor::"
                                                    "write_replicaion_area() : 'U' : last_time(before) = %lu, "
                                                    "last_time(after) = %lu.");
                            formatter % replication_area[i].last_time % temp_data.last_time;
                            putLogDebug(300219, formatter.str(), __FILE__, __LINE__);
                        }
                        /*------DEBUG LOG END------*/

                        // same data
                        replication_area[i].last_time = temp_data.last_time;
                        strncpy(replication_area[i].realserver_ip,
                                 temp_data.realserver_addr.address().to_string().c_str(),
                                 sizeof(replication_area[i].realserver_ip)-1);
                        replication_area[i].realserver_port = temp_data.realserver_addr.port();
                        break;
                    }
                }
                break;
            case 'D':
                // data delete
                for (i = 0; i < maxlist; ++i)
                {
                    if (replication_area[i].valid == 1 &&
                        memcmp(replication_area[i].session_id, temp_data.session_id.c_str(), SSLID_LENGTH) == 0)
                    {
                        /*-------- DEBUG LOG --------*/
                        if (unlikely(LOG_LV_DEBUG == getloglevel()))
                        {
                            boost::format formatter("function : void sslid_replication_data_processor::"
                                                     "write_replicaion_area() : 'D' : data valid(before) = %d, "
                                                     "data valid(after) = %d.");
                            formatter % replication_area[i].valid % 0;
                            putLogDebug(300220, formatter.str(), __FILE__, __LINE__);
                        }
                        /*------DEBUG LOG END------*/

                        // same data
                        replication_area[i].valid = 0;
                        break;
                    }
                }
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
        std::cerr << "sslid_replication_data_processor::write_replicaion_area() : exception : error " << e.what() << "." << std::endl;
        boost::format formatter("function : void sslid_replication_data_processor::write_replicaion_area() : "
                                 "exception : error = %s.");
        formatter % e.what();
        putLogError(300101, formatter.str(), __FILE__, __LINE__);
    }
    catch (boost::thread_interrupted&)
    {
        if (blocked)
        {
            replication_area_unlock();
        }
        std::cerr << "sslid_replication_data_processor::write_replicaion_area() : exception." << std::endl;
        putLogError(300102, "function : void sslid_replication_data_processor::write_replicaion_area() : "
                     "Thread_interrupted exception.", __FILE__, __LINE__);
    }
    catch (...)
    {
        if (blocked)
        {
            replication_area_unlock();
        }
        std::cerr << "sslid_replication_data_processor::write_replicaion_area() : Unknown exception." << std::endl;
        putLogError(300103, "function : void sslid_replication_data_processor::write_replicaion_area() : "
                     "Unknown exception.", __FILE__, __LINE__);
    }

    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        putLogDebug(300221, "out_function : void sslid_replication_data_processor::"
                     "write_replicaion_area().",  __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/
}

//! return the replication area pointer
//! @return sslid_replication_data pointer
sslid_replication_data* sslid_replication_data_processor::get_replication_area()
{
    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        boost::format formatter("in/out_function : sslid_replication_data* "
                                 "sslid_replication_data_processor::"
                                 "get_replication_area() : return_value = &(%d).");
        formatter % static_cast<void*>(replication_area);
        putLogDebug(300222, formatter.str(),  __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    return replication_area;
}

//! register replication area lock function
//! @param[in] lock function object
void sslid_replication_data_processor::register_replication_area_lock(
    boost::function<void(void)> intable_lock)
{
    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        putLogDebug(300223, "in_function : void sslid_replication_data_processor::"
                     "register_replication_area_lock(boost::function<void(void)> intable_lock).",
                     __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    replication_area_lock = intable_lock;

    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        putLogDebug(300224, "out_function : void sslid_replication_data_processor::"
                     "register_replication_area_lock(boost::function<void(void)> intable_lock).",
                     __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/
}

//! register replication area unlock function
//! @param[in] unlock function object
void sslid_replication_data_processor::register_replication_area_unlock(
    boost::function<void(void)> intable_unlock)
{
    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        putLogDebug(300225, "in_function : void sslid_replication_data_processor::"
                     "register_replication_area_unlock(boost::function<void(void)> intable_unlock).",
                     __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    replication_area_unlock = intable_unlock;

    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        putLogDebug(300226, "out_function : void sslid_replication_data_processor::"
                     "register_replication_area_unlock(boost::function<void(void)> intable_unlock).",
                     __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/
}

//! get data from temp list
//! @param[out] sslid_replication_temp_data refrence
//! @return  0 : success for get data
//! @return  -1 : faild to get data
int sslid_replication_data_processor::get_from_temp_list(
    sslid_replication_temp_data& data)
{
    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == getloglevel()))
    {
        putLogDebug(300227, "in_function : void sslid_replication_data_processor::"
                     "get_from_temp_list(sslid_replication_temp_data& data).", __FILE__, __LINE__);
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
        std::string buffer;
        ssl_protocol_module_base::dump_session_id(data.session_id.c_str(), data.session_id.size(), buffer);
        boost::format formatter("out_function : void sslid_replication_data_processor::"
                                "get_from_temp_list(sslid_replication_temp_data& data) : "
                                "return_value = %d, data.op_code = %c, data.session_id = %s, "
                                "data.last_time = %lu, data.endpoint = [%s]:%d.");
        formatter % ret % data.op_code % buffer % data.last_time %
        data.realserver_addr.address().to_string() % data.realserver_addr.port();
        putLogDebug(300228, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    return ret;
}

}
