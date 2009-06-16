#define	TEST_CASE

#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/function_equal.hpp>
#include <boost/test/included/unit_test.hpp>

#include "dummyclass.h"

#include "tcp_session.h"
#include "tcp_session.cpp"
#include "lockfree_queue.h"


using namespace boost::unit_test_framework;

#define UP_THREAD_ALIVE		std::bitset<TCP_SESSION_THREAD_STATE_BIT>(0x0001)
#define DOWN_THREAD_ALIVE	std::bitset<TCP_SESSION_THREAD_STATE_BIT>(0x0002)
#define UP_THREAD_ACTIVE	std::bitset<TCP_SESSION_THREAD_STATE_BIT>(0x0004)
#define DOWN_THREAD_ACTIVE	std::bitset<TCP_SESSION_THREAD_STATE_BIT>(0x0008)
#define UP_THREAD_LOCK 		std::bitset<TCP_SESSION_THREAD_STATE_BIT>(0x0010)
#define DOWN_THREAD_LOCK 	std::bitset<TCP_SESSION_THREAD_STATE_BIT>(0x0020)


// mutex lock test class
class mutex_lock_test : public l7vs::tcp_session{
	public:
		boost::thread::id befor_thread_id;
		boost::thread::id after_thread_id;
		l7vs::wr_mutex* pTest_mutex;
		boost::function< void(void) > test_func;
		
		mutex_lock_test(l7vs::virtualservice_tcp& vs,boost::asio::io_service& session_io) : l7vs::tcp_session(vs,session_io){
			pTest_mutex = NULL;
		};
		
		~mutex_lock_test(){
		};
		
		void test(){
			test_func();
		};
		
		void mutex_lock(){
			if(pTest_mutex == NULL){
				std::cout << "Test code Error!! pTest_mutex = NULL" << std::endl;
			}else{
				pTest_mutex->wrlock();
			}
		};
		
		void mutex_unlock(){
			if(pTest_mutex == NULL){
				std::cout << "Test code Error!! pTest_mutex = NULL" << std::endl;
			}else{
				pTest_mutex->unlock();
			}
		};

		void set_protocol_module(l7vs::protocol_module_base* test_protocol_module){
			protocol_module = test_protocol_module;
		};

		
/*		bool mutex_trylock(){
			return pTest_mutex->try_lock();
		};*/
		
		boost::mutex test_thread_wait;
		
		void test_run(){
			boost::mutex::scoped_lock scope_lock(test_thread_wait);
			test();
		};
		
		//-------------is_thread_wait test---------------------------------
		void set_is_thread_wait_test(){
			pTest_mutex = &thread_state_update_mutex;
			test_func = boost::bind(&mutex_lock_test::down_thread_exit,this);
		};
		
		void is_thread_wait(){
			befor_thread_id = boost::this_thread::get_id();
			l7vs::tcp_session::is_thread_wait();
			after_thread_id = boost::this_thread::get_id();
		};
		
		//-------------down_thread_exit test---------------------------------
		void set_down_thread_exit_test(){
			pTest_mutex = &exit_flag_update_mutex;
			test_func = boost::bind(&mutex_lock_test::down_thread_exit,this);
		};
		
		void down_thread_exit(){
			befor_thread_id = boost::this_thread::get_id();
			l7vs::tcp_session::down_thread_exit(l7vs::tcp_session::LOCAL_PROC);
			after_thread_id = boost::this_thread::get_id();
		};
		
		//-------------up_thread_exit test---------------------------------
		void set_up_thread_exit_test(){
			pTest_mutex = &exit_flag_update_mutex;
			test_func = boost::bind(&mutex_lock_test::up_thread_exit,this);
		};
		
		void up_thread_exit(){
			befor_thread_id = boost::this_thread::get_id();
			l7vs::tcp_session::up_thread_exit(l7vs::tcp_session::LOCAL_PROC);
			after_thread_id = boost::this_thread::get_id();
		};
		
		//-------------up_thread_client_disconnetc_event test---------------------------------
		void set_up_thread_client_disconnect_event_test(){
			pTest_mutex = &module_function_client_disconnect_mutex;
			test_func = boost::bind(&mutex_lock_test::up_thread_client_disconnect_event,this);
		};
		
		void up_thread_client_disconnect_event(){
			befor_thread_id = boost::this_thread::get_id();
			l7vs::tcp_session::up_thread_client_disconnect_event(l7vs::tcp_session::LOCAL_PROC);
			after_thread_id = boost::this_thread::get_id();
		};
		
		//-------------down_thread_client_disconnetc_event test---------------------------------
		void set_down_thread_client_disconnect_event_test(){
			pTest_mutex = &module_function_client_disconnect_mutex;
			test_func = boost::bind(&mutex_lock_test::down_thread_client_disconnect_event,this);
		};
		
		void down_thread_client_disconnect_event(){
			befor_thread_id = boost::this_thread::get_id();
			l7vs::tcp_session::down_thread_client_disconnect_event(l7vs::tcp_session::LOCAL_PROC);
			after_thread_id = boost::this_thread::get_id();
		};
		
		//-------------up_thread_realserver_disconnetc_event test---------------------------------
		void set_up_thread_realserver_disconnect_event_test(){
			pTest_mutex = &module_function_realserver_disconnect_mutex;
			test_func = boost::bind(&mutex_lock_test::up_thread_realserver_disconnect_event,this);
		};
		
		void up_thread_realserver_disconnect_event(){
			befor_thread_id = boost::this_thread::get_id();
			l7vs::tcp_session::up_thread_realserver_disconnect_event(l7vs::tcp_session::LOCAL_PROC);
			after_thread_id = boost::this_thread::get_id();
		};
		
		//-------------down_thread_realserver_disconnetc_event test---------------------------------
		void set_down_thread_realserver_disconnect_event_test(){
			pTest_mutex = &module_function_realserver_disconnect_mutex;
			test_func = boost::bind(&mutex_lock_test::down_thread_realserver_disconnect_event,this);
		};
		
		void down_thread_realserver_disconnect_event(){
			befor_thread_id = boost::this_thread::get_id();
			l7vs::tcp_session::down_thread_realserver_disconnect_event(l7vs::tcp_session::LOCAL_PROC);
			after_thread_id = boost::this_thread::get_id();
		};
		
		//-------------up_thread_sorryserver_disconnetc_event test---------------------------------
		void set_up_thread_sorryserver_disconnect_event_test(){
			pTest_mutex = &module_function_sorryserver_disconnect_mutex;
			test_func = boost::bind(&mutex_lock_test::up_thread_sorryserver_disconnect_event,this);
		};
		
		void up_thread_sorryserver_disconnect_event(){
			befor_thread_id = boost::this_thread::get_id();
			l7vs::tcp_session::up_thread_sorryserver_disconnect_event(l7vs::tcp_session::LOCAL_PROC);
			after_thread_id = boost::this_thread::get_id();
		};
		
		//-------------down_thread_sorryserver_disconnetc_event test---------------------------------
		void set_down_thread_sorryserver_disconnect_event_test(){
			pTest_mutex = &module_function_sorryserver_disconnect_mutex;
			test_func = boost::bind(&mutex_lock_test::down_thread_sorryserver_disconnect_event,this);
		};
		
		void down_thread_sorryserver_disconnect_event(){
			befor_thread_id = boost::this_thread::get_id();
			l7vs::tcp_session::down_thread_sorryserver_disconnect_event(l7vs::tcp_session::LOCAL_PROC);
			after_thread_id = boost::this_thread::get_id();
		};
		
		//-------------up_thread_sorry_enable_event test---------------------------------
		void set_up_thread_sorry_enable_event_test(){
			pTest_mutex = &module_function_sorry_enable_mutex;
			test_func = boost::bind(&mutex_lock_test::up_thread_sorry_enable_event,this);
		};
		
		void up_thread_sorry_enable_event(){
			befor_thread_id = boost::this_thread::get_id();
			l7vs::tcp_session::up_thread_sorry_enable_event(l7vs::tcp_session::LOCAL_PROC);
			after_thread_id = boost::this_thread::get_id();
		};
		
		//-------------up_thread_sorry_disable_event test---------------------------------
		void set_up_thread_sorry_disable_event_test(){
			pTest_mutex = &module_function_sorry_disable_mutex;
			test_func = boost::bind(&mutex_lock_test::up_thread_sorry_disable_event,this);
		};
		
		void up_thread_sorry_disable_event(){
			befor_thread_id = boost::this_thread::get_id();
			l7vs::tcp_session::up_thread_sorry_disable_event(l7vs::tcp_session::LOCAL_PROC);
			after_thread_id = boost::this_thread::get_id();
		};
		//-------------down_thread_sorry_enable_event test---------------------------------
		void set_down_thread_sorry_enable_event_test(){
			pTest_mutex = &module_function_sorry_enable_mutex;
			test_func = boost::bind(&mutex_lock_test::down_thread_sorry_enable_event,this);
		};
		
		void down_thread_sorry_enable_event(){
			befor_thread_id = boost::this_thread::get_id();
			l7vs::tcp_session::down_thread_sorry_enable_event(l7vs::tcp_session::LOCAL_PROC);
			after_thread_id = boost::this_thread::get_id();
		};
		
		//-------------down_thread_sorry_disable_event test---------------------------------
		void set_down_thread_sorry_disable_event_test(){
			pTest_mutex = &module_function_sorry_disable_mutex;
			test_func = boost::bind(&mutex_lock_test::down_thread_sorry_disable_event,this);
		};
		
		void down_thread_sorry_disable_event(){
			befor_thread_id = boost::this_thread::get_id();
			l7vs::tcp_session::down_thread_sorry_disable_event(l7vs::tcp_session::LOCAL_PROC);
			after_thread_id = boost::this_thread::get_id();
		};
		
		//-------------up_thread_client_respond_event test---------------------------------
		void set_up_thread_client_respond_event_test(){
			pTest_mutex = &module_function_response_send_inform_mutex;
			test_func = boost::bind(&mutex_lock_test::up_thread_client_respond_event,this);
		};
		
		void up_thread_client_respond_event(){
			befor_thread_id = boost::this_thread::get_id();
			l7vs::tcp_session::up_thread_client_respond_event(l7vs::tcp_session::LOCAL_PROC);
			after_thread_id = boost::this_thread::get_id();
		};
		
		//-------------down_thread_client_respond_event test---------------------------------
		void set_down_thread_client_respond_event_test(){
			pTest_mutex = &module_function_response_send_inform_mutex;
			test_func = boost::bind(&mutex_lock_test::down_thread_client_respond_event,this);
		};
		
		void down_thread_client_respond_event(){
			befor_thread_id = boost::this_thread::get_id();
			l7vs::tcp_session::down_thread_client_respond_event(l7vs::tcp_session::LOCAL_PROC);
			after_thread_id = boost::this_thread::get_id();
		};
		
		//-------------thread_state_update test---------------------------------
		void set_thread_state_update_test(){
			pTest_mutex = &thread_state_update_mutex;
			test_func = boost::bind(&mutex_lock_test::thread_state_update,this);
		};
		
		void thread_state_update(){
			befor_thread_id = boost::this_thread::get_id();
			l7vs::tcp_session::thread_state_update(UP_THREAD_ALIVE,true);
			after_thread_id = boost::this_thread::get_id();
		};
		
};

// dummy mirror server
#define DUMMI_SERVER_IP 	"127.0.0.1"
#define DUMMI_SERVER_PORT 	7000
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

// module event map test base class
class module_event_map_test_base_class : public l7vs::tcp_session{
	public:
		
		std::pair<l7vs::protocol_module_base::EVENT_TAG, UP_THREAD_FUNC_TYPE_TAG> up_module_map_test_data[13];
		std::pair<UP_THREAD_FUNC_TYPE_TAG , boost::function< void(const l7vs::tcp_session::TCP_PROCESS_TYPE_TAG) > > up_fuc_map_test_data[13];
		std::pair<l7vs::protocol_module_base::EVENT_TAG, DOWN_THREAD_FUNC_TYPE_TAG> down_module_map_test_data[7];
		std::pair<DOWN_THREAD_FUNC_TYPE_TAG , boost::function< void(const l7vs::tcp_session::TCP_PROCESS_TYPE_TAG) > > down_fuc_map_test_data[7];
		
		module_event_map_test_base_class(l7vs::virtualservice_tcp& vs,boost::asio::io_service& session_io) : l7vs::tcp_session(vs,session_io){
			int index;
			boost::function< void(const l7vs::tcp_session::TCP_PROCESS_TYPE_TAG) > func;
			// set test data
			
			up_thread_module_event_map.clear();
//			up_thread_function_map.clear();
			
			index = 0;
			up_module_map_test_data[index].first = l7vs::protocol_module_base::ACCEPT;
			up_module_map_test_data[index].second = l7vs::tcp_session::UP_FUNC_CLIENT_ACCEPT;
			up_fuc_map_test_data[index].first = up_module_map_test_data[index].second;
			up_fuc_map_test_data[index].second = boost::bind(&module_event_map_test_base_class::test_UP_FUNC_CLIENT_ACCEPT,this,_1);
			up_thread_function_array[up_fuc_map_test_data[index].first] = up_fuc_map_test_data[index];
			
			index = 1;
			up_module_map_test_data[index].first = l7vs::protocol_module_base::CLIENT_DISCONNECT;
			up_module_map_test_data[index].second = l7vs::tcp_session::UP_FUNC_CLIENT_DISCONNECT;
			up_fuc_map_test_data[index].first = up_module_map_test_data[index].second;
			up_fuc_map_test_data[index].second = boost::bind(&module_event_map_test_base_class::test_UP_FUNC_CLIENT_DISCONNECT,this,_1);
			up_thread_function_array[up_fuc_map_test_data[index].first] = up_fuc_map_test_data[index];
			
			index = 2;
			up_module_map_test_data[index].first = l7vs::protocol_module_base::CLIENT_RECV;
			up_module_map_test_data[index].second = l7vs::tcp_session::UP_FUNC_CLIENT_RECEIVE;
			up_fuc_map_test_data[index].first = up_module_map_test_data[index].second;
			up_fuc_map_test_data[index].second = boost::bind(&module_event_map_test_base_class::test_UP_FUNC_CLIENT_RECEIVE,this,_1);
			up_thread_function_array[up_fuc_map_test_data[index].first] = up_fuc_map_test_data[index];
			
			index = 3;
			up_module_map_test_data[index].first = l7vs::protocol_module_base::CLIENT_RESPONSE_SEND;
			up_module_map_test_data[index].second = l7vs::tcp_session::UP_FUNC_CLIENT_RESPOND_SEND;
			up_fuc_map_test_data[index].first = up_module_map_test_data[index].second;
			up_fuc_map_test_data[index].second = boost::bind(&module_event_map_test_base_class::test_UP_FUNC_CLIENT_RESPOND_SEND,this,_1);
			up_thread_function_array[up_fuc_map_test_data[index].first] = up_fuc_map_test_data[index];
			
			index = 4;
			up_module_map_test_data[index].first = l7vs::protocol_module_base::REALSERVER_SELECT;
			up_module_map_test_data[index].second = l7vs::tcp_session::UP_FUNC_REALSERVER_GET_DEST_EVENT;
			up_fuc_map_test_data[index].first = up_module_map_test_data[index].second;
			up_fuc_map_test_data[index].second = boost::bind(&module_event_map_test_base_class::test_UP_FUNC_REALSERVER_GET_DEST_EVENT,this,_1);
			up_thread_function_array[up_fuc_map_test_data[index].first] = up_fuc_map_test_data[index];
			
			index = 5;
			up_module_map_test_data[index].first = l7vs::protocol_module_base::REALSERVER_CONNECT;
			up_module_map_test_data[index].second = l7vs::tcp_session::UP_FUNC_REALSERVER_CONNECT;
			up_fuc_map_test_data[index].first = up_module_map_test_data[index].second;
			up_fuc_map_test_data[index].second = boost::bind(&module_event_map_test_base_class::test_UP_FUNC_REALSERVER_CONNECT,this,_1);
			up_thread_function_array[up_fuc_map_test_data[index].first] = up_fuc_map_test_data[index];
			
			index = 6;
			up_module_map_test_data[index].first = l7vs::protocol_module_base::REALSERVER_SEND;
			up_module_map_test_data[index].second = l7vs::tcp_session::UP_FUNC_REALSERVER_SEND;
			up_fuc_map_test_data[index].first = up_module_map_test_data[index].second;
			up_fuc_map_test_data[index].second = boost::bind(&module_event_map_test_base_class::test_UP_FUNC_REALSERVER_SEND,this,_1);
			up_thread_function_array[up_fuc_map_test_data[index].first] = up_fuc_map_test_data[index];
			
			index = 7;
			up_module_map_test_data[index].first = l7vs::protocol_module_base::REALSERVER_DISCONNECT;
			up_module_map_test_data[index].second = l7vs::tcp_session::UP_FUNC_REALSERVER_ALL_DISCONNECT;
			up_fuc_map_test_data[index].first = up_module_map_test_data[index].second;
			up_fuc_map_test_data[index].second = boost::bind(&module_event_map_test_base_class::test_UP_FUNC_REALSERVER_ALL_DISCONNECT,this,_1);
			up_thread_function_array[up_fuc_map_test_data[index].first] = up_fuc_map_test_data[index];
			
			index = 8;
			up_module_map_test_data[index].first = l7vs::protocol_module_base::SORRYSERVER_SELECT;
			up_module_map_test_data[index].second = l7vs::tcp_session::UP_FUNC_SORRYSERVER_GET_DEST;
			up_fuc_map_test_data[index].first = up_module_map_test_data[index].second;
			up_fuc_map_test_data[index].second = boost::bind(&module_event_map_test_base_class::test_UP_FUNC_SORRYSERVER_GET_DEST,this,_1);
			up_thread_function_array[up_fuc_map_test_data[index].first] = up_fuc_map_test_data[index];
			
			index = 9;
			up_module_map_test_data[index].first = l7vs::protocol_module_base::SORRYSERVER_CONNECT;
			up_module_map_test_data[index].second = l7vs::tcp_session::UP_FUNC_SORRYSERVER_CONNECT;
			up_fuc_map_test_data[index].first = up_module_map_test_data[index].second;
			up_fuc_map_test_data[index].second = boost::bind(&module_event_map_test_base_class::test_UP_FUNC_SORRYSERVER_CONNECT,this,_1);
			up_thread_function_array[up_fuc_map_test_data[index].first] = up_fuc_map_test_data[index];
			
			index = 10;
			up_module_map_test_data[index].first = l7vs::protocol_module_base::SORRYSERVER_SEND;
			up_module_map_test_data[index].second = l7vs::tcp_session::UP_FUNC_SORRYSERVER_SEND;
			up_fuc_map_test_data[index].first = up_module_map_test_data[index].second;
			up_fuc_map_test_data[index].second = boost::bind(&module_event_map_test_base_class::test_UP_FUNC_SORRYSERVER_SEND,this,_1);
			up_thread_function_array[up_fuc_map_test_data[index].first] = up_fuc_map_test_data[index];
			
			index = 11;
			up_module_map_test_data[index].first = l7vs::protocol_module_base::SORRYSERVER_DISCONNECT;
			up_module_map_test_data[index].second = l7vs::tcp_session::UP_FUNC_SORRYSERVER_DISCONNECT;
			up_fuc_map_test_data[index].first = up_module_map_test_data[index].second;
			up_fuc_map_test_data[index].second = boost::bind(&module_event_map_test_base_class::test_UP_FUNC_SORRYSERVER_DISCONNECT,this,_1);
			up_thread_function_array[up_fuc_map_test_data[index].first] = up_fuc_map_test_data[index];
			
			index = 12;
			up_module_map_test_data[index].first = l7vs::protocol_module_base::FINALIZE;
			up_module_map_test_data[index].second = l7vs::tcp_session::UP_FUNC_EXIT;
			up_fuc_map_test_data[index].first = up_module_map_test_data[index].second;
			up_fuc_map_test_data[index].second = boost::bind(&module_event_map_test_base_class::test_UP_FUNC_EXIT,this,_1);
			up_thread_function_array[up_fuc_map_test_data[index].first] = up_fuc_map_test_data[index];
			
			for(int i = 0 ; i < 13;i++){
				up_thread_module_event_map.insert(up_module_map_test_data[i]);
			}
			
			down_thread_module_event_map.clear();
//			down_thread_function_map.clear();
			
			index = 0;
			down_module_map_test_data[index].first = l7vs::protocol_module_base::CLIENT_DISCONNECT;
			down_module_map_test_data[index].second = l7vs::tcp_session::DOWN_FUNC_CLIENT_DISCONNECT;
			down_fuc_map_test_data[index].first = down_module_map_test_data[index].second;
			down_fuc_map_test_data[index].second = boost::bind(&module_event_map_test_base_class::test_DOWN_FUNC_CLIENT_DISCONNECT,this,_1);
			down_thread_function_array[down_fuc_map_test_data[index].first] = down_fuc_map_test_data[index];
			
			index = 1;
			down_module_map_test_data[index].first = l7vs::protocol_module_base::CLIENT_CONNECTION_CHECK;
			down_module_map_test_data[index].second = l7vs::tcp_session::DOWN_FUNC_CLIENT_CONNECTION_CHK;
			down_fuc_map_test_data[index].first = down_module_map_test_data[index].second;
			down_fuc_map_test_data[index].second = boost::bind(&module_event_map_test_base_class::test_DOWN_FUNC_CLIENT_CONNECTION_CHK,this,_1);
			down_thread_function_array[down_fuc_map_test_data[index].first] = down_fuc_map_test_data[index];
			
			index = 2;
			down_module_map_test_data[index].first = l7vs::protocol_module_base::REALSERVER_RECV;
			down_module_map_test_data[index].second = l7vs::tcp_session::DOWN_FUNC_REALSERVER_RECEIVE;
			down_fuc_map_test_data[index].first = down_module_map_test_data[index].second;
			down_fuc_map_test_data[index].second = boost::bind(&module_event_map_test_base_class::test_DOWN_FUNC_REALSERVER_RECEIVE,this,_1);
			down_thread_function_array[down_fuc_map_test_data[index].first] = down_fuc_map_test_data[index];
			
			index = 3;
			down_module_map_test_data[index].first = l7vs::protocol_module_base::REALSERVER_DISCONNECT;
			down_module_map_test_data[index].second = l7vs::tcp_session::DOWN_FUNC_REALSERVER_ALL_DISCONNECT;
			down_fuc_map_test_data[index].first = down_module_map_test_data[index].second;
			down_fuc_map_test_data[index].second = boost::bind(&module_event_map_test_base_class::test_DOWN_FUNC_REALSERVER_ALL_DISCONNECT,this,_1);
			down_thread_function_array[down_fuc_map_test_data[index].first] = down_fuc_map_test_data[index];
			
			index = 4;
			down_module_map_test_data[index].first = l7vs::protocol_module_base::SORRYSERVER_RECV;
			down_module_map_test_data[index].second = l7vs::tcp_session::DOWN_FUNC_SORRYSERVER_RECEIVE;
			down_fuc_map_test_data[index].first = down_module_map_test_data[index].second;
			down_fuc_map_test_data[index].second = boost::bind(&module_event_map_test_base_class::test_DOWN_FUNC_SORRYSERVER_RECEIVE,this,_1);
			down_thread_function_array[down_fuc_map_test_data[index].first] = down_fuc_map_test_data[index];
			
			index = 5;
			down_module_map_test_data[index].first = l7vs::protocol_module_base::SORRYSERVER_DISCONNECT;
			down_module_map_test_data[index].second = l7vs::tcp_session::DOWN_FUNC_SORRYSERVER_DISCONNECT;
			down_fuc_map_test_data[index].first = down_module_map_test_data[index].second;
			down_fuc_map_test_data[index].second = boost::bind(&module_event_map_test_base_class::test_DOWN_FUNC_SORRYSERVER_DISCONNECT,this,_1);
			down_thread_function_array[down_fuc_map_test_data[index].first] = down_fuc_map_test_data[index];
			
			index = 6;
			down_module_map_test_data[index].first = l7vs::protocol_module_base::FINALIZE;
			down_module_map_test_data[index].second = l7vs::tcp_session::DOWN_FUNC_EXIT;
			down_fuc_map_test_data[index].first = down_module_map_test_data[index].second;
			down_fuc_map_test_data[index].second = boost::bind(&module_event_map_test_base_class::test_DOWN_FUNC_EXIT,this,_1);
			down_thread_function_array[down_fuc_map_test_data[index].first] = down_fuc_map_test_data[index];
			
			for(int i = 0 ; i < 7;i++){
				down_thread_module_event_map.insert(down_module_map_test_data[i]);
			}
			
			
		};
		
		~module_event_map_test_base_class(){
			up_thread_module_event_map_clear();
			up_thread_function_array_clear();
			down_thread_module_event_map_clear();
			down_thread_function_array_clear();
		};
		
		void set_protocol_module(l7vs::protocol_module_base* test_protocol_module){
			protocol_module = test_protocol_module;
		};
		
		bool chk_up_thread_next_call_function(l7vs::protocol_module_base::EVENT_TAG chk_event){
			l7vs::tcp_session::UP_THREAD_FUNC_TYPE_TAG fnc_tag = l7vs::tcp_session::UP_FUNC_EXIT;
			up_call_func_type = fnc_tag;
			for(int i = 0; i < 13;i++){
				if( up_module_map_test_data[i].first == chk_event){
					fnc_tag = up_module_map_test_data[i].second;
					if(i < 12){
						up_call_func_type = up_module_map_test_data[i+1].second;
					}else{
						up_call_func_type = up_module_map_test_data[0].second;
					}
					break;
				}
			}
			if(fnc_tag == up_call_func_type){
				return false;
			}
			
			up_thread_next_call_function.second(LOCAL_PROC);
			
			return fnc_tag == up_call_func_type;
		};
		
		bool chk_down_thread_next_call_function(l7vs::protocol_module_base::EVENT_TAG chk_event){
			l7vs::tcp_session::DOWN_THREAD_FUNC_TYPE_TAG fnc_tag = l7vs::tcp_session::DOWN_FUNC_EXIT;
			down_call_func_type = fnc_tag;
			for(int i = 0; i < 7;i++){
				if( down_module_map_test_data[i].first == chk_event){
					fnc_tag = down_module_map_test_data[i].second;
					if(i < 6){
						down_call_func_type = down_module_map_test_data[i+1].second;
					}else{
						down_call_func_type = down_module_map_test_data[0].second;
					}
					break;
				}
			}
			if(fnc_tag == down_call_func_type){
				return false;
			}
			
			down_thread_next_call_function.second(LOCAL_PROC);
			
			return fnc_tag == down_call_func_type;
		};
		
		
		void up_thread_module_event_map_clear(){
			up_thread_module_event_map.clear();
		};
		
		void up_thread_function_array_clear(){
			for(int i = 0; i <= l7vs::tcp_session::UP_FUNC_EXIT;i++){
				up_thread_function_array[i].second = (tcp_session_func)NULL;
			}
		};
		
		void down_thread_module_event_map_clear(){
			down_thread_module_event_map.clear();
		};
		
		void down_thread_function_array_clear(){
			for(int i = 0; i <= l7vs::tcp_session::DOWN_FUNC_EXIT;i++){
				down_thread_function_array[i].second = (tcp_session_func)NULL;
			}
		};
		
		void set_up_thread_id(boost::thread::id set_thread_id){
			up_thread_id = set_thread_id;
		};
		void set_down_thread_id(boost::thread::id set_thread_id){
			down_thread_id = set_thread_id;
		};
		
		void set_up_thread_data_client_side(l7vs::tcp_data& set_data){
			up_thread_data_client_side.set_size(set_data.get_size());
			up_thread_data_client_side.set_send_size(set_data.get_send_size());
			up_thread_data_client_side.set_data(set_data.get_data());
			up_thread_data_client_side.set_endpoint(set_data.get_endpoint());
		};
		l7vs::tcp_data& get_up_thread_data_client_side(){
			return up_thread_data_client_side;
		}
		void set_up_thread_data_dest_side(l7vs::tcp_data& set_data){
			up_thread_data_dest_side.set_size(set_data.get_size());
			up_thread_data_dest_side.set_send_size(set_data.get_send_size());
			up_thread_data_dest_side.set_data(set_data.get_data());
			up_thread_data_dest_side.set_endpoint(set_data.get_endpoint());
		};
		l7vs::tcp_data& get_up_thread_data_dest_side(){
			return up_thread_data_dest_side;
		}
		void set_up_thread_message_data(l7vs::tcp_data& set_data){
			up_thread_message_data.set_size(set_data.get_size());
			up_thread_message_data.set_send_size(set_data.get_send_size());
			up_thread_message_data.set_data(set_data.get_data());
			up_thread_message_data.set_endpoint(set_data.get_endpoint());
		};
		l7vs::tcp_data& get_up_thread_message_data(){
			return up_thread_message_data;
		}
		void set_down_thread_data_client_side(l7vs::tcp_data& set_data){
			down_thread_data_client_side.set_size(set_data.get_size());
			down_thread_data_client_side.set_send_size(set_data.get_send_size());
			down_thread_data_client_side.set_data(set_data.get_data());
			down_thread_data_client_side.set_endpoint(set_data.get_endpoint());
		};
		l7vs::tcp_data& get_down_thread_data_client_side(){
			return down_thread_data_client_side;
		}
		void set_down_thread_data_dest_side(l7vs::tcp_data& set_data){
			down_thread_data_dest_side.set_size(set_data.get_size());
			down_thread_data_dest_side.set_send_size(set_data.get_send_size());
			down_thread_data_dest_side.set_data(set_data.get_data());
			down_thread_data_client_side.set_endpoint(set_data.get_endpoint());
		};
		l7vs::tcp_data& get_down_thread_data_dest_side(){
			return down_thread_data_dest_side;
		}
		void set_down_thread_message_data(l7vs::tcp_data& set_data){
			down_thread_message_data.set_size(set_data.get_size());
			down_thread_message_data.set_send_size(set_data.get_send_size());
			down_thread_message_data.set_data(set_data.get_data());
			down_thread_message_data.set_endpoint(set_data.get_endpoint());
		};
		l7vs::tcp_data& get_down_thread_message_data(){
			return down_thread_message_data;
		}
		void test_UP_FUNC_CLIENT_ACCEPT(const l7vs::tcp_session::TCP_PROCESS_TYPE_TAG process_type){
			up_call_func_type = l7vs::tcp_session::UP_FUNC_CLIENT_ACCEPT;
		};
		
		void test_UP_FUNC_CLIENT_DISCONNECT(const l7vs::tcp_session::TCP_PROCESS_TYPE_TAG process_type){
			up_call_func_type = l7vs::tcp_session::UP_FUNC_CLIENT_DISCONNECT;
		};
		
		void test_DOWN_FUNC_CLIENT_DISCONNECT(const l7vs::tcp_session::TCP_PROCESS_TYPE_TAG process_type){
			down_call_func_type = l7vs::tcp_session::DOWN_FUNC_CLIENT_DISCONNECT;
		};
		
		void test_DOWN_FUNC_CLIENT_CONNECTION_CHK(const l7vs::tcp_session::TCP_PROCESS_TYPE_TAG process_type){
			down_call_func_type = l7vs::tcp_session::DOWN_FUNC_CLIENT_CONNECTION_CHK;
		};
		
		
//		void test_UP_FUNC_CLIENT_DISCONNECT_EVENT(const l7vs::tcp_session::TCP_PROCESS_TYPE_TAG process_type){
//			up_call_func_type = l7vs::tcp_session::UP_FUNC_CLIENT_DISCONNECT_EVENT;
//		};
		
		void test_UP_FUNC_CLIENT_RECEIVE(const l7vs::tcp_session::TCP_PROCESS_TYPE_TAG process_type){
			up_call_func_type = l7vs::tcp_session::UP_FUNC_CLIENT_RECEIVE;
		};
		
		void test_UP_FUNC_CLIENT_RESPOND_SEND(const l7vs::tcp_session::TCP_PROCESS_TYPE_TAG process_type){
			up_call_func_type = l7vs::tcp_session::UP_FUNC_CLIENT_RESPOND_SEND;
		};
		
//		void test_UP_FUNC_CLIENT_RESPOND_SEND_EVENT(const TCP_PROCESS_TYPE_TAG process_type){
//			up_call_func_type = l7vs::tcp_session::UP_FUNC_CLIENT_RESPOND_SEND_EVENT;
//		};
		
		void test_DOWN_FUNC_REALSERVER_RECEIVE(const l7vs::tcp_session::TCP_PROCESS_TYPE_TAG process_type){
			down_call_func_type = l7vs::tcp_session::DOWN_FUNC_REALSERVER_RECEIVE;
		};
		
		
		void test_UP_FUNC_REALSERVER_GET_DEST_EVENT(const TCP_PROCESS_TYPE_TAG process_type){
			up_call_func_type = l7vs::tcp_session::UP_FUNC_REALSERVER_GET_DEST_EVENT;
		};
		
		void test_UP_FUNC_REALSERVER_CONNECT(const TCP_PROCESS_TYPE_TAG process_type){
			up_call_func_type = l7vs::tcp_session::UP_FUNC_REALSERVER_CONNECT;
		};
		
//		void test_UP_FUNC_REALSERVER_CONNECT_EVENT(const TCP_PROCESS_TYPE_TAG process_type){
//			up_call_func_type = l7vs::tcp_session::UP_FUNC_REALSERVER_CONNECT_EVENT;
//		};
		
//		void test_UP_FUNC_REALSERVER_CONNECT_FAIL_EVENT(const TCP_PROCESS_TYPE_TAG process_type){
//			up_call_func_type = l7vs::tcp_session::UP_FUNC_REALSERVER_CONNECT_FAIL_EVENT;
//		};
		
		void test_UP_FUNC_REALSERVER_SEND(const TCP_PROCESS_TYPE_TAG process_type){
			up_call_func_type = l7vs::tcp_session::UP_FUNC_REALSERVER_SEND;
		};
		
//		void test_UP_FUNC_REALSERVER_DISCONNECT(const TCP_PROCESS_TYPE_TAG process_type){
//			up_call_func_type = l7vs::tcp_session::UP_FUNC_REALSERVER_DISCONNECT;
//		};
		
//		void test_UP_FUNC_REALSERVER_DISCONNECT_EVENT(const TCP_PROCESS_TYPE_TAG process_type){
//			up_call_func_type = l7vs::tcp_session::UP_FUNC_REALSERVER_DISCONNECT_EVENT;
//		};
		
		void test_UP_FUNC_REALSERVER_ALL_DISCONNECT(const TCP_PROCESS_TYPE_TAG process_type){
			up_call_func_type = l7vs::tcp_session::UP_FUNC_REALSERVER_ALL_DISCONNECT;
		};
		
		void test_DOWN_FUNC_REALSERVER_ALL_DISCONNECT(const TCP_PROCESS_TYPE_TAG process_type){
			down_call_func_type = l7vs::tcp_session::DOWN_FUNC_REALSERVER_ALL_DISCONNECT;
		};

		void test_DOWN_FUNC_SORRYSERVER_RECEIVE(const l7vs::tcp_session::TCP_PROCESS_TYPE_TAG process_type){
			down_call_func_type = l7vs::tcp_session::DOWN_FUNC_SORRYSERVER_RECEIVE;
		};
		
		void test_UP_FUNC_SORRYSERVER_GET_DEST(const TCP_PROCESS_TYPE_TAG process_type){
			up_call_func_type = l7vs::tcp_session::UP_FUNC_SORRYSERVER_GET_DEST;
		};
		
		void test_UP_FUNC_SORRYSERVER_CONNECT(const TCP_PROCESS_TYPE_TAG process_type){
			up_call_func_type = l7vs::tcp_session::UP_FUNC_SORRYSERVER_CONNECT;
		};
		
//		void test_UP_FUNC_SORRYSERVER_CONNECT_EVENT(const TCP_PROCESS_TYPE_TAG process_type){
//			up_call_func_type = l7vs::tcp_session::UP_FUNC_SORRYSERVER_CONNECT_EVENT;
//		};
		
//		void test_UP_FUNC_SORRYSERVER_CONNECT_FAIL_EVENT(const TCP_PROCESS_TYPE_TAG process_type){
//			up_call_func_type = l7vs::tcp_session::UP_FUNC_SORRYSERVER_CONNECT_FAIL_EVENT;
//		};
		
		void test_UP_FUNC_SORRYSERVER_SEND(const TCP_PROCESS_TYPE_TAG process_type){
			up_call_func_type = l7vs::tcp_session::UP_FUNC_SORRYSERVER_SEND;
		};
		
		void test_UP_FUNC_SORRYSERVER_DISCONNECT(const TCP_PROCESS_TYPE_TAG process_type){
			up_call_func_type = l7vs::tcp_session::UP_FUNC_SORRYSERVER_DISCONNECT;
		};
		
		void test_DOWN_FUNC_SORRYSERVER_DISCONNECT(const TCP_PROCESS_TYPE_TAG process_type){
			down_call_func_type = l7vs::tcp_session::DOWN_FUNC_SORRYSERVER_DISCONNECT;
		};
		
//		void test_UP_FUNC_SORRYSERVER_DISCONNECT_EVENT(const TCP_PROCESS_TYPE_TAG process_type){
//			up_call_func_type = l7vs::tcp_session::UP_FUNC_SORRYSERVER_DISCONNECT_EVENT;	
//		};
		
//		void test_UP_FUNC_SORRY_ENABLE_EVENT(const TCP_PROCESS_TYPE_TAG process_type){
//			up_call_func_type = l7vs::tcp_session::UP_FUNC_SORRY_ENABLE_EVENT;
//		};
		
//		void test_UP_FUNC_SORRY_DISABLE_EVENT(const TCP_PROCESS_TYPE_TAG process_type){
//			up_call_func_type = l7vs::tcp_session::UP_FUNC_SORRY_DISABLE_EVENT;
//		};
		
		void test_UP_FUNC_EXIT(const TCP_PROCESS_TYPE_TAG process_type){
			up_call_func_type = l7vs::tcp_session::UP_FUNC_EXIT;
		};
		
		void test_DOWN_FUNC_EXIT(const TCP_PROCESS_TYPE_TAG process_type){
			down_call_func_type = l7vs::tcp_session::DOWN_FUNC_EXIT;
		};
		
		l7vs::tcp_session::UP_THREAD_FUNC_TYPE_TAG up_call_func_type;
		l7vs::tcp_session::DOWN_THREAD_FUNC_TYPE_TAG down_call_func_type;
};



//--test case--
// constructer test
// constructer test class
class constructer_test_class : public l7vs::tcp_session{
	public:
		constructer_test_class(l7vs::virtualservice_tcp& vs,boost::asio::io_service& session_io,const l7vs::tcp_socket::tcp_socket_option_info set_option) : l7vs::tcp_session(vs,session_io,set_option){};
		~constructer_test_class(){};
		boost::asio::io_service& get_io(){
			return io;
		};
		l7vs::virtualservice_tcp& get_parent_service(){
			return parent_service;
		};
		bool get_exit_flag(){
			return exit_flag;
		};
		std::bitset<TCP_SESSION_THREAD_STATE_BIT> get_thread_state(){
			return thread_state;
		};
				
		l7vs::protocol_module_base* get_protocol_module(){
			return protocol_module;
		};
		bool get_session_pause_flag(){
			return session_pause_flag;
		};
		l7vs::tcp_socket& get_client_socket(){
			return client_socket;
		};
		boost::shared_ptr< l7vs::tcp_socket > get_sorry_socket(){
			return sorryserver_socket.second;
		};
		
		l7vs::tcp_socket::tcp_socket_option_info* get_socket_opt_info(){
			return &socket_opt_info;
		}
		
		//! up thread raise module event of handle_accept
		//! @param[in]		process_type is prosecess type
		void up_thread_client_accept_event(const TCP_PROCESS_TYPE_TAG process_type){
			up_thread_client_accept_event_call_check = true;
		}
		bool up_thread_client_accept_event_call_check;
		//! up thread receive client side and raise module event of handle_client_recv
		//! @param[in]		process_type is prosecess type
		void up_thread_client_receive(const TCP_PROCESS_TYPE_TAG process_type){
			up_thread_client_receive_call_check = true;
		};
		bool up_thread_client_receive_call_check;
		//! up thread raise client respond send event message for up and down thread
		//! @param[in]		process_type is prosecess type
		void up_thread_client_respond(const TCP_PROCESS_TYPE_TAG process_type){
			up_thread_client_respond_call_check = true;
		};
		bool up_thread_client_respond_call_check;
		//! up thread raise module event of handle_response_send_inform
		//! @param[in]		process_type is prosecess type
		void up_thread_client_respond_event(const TCP_PROCESS_TYPE_TAG process_type){
			up_thread_client_respond_event_call_check = true;
		};
		bool up_thread_client_respond_event_call_check;
		//! up thread close client socket and raise client disconnect event message for up and down thread
		//! @param[in]		process_type is prosecess type
		void up_thread_client_disconnect(const TCP_PROCESS_TYPE_TAG process_type){
			up_thread_client_disconnect_call_check = true;
		};
		bool up_thread_client_disconnect_call_check;
		//! up thread raise module event of handle_client_disconnect
		//! @param[in]		process_type is prosecess type
		void up_thread_client_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type){
			up_thread_client_disconnect_event_call_check = true;
		};
		bool up_thread_client_disconnect_event_call_check;
		//! up thread send realserver and raise module event of handle_client_recv
		//! @param[in]		process_type is prosecess type
		void up_thread_realserver_send(const TCP_PROCESS_TYPE_TAG process_type){
			up_thread_realserver_send_call_check = true;
		};
		bool up_thread_realserver_send_call_check;
		//! up thread raise module event of handle_realserver_select
		//! @param[in]		process_type is prosecess type
		void up_thread_realserver_get_destination_event(const TCP_PROCESS_TYPE_TAG process_type){
			up_thread_realserver_get_destination_event_call_check = true;
		};
		bool up_thread_realserver_get_destination_event_call_check;
		//! up thread connect realserver
		//! @param[in]		process_type is prosecess type
		void up_thread_realserver_connect(const TCP_PROCESS_TYPE_TAG process_type){
			up_thread_realserver_connect_call_check = true;
		};
		bool up_thread_realserver_connect_call_check;
		//! up thread raise module event of handle_realserver_connect
		//! @param[in]		process_type is prosecess type
		void up_thread_realserver_connect_event(const TCP_PROCESS_TYPE_TAG process_type){
			up_thread_realserver_connect_event_call_check = true;
		};
		bool up_thread_realserver_connect_event_call_check;
		//! up thread raise module event of handle_realserver_connection_fail
		//! @param[in]		process_type is prosecess type
		void up_thread_realserver_connection_fail_event(const TCP_PROCESS_TYPE_TAG process_type){
			up_thread_realserver_connection_fail_event_call_check = true;
		};
		bool up_thread_realserver_connection_fail_event_call_check;
		//! up thread close realserver socket and raise realserver disconnect event message for up and down thread
		//! @param[in]		process_type is prosecess type
		void up_thread_realserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type){
			up_thread_realserver_disconnect_call_check = true;
		};
		bool up_thread_realserver_disconnect_call_check;
		//! up thread raise module event of handle_realserver_disconnect
		//! @param[in]		process_type is prosecess type
		void up_thread_realserver_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type){
			up_thread_realserver_disconnect_event_call_check = true;
		};
		bool up_thread_realserver_disconnect_event_call_check;
		//! up thread close all realserver socket and raise module event of handle_realserver_disconnect
		//! @param[in]		process_type is prosecess type
		void up_thread_all_realserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type){
			up_thread_all_realserver_disconnect_call_check = true;
		};
		bool up_thread_all_realserver_disconnect_call_check;
		//! up thread send sorryserver and raise module event of handle_sorryserver_send
		//! @param[in]		process_type is prosecess type
		void up_thread_sorryserver_send(const TCP_PROCESS_TYPE_TAG process_type){
			up_thread_sorryserver_send_call_check = true;
		};
		bool up_thread_sorryserver_send_call_check;
		//! up thread raise module event of handle_sorryserver_select
		//! @param[in]		process_type is prosecess type
		void up_thread_sorryserver_get_destination_event(const TCP_PROCESS_TYPE_TAG process_type){
			up_thread_sorryserver_get_destination_event_call_check = true;
		};
		bool up_thread_sorryserver_get_destination_event_call_check;
		//! up thread connect sorryserver
		//! @param[in]		process_type is prosecess type
		void up_thread_sorryserver_connect(const TCP_PROCESS_TYPE_TAG process_type){
			up_thread_sorryserver_connect_call_check = true;
		};
		bool up_thread_sorryserver_connect_call_check;
		//! up thread raise module event of handle_sorryserver_connect
		//! @param[in]		process_type is prosecess type
		void up_thread_sorryserver_connect_event(const TCP_PROCESS_TYPE_TAG process_type){
			up_thread_sorryserver_connect_event_call_check = true;
		};
		bool up_thread_sorryserver_connect_event_call_check;
		//! up thread raise module event of handle_sorryserver_connection_fail
		//! @param[in]		process_type is prosecess type
		void up_thread_sorryserver_connection_fail_event(const TCP_PROCESS_TYPE_TAG process_type){
			up_thread_sorryserver_connection_fail_event_call_check = true;
		};
		bool up_thread_sorryserver_connection_fail_event_call_check;
		//! up thread close sorryserver socket and raise sorryserver disconnect event message for up and down thread
		//! @param[in]		process_type is prosecess type
		void up_thread_sorryserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type){
			up_thread_sorryserver_disconnect_call_check = true;
		};
		bool up_thread_sorryserver_disconnect_call_check;

		void up_thread_sorryserver_mod_disconnect(const TCP_PROCESS_TYPE_TAG process_type){
			up_thread_sorryserver_mod_disconnect_call_check = true;
		};
		bool up_thread_sorryserver_mod_disconnect_call_check;

		//! up thread raise module event of handle_sorryserver_disconnect
		//! @param[in]		process_type is prosecess type
		void up_thread_sorryserver_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type){
			up_thread_sorryserver_disconnect_event_call_check = true;
		};
		bool up_thread_sorryserver_disconnect_event_call_check;
		//! up thread raise module event of handle_sorry_enable
		//! @param[in]		process_type is prosecess type
		void up_thread_sorry_enable_event(const TCP_PROCESS_TYPE_TAG process_type){
			up_thread_sorry_enable_event_call_check = true;
		};
		bool up_thread_sorry_enable_event_call_check;
		//! up thread raise module event of handle_sorry_disable
		//! @param[in]		process_type is prosecess type
		void up_thread_sorry_disable_event(const TCP_PROCESS_TYPE_TAG process_type){
			up_thread_sorry_disable_event_call_check = true;
		};
		bool up_thread_sorry_disable_event_call_check;
		//! up thread exit main loop
		//! @param[in]		process_type is prosecess type
		void up_thread_exit(const TCP_PROCESS_TYPE_TAG process_type){
			up_thread_exit_call_check = true;
		};
		bool up_thread_exit_call_check;
		//! down thread receive from realserver and raise module event of handle_realserver_recv
		//! @param[in]		process_type is prosecess type
		void down_thread_realserver_receive(const TCP_PROCESS_TYPE_TAG process_type){
			down_thread_realserver_receive_call_check = true;
		};
		bool down_thread_realserver_receive_call_check;
		//! down thread close realserver socket and raise realserver disconnect event message for up and down thread
		//! @param[in]		process_type is prosecess type
		void down_thread_realserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type){
			down_thread_realserver_disconnect_call_check = true;
		};
		bool down_thread_realserver_disconnect_call_check;
		//! down thread raise module event of handle_realserver_disconnect
		//! @param[in]		process_type is prosecess type
		void down_thread_realserver_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type){
			down_thread_realserver_disconnect_event_call_check = true;
		};
		bool down_thread_realserver_disconnect_event_call_check;
		//! down thread close all realserver socket and raise module event of handle_realserver_disconnect
		//! @param[in]		process_type is prosecess type
		void down_thread_all_realserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type){
			down_thread_all_realserver_disconnect_call_check = true;
		};
		bool down_thread_all_realserver_disconnect_call_check;
		//! down thread raise module event of handle_client_connection_check
		//! @param[in]		process_type is prosecess type
		void down_thread_client_connection_chk_event(const TCP_PROCESS_TYPE_TAG process_type){
			down_thread_client_connection_chk_event_call_check = true;
		};
		bool down_thread_client_connection_chk_event_call_check;
		//! down thread raise module event of handle_response_send_inform
		//! @param[in]		process_type is prosecess type
		void down_thread_client_respond_event(const TCP_PROCESS_TYPE_TAG process_type){
			down_thread_client_respond_event_call_check = true;
		};
		bool down_thread_client_respond_event_call_check;
		//! down thread send for client and raise module event of handle_client_send
		//! @param[in]		process_type is prosecess type
		void down_thread_client_send(const TCP_PROCESS_TYPE_TAG process_type){
			down_thread_client_send_call_check = true;
		};
		bool down_thread_client_send_call_check;
		//! down thread close client socket and raise client disconnect event message for up and down thread 
		//! @param[in]		process_type is prosecess type
		void down_thread_client_disconnect(const TCP_PROCESS_TYPE_TAG process_type){
			down_thread_client_disconnect_call_check = true;
		};
		bool down_thread_client_disconnect_call_check;
		//! down thread raise module event of handle_client_disconnect
		//! @param[in]		process_type is prosecess type
		void down_thread_client_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type){
			down_thread_client_disconnect_event_call_check = true;
		};
		bool down_thread_client_disconnect_event_call_check;
		//! down thread receive from sorryserver and raise module event of handle_sorryserver_recv
		//! @param[in]		process_type is prosecess type
		void down_thread_sorryserver_receive(const TCP_PROCESS_TYPE_TAG process_type){
			down_thread_sorryserver_receive_call_check = true;
		};
		bool down_thread_sorryserver_receive_call_check;
		//! down thread close sorryserver socket and raise sorryserver disconnect event message for up and down thread
		//! @param[in]		process_type is prosecess type
		void down_thread_sorryserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type){
			down_thread_sorryserver_disconnect_call_check = true;
		};
		bool down_thread_sorryserver_disconnect_call_check;

		void down_thread_sorryserver_mod_disconnect(const TCP_PROCESS_TYPE_TAG process_type){
			down_thread_sorryserver_mod_disconnect_call_check = true;
		};
		bool down_thread_sorryserver_mod_disconnect_call_check;
		//! down thread raise module event of handle_sorryserver_disconnect
		//! @param[in]		process_type is prosecess type
		void down_thread_sorryserver_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type){
			down_thread_sorryserver_disconnect_event_call_check = true;
		};
		bool down_thread_sorryserver_disconnect_event_call_check;
		//! down thread raise module event of handle_sorry_enable
		//! @param[in]		process_type is prosecess type
		void down_thread_sorry_enable_event(const TCP_PROCESS_TYPE_TAG process_type){
			down_thread_sorry_enable_event_call_check = true;
		};
		bool down_thread_sorry_enable_event_call_check;
		//! down thread raise module event of handle_sorry_disable
		//! @param[in]		process_type is prosecess type
		void down_thread_sorry_disable_event(const TCP_PROCESS_TYPE_TAG process_type){
			down_thread_sorry_disable_event_call_check = true;
		};
		bool down_thread_sorry_disable_event_call_check;
		//! down thread exit main loop
		//! @param[in]		process_type is prosecess type
		void down_thread_exit(const TCP_PROCESS_TYPE_TAG process_type){
			down_thread_exit_call_check = true;
		};
		bool down_thread_exit_call_check;
		
		
		
		void check_up_thread_module_event_map(){
			BOOST_CHECK(up_thread_module_event_map.find(l7vs::protocol_module_base::INITIALIZE) == up_thread_module_event_map.end());
			BOOST_CHECK(up_thread_module_event_map.find(l7vs::protocol_module_base::ACCEPT)->second == l7vs::tcp_session::UP_FUNC_CLIENT_ACCEPT);	
			BOOST_CHECK(up_thread_module_event_map.find(l7vs::protocol_module_base::CLIENT_RECV)->second == l7vs::tcp_session::UP_FUNC_CLIENT_RECEIVE);
			BOOST_CHECK(up_thread_module_event_map.find(l7vs::protocol_module_base::REALSERVER_SELECT)->second == l7vs::tcp_session::UP_FUNC_REALSERVER_GET_DEST_EVENT);
			BOOST_CHECK(up_thread_module_event_map.find(l7vs::protocol_module_base::REALSERVER_CONNECT)->second == l7vs::tcp_session::UP_FUNC_REALSERVER_CONNECT);
			BOOST_CHECK(up_thread_module_event_map.find(l7vs::protocol_module_base::REALSERVER_SEND)->second == l7vs::tcp_session::UP_FUNC_REALSERVER_SEND);
			BOOST_CHECK(up_thread_module_event_map.find(l7vs::protocol_module_base::SORRYSERVER_SELECT)->second == l7vs::tcp_session::UP_FUNC_SORRYSERVER_GET_DEST);
			BOOST_CHECK(up_thread_module_event_map.find(l7vs::protocol_module_base::SORRYSERVER_CONNECT)->second == l7vs::tcp_session::UP_FUNC_SORRYSERVER_CONNECT);
			BOOST_CHECK(up_thread_module_event_map.find(l7vs::protocol_module_base::SORRYSERVER_SEND)->second == l7vs::tcp_session::UP_FUNC_SORRYSERVER_SEND);
			BOOST_CHECK(up_thread_module_event_map.find(l7vs::protocol_module_base::REALSERVER_RECV) == up_thread_module_event_map.end());
			BOOST_CHECK(up_thread_module_event_map.find(l7vs::protocol_module_base::SORRYSERVER_RECV) == up_thread_module_event_map.end());
			BOOST_CHECK(up_thread_module_event_map.find(l7vs::protocol_module_base::CLIENT_SELECT) == up_thread_module_event_map.end());
			BOOST_CHECK(up_thread_module_event_map.find(l7vs::protocol_module_base::CLIENT_CONNECTION_CHECK) == up_thread_module_event_map.end());
			BOOST_CHECK(up_thread_module_event_map.find(l7vs::protocol_module_base::CLIENT_SEND) == up_thread_module_event_map.end());
			BOOST_CHECK(up_thread_module_event_map.find(l7vs::protocol_module_base::CLIENT_RESPONSE_SEND)->second == l7vs::tcp_session::UP_FUNC_CLIENT_RESPOND_SEND);
			BOOST_CHECK(up_thread_module_event_map.find(l7vs::protocol_module_base::REALSERVER_DISCONNECT)->second == l7vs::tcp_session::UP_FUNC_REALSERVER_ALL_DISCONNECT);
			BOOST_CHECK(up_thread_module_event_map.find(l7vs::protocol_module_base::SORRYSERVER_DISCONNECT)->second == l7vs::tcp_session::UP_FUNC_SORRYSERVER_MOD_DISCONNECT);
			BOOST_CHECK(up_thread_module_event_map.find(l7vs::protocol_module_base::CLIENT_DISCONNECT)->second == l7vs::tcp_session::UP_FUNC_CLIENT_DISCONNECT);
			BOOST_CHECK(up_thread_module_event_map.find(l7vs::protocol_module_base::REALSERVER_CLOSE) == up_thread_module_event_map.end());
			BOOST_CHECK(up_thread_module_event_map.find(l7vs::protocol_module_base::FINALIZE)->second == l7vs::tcp_session::UP_FUNC_EXIT);
			BOOST_CHECK(up_thread_module_event_map.find(l7vs::protocol_module_base::STOP) == up_thread_module_event_map.end());
		};
		void check_down_thread_module_event_map(){
			BOOST_CHECK(down_thread_module_event_map.find(l7vs::protocol_module_base::INITIALIZE) == down_thread_module_event_map.end());
			BOOST_CHECK(down_thread_module_event_map.find(l7vs::protocol_module_base::ACCEPT) == down_thread_module_event_map.end());	
			BOOST_CHECK(down_thread_module_event_map.find(l7vs::protocol_module_base::CLIENT_RECV) == down_thread_module_event_map.end());
			BOOST_CHECK(down_thread_module_event_map.find(l7vs::protocol_module_base::REALSERVER_SELECT) == down_thread_module_event_map.end());
			BOOST_CHECK(down_thread_module_event_map.find(l7vs::protocol_module_base::REALSERVER_CONNECT) == down_thread_module_event_map.end());
			BOOST_CHECK(down_thread_module_event_map.find(l7vs::protocol_module_base::REALSERVER_SEND) == down_thread_module_event_map.end());
			BOOST_CHECK(down_thread_module_event_map.find(l7vs::protocol_module_base::SORRYSERVER_SELECT) == down_thread_module_event_map.end());
			BOOST_CHECK(down_thread_module_event_map.find(l7vs::protocol_module_base::SORRYSERVER_CONNECT) == down_thread_module_event_map.end());
			BOOST_CHECK(down_thread_module_event_map.find(l7vs::protocol_module_base::SORRYSERVER_SEND) == down_thread_module_event_map.end());
			BOOST_CHECK(down_thread_module_event_map.find(l7vs::protocol_module_base::REALSERVER_RECV)->second == l7vs::tcp_session::DOWN_FUNC_REALSERVER_RECEIVE);
			BOOST_CHECK(down_thread_module_event_map.find(l7vs::protocol_module_base::SORRYSERVER_RECV)->second == l7vs::tcp_session::DOWN_FUNC_SORRYSERVER_RECEIVE);
			BOOST_CHECK(down_thread_module_event_map.find(l7vs::protocol_module_base::CLIENT_SELECT) == down_thread_module_event_map.end());
			BOOST_CHECK(down_thread_module_event_map.find(l7vs::protocol_module_base::CLIENT_CONNECTION_CHECK)->second == l7vs::tcp_session::DOWN_FUNC_CLIENT_CONNECTION_CHK);
			BOOST_CHECK(down_thread_module_event_map.find(l7vs::protocol_module_base::CLIENT_SEND)->second == l7vs::tcp_session::DOWN_FUNC_CLIENT_SEND);
			BOOST_CHECK(down_thread_module_event_map.find(l7vs::protocol_module_base::CLIENT_RESPONSE_SEND) == down_thread_module_event_map.end());
			BOOST_CHECK(down_thread_module_event_map.find(l7vs::protocol_module_base::REALSERVER_DISCONNECT)->second == l7vs::tcp_session::DOWN_FUNC_REALSERVER_ALL_DISCONNECT);
			BOOST_CHECK(down_thread_module_event_map.find(l7vs::protocol_module_base::SORRYSERVER_DISCONNECT)->second == l7vs::tcp_session::DOWN_FUNC_SORRYSERVER_MOD_DISCONNECT);
			BOOST_CHECK(down_thread_module_event_map.find(l7vs::protocol_module_base::CLIENT_DISCONNECT)->second == l7vs::tcp_session::DOWN_FUNC_CLIENT_DISCONNECT);
			BOOST_CHECK(down_thread_module_event_map.find(l7vs::protocol_module_base::REALSERVER_CLOSE) == down_thread_module_event_map.end());
			BOOST_CHECK(down_thread_module_event_map.find(l7vs::protocol_module_base::FINALIZE)->second == l7vs::tcp_session::DOWN_FUNC_EXIT);
			BOOST_CHECK(down_thread_module_event_map.find(l7vs::protocol_module_base::STOP) == down_thread_module_event_map.end());
		};
		
		void check_up_thread_function_map(){
			up_thread_function_pair check_it;
			
			// UP_FUNC_CLIENT_ACCEPT  up_thread_client_accept_event function 
			check_it = up_thread_function_array[UP_FUNC_CLIENT_ACCEPT];
			BOOST_CHECK(check_it.second != NULL);
			up_thread_client_accept_event_call_check = false;
			check_it.second(LOCAL_PROC);
			BOOST_CHECK(up_thread_client_accept_event_call_check);
			
			// UP_FUNC_CLIENT_DISCONNECT  up_thread_client_disconnect function 
			check_it = up_thread_function_array[UP_FUNC_CLIENT_DISCONNECT];
			BOOST_CHECK(check_it.second != NULL);
			up_thread_client_disconnect_call_check = false;
			check_it.second(LOCAL_PROC);
			BOOST_CHECK(up_thread_client_disconnect_call_check);
			
			// UP_FUNC_CLIENT_DISCONNECT_EVENT  up_thread_client_disconnect_event function 
			check_it = up_thread_function_array[UP_FUNC_CLIENT_DISCONNECT_EVENT];
			BOOST_CHECK(check_it.second != NULL);
			up_thread_client_disconnect_event_call_check = false;
			check_it.second(LOCAL_PROC);
			BOOST_CHECK(up_thread_client_disconnect_event_call_check);
			
			// UP_FUNC_CLIENT_RECEIVE  up_thread_client_receive function 
			check_it = up_thread_function_array[UP_FUNC_CLIENT_RECEIVE];
			BOOST_CHECK(check_it.second != NULL);
			up_thread_client_receive_call_check = false;
			check_it.second(LOCAL_PROC);
			BOOST_CHECK(up_thread_client_receive_call_check);
			
			// UP_FUNC_CLIENT_RESPOND_SEND  up_thread_client_respond function 
			check_it = up_thread_function_array[UP_FUNC_CLIENT_RESPOND_SEND];
			BOOST_CHECK(check_it.second != NULL);
			up_thread_client_respond_call_check = false;
			check_it.second(LOCAL_PROC);
			BOOST_CHECK(up_thread_client_respond_call_check);
			
			// UP_FUNC_CLIENT_RESPOND_SEND_EVENT  up_thread_client_respond_event function 
			check_it = up_thread_function_array[UP_FUNC_CLIENT_RESPOND_SEND_EVENT];
			BOOST_CHECK(check_it.second != NULL);
			up_thread_client_respond_event_call_check = false;
			check_it.second(LOCAL_PROC);
			BOOST_CHECK(up_thread_client_respond_event_call_check);
			
			// UP_FUNC_REALSERVER_GET_DEST_EVENT  up_thread_realserver_get_destination_event function 
			check_it = up_thread_function_array[UP_FUNC_REALSERVER_GET_DEST_EVENT];
			BOOST_CHECK(check_it.second != NULL);
			up_thread_realserver_get_destination_event_call_check = false;
			check_it.second(LOCAL_PROC);
			BOOST_CHECK(up_thread_realserver_get_destination_event_call_check);
			
			// UP_FUNC_REALSERVER_CONNECT  up_thread_realserver_connect function 
			check_it = up_thread_function_array[UP_FUNC_REALSERVER_CONNECT];
			BOOST_CHECK(check_it.second != NULL);
			up_thread_realserver_connect_call_check = false;
			check_it.second(LOCAL_PROC);
			BOOST_CHECK(up_thread_realserver_connect_call_check);
			
			// UP_FUNC_REALSERVER_CONNECT_EVENT  up_thread_realserver_connect_event function 
			check_it = up_thread_function_array[UP_FUNC_REALSERVER_CONNECT_EVENT];
			BOOST_CHECK(check_it.second != NULL);
			up_thread_realserver_connect_event_call_check = false;
			check_it.second(LOCAL_PROC);
			BOOST_CHECK(up_thread_realserver_connect_event_call_check);
			
			// UP_FUNC_REALSERVER_CONNECT_FAIL_EVENT  up_thread_realserver_connection_fail_event function 
			check_it = up_thread_function_array[UP_FUNC_REALSERVER_CONNECT_FAIL_EVENT];
			BOOST_CHECK(check_it.second != NULL);
			up_thread_realserver_connection_fail_event_call_check = false;
			check_it.second(LOCAL_PROC);
			BOOST_CHECK(up_thread_realserver_connection_fail_event_call_check);
			
			// UP_FUNC_REALSERVER_SEND  up_thread_realserver_send function 
			check_it = up_thread_function_array[UP_FUNC_REALSERVER_SEND];
			BOOST_CHECK(check_it.second != NULL);
			up_thread_realserver_send_call_check = false;
			check_it.second(LOCAL_PROC);
			BOOST_CHECK(up_thread_realserver_send_call_check);
			
			// UP_FUNC_REALSERVER_DISCONNECT  up_thread_realserver_disconnect function 
			check_it = up_thread_function_array[UP_FUNC_REALSERVER_DISCONNECT];
			BOOST_CHECK(check_it.second != NULL);
			up_thread_realserver_disconnect_call_check = false;
			check_it.second(LOCAL_PROC);
			BOOST_CHECK(up_thread_realserver_disconnect_call_check);
			
			// UP_FUNC_REALSERVER_DISCONNECT_EVENT  up_thread_realserver_disconnect_event function 
			check_it = up_thread_function_array[UP_FUNC_REALSERVER_DISCONNECT_EVENT];
			BOOST_CHECK(check_it.second != NULL);
			up_thread_realserver_disconnect_event_call_check = false;
			check_it.second(LOCAL_PROC);
			BOOST_CHECK(up_thread_realserver_disconnect_event_call_check);
			
			// UP_FUNC_REALSERVER_ALL_DISCONNECT  up_thread_all_realserver_disconnect function 
			check_it = up_thread_function_array[UP_FUNC_REALSERVER_ALL_DISCONNECT];
			BOOST_CHECK(check_it.second != NULL);
			up_thread_all_realserver_disconnect_call_check = false;
			check_it.second(LOCAL_PROC);
			BOOST_CHECK(up_thread_all_realserver_disconnect_call_check);
			
			// UP_FUNC_SORRYSERVER_GET_DEST  up_thread_sorryserver_get_destination_event function 
			check_it = up_thread_function_array[UP_FUNC_SORRYSERVER_GET_DEST];
			BOOST_CHECK(check_it.second != NULL);
			up_thread_sorryserver_get_destination_event_call_check = false;
			check_it.second(LOCAL_PROC);
			BOOST_CHECK(up_thread_sorryserver_get_destination_event_call_check);
			
			// UP_FUNC_SORRYSERVER_CONNECT  up_thread_sorryserver_connect function 
			check_it = up_thread_function_array[UP_FUNC_SORRYSERVER_CONNECT];
			BOOST_CHECK(check_it.second != NULL);
			up_thread_sorryserver_connect_call_check = false;
			check_it.second(LOCAL_PROC);
			BOOST_CHECK(up_thread_sorryserver_connect_call_check);
			
			// UP_FUNC_SORRYSERVER_CONNECT_EVENT  up_thread_sorryserver_connect_event function 
			check_it = up_thread_function_array[UP_FUNC_SORRYSERVER_CONNECT_EVENT];
			BOOST_CHECK(check_it.second != NULL);
			up_thread_sorryserver_connect_event_call_check = false;
			check_it.second(LOCAL_PROC);
			BOOST_CHECK(up_thread_sorryserver_connect_event_call_check);
			
			// UP_FUNC_SORRYSERVER_CONNECT_FAIL_EVENT  up_thread_sorryserver_connection_fail_event function 
			check_it = up_thread_function_array[UP_FUNC_SORRYSERVER_CONNECT_FAIL_EVENT];
			BOOST_CHECK(check_it.second != NULL);
			up_thread_sorryserver_connection_fail_event_call_check = false;
			check_it.second(LOCAL_PROC);
			BOOST_CHECK(up_thread_sorryserver_connection_fail_event_call_check);
			
			// UP_FUNC_SORRYSERVER_SEND  up_thread_sorryserver_send function 
			check_it = up_thread_function_array[UP_FUNC_SORRYSERVER_SEND];
			BOOST_CHECK(check_it.second != NULL);
			up_thread_sorryserver_send_call_check = false;
			check_it.second(LOCAL_PROC);
			BOOST_CHECK(up_thread_sorryserver_send_call_check);
			
			// UP_FUNC_SORRYSERVER_DISCONNECT  up_thread_sorryserver_disconnect function 
			check_it = up_thread_function_array[UP_FUNC_SORRYSERVER_DISCONNECT];
			BOOST_CHECK(check_it.second != NULL);
			up_thread_sorryserver_disconnect_call_check = false;
			check_it.second(LOCAL_PROC);
			BOOST_CHECK(up_thread_sorryserver_disconnect_call_check);
			
			// UP_FUNC_SORRYSERVER_MOD_DISCONNECT  up_thread_sorryserver_mod_disconnect function 
			check_it = up_thread_function_array[UP_FUNC_SORRYSERVER_MOD_DISCONNECT];
			BOOST_CHECK(check_it.second != NULL);
			up_thread_sorryserver_mod_disconnect_call_check = false;
			check_it.second(LOCAL_PROC);
			BOOST_CHECK(up_thread_sorryserver_mod_disconnect_call_check);
			
			// UP_FUNC_SORRYSERVER_DISCONNECT_EVENT  up_thread_sorryserver_disconnect_event function 
			check_it = up_thread_function_array[UP_FUNC_SORRYSERVER_DISCONNECT_EVENT];
			BOOST_CHECK(check_it.second != NULL);
			up_thread_sorryserver_disconnect_event_call_check = false;
			check_it.second(LOCAL_PROC);
			BOOST_CHECK(up_thread_sorryserver_disconnect_event_call_check);
			
			// UP_FUNC_SORRY_ENABLE_EVENT  up_thread_sorry_enable_event function 
			check_it = up_thread_function_array[UP_FUNC_SORRY_ENABLE_EVENT];
			BOOST_CHECK(check_it.second == NULL);
			
			// UP_FUNC_SORRY_DISABLE_EVENT  up_thread_sorry_disable_event function 
			check_it = up_thread_function_array[UP_FUNC_SORRY_DISABLE_EVENT];
			BOOST_CHECK(check_it.second == NULL);
			
			// UP_FUNC_EXIT  up_thread_exit function 
			check_it = up_thread_function_array[UP_FUNC_EXIT];
			BOOST_CHECK(check_it.second != NULL);
			up_thread_exit_call_check = false;
			check_it.second(LOCAL_PROC);
			BOOST_CHECK(up_thread_exit_call_check);
			
		};
		
		void check_down_thread_function_map(){
			down_thread_function_pair check_it;
			
			// DOWN_FUNC_CLIENT_DISCONNECT  down_thread_client_disconnect function 
			check_it = down_thread_function_array[DOWN_FUNC_CLIENT_DISCONNECT];
			BOOST_CHECK(check_it.second != NULL);
			down_thread_client_disconnect_call_check = false;
			check_it.second(LOCAL_PROC);
			BOOST_CHECK(down_thread_client_disconnect_call_check);
			
			// DOWN_FUNC_CLIENT_DISCONNECT_EVENT  down_thread_client_disconnect_event function 
			check_it = down_thread_function_array[DOWN_FUNC_CLIENT_DISCONNECT_EVENT];
			BOOST_CHECK(check_it.second != NULL);
			down_thread_client_disconnect_event_call_check = false;
			check_it.second(LOCAL_PROC);
			BOOST_CHECK(down_thread_client_disconnect_event_call_check);
			
			// DOWN_FUNC_CLIENT_CONNECTION_CHK  down_thread_client_connection_chk_event function 
			check_it = down_thread_function_array[DOWN_FUNC_CLIENT_CONNECTION_CHK];
			BOOST_CHECK(check_it.second != NULL);
			down_thread_client_connection_chk_event_call_check = false;
			check_it.second(LOCAL_PROC);
			BOOST_CHECK(down_thread_client_connection_chk_event_call_check);
			
			// DOWN_FUNC_CLIENT_SEND  down_thread_client_send function 
			check_it = down_thread_function_array[DOWN_FUNC_CLIENT_SEND];
			BOOST_CHECK(check_it.second != NULL);
			down_thread_client_send_call_check = false;
			check_it.second(LOCAL_PROC);
			BOOST_CHECK(down_thread_client_send_call_check);
			
			// DOWN_FUNC_CLIENT_RESPOND_SEND_EVENT  down_thread_client_respond_event function 
			check_it = down_thread_function_array[DOWN_FUNC_CLIENT_RESPOND_SEND_EVENT];
			BOOST_CHECK(check_it.second == NULL);
			
			// DOWN_FUNC_REALSERVER_RECEIVE  down_thread_realserver_receive function 
			check_it = down_thread_function_array[DOWN_FUNC_REALSERVER_RECEIVE];
			BOOST_CHECK(check_it.second != NULL);
			down_thread_realserver_receive_call_check = false;
			check_it.second(LOCAL_PROC);
			BOOST_CHECK(down_thread_realserver_receive_call_check);
			
			// DOWN_FUNC_REALSERVER_DISCONNECT  down_thread_realserver_disconnect function 
			check_it = down_thread_function_array[DOWN_FUNC_REALSERVER_DISCONNECT];
			BOOST_CHECK(check_it.second != NULL);
			down_thread_realserver_disconnect_call_check = false;
			check_it.second(LOCAL_PROC);
			BOOST_CHECK(down_thread_realserver_disconnect_call_check);
			
			// DOWN_FUNC_REALSERVER_DISCONNECT_EVENT  down_thread_realserver_disconnect_event function 
			check_it = down_thread_function_array[DOWN_FUNC_REALSERVER_DISCONNECT_EVENT];
			BOOST_CHECK(check_it.second != NULL);
			down_thread_realserver_disconnect_event_call_check = false;
			check_it.second(LOCAL_PROC);
			BOOST_CHECK(down_thread_realserver_disconnect_event_call_check);
			
			// DOWN_FUNC_REALSERVER_ALL_DISCONNECT  down_thread_all_realserver_disconnect function 
			check_it = down_thread_function_array[DOWN_FUNC_REALSERVER_ALL_DISCONNECT];
			BOOST_CHECK(check_it.second != NULL);
			down_thread_all_realserver_disconnect_call_check = false;
			check_it.second(LOCAL_PROC);
			BOOST_CHECK(down_thread_all_realserver_disconnect_call_check);
			
			// DOWN_FUNC_SORRYSERVER_RECEIVE  down_thread_sorryserver_receive function 
			check_it = down_thread_function_array[DOWN_FUNC_SORRYSERVER_RECEIVE];
			BOOST_CHECK(check_it.second != NULL);
			down_thread_sorryserver_receive_call_check = false;
			check_it.second(LOCAL_PROC);
			BOOST_CHECK(down_thread_sorryserver_receive_call_check);
			
			// DOWN_FUNC_SORRYSERVER_DISCONNECT  down_thread_sorryserver_disconnect function 
			check_it = down_thread_function_array[DOWN_FUNC_SORRYSERVER_DISCONNECT];
			BOOST_CHECK(check_it.second != NULL);
			down_thread_sorryserver_disconnect_call_check = false;
			check_it.second(LOCAL_PROC);
			BOOST_CHECK(down_thread_sorryserver_disconnect_call_check);
			
			// DOWN_FUNC_SORRYSERVER_MOD_DISCONNECT  down_thread_sorryserver_mod_disconnect function
			check_it = down_thread_function_array[DOWN_FUNC_SORRYSERVER_MOD_DISCONNECT];
			BOOST_CHECK(check_it.second != NULL);
			down_thread_sorryserver_mod_disconnect_call_check = false;
			check_it.second(LOCAL_PROC);
			BOOST_CHECK(down_thread_sorryserver_mod_disconnect_call_check);
			
			// DOWN_FUNC_SORRYSERVER_DISCONNECT_EVENT  down_thread_sorryserver_disconnect_event function 
			check_it = down_thread_function_array[DOWN_FUNC_SORRYSERVER_DISCONNECT_EVENT];
			BOOST_CHECK(check_it.second != NULL);
			down_thread_sorryserver_disconnect_event_call_check = false;
			check_it.second(LOCAL_PROC);
			BOOST_CHECK(down_thread_sorryserver_disconnect_event_call_check);
			
			// DOWN_FUNC_SORRY_ENABLE_EVENT  down_thread_sorry_enable_event function 
			check_it = down_thread_function_array[DOWN_FUNC_SORRY_ENABLE_EVENT];
			BOOST_CHECK(check_it.second == NULL);
			
			// DOWN_FUNC_SORRY_DISABLE_EVENT  down_thread_sorry_disable_event function 
			check_it = down_thread_function_array[DOWN_FUNC_SORRY_DISABLE_EVENT];
			BOOST_CHECK(check_it.second == NULL);
			
			// DOWN_FUNC_EXIT  down_thread_exit function 
			check_it = down_thread_function_array[DOWN_FUNC_EXIT];
			BOOST_CHECK(check_it.second != NULL);
			down_thread_exit_call_check = false;
			check_it.second(LOCAL_PROC);
			BOOST_CHECK(down_thread_exit_call_check);
			
		}
		
		void check_up_thread_message_down_thread_function_map(){
			std::map< DOWN_THREAD_FUNC_TYPE_TAG, tcp_session_func >::iterator check_it;
			
			// DOWN_FUNC_CLIENT_DISCONNECT  down_thread_client_disconnect function 
			check_it = up_thread_message_down_thread_function_map.find(DOWN_FUNC_CLIENT_DISCONNECT);
			BOOST_CHECK(check_it == up_thread_message_down_thread_function_map.end());
			
			// DOWN_FUNC_CLIENT_DISCONNECT_EVENT  down_thread_client_disconnect_event function 
			check_it = up_thread_message_down_thread_function_map.find(DOWN_FUNC_CLIENT_DISCONNECT_EVENT);
			BOOST_CHECK(check_it != up_thread_message_down_thread_function_map.end());
			down_thread_client_disconnect_event_call_check = false;
			check_it->second(LOCAL_PROC);
			BOOST_CHECK(down_thread_client_disconnect_event_call_check);
			
			// DOWN_FUNC_CLIENT_CONNECTION_CHK  down_thread_client_connection_chk_event function 
			check_it = up_thread_message_down_thread_function_map.find(DOWN_FUNC_CLIENT_CONNECTION_CHK);
			BOOST_CHECK(check_it == up_thread_message_down_thread_function_map.end());
			
			// DOWN_FUNC_CLIENT_SEND  down_thread_client_send function 
			check_it = up_thread_message_down_thread_function_map.find(DOWN_FUNC_CLIENT_SEND);
			BOOST_CHECK(check_it == up_thread_message_down_thread_function_map.end());
			
			// DOWN_FUNC_CLIENT_RESPOND_SEND_EVENT  down_thread_client_respond_event function 
			check_it = up_thread_message_down_thread_function_map.find(DOWN_FUNC_CLIENT_RESPOND_SEND_EVENT);
			BOOST_CHECK(check_it != up_thread_message_down_thread_function_map.end());
			down_thread_client_respond_event_call_check = false;
			check_it->second(LOCAL_PROC);
			BOOST_CHECK(down_thread_client_respond_event_call_check);
			
			// DOWN_FUNC_REALSERVER_RECEIVE  down_thread_realserver_receive function 
			check_it = up_thread_message_down_thread_function_map.find(DOWN_FUNC_REALSERVER_RECEIVE);
			BOOST_CHECK(check_it == up_thread_message_down_thread_function_map.end());
			
			// DOWN_FUNC_REALSERVER_DISCONNECT  down_thread_realserver_disconnect function 
			check_it = up_thread_message_down_thread_function_map.find(DOWN_FUNC_REALSERVER_DISCONNECT);
			BOOST_CHECK(check_it == up_thread_message_down_thread_function_map.end());
			
			// DOWN_FUNC_REALSERVER_DISCONNECT_EVENT  down_thread_realserver_disconnect_event function 
			check_it = up_thread_message_down_thread_function_map.find(DOWN_FUNC_REALSERVER_DISCONNECT_EVENT);
			BOOST_CHECK(check_it != up_thread_message_down_thread_function_map.end());
			down_thread_realserver_disconnect_event_call_check = false;
			check_it->second(LOCAL_PROC);
			BOOST_CHECK(down_thread_realserver_disconnect_event_call_check);
			
			// DOWN_FUNC_REALSERVER_ALL_DISCONNECT  down_thread_all_realserver_disconnect function 
			check_it = up_thread_message_down_thread_function_map.find(DOWN_FUNC_REALSERVER_ALL_DISCONNECT);
			BOOST_CHECK(check_it == up_thread_message_down_thread_function_map.end());
			
			// DOWN_FUNC_SORRYSERVER_RECEIVE  down_thread_sorryserver_receive function 
			check_it = up_thread_message_down_thread_function_map.find(DOWN_FUNC_SORRYSERVER_RECEIVE);
			BOOST_CHECK(check_it == up_thread_message_down_thread_function_map.end());
			
			// DOWN_FUNC_SORRYSERVER_DISCONNECT  down_thread_sorryserver_disconnect function 
			check_it = up_thread_message_down_thread_function_map.find(DOWN_FUNC_SORRYSERVER_DISCONNECT);
			BOOST_CHECK(check_it == up_thread_message_down_thread_function_map.end());
			
			// DOWN_FUNC_SORRYSERVER_DISCONNECT_EVENT  down_thread_sorryserver_disconnect_event function 
			check_it = up_thread_message_down_thread_function_map.find(DOWN_FUNC_SORRYSERVER_DISCONNECT_EVENT);
			BOOST_CHECK(check_it != up_thread_message_down_thread_function_map.end());
			down_thread_sorryserver_disconnect_event_call_check = false;
			check_it->second(LOCAL_PROC);
			BOOST_CHECK(down_thread_sorryserver_disconnect_event_call_check);
			
			// DOWN_FUNC_SORRY_ENABLE_EVENT  down_thread_sorry_enable_event function 
			check_it = up_thread_message_down_thread_function_map.find(DOWN_FUNC_SORRY_ENABLE_EVENT);
			BOOST_CHECK(check_it == up_thread_message_down_thread_function_map.end());
			
			// DOWN_FUNC_SORRY_DISABLE_EVENT  down_thread_sorry_disable_event function 
			check_it = up_thread_message_down_thread_function_map.find(DOWN_FUNC_SORRY_DISABLE_EVENT);
			BOOST_CHECK(check_it == up_thread_message_down_thread_function_map.end());
			
			// DOWN_FUNC_EXIT  down_thread_exit function 
			check_it = up_thread_message_down_thread_function_map.find(DOWN_FUNC_EXIT);
			BOOST_CHECK(check_it == up_thread_message_down_thread_function_map.end());
		}
		
		void check_down_thread_message_up_thread_function_map(){
			std::map< UP_THREAD_FUNC_TYPE_TAG, tcp_session_func >::iterator check_it;
			
			// UP_FUNC_CLIENT_ACCEPT  up_thread_client_accept_event function 
			check_it = down_thread_message_up_thread_function_map.find(UP_FUNC_CLIENT_ACCEPT);
			BOOST_CHECK(check_it == down_thread_message_up_thread_function_map.end());
			
			// UP_FUNC_CLIENT_DISCONNECT  up_thread_client_disconnect function 
			check_it = down_thread_message_up_thread_function_map.find(UP_FUNC_CLIENT_DISCONNECT);
			BOOST_CHECK(check_it == down_thread_message_up_thread_function_map.end());
			
			// UP_FUNC_CLIENT_DISCONNECT_EVENT  up_thread_client_disconnect_event function 
			check_it = down_thread_message_up_thread_function_map.find(UP_FUNC_CLIENT_DISCONNECT_EVENT);
			BOOST_CHECK(check_it != down_thread_message_up_thread_function_map.end());
			up_thread_client_disconnect_event_call_check = false;
			check_it->second(LOCAL_PROC);
			BOOST_CHECK(up_thread_client_disconnect_event_call_check);
			
			// UP_FUNC_CLIENT_RECEIVE  up_thread_client_receive function 
			check_it = down_thread_message_up_thread_function_map.find(UP_FUNC_CLIENT_RECEIVE);
			BOOST_CHECK(check_it == down_thread_message_up_thread_function_map.end());
			
			// UP_FUNC_CLIENT_RESPOND_SEND  up_thread_client_respond function 
			check_it = down_thread_message_up_thread_function_map.find(UP_FUNC_CLIENT_RESPOND_SEND);
			BOOST_CHECK(check_it == down_thread_message_up_thread_function_map.end());
			
			// UP_FUNC_CLIENT_RESPOND_SEND_EVENT  up_thread_client_respond_event function 
			check_it = down_thread_message_up_thread_function_map.find(UP_FUNC_CLIENT_RESPOND_SEND_EVENT);
			BOOST_CHECK(check_it == down_thread_message_up_thread_function_map.end());
			
			// UP_FUNC_REALSERVER_GET_DEST_EVENT  up_thread_realserver_get_distination_event function 
			check_it = down_thread_message_up_thread_function_map.find(UP_FUNC_REALSERVER_GET_DEST_EVENT);
			BOOST_CHECK(check_it == down_thread_message_up_thread_function_map.end());
			
			// UP_FUNC_REALSERVER_CONNECT  up_thread_realserver_connect function 
			check_it = down_thread_message_up_thread_function_map.find(UP_FUNC_REALSERVER_CONNECT);
			BOOST_CHECK(check_it == down_thread_message_up_thread_function_map.end());
			
			// UP_FUNC_REALSERVER_CONNECT_EVENT  _event function 
			check_it = down_thread_message_up_thread_function_map.find(UP_FUNC_REALSERVER_CONNECT_EVENT);
			BOOST_CHECK(check_it == down_thread_message_up_thread_function_map.end());
			
			// UP_FUNC_REALSERVER_CONNECT_FAIL_EVENT  up_thread_realserver_connection_fail_event function 
			check_it = down_thread_message_up_thread_function_map.find(UP_FUNC_REALSERVER_CONNECT_FAIL_EVENT);
			BOOST_CHECK(check_it == down_thread_message_up_thread_function_map.end());
			
			// UP_FUNC_REALSERVER_SEND  up_thread_realserver_send function 
			check_it = down_thread_message_up_thread_function_map.find(UP_FUNC_REALSERVER_SEND);
			BOOST_CHECK(check_it == down_thread_message_up_thread_function_map.end());
			
			// UP_FUNC_REALSERVER_DISCONNECT  up_thread_realserver_disconnect function 
			check_it = down_thread_message_up_thread_function_map.find(UP_FUNC_REALSERVER_DISCONNECT);
			BOOST_CHECK(check_it == down_thread_message_up_thread_function_map.end());
			
			// UP_FUNC_REALSERVER_DISCONNECT_EVENT  up_thread_realserver_disconnect_event function 
			check_it = down_thread_message_up_thread_function_map.find(UP_FUNC_REALSERVER_DISCONNECT_EVENT);
			BOOST_CHECK(check_it != down_thread_message_up_thread_function_map.end());
			up_thread_realserver_disconnect_event_call_check = false;
			check_it->second(LOCAL_PROC);
			BOOST_CHECK(up_thread_realserver_disconnect_event_call_check);
			
			// UP_FUNC_REALSERVER_ALL_DISCONNECT  up_thread_all_realserver_disconnect function 
			check_it = down_thread_message_up_thread_function_map.find(UP_FUNC_REALSERVER_ALL_DISCONNECT);
			BOOST_CHECK(check_it == down_thread_message_up_thread_function_map.end());
			
			// UP_FUNC_SORRYSERVER_GET_DEST  up_thread_sorryserver_get_destination_event function 
			check_it = down_thread_message_up_thread_function_map.find(UP_FUNC_SORRYSERVER_GET_DEST);
			BOOST_CHECK(check_it == down_thread_message_up_thread_function_map.end());
			
			// UP_FUNC_SORRYSERVER_CONNECT  up_thread_sorryserver_connect function 
			check_it = down_thread_message_up_thread_function_map.find(UP_FUNC_SORRYSERVER_CONNECT);
			BOOST_CHECK(check_it == down_thread_message_up_thread_function_map.end());
			
			// UP_FUNC_SORRYSERVER_CONNECT_EVENT  up_thread_sorryserver_connect_event function 
			check_it = down_thread_message_up_thread_function_map.find(UP_FUNC_SORRYSERVER_CONNECT_EVENT);
			BOOST_CHECK(check_it == down_thread_message_up_thread_function_map.end());
			
			// UP_FUNC_SORRYSERVER_CONNECT_FAIL_EVENT  up_thread_sorryserver_connection_fail_event function 
			check_it = down_thread_message_up_thread_function_map.find(UP_FUNC_SORRYSERVER_CONNECT_FAIL_EVENT);
			BOOST_CHECK(check_it == down_thread_message_up_thread_function_map.end());
			
			// UP_FUNC_SORRYSERVER_SEND  up_thread_sorryserver_send function 
			check_it = down_thread_message_up_thread_function_map.find(UP_FUNC_SORRYSERVER_SEND);
			BOOST_CHECK(check_it == down_thread_message_up_thread_function_map.end());
			
			// UP_FUNC_SORRYSERVER_DISCONNECT  up_thread_sorryserver_disconnect function 
			check_it = down_thread_message_up_thread_function_map.find(UP_FUNC_SORRYSERVER_DISCONNECT);
			BOOST_CHECK(check_it == down_thread_message_up_thread_function_map.end());
			
			// UP_FUNC_SORRYSERVER_DISCONNECT_EVENT  up_thread_sorryserver_disconnect_event function 
			check_it = down_thread_message_up_thread_function_map.find(UP_FUNC_SORRYSERVER_DISCONNECT_EVENT);
			BOOST_CHECK(check_it != down_thread_message_up_thread_function_map.end());
			up_thread_sorryserver_disconnect_event_call_check = false;
			check_it->second(LOCAL_PROC);
			BOOST_CHECK(up_thread_sorryserver_disconnect_event_call_check);
			
			// UP_FUNC_SORRY_ENABLE_EVENT  up_thread_sorry_enable_event function 
			check_it = down_thread_message_up_thread_function_map.find(UP_FUNC_SORRY_ENABLE_EVENT);
			BOOST_CHECK(check_it == down_thread_message_up_thread_function_map.end());
			
			// UP_FUNC_SORRY_DISABLE_EVENT  up_thread_sorry_disable_event function 
			check_it = down_thread_message_up_thread_function_map.find(UP_FUNC_SORRY_DISABLE_EVENT);
			BOOST_CHECK(check_it == down_thread_message_up_thread_function_map.end());
			
			// UP_FUNC_EXIT  up_thread_exit function 
			check_it = down_thread_message_up_thread_function_map.find(UP_FUNC_EXIT);
			BOOST_CHECK(check_it == down_thread_message_up_thread_function_map.end());
		}
		
		void check_virtual_service_message_up_thread_function_map(){
			std::map< TCP_VIRTUAL_SERVICE_MESSAGE_TAG, tcp_session_func >::iterator check_it;
			// SORRY_STATE_ENABLE  up_thread_sorry_enable_event function 
			check_it = virtual_service_message_up_thread_function_map.find(SORRY_STATE_ENABLE);
			BOOST_CHECK(check_it != virtual_service_message_up_thread_function_map.end());
			up_thread_sorry_enable_event_call_check = false;
			check_it->second(LOCAL_PROC);
			BOOST_CHECK(up_thread_sorry_enable_event_call_check);
			
			// SORRY_STATE_DISABLE  up_thread_sorry_disable_event function 
			check_it = virtual_service_message_up_thread_function_map.find(SORRY_STATE_DISABLE);
			BOOST_CHECK(check_it != virtual_service_message_up_thread_function_map.end());
			up_thread_sorry_disable_event_call_check = false;
			check_it->second(LOCAL_PROC);
			BOOST_CHECK(up_thread_sorry_disable_event_call_check);
			
			// SESSION_END  up_thread_exit function 
			check_it = virtual_service_message_up_thread_function_map.find(SESSION_END);
			BOOST_CHECK(check_it != virtual_service_message_up_thread_function_map.end());
			up_thread_exit_call_check = false;
			check_it->second(LOCAL_PROC);
			BOOST_CHECK(up_thread_exit_call_check);
			
			// SESSION_PAUSE_ON  non function 
			check_it = virtual_service_message_up_thread_function_map.find(SESSION_PAUSE_ON);
			BOOST_CHECK(check_it == virtual_service_message_up_thread_function_map.end());
			
			// SESSION_PAUSE_OFF  non function 
			check_it = virtual_service_message_up_thread_function_map.find(SESSION_PAUSE_OFF);
			BOOST_CHECK(check_it == virtual_service_message_up_thread_function_map.end());
		}
		
		void check_virtual_service_message_down_thread_function_map(){
			std::map< TCP_VIRTUAL_SERVICE_MESSAGE_TAG, tcp_session_func >::iterator check_it;
			// SORRY_STATE_ENABLE  down_thread_sorry_enable_event function 
			check_it = virtual_service_message_down_thread_function_map.find(SORRY_STATE_ENABLE);
			BOOST_CHECK(check_it != virtual_service_message_down_thread_function_map.end());
			down_thread_sorry_enable_event_call_check = false;
			check_it->second(LOCAL_PROC);
			BOOST_CHECK(down_thread_sorry_enable_event_call_check);
			
			// SORRY_STATE_DISABLE  down_thread_sorry_disable_event function 
			check_it = virtual_service_message_down_thread_function_map.find(SORRY_STATE_DISABLE);
			BOOST_CHECK(check_it != virtual_service_message_down_thread_function_map.end());
			down_thread_sorry_disable_event_call_check = false;
			check_it->second(LOCAL_PROC);
			BOOST_CHECK(down_thread_sorry_disable_event_call_check);
			
			// SESSION_END  down_thread_exit function 
			check_it = virtual_service_message_down_thread_function_map.find(SESSION_END);
			BOOST_CHECK(check_it != virtual_service_message_down_thread_function_map.end());
			down_thread_exit_call_check = false;
			check_it->second(LOCAL_PROC);
			BOOST_CHECK(down_thread_exit_call_check);
			
			// SESSION_PAUSE_ON  non function 
			check_it = virtual_service_message_down_thread_function_map.find(SESSION_PAUSE_ON);
			BOOST_CHECK(check_it == virtual_service_message_down_thread_function_map.end());
			
			// SESSION_PAUSE_OFF  non function 
			check_it = virtual_service_message_down_thread_function_map.find(SESSION_PAUSE_OFF);
			BOOST_CHECK(check_it == virtual_service_message_down_thread_function_map.end());
		}
};
void constructer_test(){
	BOOST_MESSAGE( "----- constructer test start -----" );
	
	boost::asio::io_service io;
	l7vs::virtualservice_tcp vs;
	
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
	
//	constructer_test_class test_obj(vs,io);
	constructer_test_class test_obj(vs,io,set_option);
	
	// unit_test [1] constructer initialize member check
	std::cout << "[1] constructer initialize member check" << std::endl;
	BOOST_CHECK_EQUAL(&io , &test_obj.get_io());
	BOOST_CHECK_EQUAL(&vs,&test_obj.get_parent_service());
	BOOST_CHECK(!test_obj.get_exit_flag());
	std::bitset<TCP_SESSION_THREAD_STATE_BIT> ref;
	ref.reset();
	BOOST_CHECK_EQUAL(ref,test_obj.get_thread_state());
	BOOST_CHECK_EQUAL((void*)NULL,test_obj.get_protocol_module());
	BOOST_CHECK(!test_obj.get_session_pause_flag());
	BOOST_CHECK_EQUAL(&io , &(test_obj.get_client_socket().get_socket().get_io_service()));
	BOOST_CHECK_EQUAL(&io , &(test_obj.get_sorry_socket()->get_socket().get_io_service()));
	// unit_test [2] constructer set up_thread_module_event_map check
	std::cout << "[2] constructer set up_thread_module_event_map check" << std::endl;
	test_obj.check_up_thread_module_event_map();
	// unit_test [3] constructer set down_thread_module_event_map check
	std::cout << "[3] constructer set down_thread_module_event_map check" << std::endl;
	test_obj.check_down_thread_module_event_map();
	// unit_test [4] constructer set up_thread_function_map check
	std::cout << "[4] constructer set up_thread_function_map check" << std::endl;
	test_obj.check_up_thread_function_map();
	// unit_test [5] constructer set down_thread_function_map check
	std::cout << "[5] constructer set down_thread_function_map check" << std::endl;
	test_obj.check_down_thread_function_map();
	// unit_test [6] constructer set up_thread_message_down_thread_function_map check
	std::cout << "[6] constructer set up_thread_message_down_thread_function_map check" << std::endl;
	test_obj.check_up_thread_message_down_thread_function_map();
	// unit_test [7] constructer set down_thread_message_up_thread_function_map check
	std::cout << "[7] constructer set down_thread_message_up_thread_function_map check" << std::endl;
	test_obj.check_down_thread_message_up_thread_function_map();
	// unit_test [8] constructer set virtual_service_message_up_thread_function_map check
	std::cout << "[8] constructer set virtual_service_message_up_thread_function_map check" << std::endl;
	test_obj.check_virtual_service_message_up_thread_function_map();
	// unit_test [9] constructer set virtual_service_message_down_thread_function_map check
	std::cout << "[9] constructer set virtual_service_message_down_thread_function_map check" << std::endl;
	test_obj.check_virtual_service_message_down_thread_function_map();
	
	// unit_test [10] constructer set socket option check
	std::cout << "[10] constructer set socket option check" << std::endl;
	//! TCP_NODELAY
	BOOST_CHECK_EQUAL(test_obj.get_socket_opt_info()->nodelay_opt , set_option.nodelay_opt);
	BOOST_CHECK_EQUAL(test_obj.get_socket_opt_info()->nodelay_val , set_option.nodelay_val);
	// unit_test [11] constructer set socket option check
	std::cout << "[11] constructer set socket option check" << std::endl;
	//! TCP_CORK
	BOOST_CHECK_EQUAL(test_obj.get_socket_opt_info()->cork_opt , set_option.cork_opt);
	BOOST_CHECK_EQUAL(test_obj.get_socket_opt_info()->cork_val , set_option.cork_val);
	// unit_test [12] constructer set socket option check
	std::cout << "[12] constructer set socket option check" << std::endl;
	//! TCP_QUICKACK
	BOOST_CHECK_EQUAL(test_obj.get_socket_opt_info()->quickack_opt , set_option.quickack_opt);
	BOOST_CHECK_EQUAL(test_obj.get_socket_opt_info()->quickack_val , set_option.quickack_val);
	
	// unit_test [13] constructer client socket set socket option check
	std::cout << "[13] constructer client socket set socket option check" << std::endl;
	//! TCP_NODELAY
	BOOST_CHECK_EQUAL(test_obj.get_client_socket().opt_info.nodelay_opt , set_option.nodelay_opt);
	BOOST_CHECK_EQUAL(test_obj.get_client_socket().opt_info.nodelay_val , set_option.nodelay_val);
	//! TCP_CORK
	BOOST_CHECK_EQUAL(test_obj.get_client_socket().opt_info.cork_opt , set_option.cork_opt);
	BOOST_CHECK_EQUAL(test_obj.get_client_socket().opt_info.cork_val , set_option.cork_val);
	//! TCP_QUICKACK
	BOOST_CHECK_EQUAL(test_obj.get_client_socket().opt_info.quickack_opt , set_option.quickack_opt);
	BOOST_CHECK_EQUAL(test_obj.get_client_socket().opt_info.quickack_val , set_option.quickack_val);
	
	// unit_test [14] constructer sorry socket set socket option check
	std::cout << "[14] constructer sorry socket set socket option check" << std::endl;
	//! TCP_NODELAY
	BOOST_CHECK_EQUAL(test_obj.get_sorry_socket()->opt_info.nodelay_opt , set_option.nodelay_opt);
	BOOST_CHECK_EQUAL(test_obj.get_sorry_socket()->opt_info.nodelay_val , set_option.nodelay_val);
	//! TCP_CORK
	BOOST_CHECK_EQUAL(test_obj.get_sorry_socket()->opt_info.cork_opt , set_option.cork_opt);
	BOOST_CHECK_EQUAL(test_obj.get_sorry_socket()->opt_info.cork_val , set_option.cork_val);
	//! TCP_QUICKACK
	BOOST_CHECK_EQUAL(test_obj.get_sorry_socket()->opt_info.quickack_opt , set_option.quickack_opt);
	BOOST_CHECK_EQUAL(test_obj.get_sorry_socket()->opt_info.quickack_val , set_option.quickack_val);
	
	
	BOOST_MESSAGE( "----- constructer test end -----" );
}

// initialize test
// initialize test class
class initialize_test_class : public l7vs::tcp_session{
	public:
		initialize_test_class(l7vs::virtualservice_tcp& vs,boost::asio::io_service& session_io) : l7vs::tcp_session(vs,session_io){};
		~initialize_test_class(){};
		bool& get_exit_flag(){
			return exit_flag;
		};
		boost::thread::id& get_up_thread_id(){
			return up_thread_id;
		};
		boost::thread::id& get_down_thread_id(){
			return down_thread_id;
		};
		std::bitset<TCP_SESSION_THREAD_STATE_BIT>& get_thread_state(){
			return thread_state;
		};
				
		l7vs::protocol_module_base* get_protocol_module(){
			return protocol_module;
		};
		bool& get_session_pause_flag(){
			return session_pause_flag;
		};
		l7vs::lockfree_queue<l7vs::tcp_thread_message>& get_up_thread_message_que(){
			return up_thread_message_que;
		};
		l7vs::lockfree_queue<l7vs::tcp_thread_message>& get_down_thread_message_que(){
			return down_thread_message_que;
		};

		int get_upstream_buffer_size(){
			return upstream_buffer_size;
		}

		int get_downstream_buffer_size(){
			return downstream_buffer_size;
		}

};
void initialize_test(){
	BOOST_MESSAGE( "----- initialize test start -----" );
	
	boost::asio::io_service io;
	l7vs::virtualservice_tcp vs;
	initialize_test_class test_obj(vs,io);
	
	std::string test_protocol_name("test protocol");
	l7vs::test_protocol_module proto_test(test_protocol_name);
	vs.get_protocol_module_res = &proto_test;
	
	test_obj.get_exit_flag() = true;
	test_obj.get_up_thread_id() = boost::this_thread::get_id();
	test_obj.get_down_thread_id() = boost::this_thread::get_id();
	test_obj.get_thread_state().set();
	BOOST_CHECK(test_obj.get_protocol_module() != &proto_test);	
	test_obj.get_session_pause_flag() = true;
	l7vs::tcp_thread_message*	test_msg	= new l7vs::tcp_thread_message;
	test_obj.get_up_thread_message_que().push(test_msg);
	BOOST_CHECK(!test_obj.get_up_thread_message_que().empty());
	test_obj.get_down_thread_message_que().push(test_msg);
	BOOST_CHECK(!test_obj.get_down_thread_message_que().empty());
	
	l7vs::session_result_message res_msg = test_obj.initialize();
	
	// unit_test [1] initialize exit flag check
	std::cout << "[1] initialize exit flag check" << std::endl;
	BOOST_CHECK(!test_obj.get_exit_flag());
	
	// unit_test [2] initialize up thread id check
	std::cout << "[2] initialize up thread id check" << std::endl;
	BOOST_CHECK(test_obj.get_up_thread_id() == boost::thread::id());
	
	// unit_test [3] initialize down thread id check
	std::cout << "[3] initialize down thread id check" << std::endl;
	BOOST_CHECK(test_obj.get_down_thread_id() == boost::thread::id());
	
	// unit_test [4] initialize thread state check
	std::cout << "[4] initialize thread state check" << std::endl;
	BOOST_CHECK(test_obj.get_thread_state().none());
	
	// unit_test [5] initialize session pause flag check
	std::cout << "[5] initialize session pause flag check" << std::endl;
	BOOST_CHECK(!test_obj.get_session_pause_flag());
	
	// unit_test [6] initialize up thread message que check
	std::cout << "[6] initialize up thread message que check" << std::endl;
	BOOST_CHECK(test_obj.get_up_thread_message_que().empty());
	
	// unit_test [7] initialize down thread message que check
	std::cout << "[7] initialize down thread message que check" << std::endl;
	BOOST_CHECK(test_obj.get_down_thread_message_que().empty());
	
	// unit_test [8] initialize get protocol module pointer check
	std::cout << "[8] initialize get protocol module pointer check" << std::endl;
	BOOST_CHECK(test_obj.get_protocol_module() == &proto_test);
	
	// unit_test [9] initialize session_result_message flag check
	std::cout << "[9] initialize session_result_message flag check" << std::endl;
	BOOST_CHECK(!res_msg.flag);
	
	// unit_test [10] initialize upstream_buffer_size load cf check
	std::cout << "[10] initialize upstream_buffer_size load cf check" << std::endl;
	BOOST_CHECK_EQUAL(test_obj.get_upstream_buffer_size() , 7777);

	// unit_test [11] initialize downstream_buffer_size load cf check
	std::cout << "[11] initialize downstream_buffer_size load cf check" << std::endl;
	BOOST_CHECK_EQUAL(test_obj.get_downstream_buffer_size() , 8888);

	// unit_test [12] initialize protocol_module NULL error check
	std::cout << "[12] initialize protocol_module NULL error check" << std::endl;
	vs.get_protocol_module_res = NULL;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;

	while( !test_obj.get_up_thread_message_que().empty() ){
		test_msg	= test_obj.get_up_thread_message_que().pop();
		delete	test_msg;
	}

	res_msg = test_obj.initialize();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(5,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	BOOST_CHECK(res_msg.flag);
	std::cout << res_msg.message << std::endl;
	
	BOOST_MESSAGE( "----- initialize test end -----" );
}

// get_client_socket test
// get_client_socket test class
class get_client_socket_test_class : public l7vs::tcp_session{
	public:
		get_client_socket_test_class(l7vs::virtualservice_tcp& vs,boost::asio::io_service& session_io) : l7vs::tcp_session(vs,session_io){};
		~get_client_socket_test_class(){};
		l7vs::tcp_socket& get_client_tcp_socket(){
			return client_socket;
		};
};

void get_client_socket_test(){
	BOOST_MESSAGE( "----- get_client_socket test start -----" );
	
	boost::asio::io_service io;
	l7vs::virtualservice_tcp vs;
	get_client_socket_test_class test_obj(vs,io);
	
	
	l7vs::tcp_socket& ref_tcp_socket = test_obj.get_client_tcp_socket();
	boost::asio::ip::tcp::socket& ref_socket = ref_tcp_socket.my_socket;
	boost::asio::ip::tcp::socket& chk_socket = test_obj.get_client_socket();

	// unit_test [1] get_client_socket pointer check
	std::cout << "[1] get_client_socket pointer check" << std::endl;
	BOOST_CHECK_EQUAL(&ref_socket,&chk_socket);

	BOOST_MESSAGE( "----- get_client_socket test end -----" );
}


// is_thread_wait test
// is_thread_wait test class
class is_thread_wait_test_class : public l7vs::tcp_session{
	public:
		is_thread_wait_test_class(l7vs::virtualservice_tcp& vs,boost::asio::io_service& session_io) : l7vs::tcp_session(vs,session_io){};
		~is_thread_wait_test_class(){};
		
		std::bitset<TCP_SESSION_THREAD_STATE_BIT>& get_thread_state(){
			return thread_state;
		};
};
void is_thread_wait_test(){
	BOOST_MESSAGE( "----- is_thread_wait test start -----" );
	
	boost::asio::io_service io;
	l7vs::virtualservice_tcp vs;
	is_thread_wait_test_class test_obj(vs,io);
	
	std::bitset<TCP_SESSION_THREAD_STATE_BIT>& test_bitset = test_obj.get_thread_state();
	
	test_bitset.reset();
	BOOST_CHECK(test_bitset.none());
	BOOST_CHECK(!test_obj.is_thread_wait());
	
	// unit_test [1] is_thread_wait return check
	std::cout << "[1] is_thread_wait return check" << std::endl;
	
	// UP_THREAD_ALIVE
	test_bitset.reset();
	BOOST_CHECK(test_bitset.none());
	test_bitset |= std::bitset<TCP_SESSION_THREAD_STATE_BIT>(0x0001);
	BOOST_CHECK(!test_obj.is_thread_wait());
	
	// DOWN_THREAD_ALIVE
	test_bitset.reset();
	BOOST_CHECK(test_bitset.none());
	test_bitset |= std::bitset<TCP_SESSION_THREAD_STATE_BIT>(0x0002);
	BOOST_CHECK(!test_obj.is_thread_wait());
	
	// UP_THREAD_ACTIVE
	test_bitset.reset();
	BOOST_CHECK(test_bitset.none());
	test_bitset |= std::bitset<TCP_SESSION_THREAD_STATE_BIT>(0x0004);
	BOOST_CHECK(!test_obj.is_thread_wait());
	
	// DOWN_THREAD_ACTIVE
	test_bitset.reset();
	BOOST_CHECK(test_bitset.none());
	test_bitset |= std::bitset<TCP_SESSION_THREAD_STATE_BIT>(0x0008);
	BOOST_CHECK(!test_obj.is_thread_wait());
	
	// UP_THREAD_LOCK
	test_bitset.reset();
	BOOST_CHECK(test_bitset.none());
	test_bitset |= std::bitset<TCP_SESSION_THREAD_STATE_BIT>(0x0010);
	BOOST_CHECK(!test_obj.is_thread_wait());
	
	// DOWN_THREAD_LOCK
	test_bitset.reset();
	BOOST_CHECK(test_bitset.none());
	test_bitset |= std::bitset<TCP_SESSION_THREAD_STATE_BIT>(0x0020);
	BOOST_CHECK(!test_obj.is_thread_wait());
	
	// UP_THREAD_LOCK & DOWN_THREAD_LOCK
	test_bitset.reset();
	BOOST_CHECK(test_bitset.none());
	test_bitset |= std::bitset<TCP_SESSION_THREAD_STATE_BIT>(0x0010);
	test_bitset |= std::bitset<TCP_SESSION_THREAD_STATE_BIT>(0x0020);
	BOOST_CHECK(test_obj.is_thread_wait());
	
	// UP_THREAD_ALIVE DOWN_THREAD_ALIVE UP_THREAD_ACTIVE DOWN_THREAD_ACTIVE
	test_bitset.reset();
	BOOST_CHECK(test_bitset.none());
	test_bitset |= std::bitset<TCP_SESSION_THREAD_STATE_BIT>(0x0001);
	test_bitset |= std::bitset<TCP_SESSION_THREAD_STATE_BIT>(0x0002);
	test_bitset |= std::bitset<TCP_SESSION_THREAD_STATE_BIT>(0x0004);
	test_bitset |= std::bitset<TCP_SESSION_THREAD_STATE_BIT>(0x0008);
	BOOST_CHECK(!test_obj.is_thread_wait());
	
	// UP_THREAD_ALIVE DOWN_THREAD_ALIVE UP_THREAD_ACTIVE DOWN_THREAD_ACTIVE UP_THREAD_LOCK
	test_bitset.reset();
	BOOST_CHECK(test_bitset.none());
	test_bitset |= std::bitset<TCP_SESSION_THREAD_STATE_BIT>(0x0001);
	test_bitset |= std::bitset<TCP_SESSION_THREAD_STATE_BIT>(0x0002);
	test_bitset |= std::bitset<TCP_SESSION_THREAD_STATE_BIT>(0x0004);
	test_bitset |= std::bitset<TCP_SESSION_THREAD_STATE_BIT>(0x0008);
	test_bitset |= std::bitset<TCP_SESSION_THREAD_STATE_BIT>(0x0010);
	BOOST_CHECK(!test_obj.is_thread_wait());
	
	// UP_THREAD_ALIVE DOWN_THREAD_ALIVE UP_THREAD_ACTIVE DOWN_THREAD_ACTIVE DOWN_THREAD_LOCK
	test_bitset.reset();
	BOOST_CHECK(test_bitset.none());
	test_bitset |= std::bitset<TCP_SESSION_THREAD_STATE_BIT>(0x0001);
	test_bitset |= std::bitset<TCP_SESSION_THREAD_STATE_BIT>(0x0002);
	test_bitset |= std::bitset<TCP_SESSION_THREAD_STATE_BIT>(0x0004);
	test_bitset |= std::bitset<TCP_SESSION_THREAD_STATE_BIT>(0x0008);
	test_bitset |= std::bitset<TCP_SESSION_THREAD_STATE_BIT>(0x0020);
	BOOST_CHECK(!test_obj.is_thread_wait());
	
	// UP_THREAD_ALIVE DOWN_THREAD_ALIVE UP_THREAD_ACTIVE DOWN_THREAD_ACTIVE UP_THREAD_LOCK DOWN_THREAD_LOCK
	test_bitset.reset();
	BOOST_CHECK(test_bitset.none());
	test_bitset |= std::bitset<TCP_SESSION_THREAD_STATE_BIT>(0x0001);
	test_bitset |= std::bitset<TCP_SESSION_THREAD_STATE_BIT>(0x0002);
	test_bitset |= std::bitset<TCP_SESSION_THREAD_STATE_BIT>(0x0004);
	test_bitset |= std::bitset<TCP_SESSION_THREAD_STATE_BIT>(0x0008);
	test_bitset |= std::bitset<TCP_SESSION_THREAD_STATE_BIT>(0x0010);
	test_bitset |= std::bitset<TCP_SESSION_THREAD_STATE_BIT>(0x0020);
	BOOST_CHECK(test_obj.is_thread_wait());
	
	mutex_lock_test test_lock_obj(vs,io);
	test_lock_obj.set_up_thread_exit_test();
	
	test_lock_obj.test_thread_wait.lock();
	boost::thread::id proc_id = boost::this_thread::get_id();
	test_lock_obj.befor_thread_id = proc_id;
	test_lock_obj.after_thread_id = proc_id;
	test_lock_obj.mutex_lock();
	
	boost::thread test_thread(boost::bind(&mutex_lock_test::test_run,&test_lock_obj));
	
	BOOST_CHECK(test_lock_obj.befor_thread_id == proc_id);
	BOOST_CHECK(test_lock_obj.after_thread_id == proc_id);
	
	boost::thread::id test_id = test_thread.get_id();
	
	BOOST_CHECK(test_id != proc_id);
	
	// test start
	test_lock_obj.test_thread_wait.unlock();
	sleep(1);
	
	// unit_test [2] is_thread_wait thread block test (mutex lock)
	std::cout << "[2] is_thread_wait thread block test (mutex lock)" << std::endl;
	BOOST_CHECK(test_lock_obj.befor_thread_id == test_id);
	BOOST_CHECK(test_lock_obj.after_thread_id == proc_id);
	
	test_lock_obj.mutex_unlock();
	sleep(1);
	
	// unit_test [3] is_thread_wait thread run test (mutex unlock)
	std::cout << "[3] is_thread_wait thread run test (mutex unlock)" << std::endl;
	BOOST_CHECK(test_lock_obj.befor_thread_id == test_id);
	BOOST_CHECK(test_lock_obj.after_thread_id == test_id);
	
	
/*	// unit_test [4] is_thread_wait thread run after mutex unlock test
	std::cout << "[4] is_thread_wait thread run after mutex unlock test" << std::endl;
	BOOST_CHECK(test_lock_obj.mutex_trylock());
	test_lock_obj.mutex_unlock();*/
	
	BOOST_MESSAGE( "----- is_thread_wait test end -----" );	
}

// set_virtual_service_message test
// set_virtual_service_message test class
class set_virtual_service_message_test_class : public l7vs::tcp_session{
	public:
		set_virtual_service_message_test_class(l7vs::virtualservice_tcp& vs,boost::asio::io_service& session_io) : l7vs::tcp_session(vs,session_io){};
		~set_virtual_service_message_test_class(){};
		
		bool& get_session_pause_flag(){
			return session_pause_flag;
		}
		
		l7vs::lockfree_queue<l7vs::tcp_thread_message>& get_up_thread_message_que(){
			return up_thread_message_que;
		};
		
		l7vs::lockfree_queue<l7vs::tcp_thread_message>& get_down_thread_message_que(){
			return down_thread_message_que;
		};
		
		std::map< TCP_VIRTUAL_SERVICE_MESSAGE_TAG, tcp_session_func>& get_virtual_service_message_up_thread_function_map(){
			return virtual_service_message_up_thread_function_map;
		};
		
		std::map< TCP_VIRTUAL_SERVICE_MESSAGE_TAG, tcp_session_func>& get_virtual_service_message_down_thread_function_map(){
			return virtual_service_message_down_thread_function_map;
		};
		
		void up_thread_sorry_enable_event(const TCP_PROCESS_TYPE_TAG process_type){
			up_thread_sorry_enable_event_call_check = true;
		};
		bool up_thread_sorry_enable_event_call_check;
		
		void up_thread_sorry_disable_event(const TCP_PROCESS_TYPE_TAG process_type){
			up_thread_sorry_disable_event_call_check = true;
		};
		bool up_thread_sorry_disable_event_call_check;
		
		void up_thread_exit(const TCP_PROCESS_TYPE_TAG process_type){
			up_thread_exit_call_check = true;
		};
		bool up_thread_exit_call_check;
		
		void down_thread_sorry_enable_event(const TCP_PROCESS_TYPE_TAG process_type){
			down_thread_sorry_enable_event_call_check = true;
		};
		bool down_thread_sorry_enable_event_call_check;
		
		void down_thread_sorry_disable_event(const TCP_PROCESS_TYPE_TAG process_type){
			down_thread_sorry_disable_event_call_check = true;
		};
		bool down_thread_sorry_disable_event_call_check;
		
		void down_thread_exit(const TCP_PROCESS_TYPE_TAG process_type){
			down_thread_exit_call_check = true;
		};
		bool down_thread_exit_call_check;
};
void set_virtual_service_message_test(){
	BOOST_MESSAGE( "----- set_virtual_service_message test start -----" );
	
	boost::asio::io_service io;
	l7vs::virtualservice_tcp vs;
	set_virtual_service_message_test_class test_obj(vs,io);
	
	bool& ref_pause_flag = test_obj.get_session_pause_flag();
	l7vs::lockfree_queue<l7vs::tcp_thread_message>&		ref_up_msg_que = test_obj.get_up_thread_message_que();
	l7vs::lockfree_queue<l7vs::tcp_thread_message>&		ref_dw_msg_que = test_obj.get_down_thread_message_que();
	l7vs::tcp_thread_message*	up_msg;
	l7vs::tcp_thread_message*	dw_msg;
	std::map< l7vs::tcp_session::TCP_VIRTUAL_SERVICE_MESSAGE_TAG, boost::function< void(l7vs::tcp_session::TCP_PROCESS_TYPE_TAG) > >& ref_vs_up_msg_map = test_obj.get_virtual_service_message_up_thread_function_map();
	std::map< l7vs::tcp_session::TCP_VIRTUAL_SERVICE_MESSAGE_TAG, boost::function< void(l7vs::tcp_session::TCP_PROCESS_TYPE_TAG) > >& ref_vs_dw_msg_map = test_obj.get_virtual_service_message_down_thread_function_map();
	
	// unit_test [1] set_virtual_service_message SORRY_STATE_ENABLE
	std::cout << "[1] set_virtual_service_message SORRY_STATE_ENABLE" << std::endl;
	
	while( !ref_up_msg_que.empty() ){
		up_msg	= ref_up_msg_que.pop();
		delete	up_msg;
	}
	BOOST_CHECK(ref_up_msg_que.empty());
	while( !ref_dw_msg_que.empty() ){
		dw_msg	=ref_dw_msg_que.pop();
		delete	dw_msg;
	}
	BOOST_CHECK(ref_dw_msg_que.empty());
	
	test_obj.set_virtual_service_message(l7vs::tcp_session::SORRY_STATE_ENABLE);
	
	BOOST_CHECK(!ref_up_msg_que.empty());
	up_msg = ref_up_msg_que.pop();
	BOOST_CHECK(ref_up_msg_que.empty());
	test_obj.up_thread_sorry_enable_event_call_check = false;
	up_msg->message(l7vs::tcp_session::LOCAL_PROC);
	BOOST_CHECK(test_obj.up_thread_sorry_enable_event_call_check);
	delete up_msg;
	
	BOOST_CHECK(!ref_dw_msg_que.empty());
	dw_msg = ref_dw_msg_que.pop();
	BOOST_CHECK(ref_dw_msg_que.empty());
	test_obj.down_thread_sorry_enable_event_call_check = false;
	dw_msg->message(l7vs::tcp_session::LOCAL_PROC);
	BOOST_CHECK(test_obj.down_thread_sorry_enable_event_call_check);
	delete dw_msg;

	// unit_test [2] set_virtual_service_message SORRY_STATE_DISABLE
	std::cout << "[2] set_virtual_service_message SORRY_STATE_DISABLE" << std::endl;
	
	while( !ref_up_msg_que.empty() ){
		up_msg	= ref_up_msg_que.pop();
		delete	up_msg;
	}
	BOOST_CHECK(ref_up_msg_que.empty());
	while( !ref_dw_msg_que.empty() ){
		dw_msg	=ref_dw_msg_que.pop();
		delete	dw_msg;
	}
	BOOST_CHECK(ref_dw_msg_que.empty());
	
	test_obj.set_virtual_service_message(l7vs::tcp_session::SORRY_STATE_DISABLE);
	
	BOOST_CHECK(!ref_up_msg_que.empty());
	up_msg = ref_up_msg_que.pop();
	BOOST_CHECK(ref_up_msg_que.empty());
	test_obj.up_thread_sorry_disable_event_call_check = false;
	up_msg->message(l7vs::tcp_session::LOCAL_PROC);
	BOOST_CHECK(test_obj.up_thread_sorry_disable_event_call_check);
	delete	up_msg;

	BOOST_CHECK(!ref_dw_msg_que.empty());
	dw_msg = ref_dw_msg_que.pop();
	BOOST_CHECK(ref_dw_msg_que.empty());
	test_obj.down_thread_sorry_disable_event_call_check = false;
	dw_msg->message(l7vs::tcp_session::LOCAL_PROC);
	BOOST_CHECK(test_obj.down_thread_sorry_disable_event_call_check);
	delete	dw_msg;
	
	// unit_test [3] set_virtual_service_message SESSION_END
	std::cout << "[3] set_virtual_service_message SESSION_END" << std::endl;
	
	while( !ref_up_msg_que.empty() ){
		up_msg	= ref_up_msg_que.pop();
		delete	up_msg;
	}
	BOOST_CHECK(ref_up_msg_que.empty());
	while( !ref_dw_msg_que.empty() ){
		dw_msg	=ref_dw_msg_que.pop();
		delete	dw_msg;
	}
	BOOST_CHECK(ref_dw_msg_que.empty());
	
	test_obj.set_virtual_service_message(l7vs::tcp_session::SESSION_END);
	
	BOOST_CHECK(!ref_up_msg_que.empty());
	up_msg = ref_up_msg_que.pop();
	BOOST_CHECK(ref_up_msg_que.empty());
	test_obj.up_thread_exit_call_check = false;
	up_msg->message(l7vs::tcp_session::LOCAL_PROC);
	BOOST_CHECK(test_obj.up_thread_exit_call_check);
	delete	up_msg;

	BOOST_CHECK(!ref_dw_msg_que.empty());
	dw_msg = ref_dw_msg_que.pop();
	BOOST_CHECK(ref_dw_msg_que.empty());
	test_obj.down_thread_exit_call_check = false;
	dw_msg->message(l7vs::tcp_session::LOCAL_PROC);
	BOOST_CHECK(test_obj.down_thread_exit_call_check);
	delete	dw_msg;

	// unit_test [4] set_virtual_service_message SESSION_PAUSE_ON
	std::cout << "[4] set_virtual_service_message SESSION_PAUSE_ON" << std::endl;
	ref_pause_flag = false;
	test_obj.set_virtual_service_message(l7vs::tcp_session::SESSION_PAUSE_ON);
	BOOST_CHECK(ref_pause_flag);
	
	// unit_test [5] set_virtual_service_message SESSION_PAUSE_OFF
	std::cout << "[5] set_virtual_service_message SESSION_PAUSE_OFF" << std::endl;
	ref_pause_flag = true;
	test_obj.set_virtual_service_message(l7vs::tcp_session::SESSION_PAUSE_OFF);
	BOOST_CHECK(!ref_pause_flag);
	
	// unit_test [6] set_virtual_service_message up thread map find not message error
	std::cout << "[6] set_virtual_service_message up thread map find not message error" << std::endl;
	ref_vs_up_msg_map.clear();
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	while( !ref_up_msg_que.empty() ){
		up_msg	= ref_up_msg_que.pop();
		delete	up_msg;
	}
	BOOST_CHECK(ref_up_msg_que.empty());
	while( !ref_dw_msg_que.empty() ){
		dw_msg	=ref_dw_msg_que.pop();
		delete	dw_msg;
	}
	BOOST_CHECK(ref_dw_msg_que.empty());
	
	test_obj.set_virtual_service_message(l7vs::tcp_session::SORRY_STATE_ENABLE);
	
	BOOST_CHECK(ref_up_msg_que.empty());
	BOOST_CHECK(!ref_dw_msg_que.empty());
	
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(6,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	
	// unit_test [7] set_virtual_service_message up thread map find not message error
	std::cout << "[7] set_virtual_service_message up thread map find not message error" << std::endl;
	ref_vs_dw_msg_map.clear();
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	ref_vs_dw_msg_map.clear();
	BOOST_CHECK(ref_up_msg_que.empty());
	while( !ref_dw_msg_que.empty() ){
		dw_msg	=ref_dw_msg_que.pop();
		delete	dw_msg;
	}
	BOOST_CHECK(ref_dw_msg_que.empty());
	
	test_obj.set_virtual_service_message(l7vs::tcp_session::SORRY_STATE_ENABLE);
	
	BOOST_CHECK(ref_up_msg_que.empty());
	BOOST_CHECK(ref_dw_msg_que.empty());
	
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(7,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	
	BOOST_MESSAGE( "----- set_virtual_service_message test end -----" );
}

// up_thread_run
// up_thread_run test class
class up_thread_run_test_class : public l7vs::tcp_session{
	public:
		up_thread_run_test_class(l7vs::virtualservice_tcp& vs,boost::asio::io_service& session_io) : l7vs::tcp_session(vs,session_io){
			test_end = false;
			test_wait = true;
		};
		~up_thread_run_test_class(){};
		bool& get_exit_flag(){
			return exit_flag;
		};
		
		bool& get_session_pause_flag(){
			return session_pause_flag;
		};
		
		std::bitset<TCP_SESSION_THREAD_STATE_BIT>& get_thread_state(){
			return thread_state;
		};
		
		boost::mutex test_thread_wait;
		void test_run(){
			boost::mutex::scoped_lock scope_lock(test_thread_wait);
			while(!test_end){
				std::cout << "up_thread_run test call" << std::endl;
				test_wait = true;
				up_thread_run();
				while(test_wait){};
			}
		};
		bool test_end;
		bool test_wait;
				
		void set_protocol_module(l7vs::protocol_module_base* set_proto){
			protocol_module = set_proto;
		};
		
		l7vs::tcp_socket& get_client_socket(){
			return client_socket;
		};
		
		boost::thread::id& get_up_thread_id(){
			return up_thread_id;
		};
		
		boost::thread::id& get_down_thread_id(){
			return down_thread_id;
		};
		
		void up_thread_exit(const TCP_PROCESS_TYPE_TAG process_type){
			up_thread_exit_process_type = process_type;
			l7vs::tcp_session::up_thread_exit(process_type);
			up_thread_exit_call_check = true;
		};
		TCP_PROCESS_TYPE_TAG up_thread_exit_process_type;
		bool up_thread_exit_call_check;
		
		void up_thread_all_socket_close(void){
			up_thread_all_socket_close_call_check = true;
		}
		bool up_thread_all_socket_close_call_check;
		
		void test_message_set(boost::asio::ip::tcp::endpoint set_endpoint){
			l7vs::tcp_thread_message*	chk_msg	= new l7vs::tcp_thread_message;
			up_thread_message_que.push(chk_msg);
			chk_msg->endpoint_info = set_endpoint;
			up_thread_function_pair func = up_thread_function_array[UP_FUNC_EXIT];
			chk_msg->message = func.second;
		};
		
		l7vs::tcp_data& get_up_thread_message_data(){
			return up_thread_message_data;
		}
		
		void clear_function_array(){
			for(int i = 0;i <= UP_FUNC_EXIT;i++){
				up_thread_function_array[i].second = NULL;
			}
		}
		void clear_event_map(){
			up_thread_module_event_map.clear();
		}
		
		void set_up_thread_next_call_function_client_disconnect(){
			up_thread_next_call_function = up_thread_function_array[UP_FUNC_CLIENT_DISCONNECT];
		}
		void set_up_thread_next_call_function_exit(){
			up_thread_next_call_function = up_thread_function_array[UP_FUNC_EXIT];
		}
		
};
void up_thread_run_test(){
	
	BOOST_MESSAGE( "----- up_thread_run test start -----" );
	
	boost::asio::io_service io;
	l7vs::virtualservice_tcp vs;
	boost::system::error_code ec;
	
	std::string test_protocol_name("test protocol");
	l7vs::test_protocol_module proto_test(test_protocol_name);

	
	up_thread_run_test_class test_obj(vs,io);
	test_obj.set_protocol_module((l7vs::protocol_module_base*)&proto_test);
	bool& exit_flag = test_obj.get_exit_flag();
	bool& session_pause_flag = test_obj.get_session_pause_flag();
	boost::thread::id& up_thread_id = test_obj.get_up_thread_id();
	boost::thread::id& down_thread_id = test_obj.get_down_thread_id();

	
	std::bitset<TCP_SESSION_THREAD_STATE_BIT>& thread_state = test_obj.get_thread_state();
	l7vs::tcp_socket& client_socket = test_obj.get_client_socket();
	
	test_mirror_server test_server;
	// accept req
	test_server.breq_acc_flag = true;
	// close wait req
	test_server.breq_close_wait_flag = true;
	// recv cont
	test_server.req_recv_cnt = 0;
	// test server start
	boost::thread server_thread(boost::bind(&test_mirror_server::run,&test_server));
	while( !test_server.brun_flag ){
		sleep(1);
	}
	
	boost::asio::ip::tcp::endpoint connect_end(boost::asio::ip::address::from_string(DUMMI_SERVER_IP), DUMMI_SERVER_PORT);
	client_socket.get_socket().connect(connect_end,ec);
	BOOST_CHECK(!ec);
	while(!test_server.bconnect_flag){
		sleep(1);
	}
		
	test_obj.test_thread_wait.lock();
	thread_state[0] = 0;	// UP_THREAD_ALIVE
	thread_state[1] = 0;	// DOWN_THREAD_ALIVE
	thread_state[2] = 0;	// UP_THREAD_ACTIVE
	thread_state[3] = 0;	// DOWN_THREAD_ACTIVE
	thread_state[4] = 0;	// UP_THREAD_LOCK
	thread_state[5] = 0;	// DOWN_THREAD_LOCK
	up_thread_id = boost::thread::id();
	boost::thread test_thread(boost::bind(&up_thread_run_test_class::test_run,&test_obj));
	sleep(1);
	boost::thread::id test_id = test_thread.get_id();
	boost::thread::id proc_id = boost::this_thread::get_id();
	
	BOOST_CHECK(!thread_state.test(0));	// UP_THREAD_ALIVE
	BOOST_CHECK(!thread_state.test(1)); // DOWN_THREAD_ALIVE
	BOOST_CHECK(!thread_state.test(2)); // UP_THREAD_ACTIVE
	BOOST_CHECK(!thread_state.test(3)); // DOWN_THREAD_ACTIVE
	BOOST_CHECK(!thread_state.test(4)); // UP_THREAD_LOCK
	BOOST_CHECK(!thread_state.test(5)); // DOWN_THREAD_LOCK
	BOOST_CHECK(up_thread_id != test_id);
	test_obj.test_thread_wait.unlock();
	sleep(1);
	
	// unit_test [1] up_thread_run thread id update check
	std::cout << "[1] up_thread_run thread id update check" << std::endl;
	BOOST_CHECK(up_thread_id == test_id);
	
	// unit_test [2] up_thread_run down thread wait check
	std::cout << "[2] up_thread_run down thread wait check" << std::endl;
	BOOST_CHECK(thread_state.test(0));	// UP_THREAD_ALIVE
	BOOST_CHECK(!thread_state.test(1)); // DOWN_THREAD_ALIVE
	BOOST_CHECK(!thread_state.test(2)); // UP_THREAD_ACTIVE
	BOOST_CHECK(!thread_state.test(3)); // DOWN_THREAD_ACTIVE
	BOOST_CHECK(!thread_state.test(4)); // UP_THREAD_LOCK
	BOOST_CHECK(!thread_state.test(5)); // DOWN_THREAD_LOCK
	
	proto_test.handle_session_initialize_res_tag = l7vs::protocol_module_base::FINALIZE;
	proto_test.handle_session_initialize_in_up_thread_id = boost::thread::id();
	proto_test.handle_session_initialize_in_down_thread_id = boost::thread::id();
	proto_test.handle_session_initialize_in_client_endpoint_tcp = boost::asio::ip::tcp::endpoint();
	proto_test.handle_session_initialize_in_client_endpoint_udp.address(boost::asio::ip::address::from_string("255.255.255.255"));
	proto_test.handle_session_initialize_in_client_endpoint_udp.port(65535);
	BOOST_CHECK(proto_test.handle_session_initialize_in_up_thread_id != test_id);
	BOOST_CHECK(proto_test.handle_session_initialize_in_down_thread_id != proc_id);
	BOOST_CHECK(proto_test.handle_session_initialize_in_client_endpoint_tcp != connect_end);
	BOOST_CHECK(proto_test.handle_session_initialize_in_client_endpoint_udp != boost::asio::ip::udp::endpoint());
	down_thread_id = proc_id;
	session_pause_flag = true;
	
	// DOWN_THREAD_ALIVE
	thread_state[1] = 1;
	sleep(1);
	
	// unit_test [3] up_thread_run handle_session_initialize call check
	std::cout << "[3] up_thread_run handle_session_initialize call check" << std::endl;
	BOOST_CHECK(proto_test.handle_session_initialize_in_up_thread_id == test_id);
	BOOST_CHECK(proto_test.handle_session_initialize_in_down_thread_id == proc_id);
	BOOST_CHECK(proto_test.handle_session_initialize_in_client_endpoint_tcp == connect_end);
	BOOST_CHECK(proto_test.handle_session_initialize_in_client_endpoint_udp == boost::asio::ip::udp::endpoint());
	
	// unit_test [4] up_thread_run state update(UP_THREAD_ACTIVE) check
	std::cout << "[4] up_thread_run state update(UP_THREAD_ACTIVE) check" << std::endl;
	BOOST_CHECK(thread_state.test(2)); // UP_THREAD_ACTIVE
	
	// unit_test [5] up_thread_run pause check
	std::cout << "[5] up_thread_run  pause check" << std::endl;
	BOOST_CHECK(thread_state.test(4)); // UP_THREAD_LOCK
	
	test_obj.up_thread_exit_process_type = l7vs::tcp_session::MESSAGE_PROC;
	test_obj.up_thread_exit_call_check = false;
	test_obj.up_thread_all_socket_close_call_check = false;
	
	session_pause_flag = false;
	sleep(1);
	
	// unit_test [6] up_thread_run restart check
	std::cout << "[6] up_thread_run  restart check" << std::endl;
	BOOST_CHECK(!thread_state.test(4)); // UP_THREAD_LOCK
	
	// unit_test [7] up_thread_run up_thread_next_call_function call (up_thread_exit) check
	std::cout << "[7] up_thread_run up_thread_next_call_function call (up_thread_exit) check" << std::endl;
	BOOST_CHECK(test_obj.up_thread_exit_call_check);
	BOOST_CHECK(test_obj.up_thread_exit_process_type == l7vs::tcp_session::LOCAL_PROC);
	
	// unit_test [8] up_thread_run main loop exit check
	std::cout << "[8] up_thread_run main loop exit check" << std::endl;
	BOOST_CHECK(exit_flag);
	BOOST_CHECK(!thread_state.test(2)); // UP_THREAD_ACTIVE
	
	
	// unit_test [9] up_thread_run up_thread_all_socket_close_call_check call check
	std::cout << "[9] up_thread_run up_thread_all_socket_close_call_check call check" << std::endl;
	BOOST_CHECK(test_obj.up_thread_all_socket_close_call_check);
	
	// unit_test [10] up_thread_run down thread end wait check
	std::cout << "[10] up_thread_run down thread wait check" << std::endl;
	BOOST_CHECK(thread_state.test(0));	// UP_THREAD_ALIVE
	BOOST_CHECK(thread_state.test(1)); // DOWN_THREAD_ALIVE
	BOOST_CHECK(!thread_state.test(2)); // UP_THREAD_ACTIVE
	BOOST_CHECK(!thread_state.test(3)); // DOWN_THREAD_ACTIVE
	BOOST_CHECK(!thread_state.test(4)); // UP_THREAD_LOCK
	BOOST_CHECK(!thread_state.test(5)); // DOWN_THREAD_LOCK
	
	
// 	proto_test.handle_session_finalize_in_up_thread_id = boost::thread::id();
// 	proto_test.handle_session_finalize_in_down_thread_id = boost::thread::id();
// 	BOOST_CHECK(proto_test.handle_session_finalize_in_up_thread_id != test_id);
// 	BOOST_CHECK(proto_test.handle_session_finalize_in_down_thread_id != proc_id);
// 	vs.release_session_ptr = NULL;
// 	BOOST_CHECK(vs.release_session_ptr != test_id);
	
	thread_state[1] = 0;
	sleep(1);
	
	// unit_test [11] up_thread_run handle_session_finalize call check
	std::cout << "[11] up_thread_run handle_session_finalize call check" << std::endl;
	BOOST_CHECK(proto_test.handle_session_finalize_in_up_thread_id == test_id);
	BOOST_CHECK(proto_test.handle_session_finalize_in_down_thread_id == proc_id);
	
	// unit_test [12] up_thread_run release_session_id call check
	std::cout << "[12] up_thread_run release_session_id call check" << std::endl;
// 	BOOST_CHECK(vs.release_session_id == test_id);
	
	// unit_test [13] up_thread_run state update(UP_THREAD_ACTIVE) check
	std::cout << "[13] up_thread_run state update(UP_THREAD_ACTIVE) check" << std::endl;
	BOOST_CHECK(!thread_state.test(0)); // UP_THREAD_ACTIVE
	
	// message call test
	exit_flag = false;
	session_pause_flag = false;
	l7vs::tcp_data& msg_data = test_obj.get_up_thread_message_data();
	test_obj.test_message_set(connect_end);
	thread_state[0] = 0;	// UP_THREAD_ALIVE
	thread_state[1] = 1;	// DOWN_THREAD_ALIVE
	thread_state[2] = 0;	// UP_THREAD_ACTIVE
	thread_state[3] = 0;	// DOWN_THREAD_ACTIVE
	thread_state[4] = 0;	// UP_THREAD_LOCK
	thread_state[5] = 0;	// DOWN_THREAD_LOCK
	msg_data.set_endpoint(boost::asio::ip::tcp::endpoint());
	test_obj.up_thread_exit_call_check = false;
	test_obj.up_thread_exit_process_type = l7vs::tcp_session::LOCAL_PROC;
	proto_test.handle_session_initialize_res_tag = l7vs::protocol_module_base::CLIENT_DISCONNECT;
	
	// test thread start
	test_obj.test_wait = false;
	sleep(1);
	BOOST_CHECK(thread_state.test(0));	// UP_THREAD_ALIVE
	BOOST_CHECK(thread_state.test(1)); // DOWN_THREAD_ALIVE
	BOOST_CHECK(!thread_state.test(2)); // UP_THREAD_ACTIVE
	BOOST_CHECK(!thread_state.test(3)); // DOWN_THREAD_ACTIVE
	BOOST_CHECK(!thread_state.test(4)); // UP_THREAD_LOCK
	BOOST_CHECK(!thread_state.test(5)); // DOWN_THREAD_LOCK
	
	// unit_test [14] up_thread_run message call check
	std::cout << "[14] up_thread_run message call check" << std::endl;
	BOOST_CHECK(test_obj.up_thread_exit_call_check);
	BOOST_CHECK(test_obj.up_thread_exit_process_type == l7vs::tcp_session::MESSAGE_PROC);
	
	proto_test.handle_session_initialize_res_tag = l7vs::protocol_module_base::FINALIZE;
	thread_state[1] = 0;
	sleep(1);
	
	// error test not find function map 
	test_obj.clear_function_array();
	exit_flag = false;
	session_pause_flag = false;
	thread_state[1] = 1;
	
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	
	test_obj.test_wait = false;
	sleep(1);
	
	// unit_test [15] up_thread_run not find function map error test
	std::cout << "[15] up_thread_run not find function map error test" << std::endl;
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(15,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;

	thread_state[1] = 0;
	sleep(1);
	
	//error test protocol_module returnd illegal EVENT_TAG
	test_obj.clear_event_map();
	exit_flag = false;
	session_pause_flag = false;
	thread_state[1] = 1;
	
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	
	test_obj.test_wait = false;
	sleep(1);
	
	// unit_test [16] up_thread_run protocol_module returnd illegal EVENT_TAG error test
	std::cout << "[16] up_thread_run protocol_module returnd illegal EVENT_TAG error test" << std::endl;
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(14,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	
	thread_state[1] = 0;
	sleep(1);
	
	
	// unit_test [17] up_thread_run set non blocking fail check
	std::cout << "[17] up_thread_run set non blocking fail check" << std::endl;
	exit_flag = false;
	session_pause_flag = false;
	thread_state[1] = 1;
	
	l7vs::tcp_socket::set_non_blocking_mode_res = false;
	l7vs::tcp_socket::set_non_blocking_mode_ec = boost::asio::error::bad_descriptor;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	
	test_obj.test_wait = false;
	sleep(1);
	
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(11,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	l7vs::tcp_socket::set_non_blocking_mode_res = true;
	l7vs::tcp_socket::set_non_blocking_mode_ec.clear();

	thread_state[1] = 0;
	sleep(1);
	
	//error test client endpoint get error 
	client_socket.get_socket().close(ec);
	exit_flag = false;
	session_pause_flag = false;
	thread_state[1] = 1;
	
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	
	test_obj.test_wait = false;
	sleep(1);
	
	// unit_test [18] up_thread_run client endpoint get error test
	std::cout << "[18] up_thread_run client endpoint get error test" << std::endl;
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(9,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	
	thread_state[1] = 0;
	sleep(1);
	
	//error test protocol module null error 
	test_obj.set_protocol_module(NULL);
	exit_flag = false;
	session_pause_flag = false;
	thread_state[1] = 1;
	
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	
	test_obj.test_wait = false;
	sleep(1);
	
	// unit_test [19] up_thread_run protocol module null error test
	std::cout << "[19] up_thread_run protocol module null error test" << std::endl;
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(8,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	
	thread_state[1] = 0;
	sleep(1);
	
	test_obj.test_end = true;
	test_obj.test_wait = false;
	std::cout << "test_thread.join wait" << std::endl;
	test_thread.join();
	std::cout << "test_thread.join ok" << std::endl;
	
	
	test_server.breq_close_wait_flag = false;	
	test_server.bstop_flag = true;
	std::cout << "server_thread.join wait" << std::endl;
	server_thread.join();
	std::cout << "server_thread.join ok" << std::endl;
	
	
	BOOST_MESSAGE( "----- up_thread_run test end -----" );
	
}


// down_thread_run
// down_thread_run test class
class down_thread_run_test_class : public l7vs::tcp_session{
	public:
		down_thread_run_test_class(l7vs::virtualservice_tcp& vs,boost::asio::io_service& session_io) : l7vs::tcp_session(vs,session_io){
			test_end = false;
			test_wait = true;
		};
		~down_thread_run_test_class(){};
		bool& get_exit_flag(){
			return exit_flag;
		};
		
		bool& get_session_pause_flag(){
			return session_pause_flag;
		};
		
		std::bitset<TCP_SESSION_THREAD_STATE_BIT>& get_thread_state(){
			return thread_state;
		};
		
		boost::mutex test_thread_wait;
		void test_run(){
			boost::mutex::scoped_lock scope_lock(test_thread_wait);
			while(!test_end){
				test_wait = true;
				down_thread_run();
				while(test_wait){};
			}
		};
		bool test_end;
		bool test_wait;
				
		void set_protocol_module(l7vs::protocol_module_base* set_proto){
			protocol_module = set_proto;
		};
		
		boost::thread::id& get_down_thread_id(){
			return down_thread_id;
		};
		
		void down_thread_exit(const TCP_PROCESS_TYPE_TAG process_type){
			down_thread_exit_process_type = process_type;
			l7vs::tcp_session::down_thread_exit(process_type);
			down_thread_exit_call_check = true;
		};
		TCP_PROCESS_TYPE_TAG down_thread_exit_process_type;
		bool down_thread_exit_call_check;
		
		void down_thread_all_socket_close(void){
			down_thread_all_socket_close_call_check = true;
		}
		bool down_thread_all_socket_close_call_check;
		
		void test_message_set(boost::asio::ip::tcp::endpoint set_endpoint){
			l7vs::tcp_thread_message*	chk_msg		= new l7vs::tcp_thread_message;
			down_thread_message_que.push(chk_msg);
			chk_msg->endpoint_info = set_endpoint;
			down_thread_function_pair func = down_thread_function_array[DOWN_FUNC_EXIT];
			chk_msg->message = func.second;
		};
		
		l7vs::tcp_data& get_down_thread_message_data(){
			return down_thread_message_data;
		}
		
		void clear_function_array(){
			for(int i = 0;i < DOWN_FUNC_EXIT;i++){
				down_thread_function_array[i].second = NULL;	
			}
		}
		
		void down_thread_realserver_receive(const TCP_PROCESS_TYPE_TAG process_type){
			down_thread_realserver_receive_call_check = true;
			down_thread_realserver_receive_process_type = process_type;
			exit_flag = true;
		};
		bool down_thread_realserver_receive_call_check;
		TCP_PROCESS_TYPE_TAG down_thread_realserver_receive_process_type;
		
		void set_down_thread_connect_socket_list(){
			for(int i = 0 ; i < 1024;i++){
				std::pair<boost::asio::ip::tcp::endpoint,tcp_socket_ptr > push_element;
				boost::asio::ip::tcp::endpoint set_end(boost::asio::ip::address::from_string(DUMMI_SERVER_IP), i);
				push_element.first = set_end;
				down_thread_connect_socket_list.push_back(push_element);
				//std::cout << push_element.first << std::endl;
			}
			if(down_thread_connect_socket_list.empty()){
				std::cout << "down_thread_connect_socket_list.empty!" << std::endl;
			}
		};
		
		bool check_down_thread_receive_realserver_socket_list(){
			bool bres = true;
			std::list<socket_element>::iterator cur_it = down_thread_receive_realserver_socket_list.begin();
			for(int i = 0 ; i < 1024;i++){
				if(cur_it == down_thread_receive_realserver_socket_list.end()){
					std::cout << "DEBUG index[";
					std::cout << i;
					std::cout << "] not element" << std::endl;
					bres = false;
					break;
				}
				boost::asio::ip::tcp::endpoint set_end(boost::asio::ip::address::from_string(DUMMI_SERVER_IP), i);
				if(cur_it->first != set_end){
					int port = (int)cur_it->first.port();
					bres = false;
					std::cout << "DEBUG index[";
					std::cout << i;
					std::cout << "] port[";
					std::cout << port;
					std::cout << "]" << std::endl;
					break;
				}
				cur_it++;
			}
			return bres;
		};
};
void down_thread_run_test(){
	
	BOOST_MESSAGE( "----- down_thread_run test start -----" );
	
	boost::asio::io_service io;
	l7vs::virtualservice_tcp vs;
	boost::system::error_code ec;
	
	std::string test_protocol_name("test protocol");
	l7vs::test_protocol_module proto_test(test_protocol_name);

	
	down_thread_run_test_class test_obj(vs,io);	
	test_obj.set_protocol_module((l7vs::protocol_module_base*)&proto_test);
	bool& exit_flag = test_obj.get_exit_flag();
	bool& session_pause_flag = test_obj.get_session_pause_flag();
	boost::thread::id& down_thread_id = test_obj.get_down_thread_id();

	
	boost::asio::ip::tcp::endpoint connect_end(boost::asio::ip::address::from_string(DUMMI_SERVER_IP), DUMMI_SERVER_PORT);
	std::bitset<TCP_SESSION_THREAD_STATE_BIT>& thread_state = test_obj.get_thread_state();
		
	test_obj.test_thread_wait.lock();
	thread_state[0] = 1;	// UP_THREAD_ALIVE
	thread_state[1] = 0;	// DOWN_THREAD_ALIVE
	thread_state[2] = 0;	// UP_THREAD_ACTIVE
	thread_state[3] = 0;	// DOWN_THREAD_ACTIVE
	thread_state[4] = 0;	// UP_THREAD_LOCK
	thread_state[5] = 0;	// DOWN_THREAD_LOCK
	down_thread_id = boost::thread::id();
	boost::thread test_thread(boost::bind(&down_thread_run_test_class::test_run,&test_obj));
	sleep(1);
	boost::thread::id test_id = test_thread.get_id();
	boost::thread::id proc_id = boost::this_thread::get_id();
	
	BOOST_CHECK(thread_state.test(0));	// UP_THREAD_ALIVE
	BOOST_CHECK(!thread_state.test(1)); // DOWN_THREAD_ALIVE
	BOOST_CHECK(!thread_state.test(2)); // UP_THREAD_ACTIVE
	BOOST_CHECK(!thread_state.test(3)); // DOWN_THREAD_ACTIVE
	BOOST_CHECK(!thread_state.test(4)); // UP_THREAD_LOCK
	BOOST_CHECK(!thread_state.test(5)); // DOWN_THREAD_LOCK
	BOOST_CHECK(down_thread_id != test_id);
	test_obj.test_thread_wait.unlock();
	sleep(1);
	
	// unit_test [1] down_thread_run thread id update check
	std::cout << "[1] down_thread_run thread id update check" << std::endl;
	BOOST_CHECK(down_thread_id == test_id);
	
	// unit_test [2] down_thread_run up thread active wait check
	std::cout << "[2] down_thread_run up thread active wait check" << std::endl;
	BOOST_CHECK(thread_state.test(0));	// UP_THREAD_ALIVE
	BOOST_CHECK(thread_state.test(1)); // DOWN_THREAD_ALIVE
	BOOST_CHECK(!thread_state.test(2)); // UP_THREAD_ACTIVE
	BOOST_CHECK(!thread_state.test(3)); // DOWN_THREAD_ACTIVE
	BOOST_CHECK(!thread_state.test(4)); // UP_THREAD_LOCK
	BOOST_CHECK(!thread_state.test(5)); // DOWN_THREAD_LOCK
		
	session_pause_flag = true;
	
	// UP_THREAD_ACTIVE
	thread_state[2] = 1;
	sleep(1);
	
	// unit_test [3] down_thread_run state update(DOWN_THREAD_ACTIVE) check
	std::cout << "[3] down_thread_run state update(DOWN_THREAD_ACTIVE) check" << std::endl;
	BOOST_CHECK(thread_state.test(3)); // DOWN_THREAD_ACTIVE
	
	// unit_test [4] down_thread_run pause check
	std::cout << "[4] down_thread_run  pause check" << std::endl;
	BOOST_CHECK(thread_state.test(5)); // DOWN_THREAD_LOCK
	
	test_obj.down_thread_realserver_receive_call_check = false;
	test_obj.down_thread_realserver_receive_process_type = l7vs::tcp_session::MESSAGE_PROC;
	test_obj.down_thread_all_socket_close_call_check = false;
	
	test_obj.set_down_thread_connect_socket_list();
	
	session_pause_flag = false;
	sleep(1);
	
	// unit_test [5] down_thread_run restart check
	std::cout << "[5] down_thread_run  restart check" << std::endl;
	BOOST_CHECK(!thread_state.test(5)); // DOWN_THREAD_LOCK
	
	
	// unit_test [6] down_thread_run connect realserver set receive list check
	std::cout << "[6] down_thread_run connect realserver set receive list check" << std::endl;
	BOOST_CHECK(test_obj.check_down_thread_receive_realserver_socket_list());
	
	// unit_test [7] down_thread_run down_thread_next_call_function call (down_thread_realserver_receive) check
	std::cout << "[7] down_thread_run down_thread_next_call_function call (down_thread_realserver_receive) check" << std::endl;
	BOOST_CHECK(test_obj.down_thread_realserver_receive_call_check);
	BOOST_CHECK(test_obj.down_thread_realserver_receive_process_type == l7vs::tcp_session::LOCAL_PROC);
	
	// unit_test [8] down_thread_run main loop exit check
	std::cout << "[8] down_thread_run main loop exit check" << std::endl;
	BOOST_CHECK(exit_flag);
	BOOST_CHECK(!thread_state.test(3)); // DOWN_THREAD_ACTIVE
	
	// unit_test [9] down_thread_run down_thread_all_socket_close_call_check call check
	std::cout << "[9] down_thread_run down_thread_all_socket_close_call_check call check" << std::endl;
	BOOST_CHECK(test_obj.down_thread_all_socket_close_call_check);
	
	// unit_test [10] down_thread_run state update(DOWN_THREAD_ACTIVE) check
	std::cout << "[10] down_thread_run state update(DOWN_THREAD_ACTIVE) check" << std::endl;
	BOOST_CHECK(!thread_state.test(1)); // DOWN_THREAD_ACTIVE
	
	// message call test
	exit_flag = false;
	session_pause_flag = false;
	l7vs::tcp_data& msg_data = test_obj.get_down_thread_message_data();
	test_obj.test_message_set(connect_end);
	thread_state[0] = 1;	// UP_THREAD_ALIVE
	thread_state[2] = 1;	// UP_THREAD_ACTIVE
	msg_data.set_endpoint(boost::asio::ip::tcp::endpoint());
	test_obj.down_thread_exit_call_check = false;
	test_obj.down_thread_exit_process_type = l7vs::tcp_session::LOCAL_PROC;
	
	// test thread start
	test_obj.test_wait = false;
	sleep(1);
	
	// unit_test [11] down_thread_run message call check
	std::cout << "[11] down_thread_run message call check" << std::endl;
	BOOST_CHECK(test_obj.down_thread_exit_call_check);
	BOOST_CHECK(test_obj.down_thread_exit_process_type == l7vs::tcp_session::MESSAGE_PROC);
	
	// error test not find function map 
	test_obj.clear_function_array();
	exit_flag = false;
	session_pause_flag = true;
	thread_state[0] = 1;	// UP_THREAD_ALIVE
	thread_state[2] = 1;	// UP_THREAD_ACTIVE
	
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	
	test_obj.test_wait = false;
	sleep(1);
	
	// unit_test [12] down_thread_run not find function map error test
	std::cout << "[12] down_thread_run not find function map error test" << std::endl;
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(16,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	
	
	test_obj.test_end = true;
	test_obj.test_wait = false;
	test_thread.join();
	
	
	BOOST_MESSAGE( "----- down_thread_run test end -----" );
	
}

// thread_state_update test
// thread_state_update test class
class thread_state_update_test_class : public l7vs::tcp_session{
	public:
		thread_state_update_test_class(l7vs::virtualservice_tcp& vs,boost::asio::io_service& session_io) : l7vs::tcp_session(vs,session_io){};
		~thread_state_update_test_class(){};
		std::bitset<TCP_SESSION_THREAD_STATE_BIT>& get_thread_state(){
			return thread_state;
		};
		
		void test_call(const std::bitset<TCP_SESSION_THREAD_STATE_BIT> thread_flag,const bool regist){
			thread_state_update(thread_flag,regist);
		};
};
void thread_state_update_test(){
	
	BOOST_MESSAGE( "----- thread_state_update test start -----" );
	
	boost::asio::io_service io;
	l7vs::virtualservice_tcp vs;
	thread_state_update_test_class test_obj(vs,io);
	std::bitset<TCP_SESSION_THREAD_STATE_BIT>& thread_state = test_obj.get_thread_state();
	
	thread_state[0] = 0;	// UP_THREAD_ALIVE
	thread_state[1] = 0;	// DOWN_THREAD_ALIVE
	thread_state[2] = 0;	// UP_THREAD_ACTIVE
	thread_state[3] = 0;	// DOWN_THREAD_ACTIVE
	thread_state[4] = 0;	// UP_THREAD_LOCK
	thread_state[5] = 0;	// DOWN_THREAD_LOCK
	BOOST_CHECK(!thread_state.test(0));	// UP_THREAD_ALIVE
	BOOST_CHECK(!thread_state.test(1)); // DOWN_THREAD_ALIVE
	BOOST_CHECK(!thread_state.test(2)); // UP_THREAD_ACTIVE
	BOOST_CHECK(!thread_state.test(3)); // DOWN_THREAD_ACTIVE
	BOOST_CHECK(!thread_state.test(4)); // UP_THREAD_LOCK
	BOOST_CHECK(!thread_state.test(5)); // DOWN_THREAD_LOCK
	
	
	// unit_test [1] thread_state_update set flag test
	std::cout << "[1] thread_state_update  set flag test" << std::endl;
	test_obj.test_call(UP_THREAD_ALIVE,true);
	BOOST_CHECK(thread_state.test(0));	// UP_THREAD_ALIVE
	BOOST_CHECK(!thread_state.test(1)); // DOWN_THREAD_ALIVE
	BOOST_CHECK(!thread_state.test(2)); // UP_THREAD_ACTIVE
	BOOST_CHECK(!thread_state.test(3)); // DOWN_THREAD_ACTIVE
	BOOST_CHECK(!thread_state.test(4)); // UP_THREAD_LOCK
	BOOST_CHECK(!thread_state.test(5)); // DOWN_THREAD_LOCK
	
	test_obj.test_call(DOWN_THREAD_ALIVE,true);
	BOOST_CHECK(thread_state.test(0));	// UP_THREAD_ALIVE
	BOOST_CHECK(thread_state.test(1)); // DOWN_THREAD_ALIVE
	BOOST_CHECK(!thread_state.test(2)); // UP_THREAD_ACTIVE
	BOOST_CHECK(!thread_state.test(3)); // DOWN_THREAD_ACTIVE
	BOOST_CHECK(!thread_state.test(4)); // UP_THREAD_LOCK
	BOOST_CHECK(!thread_state.test(5)); // DOWN_THREAD_LOCK
	
	test_obj.test_call(UP_THREAD_ACTIVE,true);
	BOOST_CHECK(thread_state.test(0));	// UP_THREAD_ALIVE
	BOOST_CHECK(thread_state.test(1)); // DOWN_THREAD_ALIVE
	BOOST_CHECK(thread_state.test(2)); // UP_THREAD_ACTIVE
	BOOST_CHECK(!thread_state.test(3)); // DOWN_THREAD_ACTIVE
	BOOST_CHECK(!thread_state.test(4)); // UP_THREAD_LOCK
	BOOST_CHECK(!thread_state.test(5)); // DOWN_THREAD_LOCK
	
	test_obj.test_call(DOWN_THREAD_ACTIVE,true);
	BOOST_CHECK(thread_state.test(0));	// UP_THREAD_ALIVE
	BOOST_CHECK(thread_state.test(1)); // DOWN_THREAD_ALIVE
	BOOST_CHECK(thread_state.test(2)); // UP_THREAD_ACTIVE
	BOOST_CHECK(thread_state.test(3)); // DOWN_THREAD_ACTIVE
	BOOST_CHECK(!thread_state.test(4)); // UP_THREAD_LOCK
	BOOST_CHECK(!thread_state.test(5)); // DOWN_THREAD_LOCK
	
	test_obj.test_call(UP_THREAD_LOCK,true);
	BOOST_CHECK(thread_state.test(0));	// UP_THREAD_ALIVE
	BOOST_CHECK(thread_state.test(1)); // DOWN_THREAD_ALIVE
	BOOST_CHECK(thread_state.test(2)); // UP_THREAD_ACTIVE
	BOOST_CHECK(thread_state.test(3)); // DOWN_THREAD_ACTIVE
	BOOST_CHECK(thread_state.test(4)); // UP_THREAD_LOCK
	BOOST_CHECK(!thread_state.test(5)); // DOWN_THREAD_LOCK
	
	test_obj.test_call(DOWN_THREAD_LOCK,true);
	BOOST_CHECK(thread_state.test(0));	// UP_THREAD_ALIVE
	BOOST_CHECK(thread_state.test(1)); // DOWN_THREAD_ALIVE
	BOOST_CHECK(thread_state.test(2)); // UP_THREAD_ACTIVE
	BOOST_CHECK(thread_state.test(3)); // DOWN_THREAD_ACTIVE
	BOOST_CHECK(thread_state.test(4)); // UP_THREAD_LOCK
	BOOST_CHECK(thread_state.test(5)); // DOWN_THREAD_LOCK
	
	// unit_test [2] thread_state_update reset flag test
	std::cout << "[2] thread_state_update  reset flag test" << std::endl;
	test_obj.test_call(UP_THREAD_ALIVE,false);
	BOOST_CHECK(!thread_state.test(0));	// UP_THREAD_ALIVE
	BOOST_CHECK(thread_state.test(1)); // DOWN_THREAD_ALIVE
	BOOST_CHECK(thread_state.test(2)); // UP_THREAD_ACTIVE
	BOOST_CHECK(thread_state.test(3)); // DOWN_THREAD_ACTIVE
	BOOST_CHECK(thread_state.test(4)); // UP_THREAD_LOCK
	BOOST_CHECK(thread_state.test(5)); // DOWN_THREAD_LOCK
	
	test_obj.test_call(DOWN_THREAD_ALIVE,false);
	BOOST_CHECK(!thread_state.test(0));	// UP_THREAD_ALIVE
	BOOST_CHECK(!thread_state.test(1)); // DOWN_THREAD_ALIVE
	BOOST_CHECK(thread_state.test(2)); // UP_THREAD_ACTIVE
	BOOST_CHECK(thread_state.test(3)); // DOWN_THREAD_ACTIVE
	BOOST_CHECK(thread_state.test(4)); // UP_THREAD_LOCK
	BOOST_CHECK(thread_state.test(5)); // DOWN_THREAD_LOCK
	
	test_obj.test_call(UP_THREAD_ACTIVE,false);
	BOOST_CHECK(!thread_state.test(0));	// UP_THREAD_ALIVE
	BOOST_CHECK(!thread_state.test(1)); // DOWN_THREAD_ALIVE
	BOOST_CHECK(!thread_state.test(2)); // UP_THREAD_ACTIVE
	BOOST_CHECK(thread_state.test(3)); // DOWN_THREAD_ACTIVE
	BOOST_CHECK(thread_state.test(4)); // UP_THREAD_LOCK
	BOOST_CHECK(thread_state.test(5)); // DOWN_THREAD_LOCK
	
	test_obj.test_call(DOWN_THREAD_ACTIVE,false);
	BOOST_CHECK(!thread_state.test(0));	// UP_THREAD_ALIVE
	BOOST_CHECK(!thread_state.test(1)); // DOWN_THREAD_ALIVE
	BOOST_CHECK(!thread_state.test(2)); // UP_THREAD_ACTIVE
	BOOST_CHECK(!thread_state.test(3)); // DOWN_THREAD_ACTIVE
	BOOST_CHECK(thread_state.test(4)); // UP_THREAD_LOCK
	BOOST_CHECK(thread_state.test(5)); // DOWN_THREAD_LOCK
	
	test_obj.test_call(UP_THREAD_LOCK,false);
	BOOST_CHECK(!thread_state.test(0));	// UP_THREAD_ALIVE
	BOOST_CHECK(!thread_state.test(1)); // DOWN_THREAD_ALIVE
	BOOST_CHECK(!thread_state.test(2)); // UP_THREAD_ACTIVE
	BOOST_CHECK(!thread_state.test(3)); // DOWN_THREAD_ACTIVE
	BOOST_CHECK(!thread_state.test(4)); // UP_THREAD_LOCK
	BOOST_CHECK(thread_state.test(5)); // DOWN_THREAD_LOCK
	
	test_obj.test_call(DOWN_THREAD_LOCK,false);
	BOOST_CHECK(!thread_state.test(0));	// UP_THREAD_ALIVE
	BOOST_CHECK(!thread_state.test(1)); // DOWN_THREAD_ALIVE
	BOOST_CHECK(!thread_state.test(2)); // UP_THREAD_ACTIVE
	BOOST_CHECK(!thread_state.test(3)); // DOWN_THREAD_ACTIVE
	BOOST_CHECK(!thread_state.test(4)); // UP_THREAD_LOCK
	BOOST_CHECK(!thread_state.test(5)); // DOWN_THREAD_LOCK
	
	boost::thread::id proc_id = boost::this_thread::get_id();
	mutex_lock_test test_lock_obj(vs,io);
	test_lock_obj.set_thread_state_update_test();
	
	test_lock_obj.test_thread_wait.lock();
	test_lock_obj.befor_thread_id = proc_id;
	test_lock_obj.after_thread_id = proc_id;
	test_lock_obj.mutex_lock();
	
	boost::thread test_thread(boost::bind(&mutex_lock_test::test_run,&test_lock_obj));
	
	BOOST_CHECK(test_lock_obj.befor_thread_id == proc_id);
	BOOST_CHECK(test_lock_obj.after_thread_id == proc_id);
	
	boost::thread::id test_id = test_thread.get_id();
	BOOST_CHECK(test_id != proc_id);
	
	// test start
	test_lock_obj.test_thread_wait.unlock();
	sleep(1);
	
	// unit_test [3] thread_state_update thread block test (mutex lock)
	std::cout << "[3] thread_state_update thread block test (mutex lock)" << std::endl;
	BOOST_CHECK(test_lock_obj.befor_thread_id == test_id);
	BOOST_CHECK(test_lock_obj.after_thread_id == proc_id);
	
	test_lock_obj.mutex_unlock();
	sleep(1);
	
	// unit_test [4] thread_state_update thread run test (mutex unlock)
	std::cout << "[4] thread_state_update thread run test (mutex unlock)" << std::endl;
	BOOST_CHECK(test_lock_obj.befor_thread_id == test_id);
	BOOST_CHECK(test_lock_obj.after_thread_id == test_id);
	
/*	// unit_test [5] thread_state_update thread run after mutex unlock test
	std::cout << "[5] thread_state_update thread run after mutex unlock test" << std::endl;
	BOOST_CHECK(test_lock_obj.mutex_trylock());
	test_lock_obj.mutex_unlock();*/
	
	BOOST_MESSAGE( "----- thread_state_update test end -----" );
	
}



// up_thread_exit test
// up_thread_exit test class
class up_thread_exit_test_class : public l7vs::tcp_session{
	public:
		up_thread_exit_test_class(l7vs::virtualservice_tcp& vs,boost::asio::io_service& session_io) : l7vs::tcp_session(vs,session_io){};
		~up_thread_exit_test_class(){};
		bool& get_exit_flag(){
			return exit_flag;
		};
		void test_call(){
			l7vs::tcp_session::up_thread_exit(LOCAL_PROC);
		};
};
void up_thread_exit_test(){
	
	BOOST_MESSAGE( "----- up_thread_exit test start -----" );
	
	boost::asio::io_service io;
	
	l7vs::virtualservice_tcp vs;
	
	// unit_test [1] up_thread_exit update exit_flag
	std::cout << "[1] up_thread_exit update exit_flag" << std::endl;
	
	up_thread_exit_test_class test_obj(vs,io);
	
	bool& ref_exit_flag = test_obj.get_exit_flag();
	
	ref_exit_flag = false;
	
	test_obj.test_call();
	
	BOOST_CHECK(ref_exit_flag);
	
	
	mutex_lock_test test_lock_obj(vs,io);
	test_lock_obj.set_up_thread_exit_test();
	
	
	test_lock_obj.test_thread_wait.lock();
	boost::thread::id proc_id = boost::this_thread::get_id();
	test_lock_obj.befor_thread_id = proc_id;
	test_lock_obj.after_thread_id = proc_id;
	test_lock_obj.mutex_lock();
	
	boost::thread test_thread(boost::bind(&mutex_lock_test::test_run,&test_lock_obj));
	
	BOOST_CHECK(test_lock_obj.befor_thread_id == proc_id);
	BOOST_CHECK(test_lock_obj.after_thread_id == proc_id);
	
	boost::thread::id test_id = test_thread.get_id();
	
	BOOST_CHECK(test_id != proc_id);
	
	// test start
	test_lock_obj.test_thread_wait.unlock();
	sleep(1);
	
	// unit_test [2] up_thread_exit thread block test (mutex lock)
	std::cout << "[2] up_thread_exit thread block test (mutex lock)" << std::endl;
	BOOST_CHECK(test_lock_obj.befor_thread_id == test_id);
	BOOST_CHECK(test_lock_obj.after_thread_id == proc_id);
	
	test_lock_obj.mutex_unlock();
	sleep(1);
	
	// unit_test [3] up_thread_exit thread run test (mutex unlock)
	std::cout << "[3] up_thread_exit thread run test (mutex unlock)" << std::endl;
	BOOST_CHECK(test_lock_obj.befor_thread_id == test_id);
	BOOST_CHECK(test_lock_obj.after_thread_id == test_id);
	
	
/*	// unit_test [4] up_thread_exit thread run after mutex unlock test
	std::cout << "[4] up_thread_exit thread run after mutex unlock test" << std::endl;
	BOOST_CHECK(test_lock_obj.mutex_trylock());
	test_lock_obj.mutex_unlock();*/
	
	BOOST_MESSAGE( "----- up_thread_exit test end -----" );
}

// down_thread_exit test
// down_thread_exit test class
class down_thread_exit_test_class : public l7vs::tcp_session{
	public:
		down_thread_exit_test_class(l7vs::virtualservice_tcp& vs,boost::asio::io_service& session_io) : l7vs::tcp_session(vs,session_io){};
		~down_thread_exit_test_class(){};
		bool& get_exit_flag(){
			return exit_flag;
		};
		void test_call(){
			l7vs::tcp_session::down_thread_exit(LOCAL_PROC);
		};
};
void down_thread_exit_test(){
	
	BOOST_MESSAGE( "----- down_thread_exit test start -----" );
	
	boost::asio::io_service io;
	
	l7vs::virtualservice_tcp vs;
	
	// unit_test [1] down_thread_exit update exit_flag
	std::cout << "[1] down_thread_exit update exit_flag" << std::endl;
	
	down_thread_exit_test_class test_obj(vs,io);
	
	bool& ref_exit_flag = test_obj.get_exit_flag();
	
	ref_exit_flag = false;
	
	test_obj.test_call();
	
	BOOST_CHECK(ref_exit_flag);
	
	
	mutex_lock_test test_lock_obj(vs,io);
	test_lock_obj.set_down_thread_exit_test();
	
	
	test_lock_obj.test_thread_wait.lock();
	boost::thread::id proc_id = boost::this_thread::get_id();
	test_lock_obj.befor_thread_id = proc_id;
	test_lock_obj.after_thread_id = proc_id;
	test_lock_obj.mutex_lock();
	
	boost::thread test_thread(boost::bind(&mutex_lock_test::test_run,&test_lock_obj));
	
	BOOST_CHECK(test_lock_obj.befor_thread_id == proc_id);
	BOOST_CHECK(test_lock_obj.after_thread_id == proc_id);
	
	boost::thread::id test_id = test_thread.get_id();
	
	BOOST_CHECK(test_id != proc_id);
	
	// test start
	test_lock_obj.test_thread_wait.unlock();
	sleep(1);
	
	// unit_test [2] down_thread_exit thread block test (mutex lock)
	std::cout << "[2] down_thread_exit thread block test (mutex lock)" << std::endl;
	BOOST_CHECK(test_lock_obj.befor_thread_id == test_id);
	BOOST_CHECK(test_lock_obj.after_thread_id == proc_id);
	
	test_lock_obj.mutex_unlock();
	sleep(1);
	
	// unit_test [3] down_thread_exit thread run test (mutex unlock)
	std::cout << "[3] down_thread_exit thread run test (mutex unlock)" << std::endl;
	BOOST_CHECK(test_lock_obj.befor_thread_id == test_id);
	BOOST_CHECK(test_lock_obj.after_thread_id == test_id);
	
	
/*	// unit_test [4] down_thread_exit thread run after mutex unlock test
	std::cout << "[4] down_thread_exit thread run after mutex unlock test" << std::endl;
	BOOST_CHECK(test_lock_obj.mutex_trylock());
	test_lock_obj.mutex_unlock();*/
	
	
		
	BOOST_MESSAGE( "----- down_thread_exit test end -----" );
}

// up_thread_client_disconnect_event test
// up_thread_client_disconnect_event test class
class up_thread_client_disconnect_event_test_class : public module_event_map_test_base_class{
	public:
		up_thread_client_disconnect_event_test_class(l7vs::virtualservice_tcp& vs,boost::asio::io_service& session_io) : module_event_map_test_base_class(vs,session_io){
		};
		
		~up_thread_client_disconnect_event_test_class(){};
		
		void test_call(){
			l7vs::tcp_session::up_thread_client_disconnect_event(LOCAL_PROC);
		};
};

void up_thread_client_disconnect_event_test(){
	
	BOOST_MESSAGE( "----- up_thread_client_disconnect_event test start -----" );
	
	boost::asio::io_service io;
	
	l7vs::virtualservice_tcp vs;	
	
	up_thread_client_disconnect_event_test_class test_obj(vs,io);
	
	std::string test_protocol_name("test protocol");
	l7vs::test_protocol_module proto_test(test_protocol_name);

	test_obj.set_protocol_module((l7vs::protocol_module_base*)&proto_test);
	
	l7vs::protocol_module_base::EVENT_TAG chek_event[13];
	chek_event[0] = l7vs::protocol_module_base::ACCEPT;
	chek_event[1] = l7vs::protocol_module_base::CLIENT_DISCONNECT;
	chek_event[2] = l7vs::protocol_module_base::CLIENT_RECV;
	chek_event[3] = l7vs::protocol_module_base::CLIENT_RESPONSE_SEND;
	chek_event[4] = l7vs::protocol_module_base::REALSERVER_SELECT;
	chek_event[5] = l7vs::protocol_module_base::REALSERVER_CONNECT;
	chek_event[6] = l7vs::protocol_module_base::REALSERVER_SEND;
	chek_event[7] = l7vs::protocol_module_base::REALSERVER_DISCONNECT;
	chek_event[8] = l7vs::protocol_module_base::SORRYSERVER_SELECT;
	chek_event[9] = l7vs::protocol_module_base::SORRYSERVER_CONNECT;
	chek_event[10] = l7vs::protocol_module_base::SORRYSERVER_SEND;
	chek_event[11] = l7vs::protocol_module_base::SORRYSERVER_DISCONNECT;
	chek_event[12] = l7vs::protocol_module_base::FINALIZE;
	
	// unit_test [1] up_thread_client_disconnect_event up_thread_next_call_function update check
	std::cout << "[1] up_thread_client_disconnect_event up_thread_next_call_function update check" << std::endl;
	for(int i = 0; i < 13;i++){
		proto_test.handle_client_disconnect_res_tag = chek_event[i];
		test_obj.test_call();
		BOOST_CHECK(test_obj.chk_up_thread_next_call_function(chek_event[i]));
	}
	
	// unit_test [2] up_thread_client_disconnect_event module parameter check thread id
	std::cout << "[2] up_thread_client_disconnect_event module parameter check thread id" << std::endl;
	boost::thread::id def_id;
	boost::thread::id proc_id = boost::this_thread::get_id();
	test_obj.set_up_thread_id(proc_id);
	proto_test.handle_client_disconnect_thread_id = def_id;	
	proto_test.handle_client_disconnect_res_tag = l7vs::protocol_module_base::FINALIZE;
	BOOST_CHECK(proto_test.handle_client_disconnect_thread_id != proc_id);
	test_obj.test_call();
	BOOST_CHECK(proto_test.handle_client_disconnect_thread_id == proc_id);
	
	// unit_test [3] up_thread_client_disconnect_event not fond function error check
	std::cout << "[3] up_thread_client_disconnect_event not fond function error check" << std::endl;
	test_obj.up_thread_function_array_clear();
	proto_test.handle_client_disconnect_res_tag = l7vs::protocol_module_base::FINALIZE;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(29,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	
	// unit_test [4] up_thread_client_disconnect_event returnd illegal EVENT_TAG error check
	std::cout << "[4] up_thread_client_disconnect_event returnd illegal EVENT_TAG error check" << std::endl;
	test_obj.up_thread_module_event_map_clear();
	proto_test.handle_client_disconnect_res_tag = l7vs::protocol_module_base::FINALIZE;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(28,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	
	mutex_lock_test test_lock_obj(vs,io);
	test_lock_obj.set_up_thread_client_disconnect_event_test();
	test_lock_obj.set_protocol_module((l7vs::protocol_module_base*)&proto_test);
	
	
	test_lock_obj.test_thread_wait.lock();
	test_lock_obj.befor_thread_id = proc_id;
	test_lock_obj.after_thread_id = proc_id;
	test_lock_obj.mutex_lock();
	
	boost::thread test_thread(boost::bind(&mutex_lock_test::test_run,&test_lock_obj));
	
	BOOST_CHECK(test_lock_obj.befor_thread_id == proc_id);
	BOOST_CHECK(test_lock_obj.after_thread_id == proc_id);
	
	boost::thread::id test_id = test_thread.get_id();
	
	BOOST_CHECK(test_id != proc_id);
	
	// test start
	test_lock_obj.test_thread_wait.unlock();
	sleep(1);
	
	// unit_test [5] up_thread_client_disconnect_event thread block test (mutex lock)
	std::cout << "[5] up_thread_client_disconnect_event thread block test (mutex lock)" << std::endl;
	BOOST_CHECK(test_lock_obj.befor_thread_id == test_id);
	BOOST_CHECK(test_lock_obj.after_thread_id == proc_id);
	
	test_lock_obj.mutex_unlock();
	sleep(1);
	
	// unit_test [6] up_thread_client_disconnect_event thread run test (mutex unlock)
	std::cout << "[6] up_thread_client_disconnect_event thread run test (mutex unlock)" << std::endl;
	BOOST_CHECK(test_lock_obj.befor_thread_id == test_id);
	BOOST_CHECK(test_lock_obj.after_thread_id == test_id);
	
	BOOST_MESSAGE( "----- up_thread_client_disconnect_event test end -----" );
	
}
// down_thread_client_disconnetc_event test
// dwon_thread_client_disconnetc_event test class
class down_thread_client_disconnect_event_test_class : public module_event_map_test_base_class{
	public:
		down_thread_client_disconnect_event_test_class(l7vs::virtualservice_tcp& vs,boost::asio::io_service& session_io) : module_event_map_test_base_class(vs,session_io){
		};
		
		~down_thread_client_disconnect_event_test_class(){};
		
		void test_call(){
			l7vs::tcp_session::down_thread_client_disconnect_event(LOCAL_PROC);
		};
};

void down_thread_client_disconnect_event_test(){
	
	BOOST_MESSAGE( "----- down_thread_client_disconnect_event test start -----" );
	
	boost::asio::io_service io;
	
	l7vs::virtualservice_tcp vs;	
	
	down_thread_client_disconnect_event_test_class test_obj(vs,io);
	
	std::string test_protocol_name("test protocol");
	l7vs::test_protocol_module proto_test(test_protocol_name);

	test_obj.set_protocol_module((l7vs::protocol_module_base*)&proto_test);
	
	l7vs::protocol_module_base::EVENT_TAG chek_event[7];
	chek_event[0] = l7vs::protocol_module_base::CLIENT_DISCONNECT;
	chek_event[1] = l7vs::protocol_module_base::CLIENT_CONNECTION_CHECK;
	chek_event[2] = l7vs::protocol_module_base::REALSERVER_RECV;
	chek_event[3] = l7vs::protocol_module_base::REALSERVER_DISCONNECT;
	chek_event[4] = l7vs::protocol_module_base::SORRYSERVER_RECV;
	chek_event[5] = l7vs::protocol_module_base::SORRYSERVER_DISCONNECT;
	chek_event[6] = l7vs::protocol_module_base::FINALIZE;
	
	// unit_test [1] down_thread_client_disconnect_event up_thread_next_call_function update check
	std::cout << "[1] down_thread_client_disconnect_event up_thread_next_call_function update check" << std::endl;
	for(int i = 0; i < 7;i++){
		proto_test.handle_client_disconnect_res_tag = chek_event[i];
		test_obj.test_call();
		BOOST_CHECK(test_obj.chk_down_thread_next_call_function(chek_event[i]));
	}
	
	// unit_test [2] down_thread_client_disconnect_event module parameter check thread id
	std::cout << "[2] down_thread_client_disconnect_event module parameter check thread id" << std::endl;
	boost::thread::id def_id;
	boost::thread::id proc_id = boost::this_thread::get_id();
	test_obj.set_down_thread_id(proc_id);
	proto_test.handle_client_disconnect_thread_id = def_id;	
	proto_test.handle_client_disconnect_res_tag = l7vs::protocol_module_base::FINALIZE;
	BOOST_CHECK(proto_test.handle_client_disconnect_thread_id != proc_id);
	test_obj.test_call();
	BOOST_CHECK(proto_test.handle_client_disconnect_thread_id == proc_id);
	
	// unit_test [3] down_thread_client_disconnect_event not fond function error check
	std::cout << "[3] down_thread_client_disconnect_event not fond function error check" << std::endl;
	test_obj.down_thread_function_array_clear();
	proto_test.handle_client_disconnect_res_tag = l7vs::protocol_module_base::FINALIZE;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(87,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	
	// unit_test [4] down_thread_client_disconnect_event returnd illegal EVENT_TAG error check
	std::cout << "[4] down_thread_client_disconnect_event returnd illegal EVENT_TAG error check" << std::endl;
	test_obj.down_thread_module_event_map_clear();
	proto_test.handle_client_disconnect_res_tag = l7vs::protocol_module_base::FINALIZE;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(86,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	
	mutex_lock_test test_lock_obj(vs,io);
	test_lock_obj.set_down_thread_client_disconnect_event_test();
	test_lock_obj.set_protocol_module((l7vs::protocol_module_base*)&proto_test);	
	
	test_lock_obj.test_thread_wait.lock();
	test_lock_obj.befor_thread_id = proc_id;
	test_lock_obj.after_thread_id = proc_id;
	test_lock_obj.mutex_lock();
	
	boost::thread test_thread(boost::bind(&mutex_lock_test::test_run,&test_lock_obj));
	
	BOOST_CHECK(test_lock_obj.befor_thread_id == proc_id);
	BOOST_CHECK(test_lock_obj.after_thread_id == proc_id);
	
	boost::thread::id test_id = test_thread.get_id();
	
	BOOST_CHECK(test_id != proc_id);
	
	// test start
	test_lock_obj.test_thread_wait.unlock();
	sleep(1);
	
	// unit_test [5] down_thread_client_disconnect_event thread block test (mutex lock)
	std::cout << "[5] down_thread_client_disconnect_event thread block test (mutex lock)" << std::endl;
	BOOST_CHECK(test_lock_obj.befor_thread_id == test_id);
	BOOST_CHECK(test_lock_obj.after_thread_id == proc_id);
	
	test_lock_obj.mutex_unlock();
	sleep(1);
	
	// unit_test [6] down_thread_client_disconnect_event thread run test (mutex unlock)
	std::cout << "[6] down_thread_client_disconnect_event thread run test (mutex unlock)" << std::endl;
	BOOST_CHECK(test_lock_obj.befor_thread_id == test_id);
	BOOST_CHECK(test_lock_obj.after_thread_id == test_id);
	
	BOOST_MESSAGE( "----- down_thread_client_disconnect_event test end -----" );
}

// up_thread_realserver_get_detination_event test
// up_thread_realserver_get_detination_event test class
class up_thread_realserver_get_detination_event_test_class : public l7vs::tcp_session{
	public:
		up_thread_realserver_get_detination_event_test_class(l7vs::virtualservice_tcp& vs,boost::asio::io_service& session_io) : l7vs::tcp_session(vs,session_io){
		};
		~up_thread_realserver_get_detination_event_test_class(){};
		
		void test_call(){
			tcp_session::up_thread_realserver_get_destination_event(LOCAL_PROC);
		};
		
		l7vs::tcp_data& get_up_thread_data_dest_side(){
			return up_thread_data_dest_side;
		};
		void next_call(){
			up_thread_next_call_function.second(LOCAL_PROC);
		}
		
		void up_thread_realserver_connect(const TCP_PROCESS_TYPE_TAG process_type){
			up_thread_realserver_connect_call_check = true;
			
		};
		bool up_thread_realserver_connect_call_check;
		
		void set_protocol_module(l7vs::protocol_module_base* set_proto){
			protocol_module = set_proto;
		};
		
		void set_up_thread_id(boost::thread::id set_id){
			up_thread_id = set_id;
		};
		
		void up_thread_function_array_clear(){
			for(int i = 0; i <= UP_FUNC_EXIT;i++){
				up_thread_function_array[i].second = NULL;
			}
		};
		void up_thread_module_event_map_clear(){
			up_thread_module_event_map.clear();
		};
};
void up_thread_realserver_get_detination_event_test(){
	BOOST_MESSAGE( "----- up_thread_realserver_get_detination_event test start -----" );
	
	boost::asio::io_service io;
	l7vs::virtualservice_tcp vs;
	up_thread_realserver_get_detination_event_test_class test_obj(vs,io);
	std::string test_protocol_name("test protocol");
	l7vs::test_protocol_module proto_test(test_protocol_name);
	test_obj.set_protocol_module((l7vs::protocol_module_base*)&proto_test);
	l7vs::tcp_data& up_thread_data_dest_side = test_obj.get_up_thread_data_dest_side();
	boost::thread::id proc_id = boost::this_thread::get_id();
	test_obj.set_up_thread_id(proc_id);
	
	proto_test.handle_realserver_select_tcp_res_tag = l7vs::protocol_module_base::REALSERVER_CONNECT;
	proto_test.handle_realserver_select_out_rs_endpoint = boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string("100.101.102.103"), 7777);
	up_thread_data_dest_side.initialize();
	BOOST_CHECK(proto_test.handle_realserver_select_in_thread_id != proc_id);
	
	test_obj.test_call();
	
	// unit_test [1] up_thread_realserver_get_detination_event handle_realserver_select call check
	std::cout << "[1] up_thread_realserver_get_detination_event handle_realserver_select call check" << std::endl;
	BOOST_CHECK(proto_test.handle_realserver_select_in_thread_id == proc_id);
	boost::asio::ip::tcp::endpoint get_endpoint = up_thread_data_dest_side.get_endpoint();
	BOOST_CHECK(get_endpoint == proto_test.handle_realserver_select_out_rs_endpoint);
	
	// unit_test [2] up_thread_realserver_get_detination_event up_thread_next_call_function update check
	std::cout << "[2] up_thread_realserver_get_detination_event up_thread_next_call_function update check" << std::endl;
	test_obj.up_thread_realserver_connect_call_check = false;
	test_obj.next_call();
	BOOST_CHECK(test_obj.up_thread_realserver_connect_call_check);
	
	// unit_test [3] up_thread_realserver_get_detination_event not fond function error check
	std::cout << "[3] up_thread_realserver_get_detination_event not fond function error check" << std::endl;
	test_obj.up_thread_function_array_clear();
	proto_test.handle_realserver_disconnect_res_tag = l7vs::protocol_module_base::FINALIZE;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(33,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	
	// unit_test [4] up_thread_realserver_get_detination_event returnd illegal EVENT_TAG error check
	std::cout << "[4] up_thread_realserver_get_detination_event returnd illegal EVENT_TAG error check" << std::endl;
	test_obj.up_thread_module_event_map_clear();
	proto_test.handle_realserver_disconnect_res_tag = l7vs::protocol_module_base::FINALIZE;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(32,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	
	BOOST_MESSAGE( "----- up_thread_realserver_get_detination_event test end -----" );
}
// up_thread_sorryserver_get_detination_event test
// up_thread_sorryserver_get_detination_event test class
class up_thread_sorryserver_get_detination_event_test_class : public l7vs::tcp_session{
	public:
		up_thread_sorryserver_get_detination_event_test_class(l7vs::virtualservice_tcp& vs,boost::asio::io_service& session_io) : l7vs::tcp_session(vs,session_io){
		};
		~up_thread_sorryserver_get_detination_event_test_class(){};
		
		void test_call(){
			tcp_session::up_thread_sorryserver_get_destination_event(LOCAL_PROC);
		};
		
		l7vs::tcp_data& get_up_thread_data_dest_side(){
			return up_thread_data_dest_side;
		};
		void next_call(){
			up_thread_next_call_function.second(LOCAL_PROC);
		}
		
		void up_thread_sorryserver_connect(const TCP_PROCESS_TYPE_TAG process_type){
			up_thread_sorryserver_connect_call_check = true;
			
		};
		bool up_thread_sorryserver_connect_call_check;
		
		void set_protocol_module(l7vs::protocol_module_base* set_proto){
			protocol_module = set_proto;
		};
		
		void set_up_thread_id(boost::thread::id set_id){
			up_thread_id = set_id;
		};
		
		void up_thread_function_array_clear(){
			for(int i = 0; i <= UP_FUNC_EXIT; i++){
				up_thread_function_array[i].second = NULL;
			}
		};
		void up_thread_module_event_map_clear(){
			up_thread_module_event_map.clear();
		};
};
void up_thread_sorryserver_get_detination_event_test(){
	BOOST_MESSAGE( "----- up_thread_sorryserver_get_detination_event test start -----" );
	
	boost::asio::io_service io;
	l7vs::virtualservice_tcp vs;
	up_thread_sorryserver_get_detination_event_test_class test_obj(vs,io);
	std::string test_protocol_name("test protocol");
	l7vs::test_protocol_module proto_test(test_protocol_name);
	test_obj.set_protocol_module((l7vs::protocol_module_base*)&proto_test);
	l7vs::tcp_data& up_thread_data_dest_side = test_obj.get_up_thread_data_dest_side();
	boost::thread::id proc_id = boost::this_thread::get_id();
	test_obj.set_up_thread_id(proc_id);
	
	proto_test.handle_sorryserver_select_res_tag = l7vs::protocol_module_base::SORRYSERVER_CONNECT;
	proto_test.handle_sorryserver_select_out_sorry_endpoint = boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string("100.101.102.103"), 7777);
	vs.my_element.sorry_endpoint = boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string("200.201.202.203"), 8888);

	up_thread_data_dest_side.initialize();
	BOOST_CHECK(proto_test.handle_sorryserver_select_in_thread_id != proc_id);
	
	test_obj.test_call();
	
	// unit_test [1] up_thread_sorryserver_get_detination_event handle_realserver_select call check
	std::cout << "[1] up_thread_sorryserver_get_detination_event handle_realserver_select call check" << std::endl;
	BOOST_CHECK(proto_test.handle_sorryserver_select_in_thread_id == proc_id);
	boost::asio::ip::tcp::endpoint get_endpoint = up_thread_data_dest_side.get_endpoint();
	BOOST_CHECK(get_endpoint == proto_test.handle_sorryserver_select_out_sorry_endpoint);
	BOOST_CHECK(vs.my_element.sorry_endpoint == proto_test.handle_sorryserver_select_in_sorry_endpoint);
	
	// unit_test [2] up_thread_sorryserver_get_detination_event up_thread_next_call_function update check
	std::cout << "[2] up_thread_sorryserver_get_detination_event up_thread_next_call_function update check" << std::endl;
	test_obj.up_thread_sorryserver_connect_call_check = false;
	test_obj.next_call();
	BOOST_CHECK(test_obj.up_thread_sorryserver_connect_call_check);
	
	// unit_test [3] up_thread_sorryserver_get_detination_event not fond function error check
	std::cout << "[3] up_thread_sorryserver_get_detination_event not fond function error check" << std::endl;
	test_obj.up_thread_function_array_clear();
	proto_test.handle_sorryserver_disconnect_res_tag = l7vs::protocol_module_base::FINALIZE;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(52,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	
	// unit_test [4] up_thread_sorryserver_get_detination_event returnd illegal EVENT_TAG error check
	std::cout << "[4] up_thread_sorryserver_get_detination_event returnd illegal EVENT_TAG error check" << std::endl;
	test_obj.up_thread_module_event_map_clear();
	proto_test.handle_sorryserver_disconnect_res_tag = l7vs::protocol_module_base::FINALIZE;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(51,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	
	BOOST_MESSAGE( "----- up_thread_sorryserver_get_detination_event test end -----" );
}


// up_thread_realserver_disconnect_event test
// up_thread_realserver_disconnect_event test class
class up_thread_realserver_disconnect_event_test_class : public module_event_map_test_base_class{
	public:
		up_thread_realserver_disconnect_event_test_class(l7vs::virtualservice_tcp& vs,boost::asio::io_service& session_io) : module_event_map_test_base_class(vs,session_io){
		};
		
		~up_thread_realserver_disconnect_event_test_class(){};
		
		void test_call(){
			l7vs::tcp_session::up_thread_realserver_disconnect_event(LOCAL_PROC);
		};
		
		std::map<endpoint,tcp_socket_ptr>& get_realserver_socket_map(){
			return up_thread_send_realserver_socket_map;
		};
};

void up_thread_realserver_disconnect_event_test(){
	
	BOOST_MESSAGE( "----- up_thread_realserver_disconnect_event test start -----" );
	
	boost::asio::io_service io;
	
	l7vs::virtualservice_tcp vs;	
	
	up_thread_realserver_disconnect_event_test_class test_obj(vs,io);
	
	std::string test_protocol_name("test protocol");
	l7vs::test_protocol_module proto_test(test_protocol_name);

	test_obj.set_protocol_module((l7vs::protocol_module_base*)&proto_test);
	
	l7vs::protocol_module_base::EVENT_TAG chek_event[13];
	chek_event[0] = l7vs::protocol_module_base::ACCEPT;
	chek_event[1] = l7vs::protocol_module_base::CLIENT_DISCONNECT;
	chek_event[2] = l7vs::protocol_module_base::CLIENT_RECV;
	chek_event[3] = l7vs::protocol_module_base::CLIENT_RESPONSE_SEND;
	chek_event[4] = l7vs::protocol_module_base::REALSERVER_SELECT;
	chek_event[5] = l7vs::protocol_module_base::REALSERVER_CONNECT;
	chek_event[6] = l7vs::protocol_module_base::REALSERVER_SEND;
	chek_event[7] = l7vs::protocol_module_base::REALSERVER_DISCONNECT;
	chek_event[8] = l7vs::protocol_module_base::SORRYSERVER_SELECT;
	chek_event[9] = l7vs::protocol_module_base::SORRYSERVER_CONNECT;
	chek_event[10] = l7vs::protocol_module_base::SORRYSERVER_SEND;
	chek_event[11] = l7vs::protocol_module_base::SORRYSERVER_DISCONNECT;
	chek_event[12] = l7vs::protocol_module_base::FINALIZE;
	
	// unit_test [1] up_thread_realserver_disconnect_event up_thread_next_call_function update check
	std::cout << "[1] up_thread_realserver_disconnect_event up_thread_next_call_function update check" << std::endl;
	for(int i = 0; i < 13;i++){
		proto_test.handle_realserver_disconnect_res_tag = chek_event[i];
		test_obj.test_call();
		BOOST_CHECK(test_obj.chk_up_thread_next_call_function(chek_event[i]));
	}
	
	// module parameter check
	boost::thread::id def_id;
	boost::thread::id proc_id = boost::this_thread::get_id();
	boost::asio::ip::tcp::endpoint test_end(boost::asio::ip::address::from_string("100.102.103.104"), 7777);
	l7vs::tcp_data test_message;
	test_message.set_endpoint(test_end);
	test_obj.set_up_thread_id(proc_id);
	test_obj.set_up_thread_message_data(test_message);
	proto_test.handle_realserver_disconnect_thread_id = def_id;
	proto_test.handle_realserver_disconnect_rs_endpoint = boost::asio::ip::tcp::endpoint();
	proto_test.handle_realserver_disconnect_res_tag = l7vs::protocol_module_base::FINALIZE;
	BOOST_CHECK(proto_test.handle_realserver_disconnect_thread_id != proc_id);
	BOOST_CHECK(proto_test.handle_realserver_disconnect_rs_endpoint != test_end);
	test_obj.test_call();
	// unit_test [2] up_thread_realserver_disconnect_event module parameter check thread id
	std::cout << "[2] up_thread_realserver_disconnect_event module parameter check thread id" << std::endl;
	BOOST_CHECK(proto_test.handle_realserver_disconnect_thread_id == proc_id);
	
	// unit_test [3] up_thread_realserver_disconnect_event module parameter check endpoint
	std::cout << "[3] up_thread_realserver_disconnect_event module parameter check endpoint" << std::endl;
	BOOST_CHECK(proto_test.handle_realserver_disconnect_rs_endpoint == test_end);
	
	
	// unit_test [4] up_thread_realserver_disconnect_event realserver map erase check
	std::cout << "[4] up_thread_realserver_disconnect_event  realserver map erase check" << std::endl;
	
	std::map<boost::asio::ip::tcp::endpoint, boost::shared_ptr< l7vs::tcp_socket > >& realserver_map = test_obj.get_realserver_socket_map();
	boost::shared_ptr< l7vs::tcp_socket > test_tcp_sock0(new l7vs::tcp_socket(io));
	boost::shared_ptr< l7vs::tcp_socket > test_tcp_sock1(new l7vs::tcp_socket(io));
	boost::shared_ptr< l7vs::tcp_socket > test_tcp_sock2(new l7vs::tcp_socket(io));
	std::pair< boost::asio::ip::tcp::endpoint , boost::shared_ptr< l7vs::tcp_socket > > test_sock_list[3];
	test_sock_list[0].first = boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string("100.100.100.100"), 0);
	test_sock_list[0].second = test_tcp_sock0;
	test_sock_list[1].first = boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string("101.101.101.101"), 1);
	test_sock_list[1].second = test_tcp_sock1;
	test_sock_list[2].first = boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string("102.102.102.102"), 2);
	test_sock_list[2].second = test_tcp_sock2;
	for(int i = 0;i < 3;i++){
		realserver_map.insert(test_sock_list[i]);
	}	
	proto_test.handle_realserver_disconnect_res_tag = l7vs::protocol_module_base::FINALIZE;
	
	//erase 1
	test_message.set_endpoint(test_sock_list[1].first);
	test_obj.set_up_thread_message_data(test_message);
	BOOST_CHECK(realserver_map.find(test_sock_list[0].first) != realserver_map.end());
	BOOST_CHECK(realserver_map.find(test_sock_list[1].first) != realserver_map.end());
	BOOST_CHECK(realserver_map.find(test_sock_list[2].first) != realserver_map.end());
	test_obj.test_call();
	BOOST_CHECK(realserver_map.find(test_sock_list[0].first) != realserver_map.end());
	BOOST_CHECK(realserver_map.find(test_sock_list[1].first) == realserver_map.end());
	BOOST_CHECK(realserver_map.find(test_sock_list[2].first) != realserver_map.end());
	//erase 0
	test_message.set_endpoint(test_sock_list[0].first);
	test_obj.set_up_thread_message_data(test_message);
	BOOST_CHECK(realserver_map.find(test_sock_list[0].first) != realserver_map.end());
	BOOST_CHECK(realserver_map.find(test_sock_list[1].first) == realserver_map.end());
	BOOST_CHECK(realserver_map.find(test_sock_list[2].first) != realserver_map.end());
	test_obj.test_call();
	BOOST_CHECK(realserver_map.find(test_sock_list[0].first) == realserver_map.end());
	BOOST_CHECK(realserver_map.find(test_sock_list[1].first) == realserver_map.end());
	BOOST_CHECK(realserver_map.find(test_sock_list[2].first) != realserver_map.end());
	//erase 2
	test_message.set_endpoint(test_sock_list[2].first);
	test_obj.set_up_thread_message_data(test_message);
	BOOST_CHECK(realserver_map.find(test_sock_list[0].first) == realserver_map.end());
	BOOST_CHECK(realserver_map.find(test_sock_list[1].first) == realserver_map.end());
	BOOST_CHECK(realserver_map.find(test_sock_list[2].first) != realserver_map.end());
	test_obj.test_call();
	BOOST_CHECK(realserver_map.find(test_sock_list[0].first) == realserver_map.end());
	BOOST_CHECK(realserver_map.find(test_sock_list[1].first) == realserver_map.end());
	BOOST_CHECK(realserver_map.find(test_sock_list[2].first) == realserver_map.end());
	BOOST_CHECK(realserver_map.empty());
		
	// unit_test [5] up_thread_realserver_disconnect_event not fond function error check
	std::cout << "[5] up_thread_realserver_disconnect_event not fond function error check" << std::endl;
	test_obj.up_thread_function_array_clear();
	proto_test.handle_realserver_disconnect_res_tag = l7vs::protocol_module_base::FINALIZE;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(46,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	
	// unit_test [6] up_thread_realserver_disconnect_event returnd illegal EVENT_TAG error check
	std::cout << "[6] up_thread_realserver_disconnect_event returnd illegal EVENT_TAG error check" << std::endl;
	test_obj.up_thread_module_event_map_clear();
	proto_test.handle_realserver_disconnect_res_tag = l7vs::protocol_module_base::FINALIZE;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(45,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	
	mutex_lock_test test_lock_obj(vs,io);
	test_lock_obj.set_up_thread_realserver_disconnect_event_test();
	test_lock_obj.set_protocol_module((l7vs::protocol_module_base*)&proto_test);	
	
	test_lock_obj.test_thread_wait.lock();
	test_lock_obj.befor_thread_id = proc_id;
	test_lock_obj.after_thread_id = proc_id;
	test_lock_obj.mutex_lock();
	
	boost::thread test_thread(boost::bind(&mutex_lock_test::test_run,&test_lock_obj));
	
	BOOST_CHECK(test_lock_obj.befor_thread_id == proc_id);
	BOOST_CHECK(test_lock_obj.after_thread_id == proc_id);
	
	boost::thread::id test_id = test_thread.get_id();
	
	BOOST_CHECK(test_id != proc_id);
	
	// test start
	test_lock_obj.test_thread_wait.unlock();
	sleep(1);
	
	// unit_test [7] up_thread_realserver_disconnect_event thread block test (mutex lock)
	std::cout << "[7] up_thread_realserver_disconnect_event thread block test (mutex lock)" << std::endl;
	BOOST_CHECK(test_lock_obj.befor_thread_id == test_id);
	BOOST_CHECK(test_lock_obj.after_thread_id == proc_id);
	
	test_lock_obj.mutex_unlock();
	sleep(1);
	
	// unit_test [8] up_thread_realserver_disconnect_event thread run test (mutex unlock)
	std::cout << "[8] up_thread_realserver_disconnect_event thread run test (mutex unlock)" << std::endl;
	BOOST_CHECK(test_lock_obj.befor_thread_id == test_id);
	BOOST_CHECK(test_lock_obj.after_thread_id == test_id);
	
	BOOST_MESSAGE( "----- up_thread_realserver_disconnect_event test end -----" );
	
}


// down_thread_realserver_disconnetc_event test
// down_thread_realserver_disconnetc_event test class
class down_thread_realserver_disconnect_event_test_class : public module_event_map_test_base_class{
	public:
		down_thread_realserver_disconnect_event_test_class(l7vs::virtualservice_tcp& vs,boost::asio::io_service& session_io) : module_event_map_test_base_class(vs,session_io){
		};
		
		~down_thread_realserver_disconnect_event_test_class(){};
		
		void test_call(){
			l7vs::tcp_session::down_thread_realserver_disconnect_event(LOCAL_PROC);
		};
		
		std::list<socket_element>& get_realserver_socket_list(){
			return down_thread_receive_realserver_socket_list;
		};
};

void down_thread_realserver_disconnect_event_test(){
	
	BOOST_MESSAGE( "----- down_thread_realserver_disconnect_event test start -----" );
	
	boost::asio::io_service io;
	
	l7vs::virtualservice_tcp vs;	
	
	down_thread_realserver_disconnect_event_test_class test_obj(vs,io);
	
	std::string test_protocol_name("test protocol");
	l7vs::test_protocol_module proto_test(test_protocol_name);

	test_obj.set_protocol_module((l7vs::protocol_module_base*)&proto_test);
	
	l7vs::protocol_module_base::EVENT_TAG chek_event[7];
	chek_event[0] = l7vs::protocol_module_base::CLIENT_DISCONNECT;
	chek_event[1] = l7vs::protocol_module_base::CLIENT_CONNECTION_CHECK;
	chek_event[2] = l7vs::protocol_module_base::REALSERVER_RECV;
	chek_event[3] = l7vs::protocol_module_base::REALSERVER_DISCONNECT;
	chek_event[4] = l7vs::protocol_module_base::SORRYSERVER_RECV;
	chek_event[5] = l7vs::protocol_module_base::SORRYSERVER_DISCONNECT;
	chek_event[6] = l7vs::protocol_module_base::FINALIZE;
	
	// unit_test [1] down_thread_realserver_disconnect_event down_thread_next_call_function update check
	std::cout << "[1] down_thread_realserver_disconnect_event down_thread_next_call_function update check" << std::endl;
	for(int i = 0; i < 7;i++){
		proto_test.handle_realserver_disconnect_res_tag = chek_event[i];
		test_obj.test_call();
		BOOST_CHECK(test_obj.chk_down_thread_next_call_function(chek_event[i]));
	}
	
	// module parameter check
	boost::thread::id def_id;
	boost::thread::id proc_id = boost::this_thread::get_id();
	boost::asio::ip::tcp::endpoint test_end(boost::asio::ip::address::from_string("100.102.103.104"), 7777);
	l7vs::tcp_data test_message;
	test_message.set_endpoint(test_end);
	test_obj.set_down_thread_id(proc_id);
	test_obj.set_down_thread_message_data(test_message);
	proto_test.handle_realserver_disconnect_thread_id = def_id;
	proto_test.handle_realserver_disconnect_rs_endpoint = boost::asio::ip::tcp::endpoint();
	proto_test.handle_realserver_disconnect_res_tag = l7vs::protocol_module_base::FINALIZE;
	BOOST_CHECK(proto_test.handle_realserver_disconnect_thread_id != proc_id);
	BOOST_CHECK(proto_test.handle_realserver_disconnect_rs_endpoint != test_end);
	test_obj.test_call();
	// unit_test [2] down_thread_realserver_disconnect_event module parameter check thread id
	std::cout << "[2] down_thread_realserver_disconnect_event module parameter check thread id" << std::endl;
	BOOST_CHECK(proto_test.handle_realserver_disconnect_thread_id == proc_id);
	
	// unit_test [3] down_thread_realserver_disconnect_event module parameter check endpoint
	std::cout << "[3] down_thread_realserver_disconnect_event module parameter check endpoint" << std::endl;
	BOOST_CHECK(proto_test.handle_realserver_disconnect_rs_endpoint == test_end);
	
	
	// unit_test [4] down_thread_realserver_disconnect_event realserver map erase check
	std::cout << "[4] down_thread_realserver_disconnect_event  realserver map erase check" << std::endl;
	
	std::list< std::pair< boost::asio::ip::tcp::endpoint , boost::shared_ptr< l7vs::tcp_socket > > >& realserver_list = test_obj.get_realserver_socket_list();
	boost::shared_ptr< l7vs::tcp_socket > test_tcp_sock0(new l7vs::tcp_socket(io));
	boost::shared_ptr< l7vs::tcp_socket > test_tcp_sock1(new l7vs::tcp_socket(io));
	boost::shared_ptr< l7vs::tcp_socket > test_tcp_sock2(new l7vs::tcp_socket(io));
	std::pair< boost::asio::ip::tcp::endpoint , boost::shared_ptr< l7vs::tcp_socket > > test_sock_list[3];
	test_sock_list[0].first = boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string("100.100.100.100"), 0);
	test_sock_list[0].second = test_tcp_sock0;
	test_sock_list[1].first = boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string("101.101.101.101"), 1);
	test_sock_list[1].second = test_tcp_sock1;
	test_sock_list[2].first = boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string("102.102.102.102"), 2);
	test_sock_list[2].second = test_tcp_sock2;
	for(int i = 0;i < 3;i++){
		realserver_list.push_back(test_sock_list[i]);
	}
	proto_test.handle_realserver_disconnect_res_tag = l7vs::protocol_module_base::FINALIZE;
	std::list< std::pair< boost::asio::ip::tcp::endpoint , boost::shared_ptr< l7vs::tcp_socket > > >::iterator ret_ite; 
	
	//erase 1
	test_message.set_endpoint(test_sock_list[1].first);
	test_obj.set_down_thread_message_data(test_message);	
	ret_ite = realserver_list.begin();
	BOOST_CHECK(ret_ite->first == test_sock_list[0].first);
	ret_ite++;
	BOOST_CHECK(ret_ite->first == test_sock_list[1].first);
	ret_ite++;
	BOOST_CHECK(ret_ite->first == test_sock_list[2].first);
	ret_ite++;
	BOOST_CHECK(ret_ite == realserver_list.end());
	test_obj.test_call();
	ret_ite = realserver_list.begin();
	BOOST_CHECK(ret_ite->first == test_sock_list[0].first);
	ret_ite++;
	BOOST_CHECK(ret_ite->first == test_sock_list[2].first);
	ret_ite++;
	BOOST_CHECK(ret_ite == realserver_list.end());
	
	//erase 0
	test_message.set_endpoint(test_sock_list[0].first);
	test_obj.set_down_thread_message_data(test_message);	
	ret_ite = realserver_list.begin();
	BOOST_CHECK(ret_ite->first == test_sock_list[0].first);
	ret_ite++;
	BOOST_CHECK(ret_ite->first == test_sock_list[2].first);
	ret_ite++;
	BOOST_CHECK(ret_ite == realserver_list.end());
	test_obj.test_call();
	ret_ite = realserver_list.begin();
	BOOST_CHECK(ret_ite->first == test_sock_list[2].first);
	ret_ite++;
	BOOST_CHECK(ret_ite == realserver_list.end());
	
	//erase 2
	test_message.set_endpoint(test_sock_list[2].first);
	test_obj.set_down_thread_message_data(test_message);	
	ret_ite = realserver_list.begin();
	BOOST_CHECK(ret_ite->first == test_sock_list[2].first);
	ret_ite++;
	BOOST_CHECK(ret_ite == realserver_list.end());
	test_obj.test_call();
	ret_ite = realserver_list.begin();
	BOOST_CHECK(ret_ite == realserver_list.end());
	BOOST_CHECK(realserver_list.empty());

	// unit_test [5] down_thread_realserver_disconnect_event not fond function error check
	std::cout << "[5] down_thread_realserver_disconnect_event not fond function error check" << std::endl;
	test_obj.down_thread_function_array_clear();
	proto_test.handle_realserver_disconnect_res_tag = l7vs::protocol_module_base::FINALIZE;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(75,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	
	// unit_test [6] down_thread_realserver_disconnect_event returnd illegal EVENT_TAG error check
	std::cout << "[6] down_thread_realserver_disconnect_event returnd illegal EVENT_TAG error check" << std::endl;
	test_obj.down_thread_module_event_map_clear();
	proto_test.handle_realserver_disconnect_res_tag = l7vs::protocol_module_base::FINALIZE;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(74,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	
	mutex_lock_test test_lock_obj(vs,io);
	test_lock_obj.set_down_thread_realserver_disconnect_event_test();
	test_lock_obj.set_protocol_module((l7vs::protocol_module_base*)&proto_test);
	
	test_lock_obj.test_thread_wait.lock();
	test_lock_obj.befor_thread_id = proc_id;
	test_lock_obj.after_thread_id = proc_id;
	test_lock_obj.mutex_lock();
	
	boost::thread test_thread(boost::bind(&mutex_lock_test::test_run,&test_lock_obj));
	
	BOOST_CHECK(test_lock_obj.befor_thread_id == proc_id);
	BOOST_CHECK(test_lock_obj.after_thread_id == proc_id);
	
	boost::thread::id test_id = test_thread.get_id();
	
	BOOST_CHECK(test_id != proc_id);
	
	// test start
	test_lock_obj.test_thread_wait.unlock();
	sleep(1);
	
	// unit_test [7] down_thread_realserver_disconnect_event thread block test (mutex lock)
	std::cout << "[7] down_thread_realserver_disconnect_event thread block test (mutex lock)" << std::endl;
	BOOST_CHECK(test_lock_obj.befor_thread_id == test_id);
	BOOST_CHECK(test_lock_obj.after_thread_id == proc_id);
	
	test_lock_obj.mutex_unlock();
	sleep(1);
	
	// unit_test [8] down_thread_realserver_disconnect_event thread run test (mutex unlock)
	std::cout << "[8] down_thread_realserver_disconnect_event thread run test (mutex unlock)" << std::endl;
	BOOST_CHECK(test_lock_obj.befor_thread_id == test_id);
	BOOST_CHECK(test_lock_obj.after_thread_id == test_id);
	
	BOOST_MESSAGE( "----- down_thread_realserver_disconnect_event test end -----" );
}

// up_thread_sorryserver_disconnect_event test
// up_thread_sorryserver_disconnect_event test class
class up_thread_sorryserver_disconnect_event_test_class : public module_event_map_test_base_class{
	public:
		up_thread_sorryserver_disconnect_event_test_class(l7vs::virtualservice_tcp& vs,boost::asio::io_service& session_io) : module_event_map_test_base_class(vs,session_io){
		};
		
		~up_thread_sorryserver_disconnect_event_test_class(){};
		
		void test_call(){
			l7vs::tcp_session::up_thread_sorryserver_disconnect_event(LOCAL_PROC);
		};
};

void up_thread_sorryserver_disconnect_event_test(){
	
	BOOST_MESSAGE( "----- up_thread_sorryserver_disconnect_event_test test start -----" );
	
	boost::asio::io_service io;
	
	l7vs::virtualservice_tcp vs;	
	
	up_thread_sorryserver_disconnect_event_test_class test_obj(vs,io);
	
	std::string test_protocol_name("test protocol");
	l7vs::test_protocol_module proto_test(test_protocol_name);

	test_obj.set_protocol_module((l7vs::protocol_module_base*)&proto_test);
	
	l7vs::protocol_module_base::EVENT_TAG chek_event[13];
	chek_event[0] = l7vs::protocol_module_base::ACCEPT;
	chek_event[1] = l7vs::protocol_module_base::CLIENT_DISCONNECT;
	chek_event[2] = l7vs::protocol_module_base::CLIENT_RECV;
	chek_event[3] = l7vs::protocol_module_base::CLIENT_RESPONSE_SEND;
	chek_event[4] = l7vs::protocol_module_base::REALSERVER_SELECT;
	chek_event[5] = l7vs::protocol_module_base::REALSERVER_CONNECT;
	chek_event[6] = l7vs::protocol_module_base::REALSERVER_SEND;
	chek_event[7] = l7vs::protocol_module_base::REALSERVER_DISCONNECT;
	chek_event[8] = l7vs::protocol_module_base::SORRYSERVER_SELECT;
	chek_event[9] = l7vs::protocol_module_base::SORRYSERVER_CONNECT;
	chek_event[10] = l7vs::protocol_module_base::SORRYSERVER_SEND;
	chek_event[11] = l7vs::protocol_module_base::SORRYSERVER_DISCONNECT;
	chek_event[12] = l7vs::protocol_module_base::FINALIZE;
	
	// unit_test [1] up_thread_sorryserver_disconnect_event up_thread_next_call_function update check
	std::cout << "[1] up_thread_sorryserver_disconnect_event up_thread_next_call_function update check" << std::endl;
	for(int i = 0; i < 13;i++){
		proto_test.handle_sorryserver_disconnect_res_tag = chek_event[i];
		test_obj.test_call();
		BOOST_CHECK(test_obj.chk_up_thread_next_call_function(chek_event[i]));
	}
	
	// module parameter check
	boost::thread::id def_id;
	boost::thread::id proc_id = boost::this_thread::get_id();
	boost::asio::ip::tcp::endpoint test_end(boost::asio::ip::address::from_string("100.102.103.104"), 7777);
	l7vs::tcp_data test_message;
	test_message.set_endpoint(test_end);
	test_obj.set_up_thread_id(proc_id);
	test_obj.set_up_thread_message_data(test_message);
	proto_test.handle_sorryserver_disconnect_thread_id = def_id;
	proto_test.handle_sorryserver_disconnect_rs_endpoint = boost::asio::ip::tcp::endpoint();
	proto_test.handle_sorryserver_disconnect_res_tag = l7vs::protocol_module_base::FINALIZE;
	BOOST_CHECK(proto_test.handle_sorryserver_disconnect_thread_id != proc_id);
	BOOST_CHECK(proto_test.handle_sorryserver_disconnect_rs_endpoint != test_end);
	test_obj.test_call();
	// unit_test [2] up_thread_sorryserver_disconnect_event module parameter check thread id
	std::cout << "[2] up_thread_sorryserver_disconnect_event module parameter check thread id" << std::endl;
	BOOST_CHECK(proto_test.handle_sorryserver_disconnect_thread_id == proc_id);
	
	// unit_test [3] up_thread_sorryserver_disconnect_event module parameter check endpoint
	std::cout << "[3] up_thread_sorryserver_disconnect_event module parameter check endpoint" << std::endl;
	BOOST_CHECK(proto_test.handle_sorryserver_disconnect_rs_endpoint == test_end);
	
	// unit_test [4] up_thread_sorryserver_disconnect_event not fond function error check
	std::cout << "[4] up_thread_sorryserver_disconnect_event not fond function error check" << std::endl;
	test_obj.up_thread_function_array_clear();
	proto_test.handle_sorryserver_disconnect_res_tag = l7vs::protocol_module_base::FINALIZE;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(65,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	
	// unit_test [5] up_thread_sorryserver_disconnect_event returnd illegal EVENT_TAG error check
	std::cout << "[5] up_thread_sorryserver_disconnect_event returnd illegal EVENT_TAG error check" << std::endl;
	test_obj.up_thread_module_event_map_clear();
	proto_test.handle_sorryserver_disconnect_res_tag = l7vs::protocol_module_base::FINALIZE;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(64,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	
	mutex_lock_test test_lock_obj(vs,io);
	test_lock_obj.set_up_thread_sorryserver_disconnect_event_test();
	test_lock_obj.set_protocol_module((l7vs::protocol_module_base*)&proto_test);
	
	test_lock_obj.test_thread_wait.lock();
	test_lock_obj.befor_thread_id = proc_id;
	test_lock_obj.after_thread_id = proc_id;
	test_lock_obj.mutex_lock();
	
	boost::thread test_thread(boost::bind(&mutex_lock_test::test_run,&test_lock_obj));
	
	BOOST_CHECK(test_lock_obj.befor_thread_id == proc_id);
	BOOST_CHECK(test_lock_obj.after_thread_id == proc_id);
	
	boost::thread::id test_id = test_thread.get_id();
	
	BOOST_CHECK(test_id != proc_id);
	
	// test start
	test_lock_obj.test_thread_wait.unlock();
	sleep(1);
	
	// unit_test [6] up_thread_sorryserver_disconnect_event thread block test (mutex lock)
	std::cout << "[6] up_thread_sorryserver_disconnect_event thread block test (mutex lock)" << std::endl;
	BOOST_CHECK(test_lock_obj.befor_thread_id == test_id);
	BOOST_CHECK(test_lock_obj.after_thread_id == proc_id);
	
	test_lock_obj.mutex_unlock();
	sleep(1);
	
	// unit_test [7] up_thread_sorryserver_disconnect_event thread run test (mutex unlock)
	std::cout << "[7] up_thread_sorryserver_disconnect_event thread run test (mutex unlock)" << std::endl;
	BOOST_CHECK(test_lock_obj.befor_thread_id == test_id);
	BOOST_CHECK(test_lock_obj.after_thread_id == test_id);
	
	BOOST_MESSAGE( "----- up_thread_sorryserver_disconnect_event_test test end -----" );
}

// up_thread_sorryserver_mod_disconnect test
// up_thread_sorryserver_mod_disconnect test class
class up_thread_sorryserver_mod_disconnect_test_class : public module_event_map_test_base_class{
	public:
		up_thread_sorryserver_mod_disconnect_test_class(l7vs::virtualservice_tcp& vs,boost::asio::io_service& session_io) : module_event_map_test_base_class(vs,session_io){
		};
		
		~up_thread_sorryserver_mod_disconnect_test_class(){};
		
		void test_call(){
			l7vs::tcp_session::up_thread_sorryserver_mod_disconnect(LOCAL_PROC);
		};
		
		boost::shared_ptr< l7vs::tcp_socket > get_sorry_socket(){
			return sorryserver_socket.second;
		};

		boost::asio::ip::tcp::endpoint& get_sorry_endpoint(){
			return sorryserver_socket.first;
		}
		
};

void up_thread_sorryserver_mod_disconnect_test(){
	
	BOOST_MESSAGE( "----- up_thread_sorryserver_mod_disconnect test start -----" );
	
	boost::asio::io_service io;
	
	l7vs::virtualservice_tcp vs;	
	
	up_thread_sorryserver_mod_disconnect_test_class test_obj(vs,io);
	
	std::string test_protocol_name("test protocol");
	l7vs::test_protocol_module proto_test(test_protocol_name);

	test_obj.set_protocol_module((l7vs::protocol_module_base*)&proto_test);
	
	l7vs::protocol_module_base::EVENT_TAG chek_event[13];
	chek_event[0] = l7vs::protocol_module_base::ACCEPT;
	chek_event[1] = l7vs::protocol_module_base::CLIENT_DISCONNECT;
	chek_event[2] = l7vs::protocol_module_base::CLIENT_RECV;
	chek_event[3] = l7vs::protocol_module_base::CLIENT_RESPONSE_SEND;
	chek_event[4] = l7vs::protocol_module_base::REALSERVER_SELECT;
	chek_event[5] = l7vs::protocol_module_base::REALSERVER_CONNECT;
	chek_event[6] = l7vs::protocol_module_base::REALSERVER_SEND;
	chek_event[7] = l7vs::protocol_module_base::REALSERVER_DISCONNECT;
	chek_event[8] = l7vs::protocol_module_base::SORRYSERVER_SELECT;
	chek_event[9] = l7vs::protocol_module_base::SORRYSERVER_CONNECT;
	chek_event[10] = l7vs::protocol_module_base::SORRYSERVER_SEND;
	chek_event[11] = l7vs::protocol_module_base::SORRYSERVER_DISCONNECT;
	chek_event[12] = l7vs::protocol_module_base::FINALIZE;
	
	// unit_test [1] up_thread_sorryserver_mod_disconnect up_thread_next_call_function update check
	std::cout << "[1] up_thread_sorryserver_mod_disconnect up_thread_next_call_function update check" << std::endl;
	for(int i = 0; i < 13;i++){
		proto_test.handle_sorryserver_disconnect_res_tag = chek_event[i];
		test_obj.test_call();
		BOOST_CHECK(test_obj.chk_up_thread_next_call_function(chek_event[i]));
	}
	
	// module parameter check
	boost::thread::id def_id;
	boost::thread::id proc_id = boost::this_thread::get_id();
	boost::asio::ip::tcp::endpoint test_end(boost::asio::ip::address::from_string("100.102.103.104"), 7777);
	test_obj.get_sorry_endpoint() = test_end;
	test_obj.set_up_thread_id(proc_id);
	proto_test.handle_sorryserver_disconnect_thread_id = def_id;
	proto_test.handle_sorryserver_disconnect_rs_endpoint = boost::asio::ip::tcp::endpoint();
	proto_test.handle_sorryserver_disconnect_res_tag = l7vs::protocol_module_base::FINALIZE;
	BOOST_CHECK(proto_test.handle_sorryserver_disconnect_thread_id != proc_id);
	BOOST_CHECK(proto_test.handle_sorryserver_disconnect_rs_endpoint != test_end);
	test_obj.get_sorry_socket()->close_call_check = false;
	test_obj.get_sorry_socket()->close_res = true;

	test_obj.test_call();

	// unit_test [2] up_thread_sorryserver_mod_disconnect socket close check
	std::cout << "[2] up_thread_sorryserver_mod_disconnect socket close check" << std::endl;
	BOOST_CHECK(test_obj.get_sorry_socket()->close_call_check);

	// unit_test [3] up_thread_sorryserver_mod_disconnect endpoint not update check
	std::cout << "[3] up_thread_sorryserver_mod_disconnect endpoint not update  check" << std::endl;
	BOOST_CHECK(test_obj.get_sorry_endpoint() == test_end);

	// unit_test [4] up_thread_sorryserver_mod_disconnect module parameter check thread id
	std::cout << "[4] up_thread_sorryserver_mod_disconnect module parameter check thread id" << std::endl;
	BOOST_CHECK(proto_test.handle_sorryserver_disconnect_thread_id == proc_id);
	
	// unit_test [5] up_thread_sorryserver_mod_disconnect module parameter check endpoint
	std::cout << "[5] up_thread_sorryserver_mod_disconnect module parameter check endpoint" << std::endl;
	BOOST_CHECK(proto_test.handle_sorryserver_disconnect_rs_endpoint == test_end);
	

	proto_test.handle_sorryserver_disconnect_thread_id = def_id;
	proto_test.handle_sorryserver_disconnect_rs_endpoint = boost::asio::ip::tcp::endpoint();
	proto_test.handle_sorryserver_disconnect_res_tag = l7vs::protocol_module_base::FINALIZE;
	test_obj.get_sorry_socket()->close_call_check = false;
	test_obj.get_sorry_socket()->close_res = false;

	test_obj.test_call();

	// unit_test [6] up_thread_sorryserver_mod_disconnect socket close check
	std::cout << "[6] up_thread_sorryserver_mod_disconnect socket close check" << std::endl;
	BOOST_CHECK(test_obj.get_sorry_socket()->close_call_check);

	// unit_test [7] up_thread_sorryserver_mod_disconnect endpoint not update check
	std::cout << "[7] up_thread_sorryserver_mod_disconnect endpoint not update  check" << std::endl;
	BOOST_CHECK(test_obj.get_sorry_endpoint() == boost::asio::ip::tcp::endpoint());

	// unit_test [8] up_thread_sorryserver_mod_disconnect module parameter check thread id
	std::cout << "[8] up_thread_sorryserver_mod_disconnect module parameter check thread id" << std::endl;
	BOOST_CHECK(proto_test.handle_sorryserver_disconnect_thread_id == proc_id);
	
	// unit_test [9] up_thread_sorryserver_mod_disconnect module parameter check endpoint
	std::cout << "[9] up_thread_sorryserver_mod_disconnect module parameter check endpoint" << std::endl;
	BOOST_CHECK(proto_test.handle_sorryserver_disconnect_rs_endpoint == test_end);
	
	proto_test.handle_sorryserver_disconnect_thread_id = def_id;
	proto_test.handle_sorryserver_disconnect_rs_endpoint = boost::asio::ip::tcp::endpoint();
	proto_test.handle_sorryserver_disconnect_res_tag = l7vs::protocol_module_base::FINALIZE;
	test_obj.get_sorry_socket()->close_call_check = false;
	test_obj.get_sorry_socket()->close_res = false;

	test_obj.test_call();

	// unit_test [10] up_thread_sorryserver_mod_disconnect socket close check
	std::cout << "[10] up_thread_sorryserver_mod_disconnect socket close check" << std::endl;
	BOOST_CHECK(test_obj.get_sorry_socket()->close_call_check);

	// unit_test [11] up_thread_sorryserver_mod_disconnect endpoint not update check
	std::cout << "[11] up_thread_sorryserver_mod_disconnect endpoint not update  check" << std::endl;
	BOOST_CHECK(test_obj.get_sorry_endpoint() == boost::asio::ip::tcp::endpoint());

	// unit_test [12] up_thread_sorryserver_mod_disconnect module parameter check thread id
	std::cout << "[12] up_thread_sorryserver_mod_disconnect module parameter check thread id" << std::endl;
	BOOST_CHECK(proto_test.handle_sorryserver_disconnect_thread_id == proc_id);
	
	// unit_test [13] up_thread_sorryserver_mod_disconnect module parameter check endpoint
	std::cout << "[13] up_thread_sorryserver_mod_disconnect module parameter check endpoint" << std::endl;
	BOOST_CHECK(proto_test.handle_sorryserver_disconnect_rs_endpoint == boost::asio::ip::tcp::endpoint());
	
	// unit_test [14] up_thread_sorryserver_mod_disconnect not fond function error check
	std::cout << "[14] up_thread_sorryserver_mod_disconnect not fond function error check" << std::endl;
	test_obj.up_thread_function_array_clear();
	proto_test.handle_sorryserver_disconnect_res_tag = l7vs::protocol_module_base::FINALIZE;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(63,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	
	// unit_test [15] up_thread_sorryserver_mod_disconnect returnd illegal EVENT_TAG error check
	std::cout << "[15] up_thread_sorryserver_mod_disconnect returnd illegal EVENT_TAG error check" << std::endl;
	test_obj.up_thread_module_event_map_clear();
	proto_test.handle_sorryserver_disconnect_res_tag = l7vs::protocol_module_base::FINALIZE;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(62,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	
	mutex_lock_test test_lock_obj(vs,io);
	test_lock_obj.set_up_thread_sorryserver_disconnect_event_test();
	test_lock_obj.set_protocol_module((l7vs::protocol_module_base*)&proto_test);
	
	test_lock_obj.test_thread_wait.lock();
	test_lock_obj.befor_thread_id = proc_id;
	test_lock_obj.after_thread_id = proc_id;
	test_lock_obj.mutex_lock();
	
	boost::thread test_thread(boost::bind(&mutex_lock_test::test_run,&test_lock_obj));
	
	BOOST_CHECK(test_lock_obj.befor_thread_id == proc_id);
	BOOST_CHECK(test_lock_obj.after_thread_id == proc_id);
	
	boost::thread::id test_id = test_thread.get_id();
	
	BOOST_CHECK(test_id != proc_id);
	
	// test start
	test_lock_obj.test_thread_wait.unlock();
	sleep(1);
	
	// unit_test [16] up_thread_sorryserver_mod_disconnect thread block test (mutex lock)
	std::cout << "[16] up_thread_sorryserver_mod_disconnect thread block test (mutex lock)" << std::endl;
	BOOST_CHECK(test_lock_obj.befor_thread_id == test_id);
	BOOST_CHECK(test_lock_obj.after_thread_id == proc_id);
	
	test_lock_obj.mutex_unlock();
	sleep(1);
	
	// unit_test [17] up_thread_sorryserver_mod_disconnect thread run test (mutex unlock)
	std::cout << "[17] up_thread_sorryserver_mod_disconnect thread run test (mutex unlock)" << std::endl;
	BOOST_CHECK(test_lock_obj.befor_thread_id == test_id);
	BOOST_CHECK(test_lock_obj.after_thread_id == test_id);
	
	BOOST_MESSAGE( "----- up_thread_sorryserver_mod_disconnect test end -----" );
}

// down_thread_sorryserver_mod_disconnect test
// down_thread_sorryserver_mod_disconnect test class
class down_thread_sorryserver_mod_disconnect_test_class : public module_event_map_test_base_class{
	public:
		down_thread_sorryserver_mod_disconnect_test_class(l7vs::virtualservice_tcp& vs,boost::asio::io_service& session_io) : module_event_map_test_base_class(vs,session_io){
		};
		
		~down_thread_sorryserver_mod_disconnect_test_class(){};
		
		void test_call(){
			l7vs::tcp_session::down_thread_sorryserver_mod_disconnect(LOCAL_PROC);
		};
		
		boost::shared_ptr< l7vs::tcp_socket > get_sorry_socket(){
			return sorryserver_socket.second;
		};

		boost::asio::ip::tcp::endpoint& get_sorry_endpoint(){
			return sorryserver_socket.first;
		}
		
};

void down_thread_sorryserver_mod_disconnect_test(){
	
	BOOST_MESSAGE( "----- down_thread_sorryserver_mod_disconnect test start -----" );
	
	boost::asio::io_service io;
	
	l7vs::virtualservice_tcp vs;	
	
	down_thread_sorryserver_mod_disconnect_test_class test_obj(vs,io);
	
	std::string test_protocol_name("test protocol");
	l7vs::test_protocol_module proto_test(test_protocol_name);

	test_obj.set_protocol_module((l7vs::protocol_module_base*)&proto_test);
	
	l7vs::protocol_module_base::EVENT_TAG chek_event[7];
	chek_event[0] = l7vs::protocol_module_base::CLIENT_DISCONNECT;
	chek_event[1] = l7vs::protocol_module_base::CLIENT_CONNECTION_CHECK;
	chek_event[2] = l7vs::protocol_module_base::REALSERVER_RECV;
	chek_event[3] = l7vs::protocol_module_base::REALSERVER_DISCONNECT;
	chek_event[4] = l7vs::protocol_module_base::SORRYSERVER_RECV;
	chek_event[5] = l7vs::protocol_module_base::SORRYSERVER_DISCONNECT;
	chek_event[6] = l7vs::protocol_module_base::FINALIZE;
	
	// unit_test [1] down_thread_sorryserver_mod_disconnect down_thread_next_call_function update check
	std::cout << "[1] down_thread_sorryserver_mod_disconnect down_thread_next_call_function update check" << std::endl;
	for(int i = 0; i < 7;i++){
		proto_test.handle_sorryserver_disconnect_res_tag = chek_event[i];
		test_obj.test_call();
		BOOST_CHECK(test_obj.chk_down_thread_next_call_function(chek_event[i]));
	}

	// module parameter check
	boost::thread::id def_id;
	boost::thread::id proc_id = boost::this_thread::get_id();
	boost::asio::ip::tcp::endpoint test_end(boost::asio::ip::address::from_string("100.102.103.104"), 7777);
	test_obj.get_sorry_endpoint() = test_end;
	test_obj.set_down_thread_id(proc_id);
	proto_test.handle_sorryserver_disconnect_thread_id = def_id;
	proto_test.handle_sorryserver_disconnect_rs_endpoint = boost::asio::ip::tcp::endpoint();
	proto_test.handle_sorryserver_disconnect_res_tag = l7vs::protocol_module_base::FINALIZE;
	BOOST_CHECK(proto_test.handle_sorryserver_disconnect_thread_id != proc_id);
	BOOST_CHECK(proto_test.handle_sorryserver_disconnect_rs_endpoint != test_end);
	test_obj.get_sorry_socket()->close_call_check = false;
	test_obj.get_sorry_socket()->close_res = true;

	test_obj.test_call();

	// unit_test [2] down_thread_sorryserver_mod_disconnect socket close check
	std::cout << "[2] down_thread_sorryserver_mod_disconnect socket close check" << std::endl;
	BOOST_CHECK(test_obj.get_sorry_socket()->close_call_check);

	// unit_test [3] down_thread_sorryserver_mod_disconnect endpoint not update check
	std::cout << "[3] down_thread_sorryserver_mod_disconnect endpoint not update  check" << std::endl;
	BOOST_CHECK(test_obj.get_sorry_endpoint() == test_end);

	// unit_test [4] down_thread_sorryserver_mod_disconnect module parameter check thread id
	std::cout << "[4] down_thread_sorryserver_mod_disconnect module parameter check thread id" << std::endl;
	BOOST_CHECK(proto_test.handle_sorryserver_disconnect_thread_id == proc_id);
	
	// unit_test [5] down_thread_sorryserver_mod_disconnect module parameter check endpoint
	std::cout << "[5] down_thread_sorryserver_mod_disconnect module parameter check endpoint" << std::endl;
	BOOST_CHECK(proto_test.handle_sorryserver_disconnect_rs_endpoint == test_end);
	
	proto_test.handle_sorryserver_disconnect_thread_id = def_id;
	proto_test.handle_sorryserver_disconnect_rs_endpoint = boost::asio::ip::tcp::endpoint();
	proto_test.handle_sorryserver_disconnect_res_tag = l7vs::protocol_module_base::FINALIZE;
	test_obj.get_sorry_socket()->close_call_check = false;
	test_obj.get_sorry_socket()->close_res = false;

	test_obj.test_call();

	// unit_test [6] down_thread_sorryserver_mod_disconnect socket close check
	std::cout << "[6] down_thread_sorryserver_mod_disconnect socket close check" << std::endl;
	BOOST_CHECK(test_obj.get_sorry_socket()->close_call_check);

	// unit_test [7] down_thread_sorryserver_mod_disconnect endpoint not update check
	std::cout << "[7] down_thread_sorryserver_mod_disconnect endpoint not update  check" << std::endl;
	BOOST_CHECK(test_obj.get_sorry_endpoint() == boost::asio::ip::tcp::endpoint());

	// unit_test [8] down_thread_sorryserver_mod_disconnect module parameter check thread id
	std::cout << "[8] down_thread_sorryserver_mod_disconnect module parameter check thread id" << std::endl;
	BOOST_CHECK(proto_test.handle_sorryserver_disconnect_thread_id == proc_id);
	
	// unit_test [9] down_thread_sorryserver_mod_disconnect module parameter check endpoint
	std::cout << "[9] down_thread_sorryserver_mod_disconnect module parameter check endpoint" << std::endl;
	BOOST_CHECK(proto_test.handle_sorryserver_disconnect_rs_endpoint == test_end);
	
	proto_test.handle_sorryserver_disconnect_thread_id = def_id;
	proto_test.handle_sorryserver_disconnect_rs_endpoint = boost::asio::ip::tcp::endpoint();
	proto_test.handle_sorryserver_disconnect_res_tag = l7vs::protocol_module_base::FINALIZE;
	test_obj.get_sorry_socket()->close_call_check = false;
	test_obj.get_sorry_socket()->close_res = false;

	test_obj.test_call();

	// unit_test [10] down_thread_sorryserver_mod_disconnect socket close check
	std::cout << "[10] down_thread_sorryserver_mod_disconnect socket close check" << std::endl;
	BOOST_CHECK(test_obj.get_sorry_socket()->close_call_check);

	// unit_test [11] down_thread_sorryserver_mod_disconnect endpoint not update check
	std::cout << "[11] down_thread_sorryserver_mod_disconnect endpoint not update  check" << std::endl;
	BOOST_CHECK(test_obj.get_sorry_endpoint() == boost::asio::ip::tcp::endpoint());

	// unit_test [12] down_thread_sorryserver_mod_disconnect module parameter check thread id
	std::cout << "[12] down_thread_sorryserver_mod_disconnect module parameter check thread id" << std::endl;
	BOOST_CHECK(proto_test.handle_sorryserver_disconnect_thread_id == proc_id);
	
	// unit_test [13] down_thread_sorryserver_mod_disconnect module parameter check endpoint
	std::cout << "[13] down_thread_sorryserver_mod_disconnect module parameter check endpoint" << std::endl;
	BOOST_CHECK(proto_test.handle_sorryserver_disconnect_rs_endpoint == boost::asio::ip::tcp::endpoint());
	
	// unit_test [14] down_thread_sorryserver_mod_disconnect not fond function error check
	std::cout << "[14] down_thread_sorryserver_mod_disconnect not fond function error check" << std::endl;
	test_obj.down_thread_function_array_clear();
	proto_test.handle_sorryserver_disconnect_res_tag = l7vs::protocol_module_base::FINALIZE;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(93,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	
	// unit_test [15] down_thread_sorryserver_mod_disconnect returnd illegal EVENT_TAG error check
	std::cout << "[15] down_thread_sorryserver_mod_disconnect returnd illegal EVENT_TAG error check" << std::endl;
	test_obj.down_thread_module_event_map_clear();
	proto_test.handle_sorryserver_disconnect_res_tag = l7vs::protocol_module_base::FINALIZE;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(92,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	
	mutex_lock_test test_lock_obj(vs,io);
	test_lock_obj.set_down_thread_sorryserver_disconnect_event_test();
	test_lock_obj.set_protocol_module((l7vs::protocol_module_base*)&proto_test);
	
	test_lock_obj.test_thread_wait.lock();
	test_lock_obj.befor_thread_id = proc_id;
	test_lock_obj.after_thread_id = proc_id;
	test_lock_obj.mutex_lock();
	
	boost::thread test_thread(boost::bind(&mutex_lock_test::test_run,&test_lock_obj));
	
	BOOST_CHECK(test_lock_obj.befor_thread_id == proc_id);
	BOOST_CHECK(test_lock_obj.after_thread_id == proc_id);
	
	boost::thread::id test_id = test_thread.get_id();
	
	BOOST_CHECK(test_id != proc_id);
	
	// test start
	test_lock_obj.test_thread_wait.unlock();
	sleep(1);
	
	// unit_test [16] down_thread_sorryserver_mod_disconnect thread block test (mutex lock)
	std::cout << "[16] down_thread_sorryserver_mod_disconnect thread block test (mutex lock)" << std::endl;
	BOOST_CHECK(test_lock_obj.befor_thread_id == test_id);
	BOOST_CHECK(test_lock_obj.after_thread_id == proc_id);
	
	test_lock_obj.mutex_unlock();
	sleep(1);
	
	// unit_test [17] down_thread_sorryserver_mod_disconnect thread run test (mutex unlock)
	std::cout << "[17] down_thread_sorryserver_mod_disconnect thread run test (mutex unlock)" << std::endl;
	BOOST_CHECK(test_lock_obj.befor_thread_id == test_id);
	BOOST_CHECK(test_lock_obj.after_thread_id == test_id);
	
	BOOST_MESSAGE( "----- down_thread_sorryserver_mod_disconnect test end -----" );
}



// down_thread_sorryserver_disconnetc_event test
// down_thread_sorryserver_disconnetc_event test class
class down_thread_sorryserver_disconnect_event_test_class : public module_event_map_test_base_class{
	public:
		down_thread_sorryserver_disconnect_event_test_class(l7vs::virtualservice_tcp& vs,boost::asio::io_service& session_io) : module_event_map_test_base_class(vs,session_io){
		};
		
		~down_thread_sorryserver_disconnect_event_test_class(){};
		
		void test_call(){
			l7vs::tcp_session::down_thread_sorryserver_disconnect_event(LOCAL_PROC);
		};
};

void down_thread_sorryserver_disconnect_event_test(){
	
	BOOST_MESSAGE( "----- down_thread_sorryserver_disconnect_event test start -----" );
	
	boost::asio::io_service io;
	
	l7vs::virtualservice_tcp vs;	
	
	down_thread_sorryserver_disconnect_event_test_class test_obj(vs,io);
	
	std::string test_protocol_name("test protocol");
	l7vs::test_protocol_module proto_test(test_protocol_name);

	test_obj.set_protocol_module((l7vs::protocol_module_base*)&proto_test);
	
	l7vs::protocol_module_base::EVENT_TAG chek_event[7];
	chek_event[0] = l7vs::protocol_module_base::CLIENT_DISCONNECT;
	chek_event[1] = l7vs::protocol_module_base::CLIENT_CONNECTION_CHECK;
	chek_event[2] = l7vs::protocol_module_base::REALSERVER_RECV;
	chek_event[3] = l7vs::protocol_module_base::REALSERVER_DISCONNECT;
	chek_event[4] = l7vs::protocol_module_base::SORRYSERVER_RECV;
	chek_event[5] = l7vs::protocol_module_base::SORRYSERVER_DISCONNECT;
	chek_event[6] = l7vs::protocol_module_base::FINALIZE;
	
	// unit_test [1] down_thread_sorryserver_disconnect_event down_thread_next_call_function update check
	std::cout << "[1] down_thread_sorryserver_disconnect_event down_thread_next_call_function update check" << std::endl;
	for(int i = 0; i < 7;i++){
		proto_test.handle_sorryserver_disconnect_res_tag = chek_event[i];
		test_obj.test_call();
		BOOST_CHECK(test_obj.chk_down_thread_next_call_function(chek_event[i]));
	}
	
	// module parameter check
	boost::thread::id def_id;
	boost::thread::id proc_id = boost::this_thread::get_id();
	boost::asio::ip::tcp::endpoint test_end(boost::asio::ip::address::from_string("100.102.103.104"), 7777);
	l7vs::tcp_data test_message;
	test_message.set_endpoint(test_end);
	test_obj.set_down_thread_id(proc_id);
	test_obj.set_down_thread_message_data(test_message);
	proto_test.handle_sorryserver_disconnect_thread_id = def_id;
	proto_test.handle_sorryserver_disconnect_rs_endpoint = boost::asio::ip::tcp::endpoint();
	proto_test.handle_sorryserver_disconnect_res_tag = l7vs::protocol_module_base::FINALIZE;
	BOOST_CHECK(proto_test.handle_sorryserver_disconnect_thread_id != proc_id);
	BOOST_CHECK(proto_test.handle_sorryserver_disconnect_rs_endpoint != test_end);
	test_obj.test_call();
	// unit_test [2] down_thread_sorryserver_disconnect_event module parameter check thread id
	std::cout << "[2] down_thread_sorryserver_disconnect_event module parameter check thread id" << std::endl;
	BOOST_CHECK(proto_test.handle_sorryserver_disconnect_thread_id == proc_id);
	
	// unit_test [3] down_thread_sorryserver_disconnect_event module parameter check endpoint
	std::cout << "[3] down_thread_sorryserver_disconnect_event module parameter check endpoint" << std::endl;
	BOOST_CHECK(proto_test.handle_sorryserver_disconnect_rs_endpoint == test_end);
	
	// unit_test [4] down_thread_sorryserver_disconnect_event not fond function error check
	std::cout << "[4] down_thread_sorryserver_disconnect_event not fond function error check" << std::endl;
	test_obj.down_thread_function_array_clear();
	proto_test.handle_sorryserver_disconnect_res_tag = l7vs::protocol_module_base::FINALIZE;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(95,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	
	// unit_test [5] down_thread_sorryserver_disconnect_event returnd illegal EVENT_TAG error check
	std::cout << "[5] down_thread_sorryserver_disconnect_event returnd illegal EVENT_TAG error check" << std::endl;
	test_obj.down_thread_module_event_map_clear();
	proto_test.handle_sorryserver_disconnect_res_tag = l7vs::protocol_module_base::FINALIZE;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(94,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	
	mutex_lock_test test_lock_obj(vs,io);
	test_lock_obj.set_down_thread_sorryserver_disconnect_event_test();
	test_lock_obj.set_protocol_module((l7vs::protocol_module_base*)&proto_test);
	
	test_lock_obj.test_thread_wait.lock();
	test_lock_obj.befor_thread_id = proc_id;
	test_lock_obj.after_thread_id = proc_id;
	test_lock_obj.mutex_lock();
	
	boost::thread test_thread(boost::bind(&mutex_lock_test::test_run,&test_lock_obj));
	
	BOOST_CHECK(test_lock_obj.befor_thread_id == proc_id);
	BOOST_CHECK(test_lock_obj.after_thread_id == proc_id);
	
	boost::thread::id test_id = test_thread.get_id();
	
	BOOST_CHECK(test_id != proc_id);
	
	// test start
	test_lock_obj.test_thread_wait.unlock();
	sleep(1);
	
	// unit_test [6] down_thread_sorryserver_disconnect_event thread block test (mutex lock)
	std::cout << "[6] down_thread_sorryserver_disconnect_event thread block test (mutex lock)" << std::endl;
	BOOST_CHECK(test_lock_obj.befor_thread_id == test_id);
	BOOST_CHECK(test_lock_obj.after_thread_id == proc_id);
	
	test_lock_obj.mutex_unlock();
	sleep(1);
	
	// unit_test [7] down_thread_sorryserver_disconnect_event thread run test (mutex unlock)
	std::cout << "[7] down_thread_sorryserver_disconnect_event thread run test (mutex unlock)" << std::endl;
	BOOST_CHECK(test_lock_obj.befor_thread_id == test_id);
	BOOST_CHECK(test_lock_obj.after_thread_id == test_id);
	
	BOOST_MESSAGE( "----- down_thread_sorryserver_disconnect_event test end -----" );
}

// up_thread_sorry_enable_event test
// up_thread_sorry_enable_event test class
class up_thread_sorry_enable_event_test_class : public module_event_map_test_base_class{
	public:
		up_thread_sorry_enable_event_test_class(l7vs::virtualservice_tcp& vs,boost::asio::io_service& session_io) : module_event_map_test_base_class(vs,session_io){
		};
		
		~up_thread_sorry_enable_event_test_class(){};
		
		void test_call(){
			l7vs::tcp_session::up_thread_sorry_enable_event(LOCAL_PROC);
		};
};

void up_thread_sorry_enable_event_test(){
	
	BOOST_MESSAGE( "----- up_thread_sorry_enable_event test start -----" );
	
	boost::asio::io_service io;
	
	l7vs::virtualservice_tcp vs;	
	
	up_thread_sorry_enable_event_test_class test_obj(vs,io);
	
	std::string test_protocol_name("test protocol");
	l7vs::test_protocol_module proto_test(test_protocol_name);

	test_obj.set_protocol_module((l7vs::protocol_module_base*)&proto_test);
	
	l7vs::protocol_module_base::EVENT_TAG chek_event[13];
	chek_event[0] = l7vs::protocol_module_base::ACCEPT;
	chek_event[1] = l7vs::protocol_module_base::CLIENT_DISCONNECT;
	chek_event[2] = l7vs::protocol_module_base::CLIENT_RECV;
	chek_event[3] = l7vs::protocol_module_base::CLIENT_RESPONSE_SEND;
	chek_event[4] = l7vs::protocol_module_base::REALSERVER_SELECT;
	chek_event[5] = l7vs::protocol_module_base::REALSERVER_CONNECT;
	chek_event[6] = l7vs::protocol_module_base::REALSERVER_SEND;
	chek_event[7] = l7vs::protocol_module_base::REALSERVER_DISCONNECT;
	chek_event[8] = l7vs::protocol_module_base::SORRYSERVER_SELECT;
	chek_event[9] = l7vs::protocol_module_base::SORRYSERVER_CONNECT;
	chek_event[10] = l7vs::protocol_module_base::SORRYSERVER_SEND;
	chek_event[11] = l7vs::protocol_module_base::SORRYSERVER_DISCONNECT;
	chek_event[12] = l7vs::protocol_module_base::FINALIZE;
	
	// unit_test [1] up_thread_sorry_enable_event up_thread_next_call_function update check
	std::cout << "[1] up_thread_sorry_enable_event up_thread_next_call_function update check" << std::endl;
	for(int i = 0; i < 13;i++){
		proto_test.handle_sorry_enable_res_tag = chek_event[i];
		test_obj.test_call();
		BOOST_CHECK(test_obj.chk_up_thread_next_call_function(chek_event[i]));
	}
	
	// unit_test [2] up_thread_sorry_enable_event module parameter check thread id
	std::cout << "[2] up_thread_sorry_enable_event module parameter check thread id" << std::endl;
	boost::thread::id def_id;
	boost::thread::id proc_id = boost::this_thread::get_id();
	test_obj.set_up_thread_id(proc_id);
	proto_test.handle_sorry_enable_thread_id = def_id;	
	proto_test.handle_sorry_enable_res_tag = l7vs::protocol_module_base::FINALIZE;
	BOOST_CHECK(proto_test.handle_sorry_enable_thread_id != proc_id);
	test_obj.test_call();
	BOOST_CHECK(proto_test.handle_sorry_enable_thread_id == proc_id);
	
	// unit_test [3] up_thread_sorry_enable_event not fond function error check
	std::cout << "[3] up_thread_sorry_enable_event not fond function error check" << std::endl;
	test_obj.up_thread_function_array_clear();
	proto_test.handle_sorry_enable_res_tag = l7vs::protocol_module_base::FINALIZE;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(67,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	
	// unit_test [4] up_thread_sorry_enable_event returnd illegal EVENT_TAG error check
	std::cout << "[4] up_thread_sorry_enable_event returnd illegal EVENT_TAG error check" << std::endl;
	test_obj.up_thread_module_event_map_clear();
	proto_test.handle_sorry_enable_res_tag = l7vs::protocol_module_base::FINALIZE;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(66,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	
	mutex_lock_test test_lock_obj(vs,io);
	test_lock_obj.set_up_thread_sorry_enable_event_test();
	test_lock_obj.set_protocol_module((l7vs::protocol_module_base*)&proto_test);
	
	test_lock_obj.test_thread_wait.lock();
	test_lock_obj.befor_thread_id = proc_id;
	test_lock_obj.after_thread_id = proc_id;
	test_lock_obj.mutex_lock();
	
	boost::thread test_thread(boost::bind(&mutex_lock_test::test_run,&test_lock_obj));
	
	BOOST_CHECK(test_lock_obj.befor_thread_id == proc_id);
	BOOST_CHECK(test_lock_obj.after_thread_id == proc_id);
	
	boost::thread::id test_id = test_thread.get_id();
	
	BOOST_CHECK(test_id != proc_id);
	
	// test start
	test_lock_obj.test_thread_wait.unlock();
	sleep(1);
	
	// unit_test [5] up_thread_sorry_enable_event thread block test (mutex lock)
	std::cout << "[5] up_thread_sorry_enable_event thread block test (mutex lock)" << std::endl;
	BOOST_CHECK(test_lock_obj.befor_thread_id == test_id);
	BOOST_CHECK(test_lock_obj.after_thread_id == proc_id);
	
	test_lock_obj.mutex_unlock();
	sleep(1);
	
	// unit_test [6] up_thread_sorry_enable_event thread run test (mutex unlock)
	std::cout << "[6] up_thread_sorry_enable_event thread run test (mutex unlock)" << std::endl;
	BOOST_CHECK(test_lock_obj.befor_thread_id == test_id);
	BOOST_CHECK(test_lock_obj.after_thread_id == test_id);
	
	BOOST_MESSAGE( "----- up_thread_sorry_enable_event test end -----" );
	
}
// up_thread_sorry_disable_event test
// up_thread_sorry_disable_event test class
class up_thread_sorry_disable_event_test_class : public module_event_map_test_base_class{
	public:
		up_thread_sorry_disable_event_test_class(l7vs::virtualservice_tcp& vs,boost::asio::io_service& session_io) : module_event_map_test_base_class(vs,session_io){
		};
		
		~up_thread_sorry_disable_event_test_class(){};
		
		void test_call(){
			l7vs::tcp_session::up_thread_sorry_disable_event(LOCAL_PROC);
		};
};

void up_thread_sorry_disable_event_test(){
	
	BOOST_MESSAGE( "----- up_thread_sorry_disable_event test start -----" );
	
	boost::asio::io_service io;
	
	l7vs::virtualservice_tcp vs;	
	
	up_thread_sorry_disable_event_test_class test_obj(vs,io);
	
	std::string test_protocol_name("test protocol");
	l7vs::test_protocol_module proto_test(test_protocol_name);

	test_obj.set_protocol_module((l7vs::protocol_module_base*)&proto_test);
	
	l7vs::protocol_module_base::EVENT_TAG chek_event[13];
	chek_event[0] = l7vs::protocol_module_base::ACCEPT;
	chek_event[1] = l7vs::protocol_module_base::CLIENT_DISCONNECT;
	chek_event[2] = l7vs::protocol_module_base::CLIENT_RECV;
	chek_event[3] = l7vs::protocol_module_base::CLIENT_RESPONSE_SEND;
	chek_event[4] = l7vs::protocol_module_base::REALSERVER_SELECT;
	chek_event[5] = l7vs::protocol_module_base::REALSERVER_CONNECT;
	chek_event[6] = l7vs::protocol_module_base::REALSERVER_SEND;
	chek_event[7] = l7vs::protocol_module_base::REALSERVER_DISCONNECT;
	chek_event[8] = l7vs::protocol_module_base::SORRYSERVER_SELECT;
	chek_event[9] = l7vs::protocol_module_base::SORRYSERVER_CONNECT;
	chek_event[10] = l7vs::protocol_module_base::SORRYSERVER_SEND;
	chek_event[11] = l7vs::protocol_module_base::SORRYSERVER_DISCONNECT;
	chek_event[12] = l7vs::protocol_module_base::FINALIZE;
	
	// unit_test [1] up_thread_sorry_disable_event up_thread_next_call_function update check
	std::cout << "[1] up_thread_sorry_disable_event up_thread_next_call_function update check" << std::endl;
	for(int i = 0; i < 13;i++){
		proto_test.handle_sorry_disable_res_tag = chek_event[i];
		test_obj.test_call();
		BOOST_CHECK(test_obj.chk_up_thread_next_call_function(chek_event[i]));
	}
	
	// unit_test [2] up_thread_sorry_disable_event module parameter check thread id
	std::cout << "[2] up_thread_sorry_disable_event module parameter check thread id" << std::endl;
	boost::thread::id def_id;
	boost::thread::id proc_id = boost::this_thread::get_id();
	test_obj.set_up_thread_id(proc_id);
	proto_test.handle_sorry_disable_thread_id = def_id;	
	proto_test.handle_sorry_disable_res_tag = l7vs::protocol_module_base::FINALIZE;
	BOOST_CHECK(proto_test.handle_sorry_disable_thread_id != proc_id);
	test_obj.test_call();
	BOOST_CHECK(proto_test.handle_sorry_disable_thread_id == proc_id);
	
	// unit_test [3] up_thread_sorry_disable_event not fond function error check
	std::cout << "[3] up_thread_sorry_disable_event not fond function error check" << std::endl;
	test_obj.up_thread_function_array_clear();
	proto_test.handle_sorry_disable_res_tag = l7vs::protocol_module_base::FINALIZE;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(69,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	
	// unit_test [4] up_thread_sorry_disable_event returnd illegal EVENT_TAG error check
	std::cout << "[4] up_thread_sorry_disable_event returnd illegal EVENT_TAG error check" << std::endl;
	test_obj.up_thread_module_event_map_clear();
	proto_test.handle_sorry_disable_res_tag = l7vs::protocol_module_base::FINALIZE;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(68,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	
	mutex_lock_test test_lock_obj(vs,io);
	test_lock_obj.set_up_thread_sorry_disable_event_test();
	test_lock_obj.set_protocol_module((l7vs::protocol_module_base*)&proto_test);
	
	test_lock_obj.test_thread_wait.lock();
	test_lock_obj.befor_thread_id = proc_id;
	test_lock_obj.after_thread_id = proc_id;
	test_lock_obj.mutex_lock();
	
	boost::thread test_thread(boost::bind(&mutex_lock_test::test_run,&test_lock_obj));
	
	BOOST_CHECK(test_lock_obj.befor_thread_id == proc_id);
	BOOST_CHECK(test_lock_obj.after_thread_id == proc_id);
	
	boost::thread::id test_id = test_thread.get_id();
	
	BOOST_CHECK(test_id != proc_id);
	
	// test start
	test_lock_obj.test_thread_wait.unlock();
	sleep(1);
	
	// unit_test [5] up_thread_sorry_disable_event thread block test (mutex lock)
	std::cout << "[5] up_thread_sorry_disable_event thread block test (mutex lock)" << std::endl;
	BOOST_CHECK(test_lock_obj.befor_thread_id == test_id);
	BOOST_CHECK(test_lock_obj.after_thread_id == proc_id);
	
	test_lock_obj.mutex_unlock();
	sleep(1);
	
	// unit_test [6] up_thread_sorry_disable_event thread run test (mutex unlock)
	std::cout << "[6] up_thread_sorry_disable_event thread run test (mutex unlock)" << std::endl;
	BOOST_CHECK(test_lock_obj.befor_thread_id == test_id);
	BOOST_CHECK(test_lock_obj.after_thread_id == test_id);
	
	BOOST_MESSAGE( "----- up_thread_sorry_disable_event test end -----" );
	
}

// down_thread_sorry_enable_event test
// dwon_thread_sorry_enable_event test class
class down_thread_sorry_enable_event_test_class : public module_event_map_test_base_class{
	public:
		down_thread_sorry_enable_event_test_class(l7vs::virtualservice_tcp& vs,boost::asio::io_service& session_io) : module_event_map_test_base_class(vs,session_io){
		};
		
		~down_thread_sorry_enable_event_test_class(){};
		
		void test_call(){
			l7vs::tcp_session::down_thread_sorry_enable_event(LOCAL_PROC);
		};
};

void down_thread_sorry_enable_event_test(){
	
	BOOST_MESSAGE( "----- down_thread_sorry_enable_event test start -----" );
	
	boost::asio::io_service io;
	
	l7vs::virtualservice_tcp vs;	
	
	down_thread_sorry_enable_event_test_class test_obj(vs,io);
	
	std::string test_protocol_name("test protocol");
	l7vs::test_protocol_module proto_test(test_protocol_name);

	test_obj.set_protocol_module((l7vs::protocol_module_base*)&proto_test);
	
	l7vs::protocol_module_base::EVENT_TAG chek_event[7];
	chek_event[0] = l7vs::protocol_module_base::CLIENT_DISCONNECT;
	chek_event[1] = l7vs::protocol_module_base::CLIENT_CONNECTION_CHECK;
	chek_event[2] = l7vs::protocol_module_base::REALSERVER_RECV;
	chek_event[3] = l7vs::protocol_module_base::REALSERVER_DISCONNECT;
	chek_event[4] = l7vs::protocol_module_base::SORRYSERVER_RECV;
	chek_event[5] = l7vs::protocol_module_base::SORRYSERVER_DISCONNECT;
	chek_event[6] = l7vs::protocol_module_base::FINALIZE;
	
	// unit_test [1] down_thread_sorry_enable_event up_thread_next_call_function update check
	std::cout << "[1] down_thread_sorry_enable_event up_thread_next_call_function update check" << std::endl;
	for(int i = 0; i < 7;i++){
		proto_test.handle_sorry_enable_res_tag = chek_event[i];
		test_obj.test_call();
		BOOST_CHECK(test_obj.chk_down_thread_next_call_function(chek_event[i]));
	}
	
	// unit_test [2] down_thread_sorry_enable_event module parameter check thread id
	std::cout << "[2] down_thread_sorry_enable_event module parameter check thread id" << std::endl;
	boost::thread::id def_id;
	boost::thread::id proc_id = boost::this_thread::get_id();
	test_obj.set_down_thread_id(proc_id);
	proto_test.handle_sorry_enable_thread_id = def_id;	
	proto_test.handle_sorry_enable_res_tag = l7vs::protocol_module_base::FINALIZE;
	BOOST_CHECK(proto_test.handle_sorry_enable_thread_id != proc_id);
	test_obj.test_call();
	BOOST_CHECK(proto_test.handle_sorry_enable_thread_id == proc_id);
	
	// unit_test [3] down_thread_sorry_enable_event not fond function error check
	std::cout << "[3] down_thread_sorry_enable_event not fond function error check" << std::endl;
	test_obj.down_thread_function_array_clear();
	proto_test.handle_sorry_enable_res_tag = l7vs::protocol_module_base::FINALIZE;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(97,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	
	// unit_test [4] down_thread_sorry_enable_event returnd illegal EVENT_TAG error check
	std::cout << "[4] down_thread_sorry_enable_event returnd illegal EVENT_TAG error check" << std::endl;
	test_obj.down_thread_module_event_map_clear();
	proto_test.handle_sorry_enable_res_tag = l7vs::protocol_module_base::FINALIZE;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(96,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	
	mutex_lock_test test_lock_obj(vs,io);
	test_lock_obj.set_down_thread_sorry_enable_event_test();
	test_lock_obj.set_protocol_module((l7vs::protocol_module_base*)&proto_test);
	
	
	test_lock_obj.test_thread_wait.lock();
	test_lock_obj.befor_thread_id = proc_id;
	test_lock_obj.after_thread_id = proc_id;
	test_lock_obj.mutex_lock();
	
	boost::thread test_thread(boost::bind(&mutex_lock_test::test_run,&test_lock_obj));
	
	BOOST_CHECK(test_lock_obj.befor_thread_id == proc_id);
	BOOST_CHECK(test_lock_obj.after_thread_id == proc_id);
	
	boost::thread::id test_id = test_thread.get_id();
	
	BOOST_CHECK(test_id != proc_id);
	
	// test start
	test_lock_obj.test_thread_wait.unlock();
	sleep(1);
	
	// unit_test [5] down_thread_sorry_enable_event thread block test (mutex lock)
	std::cout << "[5] down_thread_sorry_enable_event thread block test (mutex lock)" << std::endl;
	BOOST_CHECK(test_lock_obj.befor_thread_id == test_id);
	BOOST_CHECK(test_lock_obj.after_thread_id == proc_id);
	
	test_lock_obj.mutex_unlock();
	sleep(1);
	
	// unit_test [6] down_thread_sorry_enable_event thread run test (mutex unlock)
	std::cout << "[6] down_thread_sorry_enable_event thread run test (mutex unlock)" << std::endl;
	BOOST_CHECK(test_lock_obj.befor_thread_id == test_id);
	BOOST_CHECK(test_lock_obj.after_thread_id == test_id);
	
	BOOST_MESSAGE( "----- down_thread_sorry_enable_event test end -----" );
}
// down_thread_sorry_disable_event test
// dwon_thread_sorry_disable_event test class
class down_thread_sorry_disable_event_test_class : public module_event_map_test_base_class{
	public:
		down_thread_sorry_disable_event_test_class(l7vs::virtualservice_tcp& vs,boost::asio::io_service& session_io) : module_event_map_test_base_class(vs,session_io){
		};
		
		~down_thread_sorry_disable_event_test_class(){};
		
		void test_call(){
			l7vs::tcp_session::down_thread_sorry_disable_event(LOCAL_PROC);
		};
};

void down_thread_sorry_disable_event_test(){
	
	BOOST_MESSAGE( "----- down_thread_sorry_disable_event test start -----" );
	
	boost::asio::io_service io;
	
	l7vs::virtualservice_tcp vs;	
	
	down_thread_sorry_disable_event_test_class test_obj(vs,io);
	
	std::string test_protocol_name("test protocol");
	l7vs::test_protocol_module proto_test(test_protocol_name);

	test_obj.set_protocol_module((l7vs::protocol_module_base*)&proto_test);
	
	l7vs::protocol_module_base::EVENT_TAG chek_event[7];
	chek_event[0] = l7vs::protocol_module_base::CLIENT_DISCONNECT;
	chek_event[1] = l7vs::protocol_module_base::CLIENT_CONNECTION_CHECK;
	chek_event[2] = l7vs::protocol_module_base::REALSERVER_RECV;
	chek_event[3] = l7vs::protocol_module_base::REALSERVER_DISCONNECT;
	chek_event[4] = l7vs::protocol_module_base::SORRYSERVER_RECV;
	chek_event[5] = l7vs::protocol_module_base::SORRYSERVER_DISCONNECT;
	chek_event[6] = l7vs::protocol_module_base::FINALIZE;
	
	// unit_test [1] down_thread_sorry_disable_event up_thread_next_call_function update check
	std::cout << "[1] down_thread_sorry_disable_event up_thread_next_call_function update check" << std::endl;
	for(int i = 0; i < 7;i++){
		proto_test.handle_sorry_disable_res_tag = chek_event[i];
		test_obj.test_call();
		BOOST_CHECK(test_obj.chk_down_thread_next_call_function(chek_event[i]));
	}
	
	// unit_test [2] down_thread_sorry_disable_event module parameter check thread id
	std::cout << "[2] down_thread_sorry_disable_event module parameter check thread id" << std::endl;
	boost::thread::id def_id;
	boost::thread::id proc_id = boost::this_thread::get_id();
	test_obj.set_down_thread_id(proc_id);
	proto_test.handle_sorry_disable_thread_id = def_id;	
	proto_test.handle_sorry_disable_res_tag = l7vs::protocol_module_base::FINALIZE;
	BOOST_CHECK(proto_test.handle_sorry_disable_thread_id != proc_id);
	test_obj.test_call();
	BOOST_CHECK(proto_test.handle_sorry_disable_thread_id == proc_id);
	
	// unit_test [3] down_thread_sorry_disable_event not fond function error check
	std::cout << "[3] down_thread_sorry_disable_event not fond function error check" << std::endl;
	test_obj.down_thread_function_array_clear();
	proto_test.handle_sorry_disable_res_tag = l7vs::protocol_module_base::FINALIZE;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(99,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	
	// unit_test [4] down_thread_sorry_disable_event returnd illegal EVENT_TAG error check
	std::cout << "[4] down_thread_sorry_disable_event returnd illegal EVENT_TAG error check" << std::endl;
	test_obj.down_thread_module_event_map_clear();
	proto_test.handle_sorry_disable_res_tag = l7vs::protocol_module_base::FINALIZE;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(98,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	
	mutex_lock_test test_lock_obj(vs,io);
	test_lock_obj.set_down_thread_sorry_disable_event_test();
	test_lock_obj.set_protocol_module((l7vs::protocol_module_base*)&proto_test);
	
	test_lock_obj.test_thread_wait.lock();
	test_lock_obj.befor_thread_id = proc_id;
	test_lock_obj.after_thread_id = proc_id;
	test_lock_obj.mutex_lock();
	
	boost::thread test_thread(boost::bind(&mutex_lock_test::test_run,&test_lock_obj));
	
	BOOST_CHECK(test_lock_obj.befor_thread_id == proc_id);
	BOOST_CHECK(test_lock_obj.after_thread_id == proc_id);
	
	boost::thread::id test_id = test_thread.get_id();
	
	BOOST_CHECK(test_id != proc_id);
	
	// test start
	test_lock_obj.test_thread_wait.unlock();
	sleep(1);
	
	// unit_test [5] down_thread_sorry_disable_event thread block test (mutex lock)
	std::cout << "[5] down_thread_sorry_disable_event thread block test (mutex lock)" << std::endl;
	BOOST_CHECK(test_lock_obj.befor_thread_id == test_id);
	BOOST_CHECK(test_lock_obj.after_thread_id == proc_id);
	
	test_lock_obj.mutex_unlock();
	sleep(1);
	
	// unit_test [6] down_thread_sorry_disable_event thread run test (mutex unlock)
	std::cout << "[6] down_thread_sorry_disable_event thread run test (mutex unlock)" << std::endl;
	BOOST_CHECK(test_lock_obj.befor_thread_id == test_id);
	BOOST_CHECK(test_lock_obj.after_thread_id == test_id);
	
	BOOST_MESSAGE( "----- down_thread_sorry_disable_event test end -----" );
}

// up_thread_client_accept_event test
// up_thread_client_accept_event test class
class up_thread_client_accept_event_test_class : public module_event_map_test_base_class{
	public:
		up_thread_client_accept_event_test_class(l7vs::virtualservice_tcp& vs,boost::asio::io_service& session_io) : module_event_map_test_base_class(vs,session_io){
		};
		
		~up_thread_client_accept_event_test_class(){};
		
		void test_call(){
			l7vs::tcp_session::up_thread_client_accept_event(LOCAL_PROC);
		};
};

void up_thread_client_accept_event_test(){
	
	BOOST_MESSAGE( "----- up_thread_client_accept_event test start -----" );
	
	boost::asio::io_service io;
	
	l7vs::virtualservice_tcp vs;	
	
	up_thread_client_accept_event_test_class test_obj(vs,io);
	
	std::string test_protocol_name("test protocol");
	l7vs::test_protocol_module proto_test(test_protocol_name);

	test_obj.set_protocol_module((l7vs::protocol_module_base*)&proto_test);
	
	l7vs::protocol_module_base::EVENT_TAG chek_event[13];
	chek_event[0] = l7vs::protocol_module_base::ACCEPT;
	chek_event[1] = l7vs::protocol_module_base::CLIENT_DISCONNECT;
	chek_event[2] = l7vs::protocol_module_base::CLIENT_RECV;
	chek_event[3] = l7vs::protocol_module_base::CLIENT_RESPONSE_SEND;
	chek_event[4] = l7vs::protocol_module_base::REALSERVER_SELECT;
	chek_event[5] = l7vs::protocol_module_base::REALSERVER_CONNECT;
	chek_event[6] = l7vs::protocol_module_base::REALSERVER_SEND;
	chek_event[7] = l7vs::protocol_module_base::REALSERVER_DISCONNECT;
	chek_event[8] = l7vs::protocol_module_base::SORRYSERVER_SELECT;
	chek_event[9] = l7vs::protocol_module_base::SORRYSERVER_CONNECT;
	chek_event[10] = l7vs::protocol_module_base::SORRYSERVER_SEND;
	chek_event[11] = l7vs::protocol_module_base::SORRYSERVER_DISCONNECT;
	chek_event[12] = l7vs::protocol_module_base::FINALIZE;
	
	// unit_test [1] up_thread_client_accept_event up_thread_next_call_function update check
	std::cout << "[1] up_thread_client_accept_event up_thread_next_call_function update check" << std::endl;
	for(int i = 0; i < 13;i++){
		proto_test.handle_accept_res_tag = chek_event[i];
		test_obj.test_call();
		BOOST_CHECK(test_obj.chk_up_thread_next_call_function(chek_event[i]));
	}
	
	// unit_test [2] up_thread_client_accept_event module parameter check thread id
	std::cout << "[2] up_thread_client_accept_event module parameter check thread id" << std::endl;
	boost::thread::id def_id;
	boost::thread::id proc_id = boost::this_thread::get_id();
	test_obj.set_up_thread_id(proc_id);
	proto_test.handle_accept_in_thread_id = def_id;	
	proto_test.handle_accept_res_tag = l7vs::protocol_module_base::FINALIZE;
	BOOST_CHECK(proto_test.handle_accept_in_thread_id != proc_id);
	test_obj.test_call();
	BOOST_CHECK(proto_test.handle_accept_in_thread_id == proc_id);
	
	// unit_test [3] up_thread_client_accept_event not fond function error check
	std::cout << "[3] up_thread_client_accept_event not fond function error check" << std::endl;
	test_obj.up_thread_function_array_clear();
	proto_test.handle_accept_res_tag = l7vs::protocol_module_base::FINALIZE;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(19,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	
	// unit_test [4] up_thread_client_accept_event returnd illegal EVENT_TAG error check
	std::cout << "[4] up_thread_client_accept_event returnd illegal EVENT_TAG error check" << std::endl;
	test_obj.up_thread_module_event_map_clear();
	proto_test.handle_accept_res_tag = l7vs::protocol_module_base::FINALIZE;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(18,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	
	BOOST_MESSAGE( "----- up_thread_client_accept_event test end -----" );
	
}

// up_thread_client_respond test
// up_thread_client_respond test class
class up_thread_client_respond_test_class : public l7vs::tcp_session{
	public:
		up_thread_client_respond_test_class(l7vs::virtualservice_tcp& vs,boost::asio::io_service& session_io) : l7vs::tcp_session(vs,session_io){
		};
		~up_thread_client_respond_test_class(){};
		
		void test_call(){
			l7vs::tcp_session::up_thread_client_respond(LOCAL_PROC);
		};
		
		l7vs::lockfree_queue<l7vs::tcp_thread_message>& get_up_thread_message_que(){
			return up_thread_message_que;
		};

		l7vs::lockfree_queue<l7vs::tcp_thread_message>& get_down_thread_message_que(){
			return down_thread_message_que;
		};
		
		void up_thread_function_array_clear(){
			for(int i = 0;i <= UP_FUNC_EXIT;i++){
				up_thread_function_array[i].second = NULL;
			}
		};
		void up_thread_message_down_thread_function_map_clear(){
			up_thread_message_down_thread_function_map.clear();
		};
		
		void up_thread_client_respond_event(const TCP_PROCESS_TYPE_TAG process_type){
			up_thread_client_respond_event_call_chek = true;
		};
		bool up_thread_client_respond_event_call_chek;
		
		void down_thread_client_respond_event(const TCP_PROCESS_TYPE_TAG process_type){
			down_thread_client_respond_event_call_chek = true;
		};
		bool down_thread_client_respond_event_call_chek;
		
		void up_thread_exit(const TCP_PROCESS_TYPE_TAG process_type){
			up_thread_exit_call_chek = true;
		};
		bool up_thread_exit_call_chek;
};
void up_thread_client_respond_test(){
	BOOST_MESSAGE( "----- up_thread_client_respond test start -----" );
	boost::asio::io_service io;
	l7vs::virtualservice_tcp vs;
	up_thread_client_respond_test_class test_obj(vs,io);
	
	l7vs::lockfree_queue<l7vs::tcp_thread_message>&		up_thread_message_que	= test_obj.get_up_thread_message_que();
	l7vs::lockfree_queue<l7vs::tcp_thread_message>&		down_thread_message_que	= test_obj.get_down_thread_message_que();
	
	BOOST_CHECK(up_thread_message_que.empty());
	BOOST_CHECK(down_thread_message_que.empty());
	
	test_obj.test_call();
	
	// unit_test [1] up_thread_client_respond up thread message set check
	std::cout << "[1] up_thread_client_respond up thread message set check" << std::endl;
	BOOST_CHECK(!up_thread_message_que.empty());
	l7vs::tcp_thread_message*	up_msg		= up_thread_message_que.pop();
	BOOST_CHECK(up_thread_message_que.empty());
	test_obj.up_thread_client_respond_event_call_chek = false;
	up_msg->message(l7vs::tcp_session::MESSAGE_PROC);
	BOOST_CHECK(test_obj.up_thread_client_respond_event_call_chek);
	delete	up_msg;

	// unit_test [2] up_thread_client_respond down thread message set check
	std::cout << "[2] up_thread_client_respond down thread message set check" << std::endl;
	BOOST_CHECK(!down_thread_message_que.empty());
	l7vs::tcp_thread_message*	down_msg	= down_thread_message_que.pop();
	BOOST_CHECK(down_thread_message_que.empty());
	test_obj.down_thread_client_respond_event_call_chek = false;
	down_msg->message(l7vs::tcp_session::MESSAGE_PROC);
	BOOST_CHECK(test_obj.down_thread_client_respond_event_call_chek);
	delete	down_msg;

	// unit_test [3] up_thread_client_respond not fond down_thread_client_respond_event function error check
	std::cout << "[3] up_thread_client_respond not fond down_thread_client_respond_event function error check" << std::endl;
	test_obj.up_thread_message_down_thread_function_map_clear();
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.up_thread_exit_call_chek = false;
	test_obj.test_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(23,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	BOOST_CHECK(test_obj.up_thread_exit_call_chek);
	
	// unit_test [4] up_thread_client_respond not fond up_thread_client_respond_event function error check
	std::cout << "[4] up_thread_client_respond not fond up_thread_client_respond_event function error check" << std::endl;
	test_obj.up_thread_function_array_clear();
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.up_thread_exit_call_chek = false;
	test_obj.test_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(22,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	BOOST_CHECK(test_obj.up_thread_exit_call_chek);
	
	BOOST_MESSAGE( "----- up_thread_client_respond test end -----" );
}

// up_thread_client_respond_event test
// up_thread_client_respond_event test class
class up_thread_client_respond_event_test_class : public module_event_map_test_base_class{
	public:
		up_thread_client_respond_event_test_class(l7vs::virtualservice_tcp& vs,boost::asio::io_service& session_io) : module_event_map_test_base_class(vs,session_io){
		};
		
		~up_thread_client_respond_event_test_class(){};
		
		void test_call(){
			l7vs::tcp_session::up_thread_client_respond_event(LOCAL_PROC);
		};
};

void up_thread_client_respond_event_test(){
	
	BOOST_MESSAGE( "----- up_thread_client_respond_event test start -----" );
	
	boost::asio::io_service io;
	
	l7vs::virtualservice_tcp vs;	
	
	up_thread_client_respond_event_test_class test_obj(vs,io);
	
	std::string test_protocol_name("test protocol");
	l7vs::test_protocol_module proto_test(test_protocol_name);

	test_obj.set_protocol_module((l7vs::protocol_module_base*)&proto_test);
	
	l7vs::protocol_module_base::EVENT_TAG chek_event[13];
	chek_event[0] = l7vs::protocol_module_base::ACCEPT;
	chek_event[1] = l7vs::protocol_module_base::CLIENT_DISCONNECT;
	chek_event[2] = l7vs::protocol_module_base::CLIENT_RECV;
	chek_event[3] = l7vs::protocol_module_base::CLIENT_RESPONSE_SEND;
	chek_event[4] = l7vs::protocol_module_base::REALSERVER_SELECT;
	chek_event[5] = l7vs::protocol_module_base::REALSERVER_CONNECT;
	chek_event[6] = l7vs::protocol_module_base::REALSERVER_SEND;
	chek_event[7] = l7vs::protocol_module_base::REALSERVER_DISCONNECT;
	chek_event[8] = l7vs::protocol_module_base::SORRYSERVER_SELECT;
	chek_event[9] = l7vs::protocol_module_base::SORRYSERVER_CONNECT;
	chek_event[10] = l7vs::protocol_module_base::SORRYSERVER_SEND;
	chek_event[11] = l7vs::protocol_module_base::SORRYSERVER_DISCONNECT;
	chek_event[12] = l7vs::protocol_module_base::FINALIZE;
	
	// unit_test [1] up_thread_client_respond_event up_thread_next_call_function update check
	std::cout << "[1] up_thread_client_respond_event up_thread_next_call_function update check" << std::endl;
	for(int i = 0; i < 13;i++){
		proto_test.handle_response_send_inform_res_tag = chek_event[i];
		test_obj.test_call();
		BOOST_CHECK(test_obj.chk_up_thread_next_call_function(chek_event[i]));
	}
	
	// unit_test [2] up_thread_client_respond_event module parameter check thread id
	std::cout << "[2] up_thread_client_respond_event module parameter check thread id" << std::endl;
	boost::thread::id def_id;
	boost::thread::id proc_id = boost::this_thread::get_id();
	test_obj.set_up_thread_id(proc_id);
	proto_test.handle_response_send_inform_thread_id = def_id;	
	proto_test.handle_response_send_inform_res_tag = l7vs::protocol_module_base::FINALIZE;
	BOOST_CHECK(proto_test.handle_response_send_inform_thread_id != proc_id);
	test_obj.test_call();
	BOOST_CHECK(proto_test.handle_response_send_inform_thread_id == proc_id);
	
	// unit_test [3] up_thread_client_respond_event not fond function error check
	std::cout << "[3] up_thread_client_respond_event not fond function error check" << std::endl;
	test_obj.up_thread_function_array_clear();
	proto_test.handle_response_send_inform_res_tag = l7vs::protocol_module_base::FINALIZE;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(25,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	
	// unit_test [4] up_thread_client_respond_event returnd illegal EVENT_TAG error check
	std::cout << "[4] up_thread_client_respond_event returnd illegal EVENT_TAG error check" << std::endl;
	test_obj.up_thread_module_event_map_clear();
	proto_test.handle_response_send_inform_res_tag = l7vs::protocol_module_base::FINALIZE;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(24,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	
	mutex_lock_test test_lock_obj(vs,io);
	test_lock_obj.set_up_thread_client_respond_event_test();
	test_lock_obj.set_protocol_module((l7vs::protocol_module_base*)&proto_test);
	
	test_lock_obj.test_thread_wait.lock();
	test_lock_obj.befor_thread_id = proc_id;
	test_lock_obj.after_thread_id = proc_id;
	test_lock_obj.mutex_lock();
	
	boost::thread test_thread(boost::bind(&mutex_lock_test::test_run,&test_lock_obj));
	
	BOOST_CHECK(test_lock_obj.befor_thread_id == proc_id);
	BOOST_CHECK(test_lock_obj.after_thread_id == proc_id);
	
	boost::thread::id test_id = test_thread.get_id();
	
	BOOST_CHECK(test_id != proc_id);
	
	// test start
	test_lock_obj.test_thread_wait.unlock();
	sleep(1);
	
	// unit_test [5] up_thread_client_respond_event thread block test (mutex lock)
	std::cout << "[5] up_thread_client_respond_event thread block test (mutex lock)" << std::endl;
	BOOST_CHECK(test_lock_obj.befor_thread_id == test_id);
	BOOST_CHECK(test_lock_obj.after_thread_id == proc_id);
	
	test_lock_obj.mutex_unlock();
	sleep(1);
	
	// unit_test [6] up_thread_client_respond_event thread run test (mutex unlock)
	std::cout << "[6] up_thread_client_respond_event thread run test (mutex unlock)" << std::endl;
	BOOST_CHECK(test_lock_obj.befor_thread_id == test_id);
	BOOST_CHECK(test_lock_obj.after_thread_id == test_id);
	
	BOOST_MESSAGE( "----- up_thread_client_respond_event test end -----" );
	
}
// down_thread_client_respond_event test
// dwon_thread_client_respond_event test class
class down_thread_client_respond_event_test_class : public module_event_map_test_base_class{
	public:
		down_thread_client_respond_event_test_class(l7vs::virtualservice_tcp& vs,boost::asio::io_service& session_io) : module_event_map_test_base_class(vs,session_io){
		};
		
		~down_thread_client_respond_event_test_class(){};
		
		void test_call(){
			l7vs::tcp_session::down_thread_client_respond_event(LOCAL_PROC);
		};
};

void down_thread_client_respond_event_test(){
	
	BOOST_MESSAGE( "----- down_thread_client_respond_event test start -----" );
	
	boost::asio::io_service io;
	
	l7vs::virtualservice_tcp vs;	
	
	down_thread_client_respond_event_test_class test_obj(vs,io);
	
	std::string test_protocol_name("test protocol");
	l7vs::test_protocol_module proto_test(test_protocol_name);

	test_obj.set_protocol_module((l7vs::protocol_module_base*)&proto_test);
	
	l7vs::protocol_module_base::EVENT_TAG chek_event[7];
	chek_event[0] = l7vs::protocol_module_base::CLIENT_DISCONNECT;
	chek_event[1] = l7vs::protocol_module_base::CLIENT_CONNECTION_CHECK;
	chek_event[2] = l7vs::protocol_module_base::REALSERVER_RECV;
	chek_event[3] = l7vs::protocol_module_base::REALSERVER_DISCONNECT;
	chek_event[4] = l7vs::protocol_module_base::SORRYSERVER_RECV;
	chek_event[5] = l7vs::protocol_module_base::SORRYSERVER_DISCONNECT;
	chek_event[6] = l7vs::protocol_module_base::FINALIZE;
	
	// unit_test [1] down_thread_client_respond_event down_thread_next_call_function update check
	std::cout << "[1] down_thread_client_respond_event down_thread_next_call_function update check" << std::endl;
	for(int i = 0; i < 7;i++){
		proto_test.handle_response_send_inform_res_tag = chek_event[i];
		test_obj.test_call();
		BOOST_CHECK(test_obj.chk_down_thread_next_call_function(chek_event[i]));
	}
	
	// unit_test [2] down_thread_client_respond_event module parameter check thread id
	std::cout << "[2] down_thread_client_respond_event module parameter check thread id" << std::endl;
	boost::thread::id def_id;
	boost::thread::id proc_id = boost::this_thread::get_id();
	test_obj.set_down_thread_id(proc_id);
	proto_test.handle_response_send_inform_thread_id = def_id;	
	proto_test.handle_response_send_inform_res_tag = l7vs::protocol_module_base::FINALIZE;
	BOOST_CHECK(proto_test.handle_response_send_inform_thread_id != proc_id);
	test_obj.test_call();
	BOOST_CHECK(proto_test.handle_response_send_inform_thread_id == proc_id);
	
	// unit_test [3] down_thread_client_respond_event not fond function error check
	std::cout << "[3] down_thread_client_respond_event not fond function error check" << std::endl;
	test_obj.down_thread_function_array_clear();
	proto_test.handle_response_send_inform_res_tag = l7vs::protocol_module_base::FINALIZE;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(81,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	
	// unit_test [4] down_thread_client_respond_event returnd illegal EVENT_TAG error check
	std::cout << "[4] down_thread_client_respond_event returnd illegal EVENT_TAG error check" << std::endl;
	test_obj.down_thread_module_event_map_clear();
	proto_test.handle_response_send_inform_res_tag = l7vs::protocol_module_base::FINALIZE;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(80,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	
	mutex_lock_test test_lock_obj(vs,io);
	test_lock_obj.set_down_thread_client_respond_event_test();
	test_lock_obj.set_protocol_module((l7vs::protocol_module_base*)&proto_test);
	
	test_lock_obj.test_thread_wait.lock();
	test_lock_obj.befor_thread_id = proc_id;
	test_lock_obj.after_thread_id = proc_id;
	test_lock_obj.mutex_lock();
	
	boost::thread test_thread(boost::bind(&mutex_lock_test::test_run,&test_lock_obj));
	
	BOOST_CHECK(test_lock_obj.befor_thread_id == proc_id);
	BOOST_CHECK(test_lock_obj.after_thread_id == proc_id);
	
	boost::thread::id test_id = test_thread.get_id();
	
	BOOST_CHECK(test_id != proc_id);
	
	// test start
	test_lock_obj.test_thread_wait.unlock();
	sleep(1);
	
	// unit_test [5] down_thread_client_respond_event thread block test (mutex lock)
	std::cout << "[5] down_thread_client_respond_event thread block test (mutex lock)" << std::endl;
	BOOST_CHECK(test_lock_obj.befor_thread_id == test_id);
	BOOST_CHECK(test_lock_obj.after_thread_id == proc_id);
	
	test_lock_obj.mutex_unlock();
	sleep(1);
	
	// unit_test [6] down_thread_client_respond_event thread run test (mutex unlock)
	std::cout << "[6] down_thread_client_respond_event thread run test (mutex unlock)" << std::endl;
	BOOST_CHECK(test_lock_obj.befor_thread_id == test_id);
	BOOST_CHECK(test_lock_obj.after_thread_id == test_id);
	
	BOOST_MESSAGE( "----- down_thread_client_respond_event test end -----" );
}


// up_thread_all_socket_close test
// up_thread_all_socket_close test class
class up_thread_all_socket_close_test_class : public l7vs::tcp_session{
	public:
		up_thread_all_socket_close_test_class(l7vs::virtualservice_tcp& vs,boost::asio::io_service& session_io) : l7vs::tcp_session(vs,session_io){
		};
		~up_thread_all_socket_close_test_class(){};
		
		void test_call(){
			tcp_session::up_thread_all_socket_close();
		};
		
		l7vs::tcp_socket& get_client_socket(){
			return client_socket;
		};
		
		boost::shared_ptr< l7vs::tcp_socket > get_sorry_socket(){
			return sorryserver_socket.second;
		};
		
		std::map<endpoint,tcp_socket_ptr>& get_up_thread_send_realserver_socket_map(){
			return up_thread_send_realserver_socket_map;
		};
		
		l7vs::tcp_realserver_connect_socket_list& get_down_thread_connect_socket_list(){
			return down_thread_connect_socket_list;
		}
};
void up_thread_all_socket_close_test(){
	BOOST_MESSAGE( "----- up_thread_all_socket_close test start -----" );
	boost::asio::io_service io;
	l7vs::virtualservice_tcp vs;
	up_thread_all_socket_close_test_class test_obj(vs,io);
	l7vs::tcp_socket& client_socket = test_obj.get_client_socket();
	client_socket.close_call_check = false;
	l7vs::tcp_socket& sorry_socket = *(test_obj.get_sorry_socket());
	sorry_socket.close_call_check = false;
	std::map< boost::asio::ip::tcp::endpoint, boost::shared_ptr< l7vs::tcp_socket > >& up_thread_send_realserver_socket_map = test_obj.get_up_thread_send_realserver_socket_map();
	std::pair< boost::asio::ip::tcp::endpoint, boost::shared_ptr< l7vs::tcp_socket > > pair_socket;
	std::list< std::pair< boost::asio::ip::tcp::endpoint, boost::shared_ptr< l7vs::tcp_socket > > > rs_socket_list;
	std::list< std::pair< boost::asio::ip::tcp::endpoint, boost::shared_ptr< l7vs::tcp_socket > > >::iterator rs_list_it;
	pair_socket.first.address(boost::asio::ip::address::from_string(DUMMI_SERVER_IP));
	vs.connection_inactive_list.clear();
	std::list<boost::asio::ip::tcp::endpoint>::iterator inact_list_it;
	l7vs::tcp_realserver_connect_socket_list& connect_socket_list = test_obj.get_down_thread_connect_socket_list();
	
	for(int i = 0; i < 1024;i++){
		pair_socket.first.port(i);
		boost::shared_ptr< l7vs::tcp_socket > new_socket(new l7vs::tcp_socket(io));
		pair_socket.second = new_socket;
		new_socket->close_call_check = false;
		new_socket->close_res = true;
		rs_socket_list.push_back(pair_socket);
		up_thread_send_realserver_socket_map.insert(pair_socket);
		connect_socket_list.push_back(pair_socket);
	}
	BOOST_CHECK(!up_thread_send_realserver_socket_map.empty());
	BOOST_CHECK(!connect_socket_list.empty());
	BOOST_CHECK(!rs_socket_list.empty());
	
	test_obj.test_call();
	
	// unit_test [1] up_thread_all_socket_close realserver socket close call check
	std::cout << "[1] up_thread_all_socket_close realserver socket close call check" << std::endl;
	rs_list_it = rs_socket_list.begin();
	for(int i = 0; i < 1024;i++){
		BOOST_CHECK(rs_list_it->second->close_call_check);
		rs_list_it++;
	}
	
	// unit_test [2] up_thread_all_socket_close parent virtual service connection incactive call check
	std::cout << "[2] up_thread_all_socket_close parent virtual service connection incactive call check" << std::endl;
	rs_list_it = rs_socket_list.begin();
	for(int i = 0; i < 1024;i++){
		inact_list_it = vs.connection_inactive_list.begin();
		while(inact_list_it != vs.connection_inactive_list.end()){
			if(*inact_list_it == rs_list_it->first)
				break;
			inact_list_it++;
		}
		BOOST_CHECK(inact_list_it != vs.connection_inactive_list.end());
		vs.connection_inactive_list.erase(inact_list_it);
		rs_list_it++;
	}
	BOOST_CHECK(vs.connection_inactive_list.empty());
	
	// unit_test [3] up_thread_all_socket_close realserver send socket map clear check
	std::cout << "[3] up_thread_all_socket_close realserver send socket map clear check" << std::endl;
	BOOST_CHECK(up_thread_send_realserver_socket_map.empty());
	
	// unit_test [4] up_thread_all_socket_close down thread connect socket list clear check
	std::cout << "[4] up_thread_all_socket_close down thread connect socket list clear check" << std::endl;
	BOOST_CHECK(connect_socket_list.empty());
	
	// unit_test [5] up_thread_all_socket_close client socket close call check
	std::cout << "[5] up_thread_all_socket_close client socket close call check" << std::endl;
	BOOST_CHECK(client_socket.close_call_check);
	
	// unit_test [6] up_thread_all_socket_close sorry socket close call check
	std::cout << "[6] up_thread_all_socket_close sorry socket close call check" << std::endl;
	BOOST_CHECK(sorry_socket.close_call_check);
	
	
	rs_list_it = rs_socket_list.begin();
	for(int i = 0; i < 1024;i++){
		rs_list_it->second->close_res = false;
		up_thread_send_realserver_socket_map.insert(*rs_list_it);
		rs_list_it++;
	}
	
	test_obj.test_call();

	// unit_test [7] up_thread_all_socket_close closed socket parent virtual service connection incactive not call check
	std::cout << "[7] up_thread_all_socket_close closed socket parent virtual service connection incactive not call check" << std::endl;
	BOOST_CHECK(vs.connection_inactive_list.empty());
	
	BOOST_MESSAGE( "----- up_thread_all_socket_close test end -----" );
}
// down_thread_all_socket_close test
// down_thread_all_socket_close test class
class down_thread_all_socket_close_test_class : public l7vs::tcp_session{
	public:
		down_thread_all_socket_close_test_class(l7vs::virtualservice_tcp& vs,boost::asio::io_service& session_io) : l7vs::tcp_session(vs,session_io){
		};
		~down_thread_all_socket_close_test_class(){};
		
		void test_call(){
			tcp_session::down_thread_all_socket_close();
		};
		
		l7vs::tcp_socket& get_client_socket(){
			return client_socket;
		};
		
		boost::shared_ptr< l7vs::tcp_socket > get_sorry_socket(){
			return sorryserver_socket.second;
		};
		
		std::list<socket_element>& get_down_thread_receive_realserver_socket_list(){
			return down_thread_receive_realserver_socket_list;
		};
};
void down_thread_all_socket_close_test(){
	BOOST_MESSAGE( "----- down_thread_all_socket_close test start -----" );
	boost::asio::io_service io;
	l7vs::virtualservice_tcp vs;
	down_thread_all_socket_close_test_class test_obj(vs,io);
	l7vs::tcp_socket& client_socket = test_obj.get_client_socket();
	client_socket.close_call_check = false;
	l7vs::tcp_socket& sorry_socket = *(test_obj.get_sorry_socket());
	sorry_socket.close_call_check = false;
	std::list< std::pair< boost::asio::ip::tcp::endpoint, boost::shared_ptr< l7vs::tcp_socket > > >& down_thread_receive_realserver_socket_list = test_obj.get_down_thread_receive_realserver_socket_list();
	std::pair< boost::asio::ip::tcp::endpoint, boost::shared_ptr< l7vs::tcp_socket > > pair_socket;
	std::list< std::pair< boost::asio::ip::tcp::endpoint, boost::shared_ptr< l7vs::tcp_socket > > > rs_socket_list;
	std::list< std::pair< boost::asio::ip::tcp::endpoint, boost::shared_ptr< l7vs::tcp_socket > > >::iterator rs_list_it;
	pair_socket.first.address(boost::asio::ip::address::from_string(DUMMI_SERVER_IP));
	vs.connection_inactive_list.clear();
	std::list<boost::asio::ip::tcp::endpoint>::iterator inact_list_it;
	
	for(int i = 0; i < 1024;i++){
		pair_socket.first.port(i);
		boost::shared_ptr< l7vs::tcp_socket > new_socket(new l7vs::tcp_socket(io));
		pair_socket.second = new_socket;
		new_socket->close_call_check = false;
		new_socket->close_res = true;
		rs_socket_list.push_back(pair_socket);
		down_thread_receive_realserver_socket_list.push_back(pair_socket);
	}
	BOOST_CHECK(!down_thread_receive_realserver_socket_list.empty());
	BOOST_CHECK(!rs_socket_list.empty());
	
	test_obj.test_call();
	
	// unit_test [1] down_thread_all_socket_close realserver socket close call check
	std::cout << "[1] down_thread_all_socket_close realserver socket close call check" << std::endl;
	rs_list_it = rs_socket_list.begin();
	for(int i = 0; i < 1024;i++){
		BOOST_CHECK(rs_list_it->second->close_call_check);
		rs_list_it++;
	}
	
	// unit_test [2] down_thread_all_socket_close parent virtual service connection incactive call check
	std::cout << "[2] down_thread_all_socket_close parent virtual service connection incactive call check" << std::endl;
	rs_list_it = rs_socket_list.begin();
	for(int i = 0; i < 1024;i++){
		inact_list_it = vs.connection_inactive_list.begin();
		while(inact_list_it != vs.connection_inactive_list.end()){
			if(*inact_list_it == rs_list_it->first)
				break;
			inact_list_it++;
		}
		BOOST_CHECK(inact_list_it != vs.connection_inactive_list.end());
		vs.connection_inactive_list.erase(inact_list_it);
		rs_list_it++;
	}
	BOOST_CHECK(vs.connection_inactive_list.empty());
	
	// unit_test [3] down_thread_all_socket_close realserver receive socket list clear check
	std::cout << "[3] down_thread_all_socket_close realserver receive socket list clear check" << std::endl;
	BOOST_CHECK(down_thread_receive_realserver_socket_list.empty());
	
	// unit_test [4] up_thread_all_socket_close client socket close call check
	std::cout << "[4] up_thread_all_socket_close client socket close call check" << std::endl;
	BOOST_CHECK(client_socket.close_call_check);
	
	// unit_test [5] up_thread_all_socket_close sorry socket close call check
	std::cout << "[5] up_thread_all_socket_close sorry socket close call check" << std::endl;
	BOOST_CHECK(sorry_socket.close_call_check);
	
	rs_list_it = rs_socket_list.begin();
	for(int i = 0; i < 1024;i++){
		rs_list_it->second->close_res = false;
		down_thread_receive_realserver_socket_list.push_back(*rs_list_it);
		rs_list_it++;
	}
	
	test_obj.test_call();

	// unit_test [6] down_thread_all_socket_close closed socket parent virtual service connection incactive not call check
	std::cout << "[6] down_thread_all_socket_close closed socket parent virtual service connection incactive not call check" << std::endl;
	BOOST_CHECK(vs.connection_inactive_list.empty());
	
	BOOST_MESSAGE( "----- down_thread_all_socket_close test end -----" );
}

// up_thread_client_disconnect test
// up_thread_client_disconnect test class
class up_thread_client_disconnect : public l7vs::tcp_session{
	public:
		up_thread_client_disconnect(l7vs::virtualservice_tcp& vs,boost::asio::io_service& session_io) : l7vs::tcp_session(vs,session_io){
		};
		~up_thread_client_disconnect(){};
		
		void test_call(){
			l7vs::tcp_session::up_thread_client_disconnect(LOCAL_PROC);
		};
		
		l7vs::lockfree_queue<l7vs::tcp_thread_message>&	get_up_thread_message_que(){
			return up_thread_message_que;
		};

		l7vs::lockfree_queue<l7vs::tcp_thread_message>&	get_down_thread_message_que(){
			return down_thread_message_que;
		};
		
		void up_thread_function_array_clear(){
			for(int i = 0;i <= UP_FUNC_EXIT;i++){
				up_thread_function_array[i].second = NULL;
			}
		};
		void up_thread_message_down_thread_function_map_clear(){
			up_thread_message_down_thread_function_map.clear();
		};
		
		void up_thread_client_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type){
			up_thread_client_disconnect_event_call_chek = true;
		};
		bool up_thread_client_disconnect_event_call_chek;
		
		void down_thread_client_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type){
			down_thread_client_disconnect_event_call_chek = true;
		};
		bool down_thread_client_disconnect_event_call_chek;
		
		void up_thread_exit(const TCP_PROCESS_TYPE_TAG process_type){
			up_thread_exit_call_chek = true;
		};
		bool up_thread_exit_call_chek;
		
		l7vs::tcp_socket& get_client_socket(){
			return client_socket;
		};

};
void up_thread_client_disconnect_test(){
	BOOST_MESSAGE( "----- up_thread_client_disconnect test start -----" );
	boost::asio::io_service io;
	l7vs::virtualservice_tcp vs;
	up_thread_client_disconnect test_obj(vs,io);
	
	l7vs::lockfree_queue<l7vs::tcp_thread_message>&		up_thread_message_que	= test_obj.get_up_thread_message_que();
	l7vs::lockfree_queue<l7vs::tcp_thread_message>&		down_thread_message_que	= test_obj.get_down_thread_message_que();
	
	BOOST_CHECK(up_thread_message_que.empty());
	BOOST_CHECK(down_thread_message_que.empty());
	l7vs::tcp_socket& client_socket = test_obj.get_client_socket();
	client_socket.close_res = true;
	client_socket.close_call_check = false;
	
	test_obj.test_call();
	
	// unit_test [1] up_thread_client_disconnect client socket close call check
	std::cout << "[1] up_thread_client_disconnect client socket close call check" << std::endl;
	BOOST_CHECK(client_socket.close_call_check);
	
	// unit_test [2] up_thread_client_disconnect up thread message set check
	std::cout << "[2] up_thread_client_disconnect up thread message set check" << std::endl;
	BOOST_CHECK(!up_thread_message_que.empty());
	l7vs::tcp_thread_message*	up_msg		= up_thread_message_que.pop();
	BOOST_CHECK(up_thread_message_que.empty());
	test_obj.up_thread_client_disconnect_event_call_chek = false;
	up_msg->message(l7vs::tcp_session::MESSAGE_PROC);
	BOOST_CHECK(test_obj.up_thread_client_disconnect_event_call_chek);
	delete	up_msg;

	// unit_test [3] up_thread_client_disconnect down thread message set check
	std::cout << "[3] up_thread_client_disconnect down thread message set check" << std::endl;
	BOOST_CHECK(!down_thread_message_que.empty());
	l7vs::tcp_thread_message*	down_msg	= down_thread_message_que.pop();
	BOOST_CHECK(down_thread_message_que.empty());
	test_obj.down_thread_client_disconnect_event_call_chek = false;
	down_msg->message(l7vs::tcp_session::MESSAGE_PROC);
	BOOST_CHECK(test_obj.down_thread_client_disconnect_event_call_chek);
	delete	down_msg;

	// unit_test [4] up_thread_client_disconnect closed client socket not set message check
	std::cout << "[4] up_thread_client_disconnect closed client socket not set message check" << std::endl;
	client_socket.close_res = false;
	test_obj.test_call();
	BOOST_CHECK(up_thread_message_que.empty());
	BOOST_CHECK(down_thread_message_que.empty());
	
	client_socket.close_res = true;
	
	// unit_test [5] up_thread_client_disconnect not fond down_thread_client_disconnect_event function error check
	std::cout << "[5] up_thread_client_disconnect not fond down_thread_client_disconnect_event function error check" << std::endl;
	test_obj.up_thread_message_down_thread_function_map_clear();
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.up_thread_exit_call_chek = false;
	test_obj.test_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(27,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	BOOST_CHECK(test_obj.up_thread_exit_call_chek);
	
	// unit_test [6] up_thread_client_disconnect not fond up_thread_client_disconnect_event function error check
	std::cout << "[6] up_thread_client_disconnect not fond up_thread_client_disconnect_event function error check" << std::endl;
	test_obj.up_thread_function_array_clear();
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.up_thread_exit_call_chek = false;
	test_obj.test_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(26,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	BOOST_CHECK(test_obj.up_thread_exit_call_chek);
	
	BOOST_MESSAGE( "----- up_thread_client_disconnect test end -----" );
}
// down_thread_client_disconnect test
// down_thread_client_disconnect test class
class down_thread_client_disconnect_test_class : public l7vs::tcp_session{
	public:
		down_thread_client_disconnect_test_class(l7vs::virtualservice_tcp& vs,boost::asio::io_service& session_io) : l7vs::tcp_session(vs,session_io){
		};
		~down_thread_client_disconnect_test_class(){};
		
		void test_call(){
			l7vs::tcp_session::down_thread_client_disconnect(LOCAL_PROC);
		};
		
		l7vs::lockfree_queue<l7vs::tcp_thread_message>& get_up_thread_message_que(){
			return up_thread_message_que;
		};

		l7vs::lockfree_queue<l7vs::tcp_thread_message>& get_down_thread_message_que(){
			return down_thread_message_que;
		};
		
		void down_thread_function_array_clear(){
			for(int i = 0; i <= DOWN_FUNC_EXIT; i++){
				down_thread_function_array[i].second = NULL;
			}
		};
		void down_thread_message_up_thread_function_map_clear(){
			down_thread_message_up_thread_function_map.clear();
		};
		
		void up_thread_client_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type){
			up_thread_client_disconnect_event_call_chek = true;
		};
		bool up_thread_client_disconnect_event_call_chek;
		
		void down_thread_client_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type){
			down_thread_client_disconnect_event_call_chek = true;
		};
		bool down_thread_client_disconnect_event_call_chek;
		
		void down_thread_exit(const TCP_PROCESS_TYPE_TAG process_type){
			down_thread_exit_call_chek = true;
		};
		bool down_thread_exit_call_chek;
		
		l7vs::tcp_socket& get_client_socket(){
			return client_socket;
		};

};
void down_thread_client_disconnect_test(){
	BOOST_MESSAGE( "----- down_thread_client_disconnect test start -----" );
	boost::asio::io_service io;
	l7vs::virtualservice_tcp vs;
	down_thread_client_disconnect_test_class test_obj(vs,io);
	
	l7vs::lockfree_queue<l7vs::tcp_thread_message>&		up_thread_message_que	= test_obj.get_up_thread_message_que();
	l7vs::lockfree_queue<l7vs::tcp_thread_message>&		down_thread_message_que	= test_obj.get_down_thread_message_que();
	
	BOOST_CHECK(up_thread_message_que.empty());
	BOOST_CHECK(down_thread_message_que.empty());
	l7vs::tcp_socket& client_socket = test_obj.get_client_socket();
	client_socket.close_res = true;
	client_socket.close_call_check = false;
	
	test_obj.test_call();
	
	// unit_test [1] down_thread_client_disconnect client socket close call check
	std::cout << "[1] down_thread_client_disconnect client socket close call check" << std::endl;
	BOOST_CHECK(client_socket.close_call_check);
	
	// unit_test [2] down_thread_client_disconnect up thread message set check
	std::cout << "[2] down_thread_client_disconnect up thread message set check" << std::endl;
	BOOST_CHECK(!up_thread_message_que.empty());
	l7vs::tcp_thread_message*	up_msg		= up_thread_message_que.pop();
	BOOST_CHECK(up_thread_message_que.empty());
	test_obj.up_thread_client_disconnect_event_call_chek = false;
	up_msg->message(l7vs::tcp_session::MESSAGE_PROC);
	BOOST_CHECK(test_obj.up_thread_client_disconnect_event_call_chek);
	delete	up_msg;

	// unit_test [3] down_thread_client_disconnect down thread message set check
	std::cout << "[3] down_thread_client_disconnect down thread message set check" << std::endl;
	BOOST_CHECK(!down_thread_message_que.empty());
	l7vs::tcp_thread_message*	down_msg	= down_thread_message_que.pop();
	BOOST_CHECK(down_thread_message_que.empty());
	test_obj.down_thread_client_disconnect_event_call_chek = false;
	down_msg->message(l7vs::tcp_session::MESSAGE_PROC);
	BOOST_CHECK(test_obj.down_thread_client_disconnect_event_call_chek);
	delete	down_msg;

	// unit_test [4] down_thread_client_disconnect closed client socket not set message check
	std::cout << "[4] down_thread_client_disconnect closed client socket not set message check" << std::endl;
	client_socket.close_res = false;
	test_obj.test_call();
	BOOST_CHECK(up_thread_message_que.empty());
	BOOST_CHECK(down_thread_message_que.empty());
	
	client_socket.close_res = true;
	
	// unit_test [5] down_thread_client_disconnect not fond up_thread_client_disconnect_event function error check
	std::cout << "[5] down_thread_client_disconnect not fond up_thread_client_disconnect_event function error check" << std::endl;
	test_obj.down_thread_message_up_thread_function_map_clear();
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.down_thread_exit_call_chek = false;
	test_obj.test_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(85,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	BOOST_CHECK(test_obj.down_thread_exit_call_chek);
	
	// unit_test [6] down_thread_client_disconnect not fond down_thread_client_disconnect_event function error check
	std::cout << "[6] down_thread_client_disconnect not fond down_thread_client_disconnect_event function error check" << std::endl;
	test_obj.down_thread_function_array_clear();
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.down_thread_exit_call_chek = false;
	test_obj.test_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(84,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	BOOST_CHECK(test_obj.down_thread_exit_call_chek);
	
	BOOST_MESSAGE( "----- down_thread_client_disconnect test end -----" );
}

// up_thread_sorryserver_disconnect test
// up_thread_sorryserver_disconnect test class
class up_thread_sorryserver_disconnect_test_class : public l7vs::tcp_session{
	public:
		up_thread_sorryserver_disconnect_test_class(l7vs::virtualservice_tcp& vs,boost::asio::io_service& session_io) : l7vs::tcp_session(vs,session_io){
		};
		~up_thread_sorryserver_disconnect_test_class(){};
		
		void test_call(){
			l7vs::tcp_session::up_thread_sorryserver_disconnect(LOCAL_PROC);
		};
		
		l7vs::lockfree_queue<l7vs::tcp_thread_message>& get_up_thread_message_que(){
			return up_thread_message_que;
		};

		l7vs::lockfree_queue<l7vs::tcp_thread_message>& get_down_thread_message_que(){
			return down_thread_message_que;
		};
		
		void up_thread_function_array_clear(){
			for(int i = 0;i <= UP_FUNC_EXIT;i++){
				up_thread_function_array[i].second = NULL;
			}
		};
		void up_thread_message_down_thread_function_map_clear(){
			up_thread_message_down_thread_function_map.clear();
		};
		
		void up_thread_sorryserver_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type){
			up_thread_sorryserver_disconnect_event_call_chek = true;
		};
		bool up_thread_sorryserver_disconnect_event_call_chek;
		
		void down_thread_sorryserver_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type){
			down_thread_sorryserver_disconnect_event_call_chek = true;
		};
		bool down_thread_sorryserver_disconnect_event_call_chek;
		
		void up_thread_exit(const TCP_PROCESS_TYPE_TAG process_type){
			up_thread_exit_call_chek = true;
		};
		bool up_thread_exit_call_chek;
		
		boost::shared_ptr< l7vs::tcp_socket > get_sorry_socket(){
			return sorryserver_socket.second;
		};
		void set_sorry_endpoint(endpoint set_end){
			sorryserver_socket.first = set_end;
		}
};
void up_thread_sorryserver_disconnect_test(){
	BOOST_MESSAGE( "----- up_thread_sorryserver_disconnect test start -----" );
	boost::asio::io_service io;
	l7vs::virtualservice_tcp vs;
	up_thread_sorryserver_disconnect_test_class test_obj(vs,io);
	
	l7vs::lockfree_queue<l7vs::tcp_thread_message>&		up_thread_message_que	= test_obj.get_up_thread_message_que();
	l7vs::lockfree_queue<l7vs::tcp_thread_message>&		down_thread_message_que	= test_obj.get_down_thread_message_que();
	
	BOOST_CHECK(up_thread_message_que.empty());
	BOOST_CHECK(down_thread_message_que.empty());
	l7vs::tcp_socket& sorry_socket = *(test_obj.get_sorry_socket());
	sorry_socket.close_res = true;
	sorry_socket.close_call_check = false;
	boost::asio::ip::tcp::endpoint sorry_end(boost::asio::ip::address::from_string(DUMMI_SERVER_IP), DUMMI_SERVER_PORT);
	test_obj.set_sorry_endpoint(sorry_end);
	
	test_obj.test_call();
	
	// unit_test [1] up_thread_sorryserver_disconnect client socket close call check
	std::cout << "[1] up_thread_sorryserver_disconnect client socket close call check" << std::endl;
	BOOST_CHECK(sorry_socket.close_call_check);
	
	// unit_test [2] up_thread_sorryserver_disconnect up thread message set check
	std::cout << "[2] up_thread_sorryserver_disconnect up thread message set check" << std::endl;
	BOOST_CHECK(!up_thread_message_que.empty());
	l7vs::tcp_thread_message*	up_msg		= up_thread_message_que.pop();
	BOOST_CHECK(up_thread_message_que.empty());
	BOOST_CHECK(up_msg->endpoint_info == sorry_end);
	test_obj.up_thread_sorryserver_disconnect_event_call_chek = false;
	up_msg->message(l7vs::tcp_session::MESSAGE_PROC);
	BOOST_CHECK(test_obj.up_thread_sorryserver_disconnect_event_call_chek);
	delete	up_msg;

	// unit_test [3] up_thread_sorryserver_disconnect down thread message set check
	std::cout << "[3] up_thread_sorryserver_disconnect down thread message set check" << std::endl;
	BOOST_CHECK(!down_thread_message_que.empty());
	l7vs::tcp_thread_message*	down_msg	= down_thread_message_que.pop();
	BOOST_CHECK(down_thread_message_que.empty());
	BOOST_CHECK(down_msg->endpoint_info == sorry_end);
	test_obj.down_thread_sorryserver_disconnect_event_call_chek = false;
	down_msg->message(l7vs::tcp_session::MESSAGE_PROC);
	BOOST_CHECK(test_obj.down_thread_sorryserver_disconnect_event_call_chek);
	delete	down_msg;

	// unit_test [4] up_thread_sorryserver_disconnect closed client socket not set message check
	std::cout << "[4] up_thread_sorryserver_disconnect closed client socket not set message check" << std::endl;
	sorry_socket.close_res = false;
	test_obj.test_call();
	BOOST_CHECK(up_thread_message_que.empty());
	BOOST_CHECK(down_thread_message_que.empty());
	
	sorry_socket.close_res = true;
	
	// unit_test [5] up_thread_sorryserver_disconnect not fond down_thread_sorryserver_disconnect_event function error check
	std::cout << "[5] up_thread_sorryserver_disconnect not fond down_thread_sorryserver_disconnect_event function error check" << std::endl;
	test_obj.up_thread_message_down_thread_function_map_clear();
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.up_thread_exit_call_chek = false;
	test_obj.test_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(61,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	BOOST_CHECK(test_obj.up_thread_exit_call_chek);
	
	// unit_test [6] up_thread_sorryserver_disconnect not fond up_thread_sorryserver_disconnect_event function error check
	std::cout << "[6] up_thread_sorryserver_disconnect not fond up_thread_sorryserver_disconnect_event function error check" << std::endl;
	test_obj.up_thread_function_array_clear();
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.up_thread_exit_call_chek = false;
	test_obj.test_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(60,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	BOOST_CHECK(test_obj.up_thread_exit_call_chek);
	
	BOOST_MESSAGE( "----- up_thread_sorryserver_disconnect test end -----" );
}
// down_thread_sorryserver_disconnect test
// down_thread_sorryserver_disconnect test class
class down_thread_sorryserver_disconnect_test_class : public l7vs::tcp_session{
	public:
		down_thread_sorryserver_disconnect_test_class(l7vs::virtualservice_tcp& vs,boost::asio::io_service& session_io) : l7vs::tcp_session(vs,session_io){
		};
		~down_thread_sorryserver_disconnect_test_class(){};
		
		void test_call(){
			l7vs::tcp_session::down_thread_sorryserver_disconnect(LOCAL_PROC);
		};
		
		l7vs::lockfree_queue<l7vs::tcp_thread_message>& get_up_thread_message_que(){
			return up_thread_message_que;
		};

		l7vs::lockfree_queue<l7vs::tcp_thread_message>& get_down_thread_message_que(){
			return down_thread_message_que;
		};
		
		void down_thread_function_array_clear(){
			for(int i = 0; i <= DOWN_FUNC_EXIT;i++){
				down_thread_function_array[i].second = NULL;
			}
		};
		void down_thread_message_up_thread_function_map_clear(){
			down_thread_message_up_thread_function_map.clear();
		};
		
		void up_thread_sorryserver_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type){
			up_thread_sorryserver_disconnect_event_call_chek = true;
		};
		bool up_thread_sorryserver_disconnect_event_call_chek;
		
		void down_thread_sorryserver_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type){
			down_thread_sorryserver_disconnect_event_call_chek = true;
		};
		bool down_thread_sorryserver_disconnect_event_call_chek;
		
		void down_thread_exit(const TCP_PROCESS_TYPE_TAG process_type){
			down_thread_exit_call_chek = true;
		};
		bool down_thread_exit_call_chek;
		
		boost::shared_ptr< l7vs::tcp_socket > get_sorry_socket(){
			return sorryserver_socket.second;
		};
		void set_sorry_endpoint(endpoint set_end){
			sorryserver_socket.first = set_end;
		}
};
void down_thread_sorryserver_disconnect_test(){
	BOOST_MESSAGE( "----- down_thread_sorryserver_disconnect test start -----" );
	boost::asio::io_service io;
	l7vs::virtualservice_tcp vs;
	down_thread_sorryserver_disconnect_test_class test_obj(vs,io);
	
	l7vs::lockfree_queue<l7vs::tcp_thread_message>& up_thread_message_que = test_obj.get_up_thread_message_que();
	l7vs::lockfree_queue<l7vs::tcp_thread_message>& down_thread_message_que = test_obj.get_down_thread_message_que();
	
	BOOST_CHECK(up_thread_message_que.empty());
	BOOST_CHECK(down_thread_message_que.empty());
	l7vs::tcp_socket& sorry_socket = *(test_obj.get_sorry_socket());
	sorry_socket.close_res = true;
	sorry_socket.close_call_check = false;
	boost::asio::ip::tcp::endpoint sorry_end(boost::asio::ip::address::from_string(DUMMI_SERVER_IP), DUMMI_SERVER_PORT);
	test_obj.set_sorry_endpoint(sorry_end);
	
	test_obj.test_call();
	
	// unit_test [1] down_thread_sorryserver_disconnect client socket close call check
	std::cout << "[1] down_thread_sorryserver_disconnect client socket close call check" << std::endl;
	BOOST_CHECK(sorry_socket.close_call_check);
	
	// unit_test [2] down_thread_sorryserver_disconnect up thread message set check
	std::cout << "[2] down_thread_sorryserver_disconnect up thread message set check" << std::endl;
	BOOST_CHECK(!up_thread_message_que.empty());
	l7vs::tcp_thread_message*	up_msg		= up_thread_message_que.pop();
	BOOST_CHECK(up_thread_message_que.empty());
	BOOST_CHECK(up_msg->endpoint_info == sorry_end);
	test_obj.up_thread_sorryserver_disconnect_event_call_chek = false;
	up_msg->message(l7vs::tcp_session::MESSAGE_PROC);
	BOOST_CHECK(test_obj.up_thread_sorryserver_disconnect_event_call_chek);
	delete	up_msg;

	// unit_test [3] down_thread_sorryserver_disconnect down thread message set check
	std::cout << "[3] down_thread_sorryserver_disconnect down thread message set check" << std::endl;
	BOOST_CHECK(!down_thread_message_que.empty());
	l7vs::tcp_thread_message*	down_msg	= down_thread_message_que.pop();
	BOOST_CHECK(down_thread_message_que.empty());
	BOOST_CHECK(down_msg->endpoint_info == sorry_end);
	test_obj.down_thread_sorryserver_disconnect_event_call_chek = false;
	down_msg->message(l7vs::tcp_session::MESSAGE_PROC);
	BOOST_CHECK(test_obj.down_thread_sorryserver_disconnect_event_call_chek);
	delete	down_msg;

	// unit_test [4] down_thread_sorryserver_disconnect closed client socket not set message check
	std::cout << "[4] down_thread_sorryserver_disconnect closed client socket not set message check" << std::endl;
	sorry_socket.close_res = false;
	test_obj.test_call();
	BOOST_CHECK(up_thread_message_que.empty());
	BOOST_CHECK(down_thread_message_que.empty());
	
	sorry_socket.close_res = true;
	
	// unit_test [5] down_thread_sorryserver_disconnect not fond up_thread_sorryserver_disconnect_event function error check
	std::cout << "[5] down_thread_sorryserver_disconnect not fond up_thread_sorryserver_disconnect_event function error check" << std::endl;
	test_obj.down_thread_message_up_thread_function_map_clear();
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.down_thread_exit_call_chek = false;
	test_obj.test_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(91,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	BOOST_CHECK(test_obj.down_thread_exit_call_chek);
	
	// unit_test [6] down_thread_sorryserver_disconnect not fond down_thread_sorryserver_disconnect_event function error check
	std::cout << "[6] down_thread_sorryserver_disconnect not fond down_thread_sorryserver_disconnect_event function error check" << std::endl;
	test_obj.down_thread_function_array_clear();
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.down_thread_exit_call_chek = false;
	test_obj.test_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(90,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	BOOST_CHECK(test_obj.down_thread_exit_call_chek);
	
	BOOST_MESSAGE( "----- down_thread_sorryserver_disconnect test end -----" );
}

// up_thread_realserver_connect_event test
// up_thread_realserver_connect_event test class
class up_thread_realserver_connect_event_test_class : public module_event_map_test_base_class{
	public:
		up_thread_realserver_connect_event_test_class(l7vs::virtualservice_tcp& vs,boost::asio::io_service& session_io) : module_event_map_test_base_class(vs,session_io){
		};
		~up_thread_realserver_connect_event_test_class(){};
		
		void test_call(){
			l7vs::tcp_session::up_thread_realserver_connect_event(LOCAL_PROC);
		};
		
};
void up_thread_realserver_connect_event_test(){
	BOOST_MESSAGE( "----- up_thread_realserver_connect_event test start -----" );
	boost::asio::io_service io;
	l7vs::virtualservice_tcp vs;
	up_thread_realserver_connect_event_test_class test_obj(vs,io);
	
	std::string test_protocol_name("test protocol");
	l7vs::test_protocol_module proto_test(test_protocol_name);

	test_obj.set_protocol_module((l7vs::protocol_module_base*)&proto_test);
	
	l7vs::protocol_module_base::EVENT_TAG chek_event[13];
	chek_event[0] = l7vs::protocol_module_base::ACCEPT;
	chek_event[1] = l7vs::protocol_module_base::CLIENT_DISCONNECT;
	chek_event[2] = l7vs::protocol_module_base::CLIENT_RECV;
	chek_event[3] = l7vs::protocol_module_base::CLIENT_RESPONSE_SEND;
	chek_event[4] = l7vs::protocol_module_base::REALSERVER_SELECT;
	chek_event[5] = l7vs::protocol_module_base::REALSERVER_CONNECT;
	chek_event[6] = l7vs::protocol_module_base::REALSERVER_SEND;
	chek_event[7] = l7vs::protocol_module_base::REALSERVER_DISCONNECT;
	chek_event[8] = l7vs::protocol_module_base::SORRYSERVER_SELECT;
	chek_event[9] = l7vs::protocol_module_base::SORRYSERVER_CONNECT;
	chek_event[10] = l7vs::protocol_module_base::SORRYSERVER_SEND;
	chek_event[11] = l7vs::protocol_module_base::SORRYSERVER_DISCONNECT;
	chek_event[12] = l7vs::protocol_module_base::FINALIZE;
	
	// unit_test [1] up_thread_realserver_connect_event up_thread_next_call_function update check
	std::cout << "[1] up_thread_realserver_connect_event up_thread_next_call_function update check" << std::endl;
	for(int i = 0; i < 13;i++){
		proto_test.handle_realserver_connect_res_tag = chek_event[i];
		test_obj.test_call();
		BOOST_CHECK(test_obj.chk_up_thread_next_call_function(chek_event[i]));
	}
	
	// unit_test [2] up_thread_realserver_connect_event module parameter check
	std::cout << "[2] up_thread_realserver_connect_event module parameter check" << std::endl;
	boost::thread::id def_id;
	boost::thread::id proc_id = boost::this_thread::get_id();
	test_obj.set_up_thread_id(proc_id);
	proto_test.handle_realserver_connect_thread_id = def_id;	
	proto_test.handle_realserver_connect_res_tag = l7vs::protocol_module_base::FINALIZE;
	BOOST_CHECK(proto_test.handle_realserver_connect_thread_id != proc_id);
	char set_char = CHAR_MIN;
	for(int i = 0;i < MAX_BUFFER_SIZE;i++){
		proto_test.handle_realserver_connect_sendbuffer[i] = set_char;
		if(set_char == CHAR_MAX){
			set_char = CHAR_MIN;
		}else{
			set_char++;
		}
	}
	proto_test.handle_realserver_connect_datalen = MAX_BUFFER_SIZE;
	l7vs::tcp_data& test_data = test_obj.get_up_thread_data_dest_side();
	test_data.initialize();
	boost::asio::ip::tcp::endpoint server_end(boost::asio::ip::address::from_string(DUMMI_SERVER_IP), DUMMI_SERVER_PORT);
	test_data.set_endpoint(server_end);
	
	test_obj.test_call();
	
	BOOST_CHECK(proto_test.handle_realserver_connect_thread_id == proc_id);
	BOOST_CHECK(proto_test.handle_realserver_connect_sendbuffer == test_data.get_data());
	BOOST_CHECK(proto_test.handle_realserver_connect_datalen == test_data.get_size());
	BOOST_CHECK(server_end == test_data.get_endpoint());
	
	// unit_test [3] up_thread_realserver_connect_event not fond function error check
	std::cout << "[3] up_thread_realserver_connect_event not fond function error check" << std::endl;
	test_obj.up_thread_function_array_clear();
	proto_test.handle_realserver_connect_res_tag = l7vs::protocol_module_base::FINALIZE;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(40,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	
	// unit_test [4] up_thread_realserver_connect_event returnd illegal EVENT_TAG error check
	std::cout << "[4] up_thread_realserver_connect_event returnd illegal EVENT_TAG error check" << std::endl;
	test_obj.up_thread_module_event_map_clear();
	proto_test.handle_realserver_connect_res_tag = l7vs::protocol_module_base::FINALIZE;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(39,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	
	BOOST_MESSAGE( "----- up_thread_realserver_connect_event test end -----" );
}

// up_thread_sorryserver_connect_event test
// up_thread_sorryserver_connect_event test class
class up_thread_sorryserver_connect_event_test_class : public module_event_map_test_base_class{
	public:
		up_thread_sorryserver_connect_event_test_class(l7vs::virtualservice_tcp& vs,boost::asio::io_service& session_io) : module_event_map_test_base_class(vs,session_io){
		};
		~up_thread_sorryserver_connect_event_test_class(){};
		
		void test_call(){
			l7vs::tcp_session::up_thread_sorryserver_connect_event(LOCAL_PROC);
		};
		
};
void up_thread_sorryserver_connect_event_test(){
	BOOST_MESSAGE( "----- up_thread_sorryserver_connect_event test start -----" );
	boost::asio::io_service io;
	l7vs::virtualservice_tcp vs;
	up_thread_sorryserver_connect_event_test_class test_obj(vs,io);
	
	std::string test_protocol_name("test protocol");
	l7vs::test_protocol_module proto_test(test_protocol_name);

	test_obj.set_protocol_module((l7vs::protocol_module_base*)&proto_test);
	
	l7vs::protocol_module_base::EVENT_TAG chek_event[13];
	chek_event[0] = l7vs::protocol_module_base::ACCEPT;
	chek_event[1] = l7vs::protocol_module_base::CLIENT_DISCONNECT;
	chek_event[2] = l7vs::protocol_module_base::CLIENT_RECV;
	chek_event[3] = l7vs::protocol_module_base::CLIENT_RESPONSE_SEND;
	chek_event[4] = l7vs::protocol_module_base::REALSERVER_SELECT;
	chek_event[5] = l7vs::protocol_module_base::REALSERVER_CONNECT;
	chek_event[6] = l7vs::protocol_module_base::REALSERVER_SEND;
	chek_event[7] = l7vs::protocol_module_base::REALSERVER_DISCONNECT;
	chek_event[8] = l7vs::protocol_module_base::SORRYSERVER_SELECT;
	chek_event[9] = l7vs::protocol_module_base::SORRYSERVER_CONNECT;
	chek_event[10] = l7vs::protocol_module_base::SORRYSERVER_SEND;
	chek_event[11] = l7vs::protocol_module_base::SORRYSERVER_DISCONNECT;
	chek_event[12] = l7vs::protocol_module_base::FINALIZE;
	
	// unit_test [1] up_thread_sorryserver_connect_event up_thread_next_call_function update check
	std::cout << "[1] up_thread_sorryserver_connect_event up_thread_next_call_function update check" << std::endl;
	for(int i = 0; i < 13;i++){
		proto_test.handle_sorryserver_connect_res_tag = chek_event[i];
		test_obj.test_call();
		BOOST_CHECK(test_obj.chk_up_thread_next_call_function(chek_event[i]));
	}
	
	// unit_test [2] up_thread_sorryserver_connect_event module parameter check
	std::cout << "[2] up_thread_sorryserver_connect_event module parameter check" << std::endl;
	boost::thread::id def_id;
	boost::thread::id proc_id = boost::this_thread::get_id();
	test_obj.set_up_thread_id(proc_id);
	proto_test.handle_sorryserver_connect_thread_id = def_id;	
	proto_test.handle_sorryserver_connect_res_tag = l7vs::protocol_module_base::FINALIZE;
	BOOST_CHECK(proto_test.handle_sorryserver_connect_thread_id != proc_id);
	char set_char = CHAR_MIN;
	for(int i = 0;i < MAX_BUFFER_SIZE;i++){
		proto_test.handle_sorryserver_connect_sendbuffer[i] = set_char;
		if(set_char == CHAR_MAX){
			set_char = CHAR_MIN;
		}else{
			set_char++;
		}
	}
	proto_test.handle_sorryserver_connect_datalen = MAX_BUFFER_SIZE;
	l7vs::tcp_data& test_data = test_obj.get_up_thread_data_dest_side();
	test_data.initialize();
	boost::asio::ip::tcp::endpoint server_end(boost::asio::ip::address::from_string(DUMMI_SERVER_IP), DUMMI_SERVER_PORT);
	test_data.set_endpoint(server_end);
	
	test_obj.test_call();
	
	BOOST_CHECK(proto_test.handle_sorryserver_connect_thread_id == proc_id);
	BOOST_CHECK(proto_test.handle_sorryserver_connect_sendbuffer == test_data.get_data());
	BOOST_CHECK(proto_test.handle_sorryserver_connect_datalen == test_data.get_size());
	BOOST_CHECK(server_end == test_data.get_endpoint());
	
	// unit_test [3] up_thread_sorryserver_connect_event not fond function error check
	std::cout << "[3] up_thread_sorryserver_connect_event not fond function error check" << std::endl;
	test_obj.up_thread_function_array_clear();
	proto_test.handle_sorryserver_connect_res_tag = l7vs::protocol_module_base::FINALIZE;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(57,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	
	// unit_test [4] up_thread_sorryserver_connect_event returnd illegal EVENT_TAG error check
	std::cout << "[4] up_thread_sorryserver_connect_event returnd illegal EVENT_TAG error check" << std::endl;
	test_obj.up_thread_module_event_map_clear();
	proto_test.handle_sorryserver_connect_res_tag = l7vs::protocol_module_base::FINALIZE;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(56,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	
	BOOST_MESSAGE( "----- up_thread_sorryserver_connect_event test end -----" );
}
// down_thread_client_connection_chk_event test
// down_thread_client_connection_chk_event test class
class down_thread_client_connection_chk_event_test_class : public module_event_map_test_base_class{
	public:
		down_thread_client_connection_chk_event_test_class(l7vs::virtualservice_tcp& vs,boost::asio::io_service& session_io) : module_event_map_test_base_class(vs,session_io){
		};
		~down_thread_client_connection_chk_event_test_class(){};
		
		void test_call(){
			l7vs::tcp_session::down_thread_client_connection_chk_event(LOCAL_PROC);
		};
		
};
void down_thread_client_connection_chk_event_test(){
	BOOST_MESSAGE( "----- down_thread_client_connection_chk_event test start -----" );
	boost::asio::io_service io;
	l7vs::virtualservice_tcp vs;
	down_thread_client_connection_chk_event_test_class test_obj(vs,io);
	
	std::string test_protocol_name("test protocol");
	l7vs::test_protocol_module proto_test(test_protocol_name);

	test_obj.set_protocol_module((l7vs::protocol_module_base*)&proto_test);
	
	l7vs::protocol_module_base::EVENT_TAG chek_event[7];
	chek_event[0] = l7vs::protocol_module_base::CLIENT_DISCONNECT;
	chek_event[1] = l7vs::protocol_module_base::CLIENT_CONNECTION_CHECK;
	chek_event[2] = l7vs::protocol_module_base::REALSERVER_RECV;
	chek_event[3] = l7vs::protocol_module_base::REALSERVER_DISCONNECT;
	chek_event[4] = l7vs::protocol_module_base::SORRYSERVER_RECV;
	chek_event[5] = l7vs::protocol_module_base::SORRYSERVER_DISCONNECT;
	chek_event[6] = l7vs::protocol_module_base::FINALIZE;
	
	// unit_test [1] down_thread_client_connection_chk_event down_thread_next_call_function update check
	std::cout << "[1] down_thread_client_connection_chk_event down_thread_next_call_function update check" << std::endl;
	for(int i = 0; i < 7;i++){
		proto_test.handle_client_connection_check_res_tag = chek_event[i];
		test_obj.test_call();
		BOOST_CHECK(test_obj.chk_down_thread_next_call_function(chek_event[i]));
	}
	
	// unit_test [2] down_thread_client_connection_chk_event module parameter check
	std::cout << "[2] down_thread_client_connection_chk_event module parameter check" << std::endl;
	boost::thread::id def_id;
	boost::thread::id proc_id = boost::this_thread::get_id();
	test_obj.set_down_thread_id(proc_id);
	proto_test.handle_client_connection_check_thread_id = def_id;	
	proto_test.handle_client_connection_check_res_tag = l7vs::protocol_module_base::FINALIZE;
	BOOST_CHECK(proto_test.handle_client_connection_check_thread_id != proc_id);
	char set_char = CHAR_MIN;
	for(int i = 0;i < MAX_BUFFER_SIZE;i++){
		proto_test.handle_client_connection_check_sendbuffer[i] = set_char;
		if(set_char == CHAR_MAX){
			set_char = CHAR_MIN;
		}else{
			set_char++;
		}
	}
	proto_test.handle_client_connection_check_datalen = MAX_BUFFER_SIZE;
	l7vs::tcp_data& test_data = test_obj.get_down_thread_data_client_side();
	test_data.initialize();
	
	test_obj.test_call();
	
	BOOST_CHECK(proto_test.handle_client_connection_check_thread_id == proc_id);
	BOOST_CHECK(proto_test.handle_client_connection_check_sendbuffer == test_data.get_data());
	BOOST_CHECK(proto_test.handle_client_connection_check_datalen == test_data.get_size());
	
	// unit_test [3] down_thread_client_connection_chk_event not fond function error check
	std::cout << "[3] down_thread_client_connection_chk_event not fond function error check" << std::endl;
	test_obj.down_thread_function_array_clear();
	proto_test.handle_client_connection_check_res_tag = l7vs::protocol_module_base::FINALIZE;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(79,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	
	// unit_test [4] down_thread_client_connection_chk_event returnd illegal EVENT_TAG error check
	std::cout << "[4] down_thread_client_connection_chk_event returnd illegal EVENT_TAG error check" << std::endl;
	test_obj.down_thread_module_event_map_clear();
	proto_test.handle_client_connection_check_res_tag = l7vs::protocol_module_base::FINALIZE;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(78,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	
	BOOST_MESSAGE( "----- down_thread_client_connection_chk_event test end -----" );
}
// up_thread_realserver_connection_fail_event test
// up_thread_realserver_connection_fail_event test class
class up_thread_realserver_connection_fail_event_test_class : public module_event_map_test_base_class{
	public:
		up_thread_realserver_connection_fail_event_test_class(l7vs::virtualservice_tcp& vs,boost::asio::io_service& session_io) : module_event_map_test_base_class(vs,session_io){
		};
		~up_thread_realserver_connection_fail_event_test_class(){};
		
		void test_call(){
			l7vs::tcp_session::up_thread_realserver_connection_fail_event(LOCAL_PROC);
		};
		
};
void up_thread_realserver_connection_fail_event_test(){
	BOOST_MESSAGE( "----- up_thread_realserver_connection_fail_event test start -----" );
	boost::asio::io_service io;
	l7vs::virtualservice_tcp vs;
	up_thread_realserver_connection_fail_event_test_class test_obj(vs,io);
	
	std::string test_protocol_name("test protocol");
	l7vs::test_protocol_module proto_test(test_protocol_name);

	test_obj.set_protocol_module((l7vs::protocol_module_base*)&proto_test);
	
	l7vs::protocol_module_base::EVENT_TAG chek_event[13];
	chek_event[0] = l7vs::protocol_module_base::ACCEPT;
	chek_event[1] = l7vs::protocol_module_base::CLIENT_DISCONNECT;
	chek_event[2] = l7vs::protocol_module_base::CLIENT_RECV;
	chek_event[3] = l7vs::protocol_module_base::CLIENT_RESPONSE_SEND;
	chek_event[4] = l7vs::protocol_module_base::REALSERVER_SELECT;
	chek_event[5] = l7vs::protocol_module_base::REALSERVER_CONNECT;
	chek_event[6] = l7vs::protocol_module_base::REALSERVER_SEND;
	chek_event[7] = l7vs::protocol_module_base::REALSERVER_DISCONNECT;
	chek_event[8] = l7vs::protocol_module_base::SORRYSERVER_SELECT;
	chek_event[9] = l7vs::protocol_module_base::SORRYSERVER_CONNECT;
	chek_event[10] = l7vs::protocol_module_base::SORRYSERVER_SEND;
	chek_event[11] = l7vs::protocol_module_base::SORRYSERVER_DISCONNECT;
	chek_event[12] = l7vs::protocol_module_base::FINALIZE;
	
	// unit_test [1] up_thread_realserver_connection_fail_event up_thread_next_call_function update check
	std::cout << "[1] up_thread_realserver_connection_fail_event up_thread_next_call_function update check" << std::endl;
	for(int i = 0; i < 13;i++){
		proto_test.handle_realserver_connection_fail_res_tag = chek_event[i];
		test_obj.test_call();
		BOOST_CHECK(test_obj.chk_up_thread_next_call_function(chek_event[i]));
	}
	
	// unit_test [2] up_thread_realserver_connection_fail_event module parameter check
	std::cout << "[2] up_thread_realserver_connection_fail_event module parameter check" << std::endl;
	boost::thread::id def_id;
	boost::thread::id proc_id = boost::this_thread::get_id();
	test_obj.set_up_thread_id(proc_id);
	proto_test.handle_realserver_connection_fail_thread_id = def_id;	
	proto_test.handle_realserver_connection_fail_res_tag = l7vs::protocol_module_base::FINALIZE;
	BOOST_CHECK(proto_test.handle_realserver_connection_fail_thread_id != proc_id);
	boost::asio::ip::tcp::endpoint server_end(boost::asio::ip::address::from_string(DUMMI_SERVER_IP), DUMMI_SERVER_PORT);
	l7vs::tcp_data& test_data = test_obj.get_up_thread_data_dest_side();
	test_data.set_endpoint(server_end);
	
	test_obj.test_call();
	
	BOOST_CHECK(proto_test.handle_realserver_connection_fail_thread_id == proc_id);
	BOOST_CHECK(server_end == test_data.get_endpoint());
	
	// unit_test [3] up_thread_realserver_connection_fail_event not fond function error check
	std::cout << "[3] up_thread_realserver_connection_fail_event not fond function error check" << std::endl;
	test_obj.up_thread_function_array_clear();
	proto_test.handle_realserver_connection_fail_res_tag = l7vs::protocol_module_base::FINALIZE;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(42,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	
	// unit_test [4] up_thread_realserver_connection_fail_event returnd illegal EVENT_TAG error check
	std::cout << "[4] up_thread_realserver_connection_fail_event returnd illegal EVENT_TAG error check" << std::endl;
	test_obj.up_thread_module_event_map_clear();
	proto_test.handle_realserver_connection_fail_res_tag = l7vs::protocol_module_base::FINALIZE;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(41,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	
	BOOST_MESSAGE( "----- up_thread_realserver_connection_fail_event test end -----" );
}

// up_thread_sorryserver_connection_fail_event test
// up_thread_sorryserver_connection_fail_event test class
class up_thread_sorryserver_connection_fail_event_test_class : public module_event_map_test_base_class{
	public:
		up_thread_sorryserver_connection_fail_event_test_class(l7vs::virtualservice_tcp& vs,boost::asio::io_service& session_io) : module_event_map_test_base_class(vs,session_io){
		};
		~up_thread_sorryserver_connection_fail_event_test_class(){};
		
		void test_call(){
			l7vs::tcp_session::up_thread_sorryserver_connection_fail_event(LOCAL_PROC);
		};
		
};
void up_thread_sorryserver_connection_fail_event_test(){
	BOOST_MESSAGE( "----- up_thread_sorryserver_connection_fail_event test start -----" );
	boost::asio::io_service io;
	l7vs::virtualservice_tcp vs;
	up_thread_sorryserver_connection_fail_event_test_class test_obj(vs,io);
	
	std::string test_protocol_name("test protocol");
	l7vs::test_protocol_module proto_test(test_protocol_name);

	test_obj.set_protocol_module((l7vs::protocol_module_base*)&proto_test);
	
	l7vs::protocol_module_base::EVENT_TAG chek_event[13];
	chek_event[0] = l7vs::protocol_module_base::ACCEPT;
	chek_event[1] = l7vs::protocol_module_base::CLIENT_DISCONNECT;
	chek_event[2] = l7vs::protocol_module_base::CLIENT_RECV;
	chek_event[3] = l7vs::protocol_module_base::CLIENT_RESPONSE_SEND;
	chek_event[4] = l7vs::protocol_module_base::REALSERVER_SELECT;
	chek_event[5] = l7vs::protocol_module_base::REALSERVER_CONNECT;
	chek_event[6] = l7vs::protocol_module_base::REALSERVER_SEND;
	chek_event[7] = l7vs::protocol_module_base::REALSERVER_DISCONNECT;
	chek_event[8] = l7vs::protocol_module_base::SORRYSERVER_SELECT;
	chek_event[9] = l7vs::protocol_module_base::SORRYSERVER_CONNECT;
	chek_event[10] = l7vs::protocol_module_base::SORRYSERVER_SEND;
	chek_event[11] = l7vs::protocol_module_base::SORRYSERVER_DISCONNECT;
	chek_event[12] = l7vs::protocol_module_base::FINALIZE;
	
	// unit_test [1] up_thread_sorryserver_connection_fail_event up_thread_next_call_function update check
	std::cout << "[1] up_thread_sorryserver_connection_fail_event up_thread_next_call_function update check" << std::endl;
	for(int i = 0; i < 13;i++){
		proto_test.handle_sorryserver_connection_fail_res_tag = chek_event[i];
		test_obj.test_call();
		BOOST_CHECK(test_obj.chk_up_thread_next_call_function(chek_event[i]));
	}
	
	// unit_test [2] up_thread_sorryserver_connection_fail_event module parameter check
	std::cout << "[2] up_thread_sorryserver_connection_fail_event module parameter check" << std::endl;
	boost::thread::id def_id;
	boost::thread::id proc_id = boost::this_thread::get_id();
	test_obj.set_up_thread_id(proc_id);
	proto_test.handle_sorryserver_connection_fail_thread_id = def_id;	
	proto_test.handle_sorryserver_connection_fail_res_tag = l7vs::protocol_module_base::FINALIZE;
	BOOST_CHECK(proto_test.handle_sorryserver_connection_fail_thread_id != proc_id);
	boost::asio::ip::tcp::endpoint server_end(boost::asio::ip::address::from_string(DUMMI_SERVER_IP), DUMMI_SERVER_PORT);
	l7vs::tcp_data& test_data = test_obj.get_up_thread_data_dest_side();
	test_data.set_endpoint(server_end);
	
	test_obj.test_call();
	
	BOOST_CHECK(proto_test.handle_sorryserver_connection_fail_thread_id == proc_id);
	BOOST_CHECK(server_end == test_data.get_endpoint());
	
	// unit_test [3] up_thread_sorryserver_connection_fail_event not fond function error check
	std::cout << "[3] up_thread_sorryserver_connection_fail_event not fond function error check" << std::endl;
	test_obj.up_thread_function_array_clear();
	proto_test.handle_sorryserver_connection_fail_res_tag = l7vs::protocol_module_base::FINALIZE;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(59,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	
	// unit_test [4] up_thread_sorryserver_connection_fail_event returnd illegal EVENT_TAG error check
	std::cout << "[4] up_thread_sorryserver_connection_fail_event returnd illegal EVENT_TAG error check" << std::endl;
	test_obj.up_thread_module_event_map_clear();
	proto_test.handle_sorryserver_connection_fail_res_tag = l7vs::protocol_module_base::FINALIZE;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(58,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	
	BOOST_MESSAGE( "----- up_thread_sorryserver_connection_fail_event test end -----" );
}

// receive & send test class
class receive_send_test_class : public l7vs::tcp_session{
	public:
		receive_send_test_class(l7vs::virtualservice_tcp& vs,boost::asio::io_service& session_io) : l7vs::tcp_session(vs,session_io){
		};
		~receive_send_test_class(){};
		
		void test_call_client_receive(){
			l7vs::tcp_session::up_thread_client_receive(LOCAL_PROC);
		};
		void test_call_client_send(){
			l7vs::tcp_session::down_thread_client_send(LOCAL_PROC);
		};
		void test_call_realserver_receive(){
			l7vs::tcp_session::down_thread_realserver_receive(LOCAL_PROC);
		};
		void test_call_realserver_send(){
			l7vs::tcp_session::up_thread_realserver_send(LOCAL_PROC);
		};
		void test_call_sorryserver_receive(){
			l7vs::tcp_session::down_thread_sorryserver_receive(LOCAL_PROC);
		};
		void test_call_sorryserver_send(){
			l7vs::tcp_session::up_thread_sorryserver_send(LOCAL_PROC);
		};
		void set_protocol_module(l7vs::protocol_module_base* set_prot){
			protocol_module = set_prot;
		};
		l7vs::tcp_data& get_up_thread_data_client_side(){
			return up_thread_data_client_side;
		};
		l7vs::tcp_data& get_up_thread_data_dest_side(){
			return up_thread_data_dest_side;
		};
		l7vs::tcp_data& get_down_thread_data_client_side(){
			return down_thread_data_client_side;
		};
		l7vs::tcp_data& get_down_thread_data_dest_side(){
			return down_thread_data_dest_side;
		};
		l7vs::tcp_socket& get_client_socket(){
			return client_socket;
		};
		socket_element& get_sorryserver_socket(){
			return sorryserver_socket;
		};
		std::list<socket_element>& get_down_thread_receive_realserver_socket_list(){
			return down_thread_receive_realserver_socket_list;
		};
		std::map<endpoint,tcp_socket_ptr>& get_up_thread_send_realserver_socket_map(){
			return up_thread_send_realserver_socket_map;
		};
		std::list<socket_element>::iterator& get_down_thread_current_receive_realserver_socket(){
			return down_thread_current_receive_realserver_socket;
		};
		
		void set_up_thread_id(boost::thread::id set_id){
			up_thread_id = set_id;
		}
		void set_down_thread_id(boost::thread::id set_id){
			down_thread_id = set_id;
		}
		
		void next_up_function_call(){
			up_thread_next_call_function.second(LOCAL_PROC);
		}
		void next_down_function_call(){
			down_thread_next_call_function.second(LOCAL_PROC);
		}
		
		void set_up_thread_next_function_call_exit(){
			up_thread_function_pair fun_it = up_thread_function_array[UP_FUNC_EXIT];
			up_thread_next_call_function = fun_it;
		};
		
		void set_down_thread_next_function_call_exit(){
			down_thread_function_pair fun_it = down_thread_function_array[DOWN_FUNC_EXIT];
			down_thread_next_call_function = fun_it;
		};
		
		// next call function check
		void up_thread_realserver_get_destination_event(const TCP_PROCESS_TYPE_TAG process_type){
			up_thread_realserver_get_destination_event_call_check = true;
		};
		bool up_thread_realserver_get_destination_event_call_check;
		void up_thread_sorryserver_get_destination_event(const TCP_PROCESS_TYPE_TAG process_type){
			up_thread_sorryserver_get_destination_event_call_check = true;
		};
		bool up_thread_sorryserver_get_destination_event_call_check;
		
		void up_thread_client_disconnect(const TCP_PROCESS_TYPE_TAG process_type){
			up_thread_client_disconnect_call_check = true;
		};
		bool up_thread_client_disconnect_call_check;
		
		
		void down_thread_client_disconnect(const TCP_PROCESS_TYPE_TAG process_type){
			down_thread_client_disconnect_call_check = true;
		};
		bool down_thread_client_disconnect_call_check;
		
		void up_thread_realserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type){
			up_thread_realserver_disconnect_call_check = true;
		};
		bool up_thread_realserver_disconnect_call_check;
		
		void up_thread_sorryserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type){
			up_thread_sorryserver_disconnect_call_check = true;
		};
		bool up_thread_sorryserver_disconnect_call_check;
		
		void down_thread_realserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type){
			down_thread_realserver_disconnect_call_check = true;
		};
		bool down_thread_realserver_disconnect_call_check;
		
		void down_thread_sorryserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type){
			down_thread_sorryserver_disconnect_call_check = true;
		};
		bool down_thread_sorryserver_disconnect_call_check;
		
		void up_thread_exit(const TCP_PROCESS_TYPE_TAG process_type){
			up_thread_exit_call_check = true;
		}
		bool up_thread_exit_call_check;
		
		void down_thread_exit(const TCP_PROCESS_TYPE_TAG process_type){
			down_thread_exit_call_check = true;
		}
		bool down_thread_exit_call_check;
		
		void up_thread_client_receive(const TCP_PROCESS_TYPE_TAG process_type){
			up_thread_client_receive_call_check = true;
			tcp_session::up_thread_client_receive(process_type);
		};
		bool up_thread_client_receive_call_check;
		
		void down_thread_realserver_receive(const TCP_PROCESS_TYPE_TAG process_type){
			down_thread_realserver_receive_call_check = true;
			tcp_session::down_thread_realserver_receive(process_type);
		};
		bool down_thread_realserver_receive_call_check;
		
		void down_thread_sorryserver_receive(const TCP_PROCESS_TYPE_TAG process_type){
			down_thread_sorryserver_receive_call_check = true;
			tcp_session::down_thread_sorryserver_receive(process_type);
		};
		bool down_thread_sorryserver_receive_call_check;
		
		void down_thread_client_send(const TCP_PROCESS_TYPE_TAG process_type){
			down_thread_client_send_call_check = true;
		};
		bool down_thread_client_send_call_check;
		
		void up_thread_realserver_send(const TCP_PROCESS_TYPE_TAG process_type){
			up_thread_realserver_send_call_check = true;
		};
		bool up_thread_realserver_send_call_check;
		
		void up_thread_sorryserver_send(const TCP_PROCESS_TYPE_TAG process_type){
			up_thread_sorryserver_send_call_check = true;
		};
		bool up_thread_sorryserver_send_call_check;
		
		void down_thread_client_connection_chk_event(const TCP_PROCESS_TYPE_TAG process_type){
			down_thread_client_connection_chk_event_call_check = true;;
		};
		bool down_thread_client_connection_chk_event_call_check;
		
		// map clear
		void up_thread_function_array_clear(){
			for(int i = 0;i <= UP_FUNC_EXIT;i++){
				up_thread_function_array[i].second = NULL;
			}
		};
		void down_thread_function_array_clear(){
			for(int i = 0; i <= DOWN_FUNC_EXIT;i++){
				down_thread_function_array[i].second = NULL;
			}
		};
		
		void up_thread_module_event_map_clear(){
			up_thread_module_event_map.clear();
		};
		
		void down_thread_module_event_map_clear(){
			down_thread_module_event_map.clear();
		};
};

// up_thread_client_receive test
void up_thread_client_receive_test(){
	BOOST_MESSAGE( "----- up_thread_client_receive test end -----" );
	boost::asio::io_service io;
	l7vs::virtualservice_tcp vs;
	std::string test_protocol_name("test protocol");
	l7vs::test_protocol_module proto_test(test_protocol_name);
	// up_thread_client_receive
	receive_send_test_class test_obj(vs,io);
	test_obj.set_protocol_module((l7vs::protocol_module_base*)&proto_test);
	boost::thread::id proc_id = boost::this_thread::get_id();
	
	
	l7vs::tcp_data& up_thread_data_client_side = test_obj.get_up_thread_data_client_side();
//	l7vs::tcp_data& up_thread_data_dest_side = test_obj.get_up_thread_data_dest_side();
//	l7vs::tcp_data& down_thread_data_client_side = test_obj.get_up_thread_data_client_side();
//	l7vs::tcp_data& down_thread_data_dest_side = test_obj.get_up_thread_data_dest_side();
	
	// get client socket
	l7vs::tcp_socket& socket = test_obj.get_client_socket();
	// dummy server client socket connect
	boost::system::error_code ec;
	test_mirror_server test_server;
	// accept req
	test_server.breq_acc_flag = true;
	// close wait req
	test_server.breq_close_wait_flag = true;
	// recv cont
	test_server.req_recv_cnt = 0;
	// test server start
	boost::thread server_thread(boost::bind(&test_mirror_server::run,&test_server));
	while( !test_server.brun_flag ){
		sleep(1);
	}
	
	boost::asio::ip::tcp::endpoint connect_end(boost::asio::ip::address::from_string(DUMMI_SERVER_IP), DUMMI_SERVER_PORT);
	socket.get_socket().connect(connect_end,ec);
	BOOST_CHECK(!ec);
	socket.is_open_res = true;

	while(!test_server.bconnect_flag){
		sleep(1);
	}
	boost::asio::ip::tcp::endpoint local_endpoint = socket.get_socket().local_endpoint(ec);
	BOOST_CHECK(!ec);
	
	// tcp_session set
	up_thread_data_client_side.initialize();
	test_obj.set_up_thread_id(proc_id);
	test_obj.set_down_thread_id(boost::thread::id());
	test_obj.up_thread_realserver_get_destination_event_call_check = false;
	test_obj.up_thread_client_disconnect_call_check = false;
	test_obj.up_thread_exit_call_check = false;
	test_obj.up_thread_client_receive_call_check = false;
	// vs set
//	vs.get_qos_upstream_res = 104857600;
//	vs.get_throughput_upstream_res = 0;
 	vs.get_wait_upstream_res = 0;
	vs.update_up_recv_size_in = 0;
	// socket set
	socket.read_some_res = MAX_BUFFER_SIZE;
	socket.read_some_ec.clear();
	char set_char = CHAR_MIN;
	for(int i = 0;i < MAX_BUFFER_SIZE;i++){
		socket.read_some_buffers_out[i] = set_char;
		if(set_char == CHAR_MAX)
			set_char = CHAR_MIN;
		else
			set_char++;
	}
	socket.read_some_buffers_size_in = 0;
	socket.read_some_call_check = false;
	// protocol module set
	proto_test.handle_client_recv_res_tag = l7vs::protocol_module_base::REALSERVER_SELECT;
	proto_test.handle_client_recv_in_thread_id = boost::thread::id();
	for(int i = 0;i < MAX_BUFFER_SIZE;i++){
		proto_test.handle_client_recv_in_recvbuffer[i] = '\0';
	}
	proto_test.handle_client_recv_in_recvlen = 0;

	l7vs::Logger::test_loglevel = l7vs::LOG_LV_DEBUG;
	l7vs::Logger::putLogDebug_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogDebug_id = 0;

	
	test_obj.test_call_client_receive();
	
	// unit_test [1] up_thread_client_receive client_socket read_some call check
	std::cout << "[1] up_thread_client_receive client_socket read_some call check" << std::endl;
	BOOST_CHECK(socket.read_some_call_check);
	BOOST_CHECK(socket.read_some_buffers_out == up_thread_data_client_side.get_data());
	BOOST_CHECK(socket.read_some_buffers_size_in == MAX_BUFFER_SIZE);
	BOOST_CHECK(socket.read_some_res == up_thread_data_client_side.get_size());
	
	// unit_test [2] up_thread_client_receive virtualservice update_up_recv_size_in call check
	std::cout << "[2] up_thread_client_receive virtualservice update_up_recv_size_in call check" << std::endl;
	BOOST_CHECK(vs.update_up_recv_size_in == MAX_BUFFER_SIZE);
	
	// unit_test [3] up_thread_client_receive protocol module handle_client_recv call check
	std::cout << "[3] up_thread_client_receive protocol module handle_client_recv call check" << std::endl;
	BOOST_CHECK(proto_test.handle_client_recv_in_thread_id == proc_id);
	BOOST_CHECK(proto_test.handle_client_recv_in_recvlen == socket.read_some_res);
	BOOST_CHECK(proto_test.handle_client_recv_in_recvbuffer == socket.read_some_buffers_out);
	
	// unit_test [4] up_thread_client_receive up_thread_next_call_function update check
	std::cout << "[4] up_thread_client_receive up_thread_next_call_function update check" << std::endl;
	test_obj.next_up_function_call();
	BOOST_CHECK(test_obj.up_thread_realserver_get_destination_event_call_check);
	
	// unit_test [5] up_thread_client_receive receive debug log check
	std::cout << "[5] up_thread_client_receive receive debug log check" << std::endl;
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogDebug_category);
	BOOST_CHECK_EQUAL(36,l7vs::Logger::putLogDebug_id);
	std::cout << l7vs::Logger::putLogDebug_message << std::endl;

	// unit_test [6] up_thread_client_receive qos check
	std::cout << "[6] up_thread_client_receive qos check" << std::endl;
	socket.read_some_res = 0;
 	vs.get_wait_upstream_res = 1;

	// wait = 0 >> receive
	socket.read_some_call_check = false;
	vs.get_wait_upstream_res = 0;
	l7vs::Logger::putLogDebug_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogDebug_id = 0;
	test_obj.test_call_client_receive();
	BOOST_CHECK(socket.read_some_call_check);
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_NONE,l7vs::Logger::putLogDebug_category);
	BOOST_CHECK_EQUAL(0,l7vs::Logger::putLogDebug_id);

	// wait = 1 >> not receive
	socket.read_some_call_check = false;
	vs.get_wait_upstream_res = 1;
	l7vs::Logger::putLogDebug_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogDebug_id = 0;
	test_obj.test_call_client_receive();
	BOOST_CHECK(!socket.read_some_call_check);
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogDebug_category);
	BOOST_CHECK_EQUAL(35,l7vs::Logger::putLogDebug_id);
	std::cout << l7vs::Logger::putLogDebug_message << std::endl;

	socket.read_some_res = MAX_BUFFER_SIZE;
	vs.get_throughput_upstream_res = 0;
	vs.get_wait_upstream_res = 0;
	
	// unit_test [7] up_thread_client_receive receive size 0 check
	std::cout << "[7] up_thread_client_receive receive size 0 check" << std::endl;
	test_obj.set_up_thread_next_function_call_exit();
	socket.read_some_res = 0;
	test_obj.test_call_client_receive();
	test_obj.up_thread_client_receive_call_check = false;
	test_obj.next_up_function_call();
	BOOST_CHECK(test_obj.up_thread_client_receive_call_check);
	socket.read_some_res = MAX_BUFFER_SIZE;
	
	// unit_test [8] up_thread_client_receive socket error (try_again) check
	std::cout << "[8] up_thread_client_receive socket error (try_again) check" << std::endl;
	test_obj.set_up_thread_next_function_call_exit();
	socket.read_some_ec = boost::asio::error::try_again;
	test_obj.test_call_client_receive();
	test_obj.up_thread_client_receive_call_check = false;
	test_obj.next_up_function_call();
	BOOST_CHECK(test_obj.up_thread_client_receive_call_check);
	socket.read_some_ec.clear();
	
	// unit_test [9] up_thread_client_receive socket error (eof) check
	std::cout << "[9] up_thread_client_receive socket error (eof) check" << std::endl;
	test_obj.set_up_thread_next_function_call_exit();
	socket.read_some_ec = boost::asio::error::eof;
	test_obj.test_call_client_receive();
	test_obj.up_thread_client_disconnect_call_check = false;
	test_obj.next_up_function_call();
	BOOST_CHECK(test_obj.up_thread_client_disconnect_call_check);
	socket.read_some_ec.clear();
	
	// unit_test [10] up_thread_client_receive socket error (connection_reset) check
	std::cout << "[10] up_thread_client_receive socket error (connection_reset) check" << std::endl;
	test_obj.set_up_thread_next_function_call_exit();
	socket.read_some_ec = boost::asio::error::connection_reset;
	test_obj.test_call_client_receive();
	test_obj.up_thread_client_disconnect_call_check = false;
	test_obj.next_up_function_call();
	BOOST_CHECK(test_obj.up_thread_client_disconnect_call_check);
	socket.read_some_ec.clear();
	
	// unit_test [11] up_thread_client_receive socket error (bad_descriptor) check
	std::cout << "[11] up_thread_client_receive socket error (bad_descriptor) check" << std::endl;
	test_obj.set_up_thread_next_function_call_exit();
	socket.read_some_ec = boost::asio::error::bad_descriptor;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call_client_receive();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_NONE,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(0,l7vs::Logger::putLogError_id);
	test_obj.up_thread_client_disconnect_call_check = false;
	test_obj.next_up_function_call();
	BOOST_CHECK(test_obj.up_thread_client_disconnect_call_check);
	socket.read_some_ec.clear();

	// unit_test [12] up_thread_client_receive closed socket error (bad_descriptor) check
	std::cout << "[12] up_thread_client_receive closed socket error (bad_descriptor) check" << std::endl;
	test_obj.set_up_thread_next_function_call_exit();
	socket.read_some_ec = boost::asio::error::bad_descriptor;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	socket.is_open_res = false;
	test_obj.test_call_client_receive();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_NONE,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(0,l7vs::Logger::putLogError_id);
	test_obj.up_thread_client_disconnect_call_check = false;
	test_obj.next_up_function_call();
	BOOST_CHECK(test_obj.up_thread_client_disconnect_call_check);
	socket.read_some_ec.clear();
	
	// unit_test [13] up_thread_client_receive not fond function error check
	std::cout << "[13] up_thread_client_receive not fond function error check" << std::endl;
	test_obj.up_thread_function_array_clear();
	test_obj.up_thread_exit_call_check = false;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call_client_receive();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(21,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	BOOST_CHECK(test_obj.up_thread_exit_call_check);
	
	// unit_test [14] up_thread_client_receive returnd illegal EVENT_TAG error check
	std::cout << "[14] up_thread_client_receive returnd illegal EVENT_TAG error check" << std::endl;
	test_obj.up_thread_module_event_map_clear();
	test_obj.up_thread_exit_call_check = false;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call_client_receive();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(20,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	BOOST_CHECK(test_obj.up_thread_exit_call_check);
	
	// dummy server stop
	test_server.breq_close_wait_flag = false;	
	test_server.bstop_flag = true;
	server_thread.join();
	
	BOOST_MESSAGE( "----- up_thread_client_receive test end -----" );
}

// down_thread_realserver_receive test
void down_thread_realserver_receive_test(){
	BOOST_MESSAGE( "----- down_thread_realserver_receive test start -----" );
	boost::asio::io_service io;
	l7vs::virtualservice_tcp vs;
	std::string test_protocol_name("test protocol");
	l7vs::test_protocol_module proto_test(test_protocol_name);
	receive_send_test_class test_obj(vs,io);
	test_obj.set_protocol_module((l7vs::protocol_module_base*)&proto_test);
	boost::thread::id proc_id = boost::this_thread::get_id();
	
	l7vs::tcp_data& down_thread_data_dest_side = test_obj.get_down_thread_data_dest_side();
	
	// set realserver socket
	std::list< std::pair< boost::asio::ip::tcp::endpoint, boost::shared_ptr< l7vs::tcp_socket > > >& rs_list = test_obj.get_down_thread_receive_realserver_socket_list();
	std::list< std::pair< boost::asio::ip::tcp::endpoint, boost::shared_ptr< l7vs::tcp_socket > > >::iterator& rs_cur = test_obj.get_down_thread_current_receive_realserver_socket();
	std::pair< boost::asio::ip::tcp::endpoint, boost::shared_ptr< l7vs::tcp_socket > > push_pair;
	
	for(int i = 0 ; i < 3 ;i++){
		boost::shared_ptr< l7vs::tcp_socket > new_socket(new l7vs::tcp_socket(io));
		push_pair.first.address(boost::asio::ip::address::from_string(DUMMI_SERVER_IP));
		push_pair.first.port(7000+i);
		push_pair.second = new_socket;
		// socket set
		new_socket->read_some_res = MAX_BUFFER_SIZE;
		new_socket->read_some_ec.clear();
		new_socket->is_open_res = true;
		char set_char = CHAR_MIN;
		for(int j = 0;j < MAX_BUFFER_SIZE;j++){
			new_socket->read_some_buffers_out[j] = set_char;
			if(set_char == CHAR_MAX)
				set_char = CHAR_MIN;
			else
				set_char++;
		}
		new_socket->read_some_buffers_size_in = 0;
		new_socket->read_some_call_check = false;
		rs_list.push_back(push_pair);
	}
	rs_cur = rs_list.begin();
	
	// tcp_session set
	down_thread_data_dest_side.initialize();
	test_obj.set_down_thread_id(proc_id);
	test_obj.set_up_thread_id(boost::thread::id());
	test_obj.down_thread_client_connection_chk_event_call_check = false;
	test_obj.down_thread_realserver_disconnect_call_check = false;
	test_obj.down_thread_exit_call_check = false;
	test_obj.down_thread_realserver_receive_call_check = false;
	// vs set
//	vs.get_qos_downstream_res = 104857600;
//	vs.get_throughput_downstream_res = 0;
	vs.get_wait_downstream_res = 0;
	vs.update_down_recv_size_in = 0;
	// protocol module set
	proto_test.handle_realserver_recv_tcp_res_tag = l7vs::protocol_module_base::CLIENT_CONNECTION_CHECK;
	proto_test.handle_realserver_recv_tcp_in_thread_id = boost::thread::id();
	for(int i = 0;i < MAX_BUFFER_SIZE;i++){
		proto_test.handle_realserver_recv_tcp_in_recvbuffer[i] = '\0';
	}
	proto_test.handle_realserver_recv_tcp_in_recvlen = 0;
	proto_test.handle_realserver_recv_tcp_in_rs_endpoint = boost::asio::ip::tcp::endpoint();
	
	boost::asio::ip::tcp::endpoint rs_end = rs_cur->first;
	l7vs::tcp_socket& socket = *(rs_cur->second);
	l7vs::Logger::test_loglevel = l7vs::LOG_LV_DEBUG;
	l7vs::Logger::putLogDebug_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogDebug_id = 0;
	
	test_obj.test_call_realserver_receive();
	
	// unit_test [1] down_thread_realserver_receive realserver read_some call check
	std::cout << "[1] down_thread_realserver_receive realserver read_some call check" << std::endl;
	BOOST_CHECK(socket.read_some_call_check);
	BOOST_CHECK(socket.read_some_buffers_out == down_thread_data_dest_side.get_data());
	BOOST_CHECK(socket.read_some_buffers_size_in == MAX_BUFFER_SIZE);
	BOOST_CHECK(socket.read_some_res == down_thread_data_dest_side.get_size());
	BOOST_CHECK(rs_end == down_thread_data_dest_side.get_endpoint());
	
	// unit_test [2] down_thread_realserver_receive virtualservice update_down_recv_size_in call check
	std::cout << "[2] down_thread_realserver_receive virtualservice update_down_recv_size_in call check" << std::endl;
	BOOST_CHECK(vs.update_down_recv_size_in == MAX_BUFFER_SIZE);
	
	// unit_test [3] down_thread_realserver_receive protocol module handle_realserver_recv call check
	std::cout << "[3] down_thread_realserver_receive protocol module handle_realserver_recv call check" << std::endl;
	BOOST_CHECK(proto_test.handle_realserver_recv_tcp_in_thread_id == proc_id);
	BOOST_CHECK(proto_test.handle_realserver_recv_tcp_in_recvlen == socket.read_some_res);
	BOOST_CHECK(proto_test.handle_realserver_recv_tcp_in_recvbuffer == socket.read_some_buffers_out);
	BOOST_CHECK(proto_test.handle_realserver_recv_tcp_in_rs_endpoint == rs_end);
	
	// unit_test [4] down_thread_realserver_receive down_thread_next_call_function update check
	std::cout << "[4] down_thread_realserver_receive down_thread_next_call_function update check" << std::endl;
	test_obj.next_down_function_call();
	BOOST_CHECK(test_obj.down_thread_client_connection_chk_event_call_check);
	
	// unit_test [5] down_thread_realserver_receive debug log check
	std::cout << "[5] down_thread_realserver_receive debug log check" << std::endl;
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogDebug_category);
	BOOST_CHECK_EQUAL(43,l7vs::Logger::putLogDebug_id);
	std::cout << l7vs::Logger::putLogDebug_message << std::endl;

	// unit_test [6] down_thread_realserver_receive realserver_recv down_thread_current_receive_realserver_socket inclement & cyclic check
	std::cout << "[6] down_thread_realserver_receive realserver_recv down_thread_current_receive_realserver_socket inclement & cyclic check" << std::endl;
	std::list< std::pair< boost::asio::ip::tcp::endpoint, boost::shared_ptr< l7vs::tcp_socket > > >::iterator check_it;
	rs_cur = rs_list.begin();
	check_it = rs_list.begin();
	test_obj.test_call_realserver_receive();
	check_it++;
	BOOST_CHECK(check_it == rs_cur);
	test_obj.test_call_realserver_receive();
	check_it++;
	BOOST_CHECK(check_it == rs_cur);
	test_obj.test_call_realserver_receive();
	check_it = rs_list.begin();
	BOOST_CHECK(check_it == rs_cur);
	
	// unit_test [7] down_thread_realserver_receive qos check
	std::cout << "[7] down_thread_realserver_receive qos check" << std::endl;
	socket.read_some_res = 0;

	// wait = 0 >> receive
	rs_cur = rs_list.begin();
	socket.read_some_call_check = false;
	vs.get_wait_downstream_res = 0;
	l7vs::Logger::putLogDebug_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogDebug_id = 0;
	test_obj.test_call_realserver_receive();
	BOOST_CHECK(socket.read_some_call_check);
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_NONE,l7vs::Logger::putLogDebug_category);
	BOOST_CHECK_EQUAL(0,l7vs::Logger::putLogDebug_id);

	// wait = 1 >> not receive
	rs_cur = rs_list.begin();
	socket.read_some_call_check = false;
	vs.get_wait_downstream_res = 1;
	l7vs::Logger::putLogDebug_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogDebug_id = 0;
	test_obj.test_call_realserver_receive();
	BOOST_CHECK(!socket.read_some_call_check);
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogDebug_category);
	BOOST_CHECK_EQUAL(42,l7vs::Logger::putLogDebug_id);
	std::cout << l7vs::Logger::putLogDebug_message << std::endl;

	socket.read_some_res = MAX_BUFFER_SIZE;
	vs.get_throughput_downstream_res = 0;
	vs.get_wait_downstream_res = 0;
	
	// unit_test [8] down_thread_realserver_receive receive size 0 check
	std::cout << "[8] down_thread_realserver_receive receive size 0 check" << std::endl;
	rs_cur = rs_list.begin();
	test_obj.set_down_thread_next_function_call_exit();
	socket.read_some_res = 0;
	test_obj.test_call_realserver_receive();
	test_obj.down_thread_realserver_receive_call_check = false;
	test_obj.next_down_function_call();
	BOOST_CHECK(test_obj.down_thread_realserver_receive_call_check);
	socket.read_some_res = MAX_BUFFER_SIZE;
	
	// unit_test [9] down_thread_realserver_receive socket error (try_again) check
	std::cout << "[9] down_thread_realserver_receive socket error (try_again) check" << std::endl;
	rs_cur = rs_list.begin();
	test_obj.set_down_thread_next_function_call_exit();
	socket.read_some_ec = boost::asio::error::try_again;
	test_obj.test_call_realserver_receive();
	test_obj.down_thread_realserver_receive_call_check = false;
	test_obj.next_down_function_call();
	BOOST_CHECK(test_obj.down_thread_realserver_receive_call_check);
	socket.read_some_ec.clear();
	
	// unit_test [10] down_thread_realserver_receive socket error (eof) check
	std::cout << "[10] down_thread_realserver_receive socket error (eof) check" << std::endl;
	rs_cur = rs_list.begin();
	test_obj.set_down_thread_next_function_call_exit();
	socket.read_some_ec = boost::asio::error::eof;
	test_obj.test_call_realserver_receive();
	test_obj.down_thread_realserver_disconnect_call_check = false;
	test_obj.next_down_function_call();
	BOOST_CHECK(test_obj.down_thread_realserver_disconnect_call_check);
	socket.read_some_ec.clear();
	
	// unit_test [11] down_thread_realserver_receive socket error (connection_reset) check
	std::cout << "[11] down_thread_realserver_receive socket error (connection_reset) check" << std::endl;
	rs_cur = rs_list.begin();
	test_obj.set_down_thread_next_function_call_exit();
	socket.read_some_ec = boost::asio::error::connection_reset;
	test_obj.test_call_realserver_receive();
	test_obj.down_thread_realserver_disconnect_call_check = false;
	test_obj.next_down_function_call();
	BOOST_CHECK(test_obj.down_thread_realserver_disconnect_call_check);
	socket.read_some_ec.clear();
	
	// unit_test [12] down_thread_realserver_receive socket error (bad_descriptor) check
	std::cout << "[12] down_thread_realserver_receive socket error (bad_descriptor) check" << std::endl;
	rs_cur = rs_list.begin();
	test_obj.set_down_thread_next_function_call_exit();
	socket.read_some_ec = boost::asio::error::bad_descriptor;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call_realserver_receive();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_NONE,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(0,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	test_obj.down_thread_realserver_disconnect_call_check = false;
	test_obj.next_down_function_call();
	BOOST_CHECK(test_obj.down_thread_realserver_disconnect_call_check);
	socket.read_some_ec.clear();

	// unit_test [13] down_thread_realserver_receive closed socket error (bad_descriptor) check
	std::cout << "[13] down_thread_realserver_receive closed socket error (bad_descriptor) check" << std::endl;
	rs_cur = rs_list.begin();
	test_obj.set_down_thread_next_function_call_exit();
	socket.read_some_ec = boost::asio::error::bad_descriptor;
	socket.is_open_res = false;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call_realserver_receive();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_NONE,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(0,l7vs::Logger::putLogError_id);
	test_obj.down_thread_realserver_disconnect_call_check = false;
	test_obj.next_down_function_call();
	BOOST_CHECK(test_obj.down_thread_realserver_disconnect_call_check);
	socket.read_some_ec.clear();
		
	// unit_test [14] down_thread_realserver_receive not fond function error check
	std::cout << "[14] down_thread_realserver_receive not fond function error check" << std::endl;
	test_obj.down_thread_function_array_clear();
	test_obj.down_thread_exit_call_check = false;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call_realserver_receive();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(71,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	BOOST_CHECK(test_obj.down_thread_exit_call_check);
	
	// unit_test [15] down_thread_realserver_receive returnd illegal EVENT_TAG error check
	std::cout << "[15] down_thread_realserver_receive returnd illegal EVENT_TAG error check" << std::endl;
	test_obj.down_thread_module_event_map_clear();
	test_obj.down_thread_exit_call_check = false;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call_realserver_receive();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(70,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	BOOST_CHECK(test_obj.down_thread_exit_call_check);
	
	// unit_test [16] down_thread_realserver_receive  empty down_thread_receive_realserver_socket_list check
	std::cout << "[16] down_thread_realserver_receive  empty down_thread_receive_realserver_socket_list check" << std::endl;
	rs_cur = rs_list.begin();
	std::pair< boost::asio::ip::tcp::endpoint, boost::shared_ptr< l7vs::tcp_socket > > sock_pair = *rs_cur;
	rs_list.clear();
//	BOOST_CHECK(*rs_cur == sock_pair);
	socket.read_some_call_check = false;
	test_obj.test_call_realserver_receive();
	BOOST_CHECK(!socket.read_some_call_check);
	
	BOOST_MESSAGE( "----- down_thread_realserver_receive test end -----" );
}

// down_thread_sorryserver_receive test
void down_thread_sorryserver_receive_test(){
	BOOST_MESSAGE( "----- down_thread_sorryserver_receive test start -----" );
	boost::asio::io_service io;
	l7vs::virtualservice_tcp vs;
	std::string test_protocol_name("test protocol");
	l7vs::test_protocol_module proto_test(test_protocol_name);
	receive_send_test_class test_obj(vs,io);
	test_obj.set_protocol_module((l7vs::protocol_module_base*)&proto_test);
	boost::thread::id proc_id = boost::this_thread::get_id();
	
	l7vs::tcp_data& down_thread_data_dest_side = test_obj.get_down_thread_data_dest_side();
	
	// set sorryserver socket
	std::pair< boost::asio::ip::tcp::endpoint, boost::shared_ptr< l7vs::tcp_socket > >& socket_pair = test_obj.get_sorryserver_socket();
	boost::asio::ip::tcp::endpoint& sorry_end = socket_pair.first;
	l7vs::tcp_socket& socket = *(socket_pair.second);
	sorry_end.address(boost::asio::ip::address::from_string(DUMMI_SERVER_IP));
	sorry_end.port(7000);
	// socket set
	socket.read_some_res = MAX_BUFFER_SIZE;
	socket.read_some_ec.clear();
	socket.is_open_res = true;
	char set_char = CHAR_MIN;
	for(int j = 0;j < MAX_BUFFER_SIZE;j++){
		socket.read_some_buffers_out[j] = set_char;
		if(set_char == CHAR_MAX)
			set_char = CHAR_MIN;
		else
			set_char++;
	}
	socket.read_some_buffers_size_in = 0;
	socket.read_some_call_check = false;
	// tcp_session set
	down_thread_data_dest_side.initialize();
	test_obj.set_down_thread_id(proc_id);
	test_obj.set_up_thread_id(boost::thread::id());
	test_obj.down_thread_client_connection_chk_event_call_check = false;
	test_obj.down_thread_realserver_disconnect_call_check = false;
	test_obj.down_thread_exit_call_check = false;
	test_obj.down_thread_realserver_receive_call_check = false;
	// vs set
	vs.get_qos_downstream_res = 104857600;
	vs.get_throughput_downstream_res = 0;
	vs.update_down_recv_size_in = 0;
	// protocol module set
	proto_test.handle_sorryserver_recv_res_tag = l7vs::protocol_module_base::CLIENT_CONNECTION_CHECK;
	proto_test.handle_sorryserver_recv_in_thread_id = boost::thread::id();
	for(int i = 0;i < MAX_BUFFER_SIZE;i++){
		proto_test.handle_sorryserver_recv_in_recvbuffer[i] = '\0';
	}
	proto_test.handle_sorryserver_recv_in_recvlen = 0;
	proto_test.handle_sorryserver_recv_in_sorry_endpoint = boost::asio::ip::tcp::endpoint();
	
	l7vs::Logger::test_loglevel = l7vs::LOG_LV_DEBUG;
	l7vs::Logger::putLogDebug_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogDebug_id = 0;

	test_obj.test_call_sorryserver_receive();

	// unit_test [1] down_thread_sorryserver_receive sorryserver read_some call check
	std::cout << "[1] down_thread_sorryserver_receive sorryserver read_some call check" << std::endl;
	BOOST_CHECK(socket.read_some_call_check);
	BOOST_CHECK(socket.read_some_buffers_out == down_thread_data_dest_side.get_data());
	BOOST_CHECK(socket.read_some_buffers_size_in == MAX_BUFFER_SIZE);
	BOOST_CHECK(socket.read_some_res == down_thread_data_dest_side.get_size());
	BOOST_CHECK(sorry_end == down_thread_data_dest_side.get_endpoint());
	
	// unit_test [2] down_thread_sorryserver_receive protocol module handle_sorryserver_recv call check
	std::cout << "[2] down_thread_sorryserver_receive protocol module handle_sorryserver_recv call check" << std::endl;
	BOOST_CHECK(proto_test.handle_sorryserver_recv_in_thread_id == proc_id);
	BOOST_CHECK(proto_test.handle_sorryserver_recv_in_recvlen == socket.read_some_res);
	BOOST_CHECK(proto_test.handle_sorryserver_recv_in_recvbuffer == socket.read_some_buffers_out);
	BOOST_CHECK(proto_test.handle_sorryserver_recv_in_sorry_endpoint == sorry_end);
	
	// unit_test [3] down_thread_sorryserver_receive down_thread_next_call_function update check
	std::cout << "[3] down_thread_sorryserver_receive down_thread_next_call_function update check" << std::endl;
	test_obj.next_down_function_call();
	BOOST_CHECK(test_obj.down_thread_client_connection_chk_event_call_check);
	
	// unit_test [4] down_thread_sorryserver_receive debug log check
	std::cout << "[4] down_thread_sorryserver_receive debug log check" << std::endl;
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogDebug_category);
	BOOST_CHECK_EQUAL(46,l7vs::Logger::putLogDebug_id);
	std::cout << l7vs::Logger::putLogDebug_message << std::endl;

	// unit_test [5] down_thread_sorryserver_receive receive size 0 check
	std::cout << "[5] down_thread_sorryserver_receive receive size 0 check" << std::endl;
	test_obj.set_down_thread_next_function_call_exit();
	socket.read_some_res = 0;
	test_obj.test_call_sorryserver_receive();
	test_obj.down_thread_sorryserver_receive_call_check = false;
	test_obj.next_down_function_call();
	BOOST_CHECK(test_obj.down_thread_sorryserver_receive_call_check);
	socket.read_some_res = MAX_BUFFER_SIZE;
	
	// unit_test [6] down_thread_sorryserver_receive socket error (try_again) check
	std::cout << "[6] down_thread_sorryserver_receive socket error (try_again) check" << std::endl;
	test_obj.set_down_thread_next_function_call_exit();
	socket.read_some_ec = boost::asio::error::try_again;
	test_obj.test_call_sorryserver_receive();
	test_obj.down_thread_sorryserver_receive_call_check = false;
	test_obj.next_down_function_call();
	BOOST_CHECK(test_obj.down_thread_sorryserver_receive_call_check);
	socket.read_some_ec.clear();
	
	// unit_test [7] down_thread_sorryserver_receive socket error (eof) check
	std::cout << "[7] down_thread_sorryserver_receive socket error (eof) check" << std::endl;
	test_obj.set_down_thread_next_function_call_exit();
	socket.read_some_ec = boost::asio::error::eof;
	test_obj.test_call_sorryserver_receive();
	test_obj.down_thread_sorryserver_disconnect_call_check = false;
	test_obj.next_down_function_call();
	BOOST_CHECK(test_obj.down_thread_sorryserver_disconnect_call_check);
	socket.read_some_ec.clear();
	
	// unit_test [8] down_thread_sorryserver_receive socket error (connection_reset) check
	std::cout << "[8] down_thread_sorryserver_receive socket error (connection_reset) check" << std::endl;
	test_obj.set_down_thread_next_function_call_exit();
	socket.read_some_ec = boost::asio::error::connection_reset;
	test_obj.test_call_sorryserver_receive();
	test_obj.down_thread_sorryserver_disconnect_call_check = false;
	test_obj.next_down_function_call();
	BOOST_CHECK(test_obj.down_thread_sorryserver_disconnect_call_check);
	socket.read_some_ec.clear();
	
	// unit_test [9] down_thread_sorryserver_receive socket error (bad_descriptor) check
	std::cout << "[9] down_thread_sorryserver_receive socket error (bad_descriptor) check" << std::endl;
	test_obj.set_down_thread_next_function_call_exit();
	socket.read_some_ec = boost::asio::error::bad_descriptor;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call_sorryserver_receive();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_NONE,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(0,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	test_obj.down_thread_sorryserver_disconnect_call_check = false;
	test_obj.next_down_function_call();
	BOOST_CHECK(test_obj.down_thread_sorryserver_disconnect_call_check);
	socket.read_some_ec.clear();

	// unit_test [10] down_thread_sorryserver_receive closed socket error (bad_descriptor) check
	std::cout << "[10] down_thread_sorryserver_receive closed socket error (bad_descriptor) check" << std::endl;
	test_obj.set_down_thread_next_function_call_exit();
	socket.read_some_ec = boost::asio::error::bad_descriptor;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	socket.is_open_res = false;
	test_obj.test_call_sorryserver_receive();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_NONE,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(0,l7vs::Logger::putLogError_id);
	test_obj.down_thread_sorryserver_disconnect_call_check = false;
	test_obj.next_down_function_call();
	BOOST_CHECK(test_obj.down_thread_sorryserver_disconnect_call_check);
	socket.read_some_ec.clear();

	// unit_test [11] down_thread_sorryserver_receive not fond function error check
	std::cout << "[11] down_thread_sorryserver_receive not fond function error check" << std::endl;
	test_obj.down_thread_function_array_clear();
	test_obj.down_thread_exit_call_check = false;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call_sorryserver_receive();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(89,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	BOOST_CHECK(test_obj.down_thread_exit_call_check);

	// unit_test [12] down_thread_sorryserver_receive returnd illegal EVENT_TAG error check
	std::cout << "[12] down_thread_sorryserver_receive returnd illegal EVENT_TAG error check" << std::endl;
	test_obj.down_thread_module_event_map_clear();
	test_obj.down_thread_exit_call_check = false;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call_sorryserver_receive();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(88,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	BOOST_CHECK(test_obj.down_thread_exit_call_check);

	BOOST_MESSAGE( "----- down_thread_sorryserver_receive test end -----" );
}

// up_thread_realserver_send test
void up_thread_realserver_send_test(){
	BOOST_MESSAGE( "----- up_thread_realserver_send test end -----" );
	boost::asio::io_service io;
	l7vs::virtualservice_tcp vs;
	std::string test_protocol_name("test protocol");
	l7vs::test_protocol_module proto_test(test_protocol_name);
	// up_thread_realserver_send
	receive_send_test_class test_obj(vs,io);
	test_obj.set_protocol_module((l7vs::protocol_module_base*)&proto_test);
	boost::thread::id proc_id = boost::this_thread::get_id();
	
	l7vs::tcp_data& send_data = test_obj.get_up_thread_data_dest_side();
	
	// set realserver socket
	std::map<boost::asio::ip::tcp::endpoint, boost::shared_ptr< l7vs::tcp_socket > >& rs_map = test_obj.get_up_thread_send_realserver_socket_map();
	std::map<boost::asio::ip::tcp::endpoint, boost::shared_ptr< l7vs::tcp_socket > >::iterator rs_it;
	std::pair< boost::asio::ip::tcp::endpoint, boost::shared_ptr< l7vs::tcp_socket > > push_pair;
	for(int i = 0 ; i < 3 ;i++){
		boost::shared_ptr< l7vs::tcp_socket > new_socket(new l7vs::tcp_socket(io));
		push_pair.first.address(boost::asio::ip::address::from_string(DUMMI_SERVER_IP));
		push_pair.first.port(7000+i);
		push_pair.second = new_socket;
		new_socket->write_some_res = MAX_BUFFER_SIZE;
		new_socket->write_some_ec.clear();
		new_socket->write_some_buffers_in = NULL;
		new_socket->write_some_buffers_size_in = 0;
		new_socket->write_some_call_check = false;
		rs_map.insert(push_pair);
	}
	
	// tcp_session set
	send_data.initialize();
	send_data.set_size(MAX_BUFFER_SIZE);
	boost::asio::ip::tcp::endpoint send_end(boost::asio::ip::address::from_string(DUMMI_SERVER_IP),7002);
	send_data.set_endpoint(send_end);
	test_obj.set_up_thread_id(proc_id);
	test_obj.set_down_thread_id(boost::thread::id());
	test_obj.up_thread_client_receive_call_check = false;
	test_obj.up_thread_realserver_disconnect_call_check = false;
	test_obj.up_thread_exit_call_check = false;
	test_obj.up_thread_realserver_get_destination_event_call_check = false;
	// vs set
	vs.update_up_send_size_in = 0;
	// protocol module set
	proto_test.handle_realserver_send_res_tag = l7vs::protocol_module_base::CLIENT_RECV;
	proto_test.handle_realserver_send_in_thread_id = boost::thread::id();
	
	rs_it = rs_map.find(send_end);
	l7vs::tcp_socket& socket = *(rs_it->second);
	
	l7vs::Logger::test_loglevel = l7vs::LOG_LV_DEBUG;
	l7vs::Logger::putLogDebug_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogDebug_id = 0;
	
	test_obj.test_call_realserver_send();
	
	// unit_test [1] up_thread_realserver_send realserver_socket write_some call check
	std::cout << "[1] up_thread_realserver_send realserver_socket write_some call check" << std::endl;
	BOOST_CHECK(socket.write_some_call_check);
	BOOST_CHECK(socket.write_some_buffers_in == (void*)&(send_data.get_data()));
	BOOST_CHECK(socket.write_some_buffers_size_in == MAX_BUFFER_SIZE);
	BOOST_CHECK(socket.write_some_res == send_data.get_send_size());
	
	// unit_test [2] up_thread_realserver_send virtualservice update_up_send_size_in call check
	std::cout << "[2] up_thread_realserver_send virtualservice update_up_send_size_in call check" << std::endl;
	BOOST_CHECK(vs.update_up_send_size_in == MAX_BUFFER_SIZE);
	
	// unit_test [3] up_thread_realserver_send protocol module handle_realserver_send call check
	std::cout << "[3] up_thread_realserver_send protocol module handle_realserver_send call check" << std::endl;
	BOOST_CHECK(proto_test.handle_realserver_send_in_thread_id == proc_id);
	
	// unit_test [4] up_thread_realserver_send up_thread_next_call_function update check
	std::cout << "[4] up_thread_realserver_send up_thread_next_call_function update check" << std::endl;
	test_obj.next_up_function_call();
	BOOST_CHECK(test_obj.up_thread_client_receive_call_check);

	// unit_test [5] up_thread_realserver_send debug log check
	std::cout << "[5] up_thread_realserver_send debug log check" << std::endl;
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogDebug_category);
	BOOST_CHECK_EQUAL(37,l7vs::Logger::putLogDebug_id);
	std::cout << l7vs::Logger::putLogDebug_message << std::endl;

	// unit_test [6] up_thread_realserver_send send size check
	std::cout << "[6] up_thread_realserver_send send size check" << std::endl;
	send_data.set_send_size(0);
	socket.write_some_res = 0;
	test_obj.set_up_thread_next_function_call_exit();
	socket.write_some_buffers_in = NULL;
	test_obj.test_call_realserver_send();
//	BOOST_CHECK((char*)socket.write_some_buffers_in == (char*)&(send_data.get_data()) + 0);
	BOOST_CHECK((char*)socket.write_some_buffers_in == (send_data.get_data().data() + 0));
	test_obj.up_thread_realserver_send_call_check = false;
	test_obj.next_up_function_call();
	BOOST_CHECK(test_obj.up_thread_realserver_send_call_check);
	
	socket.write_some_res = MAX_BUFFER_SIZE / 3;
	test_obj.set_up_thread_next_function_call_exit();
	socket.write_some_buffers_in = NULL;
	test_obj.test_call_realserver_send();
//	BOOST_CHECK((char*)socket.write_some_buffers_in == (char*)&(send_data.get_data()) + 0);
	BOOST_CHECK((char*)socket.write_some_buffers_in == (send_data.get_data().data() + 0));
	test_obj.up_thread_realserver_send_call_check = false;
	test_obj.next_up_function_call();
	BOOST_CHECK(test_obj.up_thread_realserver_send_call_check);
	
	socket.write_some_res = MAX_BUFFER_SIZE / 3;
	test_obj.set_up_thread_next_function_call_exit();
	socket.write_some_buffers_in = NULL;
	test_obj.test_call_realserver_send();
//	BOOST_CHECK((char*)socket.write_some_buffers_in == (char*)(&(send_data.get_data()) + (MAX_BUFFER_SIZE / 3)));
	BOOST_CHECK((char*)socket.write_some_buffers_in == (send_data.get_data().data() + (MAX_BUFFER_SIZE / 3)));
	test_obj.up_thread_realserver_send_call_check = false;
	test_obj.next_up_function_call();
	BOOST_CHECK(test_obj.up_thread_realserver_send_call_check);
	
	socket.write_some_res = MAX_BUFFER_SIZE - send_data.get_send_size();
	test_obj.set_up_thread_next_function_call_exit();
	socket.write_some_buffers_in = NULL;
	test_obj.test_call_realserver_send();
//	BOOST_CHECK((char*)socket.write_some_buffers_in == (char*)(&(send_data.get_data()) + (MAX_BUFFER_SIZE / 3) * 2));
	BOOST_CHECK((char*)socket.write_some_buffers_in == (send_data.get_data().data() + (MAX_BUFFER_SIZE / 3) * 2));
	test_obj.up_thread_client_receive_call_check = false;
	test_obj.next_up_function_call();
	BOOST_CHECK(test_obj.up_thread_client_receive_call_check);
	
	socket.write_some_res = MAX_BUFFER_SIZE;
	
	// unit_test [7] up_thread_realserver_send socket error (try_again) check
	std::cout << "[7] up_thread_realserver_send socket error (try_again) check" << std::endl;
	send_data.set_send_size(0);
	test_obj.set_up_thread_next_function_call_exit();
	socket.write_some_ec = boost::asio::error::try_again;
	test_obj.test_call_realserver_send();
	test_obj.up_thread_realserver_send_call_check = false;
	test_obj.next_up_function_call();
	BOOST_CHECK(test_obj.up_thread_realserver_send_call_check);
	socket.read_some_ec.clear();
	
	// unit_test [8] up_thread_realserver_send socket error (eof) check
	std::cout << "[8] up_thread_realserver_send socket error (eof) check" << std::endl;
	send_data.set_send_size(0);
	test_obj.set_up_thread_next_function_call_exit();
	socket.write_some_ec = boost::asio::error::eof;
	test_obj.test_call_realserver_send();
	test_obj.up_thread_realserver_disconnect_call_check = false;
	test_obj.next_up_function_call();
	BOOST_CHECK(test_obj.up_thread_realserver_disconnect_call_check);
	socket.read_some_ec.clear();
	
	// unit_test [9] up_thread_realserver_send socket error (connection_reset) check
	std::cout << "[9] up_thread_realserver_send socket error (connection_reset) check" << std::endl;
	send_data.set_send_size(0);
	test_obj.set_up_thread_next_function_call_exit();
	socket.write_some_ec = boost::asio::error::connection_reset;
	test_obj.test_call_realserver_send();
	test_obj.up_thread_realserver_disconnect_call_check = false;
	test_obj.next_up_function_call();
	BOOST_CHECK(test_obj.up_thread_realserver_disconnect_call_check);
	socket.read_some_ec.clear();
	
	// unit_test [10] up_thread_realserver_send socket error (bad_descriptor) check
	std::cout << "[10] up_thread_realserver_send socket error (bad_descriptor) check" << std::endl;
	send_data.set_send_size(0);
	test_obj.set_up_thread_next_function_call_exit();
	socket.write_some_ec = boost::asio::error::bad_descriptor;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call_realserver_send();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_NONE,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(0,l7vs::Logger::putLogError_id);
	test_obj.up_thread_realserver_disconnect_call_check = false;
	test_obj.next_up_function_call();
	BOOST_CHECK(test_obj.up_thread_realserver_disconnect_call_check);
	socket.read_some_ec.clear();
	
	// unit_test [11] up_thread_realserver_send not fond function error check
	std::cout << "[11] up_thread_realserver_send not fond function error check" << std::endl;
	send_data.set_send_size(0);
	test_obj.up_thread_function_array_clear();
	test_obj.up_thread_exit_call_check = false;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call_realserver_send();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(31,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	BOOST_CHECK(test_obj.up_thread_exit_call_check);
	
	// unit_test [12] up_thread_realserver_send returnd illegal EVENT_TAG error check
	std::cout << "[12] up_thread_realserver_send returnd illegal EVENT_TAG error check" << std::endl;
	send_data.set_send_size(0);
	send_data.set_size(MAX_BUFFER_SIZE);
	socket.write_some_ec.clear();
	test_obj.up_thread_module_event_map_clear();
	test_obj.up_thread_exit_call_check = false;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call_realserver_send();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(30,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	BOOST_CHECK(test_obj.up_thread_exit_call_check);
	
	BOOST_MESSAGE( "----- up_thread_realserver_send test end -----" );
}

// up_thread_sorryserver_send test
void up_thread_sorryserver_send_test(){
	BOOST_MESSAGE( "----- up_thread_sorryserver_send test end -----" );
	boost::asio::io_service io;
	l7vs::virtualservice_tcp vs;
	std::string test_protocol_name("test protocol");
	l7vs::test_protocol_module proto_test(test_protocol_name);
	// up_thread_sorryserver_send
	receive_send_test_class test_obj(vs,io);
	test_obj.set_protocol_module((l7vs::protocol_module_base*)&proto_test);
	boost::thread::id proc_id = boost::this_thread::get_id();
	
	l7vs::tcp_data& send_data = test_obj.get_up_thread_data_dest_side();
	
	// set sorryserver socket
	std::pair< boost::asio::ip::tcp::endpoint, boost::shared_ptr< l7vs::tcp_socket > >& sorry_pair = test_obj.get_sorryserver_socket();
	sorry_pair.first = boost::asio::ip::tcp::endpoint();
	
	// tcp_session set
	send_data.initialize();
	send_data.set_size(MAX_BUFFER_SIZE);
	boost::asio::ip::tcp::endpoint send_end(boost::asio::ip::address::from_string(DUMMI_SERVER_IP),7000);
	send_data.set_endpoint(send_end);
	test_obj.set_up_thread_id(proc_id);
	test_obj.set_down_thread_id(boost::thread::id());
	test_obj.up_thread_client_receive_call_check = false;
	test_obj.up_thread_sorryserver_disconnect_call_check = false;
	test_obj.up_thread_exit_call_check = false;
	test_obj.up_thread_sorryserver_get_destination_event_call_check = false;
	// vs set
	vs.update_up_send_size_in = 0;
	// protocol module set
	proto_test.handle_sorryserver_send_res_tag = l7vs::protocol_module_base::CLIENT_RECV;
	proto_test.handle_sorryserver_send_in_thread_id = boost::thread::id();
	
	l7vs::tcp_socket& socket = *(sorry_pair.second);
	socket.write_some_res = MAX_BUFFER_SIZE;
	socket.write_some_ec.clear();
	socket.write_some_buffers_in = NULL;
	socket.write_some_buffers_size_in = 0;
	socket.write_some_call_check = false;
	
	l7vs::Logger::test_loglevel = l7vs::LOG_LV_DEBUG;
	l7vs::Logger::putLogDebug_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogDebug_id = 0;
	
	test_obj.test_call_sorryserver_send();
	
	// unit_test [1] up_thread_sorryserver_send realserver_socket write_some call check
	std::cout << "[1] up_thread_sorryserver_send realserver_socket write_some call check" << std::endl;
	BOOST_CHECK(socket.write_some_call_check);
	BOOST_CHECK(socket.write_some_buffers_in == (void*)&(send_data.get_data()));
	BOOST_CHECK(socket.write_some_buffers_size_in == MAX_BUFFER_SIZE);
	BOOST_CHECK(socket.write_some_res == send_data.get_send_size());
	
	// unit_test [2] up_thread_sorryserver_send protocol module handle_realserver_send call check
	std::cout << "[2] up_thread_sorryserver_send protocol module handle_realserver_send call check" << std::endl;
	BOOST_CHECK(proto_test.handle_sorryserver_send_in_thread_id == proc_id);
	
	// unit_test [3] up_thread_sorryserver_send up_thread_next_call_function update check
	std::cout << "[3] up_thread_sorryserver_send up_thread_next_call_function update check" << std::endl;
	test_obj.next_up_function_call();
	BOOST_CHECK(test_obj.up_thread_client_receive_call_check);

	// unit_test [4] up_thread_sorryserver_send debug log check
	std::cout << "[4] up_thread_sorryserver_send debug log check" << std::endl;
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogDebug_category);
	BOOST_CHECK_EQUAL(35,l7vs::Logger::putLogDebug_id);
	std::cout << l7vs::Logger::putLogDebug_message << std::endl;

	// unit_test [5] up_thread_sorryserver_send send size check
	std::cout << "[5] up_thread_sorryserver_send send size check" << std::endl;
	send_data.set_send_size(0);
	socket.write_some_res = 0;
	test_obj.set_up_thread_next_function_call_exit();
	socket.write_some_buffers_in = NULL;
	test_obj.test_call_sorryserver_send();
//	BOOST_CHECK((char*)socket.write_some_buffers_in == (char*)&(send_data.get_data()) + 0);
	BOOST_CHECK((char*)socket.write_some_buffers_in == (send_data.get_data().data() + 0));
	test_obj.up_thread_sorryserver_send_call_check = false;
	test_obj.next_up_function_call();
	BOOST_CHECK(test_obj.up_thread_sorryserver_send_call_check);
	
	socket.write_some_res = MAX_BUFFER_SIZE / 3;
	test_obj.set_up_thread_next_function_call_exit();
	socket.write_some_buffers_in = NULL;
	test_obj.test_call_sorryserver_send();
//	BOOST_CHECK((char*)socket.write_some_buffers_in == (char*)&(send_data.get_data()) + 0);
	BOOST_CHECK((char*)socket.write_some_buffers_in == (send_data.get_data().data() + 0));
	test_obj.up_thread_sorryserver_send_call_check = false;
	test_obj.next_up_function_call();
	BOOST_CHECK(test_obj.up_thread_sorryserver_send_call_check);
	
	socket.write_some_res = MAX_BUFFER_SIZE / 3;
	test_obj.set_up_thread_next_function_call_exit();
	socket.write_some_buffers_in = NULL;
	test_obj.test_call_sorryserver_send();
//	BOOST_CHECK((char*)socket.write_some_buffers_in == (char*)(&(send_data.get_data()) + (MAX_BUFFER_SIZE / 3)));
	BOOST_CHECK((char*)socket.write_some_buffers_in == (send_data.get_data().data() + (MAX_BUFFER_SIZE / 3)));
	test_obj.up_thread_sorryserver_send_call_check = false;
	test_obj.next_up_function_call();
	BOOST_CHECK(test_obj.up_thread_sorryserver_send_call_check);
	
	socket.write_some_res = MAX_BUFFER_SIZE - send_data.get_send_size();
	test_obj.set_up_thread_next_function_call_exit();
	socket.write_some_buffers_in = NULL;
	test_obj.test_call_sorryserver_send();
//	BOOST_CHECK((char*)socket.write_some_buffers_in == (char*)(&(send_data.get_data()) + (MAX_BUFFER_SIZE / 3) * 2));
	BOOST_CHECK((char*)socket.write_some_buffers_in == (send_data.get_data().data() + (MAX_BUFFER_SIZE / 3) * 2));
	test_obj.up_thread_client_receive_call_check = false;
	test_obj.next_up_function_call();
	BOOST_CHECK(test_obj.up_thread_client_receive_call_check);
	
	socket.write_some_res = MAX_BUFFER_SIZE;
		
	// unit_test [6] up_thread_sorryserver_send socket error (try_again) check
	std::cout << "[6] up_thread_sorryserver_send socket error (try_again) check" << std::endl;
	send_data.set_send_size(0);
	test_obj.set_up_thread_next_function_call_exit();
	socket.write_some_ec = boost::asio::error::try_again;
	test_obj.test_call_sorryserver_send();
	test_obj.up_thread_sorryserver_send_call_check = false;
	test_obj.next_up_function_call();
	BOOST_CHECK(test_obj.up_thread_sorryserver_send_call_check);
	socket.read_some_ec.clear();
	
	// unit_test [7] up_thread_sorryserver_send socket error (eof) check
	std::cout << "[7] up_thread_sorryserver_send socket error (eof) check" << std::endl;
	send_data.set_send_size(0);
	test_obj.set_up_thread_next_function_call_exit();
	socket.write_some_ec = boost::asio::error::eof;
	test_obj.test_call_sorryserver_send();
	test_obj.up_thread_sorryserver_disconnect_call_check = false;
	test_obj.next_up_function_call();
	BOOST_CHECK(test_obj.up_thread_sorryserver_disconnect_call_check);
	socket.read_some_ec.clear();
	
	// unit_test [8] up_thread_sorryserver_send socket error (connection_reset) check
	std::cout << "[8] up_thread_sorryserver_send socket error (connection_reset) check" << std::endl;
	send_data.set_send_size(0);
	test_obj.set_up_thread_next_function_call_exit();
	socket.write_some_ec = boost::asio::error::connection_reset;
	test_obj.test_call_sorryserver_send();
	test_obj.up_thread_sorryserver_disconnect_call_check = false;
	test_obj.next_up_function_call();
	BOOST_CHECK(test_obj.up_thread_sorryserver_disconnect_call_check);
	socket.read_some_ec.clear();
	
	// unit_test [9] up_thread_sorryserver_send socket error (bad_descriptor) check
	std::cout << "[9] up_thread_sorryserver_send socket error (bad_descriptor) check" << std::endl;
	send_data.set_send_size(0);
	test_obj.set_up_thread_next_function_call_exit();
	socket.write_some_ec = boost::asio::error::bad_descriptor;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call_sorryserver_send();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_NONE,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(0,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	test_obj.up_thread_sorryserver_disconnect_call_check = false;
	test_obj.next_up_function_call();
	BOOST_CHECK(test_obj.up_thread_sorryserver_disconnect_call_check);
	socket.write_some_ec.clear();
	
	// unit_test [10] up_thread_sorryserver_send not fond function error check
	std::cout << "[10] up_thread_sorryserver_send not fond function error check" << std::endl;
	send_data.set_send_size(0);
	test_obj.up_thread_function_array_clear();
	test_obj.up_thread_exit_call_check = false;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call_sorryserver_send();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(50,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	BOOST_CHECK(test_obj.up_thread_exit_call_check);
	
	// unit_test [11] up_thread_sorryserver_send returnd illegal EVENT_TAG error check
	std::cout << "[11] up_thread_sorryserver_send returnd illegal EVENT_TAG error check" << std::endl;
	send_data.set_size(MAX_BUFFER_SIZE);
	send_data.set_send_size(0);
	test_obj.up_thread_module_event_map_clear();
	test_obj.up_thread_exit_call_check = false;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call_sorryserver_send();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(49,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	BOOST_CHECK(test_obj.up_thread_exit_call_check);
	
	BOOST_MESSAGE( "----- up_thread_sorryserver_send test end -----" );
}

// down_thread_client_send test
void down_thread_client_send_test(){
	BOOST_MESSAGE( "----- down_thread_client_send test end -----" );
	boost::asio::io_service io;
	l7vs::virtualservice_tcp vs;
	std::string test_protocol_name("test protocol");
	l7vs::test_protocol_module proto_test(test_protocol_name);
	// up_thread_sorryserver_send
	receive_send_test_class test_obj(vs,io);
	test_obj.set_protocol_module((l7vs::protocol_module_base*)&proto_test);
	boost::thread::id proc_id = boost::this_thread::get_id();
	
	l7vs::tcp_data& send_data = test_obj.get_down_thread_data_client_side();
	
	// get client socket
	l7vs::tcp_socket& socket = test_obj.get_client_socket();
	
	// dummy server client socket connect
	boost::system::error_code ec;
	test_mirror_server test_server;
	// accept req
	test_server.breq_acc_flag = true;
	// close wait req
	test_server.breq_close_wait_flag = true;
	// recv cont
	test_server.req_recv_cnt = 0;
	// test server start
	boost::thread server_thread(boost::bind(&test_mirror_server::run,&test_server));
	while( !test_server.brun_flag ){
		sleep(1);
	}
	
	boost::asio::ip::tcp::endpoint connect_end(boost::asio::ip::address::from_string(DUMMI_SERVER_IP), DUMMI_SERVER_PORT);
	socket.get_socket().connect(connect_end,ec);
	BOOST_CHECK(!ec);
	socket.is_open_res = true;

	while(!test_server.bconnect_flag){
		sleep(1);
	}
	boost::asio::ip::tcp::endpoint local_endpoint = socket.get_socket().local_endpoint(ec);
	BOOST_CHECK(!ec);

	// tcp_session set
	send_data.initialize();
	send_data.set_size(MAX_BUFFER_SIZE);
	test_obj.set_up_thread_id(boost::thread::id());
	test_obj.set_down_thread_id(proc_id);
	test_obj.down_thread_realserver_receive_call_check = false;
	test_obj.down_thread_client_disconnect_call_check = false;
	test_obj.down_thread_exit_call_check = false;
	test_obj.down_thread_client_connection_chk_event_call_check = false;
	
	// vs set
	vs.update_down_send_size_in = 0;
	// protocol module set
	proto_test.handle_client_send_res_tag = l7vs::protocol_module_base::REALSERVER_RECV;
	proto_test.handle_client_send_in_thread_id = boost::thread::id();
	
	socket.write_some_res = MAX_BUFFER_SIZE;
	socket.write_some_ec.clear();
	socket.write_some_buffers_in = NULL;
	socket.write_some_buffers_size_in = 0;
	socket.write_some_call_check = false;

	l7vs::Logger::test_loglevel = l7vs::LOG_LV_DEBUG;
	l7vs::Logger::putLogDebug_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogDebug_id = 0;
	
	test_obj.test_call_client_send();
	
	// unit_test [1] down_thread_client_send socket write_some call check
	std::cout << "[1] down_thread_client_send socket write_some call check" << std::endl;
	BOOST_CHECK(socket.write_some_call_check);
	BOOST_CHECK(socket.write_some_buffers_in == (void*)&(send_data.get_data()));
	BOOST_CHECK(socket.write_some_buffers_size_in == MAX_BUFFER_SIZE);
	BOOST_CHECK(socket.write_some_res == send_data.get_send_size());
	
	// unit_test [2] down_thread_client_send protocol module handle_client_send call check
	std::cout << "[2] down_thread_client_send protocol module handle_client_send call check" << std::endl;
	BOOST_CHECK(proto_test.handle_client_send_in_thread_id == proc_id);
	
	// unit_test [3] down_thread_client_send up_thread_next_call_function update check
	std::cout << "[3] down_thread_client_send up_thread_next_call_function update check" << std::endl;
	test_obj.next_down_function_call();
	BOOST_CHECK(test_obj.down_thread_realserver_receive_call_check);

	// unit_test [4] down_thread_client_send send debug log check
	std::cout << "[4] down_thread_client_send  send debug log check" << std::endl;
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogDebug_category);
	BOOST_CHECK_EQUAL(45,l7vs::Logger::putLogDebug_id);
	std::cout << l7vs::Logger::putLogDebug_message << std::endl;
	
	// unit_test [5] down_thread_client_send send size check
	std::cout << "[5] down_thread_client_send send size check" << std::endl;
	send_data.set_send_size(0);
	socket.write_some_res = 0;
	test_obj.set_down_thread_next_function_call_exit();
	socket.write_some_buffers_in = NULL;
	test_obj.test_call_client_send();
//	BOOST_CHECK((char*)socket.write_some_buffers_in == (char*)&(send_data.get_data()) + 0);
	BOOST_CHECK((char*)socket.write_some_buffers_in == (send_data.get_data().data() + 0));
	test_obj.down_thread_client_send_call_check = false;
	test_obj.next_down_function_call();
	BOOST_CHECK(test_obj.down_thread_client_send_call_check);
	
	socket.write_some_res = MAX_BUFFER_SIZE / 3;
	test_obj.set_down_thread_next_function_call_exit();
	socket.write_some_buffers_in = NULL;
	test_obj.test_call_client_send();
//	BOOST_CHECK((char*)socket.write_some_buffers_in == (char*)&(send_data.get_data()) + 0);
	BOOST_CHECK((char*)socket.write_some_buffers_in == (send_data.get_data().data() + 0));
	test_obj.down_thread_client_send_call_check = false;
	test_obj.next_down_function_call();
	BOOST_CHECK(test_obj.down_thread_client_send_call_check);
	
	socket.write_some_res = MAX_BUFFER_SIZE / 3;
	test_obj.set_down_thread_next_function_call_exit();
	socket.write_some_buffers_in = NULL;
	test_obj.test_call_client_send();
//	BOOST_CHECK((char*)socket.write_some_buffers_in == (char*)(&(send_data.get_data()) + (MAX_BUFFER_SIZE / 3)));
	BOOST_CHECK((char*)socket.write_some_buffers_in == (send_data.get_data().data() + (MAX_BUFFER_SIZE / 3)));
	test_obj.down_thread_client_send_call_check = false;
	test_obj.next_down_function_call();
	BOOST_CHECK(test_obj.down_thread_client_send_call_check);
	
	socket.write_some_res = MAX_BUFFER_SIZE - send_data.get_send_size();
	test_obj.set_down_thread_next_function_call_exit();
	socket.write_some_buffers_in = NULL;
	test_obj.test_call_client_send();
//	BOOST_CHECK((char*)socket.write_some_buffers_in == (char*)(&(send_data.get_data()) + (MAX_BUFFER_SIZE / 3) * 2));
	BOOST_CHECK((char*)socket.write_some_buffers_in == (send_data.get_data().data() + (MAX_BUFFER_SIZE / 3) * 2));
	test_obj.down_thread_realserver_receive_call_check = false;
	test_obj.next_down_function_call();
	BOOST_CHECK(test_obj.down_thread_realserver_receive_call_check);
	
	socket.write_some_res = MAX_BUFFER_SIZE;
		
	// unit_test [6] down_thread_client_send socket error (try_again) check
	std::cout << "[6] down_thread_client_send socket error (try_again) check" << std::endl;
	send_data.set_send_size(0);
	test_obj.set_down_thread_next_function_call_exit();
	socket.write_some_ec = boost::asio::error::try_again;
	test_obj.test_call_client_send();
	test_obj.down_thread_client_send_call_check = false;
	test_obj.next_down_function_call();
	BOOST_CHECK(test_obj.down_thread_client_send_call_check);
	socket.read_some_ec.clear();
	
	// unit_test [7] down_thread_client_send socket error (eof) check
	std::cout << "[7] down_thread_client_send socket error (eof) check" << std::endl;
	send_data.set_send_size(0);
	test_obj.set_down_thread_next_function_call_exit();
	socket.write_some_ec = boost::asio::error::eof;
	test_obj.test_call_client_send();
	test_obj.down_thread_client_disconnect_call_check = false;
	test_obj.next_down_function_call();
	BOOST_CHECK(test_obj.down_thread_client_disconnect_call_check);
	socket.read_some_ec.clear();
	
	// unit_test [8] down_thread_client_send socket error (connection_reset) check
	std::cout << "[8] down_thread_client_send socket error (connection_reset) check" << std::endl;
	send_data.set_send_size(0);
	test_obj.set_down_thread_next_function_call_exit();
	socket.write_some_ec = boost::asio::error::connection_reset;
	test_obj.test_call_client_send();
	test_obj.down_thread_client_disconnect_call_check = false;
	test_obj.next_down_function_call();
	BOOST_CHECK(test_obj.down_thread_client_disconnect_call_check);
	socket.read_some_ec.clear();
	
	// unit_test [9] down_thread_client_send socket error (bad_descriptor) check
	std::cout << "[9] down_thread_client_send socket error (bad_descriptor) check" << std::endl;
	send_data.set_send_size(0);
	test_obj.set_down_thread_next_function_call_exit();
	socket.write_some_ec = boost::asio::error::bad_descriptor;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call_client_send();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_NONE,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(0,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	test_obj.down_thread_client_disconnect_call_check = false;
	test_obj.next_down_function_call();
	BOOST_CHECK(test_obj.down_thread_client_disconnect_call_check);
	socket.write_some_ec.clear();
	
	// unit_test [10] down_thread_client_send not fond function error check
	std::cout << "[10] down_thread_client_send not fond function error check" << std::endl;
	send_data.set_send_size(0);
	test_obj.down_thread_function_array_clear();
	test_obj.down_thread_exit_call_check = false;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call_client_send();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(83,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	BOOST_CHECK(test_obj.down_thread_exit_call_check);
	
	// unit_test [11] down_thread_client_send returnd illegal EVENT_TAG error check
	std::cout << "[11] down_thread_client_send returnd illegal EVENT_TAG error check" << std::endl;
	send_data.set_send_size(0);
	test_obj.down_thread_module_event_map_clear();
	test_obj.down_thread_exit_call_check = false;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call_client_send();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(82,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	BOOST_CHECK(test_obj.down_thread_exit_call_check);
	
	// dummy server stop
	test_server.breq_close_wait_flag = false;	
	test_server.bstop_flag = true;
	server_thread.join();

	BOOST_MESSAGE( "----- down_thread_client_send test end -----" );
}

//up_thread_realserver_connect test
//up_thread_realserver_connect test class 
class up_thread_realserver_connect_test_class : public l7vs::tcp_session{
	public:
		up_thread_realserver_connect_test_class(l7vs::virtualservice_tcp& vs,boost::asio::io_service& session_io,const l7vs::tcp_socket::tcp_socket_option_info set_option) : l7vs::tcp_session(vs,session_io,set_option){
		};
		~up_thread_realserver_connect_test_class(){};
		
		void test_call(){
			l7vs::tcp_session::up_thread_realserver_connect(LOCAL_PROC);
		};
		
		void set_protocol_module(l7vs::protocol_module_base* set_prot){
			protocol_module = set_prot;
		};
		l7vs::tcp_data& get_up_thread_data_client_side(){
			return up_thread_data_client_side;
		};
		l7vs::tcp_data& get_up_thread_data_dest_side(){
			return up_thread_data_dest_side;
		};
		l7vs::tcp_data& get_down_thread_data_client_side(){
			return down_thread_data_client_side;
		};
		l7vs::tcp_data& get_down_thread_data_dest_side(){
			return down_thread_data_dest_side;
		};
		l7vs::tcp_socket& get_client_socket(){
			return client_socket;
		};
		socket_element& get_sorryserver_socket(){
			return sorryserver_socket;
		};
		std::list<socket_element>& get_down_thread_receive_realserver_socket_list(){
			return down_thread_receive_realserver_socket_list;
		};
		std::map<endpoint,tcp_socket_ptr>& get_up_thread_send_realserver_socket_map(){
			return up_thread_send_realserver_socket_map;
		};
		std::list<socket_element>::iterator& get_down_thread_current_receive_realserver_socket(){
			return down_thread_current_receive_realserver_socket;
		};
		l7vs::tcp_realserver_connect_socket_list& get_down_thread_connect_socket_list(){
			return down_thread_connect_socket_list;
		};
		
		
		void set_up_thread_id(boost::thread::id set_id){
			up_thread_id = set_id;
		}
		void set_down_thread_id(boost::thread::id set_id){
			down_thread_id = set_id;
		}
		
		void next_up_function_call(){
			up_thread_next_call_function.second(LOCAL_PROC);
		}
		void next_down_function_call(){
			down_thread_next_call_function.second(LOCAL_PROC);
		}
		
		void set_up_thread_next_function_call_exit(){
			up_thread_function_pair fun_it = up_thread_function_array[UP_FUNC_EXIT];
			up_thread_next_call_function = fun_it;
		};
		
		void set_down_thread_next_function_call_exit(){
			down_thread_function_pair fun_it = down_thread_function_array[DOWN_FUNC_EXIT];
			down_thread_next_call_function = fun_it;
		};
		
		// next call function check
		void up_thread_realserver_connect_event(const TCP_PROCESS_TYPE_TAG process_type){
			up_thread_realserver_connect_event_call_check = true;
		};
		bool up_thread_realserver_connect_event_call_check;
		void up_thread_realserver_connection_fail_event(const TCP_PROCESS_TYPE_TAG process_type){
			up_thread_realserver_connection_fail_event_check = true;
		};
		bool up_thread_realserver_connection_fail_event_check;
		
		void up_thread_exit(const TCP_PROCESS_TYPE_TAG process_type){
			up_thread_exit_call_check = true;
		}
		bool up_thread_exit_call_check;
		
		void down_thread_exit(const TCP_PROCESS_TYPE_TAG process_type){
			down_thread_exit_call_check = true;
		}
		bool down_thread_exit_call_check;
		
		// map clear
		void up_thread_function_array_clear(){
			for(int i = 0;i <= UP_FUNC_EXIT;i++){
				up_thread_function_array[i].second = NULL;
			}
		};
		void down_thread_function_array_clear(){
			for(int i = 0;i <= DOWN_FUNC_EXIT;i++){
				down_thread_function_array[i].second = NULL;
			}
		};
		
		void up_thread_module_event_map_clear(){
			up_thread_module_event_map.clear();
		};
		
		void down_thread_module_event_map_clear(){
			down_thread_module_event_map.clear();
		};
};

void up_thread_realserver_connect_test(){
	BOOST_MESSAGE( "----- up_thread_realserver_connect test start -----" );
	
	boost::asio::io_service io;
	l7vs::virtualservice_tcp vs;
	std::string test_protocol_name("test protocol");
	l7vs::test_protocol_module proto_test(test_protocol_name);
	
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
	
	// up_thread_sorryserver_send
//	up_thread_realserver_connect_test_class test_obj(vs,io);
	up_thread_realserver_connect_test_class test_obj(vs,io,set_option);
	test_obj.set_protocol_module((l7vs::protocol_module_base*)&proto_test);
	boost::thread::id proc_id = boost::this_thread::get_id();
	
	l7vs::tcp_data& con_data = test_obj.get_up_thread_data_dest_side();
	boost::asio::ip::tcp::endpoint con_end(boost::asio::ip::address::from_string(DUMMI_SERVER_IP),DUMMI_SERVER_PORT);
	
	//up_thread_data_dest_side set
	con_data.initialize();
	con_data.set_endpoint(con_end);
	// thread_id set
	test_obj.set_up_thread_id(boost::thread::id());
	test_obj.set_down_thread_id(proc_id);
	// socket set
	l7vs::tcp_socket::connect_res = true;
	l7vs::tcp_socket::connect_connect_endpoint = boost::asio::ip::tcp::endpoint();
	l7vs::tcp_socket::connect_ec = NULL;
	l7vs::tcp_socket::connect_call_check = false;
    l7vs::tcp_socket::is_connect = true;
	// vs set
	vs.connection_active_list.clear();
	// map set
	std::map< boost::asio::ip::tcp::endpoint, boost::shared_ptr< l7vs::tcp_socket > >& rs_map = test_obj.get_up_thread_send_realserver_socket_map();
	rs_map.clear();
	// connection list set
	l7vs::tcp_realserver_connect_socket_list& con_list = test_obj.get_down_thread_connect_socket_list();
	con_list.clear();
	//tcp_session set
	test_obj.set_up_thread_next_function_call_exit();
	test_obj.up_thread_realserver_connect_event_call_check = false;
	
    // dummy server 
    test_mirror_server test_server;
    // accept req
    test_server.breq_acc_flag = true;
    // close wait req
    test_server.breq_close_wait_flag = true;
    // recv cont
    test_server.req_recv_cnt = 0;
    // test server start
    boost::thread server_thread(boost::bind(&test_mirror_server::run,&test_server));
    while( !test_server.brun_flag ){
        sleep(1);
    }
	
	test_obj.test_call();
	
	// unit_test [1] up_thread_realserver_connect socket connect call check
	std::cout << "[1] up_thread_realserver_connect socket connect call check" << std::endl;
	BOOST_CHECK(con_end == l7vs::tcp_socket::connect_connect_endpoint);
	BOOST_CHECK(l7vs::tcp_socket::connect_ec != NULL);
	BOOST_CHECK(l7vs::tcp_socket::connect_call_check);
	
	// unit_test [2] up_thread_realserver_connect virtualservice connection_active call check
	std::cout << "[2] up_thread_realserver_connect virtualservice connection_active call check" << std::endl;
	BOOST_CHECK(!vs.connection_active_list.empty());
	BOOST_CHECK(*(vs.connection_active_list.begin()) == con_end);
	
	// unit_test [3] up_thread_realserver_connect up_thread_send_realserver_socket_map insert check
	std::cout << "[3] up_thread_realserver_connect up_thread_send_realserver_socket_map insert check" << std::endl;
	BOOST_CHECK(!rs_map.empty());
	BOOST_CHECK(rs_map.begin()->first == con_end);
	BOOST_CHECK(rs_map.begin()->second != NULL);
    std::cout << l7vs::Logger::putLogError_message << std::endl;
	
	// unit_test [4] up_thread_realserver_connect down_thread_connect_socket_list push check
	std::cout << "[4] up_thread_realserver_connect down_thread_connect_socket_list push check" << std::endl;
	BOOST_CHECK(!con_list.empty());
	std::pair<boost::asio::ip::tcp::endpoint,boost::shared_ptr<l7vs::tcp_socket> > set_socket = con_list.get_socket();
	BOOST_CHECK(set_socket.first == con_end);
	BOOST_CHECK(set_socket.second == rs_map.begin()->second);
	
	
	// unit_test [5] constructer realserver socket set socket option check
	std::cout << "[5] constructer realserver socket set socket option check" << std::endl;
	//! TCP_NODELAY
	BOOST_CHECK_EQUAL(rs_map.begin()->second->opt_info.nodelay_opt , set_option.nodelay_opt);
	BOOST_CHECK_EQUAL(rs_map.begin()->second->opt_info.nodelay_val , set_option.nodelay_val);
	//! TCP_CORK
	BOOST_CHECK_EQUAL(rs_map.begin()->second->opt_info.cork_opt , set_option.cork_opt);
	BOOST_CHECK_EQUAL(rs_map.begin()->second->opt_info.cork_val , set_option.cork_val);
	//! TCP_QUICKACK
	BOOST_CHECK_EQUAL(rs_map.begin()->second->opt_info.quickack_opt , set_option.quickack_opt);
	BOOST_CHECK_EQUAL(rs_map.begin()->second->opt_info.quickack_val , set_option.quickack_val);
	
	
	
	// unit_test [6] up_thread_realserver_connect up_thread_next_call_function update check
	std::cout << "[6] up_thread_realserver_connect up_thread_next_call_function update check" << std::endl;
	test_obj.next_up_function_call();
	BOOST_CHECK(test_obj.up_thread_realserver_connect_event_call_check);

    l7vs::tcp_socket::is_connect = false;
	
	// unit_test [7] up_thread_realserver_connect duplication check
	std::cout << "[7] up_thread_realserver_connect duplication check" << std::endl;
	l7vs::tcp_socket::connect_call_check = false;
	test_obj.test_call();
	BOOST_CHECK(!l7vs::tcp_socket::connect_call_check);
	test_obj.up_thread_realserver_connect_event_call_check = false;
	test_obj.next_up_function_call();
	BOOST_CHECK(test_obj.up_thread_realserver_connect_event_call_check);
	
	// unit_test [8] up_thread_realserver_connect connect fail check
	std::cout << "[8] up_thread_realserver_connect connect fail check" << std::endl;
	rs_map.clear();
	con_list.clear();
	l7vs::tcp_socket::connect_res = false;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(37,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	BOOST_CHECK(rs_map.empty());
	BOOST_CHECK(con_list.empty());
	test_obj.up_thread_realserver_connection_fail_event_check = false;
	test_obj.next_up_function_call();
	BOOST_CHECK(test_obj.up_thread_realserver_connection_fail_event_check);
	
	l7vs::tcp_socket::connect_res = true;
	
	// unit_test [9] up_thread_realserver_connect set non blocking fail check
	std::cout << "[9] up_thread_realserver_connect set non blocking fail check" << std::endl;
	rs_map.clear();
	con_list.clear();
	l7vs::tcp_socket::set_non_blocking_mode_res = false;
	l7vs::tcp_socket::set_non_blocking_mode_ec = boost::asio::error::bad_descriptor;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.up_thread_exit_call_check = false;
	test_obj.test_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(34,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	BOOST_CHECK(test_obj.up_thread_exit_call_check);
	l7vs::tcp_socket::set_non_blocking_mode_res = true;
	l7vs::tcp_socket::set_non_blocking_mode_ec.clear();

	// unit_test [10] up_thread_realserver_connect not fond function error check
	std::cout << "[10] up_thread_realserver_connect not fond function error check" << std::endl;
	rs_map.clear();
	con_list.clear();
	test_obj.up_thread_function_array_clear();
	test_obj.up_thread_exit_call_check = false;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(35,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	BOOST_CHECK(test_obj.up_thread_exit_call_check);
    
    
    set_socket.second->get_socket().close();
	
    // dummy server stop
    test_server.breq_close_wait_flag = false;   
    test_server.bstop_flag = true;
    server_thread.join();

	BOOST_MESSAGE( "----- up_thread_realserver_connect test end -----" );
}


//up_thread_sorryserver_connect test
//up_thread_sorryserver_connect test class 
class up_thread_sorryserver_connect_test_class : public l7vs::tcp_session{
	public:
		up_thread_sorryserver_connect_test_class(l7vs::virtualservice_tcp& vs,boost::asio::io_service& session_io) : l7vs::tcp_session(vs,session_io){
		};
		~up_thread_sorryserver_connect_test_class(){};
		
		void test_call(){
			l7vs::tcp_session::up_thread_sorryserver_connect(LOCAL_PROC);
		};
		
		void set_protocol_module(l7vs::protocol_module_base* set_prot){
			protocol_module = set_prot;
		};
		l7vs::tcp_data& get_up_thread_data_client_side(){
			return up_thread_data_client_side;
		};
		l7vs::tcp_data& get_up_thread_data_dest_side(){
			return up_thread_data_dest_side;
		};
		l7vs::tcp_data& get_down_thread_data_client_side(){
			return down_thread_data_client_side;
		};
		l7vs::tcp_data& get_down_thread_data_dest_side(){
			return down_thread_data_dest_side;
		};
		l7vs::tcp_socket& get_client_socket(){
			return client_socket;
		};
		socket_element& get_sorryserver_socket(){
			return sorryserver_socket;
		};
		std::list<socket_element>& get_down_thread_receive_realserver_socket_list(){
			return down_thread_receive_realserver_socket_list;
		};
		std::map<endpoint,tcp_socket_ptr>& get_up_thread_send_realserver_socket_map(){
			return up_thread_send_realserver_socket_map;
		};
		std::list<socket_element>::iterator& get_down_thread_current_receive_realserver_socket(){
			return down_thread_current_receive_realserver_socket;
		};
		l7vs::tcp_realserver_connect_socket_list& get_down_thread_connect_socket_list(){
			return down_thread_connect_socket_list;
		};
		
		
		void set_up_thread_id(boost::thread::id set_id){
			up_thread_id = set_id;
		}
		void set_down_thread_id(boost::thread::id set_id){
			down_thread_id = set_id;
		}
		
		void next_up_function_call(){
			up_thread_next_call_function.second(LOCAL_PROC);
		}
		void next_down_function_call(){
			down_thread_next_call_function.second(LOCAL_PROC);
		}
		
		void set_up_thread_next_function_call_exit(){
			up_thread_function_pair fun_it = up_thread_function_array[UP_FUNC_EXIT];
			up_thread_next_call_function = fun_it;
		};
		
		void set_down_thread_next_function_call_exit(){
			down_thread_function_pair fun_it = down_thread_function_array[DOWN_FUNC_EXIT];
			down_thread_next_call_function = fun_it;
		};
		
		// next call function check
		void up_thread_sorryserver_connect_event(const TCP_PROCESS_TYPE_TAG process_type){
			up_thread_sorryserver_connect_event_call_check = true;
		};
		bool up_thread_sorryserver_connect_event_call_check;
		
		void up_thread_sorryserver_connection_fail_event(const TCP_PROCESS_TYPE_TAG process_type){
			up_thread_sorryserver_connection_fail_event_check = true;
		};
		bool up_thread_sorryserver_connection_fail_event_check;
		
		void up_thread_exit(const TCP_PROCESS_TYPE_TAG process_type){
			up_thread_exit_call_check = true;
		}
		bool up_thread_exit_call_check;
		
		void down_thread_exit(const TCP_PROCESS_TYPE_TAG process_type){
			down_thread_exit_call_check = true;
		}
		bool down_thread_exit_call_check;
		
		// map clear
		void up_thread_function_array_clear(){
			for(int i = 0;i <= UP_FUNC_EXIT;i++){
				up_thread_function_array[i].second = NULL;
			}
		};
		void down_thread_function_map_clear(){
			for(int i = 0;i <= DOWN_FUNC_EXIT;i++){
				down_thread_function_array[i].second = NULL;
			}
		};
		
		void up_thread_module_event_map_clear(){
			up_thread_module_event_map.clear();
		};
		
		void down_thread_module_event_map_clear(){
			down_thread_module_event_map.clear();
		};
};

void up_thread_sorryserver_connect_test(){
	BOOST_MESSAGE( "----- up_thread_sorryserver_connect test start -----" );
	
	boost::asio::io_service io;
	l7vs::virtualservice_tcp vs;
	std::string test_protocol_name("test protocol");
	l7vs::test_protocol_module proto_test(test_protocol_name);
	// up_thread_sorryserver_send
	up_thread_sorryserver_connect_test_class test_obj(vs,io);
	test_obj.set_protocol_module((l7vs::protocol_module_base*)&proto_test);
	boost::thread::id proc_id = boost::this_thread::get_id();
	
	l7vs::tcp_data& con_data = test_obj.get_up_thread_data_dest_side();
	boost::asio::ip::tcp::endpoint con_end(boost::asio::ip::address::from_string(DUMMI_SERVER_IP),7000);
	
	//up_thread_data_dest_side set
	con_data.initialize();
	con_data.set_endpoint(con_end);
	// thread_id set
	test_obj.set_up_thread_id(boost::thread::id());
	test_obj.set_down_thread_id(proc_id);
	// socket set
	l7vs::tcp_socket::connect_res = true;
	l7vs::tcp_socket::connect_connect_endpoint = boost::asio::ip::tcp::endpoint();
	l7vs::tcp_socket::connect_ec = NULL;
	l7vs::tcp_socket::connect_call_check = false;
	//tcp_session set
	test_obj.set_up_thread_next_function_call_exit();
	test_obj.up_thread_sorryserver_connect_event_call_check = false;
	
	test_obj.test_call();
	
	// unit_test [1] up_thread_sorryserver_connect socket connect call check
	std::cout << "[1] up_thread_sorryserver_connect socket connect call check" << std::endl;
	BOOST_CHECK(con_end == l7vs::tcp_socket::connect_connect_endpoint);
	BOOST_CHECK(l7vs::tcp_socket::connect_ec != NULL);
	BOOST_CHECK(l7vs::tcp_socket::connect_call_check);
	
	// unit_test [2] up_thread_sorryserver_connect up_thread_next_call_function update check
	std::cout << "[2] up_thread_sorryserver_connect up_thread_next_call_function update check" << std::endl;
	test_obj.next_up_function_call();
	BOOST_CHECK(test_obj.up_thread_sorryserver_connect_event_call_check);
	
	// unit_test [3] up_thread_sorryserver_connect connect fail check
	std::cout << "[3] up_thread_sorryserver_connect connect fail check" << std::endl;
	l7vs::tcp_socket::connect_res = false;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(54,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	test_obj.up_thread_sorryserver_connection_fail_event_check = false;
	test_obj.next_up_function_call();
	BOOST_CHECK(test_obj.up_thread_sorryserver_connection_fail_event_check);
	
	l7vs::tcp_socket::connect_res = true;

	// unit_test [4] up_thread_realserver_connect set non blocking fail check
	std::cout << "[4] up_thread_realserver_connect set non blocking fail check" << std::endl;
	l7vs::tcp_socket::set_non_blocking_mode_res = false;
	l7vs::tcp_socket::set_non_blocking_mode_ec = boost::asio::error::bad_descriptor;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.up_thread_exit_call_check = false;
	test_obj.test_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(53,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	BOOST_CHECK(test_obj.up_thread_exit_call_check);
	l7vs::tcp_socket::set_non_blocking_mode_res = true;
	l7vs::tcp_socket::set_non_blocking_mode_ec.clear();
	
	// unit_test [5] up_thread_sorryserver_connect not fond function error check
	std::cout << "[5] up_thread_sorryserver_connect not fond function error check" << std::endl;
	test_obj.up_thread_function_array_clear();
	test_obj.up_thread_exit_call_check = false;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(55,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	BOOST_CHECK(test_obj.up_thread_exit_call_check);
	
	BOOST_MESSAGE( "----- up_thread_sorryserver_connect test end -----" );
}

//realserver_disconnect test class 
class realserver_disconnect_test_class : public l7vs::tcp_session{
	public:
		realserver_disconnect_test_class(l7vs::virtualservice_tcp& vs,boost::asio::io_service& session_io) : l7vs::tcp_session(vs,session_io){
		};
		~realserver_disconnect_test_class(){};
		
		
		void test_up_call(){
			l7vs::tcp_session::up_thread_realserver_disconnect(LOCAL_PROC);
		};
		void test_down_call(){
			l7vs::tcp_session::down_thread_realserver_disconnect(LOCAL_PROC);
		};
		
		void test_up_all_call(){
			l7vs::tcp_session::up_thread_all_realserver_disconnect(LOCAL_PROC);
		};
		void test_down_all_call(){
			l7vs::tcp_session::down_thread_all_realserver_disconnect(LOCAL_PROC);
		};
		void mutex_lock(){
			module_function_realserver_disconnect_mutex.wrlock();
		};
		void mutex_unlock(){
			module_function_realserver_disconnect_mutex.unlock();
		};
/*		bool mutex_trylock(){
			return module_function_realserver_disconnect_mutex.try_lock();
		};*/
		
		void set_protocol_module(l7vs::protocol_module_base* set_prot){
			protocol_module = set_prot;
		};
		l7vs::tcp_data& get_up_thread_data_client_side(){
			return up_thread_data_client_side;
		};
		l7vs::tcp_data& get_up_thread_data_dest_side(){
			return up_thread_data_dest_side;
		};
		l7vs::tcp_data& get_down_thread_data_client_side(){
			return down_thread_data_client_side;
		};
		l7vs::tcp_data& get_down_thread_data_dest_side(){
			return down_thread_data_dest_side;
		};
		l7vs::tcp_socket& get_client_socket(){
			return client_socket;
		};
		socket_element& get_sorryserver_socket(){
			return sorryserver_socket;
		};
		std::list<socket_element>& get_down_thread_receive_realserver_socket_list(){
			return down_thread_receive_realserver_socket_list;
		};
		std::map<endpoint,tcp_socket_ptr>& get_up_thread_send_realserver_socket_map(){
			return up_thread_send_realserver_socket_map;
		};
		std::list<socket_element>::iterator& get_down_thread_current_receive_realserver_socket(){
			return down_thread_current_receive_realserver_socket;
		};
		l7vs::tcp_realserver_connect_socket_list& get_down_thread_connect_socket_list(){
			return down_thread_connect_socket_list;
		};
		l7vs::lockfree_queue<l7vs::tcp_thread_message>& get_down_thread_message_que(){
			return down_thread_message_que;
		};
		l7vs::lockfree_queue<l7vs::tcp_thread_message>& get_up_thread_message_que(){
			return up_thread_message_que;
		};
		
		void set_up_thread_id(boost::thread::id set_id){
			up_thread_id = set_id;
		}
		void set_down_thread_id(boost::thread::id set_id){
			down_thread_id = set_id;
		}
		
		void next_up_function_call(){
			up_thread_next_call_function.second(LOCAL_PROC);
		}
		void next_down_function_call(){
			down_thread_next_call_function.second(LOCAL_PROC);
		}
		
		void set_up_thread_next_function_call_exit(){
			up_thread_function_pair fun_it = up_thread_function_array[UP_FUNC_EXIT];
			up_thread_next_call_function = fun_it;
		};
		
		void set_down_thread_next_function_call_exit(){
			down_thread_function_pair fun_it = down_thread_function_array[DOWN_FUNC_EXIT];
			down_thread_next_call_function = fun_it;
		};
		
		// next call function check
		void up_thread_realserver_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type){
			up_thread_realserver_disconnect_event_call_check = true;
		};
		bool up_thread_realserver_disconnect_event_call_check;
		
		void down_thread_realserver_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type){
			down_thread_realserver_disconnect_event_call_check = true;
		};
		bool down_thread_realserver_disconnect_event_call_check;
		
		void up_thread_client_disconnect(const TCP_PROCESS_TYPE_TAG process_type){
			up_thread_client_disconnect_call_check = true;
		};
		bool up_thread_client_disconnect_call_check;
		
		void down_thread_client_disconnect(const TCP_PROCESS_TYPE_TAG process_type){
			down_thread_client_disconnect_call_check = true;
		};
		bool down_thread_client_disconnect_call_check;
		
		void up_thread_exit(const TCP_PROCESS_TYPE_TAG process_type){
			up_thread_exit_call_check = true;
		}
		bool up_thread_exit_call_check;
		
		void down_thread_exit(const TCP_PROCESS_TYPE_TAG process_type){
			down_thread_exit_call_check = true;
		}
		bool down_thread_exit_call_check;
		
		// map clear
		void up_thread_function_array_clear(){
			for(int i = 0;i <= UP_FUNC_EXIT;i++){
				up_thread_function_array[i].second = NULL;
			}
		};
		void down_thread_function_array_clear(){
			for(int i = 0;i <= DOWN_FUNC_EXIT;i++){
				down_thread_function_array[i].second = NULL;
			}
		};
		
		void up_thread_module_event_map_clear(){
			up_thread_module_event_map.clear();
		};
		
		void down_thread_module_event_map_clear(){
			down_thread_module_event_map.clear();
		};
		
		void up_thread_message_down_thread_function_map_clear(){
			up_thread_message_down_thread_function_map.clear();
		};
		
		void down_thread_message_up_thread_function_map_clear(){
			down_thread_message_up_thread_function_map.clear();
		};
		
};
//up_thread_realserver_disconnect test
void up_thread_realserver_disconnect_test(){
	BOOST_MESSAGE( "----- up_thread_realserver_disconnect test start -----" );
	
	boost::asio::io_service io;
	l7vs::virtualservice_tcp vs;
	std::string test_protocol_name("test protocol");
	l7vs::test_protocol_module proto_test(test_protocol_name);
	realserver_disconnect_test_class test_obj(vs,io);
	test_obj.set_protocol_module((l7vs::protocol_module_base*)&proto_test);
	boost::thread::id proc_id = boost::this_thread::get_id();
	
	l7vs::tcp_data& discon_data = test_obj.get_up_thread_data_dest_side();
	
	//up_thread_data_dest_side set
	boost::asio::ip::tcp::endpoint discon_end(boost::asio::ip::address::from_string(DUMMI_SERVER_IP),7001);
	discon_data.initialize();
	discon_data.set_endpoint(discon_end);
	// thread_id set
	test_obj.set_down_thread_id(boost::thread::id());
	test_obj.set_up_thread_id(proc_id);
	// vs set
	vs.connection_inactive_list.clear();
	// map set
	// set realserver socket
	std::map<boost::asio::ip::tcp::endpoint, boost::shared_ptr< l7vs::tcp_socket > >& rs_map = test_obj.get_up_thread_send_realserver_socket_map();
	std::map<boost::asio::ip::tcp::endpoint, boost::shared_ptr< l7vs::tcp_socket > >::iterator rs_it;
	std::pair< boost::asio::ip::tcp::endpoint, boost::shared_ptr< l7vs::tcp_socket > > push_pair;
	for(int i = 0 ; i < 3 ;i++){
		boost::shared_ptr< l7vs::tcp_socket > new_socket(new l7vs::tcp_socket(io));
		push_pair.first.address(boost::asio::ip::address::from_string(DUMMI_SERVER_IP));
		push_pair.first.port(7000+i);
		push_pair.second = new_socket;
		new_socket->close_res = true;
		new_socket->close_out_ec.clear();
		new_socket->close_call_check = false;
		rs_map.insert(push_pair);
	}
	// message que
	l7vs::lockfree_queue<l7vs::tcp_thread_message>& down_thread_message_que	= test_obj.get_down_thread_message_que();
	l7vs::lockfree_queue<l7vs::tcp_thread_message>& up_thread_message_que	= test_obj.get_up_thread_message_que();
	while( !down_thread_message_que.empty() ){
		l7vs::tcp_thread_message*	tmp_ptr	= down_thread_message_que.pop();
		delete	tmp_ptr;
	}
	while( !up_thread_message_que.empty() ){
		l7vs::tcp_thread_message*	tmp_ptr	= up_thread_message_que.pop();
		delete	tmp_ptr;
	}
	
	//tcp_session set
	test_obj.set_up_thread_next_function_call_exit();
	test_obj.up_thread_realserver_disconnect_event_call_check = false;
	
	test_obj.test_up_call();
	
	rs_it = rs_map.find(discon_end);
	
	// unit_test [1] up_thread_realserver_disconnect socket close call check
	std::cout << "[1] up_thread_realserver_disconnect socket close call check" << std::endl;
	BOOST_CHECK(rs_it != rs_map.end());
	BOOST_CHECK(rs_it->second->close_call_check);
	
	// unit_test [2] up_thread_realserver_disconnect virtualservice connection_inactive call check
	std::cout << "[2] up_thread_realserver_disconnect virtualservice connection_inactive call check" << std::endl;
	BOOST_CHECK(!vs.connection_inactive_list.empty());
	BOOST_CHECK(*(vs.connection_inactive_list.begin()) == discon_end);
	
	// unit_test [3] up_thread_realserver_disconnect up thread message set check
	std::cout << "[3] up_thread_realserver_disconnect up thread message set check" << std::endl;
	BOOST_CHECK(!up_thread_message_que.empty());
	l7vs::tcp_thread_message*	up_msg		= up_thread_message_que.pop();
	BOOST_CHECK(up_thread_message_que.empty());
	BOOST_CHECK(up_msg->endpoint_info == discon_end);
	test_obj.up_thread_realserver_disconnect_event_call_check = false;
	up_msg->message(l7vs::tcp_session::MESSAGE_PROC);
	BOOST_CHECK(test_obj.up_thread_realserver_disconnect_event_call_check);
	delete	up_msg;

	// unit_test [4] up_thread_realserver_disconnect down thread message set check
	std::cout << "[4] up_thread_realserver_disconnect down thread message set check" << std::endl;
	BOOST_CHECK(!down_thread_message_que.empty());
	l7vs::tcp_thread_message*	down_msg	= down_thread_message_que.pop();
	BOOST_CHECK(down_thread_message_que.empty());
	BOOST_CHECK(down_msg->endpoint_info == discon_end);
	test_obj.down_thread_realserver_disconnect_event_call_check = false;
	down_msg->message(l7vs::tcp_session::MESSAGE_PROC);
	BOOST_CHECK(test_obj.down_thread_realserver_disconnect_event_call_check);
	delete	down_msg;

	// unit_test [5] up_thread_realserver_disconnect closed socket not set message check
	std::cout << "[5] up_thread_realserver_disconnect closed socket not set message check" << std::endl;
	rs_it->second->close_res = false;
	test_obj.test_up_call();
	BOOST_CHECK(up_thread_message_que.empty());
	BOOST_CHECK(down_thread_message_que.empty());
	
	rs_it->second->close_res = true;
	
	// unit_test [6] up_thread_realserver_disconnect not fond down_thread_realserver_disconnect_event function error check
	std::cout << "[6] up_thread_realserver_disconnect not fond down_thread_realserver_disconnect_event function error check" << std::endl;
	test_obj.up_thread_message_down_thread_function_map_clear();
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.up_thread_exit_call_check = false;
	test_obj.test_up_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(44,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	BOOST_CHECK(test_obj.up_thread_exit_call_check);
	
	// unit_test [7] up_thread_realserver_disconnect not fond up_thread_sorryserver_disconnect_event function error check
	std::cout << "[7] up_thread_realserver_disconnect not fond up_thread_sorryserver_disconnect_event function error check" << std::endl;
	test_obj.up_thread_function_array_clear();
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.up_thread_exit_call_check = false;
	test_obj.test_up_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(43,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	BOOST_CHECK(test_obj.up_thread_exit_call_check);
	
	BOOST_MESSAGE( "----- up_thread_realserver_disconnect test end -----" );
}

//down_thread_realserver_disconnect test
void down_thread_realserver_disconnect_test(){
	BOOST_MESSAGE( "----- down_thread_realserver_disconnect test start -----" );
	
	boost::asio::io_service io;
	l7vs::virtualservice_tcp vs;
	std::string test_protocol_name("test protocol");
	l7vs::test_protocol_module proto_test(test_protocol_name);
	realserver_disconnect_test_class test_obj(vs,io);
	test_obj.set_protocol_module((l7vs::protocol_module_base*)&proto_test);
	boost::thread::id proc_id = boost::this_thread::get_id();
	
	l7vs::tcp_data& discon_data = test_obj.get_down_thread_data_dest_side();
	
	//up_thread_data_dest_side set
	boost::asio::ip::tcp::endpoint discon_end(boost::asio::ip::address::from_string(DUMMI_SERVER_IP),7001);
	discon_data.initialize();
	discon_data.set_endpoint(discon_end);
	// thread_id set
	test_obj.set_up_thread_id(boost::thread::id());
	test_obj.set_down_thread_id(proc_id);
	// vs set
	vs.connection_inactive_list.clear();
	// map set
	// set realserver socket
	std::list< std::pair< boost::asio::ip::tcp::endpoint, boost::shared_ptr< l7vs::tcp_socket > > >& rs_list = test_obj.get_down_thread_receive_realserver_socket_list();
	std::list< std::pair< boost::asio::ip::tcp::endpoint, boost::shared_ptr< l7vs::tcp_socket > > >::iterator rs_it;
	std::pair< boost::asio::ip::tcp::endpoint, boost::shared_ptr< l7vs::tcp_socket > > push_pair;
	for(int i = 0 ; i < 3 ;i++){
		boost::shared_ptr< l7vs::tcp_socket > new_socket(new l7vs::tcp_socket(io));
		push_pair.first.address(boost::asio::ip::address::from_string(DUMMI_SERVER_IP));
		push_pair.first.port(7000+i);
		push_pair.second = new_socket;
		new_socket->close_res = true;
		new_socket->close_out_ec.clear();
		new_socket->close_call_check = false;
		rs_list.push_back(push_pair);
	}
	
	// message que
	l7vs::lockfree_queue<l7vs::tcp_thread_message>& down_thread_message_que = test_obj.get_down_thread_message_que();
	l7vs::lockfree_queue<l7vs::tcp_thread_message>& up_thread_message_que = test_obj.get_up_thread_message_que();
	while( !down_thread_message_que.empty() ){
		l7vs::tcp_thread_message*	tmp_ptr	= down_thread_message_que.pop();
		delete	tmp_ptr;
	}
	while( !up_thread_message_que.empty() ){
		l7vs::tcp_thread_message*	tmp_ptr	= up_thread_message_que.pop();
		delete	tmp_ptr;
	}
	
	//tcp_session set
	test_obj.set_down_thread_next_function_call_exit();
	test_obj.down_thread_realserver_disconnect_event_call_check = false;
	
	test_obj.test_down_call();
	
	rs_it = rs_list.begin();
	while(rs_it != rs_list.end()){
		if( rs_it->first == discon_end )
			break;
		rs_it++;
	}
	
	// unit_test [1] down_thread_realserver_disconnect socket close call check
	std::cout << "[1] down_thread_realserver_disconnect socket close call check" << std::endl;
	BOOST_CHECK(rs_it != rs_list.end());
	BOOST_CHECK(rs_it->second->close_call_check);
	
	// unit_test [2] down_thread_realserver_disconnect virtualservice connection_inactive call check
	std::cout << "[2] down_thread_realserver_disconnect virtualservice connection_inactive call check" << std::endl;
	BOOST_CHECK(!vs.connection_inactive_list.empty());
	BOOST_CHECK(*(vs.connection_inactive_list.begin()) == discon_end);
	
	// unit_test [3] down_thread_realserver_disconnect up thread message set check
	std::cout << "[3] down_thread_realserver_disconnect up thread message set check" << std::endl;
	BOOST_CHECK(!up_thread_message_que.empty());
	l7vs::tcp_thread_message*	up_msg		= up_thread_message_que.pop();
	BOOST_CHECK(up_thread_message_que.empty());
	BOOST_CHECK(up_msg->endpoint_info == discon_end);
	test_obj.up_thread_realserver_disconnect_event_call_check = false;
	up_msg->message(l7vs::tcp_session::MESSAGE_PROC);
	BOOST_CHECK(test_obj.up_thread_realserver_disconnect_event_call_check);
	delete	up_msg;

	// unit_test [4] up_thread_sorryserver_disconnect down thread message set check
	std::cout << "[4] up_thread_sorryserver_disconnect down thread message set check" << std::endl;
	BOOST_CHECK(!down_thread_message_que.empty());
	l7vs::tcp_thread_message*	down_msg	= down_thread_message_que.pop();
	BOOST_CHECK(down_thread_message_que.empty());
	BOOST_CHECK(down_msg->endpoint_info == discon_end);
	test_obj.down_thread_realserver_disconnect_event_call_check = false;
	down_msg->message(l7vs::tcp_session::MESSAGE_PROC);
	BOOST_CHECK(test_obj.down_thread_realserver_disconnect_event_call_check);
	delete	down_msg;

	// unit_test [5] down_thread_realserver_disconnect closed socket not set message check
	std::cout << "[5] down_thread_realserver_disconnect closed socket not set message check" << std::endl;
	rs_it->second->close_res = false;
	test_obj.test_down_call();
	BOOST_CHECK(up_thread_message_que.empty());
	BOOST_CHECK(down_thread_message_que.empty());
	
	rs_it->second->close_res = true;
	
	// unit_test [6] down_thread_realserver_disconnect not fond down_thread_realserver_disconnect_event function error check
	std::cout << "[6] down_thread_realserver_disconnect not fond down_thread_realserver_disconnect_event function error check" << std::endl;
	test_obj.down_thread_message_up_thread_function_map_clear();
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.down_thread_exit_call_check = false;
	test_obj.test_down_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(73,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	BOOST_CHECK(test_obj.down_thread_exit_call_check);
	
	// unit_test [7] down_thread_realserver_disconnect not fond up_thread_sorryserver_disconnect_event function error check
	std::cout << "[7] down_thread_realserver_disconnect not fond up_thread_sorryserver_disconnect_event function error check" << std::endl;
	test_obj.down_thread_function_array_clear();
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.down_thread_exit_call_check = false;
	test_obj.test_down_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(72,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	BOOST_CHECK(test_obj.down_thread_exit_call_check);
	
	BOOST_MESSAGE( "----- down_thread_realserver_disconnect test end -----" );
}


//up_thread_all_realserver_disconnect test
void up_thread_all_realserver_disconnect_test(){
	BOOST_MESSAGE( "----- up_thread_all_realserver_disconnect test start -----" );
	
	boost::asio::io_service io;
	l7vs::virtualservice_tcp vs;
	std::string test_protocol_name("test protocol");
	l7vs::test_protocol_module proto_test(test_protocol_name);
	realserver_disconnect_test_class test_obj(vs,io);
	test_obj.set_protocol_module((l7vs::protocol_module_base*)&proto_test);
	boost::thread::id proc_id = boost::this_thread::get_id();
	
	l7vs::tcp_data& discon_data = test_obj.get_up_thread_data_dest_side();
	
	//up_thread_data_dest_side set
	boost::asio::ip::tcp::endpoint discon_end(boost::asio::ip::address::from_string(DUMMI_SERVER_IP),7001);
	discon_data.initialize();
	discon_data.set_endpoint(discon_end);
	// thread_id set
	test_obj.set_down_thread_id(boost::thread::id());
	test_obj.set_up_thread_id(proc_id);
	// vs set
	vs.connection_inactive_list.clear();
	// map set
	// set realserver socket
	std::map<boost::asio::ip::tcp::endpoint, boost::shared_ptr< l7vs::tcp_socket > >& rs_map = test_obj.get_up_thread_send_realserver_socket_map();
	std::map<boost::asio::ip::tcp::endpoint, boost::shared_ptr< l7vs::tcp_socket > >::iterator rs_it;
	std::pair< boost::asio::ip::tcp::endpoint, boost::shared_ptr< l7vs::tcp_socket > > push_pair;
	std::list< std::pair< boost::asio::ip::tcp::endpoint, boost::shared_ptr< l7vs::tcp_socket > > > ref_rs_list;
	std::list< std::pair< boost::asio::ip::tcp::endpoint, boost::shared_ptr< l7vs::tcp_socket > > >::iterator ref_rs_it;
	for(int i = 0 ; i < 3 ;i++){
		boost::shared_ptr< l7vs::tcp_socket > new_socket(new l7vs::tcp_socket(io));
		push_pair.first.address(boost::asio::ip::address::from_string(DUMMI_SERVER_IP));
		push_pair.first.port(7000+i);
		push_pair.second = new_socket;
		new_socket->close_res = true;
		new_socket->close_out_ec.clear();
		new_socket->close_call_check = false;
		rs_map.insert(push_pair);
		ref_rs_list.push_back(push_pair);
	}
	// message que
	l7vs::lockfree_queue<l7vs::tcp_thread_message>& down_thread_message_que = test_obj.get_down_thread_message_que();
	l7vs::lockfree_queue<l7vs::tcp_thread_message>& up_thread_message_que = test_obj.get_up_thread_message_que();
	while( !down_thread_message_que.empty() ){
		l7vs::tcp_thread_message*	tmp_ptr	= down_thread_message_que.pop();
		delete	tmp_ptr;
	}
	while( !up_thread_message_que.empty() ){
		l7vs::tcp_thread_message*	tmp_ptr	= up_thread_message_que.pop();
		delete	tmp_ptr;
	}
	
	//tcp_session set
	test_obj.set_up_thread_next_function_call_exit();
	test_obj.up_thread_client_disconnect_call_check = false;
	
	//protocol module set
	proto_test.handle_realserver_disconnect_res_tag = l7vs::protocol_module_base::CLIENT_DISCONNECT;
	proto_test.handle_realserver_disconnect_thread_id = boost::thread::id();
	proto_test.handle_realserver_disconnect_rs_endpoint_list.clear();
	
	test_obj.test_up_all_call();
	
	// unit_test [1] up_thread_all_realserver_disconnect socket close call check
	std::cout << "[1] up_thread_all_realserver_disconnect socket close call check" << std::endl;
	ref_rs_it = ref_rs_list.begin();
	while(ref_rs_it != ref_rs_list.end()){
		BOOST_CHECK(ref_rs_it->second->close_call_check);
		ref_rs_it++;
	}

	// unit_test [2] up_thread_all_realserver_disconnect virtualservice connection_inactive call check
	std::cout << "[2] up_thread_all_realserver_disconnect virtualservice connection_inactive call check" << std::endl;
	BOOST_CHECK(!vs.connection_inactive_list.empty());
	ref_rs_it = ref_rs_list.begin();
	while(ref_rs_it != ref_rs_list.end()){
		std::list<boost::asio::ip::tcp::endpoint>::iterator end_list_it = vs.connection_inactive_list.begin();
		while(end_list_it == vs.connection_inactive_list.end()){
			if(ref_rs_it->first == *end_list_it)
				break;
			end_list_it++;
		}
		BOOST_CHECK(end_list_it != vs.connection_inactive_list.end());
		ref_rs_it++;
	}
	
	// unit_test [3] up_thread_all_realserver_disconnect handle_realserver_disconnect call check
	std::cout << "[3] up_thread_all_realserver_disconnect handle_realserver_disconnect call check" << std::endl;
	BOOST_CHECK(proto_test.handle_realserver_disconnect_thread_id == proc_id);
	BOOST_CHECK(!proto_test.handle_realserver_disconnect_rs_endpoint_list.empty());
	ref_rs_it = ref_rs_list.begin();
	while(ref_rs_it != ref_rs_list.end()){
		std::list<boost::asio::ip::tcp::endpoint>::iterator end_list_it = proto_test.handle_realserver_disconnect_rs_endpoint_list.begin();
		while(end_list_it == vs.connection_inactive_list.end()){
			if(ref_rs_it->first == *end_list_it)
				break;
			end_list_it++;
		}
		BOOST_CHECK(end_list_it != proto_test.handle_realserver_disconnect_rs_endpoint_list.end());
		ref_rs_it++;
	}
	
	// unit_test [4] up_thread_all_realserver_disconnect update up_thread_next_call_function check
	std::cout << "[4] up_thread_all_realserver_disconnect update up_thread_next_call_function check" << std::endl;
	test_obj.next_up_function_call();
	BOOST_CHECK(test_obj.up_thread_client_disconnect_call_check);
	
	// unit_test [5] up_thread_all_realserver_disconnect realserver not fond check
	std::cout << "[5] up_thread_all_realserver_disconnect realserver not fond check" << std::endl;
	proto_test.handle_realserver_disconnect_thread_id = boost::thread::id();
	proto_test.handle_realserver_disconnect_rs_endpoint_list.clear();
	test_obj.up_thread_client_disconnect_call_check = false;

	test_obj.test_up_all_call();

	BOOST_CHECK(proto_test.handle_realserver_disconnect_thread_id == proc_id);
	BOOST_CHECK(!proto_test.handle_realserver_disconnect_rs_endpoint_list.empty());
	BOOST_CHECK( *(proto_test.handle_realserver_disconnect_rs_endpoint_list.begin()) == boost::asio::ip::tcp::endpoint() );
	test_obj.next_up_function_call();
	BOOST_CHECK(test_obj.up_thread_client_disconnect_call_check);

	// unit_test [6] up_thread_all_realserver_disconnect handle_realserver_disconnect mutex lock check
	std::cout << "[6] up_thread_all_realserver_disconnect handle_realserver_disconnect mutex lock check" << std::endl;
	ref_rs_it = ref_rs_list.begin();
	while(ref_rs_it != ref_rs_list.end()){
		rs_map.insert(*ref_rs_it);
		ref_rs_it++;
	}
	proto_test.handle_realserver_disconnect_thread_id = boost::thread::id();
	test_obj.mutex_lock();
	boost::thread test_thread(boost::bind(&realserver_disconnect_test_class::test_up_all_call,&test_obj));
	sleep(1);
	BOOST_CHECK(proto_test.handle_realserver_disconnect_thread_id == boost::thread::id());
	
	// unit_test [7] up_thread_all_realserver_disconnect handle_realserver_disconnect mutex unlock run check
	std::cout << "[7] up_thread_all_realserver_disconnect handle_realserver_disconnect mutex unlock run check" << std::endl;
	boost::thread::id test_id = test_thread.get_id();
	test_obj.set_up_thread_id(test_id);
	test_obj.mutex_unlock();
	sleep(1);
	BOOST_CHECK(proto_test.handle_realserver_disconnect_thread_id == test_id);
	test_thread.join();
	
	// unit_test [8] up_thread_all_realserver_disconnect not fond function error check
	std::cout << "[8] up_thread_all_realserver_disconnect not fond function error check" << std::endl;
	ref_rs_it = ref_rs_list.begin();
	while(ref_rs_it != ref_rs_list.end()){
		rs_map.insert(*ref_rs_it);
		ref_rs_it++;
	}
	test_obj.up_thread_function_array_clear();
	test_obj.up_thread_exit_call_check = false;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_up_all_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(48,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	BOOST_CHECK(test_obj.up_thread_exit_call_check);
	
	// unit_test [9] up_thread_all_realserver_disconnect returnd illegal EVENT_TAG error check
	std::cout << "[9] up_thread_all_realserver_disconnect returnd illegal EVENT_TAG error check" << std::endl;
	ref_rs_it = ref_rs_list.begin();
	while(ref_rs_it != ref_rs_list.end()){
		rs_map.insert(*ref_rs_it);
		ref_rs_it++;
	}
	test_obj.up_thread_module_event_map_clear();
	test_obj.up_thread_exit_call_check = false;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_up_all_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(47,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	BOOST_CHECK(test_obj.up_thread_exit_call_check);
	
	BOOST_MESSAGE( "----- up_thread_all_realserver_disconnect test end -----" );
}

//down_thread_all_realserver_disconnect test
void down_thread_all_realserver_disconnect_test(){
	BOOST_MESSAGE( "----- down_thread_all_realserver_disconnect test start -----" );
	
	boost::asio::io_service io;
	l7vs::virtualservice_tcp vs;
	std::string test_protocol_name("test protocol");
	l7vs::test_protocol_module proto_test(test_protocol_name);
	realserver_disconnect_test_class test_obj(vs,io);
	test_obj.set_protocol_module((l7vs::protocol_module_base*)&proto_test);
	boost::thread::id proc_id = boost::this_thread::get_id();
	
	l7vs::tcp_data& discon_data = test_obj.get_down_thread_data_dest_side();
	
	//down_thread_data_dest_side set
	boost::asio::ip::tcp::endpoint discon_end(boost::asio::ip::address::from_string(DUMMI_SERVER_IP),7001);
	discon_data.initialize();
	discon_data.set_endpoint(discon_end);
	// thread_id set
	test_obj.set_up_thread_id(boost::thread::id());
	test_obj.set_down_thread_id(proc_id);
	// vs set
	vs.connection_inactive_list.clear();
	// map set
	// set realserver socket
	std::list< std::pair< boost::asio::ip::tcp::endpoint, boost::shared_ptr< l7vs::tcp_socket > > >& rs_list = test_obj.get_down_thread_receive_realserver_socket_list();
	std::list< std::pair< boost::asio::ip::tcp::endpoint, boost::shared_ptr< l7vs::tcp_socket > > >::iterator rs_it;
	std::pair< boost::asio::ip::tcp::endpoint, boost::shared_ptr< l7vs::tcp_socket > > push_pair;
	std::list< std::pair< boost::asio::ip::tcp::endpoint, boost::shared_ptr< l7vs::tcp_socket > > > ref_rs_list;
	std::list< std::pair< boost::asio::ip::tcp::endpoint, boost::shared_ptr< l7vs::tcp_socket > > >::iterator ref_rs_it;
	for(int i = 0 ; i < 3 ;i++){
		boost::shared_ptr< l7vs::tcp_socket > new_socket(new l7vs::tcp_socket(io));
		push_pair.first.address(boost::asio::ip::address::from_string(DUMMI_SERVER_IP));
		push_pair.first.port(7000+i);
		push_pair.second = new_socket;
		new_socket->close_res = true;
		new_socket->close_out_ec.clear();
		new_socket->close_call_check = false;
		rs_list.push_back(push_pair);
		ref_rs_list.push_back(push_pair);
	}
	// message que
	l7vs::lockfree_queue<l7vs::tcp_thread_message>&	down_thread_message_que		= test_obj.get_down_thread_message_que();
	l7vs::lockfree_queue<l7vs::tcp_thread_message>&	up_thread_message_que		= test_obj.get_up_thread_message_que();
	while( !down_thread_message_que.empty() ){
		l7vs::tcp_thread_message*	tmp_ptr	= down_thread_message_que.pop();
		delete	tmp_ptr;
	}
	while( !up_thread_message_que.empty() ){
		l7vs::tcp_thread_message*	tmp_ptr	= up_thread_message_que.pop();
		delete	tmp_ptr;
	}
	
	//tcp_session set
	test_obj.set_down_thread_next_function_call_exit();
	test_obj.down_thread_client_disconnect_call_check = false;
	
	//protocol module set
	proto_test.handle_realserver_disconnect_res_tag = l7vs::protocol_module_base::CLIENT_DISCONNECT;
	proto_test.handle_realserver_disconnect_thread_id = boost::thread::id();
	proto_test.handle_realserver_disconnect_rs_endpoint_list.clear();
	
	test_obj.test_down_all_call();
	
	// unit_test [1] down_thread_all_realserver_disconnect socket close call check
	std::cout << "[1] down_thread_all_realserver_disconnect socket close call check" << std::endl;
	ref_rs_it = ref_rs_list.begin();
	while(ref_rs_it != ref_rs_list.end()){
		BOOST_CHECK(ref_rs_it->second->close_call_check);
		ref_rs_it++;
	}

	// unit_test [2] down_thread_all_realserver_disconnect virtualservice connection_inactive call check
	std::cout << "[2] down_thread_all_realserver_disconnect virtualservice connection_inactive call check" << std::endl;
	BOOST_CHECK(!vs.connection_inactive_list.empty());
	ref_rs_it = ref_rs_list.begin();
	while(ref_rs_it != ref_rs_list.end()){
		std::list<boost::asio::ip::tcp::endpoint>::iterator end_list_it = vs.connection_inactive_list.begin();
		while(end_list_it == vs.connection_inactive_list.end()){
			if(ref_rs_it->first == *end_list_it)
				break;
			end_list_it++;
		}
		BOOST_CHECK(end_list_it != vs.connection_inactive_list.end());
		ref_rs_it++;
	}
	
	// unit_test [3] down_thread_all_realserver_disconnect handle_realserver_disconnect call check
	std::cout << "[3] down_thread_all_realserver_disconnect handle_realserver_disconnect call check" << std::endl;
	BOOST_CHECK(proto_test.handle_realserver_disconnect_thread_id == proc_id);
	BOOST_CHECK(!proto_test.handle_realserver_disconnect_rs_endpoint_list.empty());
	ref_rs_it = ref_rs_list.begin();
	while(ref_rs_it != ref_rs_list.end()){
		std::list<boost::asio::ip::tcp::endpoint>::iterator end_list_it = proto_test.handle_realserver_disconnect_rs_endpoint_list.begin();
		while(end_list_it == vs.connection_inactive_list.end()){
			if(ref_rs_it->first == *end_list_it)
				break;
			end_list_it++;
		}
		BOOST_CHECK(end_list_it != proto_test.handle_realserver_disconnect_rs_endpoint_list.end());
		ref_rs_it++;
	}
	
	// unit_test [4] down_thread_all_realserver_disconnect update up_thread_next_call_function check
	std::cout << "[4] down_thread_all_realserver_disconnect update up_thread_next_call_function check" << std::endl;
	test_obj.next_down_function_call();
	BOOST_CHECK(test_obj.down_thread_client_disconnect_call_check);

	// unit_test [5] down_thread_all_realserver_disconnect realserver not fond check
	std::cout << "[5] down_thread_all_realserver_disconnect realserver not fond check" << std::endl;
	proto_test.handle_realserver_disconnect_thread_id = boost::thread::id();
	proto_test.handle_realserver_disconnect_rs_endpoint_list.clear();
	test_obj.down_thread_client_disconnect_call_check = false;

	test_obj.test_down_all_call();

	BOOST_CHECK(proto_test.handle_realserver_disconnect_thread_id == proc_id);
	BOOST_CHECK(!proto_test.handle_realserver_disconnect_rs_endpoint_list.empty());
	BOOST_CHECK( *(proto_test.handle_realserver_disconnect_rs_endpoint_list.begin()) == boost::asio::ip::tcp::endpoint() );
	test_obj.next_down_function_call();
	BOOST_CHECK(test_obj.down_thread_client_disconnect_call_check);
	
	// unit_test [6] down_thread_all_realserver_disconnect handle_realserver_disconnect mutex lock check
	std::cout << "[6] down_thread_all_realserver_disconnect handle_realserver_disconnect mutex lock check" << std::endl;
	ref_rs_it = ref_rs_list.begin();
	while(ref_rs_it != ref_rs_list.end()){
		rs_list.push_back(*ref_rs_it);
		ref_rs_it++;
	}
	proto_test.handle_realserver_disconnect_thread_id = boost::thread::id();
	test_obj.mutex_lock();
	boost::thread test_thread(boost::bind(&realserver_disconnect_test_class::test_down_all_call,&test_obj));
	sleep(1);
	BOOST_CHECK(proto_test.handle_realserver_disconnect_thread_id == boost::thread::id());
	
	// unit_test [7] down_thread_all_realserver_disconnect handle_realserver_disconnect mutex unlock run check
	std::cout << "[7] down_thread_all_realserver_disconnect handle_realserver_disconnect mutex unlock run check" << std::endl;
	boost::thread::id test_id = test_thread.get_id();
	test_obj.set_down_thread_id(test_id);
	test_obj.mutex_unlock();
	sleep(1);
	BOOST_CHECK(proto_test.handle_realserver_disconnect_thread_id == test_id);
	test_thread.join();
	
	// unit_test [8] down_thread_all_realserver_disconnect not fond function error check
	std::cout << "[8] down_thread_all_realserver_disconnect not fond function error check" << std::endl;
	ref_rs_it = ref_rs_list.begin();
	while(ref_rs_it != ref_rs_list.end()){
		rs_list.push_back(*ref_rs_it);
		ref_rs_it++;
	}
	test_obj.down_thread_function_array_clear();
	test_obj.down_thread_exit_call_check = false;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_down_all_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(77,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	BOOST_CHECK(test_obj.down_thread_exit_call_check);
	
	// unit_test [9] down_thread_all_realserver_disconnect returnd illegal EVENT_TAG error check
	std::cout << "[9] down_thread_all_realserver_disconnect returnd illegal EVENT_TAG error check" << std::endl;
	ref_rs_it = ref_rs_list.begin();
	while(ref_rs_it != ref_rs_list.end()){
		rs_list.push_back(*ref_rs_it);
		ref_rs_it++;
	}
	test_obj.down_thread_module_event_map_clear();
	test_obj.down_thread_exit_call_check = false;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.test_down_all_call();
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(76,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	BOOST_CHECK(test_obj.down_thread_exit_call_check);
	
	BOOST_MESSAGE( "----- down_thread_all_realserver_disconnect test end -----" );
}



test_suite*	init_unit_test_suite( int argc, char* argv[] ){

	test_suite* ts = BOOST_TEST_SUITE( "l7vs::tcp_socket class test" );

	ts->add( BOOST_TEST_CASE( &constructer_test ) );
	ts->add( BOOST_TEST_CASE( &initialize_test ) );
	ts->add( BOOST_TEST_CASE( &get_client_socket_test) );
	ts->add( BOOST_TEST_CASE( &is_thread_wait_test) );
	ts->add( BOOST_TEST_CASE( &set_virtual_service_message_test) );
	ts->add( BOOST_TEST_CASE( &up_thread_run_test) );
	ts->add( BOOST_TEST_CASE( &down_thread_run_test) );
	ts->add( BOOST_TEST_CASE( &thread_state_update_test) );
	ts->add( BOOST_TEST_CASE( &up_thread_client_respond_test) );
	ts->add( BOOST_TEST_CASE( &up_thread_realserver_get_detination_event_test) );
	ts->add( BOOST_TEST_CASE( &up_thread_sorryserver_get_detination_event_test) );

	ts->add( BOOST_TEST_CASE( &up_thread_all_socket_close_test) );
	ts->add( BOOST_TEST_CASE( &down_thread_all_socket_close_test) );
	ts->add( BOOST_TEST_CASE( &up_thread_client_disconnect_test) );
	ts->add( BOOST_TEST_CASE( &down_thread_client_disconnect_test) );
	ts->add( BOOST_TEST_CASE( &up_thread_sorryserver_disconnect_test) );
	ts->add( BOOST_TEST_CASE( &down_thread_sorryserver_disconnect_test) );
	ts->add( BOOST_TEST_CASE( &up_thread_realserver_connect_event_test) );
	ts->add( BOOST_TEST_CASE( &up_thread_sorryserver_connect_event_test) );
	ts->add( BOOST_TEST_CASE( &down_thread_client_connection_chk_event_test) );
	ts->add( BOOST_TEST_CASE( &up_thread_realserver_connection_fail_event_test) );
	ts->add( BOOST_TEST_CASE( &up_thread_sorryserver_connection_fail_event_test) );

	ts->add( BOOST_TEST_CASE( &up_thread_client_receive_test) );
	ts->add( BOOST_TEST_CASE( &down_thread_realserver_receive_test) );
	ts->add( BOOST_TEST_CASE( &down_thread_sorryserver_receive_test) );
	ts->add( BOOST_TEST_CASE( &up_thread_realserver_send_test) );
	ts->add( BOOST_TEST_CASE( &up_thread_sorryserver_send_test) );
	ts->add( BOOST_TEST_CASE( &down_thread_client_send_test) );
	ts->add( BOOST_TEST_CASE( &up_thread_realserver_connect_test) );
	ts->add( BOOST_TEST_CASE( &up_thread_sorryserver_connect_test) );
	ts->add( BOOST_TEST_CASE( &up_thread_realserver_disconnect_test) );
	ts->add( BOOST_TEST_CASE( &down_thread_realserver_disconnect_test) );
	ts->add( BOOST_TEST_CASE( &up_thread_all_realserver_disconnect_test) );
	ts->add( BOOST_TEST_CASE( &down_thread_all_realserver_disconnect_test) );

	ts->add( BOOST_TEST_CASE( &up_thread_exit_test ) );
	ts->add( BOOST_TEST_CASE( &down_thread_exit_test ) );
	ts->add( BOOST_TEST_CASE( &up_thread_client_disconnect_event_test ) );
	ts->add( BOOST_TEST_CASE( &down_thread_client_disconnect_event_test ) );
	ts->add( BOOST_TEST_CASE( &up_thread_realserver_disconnect_event_test ) );
	ts->add( BOOST_TEST_CASE( &down_thread_realserver_disconnect_event_test ) );
	ts->add( BOOST_TEST_CASE( &up_thread_sorryserver_disconnect_event_test ) );
	ts->add( BOOST_TEST_CASE( &down_thread_sorryserver_disconnect_event_test ) );
	ts->add( BOOST_TEST_CASE( &up_thread_sorryserver_mod_disconnect_test ) );
	ts->add( BOOST_TEST_CASE( &down_thread_sorryserver_mod_disconnect_test ) );

	ts->add( BOOST_TEST_CASE( &up_thread_sorry_enable_event_test ) );
	ts->add( BOOST_TEST_CASE( &up_thread_sorry_disable_event_test ) );
	ts->add( BOOST_TEST_CASE( &down_thread_sorry_enable_event_test ) );
	ts->add( BOOST_TEST_CASE( &down_thread_sorry_disable_event_test ) );
	ts->add( BOOST_TEST_CASE( &up_thread_client_accept_event_test ) );
	ts->add( BOOST_TEST_CASE( &up_thread_client_respond_event_test ) );
	ts->add( BOOST_TEST_CASE( &down_thread_client_respond_event_test ) );
	
	framework::master_test_suite().add( ts );

	return NULL;
}
