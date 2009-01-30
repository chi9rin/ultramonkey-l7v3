
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
				brecv_triger(false){
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
						receive_size = con.read_some(boost::asio::buffer(buf,MAX_BUFFER_SIZE),ec);
						
						if(ec){
							if(ec == boost::asio::error::eof || ec == boost::asio::error::connection_reset){
								std::cout << "dummy mirror server detect client disconnect!" << std::endl;
								bdisconnect_flag = true;
							}else{
								std::cout << "dummy mirror server receive NG!" << std::endl;
							}
						}else{
							if(receive_size > 0){
								// send
								con.write_some(boost::asio::buffer(buf,receive_size),ec);
								if(ec){
									std::cout << "dummy mirror server send NG!" << std::endl;
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
		
	test_socket_class(boost::asio::io_service& io) : l7vs::tcp_socket(io){
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
	
	bool get_open_flag(){
		return open_flag;
	}
	
	boost::asio::ip::tcp::socket* get_socket_pointer(){
		return &my_socket;
	}
		
};


//--test case--
// construcor test
void construcor_test(){

	
	BOOST_MESSAGE( "----- construcor test start -----" );
	
	boost::asio::io_service io;
	
	test_socket_class test_obj(io);
	
	// ## construcor test [1] set io object
	std::cout << "construcor test [1] set io object" << std::endl;
	boost::asio::io_service& set_io = test_obj.get_io();
	BOOST_CHECK_EQUAL(&io , &set_io);
	
	// ## construcor test [2] init open_flag
	std::cout << "construcor test [2] init open_flag" << std::endl;
	BOOST_CHECK(!test_obj.get_open_flag());

	
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
	
	// ## connect test [1] connection success error_code object
	std::cout << "connect test [1] connection success error_code object" << std::endl;
	boost::asio::ip::tcp::endpoint connect_end(boost::asio::ip::address::from_string(DUMMI_SERVER_IP), DUMMI_SERVER_PORT);
	boost::asio::io_service io;
	boost::system::error_code ec;
	test_socket_class test_obj(io);
	test_obj.connect(connect_end,ec);
	BOOST_CHECK(!ec);
	
	
	// ## connect test [2] connection success open_flag
	std::cout << "connect test [2] connection success open_flag" << std::endl;
	BOOST_CHECK(test_obj.get_open_flag());
	
	while(!test_server.bconnect_flag){
		sleep(1);
	}
	
	boost::asio::ip::tcp::endpoint chk_end;
	boost::asio::ip::tcp::endpoint ref_end;
	
	// ## connect test [3] connect local endpoint
	std::cout << "connect test [3] connect local endpoint" << std::endl;	
	chk_end = test_obj.get_local_end();
	ref_end = test_server.connect_end;
	BOOST_CHECK_EQUAL(chk_end , ref_end);
	
	
	// ## connect test [4] connect remote endpoint
	std::cout << "connect test [4] connect remote endpoint" << std::endl;
	chk_end = test_obj.get_remote_end();
	ref_end = connect_end;
	BOOST_CHECK_EQUAL(chk_end , ref_end);
		
	test_obj.test_close(ec);
	
	test_server.breq_close_wait_flag = false;	
	test_server.bstop_flag = true;
	server_thread.join();
		
	// ## connect test [5] connection faile error_code object
	std::cout << "connect test [5] connection faile error_code object" << std::endl;
	test_obj.connect(connect_end,ec);
	BOOST_CHECK(ec);
		
	// ## connect test [6] connection faile open_flag
	std::cout << "connect test [6] connection faile open_flag" << std::endl;
	BOOST_CHECK(!test_obj.get_open_flag());
	
	BOOST_MESSAGE( "----- connect test end -----" );
	
}

void set_non_blocking_mode_test(){
	BOOST_MESSAGE( "----- set non blocking mode test start -----" );
	
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
	test_socket_class test_obj(io);	
	test_obj.connect(connect_end,ec);
	BOOST_CHECK(!ec);
	
	while(!test_server.bconnect_flag){
		sleep(1);
	}

	// ## set non blocking mode test [1] set non blocking mode success error_code object
	std::cout << "set non blocking mode test [1] set non blocking mode success error_code object" << std::endl;
	test_obj.set_non_blocking_mode(ec);
	BOOST_CHECK(!ec);
	
	test_obj.test_close(ec);
	
	test_server.breq_close_wait_flag = false;	
	test_server.bstop_flag = true;
	server_thread.join();
	
	// ## set non blocking mode test [2] set non blocking mode faile error_code object
	std::cout << "set non blocking mode test [2] set non blocking mode faile error_code object" << std::endl;
	test_obj.set_non_blocking_mode(ec);
	BOOST_CHECK(ec);
	
	BOOST_MESSAGE( "----- set non blocking mode test end -----" );
}


void write_some_read_some_test(){
	BOOST_MESSAGE( "----- write some read some test start -----" );
	
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
	test_socket_class test_obj(io);
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
	while(true){
		res_size = test_obj.write_some(boost::asio::buffer(send_buff, send_size),ec);
		if(ec){
			if(ec == boost::asio::error::try_again){
				continue;
			}
		}
		break;
	}
	// ## write some read some test [1] size 0 write error_code object
	std::cout << "write some read some test [1] size 0 write error_code object" << std::endl;
	BOOST_CHECK(!ec);
	// ## write some read some test [2] size 0 write size
	std::cout << "write some read some test [2] size 0 write send size" << std::endl;
	BOOST_CHECK_EQUAL(res_size, send_size);
	
	sleep(1);
	
	res_size = test_obj.read_some(boost::asio::buffer(recv_buff, MAX_BUFFER_SIZE),ec);
	
	// ## write some read some test [3] size 0 read error_code object
	std::cout << "write some read some test [3] size 0 read error_code object" << std::endl;
	BOOST_CHECK_EQUAL(ec,boost::asio::error::try_again);
	
	// ## write some read some test [4] size 0 read size
	std::cout << "write some read some test [4] size 0 read size" << std::endl;
	BOOST_CHECK_EQUAL(res_size,0UL);
		
	// size 1	
	send_size = 1;
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
	// ## write some read some test [5] size 1 write error_code object
	std::cout << "write some read some test [5] size 1 write error_code object" << std::endl;
	BOOST_CHECK(!ec);
	// ## write some read some test [6] size 1 write send size
	std::cout << "write some read some test [6] size 1 write size" << std::endl;
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
	// ## write some read some test [7] size 1 read error_code object
	std::cout << "write some read some test [7] size 1 read error_code object" << std::endl;
	BOOST_CHECK(!ec);
	// ## write some read some test [8] size 1 read size
	std::cout << "write some read some test [8] size 1 read size" << std::endl;
	BOOST_CHECK_EQUAL(res_size, send_size);
	
	// ## write some read some test [9] size 1 data check
	std::cout << "write some read some test [9] size 1 data check" << std::endl;
	BOOST_CHECK_EQUAL(send_buff[0],recv_buff[0]);
	
		
	// size MAX_BUFFER_SIZE	
	send_size = MAX_BUFFER_SIZE;
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
	// ## write some read some test [10] size MAX_BUFFER_SIZE write error_code object
	std::cout << "write some read some test [10] size MAX_BUFFER_SIZE write and read error_code object" << std::endl;
	BOOST_CHECK(!ec);
	
	// ## write some read some test [11] size MAX_BUFFER_SIZE write size
	std::cout << "write some read some test [11] size MAX_BUFFER_SIZE write size" << std::endl;		
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
	// ## write some read some test [12] size MAX_BUFFER_SIZE read error_code object
	std::cout << "write some read some test [12] size MAX_BUFFER_SIZE read error_code object" << std::endl;
	BOOST_CHECK(!ec);
	
	// ## write some read some test [13] size MAX_BUFFER_SIZE read size
	std::cout << "write some read some test [13] size MAX_BUFFER_SIZE read size" << std::endl;		
	BOOST_CHECK_EQUAL(receve_data_size, send_size);
	
	// ## write some read some test [14] size MAX_BUFFER_SIZE data check
	std::cout << "write some read some test [14] size MAX_BUFFER_SIZE data check" << std::endl;
	for(int i = 0;i < (int)send_size;i++){
		BOOST_CHECK_EQUAL(send_buff[i],recv_buff[i]);
	}
	
	test_obj.test_close(ec);
	
	test_server.breq_close_wait_flag = false;	
	test_server.bstop_flag = true;
	server_thread.join();
	
	BOOST_MESSAGE( "----- write some read some test end -----" );
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
		
	// test server start
	boost::thread server_thread(boost::bind(&test_mirror_server::run,&test_server));
	
	while( !test_server.brun_flag ){
		sleep(1);
	}
	
	std::cout << "ready dummy mirror server" << std::endl;
	
	
	
	boost::asio::ip::tcp::endpoint connect_end(boost::asio::ip::address::from_string(DUMMI_SERVER_IP), DUMMI_SERVER_PORT);
	boost::asio::io_service io;
	boost::system::error_code ec;
	test_socket_class test_obj(io);	
	test_obj.connect(connect_end,ec);
	BOOST_CHECK(!ec);
	
	while(!test_server.bconnect_flag){
		sleep(1);
	}
	
	BOOST_CHECK(test_obj.get_open_flag());
	
	BOOST_CHECK(!test_server.bdisconnect_flag);
	
	test_server.brecv_triger = true;

	// ## close test [1] close success error_code object
	std::cout << "close test [1] close success error_code object" << std::endl;
	test_obj.close(ec);
	BOOST_CHECK(!ec);
	
	// ## close test [2] open_flag set false
	std::cout << "close test [2] open_flag set false" << std::endl;
	BOOST_CHECK(!test_obj.get_open_flag());
	
	// ## close test [3] disconnect check
	std::cout << "close test [3] disconnect check" << std::endl;
	BOOST_CHECK(test_server.bdisconnect_flag);
	
	test_server.breq_close_wait_flag = false;	
	test_server.bstop_flag = true;
	server_thread.join();	
	BOOST_MESSAGE( "----- close test end -----" );	
}

void get_socket_test(){
	BOOST_MESSAGE( "----- get socket test start -----" );
	
	boost::asio::io_service io;
	test_socket_class test_obj(io);
	
	// ## get socket test [1] socket check
	std::cout << "get socket test [1] socket check" << std::endl;
	
	boost::asio::ip::tcp::socket& chk_socket = test_obj.get_socket();
	boost::asio::ip::tcp::socket* pref_pointer = test_obj.get_socket_pointer();
	
	BOOST_CHECK_EQUAL(&chk_socket,pref_pointer);
	
	BOOST_MESSAGE( "----- get socket test end -----" );	
}


class connect_lock_test_class : public l7vs::tcp_socket{
	public:
		boost::thread::id befor_thread_id;
		boost::thread::id after_thread_id;
		
		connect_lock_test_class(boost::asio::io_service& io) : l7vs::tcp_socket(io){
			
		};
		
		~connect_lock_test_class(){
		};
		
		void connect(const boost::asio::ip::tcp::endpoint connect_endpoint,boost::system::error_code& ec){
			befor_thread_id = boost::this_thread::get_id();
			l7vs::tcp_socket::connect(connect_endpoint,ec);
			after_thread_id = boost::this_thread::get_id();
			
		};
		void mutex_lock(){
			socket_mutex.lock();
		};
		void mutex_unlock(){
			socket_mutex.unlock();
		};
		
		bool mutex_trylock(){
			return socket_mutex.try_lock();
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
	connect_lock_test_class test_obj(io);
	
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
	
	// ## connect lock test [1] thread block test (mutex lock)
	std::cout << "connect lock test [1] thread block test (mutex lock)" << std::endl;
	BOOST_CHECK(test_obj.befor_thread_id == test_id);
	BOOST_CHECK(test_obj.after_thread_id == proc_id);
	
	test_obj.mutex_unlock();
	sleep(1);
	
	// ## connect lock test [2] thread run test (mutex unlock)
	std::cout << "connect lock test [2] thread run test (mutex unlock)" << std::endl;
	BOOST_CHECK(test_obj.befor_thread_id == test_id);
	BOOST_CHECK(test_obj.after_thread_id == test_id);
	
	
	// ## connect lock test [3] thread run after mutex unlock test
	std::cout << "connect lock test [3] thread run after mutex unlock test" << std::endl;
	BOOST_CHECK(test_obj.mutex_trylock());
	test_obj.mutex_unlock();
	
	BOOST_MESSAGE( "----- connetc lock test end -----" );
	
}

class close_lock_test_class : public l7vs::tcp_socket{
	public:
		boost::thread::id befor_thread_id;
		boost::thread::id after_thread_id;
		
		close_lock_test_class(boost::asio::io_service& io) : l7vs::tcp_socket(io){
			
		};
		
		~close_lock_test_class(){
		};
		
		void close(boost::system::error_code& ec){
			befor_thread_id = boost::this_thread::get_id();
			l7vs::tcp_socket::close(ec);
			after_thread_id = boost::this_thread::get_id();
			
		};
		void mutex_lock(){
			socket_mutex.lock();
		};
		void mutex_unlock(){
			socket_mutex.unlock();
		};
		
		bool mutex_trylock(){
			return socket_mutex.try_lock();
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
	close_lock_test_class test_obj(io);
	
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
	
	// ## close lock test [1] thread block test (mutex lock)
	std::cout << "close lock test [1] thread block test (mutex lock)" << std::endl;
	BOOST_CHECK(test_obj.befor_thread_id == test_id);
	BOOST_CHECK(test_obj.after_thread_id == proc_id);
	
	test_obj.mutex_unlock();
	sleep(1);
	
	// ## close lock test [2] thread run test (mutex unlock)
	std::cout << "close lock test [2] thread run test (mutex unlock)" << std::endl;
	BOOST_CHECK(test_obj.befor_thread_id == test_id);
	BOOST_CHECK(test_obj.after_thread_id == test_id);
	
	
	// ## close lock test [3] thread run after mutex unlock test
	std::cout << "close lock test [3] thread run after mutex unlock test" << std::endl;
	BOOST_CHECK(test_obj.mutex_trylock());
	test_obj.mutex_unlock();
	
	BOOST_MESSAGE( "----- close lock test end -----" );
	
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

	framework::master_test_suite().add( ts );

	return NULL;
}
