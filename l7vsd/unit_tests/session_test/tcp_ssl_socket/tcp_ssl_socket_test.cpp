
#define    TEST_CASE

#include <boost/thread/thread.hpp>
#include <boost/test/included/unit_test.hpp>

#include "tcp_ssl_socket.h"
#include "wrlock.h"
#include "logger.h"

#define DUMMI_SERVER_IP     "127.0.0.1"
#define DUMMI_SERVER_PORT     7000


#define CLIENT_CTX_LOAD_VERIFY_FILE         "ca.pem"
#define SERVER_CTX_CERTIFICATE_CHAIN_FILE   "server.pem"
#define SERVER_CTX_PRIVATE_KEY_FILE         "server.pem"
#define SERVER_CTX_TMP_DH_FILE              "dh512.pem"



using namespace boost::unit_test_framework;

// dummy client
class test_client{
    public:
        // 
        test_client(boost::asio::io_service& io_service, boost::asio::ssl::context& context) :
            my_socket(io_service,context){
        };

        ~test_client(){
        };
        void all_lock(){

            //! socket connect mutex
            connect_mutex.wrlock();
            //! socket handshake mutex
            handshake_mutex.wrlock();
            //! socket read mutex
            read_mutex.wrlock();
            //! socket write mutex
            write_mutex.wrlock();
            //! socket close mutex
            close_mutex.wrlock();

        }

        void connect_close_only_test_run(){
            // dummy client start

            // connect
            {
                l7vs::rw_scoped_lock scope_lock(connect_mutex);

                if(!connect_test()){
                    return;
                }
            }

            // close 
            {
                l7vs::rw_scoped_lock scope_lock(close_mutex);
                close_test();
            }

        };

        void handshake_test_run(){
            // dummy client start

            // connect
            {
                l7vs::rw_scoped_lock scope_lock(connect_mutex);

                if(!connect_test()){
                    return;
                }
            }

            // handshake
            {
                l7vs::rw_scoped_lock scope_lock(handshake_mutex);

                if(!handshake_test()){
                    return;
                }
            }

            // close 
            {
                l7vs::rw_scoped_lock scope_lock(close_mutex);
                close_test();
            }

        };

        void receive_send_test_run(){
            // dummy client start

            // connect
            {
                l7vs::rw_scoped_lock scope_lock(connect_mutex);

                if(!connect_test()){
                    return;
                }
            }

            // handshake
            {
                l7vs::rw_scoped_lock scope_lock(handshake_mutex);

                if(!handshake_test()){
                    return;
                }
            }

            // 1st
            receive_data_size = 0;
            send_data_size = 0;
            // receive
            {
                l7vs::rw_scoped_lock scope_lock(read_mutex);
                if(!receive_test()){
                    close_test();
                    return;
                }
            }
            read_mutex.wrlock();

            // send
            {
                l7vs::rw_scoped_lock scope_lock(write_mutex);
                if(!send_test()){
                    close_test();
                    return;
                }
            }
            write_mutex.wrlock();

            //2nd
            receive_data_size = 0;
            send_data_size = 0;
            // receive
            {
                l7vs::rw_scoped_lock scope_lock(read_mutex);
                if(!receive_test()){
                    close_test();
                    return;
                }
            }
            read_mutex.wrlock();

            // send
            {
                l7vs::rw_scoped_lock scope_lock(write_mutex);
                if(!send_test()){
                    close_test();
                    return;
                }
            }
            write_mutex.wrlock();

            //3rd
            receive_data_size = 0;
            send_data_size = 0;
            while(receive_data_size < MAX_BUFFER_SIZE){
                l7vs::rw_scoped_lock scope_lock(read_mutex);
                // receive
                {
                    if(!receive_test()){
                        close_test();
                        return;
                    }
                }
            }

            while(send_data_size < receive_data_size){
                l7vs::rw_scoped_lock scope_lock(write_mutex);
                // send
                {
                    if(!send_test()){
                        close_test();
                        return;
                    }
                }
            }

            // close 
            {
                l7vs::rw_scoped_lock scope_lock(close_mutex);
                close_test();
            }

        };




        bool connect_test(){
            sleep(1);
            boost::system::error_code ec;
            std::cout << "dummy client connect try" << std::endl;
            boost::asio::ip::tcp::endpoint connect_end(boost::asio::ip::address::from_string(DUMMI_SERVER_IP), DUMMI_SERVER_PORT);
            my_socket.lowest_layer().connect(connect_end,ec);
            if(ec){
                //receive error
                std::cout << "dummy client connect Error!" << std::endl;
                std::cout << ec << std::endl;
                return false;
            }
            std::cout << "dummy client connect OK" << std::endl;
            return true;
        };

        bool handshake_test(){
            boost::system::error_code ec;
            std::cout << "dummy client handshake try" << std::endl;
            my_socket.handshake(boost::asio::ssl::stream_base::client, ec);
            if(ec){
                //receive error
                std::cout << "dummy client handshake Error!" << std::endl;
                std::cout << ec << std::endl;
                return false;
            }
            std::cout << "dummy client handshake OK" << std::endl;
            return true;
        };

