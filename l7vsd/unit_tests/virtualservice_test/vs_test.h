#ifndef VS_TEST
#define VS_TEST

#include <boost/serialization/list.hpp>
#include "./parameter_stub/parameter.h"
#include "virtualservice.h"
#include "protocol_module_control.h"
#include "schedule_module_control.h"

namespace l7vs{

class    vs_test : public virtualservice_base {
protected:
    boost::xtime    diffelencial_time;
    void    handle_replication_interrupt( const boost::system::error_code& err ){
    }
    void    read_replicationdata(){
    }

public:
    vs_test(    const l7vsd& in_vsd,
                const replication& in_rep,
                const virtualservice_element& in_el ) : virtualservice_base( in_vsd, in_rep, in_el ){
    }
    ~vs_test(){};

    void    initialize( error_code& err ){
        protomod = protocol_module_control::getInstance().load_module( "PMtest1" );
        schedmod = schedule_module_control::getInstance().load_module( "SMtest1" );
    }
    void    finalize( error_code& err ){
    }

    bool    operator==( const virtualservice_base& vs ){
        return true;
    }
    bool    operator!=( const virtualservice_base& vs ){
        return true;
    }

    void    set_virtualservice( const virtualservice_element& el, error_code& err ){
    }
    void    edit_virtualservice( const virtualservice_element& el, error_code& err ){
    }

    void    add_realserver( const virtualservice_element& el, error_code& err ){
    }
    void    edit_realserver( const virtualservice_element& el, error_code& err ){
    }
    void    del_realserver( const virtualservice_element& el, error_code& err ){
    }

    void    run(){
    }
    void    stop(){
    }

    void    connection_active( const boost::asio::ip::tcp::endpoint& ep ){
    }
    void    connection_inactive( const boost::asio::ip::tcp::endpoint& ep ){
    }
    void    release_session( const tcp_session* session_ptr ){
    }

    //imprement for unit_test
    const l7vsd&        get_vsd(){ return vsd; }
    const replication&    get_rep(){ return rep; }

    void    call_handle_protomod_replication( const boost::system::error_code& err ){
        handle_protomod_replication( err );
    }
    void    call_handle_schedmod_replication( const boost::system::error_code& err ){
        handle_schedmod_replication( err );
    }

    unsigned long long    get_refcount(){
        return rs_list_ref_count.get();
//         return rs_list_ref_count;
    }
    void                set_refcount( unsigned long long in ){
        rs_list_ref_count = in;
    }

    unsigned long long    get_curr_up_recv_size(){
        return current_up_recvsize.get();
//         return current_up_recvsize;
    }
    unsigned long long    get_curr_down_recv_size(){
        return current_down_recvsize.get();
//         return current_down_recvsize;
    }
    void    call_handle_throughput_update( const boost::system::error_code& err ){
        boost::xtime_get( &diffelencial_time, boost::TIME_UTC );
        handle_throughput_update( err );

        if( 0 < diffelencial_time.sec ){
            std::cout << diffelencial_time.sec << "[sec]" << std::endl;
        }else{
            std::cout << diffelencial_time.nsec << "[nsec]" << std::endl;
        }
    }
    void    test_replication(){
/*        //serialize出力(header&data)
        virtualservice_base::replication_data    a1;
        //set data
        a1.udpflag        = true;
        a1.tcp_endpoint    = boost::asio::ip::tcp::endpoint( boost::asio::ip::address::from_string( "192.168.10.10" ), 8080 );
        a1.udp_endpoint    = boost::asio::ip::udp::endpoint( boost::asio::ip::address::from_string( "192.168.10.20" ), 8080 );
        a1.sorry_maxconnection    = LLONG_MAX;
        a1.sorry_endpoint        = boost::asio::ip::tcp::endpoint( boost::asio::ip::address::from_string( "192.168.10.1" ), 80 );
        a1.sorry_flag            = true;
        a1.qos_up                = 0ULL;
        a1.qos_down                = 30ULL;

        std::stringstream    ss;
        boost::archive::text_oarchive    oa( ss );
        oa << (const l7vs::virtualservice_base::replication_data&)a1;

        std::cout << ss.str() << std::endl;
        std::cout << ss.str().size() << std::endl;

        //serialize入力
        boost::archive::text_iarchive    ia( ss );
        l7vs::virtualservice_base::replication_data    a2;
        ia >> a2;
        BOOST_CHECK( a2.udpflag == a1.udpflag );
        BOOST_CHECK( a2.tcp_endpoint == a1.tcp_endpoint );
        BOOST_CHECK( a2.udp_endpoint == a1.udp_endpoint );
        BOOST_CHECK( a2.sorry_maxconnection == a1.sorry_maxconnection );
        BOOST_CHECK( a2.sorry_endpoint == a1.sorry_endpoint );
        BOOST_CHECK( a2.sorry_flag == a1.sorry_flag );
        BOOST_CHECK( a2.qos_up == a1.qos_up );
        BOOST_CHECK( a2.qos_down == a1.qos_down );*/
    }
    parameter_data&    get_param_data(){
        return param_data;
    }
    void    call_load_parameter(){
        l7vs::error_code    err;
        load_parameter( err );
    }
};

}

#endif
