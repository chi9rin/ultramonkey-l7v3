
#define	TEST_CASE

#include <boost/thread/thread.hpp>
#include <boost/test/included/unit_test.hpp>

#include "tcp_socket.h"
#include "logger.h"

#define DUMMI_SERVER_IP 	"127.0.0.1"
#define DUMMI_SERVER_PORT 	7000

using namespace boost::unit_test_framework;

// dummy mirror server
class test_mirror_server{
	
	public:
		bool bstop_flag;
		bool brun_flag;
		bool breq_acc_flag;
		bool breq_close_wait_flag;
		bool bconnect_flag;
		bool bdisconnect_flag;
		boost::asio::ip::tcp::endpoint accept_end;
		boost::asio::ip::tcp::endpoint connect_end;
		size_t receive_size;
		int req_recv_cnt;
		bool brecv_triger;
		size_t data_size;
		
		test_mirror_server() : 
				bstop_flag(false),
				brun_flag(false),
				breq_acc_flag(false),
				breq_close_wait_flag(false),
				bconnect_flag(false),
				bdisconnect_flag(false),
				accept_end(boost::asio::ip::address::from_string(DUMMI_SERVER_IP), DUMMI_SERVER_PORT),
				connect_end(boost::asio::ip::address::from_string(DUMMI_SERVER_IP), DUMMI_SERVER_PORT),
				receive_size(0),
				req_recv_cnt(0),
				brecv_triger(false),
				data_size(0){
		};
		
		~test_mirror_server(){
			bstop_flag = true;
			
			while(brun_flag){
				sleep(1);
			}
			
		};
		
		void run(){
			std::cout << "dummy mirror server run start!" << std::endl;
			
			brun_flag = true;
			bconnect_flag = false;
			bdisconnect_flag = false;
			boost::asio::io_service io;
			boost::asio::ip::tcp::acceptor acc(io,accept_end);
			boost::system::error_code ec;
			
			boost::array<char,MAX_BUFFER_SIZE> buf;
			receive_size = 0;
			size_t send_size = 0;
			
			
			while(!bstop_flag){
				if(!breq_acc_flag){
					continue;
				}
				
				boost::asio::ip::tcp::socket con(io);
				
				// accept
				acc.accept(con,ec);
				if(ec){
					std::cout << "dummy mirror server accept NG!" << std::endl;
					break;
				}else{
					connect_end = con.remote_endpoint();
					std::cout << "dummy mirror server accept OK! from " << connect_end << std::endl;
					
					breq_acc_flag = false;
					// client chk
					
					bconnect_flag = true;
					
					for(int i = 0; i < req_recv_cnt ;i++){
						while(!brecv_triger){
							if(bstop_flag)
								break;
						}
						brecv_triger = false;
						if(bstop_flag)
							break;
							
						// receive
						receive_size = 0;
						while(receive_size < data_size){
							size_t ret_size = con.read_some(boost::asio::buffer(buf.data() + receive_size,data_size - receive_size),ec);
							if(ec){
								if(ec == boost::asio::error::eof || ec == boost::asio::error::connection_reset){
									std::cout << "dummy mirror server detect client disconnect!" << std::endl;
									bdisconnect_flag = true;
									break;
								}else{
									std::cout << "dummy mirror server receive NG!" << std::endl;
									break;
								}
							}else if(ret_size > 0){
								receive_size += ret_size;
								std::cout << "dummy mirror server receive " << receive_size << "Byte" << std::endl;
							}
						}
						if(!ec){
							if(receive_size > 0){
								// send
								send_size = 0;
								while(send_size < receive_size){
									size_t ret_size = con.write_some(boost::asio::buffer(buf.data() + send_size,receive_size - send_size),ec);
									if(ec){
										std::cout << "dummy mirror server send NG!" << std::endl;
										break;
									}else if(ret_size > 0){
										send_size += ret_size;
										std::cout << "dummy mirror server send " << send_size << "Byte" << std::endl;
									}
								}
							}
						}
					}
					
					std::cout << "dummy mirror server connection close wait start" << std::endl;
					while(breq_close_wait_flag){
						sleep(1);
					}
					std::cout << "dummy mirror server connection close wait end" << std::endl;
					// close
					con.close(ec);
				}
			}
			
			acc.close(ec);
			
			brun_flag = false;
			
			std::cout << "dummy mirror server run end!" << std::endl;
		};
};