        bool send_test(){
            sleep(1);
            boost::system::error_code ec;
            std::cout << "dummy client write try" << std::endl;
            std::size_t write_size = my_socket.write_some(boost::asio::buffer(data_buff.data() + send_data_size,receive_data_size - send_data_size), ec);
            if(ec){
                //receive error
                std::cout << "dummy client send Error!" << std::endl;
                std::cout << ec << std::endl;
                return false;
            }
            send_data_size += write_size;
            std::cout << "dummy client send OK" << std::endl;
            return true;
        };
        bool receive_test(){
            sleep(1);
            boost::system::error_code ec;
            std::cout << "dummy client read try" << std::endl;
            std::size_t read_size = my_socket.read_some(boost::asio::buffer(data_buff.data() + receive_data_size,MAX_BUFFER_SIZE), ec);
            if(ec){
                //receive error
                std::cout << "dummy client receive Error!" << std::endl;
                std::cout << ec << std::endl;
                return false;
            }
            receive_data_size += read_size;
            std::cout << "dummy client receive OK" << std::endl;
            return true;
        };
        void close_test(){
            sleep(1);
            boost::system::error_code ec;
            std::cout << "dummy client close try" << std::endl;
            my_socket.lowest_layer().close(ec);
            if(ec){
                //close error
                std::cout << "dummy client close Error!" << std::endl;
                std::cout << ec << std::endl;
                return;
            }
            std::cout << "dummy client close OK" << std::endl;
        };

        boost::asio::ssl::stream<boost::asio::ip::tcp::socket> my_socket;
        boost::array<char,MAX_BUFFER_SIZE> data_buff;
        std::size_t receive_data_size;
        std::size_t send_data_size;

        //! socket connect mutex
        l7vs::wr_mutex connect_mutex;
        //! socket handshake mutex
        l7vs::wr_mutex handshake_mutex;
        //! socket read mutex
        l7vs::wr_mutex read_mutex;
        //! socket write mutex
        l7vs::wr_mutex write_mutex;
        //! socket close mutex
        l7vs::wr_mutex close_mutex;
};

// 
class test_ssl_socket_class : public l7vs::tcp_ssl_socket{
    public:

    test_ssl_socket_class(boost::asio::io_service& io,boost::asio::ssl::context& context,const l7vs::tcp_socket_option_info set_option) : l7vs::tcp_ssl_socket(io,context,set_option){
    };
    ~test_ssl_socket_class(){};

    boost::asio::ip::tcp::endpoint get_local_end(){
        return my_socket.lowest_layer().local_endpoint();
    };
    boost::asio::ip::tcp::endpoint get_remote_end(){
        return my_socket.lowest_layer().remote_endpoint();
    };
    boost::asio::io_service& get_io(){
        return my_socket.lowest_layer().get_io_service();
    };

    void test_close(boost::system::error_code& ec){
        my_socket.lowest_layer().close(ec);
    };

    bool& get_open_flag(){
        return open_flag;
    };

    boost::asio::ssl::stream<boost::asio::ip::tcp::socket>* get_socket_pointer(){
        return &my_socket;
    };

    l7vs::tcp_socket_option_info* get_opt_info(){
        return &opt_info;
    };

    std::string get_password() const{
        std::cout << "call get_password" << std::endl;
        return "test";
    };

};

class authority{
    public:
        authority(){
        };
        ~authority(){
        };
        std::string get_password() const{
            std::cout << "call get_password" << std::endl;
            return "test";
        };
};


//--test case--
// construcor test
void construcor_test(){

    
    BOOST_MESSAGE( "----- construcor test start -----" );
    
    boost::asio::io_service io;
    
    l7vs::tcp_socket_option_info set_option;
    //! TCP_NODELAY   (false:not set,true:set option)
    set_option.nodelay_opt = true;
    //! TCP_NODELAY option value  (false:off,true:on)
    set_option.nodelay_val = true;
    //! TCP_CORK      (false:not set,true:set option)
    set_option.cork_opt = true;
    //! TCP_CORK option value     (false:off,true:on)
    set_option.cork_val = true;
    //! TCP_QUICKACK  (false:not set,true:set option)
    set_option.quickack_opt = true;
    //! TCP_QUICKACK option value (false:off,true:on)
    set_option.quickack_val = true;

    // Server context
    boost::asio::ssl::context server_ctx(io,boost::asio::ssl::context::sslv23);

   test_ssl_socket_class test_obj(io,server_ctx,set_option);
    
    // unit_test [1] construcor test set io object
    std::cout << "[1] construcor test set io object" << std::endl;
    boost::asio::io_service& set_io = test_obj.get_io();
    BOOST_CHECK_EQUAL(&io , &set_io);
    
    // unit_test [2] construcor test init open_flag
    std::cout << "[2] construcor test init open_flag" << std::endl;
    BOOST_CHECK(!test_obj.get_open_flag());

    // unit_test [3] construcor test set socket option nodelay_opt
    std::cout << "[3] construcor test set socket option nodelay_opt" << std::endl;
    BOOST_CHECK_EQUAL(test_obj.get_opt_info()->nodelay_opt , set_option.nodelay_opt);
    
    // unit_test [4] construcor test set socket option nodelay_val
    std::cout << "[4] construcor test set socket option nodelay_val" << std::endl;
    BOOST_CHECK_EQUAL(test_obj.get_opt_info()->nodelay_val , set_option.nodelay_val);
    
    // unit_test [5] construcor test set socket option cork_opt
    std::cout << "[5] construcor test set socket option cork_opt" << std::endl;
    BOOST_CHECK_EQUAL(test_obj.get_opt_info()->cork_opt , set_option.cork_opt);
    
    // unit_test [6] construcor test set socket option cork_val
    std::cout << "[6] construcor test set socket option cork_val" << std::endl;
    BOOST_CHECK_EQUAL(test_obj.get_opt_info()->cork_val , set_option.cork_val);
    
    // unit_test [7] construcor test set socket option quickack_opt
    std::cout << "[7] construcor test set socket option quickack_opt" << std::endl;
    BOOST_CHECK_EQUAL(test_obj.get_opt_info()->quickack_opt , set_option.quickack_opt);
    
    // unit_test [8] construcor test set socket option quickack_val
    std::cout << "[8] construcor test set socket option quickack_val" << std::endl;
    BOOST_CHECK_EQUAL(test_obj.get_opt_info()->quickack_val , set_option.quickack_val);
    
    BOOST_MESSAGE( "----- construcor test end -----" );
}

