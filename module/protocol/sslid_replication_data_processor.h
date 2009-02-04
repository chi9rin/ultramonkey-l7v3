#ifndef SSLID_REPLICATION_DATA_PROCESSOR_H
#define SSLID_REPLICATION_DATA_PROCESSOR_H

#include <boost/asio.hpp>
#include <boost/thread/condition.hpp>
#include "logger_enum.h"

#define SSLID_LENGTH (32)
#define SSLID_IP_MAXLENGTH (40)

namespace l7vs
{

struct sslid_replication_data_header
{
    char virtualserver_ip[SSLID_IP_MAXLENGTH];
    unsigned short virtualserver_port;
    size_t offset;
    size_t size;
};

struct sslid_replication_data
{
    char session_id[SSLID_LENGTH];
    char realserver_ip[SSLID_IP_MAXLENGTH];
    unsigned short realserver_port;
    time_t last_time;
    int valid;
};

struct sslid_replication_temp_data
{
    char op_code;
    std::string session_id;
    time_t last_time;
    boost::asio::ip::tcp::endpoint realserver_addr;
};

class sslid_replication_data_processor
{
    public:
        //! loglevel get function object type
        typedef	boost::function< LOG_LEVEL_TAG(void) >
                                        getloglevel_func_type;
        //! log output function object type
        typedef	boost::function< void ( const unsigned int, const std::string&, const char*, int ) >
                                        logger_func_type;

    public:
        sslid_replication_data_processor( int maxlist,
                                                        char* sslid_replication_area_begain,
                                                        int sslid_replication_area_size,
                                                        const boost::asio::ip::tcp::endpoint& virtual_service_endpoint,
                                                        getloglevel_func_type	ingetloglevel,
                                                        logger_func_type inputLogFatal,
                                                        logger_func_type inputLogError,
                                                        logger_func_type inputLogWarn,
                                                        logger_func_type inputLogInfo,
                                                        logger_func_type inputLogDebug );

        virtual ~sslid_replication_data_processor();

        void put_into_temp_list( const sslid_replication_temp_data& data );

        void write_replicaion_area();

        sslid_replication_data* get_replication_area();

        void register_replication_area_lock( boost::function<void(void)> intable_lock );

        void register_replication_area_unlock( boost::function<void(void)> intable_unlock );

    protected:
        void get_from_temp_list( sslid_replication_temp_data& data );

    protected:
        int maxlist;
        int max_temp_list;// : int = 2*maxlist
        std::deque<sslid_replication_temp_data> temp_list;
        boost::mutex temp_list_mutex;
        boost::condition temp_list_condition;
        sslid_replication_data* replication_area;
        boost::function<void(void)> replication_area_lock;
        boost::function<void(void)> replication_area_unlock;
        boost::asio::ip::tcp::endpoint virtual_service_endpoint;
        getloglevel_func_type	 getloglevel;	    //!< get loglevel function object
        logger_func_type putLogFatal;	            //!< fatal log output function object
        logger_func_type putLogError;	            //!< error log output function object
        logger_func_type putLogWarn;		        //!< warn log output function object
        logger_func_type putLogInfo;		        //!< info log output function object
        logger_func_type putLogDebug;	        //!< debug log output function object
};

}

#endif // SSLID_REPLICATION_DATA_PROCESSOR_H
