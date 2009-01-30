#include "sslid_replication_data_processor.h"
#include "boost/format.hpp"

#define SSLID_SERVICE_NUMBER (128)

namespace l7vs
{

sslid_replication_data_processor::sslid_replication_data_processor(
                                                int maxlist,
                                                char* sslid_replication_area_begain,
                                                int sslid_replication_area_size,
                                                const boost::asio::ip::tcp::endpoint& virtual_service_endpoint,
                                                getloglevel_func_type	ingetloglevel,
                                                logger_func_type inputLogFatal,
                                                logger_func_type inputLogError,
                                                logger_func_type inputLogWarn,
                                                logger_func_type inputLogInfo,
                                                logger_func_type inputLogDebug ) :
                                                maxlist( maxlist ),
                                                max_temp_list( 2 * maxlist ),
                                                replication_area( NULL ),
                                                virtual_service_endpoint( virtual_service_endpoint ),
                                                getloglevel( ingetloglevel ),
                                                putLogFatal( inputLogFatal ),
                                                putLogError( inputLogError ),
                                                putLogWarn( inputLogWarn ),
                                                putLogInfo( inputLogInfo ),
                                                putLogDebug( inputLogDebug )
{

}

sslid_replication_data_processor::~sslid_replication_data_processor()
{
    //dtor
}

void sslid_replication_data_processor::put_into_temp_list(
                                                const sslid_replication_temp_data& data )
{

}

void sslid_replication_data_processor::write_replicaion_area()
{

}

sslid_replication_data* sslid_replication_data_processor::get_replication_area()
{
    return NULL;
}

void sslid_replication_data_processor::register_replication_area_lock(
                                                boost::function<void(void)> intable_lock )
{

}

void sslid_replication_data_processor::register_replication_area_unlock(
                                                boost::function<void(void)> intable_unlock )
{

}

void sslid_replication_data_processor::get_from_temp_list(
                                                sslid_replication_temp_data& data )
{

}

}