// handshake test
void handshake_test(){

    BOOST_MESSAGE( "----- handshake test start -----" );

    boost::asio::io_service io;
    boost::system::error_code ec;
    authority test_auth;

    l7vs::tcp_socket_option_info set_option;
    //! TCP_NODELAY   (false:not set,true:set option)
    set_option.nodelay_opt = false;
    //! TCP_NODELAY option value  (false:off,true:on)
    set_option.nodelay_val = false;
    //! TCP_CORK      (false:not set,true:set option)
    set_option.cork_opt = false;
    //! TCP_CORK option value     (false:off,true:on)
    set_option.cork_val = false;
    //! TCP_QUICKACK  (false:not set,true:set option)
    set_option.quickack_opt = false;
    //! TCP_QUICKACK option value (false:off,true:on)
    set_option.quickack_val = false;

    // Client context
    boost::asio::ssl::context client_ctx(io,boost::asio::ssl::context::sslv23);
    client_ctx.set_verify_mode(boost::asio::ssl::context::verify_peer);
    client_ctx.load_verify_file(CLIENT_CTX_LOAD_VERIFY_FILE);

    // Server context
    boost::asio::ssl::context server_ctx(io,boost::asio::ssl::context::sslv23);
    server_ctx.set_options(
        boost::asio::ssl::context::default_workarounds
        | boost::asio::ssl::context::no_sslv2
        | boost::asio::ssl::context::single_dh_use);
    server_ctx.set_password_callback(boost::bind(&authority::get_password, &test_auth));
    server_ctx.use_certificate_chain_file(SERVER_CTX_CERTIFICATE_CHAIN_FILE);
    server_ctx.use_private_key_file(SERVER_CTX_PRIVATE_KEY_FILE, boost::asio::ssl::context::pem);
    server_ctx.use_tmp_dh_file(SERVER_CTX_TMP_DH_FILE);

    // test socket
    test_ssl_socket_class test_obj(io,server_ctx,set_option);

    // test acceptor
    boost::asio::ip::tcp::endpoint listen_end(boost::asio::ip::address::from_string(DUMMI_SERVER_IP), DUMMI_SERVER_PORT);
    boost::asio::ip::tcp::acceptor test_acceptor(io,listen_end,ec);

    // test client
    test_client dummy_cl(io,client_ctx);
    dummy_cl.all_lock();

    // client start
    boost::thread cl_thread(boost::bind(&test_client::handshake_test_run,&dummy_cl));

    // accept
    dummy_cl.connect_mutex.unlock();
    test_acceptor.accept(test_obj.get_socket().lowest_layer(),ec);
    if(ec){
        std::cout << "server side client connect ERROR" << std::endl;
        std::cout << ec << std::endl;
    }else{
        std::cout << "server side client connect OK" << std::endl;
    }
    BOOST_CHECK(!ec);

    // handshake
    dummy_cl.handshake_mutex.unlock();
    bool bres = test_obj.handshake(ec);
    if(ec){
        std::cout << "server side client handshake ERROR" << std::endl;
        std::cout << ec << std::endl;
    }else{
        std::cout << "server side handshake OK" << std::endl;
    }
    // unit_test [1] handshake test no error check
    std::cout << "[1] handshake test no error check" << std::endl;

    BOOST_CHECK(!ec);
    // unit_test [2] handshake test return value check
    std::cout << "[2] handshake test return value check" << std::endl;
    BOOST_CHECK(bres);

    // close
    dummy_cl.close_mutex.unlock();
    cl_thread.join();

    test_obj.get_socket().lowest_layer().close();



    // error test socket
    test_ssl_socket_class error_test_obj(io,server_ctx,set_option);

    // test client
    dummy_cl.all_lock();

    // client start
    boost::thread cl_thread2(boost::bind(&test_client::connect_close_only_test_run,&dummy_cl));

    // accept
    dummy_cl.connect_mutex.unlock();
    test_acceptor.accept(error_test_obj.get_socket().lowest_layer(),ec);
    if(ec){
        std::cout << "server side client connect ERROR" << std::endl;
        std::cout << ec << std::endl;
    }else{
        std::cout << "server side client connect OK" << std::endl;
    }
    BOOST_CHECK(!ec);

    // close
    dummy_cl.close_mutex.unlock();
    cl_thread2.join();

    // handshake
    bres = error_test_obj.handshake(ec);
    if(ec){
        std::cout << "server side client handshake ERROR" << std::endl;
        std::cout << ec << std::endl;

    }else{
        std::cout << "server side handshake OK" << std::endl;
    }
    // unit_test [3] handshake test error check
    std::cout << "[3] handshake test error check" << std::endl;
    BOOST_CHECK(ec);
    // unit_test [4] handshake test return value check
    std::cout << "[4] handshake test return value check" << std::endl;
    BOOST_CHECK(!bres);

    error_test_obj.get_socket().lowest_layer().close();

    // accepter close
    test_acceptor.close();

    BOOST_MESSAGE( "----- handshake_test test end -----" );
}
void get_socket_test(){
    BOOST_MESSAGE( "----- get_socket test start -----" );

    boost::asio::io_service io;
    
    l7vs::tcp_socket_option_info set_option;
    //! TCP_NODELAY   (false:not set,true:set option)
    set_option.nodelay_opt = true;
    //! TCP_NODELAY option value  (false:off,true:on)
    set_option.nodelay_val = true;
    //! TCP_CORK      (false:not set,true:set option)
    set_option.cork_opt = true;
    //! TCP_CORK option value     (false:off,true:on)
    set_option.cork_val = true;
    //! TCP_QUICKACK  (false:not set,true:set option)
    set_option.quickack_opt = true;
    //! TCP_QUICKACK option value (false:off,true:on)
    set_option.quickack_val = true;

    // Server context
    boost::asio::ssl::context server_ctx(io,boost::asio::ssl::context::sslv23);

   test_ssl_socket_class test_obj(io,server_ctx,set_option);
    
    // unit_test [1] get_socket socket check
    std::cout << "[1] get_socket socket check" << std::endl;
    
    boost::asio::ssl::stream<boost::asio::ip::tcp::socket>& chk_socket = test_obj.get_socket();
    boost::asio::ssl::stream<boost::asio::ip::tcp::socket>* pref_pointer = test_obj.get_socket_pointer();
    
    BOOST_CHECK_EQUAL(&chk_socket,pref_pointer);
    
    BOOST_MESSAGE( "----- get_socket test end -----" );    
}

