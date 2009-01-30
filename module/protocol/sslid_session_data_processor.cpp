#include "sslid_session_data_processor.h"
#include "boost/format.hpp"

namespace l7vs
{

sslid_session_data_processor::sslid_session_data_processor(
                                    int maxlist,
                                    int timeout,
                                    sslid_replication_data_processor* replication_data_processor,
                                    getloglevel_func_type	ingetloglevel,
                                    logger_func_type inputLogFatal,
                                    logger_func_type inputLogError,
                                    logger_func_type inputLogWarn,
                                    logger_func_type inputLogInfo,
                                    logger_func_type inputLogDebug )
                                    :maxlist( maxlist ),
                                    timeout( timeout ),
                                    replication_data_processor( replication_data_processor ),
                                    getloglevel( ingetloglevel ),
                                    putLogFatal( inputLogFatal ),
                                    putLogError( inputLogError ),
                                    putLogWarn( inputLogWarn ),
                                    putLogInfo( inputLogInfo ),
                                    putLogDebug( inputLogDebug )
{
    //ctor
}

sslid_session_data_processor::~sslid_session_data_processor()
{
    //dtor
}

//! get endpoint from session data function
//! @param const std::string& session_id
//! @param boost::asio::ip::tcp::endpoint& endpoint
//! @return 0 : success 1: failed -1:exception
int sslid_session_data_processor::get_endpoint_from_session_data(
                                    const std::string& session_id,
                                    boost::asio::ip::tcp::endpoint& endpoint )
{
    return 0;
}

int sslid_session_data_processor::write_session_data(
                                    const std::string& session_id,
                                    const boost::asio::ip::tcp::endpoint& endpoint,
                                    time_t now_time )
{
   return 0;
}

int sslid_session_data_processor::read_session_data_from_replication_area(
                                                    sslid_replication_data* replication_area  )
{
  return 0;
}

int sslid_session_data_processor::clear_expired_session_data()
{
  return 0;
}

}
