#ifndef SSLID_SESSION_DATA_PROCESSOR_H
#define SSLID_SESSION_DATA_PROCESSOR_H

#include "sslid_replication_data_processor.h"
#include "logger_enum.h"
#include <boost/asio.hpp>
#include <string>

namespace l7vs
{

class sslid_session_data_processor
{
    public:
        //! loglevel get function object type
        typedef    boost::function< LOG_LEVEL_TAG(void) >
                                        getloglevel_func_type;
        //! log output function object type
        typedef    boost::function< void ( const unsigned int, const std::string&, const char*, int ) >
                                        logger_func_type;

    public:
        sslid_session_data_processor(
                            int maxlist,
                            int timeout,
                            sslid_replication_data_processor* replication_data_processor,
                            getloglevel_func_type    ingetloglevel,
                            logger_func_type inputLogFatal,
                            logger_func_type inputLogError,
                            logger_func_type inputLogWarn,
                            logger_func_type inputLogInfo,
                            logger_func_type inputLogDebug );

        virtual ~sslid_session_data_processor();

        int get_endpoint_from_session_data(
                            const std::string& session_id,
                            boost::asio::ip::tcp::endpoint& endpoint );

        int write_session_data(
                            const std::string& session_id,
                            const boost::asio::ip::tcp::endpoint& endpoint,
                            time_t now_time );

        int read_session_data_from_replication_area( sslid_replication_data* replication_area );

    protected:
       int clear_expired_session_data();

    protected:
        int maxlist;
        int timeout;
        std::map<std::string, boost::asio::ip::tcp::endpoint> session_endpoint_map;
        std::map<std::string, time_t> session_lasttime_map;
        std::multimap<time_t, std::string> lasttime_session_map;
        boost::mutex session_map_mutex;
        sslid_replication_data_processor* replication_data_processor;
        getloglevel_func_type     getloglevel;        //!< get loglevel function object
        logger_func_type putLogFatal;                //!< fatal log output function object
        logger_func_type putLogError;                //!< error log output function object
        logger_func_type putLogWarn;                //!< warn log output function object
        logger_func_type putLogInfo;                //!< info log output function object
        logger_func_type putLogDebug;            //!< debug log output function object
    private:
};

}
#endif // SSLID_SESSION_DATA_PROCESSOR_H