void write_some_read_some_test(){
    BOOST_MESSAGE( "----- write_some read_some test start -----" );

    boost::asio::io_service io;
    boost::system::error_code ec;
    authority test_auth;

    l7vs::tcp_socket_option_info set_option;
    //! TCP_NODELAY   (false:not set,true:set option)
    set_option.nodelay_opt = false;
    //! TCP_NODELAY option value  (false:off,true:on)
    set_option.nodelay_val = false;
    //! TCP_CORK      (false:not set,true:set option)
    set_option.cork_opt = false;
    //! TCP_CORK option value     (false:off,true:on)
    set_option.cork_val = false;
    //! TCP_QUICKACK  (false:not set,true:set option)
    set_option.quickack_opt = false;
    //! TCP_QUICKACK option value (false:off,true:on)
    set_option.quickack_val = false;

    // Client context
    boost::asio::ssl::context client_ctx(io,boost::asio::ssl::context::sslv23);
    client_ctx.set_verify_mode(boost::asio::ssl::context::verify_peer);
    client_ctx.load_verify_file(CLIENT_CTX_LOAD_VERIFY_FILE);

    // Server context
    boost::asio::ssl::context server_ctx(io,boost::asio::ssl::context::sslv23);
    server_ctx.set_options(
        boost::asio::ssl::context::default_workarounds
        | boost::asio::ssl::context::no_sslv2
        | boost::asio::ssl::context::single_dh_use);
    server_ctx.set_password_callback(boost::bind(&authority::get_password, &test_auth));
    server_ctx.use_certificate_chain_file(SERVER_CTX_CERTIFICATE_CHAIN_FILE);
    server_ctx.use_private_key_file(SERVER_CTX_PRIVATE_KEY_FILE, boost::asio::ssl::context::pem);
    server_ctx.use_tmp_dh_file(SERVER_CTX_TMP_DH_FILE);

    // test socket
    test_ssl_socket_class test_obj(io,server_ctx,set_option);

    // test acceptor
    boost::asio::ip::tcp::endpoint listen_end(boost::asio::ip::address::from_string(DUMMI_SERVER_IP), DUMMI_SERVER_PORT);
    boost::asio::ip::tcp::acceptor test_acceptor(io,listen_end,ec);

    // test client
    test_client dummy_cl(io,client_ctx);
    dummy_cl.all_lock();

    // client start
    boost::thread cl_thread(boost::bind(&test_client::receive_send_test_run,&dummy_cl));

    // accept
    dummy_cl.connect_mutex.unlock();
    test_acceptor.accept(test_obj.get_socket().lowest_layer(),ec);
    if(ec){
        std::cout << "server side client connect ERROR" << std::endl;
        std::cout << ec << std::endl;
    }else{
        std::cout << "server side client connect OK" << std::endl;
    }
    BOOST_CHECK(!ec);

    // handshake
    dummy_cl.handshake_mutex.unlock();
    test_obj.handshake(ec);
    if(ec){
        std::cout << "server side client handshake ERROR" << std::endl;
        std::cout << ec << std::endl;
    }else{
        std::cout << "server side handshake OK" << std::endl;
    }
    BOOST_CHECK(!ec);

    test_obj.set_non_blocking_mode(ec);
    BOOST_CHECK(!ec);

    boost::array<char,MAX_BUFFER_SIZE> send_buff;
    boost::array<char,MAX_BUFFER_SIZE> recv_buff;
    size_t send_size;
    size_t res_size;
    size_t send_data_size;
    size_t receve_data_size;

    //size 0
    // ## write some read some test [1] size 0 
    dummy_cl.read_mutex.unlock();
    send_size = 0;
    while(true){
        res_size = test_obj.write_some(boost::asio::buffer(send_buff, send_size),ec);
        if(ec){
            if(ec == boost::asio::error::try_again){
                continue;
            }
        }
        break;
    }
    // unit_test [1] write_some & read_some test size 0 write error_code object
    std::cout << "[1] write_some & read_some test size 0 write error_code object" << std::endl;
    BOOST_CHECK(!ec);

    // unit_test [2] write_some & read_some test size 0 write size
    std::cout << "[2] write_some & read_some test size 0 write size" << std::endl;
    BOOST_CHECK_EQUAL(res_size, send_size);


    dummy_cl.write_mutex.unlock();
    res_size = test_obj.read_some(boost::asio::buffer(recv_buff, MAX_BUFFER_SIZE),ec);
    
    // unit_test [3] write_some & read_some test size 0 read error_code object
    std::cout << "[3] write_some & read_some test size 0 read error_code object" << std::endl;
    BOOST_CHECK_EQUAL(ec,boost::asio::error::try_again);
    
    // unit_test [4] write_some & read_some test size 0 read size
    std::cout << "[4] write_some & read_some test size 0 read size" << std::endl;
    BOOST_CHECK_EQUAL(res_size,0UL);

        
    // size 1    
    send_size = 1;
    send_buff[0] = 'A';
    recv_buff[0] = 'B';

    dummy_cl.read_mutex.unlock();

    while(true){
        res_size = test_obj.write_some(boost::asio::buffer(send_buff, send_size),ec);
        if(ec){
            if(ec == boost::asio::error::try_again){
                continue;
            }
        }
        break;
    }

    // unit_test [5] write_some & read_some test size 1 write error_code object
    std::cout << "[5] write_some & read_some test size 1 write error_code object" << std::endl;
    BOOST_CHECK(!ec);

    // unit_test [6] write_some & read_some test size 1 write send size
    std::cout << "[6] write_some & read_some test size 1 write send size" << std::endl;
    BOOST_CHECK_EQUAL(res_size, send_size);

    dummy_cl.write_mutex.unlock();

    while(true){
        res_size = test_obj.read_some(boost::asio::buffer(recv_buff, MAX_BUFFER_SIZE),ec);
        if(ec){
            if(ec == boost::asio::error::try_again){
                continue;
            }
        }
        break;
    }

    // unit_test [7] write_some & read_some test size 1 read error_code object
    std::cout << "[7] write_some & read_some test size 1 read error_code object" << std::endl;
    BOOST_CHECK(!ec);

    // unit_test [8] write_some & read_some test size 1 read size
    std::cout << "[8] write_some & read_some test size 1 read size" << std::endl;
    BOOST_CHECK_EQUAL(res_size, send_size);
    
    // unit_test [9] write_some & read_some test size 1 data check
    std::cout << "[9] write_some & read_some test size 1 data check" << std::endl;
    BOOST_CHECK_EQUAL(send_buff[0],recv_buff[0]);
    


    // size MAX_BUFFER_SIZE    
    send_size = MAX_BUFFER_SIZE;
    for(int i = 0;i < (int)send_size;i++)
        send_buff[i] = 'A';
    
    for(int i = 0;i < (int)send_size;i++)
        recv_buff[i] = 'B';
        
    send_data_size = 0;
    receve_data_size = 0;


    dummy_cl.read_mutex.unlock();
    while(true){
        if(send_data_size == MAX_BUFFER_SIZE){
            break;
        }
        res_size = test_obj.write_some(boost::asio::buffer(send_buff.data() + send_data_size, send_size - send_data_size),ec);
        if(!ec){
            send_data_size += res_size;
            std::cout << send_data_size;
            std::cout << " sending data" << std::endl;
        }else{
            if(ec != boost::asio::error::try_again){
                break;
            }
        }
    }

    // unit_test [10] write_some & read_some test size MAX_BUFFER_SIZE write error_code object
    std::cout << "[10] write_some & read_some test size MAX_BUFFER_SIZE write error_code object" << std::endl;
    BOOST_CHECK(!ec);
    
    // unit_test [11] write_some & read_some test size MAX_BUFFER_SIZE write size
    std::cout << "[11] write_some & read_some test size MAX_BUFFER_SIZE write size" << std::endl;        
    BOOST_CHECK_EQUAL(send_data_size, send_size);

    dummy_cl.write_mutex.unlock();
    
    while(true){
        if(receve_data_size == MAX_BUFFER_SIZE){
            break;
        }
        res_size = test_obj.read_some(boost::asio::buffer(recv_buff.data() + receve_data_size, MAX_BUFFER_SIZE - receve_data_size),ec);
        if(!ec){
            receve_data_size += res_size;
            std::cout << receve_data_size;
            std::cout << " receiving data" << std::endl;
        }else{
            if(ec != boost::asio::error::try_again){
                break;
            }
        }
    }

    // unit_test [12] write_some & read_some test size MAX_BUFFER_SIZE read error_code object
    std::cout << "[12] write_some & read_some test size MAX_BUFFER_SIZE read error_code object" << std::endl;
    BOOST_CHECK(!ec);
    
    // unit_test [13] write_some & read_some test size MAX_BUFFER_SIZE read size
    std::cout << "[13] write_some & read_some test size MAX_BUFFER_SIZE read size" << std::endl;        
    BOOST_CHECK_EQUAL(receve_data_size, send_size);
    
    // unit_test [14] write_some & read_some test size MAX_BUFFER_SIZE data check
    std::cout << "[14] write_some & read_some test size MAX_BUFFER_SIZE data check" << std::endl;
    for(int i = 0;i < (int)send_size;i++){
        BOOST_CHECK_EQUAL(send_buff[i],recv_buff[i]);
        if(send_buff[i] != recv_buff[i]) break;
    }
    
    // close
    dummy_cl.close_mutex.unlock();
    cl_thread.join();

    test_obj.test_close(ec);

    test_acceptor.close();

    BOOST_MESSAGE( "----- write_some & read_some test end -----" );
}

