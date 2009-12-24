/*!
 *    @file    virtualservice_element.h
 *    @brief    use l7vscommand virtualservice_data prototype
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

#ifndef    VIRTUALSERVICE_COMMAND_ELEMENT
#define    VIRTUALSERVICE_COMMAND_ELEMENT

#include <vector>
#include <boost/foreach.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/map.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include "realserver_element.h"
#include "endpoint.h"

namespace l7vs{

// virtual service element includes.
class    virtualservice_element{
public:
    bool                    udpmode;
    boost::asio::ip::tcp::endpoint
                            tcp_accept_endpoint;
    boost::asio::ip::udp::endpoint
                            udp_recv_endpoint;
    std::vector<realserver_element>
                            realserver_vector;
    std::string
                            protocol_module_name;
    std::string
                            schedule_module_name;
    std::vector<std::string>
                            protocol_args;
    long long               sorry_maxconnection;
    boost::asio::ip::tcp::endpoint
                            sorry_endpoint;
    int                     sorry_flag;
    unsigned long long      qos_upstream;
    unsigned long long      qos_downstream;
    unsigned long long      throughput_upstream;
    unsigned long long      throughput_downstream;

    unsigned long long      qos_upstream_ten_multiple_val;
    unsigned long long      qos_downstream_ten_multiple_val;

    int                     access_log_flag;
    std::string             ssl_file_name;
    std::string             access_log_file_name;
    typedef std::pair< std::string, std::string >
                            access_log_rotate_arguments_pair_type;
    typedef std::map< std::string, std::string >
                            access_log_rotate_arguments_map_type;
    access_log_rotate_arguments_map_type
                            access_log_rotate_arguments;
    std::string             protocol_module_for_indication_options;
    std::string             access_log_rotate_key_info;
    std::string             access_log_rotate_verbose_info;
    int                     socket_option_tcp_defer_accept;
    int                     socket_option_tcp_nodelay;
    int                     socket_option_tcp_cork;
    int                     socket_option_tcp_quickack;
    std::string             socket_option_string;

    virtualservice_element() :  udpmode(false),
                                sorry_maxconnection(0LL),
                                sorry_flag(0),
                                qos_upstream(0ULL),
                                qos_downstream(0ULL),
                                throughput_upstream(0ULL),
                                throughput_downstream(0ULL),
                                qos_upstream_ten_multiple_val(0ULL),
                                qos_downstream_ten_multiple_val(0ULL),
                                access_log_flag(0),
                                socket_option_tcp_defer_accept(0),
                                socket_option_tcp_nodelay(0),
                                socket_option_tcp_cork(0),
                                socket_option_tcp_quickack(0) {}
                
    virtualservice_element( const virtualservice_element& in )
            :   udpmode( in.udpmode ),
                tcp_accept_endpoint( in.tcp_accept_endpoint ),
                udp_recv_endpoint( in.udp_recv_endpoint ),
                protocol_module_name( in.protocol_module_name ),
                schedule_module_name( in.schedule_module_name ),
                sorry_maxconnection( in.sorry_maxconnection ),
                sorry_endpoint( in.sorry_endpoint ),
                sorry_flag( in.sorry_flag ),
                qos_upstream( in.qos_upstream ),
                qos_downstream( in.qos_downstream ),
                throughput_upstream( in.throughput_upstream ),
                throughput_downstream( in.throughput_downstream ),
                qos_upstream_ten_multiple_val( in.qos_upstream_ten_multiple_val ),
                qos_downstream_ten_multiple_val( in.qos_downstream_ten_multiple_val ),
                access_log_flag( in.access_log_flag ),
                ssl_file_name( in.ssl_file_name ),
                access_log_file_name( in.access_log_file_name ),
                protocol_module_for_indication_options( in.protocol_module_for_indication_options ),
                access_log_rotate_key_info( in.access_log_rotate_key_info ),
                access_log_rotate_verbose_info( in.access_log_rotate_verbose_info ),
                socket_option_tcp_defer_accept( in.socket_option_tcp_defer_accept ),
                socket_option_tcp_nodelay( in.socket_option_tcp_nodelay ),
                socket_option_tcp_cork( in.socket_option_tcp_cork ),
                socket_option_tcp_quickack( in.socket_option_tcp_quickack ),
                socket_option_string( in.socket_option_string ){
        protocol_args.clear();
        BOOST_FOREACH( std::string str, in.protocol_args ){
            protocol_args.push_back( str );
        }
        realserver_vector.clear();
        BOOST_FOREACH( realserver_element elem, in.realserver_vector ){
            realserver_vector.push_back( elem );
        }
        access_log_rotate_arguments.clear();
        BOOST_FOREACH( access_log_rotate_arguments_pair_type pair, in.access_log_rotate_arguments ){
            access_log_rotate_arguments.insert( pair );
        }
    }
    
    virtualservice_element& operator=( const virtualservice_element& in ){
        udpmode = in.udpmode;
        tcp_accept_endpoint = in.tcp_accept_endpoint;
        udp_recv_endpoint = in.udp_recv_endpoint;
        protocol_module_name = in.protocol_module_name;
        schedule_module_name = in.schedule_module_name;
        sorry_maxconnection = in.sorry_maxconnection;
        sorry_endpoint = in.sorry_endpoint;
        sorry_flag = in.sorry_flag;
        qos_upstream = in.qos_upstream;
        qos_downstream = in.qos_downstream;
        throughput_upstream = in.throughput_upstream;
        throughput_downstream = in.throughput_downstream;
        qos_upstream_ten_multiple_val = in.qos_upstream_ten_multiple_val;
        qos_downstream_ten_multiple_val = in.qos_downstream_ten_multiple_val;
        access_log_flag = in.access_log_flag;
        ssl_file_name = in.ssl_file_name;
        access_log_file_name = in.access_log_file_name;
        protocol_module_for_indication_options = in.protocol_module_for_indication_options;
        access_log_rotate_key_info = in.access_log_rotate_key_info;
        access_log_rotate_verbose_info = in.access_log_rotate_verbose_info;
        socket_option_tcp_defer_accept = in.socket_option_tcp_defer_accept;
        socket_option_tcp_nodelay = in.socket_option_tcp_nodelay;
        socket_option_tcp_cork = in.socket_option_tcp_cork;
        socket_option_tcp_quickack = in.socket_option_tcp_quickack;
        socket_option_string = in.socket_option_string;
        protocol_args.clear();
        BOOST_FOREACH( std::string str, in.protocol_args ){
            protocol_args.push_back( str );
        }
        realserver_vector.clear();
        BOOST_FOREACH( realserver_element elem, in.realserver_vector ){
            realserver_vector.push_back( elem );
        }
        access_log_rotate_arguments.clear();
        BOOST_FOREACH( access_log_rotate_arguments_pair_type pair, in.access_log_rotate_arguments ){
            access_log_rotate_arguments.insert( pair );
        }
        return *this;
    }
    
    friend    bool operator==( const virtualservice_element& elem1, const virtualservice_element& elem2 ){
        if( elem1.udpmode == elem2.udpmode &&
            elem1.tcp_accept_endpoint == elem2.tcp_accept_endpoint &&
            elem1.udp_recv_endpoint == elem2.udp_recv_endpoint &&
            elem1.protocol_module_name == elem2.protocol_module_name &&
            elem1.sorry_maxconnection == elem2.sorry_maxconnection &&
            elem1.sorry_flag == elem2.sorry_flag &&
            elem1.qos_upstream == elem2.qos_upstream &&
            elem1.qos_downstream == elem2.qos_downstream &&
            elem1.throughput_upstream == elem2.throughput_upstream &&
            elem1.throughput_downstream == elem2.throughput_downstream &&
            elem1.qos_upstream_ten_multiple_val == elem2.qos_upstream_ten_multiple_val &&
            elem1.qos_downstream_ten_multiple_val == elem2.qos_downstream_ten_multiple_val &&
            elem1.access_log_flag == elem2.access_log_flag &&
            elem1.ssl_file_name == elem2.ssl_file_name &&
            elem1.access_log_file_name == elem2.access_log_file_name &&
            elem1.protocol_module_for_indication_options == elem2.protocol_module_for_indication_options &&
            elem1.access_log_rotate_key_info == elem2.access_log_rotate_key_info &&
            elem1.access_log_rotate_verbose_info == elem2.access_log_rotate_verbose_info &&
            elem1.socket_option_tcp_defer_accept == elem2.socket_option_tcp_defer_accept &&
            elem1.socket_option_tcp_nodelay == elem2.socket_option_tcp_nodelay &&
            elem1.socket_option_tcp_cork == elem2.socket_option_tcp_cork &&
            elem1.socket_option_tcp_quickack == elem2.socket_option_tcp_quickack &&
            elem1.socket_option_string == elem2.socket_option_string ) {

            if( elem1.realserver_vector.size() != elem2.realserver_vector.size() ) return false;
            for( unsigned int i = 0; i < elem1.realserver_vector.size(); ++i ){
                if( elem1.realserver_vector[i] != elem2.realserver_vector[i] ) return false;
            }
            if( elem1.protocol_args.size() != elem2.protocol_args.size() ) return false;
            for( unsigned int i = 0; i < elem1.protocol_args.size(); ++i ){
                if( elem1.protocol_args[i] != elem2.protocol_args[i] ) return false;
            }
            if( elem1.access_log_rotate_arguments.size() != elem2.access_log_rotate_arguments.size() ) return false;
            BOOST_FOREACH( access_log_rotate_arguments_pair_type pair, elem1.access_log_rotate_arguments ){
                access_log_rotate_arguments_map_type::const_iterator it = elem2.access_log_rotate_arguments.find( pair.first );
                if( elem2.access_log_rotate_arguments.end() == it ) return false;
                if( it->second != pair.second ) return false;
            }
            return true;
        }
        return false;
    }
    
    friend bool operator!=( const virtualservice_element& elem1, const virtualservice_element& elem2 ){
         if( elem1.udpmode == elem2.udpmode &&
            elem1.tcp_accept_endpoint == elem2.tcp_accept_endpoint &&
            elem1.udp_recv_endpoint == elem2.udp_recv_endpoint &&
            elem1.protocol_module_name == elem2.protocol_module_name &&
            elem1.sorry_maxconnection == elem2.sorry_maxconnection &&
            elem1.sorry_flag == elem2.sorry_flag &&
            elem1.qos_upstream == elem2.qos_upstream &&
            elem1.qos_downstream == elem2.qos_downstream &&
            elem1.throughput_upstream == elem2.throughput_upstream &&
            elem1.throughput_downstream == elem2.throughput_downstream &&
            elem1.qos_upstream_ten_multiple_val == elem2.qos_upstream_ten_multiple_val &&
            elem1.qos_downstream_ten_multiple_val == elem2.qos_downstream_ten_multiple_val &&
            elem1.access_log_flag == elem2.access_log_flag &&
            elem1.ssl_file_name == elem2.ssl_file_name &&
            elem1.access_log_file_name == elem2.access_log_file_name &&
            elem1.protocol_module_for_indication_options == elem2.protocol_module_for_indication_options &&
            elem1.access_log_rotate_key_info == elem2.access_log_rotate_key_info &&
            elem1.access_log_rotate_verbose_info == elem2.access_log_rotate_verbose_info &&
            elem1.socket_option_tcp_defer_accept == elem2.socket_option_tcp_defer_accept &&
            elem1.socket_option_tcp_nodelay == elem2.socket_option_tcp_nodelay &&
            elem1.socket_option_tcp_cork == elem2.socket_option_tcp_cork &&
            elem1.socket_option_tcp_quickack == elem2.socket_option_tcp_quickack &&
            elem1.socket_option_string == elem2.socket_option_string ) {

            if( elem1.realserver_vector.size() != elem2.realserver_vector.size() ) return true;
            for( unsigned int i = 0; i < elem1.realserver_vector.size(); ++i ){
                if( elem1.realserver_vector[i] != elem2.realserver_vector[i] ) return true;    
            }
            if( elem1.protocol_args.size() != elem2.protocol_args.size() ) return true;
            for( unsigned int i = 0; i < elem1.protocol_args.size(); ++i ){
                if( elem1.protocol_args[i] != elem2.protocol_args[i] ) return true;
            }
            if( elem1.access_log_rotate_arguments.size() != elem2.access_log_rotate_arguments.size() ) return true;
            BOOST_FOREACH( access_log_rotate_arguments_pair_type pair, elem1.access_log_rotate_arguments ){
                access_log_rotate_arguments_map_type::const_iterator it = elem2.access_log_rotate_arguments.find( pair.first );
                if( elem2.access_log_rotate_arguments.end() == it ) return true;
                if( it->second != pair.second ) return true;
            }
            return false;
        }
        return true;
    }

    friend bool operator<( const virtualservice_element& elem1, const virtualservice_element& elem2 ){
        if( !elem1.udpmode && !elem2.udpmode ){
            return elem1.tcp_accept_endpoint < elem2.tcp_accept_endpoint;
        }
        else if( elem1.udpmode && elem2.udpmode ){
            return elem1.udp_recv_endpoint < elem2.udp_recv_endpoint;
        }
        return false;
    }

    template <typename Elem, typename Traits>
    friend std::basic_ostream<Elem, Traits>& operator<<(
        std::basic_ostream<Elem, Traits>& os,
        const virtualservice_element& elem ){

        os << "virtualservice_element={";
        os << boost::format(    "udpmode=%s: "
                                "tcp_accept_endpoint=%s: "
                                "udp_recv_endpoint=%s: " )
                                % elem.udpmode
                                % elem.tcp_accept_endpoint
                                % elem.udp_recv_endpoint;

        {
            unsigned int i = 0;
            BOOST_FOREACH( realserver_element rs_elem, elem.realserver_vector ){
                os << boost::format( "realserver_vector[%d]=" ) % i;
                os << rs_elem;
                os << ": ";
                ++i;
            }
        }

        os << boost::format(    "protocol_module_name=%s: "
                                "schedule_module_name=%s: " )
                                % elem.protocol_module_name
                                % elem.schedule_module_name;

        std::stringstream access_log_rotate_arguments_stream;
        {
            unsigned int i = 0;
            BOOST_FOREACH( access_log_rotate_arguments_pair_type pair, elem.access_log_rotate_arguments ){
                access_log_rotate_arguments_stream << boost::format( "access_log_rotate_arguments[%d]=" ) % i;
                access_log_rotate_arguments_stream << boost::format( "{key=%s, value=%s}" ) % pair.first % pair.second;
                access_log_rotate_arguments_stream << ": ";
                ++i;
            }
        }

        std::string    args = boost::algorithm::join( elem.protocol_args, " " );
        os << boost::format(    "protocol_args=%s: "
                                "sorry_maxconnection=%d: "
                                "sorry_endpoint=%s: "
                                "sorry_flag=%d: "
                                "qos_upstream=%d: "
                                "qos_downstream=%d: "
                                "throughput_upstream=%d: " 
                                "throughput_downstream=%d: "
                                "qos_upstream_ten_multiple_val=%d: "
                                "qos_downstream_ten_multiple_val=%d: "
                                "access_log_flag=%d: "
                                "ssl_file_name=%s: "
                                "access_log_file_name=%s: "
                                "access_log_rotate_arguments=%s: "
                                "protocol_module_for_indication_options=%s: "
                                "access_log_rotate_key_info=%s: "
                                "access_log_rotate_verbose_info=%s: "
                                "socket_option_tcp_defer_accept=%d: "
                                "socket_option_tcp_nodelay=%d: "
                                "socket_option_tcp_cork=%d: "
                                "socket_option_tcp_quickack=%d: "
                                "socket_option_string=%s}" )
                                % args
                                % elem.sorry_maxconnection
                                % elem.sorry_endpoint
                                % elem.sorry_flag
                                % elem.qos_upstream
                                % elem.qos_downstream
                                % elem.throughput_upstream
                                % elem.throughput_downstream
                                % elem.qos_upstream_ten_multiple_val
                                % elem.qos_downstream_ten_multiple_val
                                % elem.access_log_flag
                                % elem.ssl_file_name
                                % elem.access_log_file_name
                                % access_log_rotate_arguments_stream.str()
                                % elem.protocol_module_for_indication_options
                                % elem.access_log_rotate_key_info
                                % elem.access_log_rotate_verbose_info
                                % elem.socket_option_tcp_defer_accept
                                % elem.socket_option_tcp_nodelay
                                % elem.socket_option_tcp_cork
                                % elem.socket_option_tcp_quickack
                                % elem.socket_option_string;

        return os;
    }

private:
    friend class    boost::serialization::access;        //! friend boost serializable class
    //! serializable
    //! @brief using boost serialiable. class serializable function.
    //! @param[in]    archive
    //! @param[in]    version
    template <class Archive > void serialize( Archive& ar, const unsigned int version ){
        ar & udpmode;
        ar & tcp_accept_endpoint;
        ar & udp_recv_endpoint;
        ar & realserver_vector;
        ar & protocol_module_name;
        ar & schedule_module_name;
        ar & protocol_args;
        ar & sorry_maxconnection;
        ar & sorry_endpoint;
        ar & sorry_flag;
        ar & qos_upstream;
        ar & qos_downstream;
        ar & throughput_upstream;
        ar & throughput_downstream;
        ar & qos_upstream_ten_multiple_val;
        ar & qos_downstream_ten_multiple_val;
        ar & access_log_flag;
        ar & ssl_file_name;
        ar & access_log_file_name;
        ar & access_log_rotate_arguments;
        ar & protocol_module_for_indication_options;
        ar & access_log_rotate_key_info;
        ar & access_log_rotate_verbose_info;
        ar & socket_option_tcp_defer_accept;
        ar & socket_option_tcp_nodelay;
        ar & socket_option_tcp_cork;
        ar & socket_option_tcp_quickack;
        ar & socket_option_string;
    }
};

}            //namespace l7vsd
#endif        //L7VS_VIRTUALSERVICE_COMMAND_ELEMENT