// 
class test_socket_class : public l7vs::tcp_socket{
	public:
		
//	test_socket_class(boost::asio::io_service& io) : l7vs::tcp_socket(io){
//	};
	test_socket_class(boost::asio::io_service& io,const tcp_socket_option_info set_option) : l7vs::tcp_socket(io,set_option){
	};
	~test_socket_class(){};
	
	boost::asio::ip::tcp::endpoint get_local_end(){
		return my_socket.local_endpoint();
	}
	boost::asio::ip::tcp::endpoint get_remote_end(){
		return my_socket.remote_endpoint();
	}
	boost::asio::io_service& get_io(){
		return my_socket.get_io_service();
	}
	
	void test_close(boost::system::error_code& ec){
		my_socket.close(ec);
	}
	
	bool& get_open_flag(){
		return open_flag;
	}
	
	boost::asio::ip::tcp::socket* get_socket_pointer(){
		return &my_socket;
	}
	
	l7vs::tcp_socket::tcp_socket_option_info* get_opt_info(){
		return &opt_info;
	}
		
};


//--test case--
// construcor test
void construcor_test(){

	
	BOOST_MESSAGE( "----- construcor test start -----" );
	
	boost::asio::io_service io;
	
	l7vs::tcp_socket::tcp_socket_option_info set_option;
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
			
	test_socket_class test_obj(io,set_option);
	
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

void connect_test(){
	
	BOOST_MESSAGE( "----- connect test start -----" );
	
	
	test_mirror_server test_server;
	
	// accept req
	test_server.breq_acc_flag = true;
	// close wait req
	test_server.breq_close_wait_flag = true;
		
	// test server start
	boost::thread server_thread(boost::bind(&test_mirror_server::run,&test_server));
	
	while( !test_server.brun_flag ){
		sleep(1);
	}
	
	std::cout << "ready dummy mirror server" << std::endl;
	
	// unit_test [1] connect test connection success error_code object
	std::cout << "[1] connect test connection success error_code object" << std::endl;
	boost::asio::ip::tcp::endpoint connect_end(boost::asio::ip::address::from_string(DUMMI_SERVER_IP), DUMMI_SERVER_PORT);
	boost::asio::io_service io;
	boost::system::error_code ec;
	
	l7vs::tcp_socket::tcp_socket_option_info set_option;
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
	
	test_socket_class test_obj(io,set_option);
	test_obj.connect(connect_end,ec);
	BOOST_CHECK(!ec);
	
	// unit_test [2] connect test connection success open_flag
	std::cout << "[2] connect test connection success open_flag" << std::endl;
	BOOST_CHECK(test_obj.get_open_flag());
	
	// TCP_NODELAY check!!
	// unit_test [3] connect test set socket option TCP_NODELAY
	std::cout << "[3] connect test set socket option TCP_NODELAY" << std::endl;
	boost::asio::ip::tcp::no_delay get_option;
	test_obj.get_socket_pointer()->get_option(get_option,ec);
	BOOST_CHECK(!ec);
	BOOST_CHECK(get_option == set_option.nodelay_val);
	
	// TCP_CORK check!!
	// unit_test [4] connect test set socket option TCP_CORK
	std::cout << "[4] connect test set socket option TCP_CORK" << std::endl;
	int val;
	size_t len = sizeof(val);
	boost::asio::detail::socket_ops::getsockopt(test_obj.get_socket_pointer()->native(),IPPROTO_TCP,TCP_CORK,&val,&len,ec);
	BOOST_CHECK(!ec);
	BOOST_CHECK((bool)val == set_option.cork_val);
	
	
	while(!test_server.bconnect_flag){
		sleep(1);
	}
	
	boost::asio::ip::tcp::endpoint chk_end;
	boost::asio::ip::tcp::endpoint ref_end;
	
	// unit_test [5] connect test connect local endpoint
	std::cout << "[5] connect test connect local endpoint" << std::endl;	
	chk_end = test_obj.get_local_end();
	ref_end = test_server.connect_end;
	BOOST_CHECK_EQUAL(chk_end , ref_end);
	
	
	// unit_test [6] connect test connect remote endpoint
	std::cout << "[6] connect test connect remote endpoint" << std::endl;
	chk_end = test_obj.get_remote_end();
	ref_end = connect_end;
	BOOST_CHECK_EQUAL(chk_end , ref_end);

	// unit_test [7] connect test connect recall check
	std::cout << "[7] connect test connect recall check" << std::endl;
	test_obj.connect(connect_end,ec);
	BOOST_CHECK(!ec);
	BOOST_CHECK(test_obj.get_open_flag());
	
	test_obj.test_close(ec);
	test_obj.get_open_flag() = false;
	
	test_server.breq_close_wait_flag = false;	
	test_server.bstop_flag = true;
	server_thread.join();
		
	// unit_test [8] connect test connection faile error_code object
	std::cout << "[8] connect test connection faile error_code object" << std::endl;
	test_obj.connect(connect_end,ec);
	BOOST_CHECK(ec);
		
	// unit_test [9] connect test connection faile open_flag
	std::cout << "[9] connect test connection faile open_flag" << std::endl;
	BOOST_CHECK(!test_obj.get_open_flag());
	
	BOOST_MESSAGE( "----- connect test end -----" );
	
}

void set_non_blocking_mode_test(){
	BOOST_MESSAGE( "----- set_non_blocking_mode test start -----" );
	
	test_mirror_server test_server;
	
	// accept req
	test_server.breq_acc_flag = true;
	// close wait req
	test_server.breq_close_wait_flag = true;
		
	// test server start
	boost::thread server_thread(boost::bind(&test_mirror_server::run,&test_server));
	
	while( !test_server.brun_flag ){
		sleep(1);
	}
	
	std::cout << "ready dummy mirror server" << std::endl;
	
	
	
	boost::asio::ip::tcp::endpoint connect_end(boost::asio::ip::address::from_string(DUMMI_SERVER_IP), DUMMI_SERVER_PORT);
	boost::asio::io_service io;
	boost::system::error_code ec;
	
	l7vs::tcp_socket::tcp_socket_option_info set_option;
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
	
	test_socket_class test_obj(io,set_option);
	test_obj.connect(connect_end,ec);
	BOOST_CHECK(!ec);
	
	while(!test_server.bconnect_flag){
		sleep(1);
	}

	// unit_test [1] set_non_blocking_mode test set non blocking mode success error_code object
	std::cout << "[1] set_non_blocking_mode test set non blocking mode success error_code object" << std::endl;
	test_obj.set_non_blocking_mode(ec);
	BOOST_CHECK(!ec);
	
	test_obj.test_close(ec);
	
	test_server.breq_close_wait_flag = false;	
	test_server.bstop_flag = true;
	server_thread.join();
	
	// unit_test [2] set_non_blocking_mode test set non blocking mode faile error_code object
	std::cout << "[2] set_non_blocking_mode test set non blocking mode faile error_code object" << std::endl;
	test_obj.set_non_blocking_mode(ec);
	BOOST_CHECK(ec);
	
	BOOST_MESSAGE( "----- set_non_blocking_mode test end -----" );
}


void write_some_read_some_test(){
	BOOST_MESSAGE( "----- write_some read_some test start -----" );
	
	test_mirror_server test_server;
	
	// accept req
	test_server.breq_acc_flag = true;
	// close wait req
	test_server.breq_close_wait_flag = true;
	// recv cont
	test_server.req_recv_cnt = 513;
		
	
	// test server start
	boost::thread server_thread(boost::bind(&test_mirror_server::run,&test_server));
	
	while( !test_server.brun_flag ){
		sleep(1);
	}
	
	std::cout << "ready dummy mirror server" << std::endl;
	
	// connect
	boost::asio::ip::tcp::endpoint connect_end(boost::asio::ip::address::from_string(DUMMI_SERVER_IP), DUMMI_SERVER_PORT);
	boost::asio::io_service io;
	boost::system::error_code ec;
	
	l7vs::tcp_socket::tcp_socket_option_info set_option;
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
	
	test_socket_class test_obj(io,set_option);
	test_obj.connect(connect_end,ec);
	BOOST_CHECK(!ec);
	
	while(!test_server.bconnect_flag){
		sleep(1);
	}
	
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
	send_size = 0;
	test_server.brecv_triger = true;
	test_server.data_size = send_size;
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
	
	sleep(1);
	
	res_size = test_obj.read_some(boost::asio::buffer(recv_buff, MAX_BUFFER_SIZE),ec);
	
	// unit_test [3] write_some & read_some test size 0 read error_code object
	std::cout << "[3] write_some & read_some test size 0 read error_code object" << std::endl;
	BOOST_CHECK_EQUAL(ec,boost::asio::error::try_again);
	
	// unit_test [4] write_some & read_some test size 0 read size
	std::cout << "[4] write_some & read_some test size 0 read size" << std::endl;
	BOOST_CHECK_EQUAL(res_size,0UL);
		
	// size 1	
	send_size = 1;
	test_server.data_size = send_size;
	send_buff[0] = 'A';
	recv_buff[0] = 'B';
	
	test_server.brecv_triger = true;
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
	test_server.data_size = send_size;
	for(int i = 0;i < (int)send_size;i++)
		send_buff[i] = 'A';
	
	for(int i = 0;i < (int)send_size;i++)
		recv_buff[i] = 'B';
		
	send_data_size = 0;
	receve_data_size = 0;
	test_server.brecv_triger = true;
	
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
	
	test_obj.test_close(ec);
	
	test_server.breq_close_wait_flag = false;	
	test_server.bstop_flag = true;
	server_thread.join();
	
	BOOST_MESSAGE( "----- write_some & read_some test end -----" );
}

void close_test(){
	
	BOOST_MESSAGE( "----- close test start -----" );
	
	test_mirror_server test_server;
	
	// accept req
	test_server.breq_acc_flag = true;
	// close wait req
	test_server.breq_close_wait_flag = true;
		
	// recv cont
	test_server.req_recv_cnt = 1;
	test_server.data_size = 1;
		
	// test server start
	boost::thread server_thread(boost::bind(&test_mirror_server::run,&test_server));
	
	while( !test_server.brun_flag ){
		sleep(1);
	}
	
	std::cout << "ready dummy mirror server" << std::endl;
	
	
	
	boost::asio::ip::tcp::endpoint connect_end(boost::asio::ip::address::from_string(DUMMI_SERVER_IP), DUMMI_SERVER_PORT);
	boost::asio::io_service io;
	boost::system::error_code ec;
	
	l7vs::tcp_socket::tcp_socket_option_info set_option;
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
	
	test_socket_class test_obj(io,set_option);
	test_obj.connect(connect_end,ec);
	BOOST_CHECK(!ec);
	
	while(!test_server.bconnect_flag){
		sleep(1);
	}
	
	BOOST_CHECK(test_obj.get_open_flag());
	
	BOOST_CHECK(!test_server.bdisconnect_flag);
	
	// unit_test [1] close test close success error_code object
	std::cout << "[1] close test close success error_code object" << std::endl;
	test_obj.close(ec);
	BOOST_CHECK(!ec);
	
	test_server.brecv_triger = true;
	sleep(1);
	
	// unit_test [2] close test open_flag set false
	std::cout << "[2] close test open_flag set false" << std::endl;
	BOOST_CHECK(!test_obj.get_open_flag());
	
	// unit_test [3] close test disconnect check
	std::cout << "[3] close test disconnect check" << std::endl;
	BOOST_CHECK(test_server.bdisconnect_flag);
	
	test_server.breq_close_wait_flag = false;	
	test_server.bstop_flag = true;
	server_thread.join();	
	BOOST_MESSAGE( "----- close test end -----" );	
}

void get_socket_test(){
	BOOST_MESSAGE( "----- get_socket test start -----" );
	
	boost::asio::io_service io;
	l7vs::tcp_socket::tcp_socket_option_info set_option;
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
	
	test_socket_class test_obj(io,set_option);
	
	// unit_test [1] get_socket socket check
	std::cout << "[1] get_socket socket check" << std::endl;
	
	boost::asio::ip::tcp::socket& chk_socket = test_obj.get_socket();
	boost::asio::ip::tcp::socket* pref_pointer = test_obj.get_socket_pointer();
	
	BOOST_CHECK_EQUAL(&chk_socket,pref_pointer);
	
	BOOST_MESSAGE( "----- get_socket test end -----" );	
}


class connect_lock_test_class : public l7vs::tcp_socket{
	public:
		boost::thread::id befor_thread_id;
		boost::thread::id after_thread_id;
		
		connect_lock_test_class(boost::asio::io_service& io,const tcp_socket_option_info set_option) : l7vs::tcp_socket(io,set_option){
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
	
	l7vs::tcp_socket::tcp_socket_option_info set_option;
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
		
		close_lock_test_class(boost::asio::io_service& io,const tcp_socket_option_info set_option) : l7vs::tcp_socket(io,set_option){
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
	l7vs::tcp_socket::tcp_socket_option_info set_option;
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

void is_open_test(){
	
	BOOST_MESSAGE( "----- is_open test start -----" );
	
	test_mirror_server test_server;
	
	// accept req
	test_server.breq_acc_flag = true;
	// close wait req
	test_server.breq_close_wait_flag = true;
		
	// recv cont
	test_server.req_recv_cnt = 1;
	test_server.data_size = 1;
		
	// test server start
	boost::thread server_thread(boost::bind(&test_mirror_server::run,&test_server));
	
	while( !test_server.brun_flag ){
		sleep(1);
	}
	
	std::cout << "ready dummy mirror server" << std::endl;
	
	
	
	boost::asio::ip::tcp::endpoint connect_end(boost::asio::ip::address::from_string(DUMMI_SERVER_IP), DUMMI_SERVER_PORT);
	boost::asio::io_service io;
	boost::system::error_code ec;
	
	l7vs::tcp_socket::tcp_socket_option_info set_option;
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
	
	test_socket_class test_obj(io,set_option);

	// unit_test [1] is_open before connect check
	std::cout << "[1] is_open before connect check" << std::endl;
	BOOST_CHECK(!test_obj.is_open());

	test_obj.connect(connect_end,ec);
	BOOST_CHECK(!ec);

	// unit_test [2] is_open after connect check
	std::cout << "[2] is_open after connect check" << std::endl;
	BOOST_CHECK(test_obj.is_open());
	
	while(!test_server.bconnect_flag){
		sleep(1);
	}
	
	BOOST_CHECK(test_obj.get_open_flag());
	
	BOOST_CHECK(!test_server.bdisconnect_flag);
	
	test_obj.close(ec);
	BOOST_CHECK(!ec);

	// unit_test [3] is_open close after check
	std::cout << "[3] is_open close after check" << std::endl;
	BOOST_CHECK(!test_obj.is_open());
	
	test_server.brecv_triger = true;
	sleep(1);
	
	test_server.breq_close_wait_flag = false;	
	test_server.bstop_flag = true;
	server_thread.join();	
	BOOST_MESSAGE( "----- is_open test end -----" );	
}


test_suite*	init_unit_test_suite( int argc, char* argv[] ){

	test_suite* ts = BOOST_TEST_SUITE( "l7vs::tcp_socket class test" );

	ts->add( BOOST_TEST_CASE( &construcor_test ) );
	ts->add( BOOST_TEST_CASE( &get_socket_test ) );
	ts->add( BOOST_TEST_CASE( &connect_test ) );
	ts->add( BOOST_TEST_CASE( &connect_lock_test ) );
	ts->add( BOOST_TEST_CASE( &set_non_blocking_mode_test ) );
	ts->add( BOOST_TEST_CASE( &write_some_read_some_test ) );
	ts->add( BOOST_TEST_CASE( &close_test ) );
	ts->add( BOOST_TEST_CASE( &close_lock_test ) );
	ts->add( BOOST_TEST_CASE( &is_open_test ) );

	framework::master_test_suite().add( ts );

	return NULL;
}