void close_test(){
    
    BOOST_MESSAGE( "----- close test start -----" );

    boost::asio::io_service io;
    boost::system::error_code ec;
    authority test_auth;

    l7vs::tcp_socket_option_info set_option;
    //! TCP_NODELAY   (false:not set,true:set option)
    set_option.nodelay_opt = false;
    //! TCP_NODELAY option value  (false:off,true:on)
    set_option.nodelay_val = false;
    //! TCP_CORK      (false:not set,true:set option)
    set_option.cork_opt = false;
    //! TCP_CORK option value     (false:off,true:on)
    set_option.cork_val = false;
    //! TCP_QUICKACK  (false:not set,true:set option)
    set_option.quickack_opt = false;
    //! TCP_QUICKACK option value (false:off,true:on)
    set_option.quickack_val = false;

    // Client context
    boost::asio::ssl::context client_ctx(io,boost::asio::ssl::context::sslv23);
    client_ctx.set_verify_mode(boost::asio::ssl::context::verify_peer);
    client_ctx.load_verify_file(CLIENT_CTX_LOAD_VERIFY_FILE);

    // Server context
    boost::asio::ssl::context server_ctx(io,boost::asio::ssl::context::sslv23);
    server_ctx.set_options(
        boost::asio::ssl::context::default_workarounds
        | boost::asio::ssl::context::no_sslv2
        | boost::asio::ssl::context::single_dh_use);
    server_ctx.set_password_callback(boost::bind(&authority::get_password, &test_auth));
    server_ctx.use_certificate_chain_file(SERVER_CTX_CERTIFICATE_CHAIN_FILE);
    server_ctx.use_private_key_file(SERVER_CTX_PRIVATE_KEY_FILE, boost::asio::ssl::context::pem);
    server_ctx.use_tmp_dh_file(SERVER_CTX_TMP_DH_FILE);

    // test socket
    test_ssl_socket_class test_obj(io,server_ctx,set_option);

    // test acceptor
    boost::asio::ip::tcp::endpoint listen_end(boost::asio::ip::address::from_string(DUMMI_SERVER_IP), DUMMI_SERVER_PORT);
    boost::asio::ip::tcp::acceptor test_acceptor(io,listen_end,ec);

    // test client
    test_client dummy_cl(io,client_ctx);
    dummy_cl.all_lock();

    // client start
    boost::thread cl_thread(boost::bind(&test_client::connect_close_only_test_run,&dummy_cl));

    // accept
    dummy_cl.connect_mutex.unlock();
    test_acceptor.accept(test_obj.get_socket().lowest_layer(),ec);
    if(ec){
        std::cout << "server side client connect ERROR" << std::endl;
        std::cout << ec << std::endl;
    }else{
        std::cout << "server side client connect OK" << std::endl;
    }
    BOOST_CHECK(!ec);

    test_obj.get_open_flag() = true;

    // unit_test [1] close test close success error_code object
    std::cout << "[1] close test close success error_code object" << std::endl;
    test_obj.close(ec);
    BOOST_CHECK(!ec);

    // unit_test [2] close test open_flag set false
    std::cout << "[2] close test open_flag set false" << std::endl;
    BOOST_CHECK(!test_obj.get_open_flag());

    // close
    dummy_cl.close_mutex.unlock();
    cl_thread.join();

    test_acceptor.close();

    BOOST_MESSAGE( "----- close test end -----" );    
}

