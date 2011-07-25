#ifndef    STUB_H
#define    STUB_H

#include <iostream>
#include <fstream>

#include "virtualservice.h"

#define    SESSION_POOL_NUM_PARAM    (512)

class    debugg_flug_struct{
protected:
    bool    session_pool_param_flg;
    bool    pmcontrol_error_flag;
    bool    smcontrol_error_flag;
    bool    pm_function_called;

    int        pm_rep_count;
    int        sm_rep_count;

    char*    rep_area;

    debugg_flug_struct() : session_pool_param_flg( false ),
                                pm_rep_count( 0 ),
                                sm_rep_count( 0 ) {
    }
    debugg_flug_struct( const debugg_flug_struct & in ){ rep_area = NULL; }
    debugg_flug_struct& operator=( const debugg_flug_struct & in ){ return *this; }
public:
    static debugg_flug_struct&    getInstance(){
        static    debugg_flug_struct    instance;
        return    instance;
    }
    bool&    param_exist_flag(){
        return session_pool_param_flg;
    }
    bool&    pmcontrol_err_flag(){
        return pmcontrol_error_flag;
    }
    bool&    smcontrol_err_flag(){
        return smcontrol_error_flag;
    }
    bool&    pm_function_called_flag(){
        return pm_function_called;
    }
    void    pm_rep_count_inc(){
        pm_rep_count = pm_rep_count+1;
    }
    int        get_pm_rep_count(){
        return pm_rep_count;
    }
    void    sm_rep_count_inc(){
        sm_rep_count = sm_rep_count+1;
    }
    int        get_sm_rep_count(){
        return sm_rep_count;
    }
    void    create_rep_area(){
        if( NULL == rep_area )
            rep_area = new char[(l7vs::virtualservice_base::MAX_REPLICATION_DATA_NUM)*480];
            memset( rep_area, 0, (sizeof(char)*480*l7vs::virtualservice_base::MAX_REPLICATION_DATA_NUM) );
    }
    void*    get_rep_area(){
        return reinterpret_cast<void*>( rep_area );
    }
    void    destroy_rep_area(){
        if( NULL != rep_area ){
            delete [] rep_area;
            rep_area = NULL;
        }
    }

void    get_nic_list( std::vector< std::string >& nic_vec ){
    using namespace boost;
    nic_vec.clear();
    std::string    buff;
    std::ifstream    ifs( "/proc/net/dev" );
    if( ifs )    ifs.seekg( 200 );
    else    return;
    while( std::getline( ifs, buff ) ){
        std::string nic = buff.substr( 0, 6 );
        algorithm::trim( nic );
        nic_vec.push_back( nic );
    }    
}

cpu_set_t    get_cpu_mask( std::string    nic_name ){
    using namespace std;
    using namespace boost;

    vector< string >         split_vec;
    map< size_t, string >    cpu_nic_map;
    string                    buff;
    ifstream                ifs( "/proc/interrupts" );
    unsigned int            target_interrupt;
    size_t                    target_cpuid = 0;
    cpu_set_t                mask;
    sched_getaffinity( 0, sizeof( cpu_set_t ), &mask );

    //read cpu lists.
    if( !ifs ) return mask;
    getline( ifs, buff );
    algorithm::split( split_vec, buff, algorithm::is_any_of( " " ) );
    for( vector< string >::iterator itr = split_vec.begin();
         itr != split_vec.end();
         ++itr ){
        algorithm::trim( *itr );
        if( itr->size() ) cpu_nic_map.insert( make_pair( cpu_nic_map.size(), "" ) );
    }
    // read interrupts.
    while( getline( ifs,  buff ) ){
        if( string::npos == buff.find( nic_name ) ) continue;
        //割り込みIDを取得
        algorithm::split( split_vec, buff, algorithm::is_any_of( ":" ));
        if( !split_vec.size() ) return mask;    //interrupt分割不可
        algorithm::trim( split_vec[0] );
        target_interrupt = lexical_cast<unsigned int>( split_vec[0] );
        for( size_t i = 0; i < cpu_nic_map.size(); ++i ){
            size_t    start_position = 4 + ( i * 11 );
            size_t    end_position = 11;
            map< size_t, string>::iterator itr = cpu_nic_map.find( i );
            if( itr == cpu_nic_map.end() ) return mask;
            itr->second = buff.substr( start_position, end_position );
            algorithm::trim( itr->second );
        }
        unsigned long long max_events = 0;
        for( map< size_t, string >::iterator itr = cpu_nic_map.begin();
             itr != cpu_nic_map.end();
             ++itr ){
            if( lexical_cast< unsigned long long >( itr->second ) > max_events ){
                target_cpuid = itr->first;
                max_events = lexical_cast< unsigned long long >( itr->second );
            }         
        }
        break;
    }
    CPU_ZERO( &mask );
    CPU_SET( static_cast<int>( target_cpuid ), &mask );
    return mask;
}

cpu_set_t    get_cpu_mask( boost::asio::ip::address& address ){
    using namespace boost;
    cpu_set_t    mask;

    std::vector< std::string > nic_vector;
    get_nic_list( nic_vector );
    for( std::vector< std::string >::iterator itr = nic_vector.begin();
         itr != nic_vector.end();
         ++itr ){
        struct ifreq    ifr;
        memset( &ifr, 0, sizeof( struct ifreq ) );
        if( address.is_v4() )    ifr.ifr_addr.sa_family = AF_INET;
        else                    ifr.ifr_addr.sa_family = AF_INET6;
        strncpy( ifr.ifr_name, itr->c_str(), IFNAMSIZ -1 );
        int fd = socket( AF_INET, SOCK_STREAM, 0 );
        if( ioctl( fd, SIOCGIFADDR, &ifr ) != 0 ){
            close( fd );
            continue;
        }
        struct sockaddr_in* ifaddr = reinterpret_cast<struct sockaddr_in*>( &( ifr.ifr_addr ) );
        asio::ip::address    add = asio::ip::address::from_string( inet_ntoa( ifaddr->sin_addr ) );
        if( add == address ){
            mask = get_cpu_mask( *itr );
            close( fd );
            return mask;
        }
        close( fd );
    }
    return mask;
}
};

#endif
