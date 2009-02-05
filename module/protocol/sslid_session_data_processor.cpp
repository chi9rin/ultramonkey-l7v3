#include "sslid_session_data_processor.h"
#include "boost/format.hpp"

namespace l7vs
{
//! constructor
sslid_session_data_processor::sslid_session_data_processor(
                                    int maxlist,
                                    int timeout,
                                    sslid_replication_data_processor* replication_data_processor,
                                    getloglevel_func_type ingetloglevel,
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
    /*-------- DEBUG LOG --------*/
    if ( LOG_LV_DEBUG == getloglevel() )
    {
        boost::format formatter( "in_function: Constructor sslid_session_data_processor::"
                                            "sslid_session_data_processor(int maxlist,"
                                            "int timeout, sslid_replication_data_processor* replication_data_processor,"
                                            "getloglevel_func_type	ingetloglevel, logger_func_type		inputLogFatal,"
                                            "logger_func_type		inputLogError, logger_func_type		inputLogWarn,"
                                            "logger_func_type		inputLogInfo, logger_func_type		inputLogDebug ): "
                                            "maxlist=%d, timeout=%d, replication_data_processor=&(%d)." );
        formatter % maxlist % timeout % reinterpret_cast<int>(replication_data_processor);
        putLogDebug( 30000, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    // parameter check
    if ( maxlist < 0 || timeout < 0 || replication_data_processor == NULL )
    {
        putLogDebug( 30000, "out_function: Constructor sslid_session_data_processor::"
                            "sslid_session_data_processor(int maxlist,"
                            "int timeout, sslid_replication_data_processor* replication_data_processor,"
                            "getloglevel_func_type	ingetloglevel, logger_func_type		inputLogFatal,"
                            "logger_func_type		inputLogError, logger_func_type		inputLogWarn,"
                            "logger_func_type		inputLogInfo, logger_func_type		inputLogDebug )."
                            , __FILE__, __LINE__ );
        // waiting for jp response??????????????????
        throw std::logic_error("invalid parameter value.");
    }

    /*-------- DEBUG LOG --------*/
    if ( LOG_LV_DEBUG == getloglevel() )
    {
        putLogDebug( 30000, "out_function: Constructor sslid_session_data_processor::"
                            "sslid_session_data_processor(int maxlist,"
                            "int timeout, sslid_replication_data_processor* replication_data_processor,"
                            "getloglevel_func_type	ingetloglevel, logger_func_type		inputLogFatal,"
                            "logger_func_type		inputLogError, logger_func_type		inputLogWarn,"
                            "logger_func_type		inputLogInfo, logger_func_type		inputLogDebug )."
                            , __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/
}

//! destructor
sslid_session_data_processor::~sslid_session_data_processor()
{
    //dtor
    /*-------- DEBUG LOG --------*/
    if ( LOG_LV_DEBUG == getloglevel() )
    {
        putLogDebug( 30000, "in/out_function: Destructor sslid_session_data_processor::"
                            "~sslid_session_data_processor()", __FILE__, __LINE__ );
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
                                    boost::asio::ip::tcp::endpoint& endpoint )
{
    /*-------- DEBUG LOG --------*/
    if ( LOG_LV_DEBUG == getloglevel() )
    {
        boost::format formatter( "in_function: int sslid_session_data_processor::"
                                            "get_endpoint_from_session_data( const std::string& session_id,"
                                            "boost::asio::ip::tcp::endpoint& endpoint ): "
                                            "session_id=%s, endpoint=[%s]:%d" );
        formatter % session_id % endpoint.address().to_string() % endpoint.port();
        putLogDebug( 30000, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    // lock the session map
    boost::mutex::scoped_lock sclock( session_map_mutex );
    int ret = 0;

    try
    {
        std::map<std::string, boost::asio::ip::tcp::endpoint>::iterator itendpoint;
        std::map<std::string, time_t>::iterator itlasttime;
        time_t oldtime = 0;
        boost::asio::ip::tcp::endpoint temp_endpoint;

        itendpoint = session_endpoint_map.find( session_id );
        itlasttime = session_lasttime_map.find( session_id );
        // can't get the data
        if ( itendpoint == session_endpoint_map.end() ||
             itlasttime == session_lasttime_map.end() )
        {
            /*-------- DEBUG LOG --------*/
            if ( LOG_LV_DEBUG == getloglevel() )
            {
                putLogDebug( 30000, "out_function: int sslid_session_data_processor::"
                                    "get_endpoint_from_session_data( const std::string& session_id,"
                                    "boost::asio::ip::tcp::endpoint& endpoint ): return_value=1",
                                    __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/

            return 1;
        }
        oldtime = itlasttime->second;
        temp_endpoint = itendpoint->second;
        sclock.unlock();

        // expired time check
        time_t now;
        time(&now);
        if ( now - oldtime <= timeout )
        {
            // time in
             endpoint = temp_endpoint;
        }
        else
        {
            // time out
            sclock.lock();
            // delete session from the map
            std::multimap<time_t, std::string>::iterator itbegin;
            std::multimap<time_t, std::string>::iterator itend;
            itbegin = lasttime_session_map.lower_bound(oldtime);
            itend = lasttime_session_map.upper_bound(oldtime);

            while ( itbegin != itend )
            {
                if ( itbegin->first == oldtime && itbegin->second == session_id )
                {
                    lasttime_session_map.erase( itbegin );
                    break;
                }
                ++itbegin;
            }
            session_endpoint_map.erase( session_id );
            session_lasttime_map.erase( session_id );
            sclock.unlock();

            // update the replication list
            sslid_replication_temp_data temp_data;
            temp_data.op_code = 'D';
            temp_data.session_id = session_id;

            replication_data_processor->put_into_temp_list( temp_data );
            /*-------- DEBUG LOG --------*/
            if ( LOG_LV_DEBUG == getloglevel() )
            {
                putLogDebug( 30000, "function: int sslid_session_data_processor::"
                                    "get_endpoint_from_session_data():put_into_temp_list() --delete item-- END.",
                                    __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/
        }
    }
    catch( const std::exception& e )
    {
        std::cerr << "get_endpoint_from_session_data exception: error=%s" << e.what() << "." << std::endl;
        boost::format formatter( "function: int sslid_session_data_processor::"
                                            "get_endpoint_from_session_data() exception: result=%d, error=%s." );
        formatter % -1 % e.what();
        putLogError( 37000, formatter.str(), __FILE__, __LINE__ );

        ret = -1;
    }

    /*-------- DEBUG LOG --------*/
    if ( LOG_LV_DEBUG == getloglevel() )
    {
        boost::format formatter( "out_function: int sslid_session_data_processor::"
                            "get_endpoint_from_session_data( const std::string& session_id,"
                            "boost::asio::ip::tcp::endpoint& endpoint ): return_value=%d." );
        formatter % ret;
        putLogDebug( 30000, formatter.str(), __FILE__, __LINE__ );
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
                                    time_t now_time )
{
    /*-------- DEBUG LOG --------*/
    if ( LOG_LV_DEBUG == getloglevel() )
    {
        boost::format formatter( "in_function: int sslid_session_data_processor::"
                                            "write_session_data( const std::string& session_id,"
                                            "const boost::asio::ip::tcp::endpoint& endpoint, "
                                            "time_t now_time ): session_id=%s, "
                                            "endpoint=[%s]:%d, now_time=%lu" );
        formatter % session_id % endpoint.address().to_string() % endpoint.port() % now_time;
        putLogDebug( 30000, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

     // lock the session map
    boost::mutex::scoped_lock sclock( session_map_mutex );
    try
    {
        // map size check
        if ( maxlist <= 0 )
        {
             /*-------- DEBUG LOG --------*/
            if ( LOG_LV_DEBUG == getloglevel() )
            {
                putLogDebug( 30000, "out_function: int sslid_session_data_processor::"
                                "write_session_data( const std::string& session_id,"
                                "const boost::asio::ip::tcp::endpoint& endpoint,  time_t now_time ): "
                                "return_value=0", __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/
            return 0;
        }

        std::map<std::string, boost::asio::ip::tcp::endpoint>::iterator itendpoint;
        itendpoint = session_endpoint_map.find( session_id );

        if ( itendpoint != session_endpoint_map.end() )
        {
            // endpoint exist
            session_endpoint_map[session_id] = endpoint;
            session_lasttime_map[session_id] = now_time;
            lasttime_session_map.insert( std::make_pair(now_time, session_id) );

            sclock.unlock();

            // update the replication list
            sslid_replication_temp_data temp_data;
            temp_data.op_code = 'U';
            temp_data.session_id = session_id;
            temp_data.realserver_addr = endpoint;
            temp_data.last_time = now_time;
            replication_data_processor->put_into_temp_list( temp_data );
            /*-------- DEBUG LOG --------*/
            if ( LOG_LV_DEBUG == getloglevel() )
            {
                putLogDebug( 30000, "function: int sslid_session_data_processor::"
                                    "write_session_data():put_into_temp_list() --update item-- END.",
                                    __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/
        }
        else
        {
            // endpoint not exist
            if ( session_endpoint_map.size() >= static_cast<size_t>(maxlist) )
            {
                // map size arrived to top
                if ( clear_expired_session_data() == -1 )
                {
                    /*-------- DEBUG LOG --------*/
                    if ( LOG_LV_DEBUG == getloglevel() )
                    {
                        putLogDebug( 30000, "out_function: int sslid_session_data_processor::"
                                        "write_session_data( const std::string& session_id,"
                                        "const boost::asio::ip::tcp::endpoint& endpoint,  time_t now_time ): "
                                        "return_value=-1", __FILE__, __LINE__ );
                    }
                    /*------DEBUG LOG END------*/

                    return -1;
                }

                /*-------- DEBUG LOG --------*/
                if ( LOG_LV_DEBUG == getloglevel() )
                {
                    putLogDebug( 30000, "function: int sslid_session_data_processor::"
                                        "write_session_data():clear_expired_session_data() END.",
                                        __FILE__, __LINE__ );
                }
                /*------DEBUG LOG END------*/
            }

            session_endpoint_map[session_id] = endpoint;
            session_lasttime_map[session_id] = now_time;
            lasttime_session_map.insert( std::make_pair(now_time, session_id) );

            sclock.unlock();

            // add the replication list
            sslid_replication_temp_data temp_data;
            temp_data.op_code = 'A';
            temp_data.session_id = session_id;
            temp_data.realserver_addr = endpoint;
            temp_data.last_time = now_time;
            replication_data_processor->put_into_temp_list( temp_data );
            /*-------- DEBUG LOG --------*/
            if ( LOG_LV_DEBUG == getloglevel() )
            {
                putLogDebug( 30000, "function: int sslid_session_data_processor::"
                                    "write_session_data():put_into_temp_list() --add item-- END.",
                                    __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/
        }
    }
    catch ( const std::exception& e )
    {
        std::cerr << "write_session_data exception: error=%s" << e.what() << "." << std::endl;
        boost::format formatter( "function: int sslid_session_data_processor::"
                                            "write_session_data() exception: result=-1, error=%s." );
        formatter % e.what();
        putLogError( 37000, formatter.str(), __FILE__, __LINE__ );

        /*-------- DEBUG LOG --------*/
        if ( LOG_LV_DEBUG == getloglevel() )
        {
            putLogDebug( 30000, "out_function: int sslid_session_data_processor::"
                            "write_session_data( const std::string& session_id,"
                            "const boost::asio::ip::tcp::endpoint& endpoint,  time_t now_time ): "
                            "return_value=-1", __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        return -1;
    }

    /*-------- DEBUG LOG --------*/
    if ( LOG_LV_DEBUG == getloglevel() )
    {
        putLogDebug( 30000, "out_function: int sslid_session_data_processor::"
                            "write_session_data( const std::string& session_id,"
                            "const boost::asio::ip::tcp::endpoint& endpoint,  time_t now_time ): "
                            "return_value=0", __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    return 0;
}

//! read session data from replication area
//! @param[in] sslid_replication_data pointer
//! @return 0 : success
//! @return -1:exception
int sslid_session_data_processor::read_session_data_from_replication_area(
                                                    sslid_replication_data* replication_area  )
{
    /*-------- DEBUG LOG --------*/
    if ( LOG_LV_DEBUG == getloglevel() )
    {
        boost::format formatter( "in_function: int sslid_session_data_processor::"
                                            "read_session_data_from_replication_area( "
                                            "sslid_replication_data* replication_area ): "
                                            "replication_area=&(%d) " );
        formatter % reinterpret_cast<int>(replication_area);
        putLogDebug( 30000, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    // null check
    if ( replication_area == NULL )
    {
        putLogError( 37000, "Replication area is NULL.", __FILE__, __LINE__ );
        /*-------- DEBUG LOG --------*/
        if ( LOG_LV_DEBUG == getloglevel() )
        {
            putLogDebug( 30000, "out_function: int sslid_session_data_processor::"
                                "read_session_data_from_replication_area( "
                                "sslid_replication_data* replication_area ): "
                                "return_value=1", __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/
        return -1;
    }

    int ret = 0;
    try
    {
        // restore replication data to map
        for ( int i = 0; i < maxlist; ++i )
        {
            if ( replication_area[i].valid == 1 )
            {
                // valid session data
                boost::asio::ip::address address;
                char sessionid_temp[SSLID_LENGTH+1] = {0};
                memcpy( sessionid_temp, replication_area[i].realserver_ip, SSLID_LENGTH );
                address.from_string(replication_area[i].realserver_ip);
                boost::asio::ip::tcp::endpoint endpoint( address, replication_area[i].realserver_port );
                session_endpoint_map[sessionid_temp] = endpoint;
                session_lasttime_map[sessionid_temp] = replication_area[i].last_time;
                lasttime_session_map.insert( std::make_pair( replication_area[i].last_time, sessionid_temp ) );
            }
        }
    }
    catch( const std::exception& e)
    {
        std::cerr << "read_session_data_from_replication_area exception: error=%s" << e.what() << "." << std::endl;
        boost::format formatter( "function: int sslid_session_data_processor::"
                                            "read_session_data_from_replication_area() exception: result=-1, error=%s." );
        formatter % e.what();
        putLogError( 37000, formatter.str(), __FILE__, __LINE__ );

        ret = -1;
    }

    /*-------- DEBUG LOG --------*/
    if ( LOG_LV_DEBUG == getloglevel() )
    {
        boost::format formatter( "out_function: int sslid_session_data_processor::"
                                            "read_session_data_from_replication_area( "
                                            "sslid_replication_data* replication_area ): "
                                            "return_value=%d." );
        formatter % ret;
        putLogDebug( 30000, formatter.str(), __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    return ret;
}

//! clear expired session data
//! @return 0 : success
int sslid_session_data_processor::clear_expired_session_data()
{
    /*-------- DEBUG LOG --------*/
    if ( LOG_LV_DEBUG == getloglevel() )
    {
        putLogDebug( 30000, "in_function: int sslid_session_data_processor::"
                            "clear_expired_session_data( ) ", __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    // map must be already locked
    int ret = 0;

    bool bdelete = false;

    if ( session_endpoint_map.size() == 0 )
    {
         /*-------- DEBUG LOG --------*/
        if ( LOG_LV_DEBUG == getloglevel() )
        {
            putLogDebug( 30000, "out_function: int sslid_session_data_processor::"
                                "clear_expired_session_data( ): return_value=0.",  __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/

        return 0;
    }

    // none record time expired, delete the oldest session
    // get current time
    time_t now;
    time( &now );
    // delete all the expired time from the session_lasttime_map
    sslid_replication_temp_data temp_data;
    std::multimap<time_t, std::string>::iterator itmulti;
    for ( itmulti = lasttime_session_map.begin(); itmulti != lasttime_session_map.end(); )
    {
        // record the oldest time
        if ( now - itmulti->first > timeout )
        {
            // time out
            bdelete = true;
            temp_data.op_code = 'D';
            temp_data.session_id = itmulti->second;

            session_endpoint_map.erase( itmulti->second );
            session_lasttime_map.erase( itmulti->second );
            lasttime_session_map.erase( itmulti++ );

            // put into replication temp list
            replication_data_processor->put_into_temp_list( temp_data );
            /*-------- DEBUG LOG --------*/
            if ( LOG_LV_DEBUG == getloglevel() )
            {
                putLogDebug( 30000, "function: int sslid_session_data_processor::"
                                    "clear_expired_session_data():put_into_temp_list() "
                                    "--delete expired time item-- END.", __FILE__, __LINE__ );
            }
            /*------DEBUG LOG END------*/
        }
        else
        {
            // time in
            break;
        }
    }

    if ( !bdelete )
    {
        std::string session_id = lasttime_session_map.begin()->second;
        lasttime_session_map.erase( lasttime_session_map.begin() );
        session_endpoint_map.erase( session_id );
        session_lasttime_map.erase( session_id );

        // put into replication temp list
        temp_data.op_code = 'D';
        temp_data.session_id = session_id;
        replication_data_processor->put_into_temp_list( temp_data );
        /*-------- DEBUG LOG --------*/
        if ( LOG_LV_DEBUG == getloglevel() )
        {
            putLogDebug( 30000, "function: int sslid_session_data_processor::"
                                "clear_expired_session_data():put_into_temp_list() "
                                "--delete oldest time item-- END.", __FILE__, __LINE__ );
        }
        /*------DEBUG LOG END------*/
    }

    /*-------- DEBUG LOG --------*/
    if ( LOG_LV_DEBUG == getloglevel() )
    {
        boost::format formatter( "out_function: int sslid_session_data_processor::"
                                              "clear_expired_session_data( ): return_value=%d." );
        formatter % ret;
        putLogDebug( 30000, formatter.str(),  __FILE__, __LINE__ );
    }
    /*------DEBUG LOG END------*/

    return ret;
}

}