void set_non_blocking_mode_test(){
    BOOST_MESSAGE( "----- set_non_blocking_mode test start -----" );
    
    boost::asio::io_service io;
    boost::system::error_code ec;
    authority test_auth;

    l7vs::tcp_socket_option_info set_option;
    //! TCP_NODELAY   (false:not set,true:set option)
    set_option.nodelay_opt = false;
    //! TCP_NODELAY option value  (false:off,true:on)
    set_option.nodelay_val = false;
    //! TCP_CORK      (false:not set,true:set option)
    set_option.cork_opt = false;
    //! TCP_CORK option value     (false:off,true:on)
    set_option.cork_val = false;
    //! TCP_QUICKACK  (false:not set,true:set option)
    set_option.quickack_opt = false;
    //! TCP_QUICKACK option value (false:off,true:on)
    set_option.quickack_val = false;

    // Client context
    boost::asio::ssl::context client_ctx(io,boost::asio::ssl::context::sslv23);
    client_ctx.set_verify_mode(boost::asio::ssl::context::verify_peer);
    client_ctx.load_verify_file(CLIENT_CTX_LOAD_VERIFY_FILE);

    // Server context
    boost::asio::ssl::context server_ctx(io,boost::asio::ssl::context::sslv23);
    server_ctx.set_options(
        boost::asio::ssl::context::default_workarounds
        | boost::asio::ssl::context::no_sslv2
        | boost::asio::ssl::context::single_dh_use);
    server_ctx.set_password_callback(boost::bind(&authority::get_password, &test_auth));
    server_ctx.use_certificate_chain_file(SERVER_CTX_CERTIFICATE_CHAIN_FILE);
    server_ctx.use_private_key_file(SERVER_CTX_PRIVATE_KEY_FILE, boost::asio::ssl::context::pem);
    server_ctx.use_tmp_dh_file(SERVER_CTX_TMP_DH_FILE);

    // test socket
    test_ssl_socket_class test_obj(io,server_ctx,set_option);

    // test acceptor
    boost::asio::ip::tcp::endpoint listen_end(boost::asio::ip::address::from_string(DUMMI_SERVER_IP), DUMMI_SERVER_PORT);
    boost::asio::ip::tcp::acceptor test_acceptor(io,listen_end,ec);

    // test client
    test_client dummy_cl(io,client_ctx);
    dummy_cl.all_lock();

    // client start
    boost::thread cl_thread(boost::bind(&test_client::connect_close_only_test_run,&dummy_cl));

    // accept
    dummy_cl.connect_mutex.unlock();
    test_acceptor.accept(test_obj.get_socket().lowest_layer(),ec);
    if(ec){
        std::cout << "server side client connect ERROR" << std::endl;
        std::cout << ec << std::endl;
    }else{
        std::cout << "server side client connect OK" << std::endl;
    }
    BOOST_CHECK(!ec);

    // unit_test [1] set_non_blocking_mode test set non blocking mode success error_code object
    std::cout << "[1] set_non_blocking_mode test set non blocking mode success error_code object" << std::endl;
    test_obj.set_non_blocking_mode(ec);
    BOOST_CHECK(!ec);
    
    // close
    dummy_cl.close_mutex.unlock();
    cl_thread.join();

    test_obj.get_socket().lowest_layer().close();

    
    // unit_test [2] set_non_blocking_mode test set non blocking mode faile error_code object
    std::cout << "[2] set_non_blocking_mode test set non blocking mode faile error_code object" << std::endl;
    test_obj.set_non_blocking_mode(ec);
    BOOST_CHECK(ec);

    // accepter close
    test_acceptor.close();

    
    BOOST_MESSAGE( "----- set_non_blocking_mode test end -----" );
}

