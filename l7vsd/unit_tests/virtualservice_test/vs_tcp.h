#ifndef VS_TEST
#define VS_TEST

#include <boost/serialization/list.hpp>
#include "./parameter_stub/parameter.h"
#include "virtualservice.h"
#include "protocol_module_control.h"
#include "schedule_module_control.h"

namespace l7vs
{

class    vs_tcp : public virtualservice_tcp
{
public:
        vs_tcp(const l7vsd                    &in_vsd,
               const replication                &in_rep,
               const virtualservice_element    &in_element) : virtualservice_tcp(in_vsd, in_rep, in_element) {}

        //imprementation for UT
        const l7vsd        &get_vsd() {
                return vsd;
        }
        const replication    &get_rep() {
                return rep;
        }

        void    call_handle_replication_interrupt(const boost::system::error_code &in_err) {
                handle_replication_interrupt(in_err);
        }

        void    call_read_replicationdata() {
                read_replicationdata();
        }

//    void    call_handle_accept(    const session_thread_control_ptr,
//                                const boost::system::error_code& );

        session_queue_type    &get_pool_sessions() {
                return    pool_sessions;
        }
        session_map_type    &get_active_sessions() {
                return    active_sessions;
        }
        session_map_type    &get_waiting_sessions() {
                return    waiting_sessions;
        }
        unsigned long long    get_active_count() {
                return    active_count.get();
        }
        std::list<realserver>&    get_rs_list() {
                return rs_list;
        }
        void    release_session(const tcp_session *session_ptr) {
                std::cout << "release_session called." << std::endl;
                virtualservice_tcp::release_session(session_ptr);
        }
        unsigned long long    get_ref_count() {
                return rs_list_ref_count.get();
//         return rs_list_ref_count;
        }

        bool &get_defer_accept_opt() {
                return defer_accept_opt;
        };
        int &get_defer_accept_val() {
                return defer_accept_val;
        };
        l7vs::tcp_socket_option_info &get_set_sock_opt() {
                return set_sock_opt;
        };
        boost::asio::ip::tcp::acceptor &get_acceptor() {
                return acceptor_;
        };

        void set_socket_option() {
                l7vs::virtualservice_tcp::set_socket_option();
        };

//--------------------
//--------------------
// 2009/12/12 test
//--------------------
//--------------------


        std::string get_ssl_password() {
                return l7vs::virtualservice_tcp::get_ssl_password();
        };

        int conv_verify_option(std::string in) {
                return l7vs::virtualservice_tcp::conv_verify_option(in);
        };

        long int    conv_ssl_option(std::string in) {
                return l7vs::virtualservice_tcp::conv_ssl_option(in);
        };

        bool        get_ssl_parameter() {
                return l7vs::virtualservice_tcp::get_ssl_parameter();
        };

        bool        set_ssl_config() {
                return l7vs::virtualservice_tcp::set_ssl_config();
        };

        // SSL flag
        bool &get_ssl_virtualservice_mode_flag() {
                return(ssl_virtualservice_mode_flag);
        };

        void set_ssl_virtualservice_mode_flag(bool in) {
                ssl_virtualservice_mode_flag = in;
        };


        std::string &get_ssl_file_name() {
                return(ssl_file_name);
        };

        void set_ssl_file_name(std::string in) {
                ssl_file_name = in;
        };



        // SSL context
        boost::asio::ssl::context &get_sslcontext() {
                return(sslcontext);
        };
        /*
            void set_sslcontext(boost::asio::ssl::context &in)
            {
                sslcontext = in;
            };
        */


        // SSL context parameter
        std::string &get_ca_dir() {
                return(ca_dir);
        };

        void set_ca_dir(std::string in) {
                ca_dir = in;
        };



        std::string &get_ca_file() {
                return(ca_file);
        };

        void set_ca_file(std::string in) {
                ca_file = in;
        };




        std::string &get_cert_chain_dir() {
                return(cert_chain_dir);
        };

        void set_cert_chain_dir(std::string in) {
                cert_chain_dir = in;
        };




        std::string &get_cert_chain_file() {
                return(cert_chain_file);
        };

        void set_cert_chain_file(std::string in) {
                cert_chain_file = in;
        };



        std::string &get_private_key_dir() {
                return(private_key_dir);
        };

        void set_private_key_dir(std::string in) {
                private_key_dir = in;
        };





        std::string &get_private_key_file() {
                return(private_key_file);
        };

        void set_private_key_file(std::string in) {
                private_key_file = in;
        };




        boost::asio::ssl::context::file_format
        &get_private_key_filetype() {
                return(private_key_filetype);
        };


        void set_private_key_filetype(boost::asio::ssl::context::file_format in) {
                private_key_filetype = in;
        };




        std::string &get_private_key_passwd_dir() {
                return(private_key_passwd_dir);
        };

        void set_private_key_passwd_dir(std::string in) {
                private_key_passwd_dir = in;
        };




        std::string &get_private_key_passwd_file() {
                return(private_key_passwd_file);
        };

        void set_private_key_passwd_file(std::string in) {
                private_key_passwd_file = in;
        };





        int &get_verify_options() {
                return(verify_options);
        };

        void set_verify_options(int in) {
                verify_options = in;
        };




        int &get_verify_cert_depth() {
                return(verify_cert_depth);
        };

        void set_verify_cert_depth(int in) {
                verify_cert_depth = in;
        };




        long int &get_ssl_options() {
                return(ssl_options);
        };

        void set_ssl_options(long int in) {
                ssl_options = in;
        };




        bool &get_is_tmp_dh_use() {
                return(is_tmp_dh_use);
        };

        void set_is_tmp_dh_use(bool in) {
                is_tmp_dh_use = in;
        };




        std::string &get_tmp_dh_dir() {
                return(tmp_dh_dir);
        };

        void set_tmp_dh_dir(std::string in) {
                tmp_dh_dir = in;
        };




        std::string &get_tmp_dh_file() {
                return(tmp_dh_file);
        };

        void set_tmp_dh_file(std::string in) {
                tmp_dh_file = in;
        };




        std::string &get_cipher_list() {
                return(cipher_list);
        };

        void set_cipher_list(std::string in) {
                cipher_list = in;
        };




        // SSL session cache parameter
        bool &get_is_session_cache_use() {
                return(is_session_cache_use);
        };

        void set_is_session_cache_use(bool in) {
                is_session_cache_use = in;
        };




        long &get_session_cache_mode() {
                return(session_cache_mode);
        };

        void set_session_cache_mode(long in) {
                session_cache_mode = in;
        };




        long &get_session_cache_size() {
                return(session_cache_size);
        };

        void set_session_cache_size(long in) {
                session_cache_size = in;
        };





        long &get_session_cache_timeout() {
                return(session_cache_timeout);
        };

        void set_session_cache_timeout(long in) {
                session_cache_timeout = in;
        };




        // SSL handshake timer parameter
        int &get_handshake_timeout() {
                return(handshake_timeout);
        };

        void set_handshake_timeout(int in) {
                handshake_timeout = in;
        };




        std::string &get_access_log_file_name() {
                return(access_log_file_name);
        };

        void set_access_log_file_name(std::string in) {
                access_log_file_name = in;
        };




        accesslog_argument_map_type &get_access_log_rotate_arguments() {
                return(access_log_rotate_arguments);
        };

        void set_access_log_rotate_arguments(accesslog_argument_map_type in) {
                access_log_rotate_arguments = in;
        };




        bool &get_access_log_flag() {
                return(access_log_flag);
        };

        void set_access_log_flag(bool in) {
                access_log_flag = in;
        };

};

}

#endif
