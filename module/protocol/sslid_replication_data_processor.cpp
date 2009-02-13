#include "sslid_replication_data_processor.h"
#include "protocol_module_base.h"
#include "boost/format.hpp"

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
    if (LOG_LV_DEBUG == getloglevel())
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
        putLogDebug(30000, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    //ctor
    struct sslid_replication_data_header* head = NULL;
    struct sslid_replication_data_header* pick = NULL;
    unsigned int used = 0;
    char exist = -1;
    int srv_num = 0;

    // parameter check
    if (maxlist < 0 || sslid_replication_area_size < 0)
    {
        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == getloglevel())
        {
            putLogDebug(30001, "out_function : Constructor sslid_replication_data_processor::"
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

    if (sslid_replication_area_begain == NULL)
    {
        // replication area is null
        putLogError(37000, "Replication area is null.", __FILE__, __LINE__);
        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == getloglevel())
        {
            putLogDebug(30002, "function : sslid_replication_data_processor::"
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
                putLogError(37001, "Over replication area.", __FILE__, __LINE__);
                /*-------- DEBUG LOG --------*/
                if (LOG_LV_DEBUG == getloglevel())
                {
                    putLogDebug(30003, "function : sslid_replication_data_processor::"
                                 "sslid_replication_data_processor() : Over replication area.",
                                 __FILE__, __LINE__);
                    putLogDebug(30004, "out_function : Constructor sslid_replication_data_processor::"
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
                        putLogError(37002, "Replication area is full.", __FILE__, __LINE__);
                        /*-------- DEBUG LOG --------*/
                        if (LOG_LV_DEBUG == getloglevel())
                        {
                            putLogDebug(30005, "function : sslid_replication_data_processor::"
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
                        if (LOG_LV_DEBUG == getloglevel())
                        {
                            std::string datadump;
                            protocol_module_base::dump_memory(sslid_replication_area_begain+head->offset, head->size, datadump);

                            boost::format formatter("function : sslid_replication_data_processor::"
                                                    "sslid_replication_data_processor() : before memcpy (data dump) : "
                                                    "data begin = %d, data_size = %d, data = %s.");
                            formatter % head->offset % head->size % datadump;
                            putLogDebug(30006, formatter.str(), __FILE__, __LINE__);
                        }
                        /*------DEBUG LOG END------*/
                        memcpy(replication_area, sslid_replication_area_begain + head->offset, head->size);
                        /*-------- DEBUG LOG --------*/
                        if (LOG_LV_DEBUG == getloglevel())
                        {
                            char* pdata = reinterpret_cast<char*>(replication_area);
                            std::string datadump;
                            protocol_module_base::dump_memory(pdata, head->size, datadump);

                            boost::format formatter("function : sslid_replication_data_processor::"
                                                    "sslid_replication_data_processor() : after memcpy (data dump) : "
                                                    "data begin = 0, data_size = %d, data = %s.");
                            formatter % head->size % datadump;
                            putLogDebug(30007, formatter.str(), __FILE__, __LINE__);
                        }
                        /*------DEBUG LOG END------*/
                        memset(sslid_replication_area_begain + head->offset, 0, head->size);
                        strncpy(pick->virtualserver_ip, virtual_service_endpoint.address().to_string().c_str(),
                                sizeof(pick->virtualserver_ip)-1);
                        pick->virtualserver_ip[sizeof(pick->virtualserver_ip)] = '\0';
                        pick->virtualserver_port = virtual_service_endpoint.port();
                        pick->size = maxlist * sizeof(sslid_replication_data);
                        pick->offset = sizeof(sslid_replication_data_header) * SSLID_SERVICE_NUMBER +
                                       used;

                        /*-------- DEBUG LOG --------*/
                        if (LOG_LV_DEBUG == getloglevel())
                        {
                            boost::format formatter("Write replication area : head = (ip = %s, port = %d, "
                                                    "size = %d, offset = %d).");
                            formatter % pick->virtualserver_ip % pick->virtualserver_port
                            % pick->size % pick->offset;
                            putLogDebug(30008, formatter.str(), __FILE__, __LINE__);
                        }
                        /*------DEBUG LOG END------*/
                    }
                }
                else
                {
                    // replication area is full
                    putLogError(37003, "Replication area is full.", __FILE__, __LINE__);
                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        putLogDebug(30009, "function : sslid_replication_data_processor::"
                                     "sslid_replication_data_processor() : Replication area is full.",
                                     __FILE__, __LINE__);
                    }
                    /*------DEBUG LOG END------*/
                }
            }
            else
            {
                // restore session data from replication direct
                head->size = maxlist * sizeof(sslid_replication_data_header);
                /*-------- DEBUG LOG --------*/
                if (LOG_LV_DEBUG == getloglevel())
                {
                    boost::format formatter("Write replication area : head = (ip = %s, port = %d, "
                                             "size = %d, offset = %d).");
                    formatter % pick->virtualserver_ip % pick->virtualserver_port
                    % pick->size % pick->offset;
                    putLogDebug(30010, formatter.str(), __FILE__, __LINE__);
                }
                /*------DEBUG LOG END------*/
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
                    putLogError(37004, "Replication area is full.", __FILE__, __LINE__);
                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        putLogDebug(30011, "function : sslid_replication_data_processor::"
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
                    pick->virtualserver_ip[sizeof(pick->virtualserver_ip)] = '\0';
                    pick->virtualserver_port = virtual_service_endpoint.port();
                    pick->size = maxlist * sizeof(sslid_replication_data);
                    pick->offset = sizeof(sslid_replication_data_header) * SSLID_SERVICE_NUMBER +
                                   used;
                    /*-------- DEBUG LOG --------*/
                    if (LOG_LV_DEBUG == getloglevel())
                    {
                        boost::format formatter("Write replication area : head = (ip = %s, port = %d, "
                                                 "size = %d, offset = %d).");
                        formatter % pick->virtualserver_ip % pick->virtualserver_port
                        % pick->size % pick->offset;
                        putLogDebug(30012, formatter.str(), __FILE__, __LINE__);
                    }
                    /*------DEBUG LOG END------*/
                }
            }
            else
            {
                // replication area is full
                putLogError(37005, "Replication area is full.", __FILE__, __LINE__);
                /*-------- DEBUG LOG --------*/
                if (LOG_LV_DEBUG == getloglevel())
                {
                    putLogDebug(30013, "function : sslid_replication_data_processor::"
                                 "sslid_replication_data_processor() : Replication area is full.",
                                 __FILE__, __LINE__);
                }
                /*------DEBUG LOG END------*/
            }
        }
    }

    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        putLogDebug(30014, "out_function: Constructor sslid_replication_data_processor::"
                     "sslid_replication_data_processor(int maxlist, "
                     "char* sslid_replication_area_begain, int sslid_replication_area_size, "
                     "const boost::asio::ip::tcp::endpoint& virtual_service_endpoint, "
                     "getloglevel_func_type ingetloglevel, logger_func_type inputLogFatal, "
                     "logger_func_type inputLogError, logger_func_type inputLogWarn,"
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
    if (LOG_LV_DEBUG == getloglevel())
    {
        putLogDebug(30015, "in/out_function : Destructor sslid_replication_data_processor::"
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
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("in_function : void sslid_replication_data_processor::put_into_temp_list("
                                 "const sslid_replication_temp_data& data) : data.op_code = %c, "
                                 "data.session_id = %s, data.last_time = %lu, data.endpoint = [%s]:%d.");
        formatter % data.op_code % data.session_id % data. realserver_addr.address().to_string() %
        data. realserver_addr.port();
        putLogDebug(30016, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    boost::mutex::scoped_lock sclock(temp_list_mutex);
    while (temp_list.size() >= static_cast<size_t>(max_temp_list))
    {
        // wait for empty list item
        temp_list_condition.wait(sclock);
    }

    temp_list.push_back(data);
    temp_list_condition.notify_one();

    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        putLogDebug(30017, "out_function : void sslid_replication_data_processor::"
                     "put_into_temp_list(const sslid_replication_temp_data& data).", __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/
}

//! write replication area, called by protocol_module_sslid::replication_interrupt
void sslid_replication_data_processor::write_replicaion_area()
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        putLogDebug(30018, "in_function : void sslid_replication_data_processor::"
                     "write_replicaion_area().", __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    // maxlist check
    if (maxlist <= 0)
    {
        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == getloglevel())
        {
            putLogDebug(30019, "out_function : void sslid_replication_data_processor::"
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
            get_from_temp_list(temp_data);
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
                        if (LOG_LV_DEBUG == getloglevel())
                        {
                            char session_id_temp[SSLID_LENGTH+1] = {0};
                            memcpy(session_id_temp, replication_area[i].session_id, SSLID_LENGTH);
                            boost::format formatter("function : void sslid_replication_data_processor::"
                                                     "write_replicaion_area() : 'A' : session_id = %s.");
                            formatter % session_id_temp;
                            putLogDebug(30020, formatter.str(), __FILE__, __LINE__);
                        }
                        /*------DEBUG LOG END------*/

                        replication_area[i].last_time = temp_data.last_time;
                        strncpy(replication_area[i].realserver_ip,
                                 temp_data.realserver_addr.address().to_string().c_str(),
                                 sizeof(replication_area[i].realserver_ip)-1);
                        replication_area[i].realserver_port = temp_data.realserver_addr.port();

                        /*-------- DEBUG LOG --------*/
                        if (LOG_LV_DEBUG == getloglevel())
                        {
                            boost::format formatter("function : void sslid_replication_data_processor::"
                                                     "write_replicaion_area() : 'A' : realserver_ip = %s,"
                                                     "realserver_prot = %d.");
                            formatter % replication_area[i].realserver_ip % replication_area[i].realserver_port;
                            putLogDebug(30021, formatter.str(), __FILE__, __LINE__);
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
                        if (LOG_LV_DEBUG == getloglevel())
                        {
                            boost::format formatter("function : void sslid_replication_data_processor::"
                                                    "write_replicaion_area() : 'U' : last_time(before) = %lu,"
                                                    "last_time(after) = %lu.");
                            formatter % replication_area[i].last_time % temp_data.last_time;
                            putLogDebug(30022, formatter.str(), __FILE__, __LINE__);
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
                        if (LOG_LV_DEBUG == getloglevel())
                        {
                            boost::format formatter("function : void sslid_replication_data_processor::"
                                                     "write_replicaion_area() : 'D' : data valid(before) = %d,"
                                                     "data valid(after) = %d.");
                            formatter % replication_area[i].valid % 0;
                            putLogDebug(30023, formatter.str(), __FILE__, __LINE__);
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
        std::cerr << "write_replicaion_area exception : error " << e.what() << "." << std::endl;
        boost::format formatter("function : void sslid_replication_data_processor::write_replicaion_area() : "
                                 "exception : error = %s.");
        formatter % e.what();
        putLogError(37006, formatter.str(), __FILE__, __LINE__);
    }
    catch (boost::thread_interrupted&)
    {
        if (blocked)
        {
            replication_area_unlock();
        }
        std::cerr << "write_replicaion_area boost::thread_interrupted exception." << std::endl;
        putLogError(37007, "function : void sslid_replication_data_processor::write_replicaion_area() : "
                     "thread_interrupted exception.", __FILE__, __LINE__);
    }
    catch (...)
    {
        if (blocked)
        {
            replication_area_unlock();
        }
        std::cerr << "sslid_replication_data_processor::write_replicaion_area() : Unknown exception." << std::endl;
        putLogError(37008, "function : void sslid_replication_data_processor::write_replicaion_area() : "
                     "Unknown exception.", __FILE__, __LINE__);
    }

    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        putLogDebug(30024, "out_function : void sslid_replication_data_processor::"
                     "write_replicaion_area().",  __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/
}

//! return the replication area pointer
//! @return sslid_replication_data pointer
sslid_replication_data* sslid_replication_data_processor::get_replication_area()
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("in/out_function : sslid_replication_data* "
                                 "sslid_replication_data_processor::"
                                 "get_replication_area() : return_value = &(%d).");
        formatter % static_cast<void*>(replication_area);
        putLogDebug(30025, formatter.str(),  __FILE__, __LINE__);
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
    if (LOG_LV_DEBUG == getloglevel())
    {
        putLogDebug(30026, "in_function : void sslid_replication_data_processor::"
                     "register_replication_area_lock(boost::function<void(void)> intable_lock).",
                     __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    replication_area_lock = intable_lock;

    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        putLogDebug(30027, "out_function : void sslid_replication_data_processor::"
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
    if (LOG_LV_DEBUG == getloglevel())
    {
        putLogDebug(30028, "in_function : void sslid_replication_data_processor::"
                     "register_replication_area_unlock(boost::function<void(void)> intable_unlock).",
                     __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    replication_area_unlock = intable_unlock;

    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        putLogDebug(30029, "out_function : void sslid_replication_data_processor::"
                     "register_replication_area_unlock(boost::function<void(void)> intable_unlock).",
                     __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/
}

//! get data from temp list
//! @param[out] sslid_replication_temp_data refrence
void sslid_replication_data_processor::get_from_temp_list(
    sslid_replication_temp_data& data)
{
    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        putLogDebug(30030, "in_function : void sslid_replication_data_processor::"
                     "get_from_temp_list(sslid_replication_temp_data& data).", __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/

    boost::mutex::scoped_lock sclock(temp_list_mutex);
    while (temp_list.size() <= 0)
    {
        temp_list_condition.wait(sclock);
    }

    data = temp_list[0];
    temp_list.pop_front();

    temp_list_condition.notify_one();

    /*-------- DEBUG LOG --------*/
    if (LOG_LV_DEBUG == getloglevel())
    {
        boost::format formatter("out_function : void sslid_replication_data_processor::"
                                 "get_from_temp_list(sslid_replication_temp_data& data) : "
                                 "data.op_code = %c, data.session_id = %s, data.last_time = %lu, "
                                 "data.endpoint = [%s]:%d.");
        formatter % data.op_code % data.session_id % data. realserver_addr.address().to_string() %
        data. realserver_addr.port();
        putLogDebug(30031, formatter.str(), __FILE__, __LINE__);
    }
    /*------DEBUG LOG END------*/
}

}