void is_open_test(){
    
    BOOST_MESSAGE( "----- is_open test start -----" );
    
    
    boost::asio::io_service io;
    
    l7vs::tcp_socket_option_info set_option;
    //! TCP_NODELAY   (false:not set,true:set option)
    set_option.nodelay_opt = false;
    //! TCP_NODELAY option value  (false:off,true:on)
    set_option.nodelay_val = false;
    //! TCP_CORK      (false:not set,true:set option)
    set_option.cork_opt = false;
    //! TCP_CORK option value     (false:off,true:on)
    set_option.cork_val = false;
    //! TCP_QUICKACK  (false:not set,true:set option)
    set_option.quickack_opt = false;
    //! TCP_QUICKACK option value (false:off,true:on)
    set_option.quickack_val = false;

    // Server context
    boost::asio::ssl::context server_ctx(io,boost::asio::ssl::context::sslv23);

    test_ssl_socket_class test_obj(io,server_ctx,set_option);

    test_obj.get_open_flag() = true;

    // unit_test [1] is_open open check
    std::cout << "[1] is_open open check" << std::endl;
    BOOST_CHECK(test_obj.is_open());

    test_obj.get_open_flag() = false;
    // unit_test [2] is_open close check
    std::cout << "[2] is_open close check" << std::endl;
    BOOST_CHECK(!test_obj.is_open());
    
    BOOST_MESSAGE( "----- is_open test end -----" );    
}



test_suite*    init_unit_test_suite( int argc, char* argv[] ){

    test_suite* ts = BOOST_TEST_SUITE( "l7vs::tcp_ssl_socket class test" );

//    ts->add( BOOST_TEST_CASE( &construcor_test ) );
//NG    ts->add( BOOST_TEST_CASE( &accept_test ) );
//    ts->add( BOOST_TEST_CASE( &handshake_test ) );
//    ts->add( BOOST_TEST_CASE( &get_socket_test ) );
//    ts->add( BOOST_TEST_CASE( &set_non_blocking_mode_test ) );
    ts->add( BOOST_TEST_CASE( &write_some_read_some_test ) );
//    ts->add( BOOST_TEST_CASE( &close_test ) );
//NG    ts->add( BOOST_TEST_CASE( &close_lock_test ) );
//    ts->add( BOOST_TEST_CASE( &is_open_test ) );

    framework::master_test_suite().add( ts );

    return NULL;
}










/*






class connect_lock_test_class : public l7vs::tcp_socket{
    public:
        boost::thread::id befor_thread_id;
        boost::thread::id after_thread_id;
        
        connect_lock_test_class(boost::asio::io_service& io,const l7vs::tcp_socket_option_info set_option) : l7vs::tcp_socket(io,set_option){
        };
        
        ~connect_lock_test_class(){
        };
        
        void connect(const boost::asio::ip::tcp::endpoint connect_endpoint,boost::system::error_code& ec){
            befor_thread_id = boost::this_thread::get_id();
            l7vs::tcp_socket::connect(connect_endpoint,ec);
            after_thread_id = boost::this_thread::get_id();
            
        };
        void mutex_lock(){
            close_mutex.wrlock();
        };
        void mutex_unlock(){
            close_mutex.unlock();
        };
        
        boost::mutex test_thread_wait;
        
        void test_run(){
            boost::mutex::scoped_lock scope_lock(test_thread_wait);
            boost::asio::ip::tcp::endpoint connect_end(boost::asio::ip::address::from_string(DUMMI_SERVER_IP), DUMMI_SERVER_PORT);
            boost::system::error_code ec;
            connect(connect_end,ec);
        };
    
};



void connect_lock_test(){
    BOOST_MESSAGE( "----- connetc lock test start -----" );
        
    boost::asio::io_service io;
    
    l7vs::tcp_socket_option_info set_option;
    //! TCP_NODELAY   (false:not set,true:set option)
    set_option.nodelay_opt = false;
    //! TCP_NODELAY option value  (false:off,true:on)
    set_option.nodelay_val = false;
    //! TCP_CORK      (false:not set,true:set option)
    set_option.cork_opt = false;
    //! TCP_CORK option value     (false:off,true:on)
    set_option.cork_val = false;
    //! TCP_QUICKACK  (false:not set,true:set option)
    set_option.quickack_opt = false;
    //! TCP_QUICKACK option value (false:off,true:on)
    set_option.quickack_val = false;
    
    connect_lock_test_class test_obj(io,set_option);
    
    test_obj.test_thread_wait.lock();
    boost::thread::id proc_id = boost::this_thread::get_id();
    test_obj.befor_thread_id = proc_id;
    test_obj.after_thread_id = proc_id;
    test_obj.mutex_lock();
    
    boost::thread test_thread(boost::bind(&connect_lock_test_class::test_run,&test_obj));
    
    BOOST_CHECK(test_obj.befor_thread_id == proc_id);
    BOOST_CHECK(test_obj.after_thread_id == proc_id);
    
    boost::thread::id test_id = test_thread.get_id();
    
    BOOST_CHECK(test_id != proc_id);
    
    // test start
    test_obj.test_thread_wait.unlock();
    sleep(1);
    
    // unit_test [1] connect lock test thread block test (mutex lock)
    std::cout << "[1] connect lock test thread block test (mutex lock)" << std::endl;
    BOOST_CHECK(test_obj.befor_thread_id == test_id);
    BOOST_CHECK(test_obj.after_thread_id == proc_id);
    
    test_obj.mutex_unlock();
    sleep(1);
    
    // unit_test [2] connect lock test thread run test (mutex unlock)
    std::cout << "[2] connect lock test thread run test (mutex unlock)" << std::endl;
    BOOST_CHECK(test_obj.befor_thread_id == test_id);
    BOOST_CHECK(test_obj.after_thread_id == test_id);
    
    
    BOOST_MESSAGE( "----- connetc lock test end -----" );
    
}

class close_lock_test_class : public l7vs::tcp_socket{
    public:
        boost::thread::id befor_thread_id;
        boost::thread::id after_thread_id;
        
        close_lock_test_class(boost::asio::io_service& io,const l7vs::tcp_socket_option_info set_option) : l7vs::tcp_socket(io,set_option){
        };
        
        ~close_lock_test_class(){
        };
        
        void close(boost::system::error_code& ec){
            befor_thread_id = boost::this_thread::get_id();
            l7vs::tcp_socket::close(ec);
            after_thread_id = boost::this_thread::get_id();
            
        };
        void mutex_lock(){
            close_mutex.wrlock();
        };
        void mutex_unlock(){
            close_mutex.unlock();
        };
        
        boost::mutex test_thread_wait;
        
        void test_run(){
            boost::mutex::scoped_lock scope_lock(test_thread_wait);
            boost::system::error_code ec;
            close(ec);
        };
    
};


void close_lock_test(){
    BOOST_MESSAGE( "----- close lock test start -----" );
        
    boost::asio::io_service io;
    l7vs::tcp_socket_option_info set_option;
    //! TCP_NODELAY   (false:not set,true:set option)
    set_option.nodelay_opt = false;
    //! TCP_NODELAY option value  (false:off,true:on)
    set_option.nodelay_val = false;
    //! TCP_CORK      (false:not set,true:set option)
    set_option.cork_opt = false;
    //! TCP_CORK option value     (false:off,true:on)
    set_option.cork_val = false;
    //! TCP_QUICKACK  (false:not set,true:set option)
    set_option.quickack_opt = false;
    //! TCP_QUICKACK option value (false:off,true:on)
    set_option.quickack_val = false;
        
    close_lock_test_class test_obj(io,set_option);
    
    test_obj.test_thread_wait.lock();
    boost::thread::id proc_id = boost::this_thread::get_id();
    test_obj.befor_thread_id = proc_id;
    test_obj.after_thread_id = proc_id;
    test_obj.mutex_lock();
    
    boost::thread test_thread(boost::bind(&close_lock_test_class::test_run,&test_obj));
    
    BOOST_CHECK(test_obj.befor_thread_id == proc_id);
    BOOST_CHECK(test_obj.after_thread_id == proc_id);
    
    boost::thread::id test_id = test_thread.get_id();
    
    BOOST_CHECK(test_id != proc_id);
    
    // test start
    test_obj.test_thread_wait.unlock();
    sleep(1);
    
    // unit_test [1] close lock test thread block test (mutex lock)
    std::cout << "[1] close lock test thread block test (mutex lock)" << std::endl;
    BOOST_CHECK(test_obj.befor_thread_id == test_id);
    BOOST_CHECK(test_obj.after_thread_id == proc_id);
    
    test_obj.mutex_unlock();
    sleep(1);
    
    // unit_test [2] close lock test thread run test (mutex unlock)
    std::cout << "[2] close lock test  thread run test (mutex unlock)" << std::endl;
    BOOST_CHECK(test_obj.befor_thread_id == test_id);
    BOOST_CHECK(test_obj.after_thread_id == test_id);
    
    BOOST_MESSAGE( "----- close lock test end -----" );
    
}

*/
