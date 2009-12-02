#define	TEST_CASE

#include <boost/test/included/unit_test.hpp>
#include <boost/thread/condition.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/format.hpp>

#include "protocol_module_cinsert.h"

using namespace boost::unit_test_framework;
using namespace l7vs;

//--stub functions--
LOG_LEVEL_TAG	stb_getloglevel(){
//	std::cout << "getloglevel called." << std::endl;
	return LOG_LV_NONE;
}

void	stb_putLogFatal( const unsigned int, const std::string&, const char*, int ){
//	std::cout << "putLogFatal called." << std::endl;
}
void	stb_putLogError( const unsigned int, const std::string&, const char*, int ){
//	std::cout << "putLogError called." << std::endl;
}
void	stb_putLogWarn( const unsigned int, const std::string&, const char*, int ){
//	std::cout << "putLogWarn called." << std::endl;
}
void	stb_putLogInfo( const unsigned int, const std::string&, const char*, int ){
//	std::cout << "putLogInfo called." << std::endl;
}
void	stb_putLogDebug( const unsigned int, const std::string&, const char*, int ){
//	std::cout << "putLogDebug called." << std::endl;
}

//--handle_sorry_enable_test_class --
class	handle_sorry_enable_test_class : public protocol_module_cinsert
{
	public:
		handle_sorry_enable_test_class() : protocol_module_cinsert()
		{
			getloglevel	= &stb_getloglevel;
			putLogFatal	= &stb_putLogFatal;
			putLogError	= &stb_putLogError;
			putLogWarn	= &stb_putLogWarn;
			putLogInfo	= &stb_putLogInfo;
			putLogDebug	= &stb_putLogDebug;
		};
		~handle_sorry_enable_test_class(){};
		
		
		boost::condition	sync_condition;
		
		boost::thread::id thread_id_01;
		boost::mutex		sync_mutex_01;
		protocol_module_cinsert::EVENT_TAG	status_01;
		boost::xtime	start_time_01;
		boost::xtime	end_time_01;
		
		boost::thread::id thread_id_02;
		boost::mutex		sync_mutex_02;
		protocol_module_cinsert::EVENT_TAG	status_02;
		boost::xtime	start_time_02;
		boost::xtime	end_time_02;
		
		boost::thread::id thread_id_03;
		boost::mutex		sync_mutex_03;
		protocol_module_cinsert::EVENT_TAG	status_03;
		boost::xtime	start_time_03;
		boost::xtime	end_time_03;
		
		boost::thread::id thread_id_04;
		boost::mutex		sync_mutex_04;
		protocol_module_cinsert::EVENT_TAG	status_04;
		boost::xtime	start_time_04;
		boost::xtime	end_time_04;
		
		boost::thread::id thread_id_05;
		boost::mutex		sync_mutex_05;
		protocol_module_cinsert::EVENT_TAG	status_05;
		boost::xtime	start_time_05;
		boost::xtime	end_time_05;
		
		boost::thread::id thread_id_06;
		boost::mutex		sync_mutex_06;
		protocol_module_cinsert::EVENT_TAG	status_06;
		boost::xtime	start_time_06;
		boost::xtime	end_time_06;
		
		boost::thread::id thread_id_07;
		boost::mutex		sync_mutex_07;
		protocol_module_cinsert::EVENT_TAG	status_07;
		boost::xtime	start_time_07;
		boost::xtime	end_time_07;
		
		boost::thread::id thread_id_08;
		boost::mutex		sync_mutex_08;
		protocol_module_cinsert::EVENT_TAG	status_08;
		boost::xtime	start_time_08;
		boost::xtime	end_time_08;
		
		boost::thread::id thread_id_09;
		boost::mutex		sync_mutex_09;
		protocol_module_cinsert::EVENT_TAG	status_09;
		boost::xtime	start_time_09;
		boost::xtime	end_time_09;
		
		boost::thread::id thread_id_10;
		boost::mutex		sync_mutex_10;
		protocol_module_cinsert::EVENT_TAG	status_10;
		boost::xtime	start_time_10;
		boost::xtime	end_time_10;
		
		boost::thread::id thread_id_11;
		boost::mutex		sync_mutex_11;
		protocol_module_cinsert::EVENT_TAG	status_11;
		boost::xtime	start_time_11;
		boost::xtime	end_time_11;
		
		boost::thread::id thread_id_12;
		boost::mutex		sync_mutex_12;
		protocol_module_cinsert::EVENT_TAG	status_12;
		boost::xtime	start_time_12;
		boost::xtime	end_time_12;
		
		boost::thread::id thread_id_13;
		boost::mutex		sync_mutex_13;
		protocol_module_cinsert::EVENT_TAG	status_13;
		boost::xtime	start_time_13;
		boost::xtime	end_time_13;
		
		void test_thread_01_run(){
			thread_id_01 = boost::this_thread::get_id();
			boost::mutex::scoped_lock	lk( sync_mutex_01 );
			sync_condition.wait( lk );
			boost::xtime_get(&start_time_01, boost::TIME_UTC);
			status_01 = handle_sorry_enable(thread_id_01);
			boost::xtime_get(&end_time_01, boost::TIME_UTC);
		}
		
		void test_thread_02_run(){
			thread_id_02 = boost::this_thread::get_id();
			boost::mutex::scoped_lock	lk( sync_mutex_02 );
			sync_condition.wait( lk );
			boost::xtime_get(&start_time_02, boost::TIME_UTC);
			status_02 = handle_sorry_enable(thread_id_02);
			boost::xtime_get(&end_time_02, boost::TIME_UTC);
		}
		
		void test_thread_03_run(){
			thread_id_03 = boost::this_thread::get_id();
			boost::mutex::scoped_lock	lk( sync_mutex_03 );
			sync_condition.wait( lk );
			boost::xtime_get(&start_time_03, boost::TIME_UTC);
			status_03 = handle_sorry_enable(thread_id_03);
			boost::xtime_get(&end_time_03, boost::TIME_UTC);
		}
		
		void test_thread_04_run(){
			thread_id_04 = boost::this_thread::get_id();
			boost::mutex::scoped_lock	lk( sync_mutex_04 );
			sync_condition.wait( lk );
			boost::xtime_get(&start_time_04, boost::TIME_UTC);
			status_04 = handle_sorry_enable(thread_id_04);
			boost::xtime_get(&end_time_04, boost::TIME_UTC);
		}
		
		void test_thread_05_run(){
			thread_id_05 = boost::this_thread::get_id();
			boost::mutex::scoped_lock	lk( sync_mutex_05 );
			sync_condition.wait( lk );
			boost::xtime_get(&start_time_05, boost::TIME_UTC);
			status_05 = handle_sorry_enable(thread_id_05);
			boost::xtime_get(&end_time_05, boost::TIME_UTC);
		}
		
		void test_thread_06_run(){
			thread_id_06 = boost::this_thread::get_id();
			boost::mutex::scoped_lock	lk( sync_mutex_06 );
			sync_condition.wait( lk );
			boost::xtime_get(&start_time_06, boost::TIME_UTC);
			status_06 = handle_sorry_enable(thread_id_06);
			boost::xtime_get(&end_time_06, boost::TIME_UTC);
		}
		
		void test_thread_07_run(){
			thread_id_07 = boost::this_thread::get_id();
			boost::mutex::scoped_lock	lk( sync_mutex_07 );
			sync_condition.wait( lk );
			boost::xtime_get(&start_time_07, boost::TIME_UTC);
			status_07 = handle_sorry_enable(thread_id_07);
			boost::xtime_get(&end_time_07, boost::TIME_UTC);
		}
		
		void test_thread_08_run(){
			thread_id_07 = boost::this_thread::get_id();
			boost::mutex::scoped_lock	lk( sync_mutex_08 );
			sync_condition.wait( lk );
			boost::xtime_get(&start_time_08, boost::TIME_UTC);
			status_08 = handle_sorry_enable(thread_id_08);
			boost::xtime_get(&end_time_08, boost::TIME_UTC);
		}
		
		void test_thread_09_run(){
			thread_id_09 = boost::this_thread::get_id();
			boost::mutex::scoped_lock	lk( sync_mutex_09 );
			sync_condition.wait( lk );
			boost::xtime_get(&start_time_09, boost::TIME_UTC);
			status_09 = handle_sorry_enable(thread_id_09);
			boost::xtime_get(&end_time_09, boost::TIME_UTC);
		}
		
		void test_thread_10_run(){
			thread_id_10 = boost::this_thread::get_id();
			boost::mutex::scoped_lock	lk( sync_mutex_10 );
			sync_condition.wait( lk );
			boost::xtime_get(&start_time_10, boost::TIME_UTC);
			status_10 = handle_sorry_enable(thread_id_10);
			boost::xtime_get(&end_time_10, boost::TIME_UTC);
		}
		
		void test_thread_11_run(){
			thread_id_11 = boost::this_thread::get_id();
			boost::mutex::scoped_lock	lk( sync_mutex_11 );
			sync_condition.wait( lk );
			boost::xtime_get(&start_time_11, boost::TIME_UTC);
			status_11 = handle_sorry_enable(thread_id_11);
			boost::xtime_get(&end_time_11, boost::TIME_UTC);
		}
		
		void test_thread_12_run(){
			thread_id_12 = boost::this_thread::get_id();
			boost::mutex::scoped_lock	lk( sync_mutex_12 );
			sync_condition.wait( lk );
			boost::xtime_get(&start_time_12, boost::TIME_UTC);
			status_12 = handle_sorry_enable(thread_id_12);
			boost::xtime_get(&end_time_12, boost::TIME_UTC);
		}
		
		void test_thread_13_run(){
			thread_id_13 = boost::this_thread::get_id();
			boost::mutex::scoped_lock	lk( sync_mutex_13 );
			sync_condition.wait( lk );
			boost::xtime_get(&start_time_13, boost::TIME_UTC);
			status_13 = handle_sorry_enable(thread_id_13);
			boost::xtime_get(&end_time_13, boost::TIME_UTC);
		}
		
		t_session_thread_data_map& get_session_thread_data_map(){
			return session_thread_data_map;
		};
		
};

void	handle_sorry_enable_test(){
	BOOST_MESSAGE( "----- handle_sorry_enable test start -----" );
	
	handle_sorry_enable_test_class	test_obj;
	protocol_module_cinsert::t_session_thread_data_map& session_thread_data_map = test_obj.get_session_thread_data_map();
	
	
	// thread_data->thread_division == THREAD_DIVISION_UP_STREAM
		// thread_data->accept_end_flag == ACCEPT_END_FLAG_OFF
			// thread 1 
			// check1 thread_data->sorry_flag = SORRY_FLAG_ON;
			// check2 return = ACCEPT;
		// thread_data->accept_end_flag != ACCEPT_END_FLAG_OFF
			// thread_data->sorry_flag == SORRY_FLAG_ON
				// thread_data->recive_data_map.find( thread_data->client_endpoint_tcp ) fond
					//recive_data in status == SEND_OK fond
						// thread 2
						// check1 return SORRYSERVER_SELECT
					//recive_data in status == SEND_OK not fond
						// thread 3
						// check1 return CLIENT_RECV
				// thread_data->recive_data_map.find( thread_data->client_endpoint_tcp ) not fond
					// thread 4
					// check1 return STOP
			// thread_data->sorry_flag != SORRY_FLAG_ON
				//thread_data->recive_data_map.find( thread_data->client_endpoint_tcp ) fond
					//recive_data in status  == SEND_OK fond
						// thread 5
						// check1 thread_data->sorryserver_switch_flag = SORRYSERVER_SWITCH_FLAG_ON;
						// check2 thread_data->sorry_flag = SORRY_FLAG_ON;
						// check3 return = REALSERVER_DISCONNECT;
						// check4 thread_data->end_flag = END_FLAG_ON <- END_FLAG_OFF
					//recive_data in status == SEND_OK not fond
						// thread 6
						// check1 thread_data->sorryserver_switch_flag = SORRYSERVER_SWITCH_FLAG_ON;
						// check2 thread_data->sorry_flag = SORRY_FLAG_ON;
						// check3 return = REALSERVER_DISCONNECT;
						// check4 thread_data->end_flag = END_FLAG_OFF not update
				//thread_data->recive_data_map.find( thread_data->client_endpoint_tcp ) not fond
					// thread 7
					// check1 return STOP 
	// thread_data->thread_division != THREAD_DIVISION_UP_STREAM
		// thread_data->sorry_flag == SORRY_FLAG_ON
			//recive_data in status == SEND_OK fond
				// thread 8
				// check1 return CLIENT_CONNECTION_CHECK
			//recive_data in status == SEND_OK not fond
				// thread 9
				// check1 return SORRYSERVER_RECV
		// thread_data->sorry_flag != SORRY_FLAG_ON
			// recive_data in ALL send_rest_size == 0 & status == SEND_NG fond
				// thread 10
				// check 1 thread_data->sorry_flag = SORRY_FLAG_ON
				// check 2 thread_data->end_flag = END_FLAG_ON
				// check 3 status = REALSERVER_DISCONNECT
			// recive_data in ALL status != SEND_NG & send_rest_size > 0 fond
				// thread 11
				// check 1 thread_data->sorry_flag = SORRY_FLAG_ON
				// check 2 thread_data->end_flag = END_FLAG_ON
				// check 3 status = REALSERVER_DISCONNECT
			// recive_data in ALL status != SEND_NG & ALL send_rest_size != 0  status = SEND_OK fond
				// thread 12
				// check 1 thread_data->sorry_flag = SORRY_FLAG_ON
				// check 2 thread_data->end_flag = END_FLAG_OFF not update
				// check 3 status = CLIENT_CONNECTION_CHECK
			// recive_data in status == SEND_NG or status == SEND_OK or send_rest_size == 0 not fond
				// thread 13
				// check 1 thread_data->sorry_flag = SORRY_FLAG_OFF  not update
				// check 2 thread_data->end_flag = END_FLAG_OFF not update
				// check 3 status = SORRYSERVER_RECV
	
	
	
	// session_thread_data_map empty
		// check1 return STOP 
	// unit_test [1] handle_sorry_enable session_thread_data_map empty return STOP
	std::cout << "[1] handle_sorry_enable session_thread_data_map empty return STOP" << std::endl;
	session_thread_data_map.clear();
	protocol_module_cinsert::EVENT_TAG status = test_obj.handle_sorry_enable(boost::this_thread::get_id());
	BOOST_CHECK_EQUAL(status,protocol_module_cinsert::STOP);
	
	boost::thread test_thread_01(boost::bind(&handle_sorry_enable_test_class::test_thread_01_run,&test_obj));
	boost::thread test_thread_02(boost::bind(&handle_sorry_enable_test_class::test_thread_02_run,&test_obj));
	boost::thread test_thread_03(boost::bind(&handle_sorry_enable_test_class::test_thread_03_run,&test_obj));
	boost::thread test_thread_04(boost::bind(&handle_sorry_enable_test_class::test_thread_04_run,&test_obj));
	boost::thread test_thread_05(boost::bind(&handle_sorry_enable_test_class::test_thread_05_run,&test_obj));
	boost::thread test_thread_06(boost::bind(&handle_sorry_enable_test_class::test_thread_06_run,&test_obj));
	boost::thread test_thread_07(boost::bind(&handle_sorry_enable_test_class::test_thread_07_run,&test_obj));
	boost::thread test_thread_08(boost::bind(&handle_sorry_enable_test_class::test_thread_08_run,&test_obj));
	boost::thread test_thread_09(boost::bind(&handle_sorry_enable_test_class::test_thread_09_run,&test_obj));
	boost::thread test_thread_10(boost::bind(&handle_sorry_enable_test_class::test_thread_10_run,&test_obj));
	boost::thread test_thread_11(boost::bind(&handle_sorry_enable_test_class::test_thread_11_run,&test_obj));
	boost::thread test_thread_12(boost::bind(&handle_sorry_enable_test_class::test_thread_12_run,&test_obj));
	boost::thread test_thread_13(boost::bind(&handle_sorry_enable_test_class::test_thread_13_run,&test_obj));

	// sync_condition wait
	sleep(2);
	
	// set session_thread_data_map
	boost::asio::ip::tcp::endpoint cl_end(boost::asio::ip::address::from_string("10.10.10.10"), 1110);
	boost::asio::ip::tcp::endpoint dummy_1(boost::asio::ip::address::from_string("10.10.10.11"), 1111);
	boost::asio::ip::tcp::endpoint dummy_2(boost::asio::ip::address::from_string("10.10.10.12"), 1112);
	boost::asio::ip::tcp::endpoint dummy_3(boost::asio::ip::address::from_string("10.10.10.13"), 1113);
	std::pair< boost::asio::ip::tcp::endpoint , protocol_module_cinsert::recive_data > recive_data_pair;
	protocol_module_cinsert::send_status snd_status;
	
	// thread 1 data
	// thread_data->thread_division = THREAD_DIVISION_UP_STREAM
	// thread_data->accept_end_flag = ACCEPT_END_FLAG_OFF
	// thread_data->sorry_flag = SORRY_FLAG_OFF
	std::pair< boost::thread::id, protocol_module_cinsert::t_session_thread_data_cinsert > thread_1_data;
	boost::shared_ptr< protocol_module_cinsert::session_thread_data_cinsert > thread_1_read_data(new protocol_module_cinsert::session_thread_data_cinsert);
	thread_1_data.first = test_obj.thread_id_01;
	thread_1_data.second = thread_1_read_data;
	thread_1_read_data->thread_division = protocol_module_cinsert::THREAD_DIVISION_UP_STREAM;
	thread_1_read_data->accept_end_flag = protocol_module_cinsert::ACCEPT_END_FLAG_OFF;
	thread_1_read_data->sorry_flag = protocol_module_cinsert::SORRY_FLAG_OFF;
	session_thread_data_map.insert(thread_1_data);
		
	// thread 2 data
	// thread_data->thread_division = THREAD_DIVISION_UP_STREAM
	// thread_data->accept_end_flag = ACCEPT_END_FLAG_OFF
	// thread_data->sorry_flag = SORRY_FLAG_ON
	// thread_data->recive_data_map.find( thread_data->client_endpoint_tcp ) fond
	// recive_data in status SEND_OK fond
	std::pair< boost::thread::id, protocol_module_cinsert::t_session_thread_data_cinsert > thread_2_data;
	boost::shared_ptr< protocol_module_cinsert::session_thread_data_cinsert > thread_2_read_data(new protocol_module_cinsert::session_thread_data_cinsert);
	thread_2_data.first = test_obj.thread_id_02;
	thread_2_data.second = thread_2_read_data;
	
	thread_2_read_data->thread_division = protocol_module_cinsert::THREAD_DIVISION_UP_STREAM;
	thread_2_read_data->accept_end_flag = protocol_module_cinsert::ACCEPT_END_FLAG_ON;
	thread_2_read_data->sorry_flag = protocol_module_cinsert::SORRY_FLAG_ON;
	thread_2_read_data->client_endpoint_tcp = cl_end;
	
	recive_data_pair.first = cl_end;
	snd_status.status = protocol_module_cinsert::SEND_NG;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	snd_status.status = protocol_module_cinsert::SEND_OK;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	snd_status.status = protocol_module_cinsert::SEND_NG;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	thread_2_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_1;
	recive_data_pair.second.send_status_list.clear();
	thread_2_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_2;
	recive_data_pair.second.send_status_list.clear();
	thread_2_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_3;
	recive_data_pair.second.send_status_list.clear();
	thread_2_read_data->recive_data_map.insert(recive_data_pair);
	
	session_thread_data_map.insert(thread_2_data);
	
	// thread 3 data
	// thread_data->thread_division = THREAD_DIVISION_UP_STREAM
	// thread_data->accept_end_flag = ACCEPT_END_FLAG_ON
	// thread_data->sorry_flag = SORRY_FLAG_ON
	// thread_data->recive_data_map.find( thread_data->client_endpoint_tcp ) fond
	// recive_data in status SEND_OK not fond
	std::pair< boost::thread::id, protocol_module_cinsert::t_session_thread_data_cinsert > thread_3_data;
	boost::shared_ptr< protocol_module_cinsert::session_thread_data_cinsert > thread_3_read_data(new protocol_module_cinsert::session_thread_data_cinsert);
	thread_3_data.first = test_obj.thread_id_03;
	thread_3_data.second = thread_3_read_data;
	
	thread_3_read_data->thread_division = protocol_module_cinsert::THREAD_DIVISION_UP_STREAM;
	thread_3_read_data->accept_end_flag = protocol_module_cinsert::ACCEPT_END_FLAG_ON;
	thread_3_read_data->sorry_flag = protocol_module_cinsert::SORRY_FLAG_ON;
	thread_3_read_data->client_endpoint_tcp = cl_end;
	
	recive_data_pair.first = cl_end;
	snd_status.status = protocol_module_cinsert::SEND_NG;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	snd_status.status = protocol_module_cinsert::SEND_NG;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	snd_status.status = protocol_module_cinsert::SEND_NG;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	thread_3_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_1;
	recive_data_pair.second.send_status_list.clear();
	thread_3_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_2;
	recive_data_pair.second.send_status_list.clear();
	thread_3_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_3;
	recive_data_pair.second.send_status_list.clear();
	thread_3_read_data->recive_data_map.insert(recive_data_pair);
	
	session_thread_data_map.insert(thread_3_data);
	
	// thread 4 data
	std::pair< boost::thread::id, protocol_module_cinsert::t_session_thread_data_cinsert > thread_4_data;
	boost::shared_ptr< protocol_module_cinsert::session_thread_data_cinsert > thread_4_read_data(new protocol_module_cinsert::session_thread_data_cinsert);
	thread_4_data.first = test_obj.thread_id_04;
	thread_4_data.second = thread_4_read_data;
	
	thread_4_read_data->thread_division = protocol_module_cinsert::THREAD_DIVISION_UP_STREAM;
	thread_4_read_data->accept_end_flag = protocol_module_cinsert::ACCEPT_END_FLAG_ON;
	thread_4_read_data->sorry_flag = protocol_module_cinsert::SORRY_FLAG_ON;
	thread_4_read_data->client_endpoint_tcp = cl_end;
	
	recive_data_pair.first = dummy_1;
	snd_status.status = protocol_module_cinsert::SEND_NG;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	snd_status.status = protocol_module_cinsert::SEND_NG;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	snd_status.status = protocol_module_cinsert::SEND_NG;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	thread_4_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_1;
	recive_data_pair.second.send_status_list.clear();
	thread_4_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_2;
	recive_data_pair.second.send_status_list.clear();
	thread_4_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_3;
	recive_data_pair.second.send_status_list.clear();
	thread_4_read_data->recive_data_map.insert(recive_data_pair);
	
	session_thread_data_map.insert(thread_4_data);
	
	
	// thread 5 data
	// thread_data->thread_division = THREAD_DIVISION_UP_STREAM
	// thread_data->accept_end_flag = ACCEPT_END_FLAG_ON
	// thread_data->sorry_flag = SORRY_FLAG_OFF
	// thread_data->recive_data_map.find( thread_data->client_endpoint_tcp ) fond
	// thread_data->end_flag = END_FLAG_OFF
	// recive_data in status SEND_CONTINUE fond
	std::pair< boost::thread::id, protocol_module_cinsert::t_session_thread_data_cinsert > thread_5_data;
	boost::shared_ptr< protocol_module_cinsert::session_thread_data_cinsert > thread_5_read_data(new protocol_module_cinsert::session_thread_data_cinsert);
	thread_5_data.first = test_obj.thread_id_05;
	thread_5_data.second = thread_5_read_data;
	
	thread_5_read_data->thread_division = protocol_module_cinsert::THREAD_DIVISION_UP_STREAM;
	thread_5_read_data->accept_end_flag = protocol_module_cinsert::ACCEPT_END_FLAG_ON;
	thread_5_read_data->sorry_flag = protocol_module_cinsert::SORRY_FLAG_OFF;
	thread_5_read_data->client_endpoint_tcp = cl_end;
	thread_5_read_data->end_flag = protocol_module_cinsert::END_FLAG_OFF;
	thread_5_read_data->sorryserver_switch_flag = protocol_module_cinsert::SORRYSERVER_SWITCH_FLAG_OFF;
	
	recive_data_pair.first = cl_end;
	snd_status.status = protocol_module_cinsert::SEND_NG;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	snd_status.status = protocol_module_cinsert::SEND_NG;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	snd_status.status = protocol_module_cinsert::SEND_CONTINUE;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	thread_5_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_1;
	recive_data_pair.second.send_status_list.clear();
	thread_5_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_2;
	recive_data_pair.second.send_status_list.clear();
	thread_5_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_3;
	recive_data_pair.second.send_status_list.clear();
	thread_5_read_data->recive_data_map.insert(recive_data_pair);
	
	session_thread_data_map.insert(thread_5_data);
	
	// thread 6 data
	// thread_data->thread_division = THREAD_DIVISION_UP_STREAM
	// thread_data->accept_end_flag = ACCEPT_END_FLAG_ON
	// thread_data->sorry_flag = SORRY_FLAG_OFF
	// thread_data->recive_data_map.find( thread_data->client_endpoint_tcp ) fond
	// thread_data->end_flag = END_FLAG_OFF
	// recive_data in status SEND_CONTINUE not fond
	std::pair< boost::thread::id, protocol_module_cinsert::t_session_thread_data_cinsert > thread_6_data;
	boost::shared_ptr< protocol_module_cinsert::session_thread_data_cinsert > thread_6_read_data(new protocol_module_cinsert::session_thread_data_cinsert);
	thread_6_data.first = test_obj.thread_id_06;
	thread_6_data.second = thread_6_read_data;
	
	thread_6_read_data->thread_division = protocol_module_cinsert::THREAD_DIVISION_UP_STREAM;
	thread_6_read_data->accept_end_flag = protocol_module_cinsert::ACCEPT_END_FLAG_ON;
	thread_6_read_data->sorry_flag = protocol_module_cinsert::SORRY_FLAG_OFF;
	thread_6_read_data->client_endpoint_tcp = cl_end;
	thread_6_read_data->end_flag = protocol_module_cinsert::END_FLAG_OFF;
	thread_6_read_data->sorryserver_switch_flag = protocol_module_cinsert::SORRYSERVER_SWITCH_FLAG_OFF;
	
	recive_data_pair.first = cl_end;
	snd_status.status = protocol_module_cinsert::SEND_NG;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	snd_status.status = protocol_module_cinsert::SEND_OK;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	snd_status.status = protocol_module_cinsert::SEND_NG;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	thread_6_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_1;
	recive_data_pair.second.send_status_list.clear();
	thread_6_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_2;
	recive_data_pair.second.send_status_list.clear();
	thread_6_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_3;
	recive_data_pair.second.send_status_list.clear();
	thread_6_read_data->recive_data_map.insert(recive_data_pair);
	
	session_thread_data_map.insert(thread_6_data);
	
	
	// thread 7 data
	// thread_data->thread_division = THREAD_DIVISION_UP_STREAM
	// thread_data->accept_end_flag = ACCEPT_END_FLAG_ON
	// thread_data->sorry_flag = SORRY_FLAG_OFF
	// thread_data->recive_data_map.find( thread_data->client_endpoint_tcp ) not fond
	std::pair< boost::thread::id, protocol_module_cinsert::t_session_thread_data_cinsert > thread_7_data;
	boost::shared_ptr< protocol_module_cinsert::session_thread_data_cinsert > thread_7_read_data(new protocol_module_cinsert::session_thread_data_cinsert);
	thread_7_data.first = test_obj.thread_id_07;
	thread_7_data.second = thread_7_read_data;
	
	thread_7_read_data->thread_division = protocol_module_cinsert::THREAD_DIVISION_UP_STREAM;
	thread_7_read_data->accept_end_flag = protocol_module_cinsert::ACCEPT_END_FLAG_ON;
	thread_7_read_data->sorry_flag = protocol_module_cinsert::SORRY_FLAG_OFF;
	thread_7_read_data->client_endpoint_tcp = cl_end;
	
	recive_data_pair.first = dummy_1;
	snd_status.status = protocol_module_cinsert::SEND_NG;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	snd_status.status = protocol_module_cinsert::SEND_NG;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	snd_status.status = protocol_module_cinsert::SEND_NG;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	thread_7_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_1;
	recive_data_pair.second.send_status_list.clear();
	thread_7_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_2;
	recive_data_pair.second.send_status_list.clear();
	thread_7_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_3;
	recive_data_pair.second.send_status_list.clear();
	thread_7_read_data->recive_data_map.insert(recive_data_pair);
	
	session_thread_data_map.insert(thread_7_data);
	
	// thread 8 data
	// thread_data->thread_division = THREAD_DIVISION_DOWN_STREAM
	// thread_data->sorry_flag = SORRY_FLAG_ON
	// recive_data in status = SEND_OK fond
	std::pair< boost::thread::id, protocol_module_cinsert::t_session_thread_data_cinsert > thread_8_data;
	boost::shared_ptr< protocol_module_cinsert::session_thread_data_cinsert > thread_8_read_data(new protocol_module_cinsert::session_thread_data_cinsert);
	thread_8_data.first = test_obj.thread_id_08;
	thread_8_data.second = thread_8_read_data;
	
	thread_8_read_data->thread_division = protocol_module_cinsert::THREAD_DIVISION_DOWN_STREAM;
	thread_8_read_data->sorry_flag = protocol_module_cinsert::SORRY_FLAG_ON;
	thread_8_read_data->client_endpoint_tcp = cl_end;
	
	recive_data_pair.first = dummy_1;
	snd_status.status = protocol_module_cinsert::SEND_NG;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	snd_status.status = protocol_module_cinsert::SEND_OK;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	snd_status.status = protocol_module_cinsert::SEND_NG;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	thread_8_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_1;
	recive_data_pair.second.send_status_list.clear();
	thread_8_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_2;
	recive_data_pair.second.send_status_list.clear();
	thread_8_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_3;
	recive_data_pair.second.send_status_list.clear();
	thread_8_read_data->recive_data_map.insert(recive_data_pair);
	
	session_thread_data_map.insert(thread_8_data);
	
	// thread 9 data
	// thread_data->thread_division = THREAD_DIVISION_DOWN_STREAM
	// thread_data->sorry_flag = SORRY_FLAG_ON
	// recive_data in status = SEND_OK not fond
	std::pair< boost::thread::id, protocol_module_cinsert::t_session_thread_data_cinsert > thread_9_data;
	boost::shared_ptr< protocol_module_cinsert::session_thread_data_cinsert > thread_9_read_data(new protocol_module_cinsert::session_thread_data_cinsert);
	thread_9_data.first = test_obj.thread_id_09;
	thread_9_data.second = thread_9_read_data;
		
	thread_9_read_data->thread_division = protocol_module_cinsert::THREAD_DIVISION_DOWN_STREAM;
	thread_9_read_data->sorry_flag = protocol_module_cinsert::SORRY_FLAG_ON;
	thread_9_read_data->client_endpoint_tcp = cl_end;
	
	recive_data_pair.first = dummy_1;
	snd_status.status = protocol_module_cinsert::SEND_NG;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	snd_status.status = protocol_module_cinsert::SEND_NG;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	snd_status.status = protocol_module_cinsert::SEND_NG;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	thread_9_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_1;
	recive_data_pair.second.send_status_list.clear();
	thread_9_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_2;
	recive_data_pair.second.send_status_list.clear();
	thread_9_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_3;
	recive_data_pair.second.send_status_list.clear();
	thread_9_read_data->recive_data_map.insert(recive_data_pair);
	
	session_thread_data_map.insert(thread_9_data);
	
	
	// thread 10 data
	// thread_data->thread_division = THREAD_DIVISION_DOWN_STREAM
	// thread_data->sorry_flag = SORRY_FLAG_OFF
	// recive_data in ALL send_rest_size = 0 & status = SEND_NG fond
	// thread_data->end_flag = END_FLAG_OFF
	std::pair< boost::thread::id, protocol_module_cinsert::t_session_thread_data_cinsert > thread_10_data;
	boost::shared_ptr< protocol_module_cinsert::session_thread_data_cinsert > thread_10_read_data(new protocol_module_cinsert::session_thread_data_cinsert);
	thread_10_data.first = test_obj.thread_id_10;
	thread_10_data.second = thread_10_read_data;
	
	thread_10_read_data->thread_division = protocol_module_cinsert::THREAD_DIVISION_DOWN_STREAM;
	thread_10_read_data->sorry_flag = protocol_module_cinsert::SORRY_FLAG_OFF;
	thread_10_read_data->end_flag = protocol_module_cinsert::END_FLAG_OFF;
			
	recive_data_pair.first = dummy_1;
	snd_status.status = protocol_module_cinsert::SEND_OK;
	snd_status.send_rest_size = 0;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	snd_status.status = protocol_module_cinsert::SEND_OK;
	snd_status.send_rest_size = 0;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	snd_status.status = protocol_module_cinsert::SEND_NG;
	snd_status.send_rest_size = 0;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	thread_10_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_1;
	recive_data_pair.second.send_status_list.clear();
	thread_10_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_2;
	recive_data_pair.second.send_status_list.clear();
	thread_10_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_3;
	recive_data_pair.second.send_status_list.clear();
	thread_10_read_data->recive_data_map.insert(recive_data_pair);
	
	session_thread_data_map.insert(thread_10_data);
	
	// thread 11 data
	// thread_data->thread_division = THREAD_DIVISION_DOWN_STREAM
	// thread_data->sorry_flag = SORRY_FLAG_OFF
	// recive_data in ALL status = SEND_NG & send_rest_size > 0 fond
	// thread_data->sorry_flag = SORRY_FLAG_ON
	// thread_data->end_flag = END_FLAG_OFF
	std::pair< boost::thread::id, protocol_module_cinsert::t_session_thread_data_cinsert > thread_11_data;
	boost::shared_ptr< protocol_module_cinsert::session_thread_data_cinsert > thread_11_read_data(new protocol_module_cinsert::session_thread_data_cinsert);
	thread_11_data.first = test_obj.thread_id_11;
	thread_11_data.second = thread_11_read_data;
	
	thread_11_read_data->thread_division = protocol_module_cinsert::THREAD_DIVISION_DOWN_STREAM;
	thread_11_read_data->sorry_flag = protocol_module_cinsert::SORRY_FLAG_OFF;
	thread_11_read_data->end_flag = protocol_module_cinsert::END_FLAG_OFF;
	
	recive_data_pair.first = dummy_1;
	snd_status.status = protocol_module_cinsert::SEND_OK;
	snd_status.send_rest_size = 0;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	snd_status.status = protocol_module_cinsert::SEND_OK;
	snd_status.send_rest_size = 1;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	snd_status.status = protocol_module_cinsert::SEND_OK;
	snd_status.send_rest_size = 0;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	thread_11_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_1;
	recive_data_pair.second.send_status_list.clear();
	thread_11_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_2;
	recive_data_pair.second.send_status_list.clear();
	thread_11_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_3;
	recive_data_pair.second.send_status_list.clear();
	thread_11_read_data->recive_data_map.insert(recive_data_pair);
	
	session_thread_data_map.insert(thread_11_data);
	
	// thread 12 data
	// thread_data->thread_division = THREAD_DIVISION_DOWN_STREAM
	// thread_data->sorry_flag = SORRY_FLAG_OFF
	// recive_data in ALL status != SEND_NG & ALL send_rest_size = 0  status = SEND_OK fond
	// thread_data->end_flag = END_FLAG_OFF
	std::pair< boost::thread::id, protocol_module_cinsert::t_session_thread_data_cinsert > thread_12_data;
	boost::shared_ptr< protocol_module_cinsert::session_thread_data_cinsert > thread_12_read_data(new protocol_module_cinsert::session_thread_data_cinsert);
	thread_12_data.first = test_obj.thread_id_12;
	thread_12_data.second = thread_12_read_data;
	
	thread_12_read_data->thread_division = protocol_module_cinsert::THREAD_DIVISION_DOWN_STREAM;
	thread_12_read_data->sorry_flag = protocol_module_cinsert::SORRY_FLAG_OFF;
	thread_12_read_data->end_flag = protocol_module_cinsert::END_FLAG_OFF;
	
	recive_data_pair.first = dummy_1;
	snd_status.status = protocol_module_cinsert::SEND_CONTINUE;
	snd_status.send_rest_size = 0;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	snd_status.status = protocol_module_cinsert::SEND_OK;
	snd_status.send_rest_size = 0;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	snd_status.status = protocol_module_cinsert::SEND_CONTINUE;
	snd_status.send_rest_size = 0;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	thread_12_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_1;
	recive_data_pair.second.send_status_list.clear();
	thread_12_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_2;
	recive_data_pair.second.send_status_list.clear();
	thread_12_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_3;
	recive_data_pair.second.send_status_list.clear();
	thread_12_read_data->recive_data_map.insert(recive_data_pair);
	
	session_thread_data_map.insert(thread_12_data);
	
	// thread 13 data
	// thread_data->thread_division = THREAD_DIVISION_DOWN_STREAM
	// thread_data->sorry_flag = SORRY_FLAG_OFF
	// recive_data in status = SEND_NG or status = SEND_OK or send_rest_size > 0 not fond
	// thread_data->end_flag = END_FLAG_OFF
	std::pair< boost::thread::id, protocol_module_cinsert::t_session_thread_data_cinsert > thread_13_data;
	boost::shared_ptr< protocol_module_cinsert::session_thread_data_cinsert > thread_13_read_data(new protocol_module_cinsert::session_thread_data_cinsert);
	thread_13_data.first = test_obj.thread_id_13;
	thread_13_data.second = thread_13_read_data;
	
	thread_13_read_data->thread_division = protocol_module_cinsert::THREAD_DIVISION_DOWN_STREAM;
	thread_13_read_data->sorry_flag = protocol_module_cinsert::SORRY_FLAG_OFF;
	thread_13_read_data->end_flag = protocol_module_cinsert::END_FLAG_OFF;
	
	recive_data_pair.first = dummy_1;
	snd_status.status = protocol_module_cinsert::SEND_CONTINUE;
	snd_status.send_rest_size = 0;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	snd_status.status = protocol_module_cinsert::SEND_CONTINUE;
	snd_status.send_rest_size = 0;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	snd_status.status = protocol_module_cinsert::SEND_CONTINUE;
	snd_status.send_rest_size = 0;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	thread_13_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_1;
	recive_data_pair.second.send_status_list.clear();
	thread_13_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_2;
	recive_data_pair.second.send_status_list.clear();
	thread_13_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_3;
	recive_data_pair.second.send_status_list.clear();
	thread_13_read_data->recive_data_map.insert(recive_data_pair);
	
	session_thread_data_map.insert(thread_13_data);
	
	
	test_obj.sync_condition.notify_all();
	std::cout << "sync_condition.notify_all" << std::endl;
	
	test_thread_01.join();
//	std::cout << "test_thread_01 stop" << std::endl;
	test_thread_02.join();
//	std::cout << "test_thread_02 stop" << std::endl;
	test_thread_03.join();
//	std::cout << "test_thread_03 stop" << std::endl;
	test_thread_04.join();
//	std::cout << "test_thread_04 stop" << std::endl;
	test_thread_05.join();
//	std::cout << "test_thread_05 stop" << std::endl;
	test_thread_06.join();
//	std::cout << "test_thread_06 stop" << std::endl;
	test_thread_07.join();
//	std::cout << "test_thread_07 stop" << std::endl;
	test_thread_08.join();
//	std::cout << "test_thread_08 stop" << std::endl;
	test_thread_09.join();
//	std::cout << "test_thread_09 stop" << std::endl;
	test_thread_10.join();
//	std::cout << "test_thread_10 stop" << std::endl;
	test_thread_11.join();
//	std::cout << "test_thread_11 stop" << std::endl;
	test_thread_12.join();
//	std::cout << "test_thread_12 stop" << std::endl;
	test_thread_13.join();
//	std::cout << "test_thread_13 stop" << std::endl;
	
	std::cout << "test_thread_01 start time [" << test_obj.start_time_01.nsec << "]" << std::endl;
	std::cout << "test_thread_02 start time [" << test_obj.start_time_02.nsec << "]" << std::endl;
	std::cout << "test_thread_03 start time [" << test_obj.start_time_03.nsec << "]" << std::endl;
	std::cout << "test_thread_04 start time [" << test_obj.start_time_04.nsec << "]" << std::endl;
	std::cout << "test_thread_05 start time [" << test_obj.start_time_05.nsec << "]" << std::endl;
	std::cout << "test_thread_06 start time [" << test_obj.start_time_06.nsec << "]" << std::endl;
	std::cout << "test_thread_07 start time [" << test_obj.start_time_07.nsec << "]" << std::endl;
	std::cout << "test_thread_08 start time [" << test_obj.start_time_08.nsec << "]" << std::endl;
	std::cout << "test_thread_09 start time [" << test_obj.start_time_09.nsec << "]" << std::endl;
	std::cout << "test_thread_10 start time [" << test_obj.start_time_10.nsec << "]" << std::endl;
	std::cout << "test_thread_11 start time [" << test_obj.start_time_11.nsec << "]" << std::endl;
	std::cout << "test_thread_12 start time [" << test_obj.start_time_12.nsec << "]" << std::endl;
	std::cout << "test_thread_13 start time [" << test_obj.start_time_13.nsec << "]" << std::endl;
	
	std::cout << "test_thread_01 end time [" << test_obj.end_time_01.nsec << "]" << std::endl;
	std::cout << "test_thread_02 end time [" << test_obj.end_time_02.nsec << "]" << std::endl;
	std::cout << "test_thread_03 end time [" << test_obj.end_time_03.nsec << "]" << std::endl;
	std::cout << "test_thread_04 end time [" << test_obj.end_time_04.nsec << "]" << std::endl;
	std::cout << "test_thread_05 end time [" << test_obj.end_time_05.nsec << "]" << std::endl;
	std::cout << "test_thread_06 end time [" << test_obj.end_time_06.nsec << "]" << std::endl;
	std::cout << "test_thread_07 end time [" << test_obj.end_time_07.nsec << "]" << std::endl;
	std::cout << "test_thread_08 end time [" << test_obj.end_time_08.nsec << "]" << std::endl;
	std::cout << "test_thread_09 end time [" << test_obj.end_time_09.nsec << "]" << std::endl;
	std::cout << "test_thread_10 end time [" << test_obj.end_time_10.nsec << "]" << std::endl;
	std::cout << "test_thread_11 end time [" << test_obj.end_time_11.nsec << "]" << std::endl;
	std::cout << "test_thread_12 end time [" << test_obj.end_time_12.nsec << "]" << std::endl;
	std::cout << "test_thread_13 end time [" << test_obj.end_time_13.nsec << "]" << std::endl;
	
	// thread 1
	// unit_test [2] handle_sorry_enable THREAD_DIVISION_UP_STREAM & ACCEPT_END_FLAG_OFF case check
	std::cout << "[2] handle_sorry_enable THREAD_DIVISION_UP_STREAM & ACCEPT_END_FLAG_OFF case check" << std::endl;
	// check1 thread_data->sorry_flag = SORRY_FLAG_ON;
	BOOST_CHECK( thread_1_read_data->sorry_flag == protocol_module_cinsert::SORRY_FLAG_ON );
	// check2 return = ACCEPT;
	BOOST_CHECK_EQUAL(test_obj.status_01,protocol_module_cinsert::ACCEPT);
	
	// thread 2 
	// unit_test [3] handle_sorry_enable THREAD_DIVISION_UP_STREAM & ACCEPT_END_FLAG_ON & SORRY_FLAG_ON & status SEND_OK fond case check
	std::cout << "[3] handle_sorry_enable THREAD_DIVISION_UP_STREAM & ACCEPT_END_FLAG_ON & SORRY_FLAG_ON & status SEND_OK fond case check" << std::endl;
	// check1 return SORRYSERVER_SELECT
	BOOST_CHECK_EQUAL(test_obj.status_02,protocol_module_cinsert::SORRYSERVER_SELECT);
	
	// thread 3 
	// unit_test [4] handle_sorry_enable THREAD_DIVISION_UP_STREAM & ACCEPT_END_FLAG_ON & SORRY_FLAG_ON status SEND_OK not fond case check
	std::cout << "[4] handle_sorry_enable THREAD_DIVISION_UP_STREAM & ACCEPT_END_FLAG_ON & SORRY_FLAG_ON status SEND_OK not fond case check" << std::endl;
	// check1 return CLIENT_RECV
	BOOST_CHECK_EQUAL(test_obj.status_03,protocol_module_cinsert::CLIENT_RECV);
	
	// thread 4 
	// unit_test [5] handle_sorry_enable THREAD_DIVISION_UP_STREAM & ACCEPT_END_FLAG_ON & SORRY_FLAG_ON recive_data_map not fond client_endpoint_tcp case check
	std::cout << "[5] handle_sorry_enable THREAD_DIVISION_UP_STREAM & ACCEPT_END_FLAG_ON & SORRY_FLAG_ON recive_data_map not fond client_endpoint_tcp case check" << std::endl;
	// check1 return STOP
	BOOST_CHECK_EQUAL(test_obj.status_04,protocol_module_cinsert::STOP);
	
	// thread 5
	// unit_test [6] handle_sorry_enable THREAD_DIVISION_UP_STREAM & ACCEPT_END_FLAG_ON & SORRY_FLAG_ON & status SEND_CONTINUE fond case check
	std::cout << "[6] handle_sorry_enable THREAD_DIVISION_UP_STREAM & ACCEPT_END_FLAG_ON & SORRY_FLAG_ON & status SEND_CONTINUE fond case check" << std::endl;
	// check1 thread_data->sorryserver_switch_flag = SORRYSERVER_SWITCH_FLAG_ON;
	BOOST_CHECK( thread_5_read_data->sorryserver_switch_flag == protocol_module_cinsert::SORRYSERVER_SWITCH_FLAG_ON );
	// check2 thread_data->sorry_flag = SORRY_FLAG_ON;
	BOOST_CHECK( thread_5_read_data->sorry_flag == protocol_module_cinsert::SORRY_FLAG_ON );
	// check4 thread_data->end_flag = END_FLAG_ON
	BOOST_CHECK( thread_5_read_data->end_flag == protocol_module_cinsert::END_FLAG_ON );
	// check3 return = REALSERVER_DISCONNECT;	
	BOOST_CHECK_EQUAL(test_obj.status_05,protocol_module_cinsert::REALSERVER_DISCONNECT);
	
	// thread 6
	// unit_test [7] handle_sorry_enable THREAD_DIVISION_UP_STREAM & ACCEPT_END_FLAG_ON & SORRY_FLAG_ON & status SEND_CONTINUE fond case check
	std::cout << "[7] handle_sorry_enable THREAD_DIVISION_UP_STREAM & ACCEPT_END_FLAG_ON & SORRY_FLAG_ON & status SEND_CONTINUE fond case check" << std::endl;
	// check1 thread_data->sorryserver_switch_flag = SORRYSERVER_SWITCH_FLAG_ON;
	BOOST_CHECK( thread_6_read_data->sorryserver_switch_flag == protocol_module_cinsert::SORRYSERVER_SWITCH_FLAG_ON );
	// check2 thread_data->sorry_flag = SORRY_FLAG_ON;
	BOOST_CHECK( thread_6_read_data->sorry_flag == protocol_module_cinsert::SORRY_FLAG_ON );
	// check4 thread_data->end_flag = END_FLAG_OFF
	BOOST_CHECK( thread_6_read_data->end_flag == protocol_module_cinsert::END_FLAG_OFF );
	// check3 return = REALSERVER_DISCONNECT;	
	BOOST_CHECK_EQUAL(test_obj.status_06,protocol_module_cinsert::REALSERVER_DISCONNECT);
	
	// thread 7
	// unit_test [8] handle_sorry_enable THREAD_DIVISION_UP_STREAM & ACCEPT_END_FLAG_ON & SORRY_FLAG_ON recive_data_map not fond client_endpoint_tcp case check
	std::cout << "[8] handle_sorry_enable THREAD_DIVISION_UP_STREAM & ACCEPT_END_FLAG_ON & SORRY_FLAG_ON recive_data_map not fond client_endpoint_tcp case check" << std::endl;
	// check1 return STOP
	BOOST_CHECK_EQUAL(test_obj.status_07,protocol_module_cinsert::STOP);
	
	// thread 8
	// unit_test [9] handle_sorry_enable THREAD_DIVISION_DOWN_STREAM & SORRY_FLAG_ON & status SEND_OK fond case check
	std::cout << "[9] handle_sorry_enable THREAD_DIVISION_DOWN_STREAM & SORRY_FLAG_ON & status SEND_OK fond case check" << std::endl;
	// check1 return CLIENT_CONNECTION_CHECK
	BOOST_CHECK_EQUAL(test_obj.status_08,protocol_module_cinsert::CLIENT_CONNECTION_CHECK);
	
	// thread 9 
	// unit_test [10] handle_sorry_enable THREAD_DIVISION_DOWN_STREAM & SORRY_FLAG_ON & status SEND_OK not fond case check
	std::cout << "[10] handle_sorry_enable THREAD_DIVISION_DOWN_STREAM & SORRY_FLAG_ON & status SEND_OK not fond case check" << std::endl;
	// check1 return SORRYSERVER_RECV
	BOOST_CHECK_EQUAL(test_obj.status_09,protocol_module_cinsert::SORRYSERVER_RECV);
	
	// thread 10 
	// unit_test [11] handle_sorry_enable THREAD_DIVISION_DOWN_STREAM & SORRY_FLAG_OFF & ALL send_rest_size == 0 & status == SEND_NG fond case check
	std::cout << "[11] handle_sorry_enable THREAD_DIVISION_DOWN_STREAM & SORRY_FLAG_OFF & ALL send_rest_size == 0 & status == SEND_NG fond case check" << std::endl;
	// check 1 thread_data->sorry_flag = SORRY_FLAG_ON
	BOOST_CHECK( thread_10_read_data->sorry_flag == protocol_module_cinsert::SORRY_FLAG_ON );
	// check 2 thread_data->end_flag = END_FLAG_ON
	BOOST_CHECK( thread_10_read_data->end_flag == protocol_module_cinsert::END_FLAG_ON );
	// check 3 status = REALSERVER_DISCONNECT
	BOOST_CHECK_EQUAL(test_obj.status_10,protocol_module_cinsert::REALSERVER_DISCONNECT);
	
	// thread 11 
	// unit_test [12] handle_sorry_enable THREAD_DIVISION_DOWN_STREAM & SORRY_FLAG_OFF & ALL status != SEND_NG & send_rest_size > 0 fond case check
	std::cout << "[12] handle_sorry_enable THREAD_DIVISION_DOWN_STREAM & SORRY_FLAG_OFF & ALL status != SEND_NG & send_rest_size > 0 fond case check" << std::endl;
	// check 1 thread_data->sorry_flag = SORRY_FLAG_ON
	BOOST_CHECK( thread_11_read_data->sorry_flag == protocol_module_cinsert::SORRY_FLAG_ON );
	// check 2 thread_data->end_flag = END_FLAG_ON
	BOOST_CHECK( thread_11_read_data->end_flag == protocol_module_cinsert::END_FLAG_ON );
	// check 3 status = REALSERVER_DISCONNECT
	BOOST_CHECK_EQUAL(test_obj.status_11,protocol_module_cinsert::REALSERVER_DISCONNECT);
	
	// thread 12
	// unit_test [13] handle_sorry_enable THREAD_DIVISION_DOWN_STREAM & SORRY_FLAG_OFF & ALL status != SEND_NG & send_rest_size > 0 fond case check
	std::cout << "[13] handle_sorry_enable THREAD_DIVISION_DOWN_STREAM & SORRY_FLAG_OFF & ALL status != SEND_NG & ALL send_rest_size != 0  status = SEND_OK fond case check" << std::endl;
	// check 1 thread_data->sorry_flag = SORRY_FLAG_ON
	BOOST_CHECK( thread_13_read_data->sorry_flag == protocol_module_cinsert::SORRY_FLAG_ON );
	// check 2 thread_data->end_flag = END_FLAG_OFF not update
	BOOST_CHECK( thread_13_read_data->end_flag == protocol_module_cinsert::END_FLAG_OFF );
	// check 3 status = CLIENT_CONNECTION_CHECK
	BOOST_CHECK_EQUAL(test_obj.status_12,protocol_module_cinsert::CLIENT_CONNECTION_CHECK);
	
	// thread 13
	// unit_test [14] handle_sorry_enable THREAD_DIVISION_DOWN_STREAM & SORRY_FLAG_OFF & ALL status != SEND_NG & send_rest_size > 0 fond case check
	std::cout << "[14] handle_sorry_enable THREAD_DIVISION_DOWN_STREAM & SORRY_FLAG_OFF & status == SEND_NG or status == SEND_OK or send_rest_size == 0 not fond case check" << std::endl;
	// check 1 thread_data->sorry_flag = SORRY_FLAG_OFF  not update
	BOOST_CHECK( thread_13_read_data->end_flag == protocol_module_cinsert::SORRY_FLAG_OFF );
	// check 2 thread_data->end_flag = END_FLAG_OFF not update
	BOOST_CHECK( thread_13_read_data->end_flag == protocol_module_cinsert::END_FLAG_OFF );
	// check 3 status = SORRYSERVER_RECV
	BOOST_CHECK_EQUAL(test_obj.status_13,protocol_module_cinsert::SORRYSERVER_RECV);
	
	BOOST_MESSAGE( "----- handle_sorry_enable test end -----" );
}


//--handle_sorry_disable_test_class --
class	handle_sorry_disable_test_class : public protocol_module_cinsert
{
	public:
		handle_sorry_disable_test_class() : protocol_module_cinsert()
		{
			getloglevel	= &stb_getloglevel;
			putLogFatal	= &stb_putLogFatal;
			putLogError	= &stb_putLogError;
			putLogWarn	= &stb_putLogWarn;
			putLogInfo	= &stb_putLogInfo;
			putLogDebug	= &stb_putLogDebug;
		};
		~handle_sorry_disable_test_class(){};
		
		
		boost::condition	sync_condition;
		
		boost::thread::id thread_id_01;
		boost::mutex		sync_mutex_01;
		protocol_module_cinsert::EVENT_TAG	status_01;
		boost::xtime	start_time_01;
		boost::xtime	end_time_01;
		
		boost::thread::id thread_id_02;
		boost::mutex		sync_mutex_02;
		protocol_module_cinsert::EVENT_TAG	status_02;
		boost::xtime	start_time_02;
		boost::xtime	end_time_02;
		
		boost::thread::id thread_id_03;
		boost::mutex		sync_mutex_03;
		protocol_module_cinsert::EVENT_TAG	status_03;
		boost::xtime	start_time_03;
		boost::xtime	end_time_03;
		
		boost::thread::id thread_id_04;
		boost::mutex		sync_mutex_04;
		protocol_module_cinsert::EVENT_TAG	status_04;
		boost::xtime	start_time_04;
		boost::xtime	end_time_04;
		
		boost::thread::id thread_id_05;
		boost::mutex		sync_mutex_05;
		protocol_module_cinsert::EVENT_TAG	status_05;
		boost::xtime	start_time_05;
		boost::xtime	end_time_05;
		
		boost::thread::id thread_id_06;
		boost::mutex		sync_mutex_06;
		protocol_module_cinsert::EVENT_TAG	status_06;
		boost::xtime	start_time_06;
		boost::xtime	end_time_06;
		
		boost::thread::id thread_id_07;
		boost::mutex		sync_mutex_07;
		protocol_module_cinsert::EVENT_TAG	status_07;
		boost::xtime	start_time_07;
		boost::xtime	end_time_07;
		
		boost::thread::id thread_id_08;
		boost::mutex		sync_mutex_08;
		protocol_module_cinsert::EVENT_TAG	status_08;
		boost::xtime	start_time_08;
		boost::xtime	end_time_08;
		
		boost::thread::id thread_id_09;
		boost::mutex		sync_mutex_09;
		protocol_module_cinsert::EVENT_TAG	status_09;
		boost::xtime	start_time_09;
		boost::xtime	end_time_09;
		
		boost::thread::id thread_id_10;
		boost::mutex		sync_mutex_10;
		protocol_module_cinsert::EVENT_TAG	status_10;
		boost::xtime	start_time_10;
		boost::xtime	end_time_10;
		
		boost::thread::id thread_id_11;
		boost::mutex		sync_mutex_11;
		protocol_module_cinsert::EVENT_TAG	status_11;
		boost::xtime	start_time_11;
		boost::xtime	end_time_11;
		
		boost::thread::id thread_id_12;
		boost::mutex		sync_mutex_12;
		protocol_module_cinsert::EVENT_TAG	status_12;
		boost::xtime	start_time_12;
		boost::xtime	end_time_12;
		
		boost::thread::id thread_id_13;
		boost::mutex		sync_mutex_13;
		protocol_module_cinsert::EVENT_TAG	status_13;
		boost::xtime	start_time_13;
		boost::xtime	end_time_13;
		
		void test_thread_01_run(){
			thread_id_01 = boost::this_thread::get_id();
			boost::mutex::scoped_lock	lk( sync_mutex_01 );
			sync_condition.wait( lk );
			boost::xtime_get(&start_time_01, boost::TIME_UTC);
			status_01 = handle_sorry_disable(thread_id_01);
			boost::xtime_get(&end_time_01, boost::TIME_UTC);
}
		
		void test_thread_02_run(){
			thread_id_02 = boost::this_thread::get_id();
			boost::mutex::scoped_lock	lk( sync_mutex_02 );
			sync_condition.wait( lk );
			boost::xtime_get(&start_time_02, boost::TIME_UTC);
			status_02 = handle_sorry_disable(thread_id_02);
			boost::xtime_get(&end_time_02, boost::TIME_UTC);
}
		
		void test_thread_03_run(){
			thread_id_03 = boost::this_thread::get_id();
			boost::mutex::scoped_lock	lk( sync_mutex_03 );
			sync_condition.wait( lk );
			boost::xtime_get(&start_time_03, boost::TIME_UTC);
			status_03 = handle_sorry_disable(thread_id_03);
			boost::xtime_get(&end_time_03, boost::TIME_UTC);
}
		
		void test_thread_04_run(){
			thread_id_04 = boost::this_thread::get_id();
			boost::mutex::scoped_lock	lk( sync_mutex_04 );
			sync_condition.wait( lk );
			boost::xtime_get(&start_time_04, boost::TIME_UTC);
			status_04 = handle_sorry_disable(thread_id_04);
			boost::xtime_get(&end_time_04, boost::TIME_UTC);
}
		
		void test_thread_05_run(){
			thread_id_05 = boost::this_thread::get_id();
			boost::mutex::scoped_lock	lk( sync_mutex_05 );
			sync_condition.wait( lk );
			boost::xtime_get(&start_time_05, boost::TIME_UTC);
			status_05 = handle_sorry_disable(thread_id_05);
			boost::xtime_get(&end_time_05, boost::TIME_UTC);
}
		
		void test_thread_06_run(){
			thread_id_06 = boost::this_thread::get_id();
			boost::mutex::scoped_lock	lk( sync_mutex_06 );
			sync_condition.wait( lk );
			boost::xtime_get(&start_time_06, boost::TIME_UTC);
			status_06 = handle_sorry_disable(thread_id_06);
			boost::xtime_get(&end_time_06, boost::TIME_UTC);
}
		
		void test_thread_07_run(){
			thread_id_07 = boost::this_thread::get_id();
			boost::mutex::scoped_lock	lk( sync_mutex_07 );
			sync_condition.wait( lk );
			boost::xtime_get(&start_time_07, boost::TIME_UTC);
			status_07 = handle_sorry_disable(thread_id_07);
			boost::xtime_get(&end_time_07, boost::TIME_UTC);
}
		
		void test_thread_08_run(){
			thread_id_07 = boost::this_thread::get_id();
			boost::mutex::scoped_lock	lk( sync_mutex_08 );
			sync_condition.wait( lk );
			boost::xtime_get(&start_time_08, boost::TIME_UTC);
			status_08 = handle_sorry_disable(thread_id_08);
			boost::xtime_get(&end_time_08, boost::TIME_UTC);
}
		
		void test_thread_09_run(){
			thread_id_09 = boost::this_thread::get_id();
			boost::mutex::scoped_lock	lk( sync_mutex_09 );
			sync_condition.wait( lk );
			boost::xtime_get(&start_time_09, boost::TIME_UTC);
			status_09 = handle_sorry_disable(thread_id_09);
			boost::xtime_get(&end_time_09, boost::TIME_UTC);
}
		
		void test_thread_10_run(){
			thread_id_10 = boost::this_thread::get_id();
			boost::mutex::scoped_lock	lk( sync_mutex_10 );
			sync_condition.wait( lk );
			boost::xtime_get(&start_time_10, boost::TIME_UTC);
			status_10 = handle_sorry_disable(thread_id_10);
			boost::xtime_get(&end_time_10, boost::TIME_UTC);
}
		
		void test_thread_11_run(){
			thread_id_11 = boost::this_thread::get_id();
			boost::mutex::scoped_lock	lk( sync_mutex_11 );
			sync_condition.wait( lk );
			boost::xtime_get(&start_time_11, boost::TIME_UTC);
			status_11 = handle_sorry_disable(thread_id_11);
			boost::xtime_get(&end_time_11, boost::TIME_UTC);
}
		
		void test_thread_12_run(){
			thread_id_12 = boost::this_thread::get_id();
			boost::mutex::scoped_lock	lk( sync_mutex_12 );
			sync_condition.wait( lk );
			boost::xtime_get(&start_time_12, boost::TIME_UTC);
			status_12 = handle_sorry_disable(thread_id_12);
			boost::xtime_get(&end_time_12, boost::TIME_UTC);
}
		
		void test_thread_13_run(){
			thread_id_13 = boost::this_thread::get_id();
			boost::mutex::scoped_lock	lk( sync_mutex_13 );
			sync_condition.wait( lk );
			boost::xtime_get(&start_time_13, boost::TIME_UTC);
			status_13 = handle_sorry_disable(thread_id_13);
			boost::xtime_get(&end_time_13, boost::TIME_UTC);
}
		
		t_session_thread_data_map& get_session_thread_data_map(){
			return session_thread_data_map;
};
		
};

void	handle_sorry_disable_test(){
	BOOST_MESSAGE( "----- handle_sorry_disable test start -----" );
	
	handle_sorry_disable_test_class	test_obj;
	protocol_module_cinsert::t_session_thread_data_map& session_thread_data_map = test_obj.get_session_thread_data_map();
	
	// session_thread_data_map empty
		// check1 return STOP 
	// unit_test [1] handle_sorry_disable session_thread_data_map empty return STOP
	std::cout << "[1] handle_sorry_disable session_thread_data_map empty return STOP" << std::endl;
	session_thread_data_map.clear();
	protocol_module_cinsert::EVENT_TAG status = test_obj.handle_sorry_disable(boost::this_thread::get_id());
	BOOST_CHECK_EQUAL(status,protocol_module_cinsert::STOP);
	
	// thread_data->thread_division == THREAD_DIVISION_UP_STREAM
		// thread_data->accept_end_flag == ACCEPT_END_FLAG_OFF
			// thread 1 
			// check1 thread_data->sorry_flag = SORRY_FLAG_OFF;
			// check2 return = ACCEPT;
	
		// thread_data->accept_end_flag == ACCEPT_END_FLAG_ON
			// thread_data->sorry_flag == SORRY_FLAG_OFF
				// thread_data->recive_data_map.find( thread_data->client_endpoint_tcp ) fond
					//recive_data in status == SEND_OK fond
						// thread 2
						// check1 return REALSERVER_SELECT
					//recive_data in status == SEND_OK not fond
						// thread 3
						// check1 return CLIENT_RECV
				// thread_data->recive_data_map.find( thread_data->client_endpoint_tcp ) not fond
					// thread 4
					// check1 return STOP
			// thread_data->sorry_flag != SORRY_FLAG_OFF
				//thread_data->recive_data_map.find( thread_data->client_endpoint_tcp ) fond
					//recive_data in status  == SEND_CONTINUE fond
						// thread 5
						// check1 thread_data->sorryserver_switch_flag = REALSERVER_SWITCH_FLAG_ON;
						// check2 thread_data->sorry_flag = SORRY_FLAG_OFF;
						// check3 return = SORRYSERVER_DISCONNECT;
						// check4 thread_data->end_flag = END_FLAG_ON
					//recive_data in status == SEND_CONTINUE not fond
						// thread 6
						// check1 thread_data->sorryserver_switch_flag = REALSERVER_SWITCH_FLAG_ON;
						// check2 thread_data->sorry_flag = SORRY_FLAG_OFF;
						// check3 return = SORRYSERVER_DISCONNECT;
						// check4 thread_data->end_flag = END_FLAG_OFF not update
				//thread_data->recive_data_map.find( thread_data->client_endpoint_tcp ) not fond
					// thread 7
					// check1 return STOP 
	// thread_data->thread_division == THREAD_DIVISION_DOWN_STREAM
		// thread_data->sorry_flag == SORRY_FLAG_OFF
			//recive_data in status == SEND_OK fond
				// thread 8
				// check1 return CLIENT_CONNECTION_CHECK
			//recive_data in status == SEND_OK not fond
				// thread 9
				// check1 return REALSERVER_RECV
		// thread_data->sorry_flag != SORRY_FLAG_OFF
			// recive_data in ALL send_rest_size == 0 & status == SEND_NG fond
				// thread 10
				// check 1 thread_data->sorry_flag = SORRY_FLAG_OFF
				// check 2 thread_data->end_flag = END_FLAG_ON
				// check 3 status = SORRYSERVER_DISCONNECT
			// recive_data in ALL status != SEND_NG & send_rest_size > 0 fond
				// thread 11
				// check 1 thread_data->sorry_flag = SORRY_FLAG_OFF
				// check 2 thread_data->end_flag = END_FLAG_ON
				// check 3 status = SORRYSERVER_DISCONNECT
			// recive_data in ALL status != SEND_NG & ALL send_rest_size != 0  status = SEND_OK fond
				// thread 12
				// check 1 thread_data->sorry_flag = SORRY_FLAG_OFF
				// check 2 thread_data->end_flag = END_FLAG_OFF not update
				// check 3 status = CLIENT_CONNECTION_CHECK
			// recive_data in status == SEND_NG or status == SEND_OK or send_rest_size == 0 not fond
				// thread 13
				// check 1 thread_data->sorry_flag = SORRY_FLAG_OFF not update
				// check 2 thread_data->end_flag = END_FLAG_OFF not update
				// check 3 status = SORRYSERVER_RECV
	
	boost::thread test_thread_01(boost::bind(&handle_sorry_disable_test_class::test_thread_01_run,&test_obj));
	boost::thread test_thread_02(boost::bind(&handle_sorry_disable_test_class::test_thread_02_run,&test_obj));
	boost::thread test_thread_03(boost::bind(&handle_sorry_disable_test_class::test_thread_03_run,&test_obj));
	boost::thread test_thread_04(boost::bind(&handle_sorry_disable_test_class::test_thread_04_run,&test_obj));
	boost::thread test_thread_05(boost::bind(&handle_sorry_disable_test_class::test_thread_05_run,&test_obj));
	boost::thread test_thread_06(boost::bind(&handle_sorry_disable_test_class::test_thread_06_run,&test_obj));
	boost::thread test_thread_07(boost::bind(&handle_sorry_disable_test_class::test_thread_07_run,&test_obj));
	boost::thread test_thread_08(boost::bind(&handle_sorry_disable_test_class::test_thread_08_run,&test_obj));
	boost::thread test_thread_09(boost::bind(&handle_sorry_disable_test_class::test_thread_09_run,&test_obj));
	boost::thread test_thread_10(boost::bind(&handle_sorry_disable_test_class::test_thread_10_run,&test_obj));
	boost::thread test_thread_11(boost::bind(&handle_sorry_disable_test_class::test_thread_11_run,&test_obj));
	boost::thread test_thread_12(boost::bind(&handle_sorry_disable_test_class::test_thread_12_run,&test_obj));
	boost::thread test_thread_13(boost::bind(&handle_sorry_disable_test_class::test_thread_13_run,&test_obj));

	// sync_condition wait
	sleep(2);
	
	// set session_thread_data_map
	boost::asio::ip::tcp::endpoint cl_end(boost::asio::ip::address::from_string("10.10.10.10"), 1110);
	boost::asio::ip::tcp::endpoint dummy_1(boost::asio::ip::address::from_string("10.10.10.11"), 1111);
	boost::asio::ip::tcp::endpoint dummy_2(boost::asio::ip::address::from_string("10.10.10.12"), 1112);
	boost::asio::ip::tcp::endpoint dummy_3(boost::asio::ip::address::from_string("10.10.10.13"), 1113);
	std::pair< boost::asio::ip::tcp::endpoint , protocol_module_cinsert::recive_data > recive_data_pair;
	protocol_module_cinsert::send_status snd_status;
	
	// thread 1 data
	// thread_data->thread_division = THREAD_DIVISION_UP_STREAM
	// thread_data->accept_end_flag = ACCEPT_END_FLAG_OFF
	// thread_data->sorry_flag = SORRY_FLAG_ON
	
	std::pair< boost::thread::id, protocol_module_cinsert::t_session_thread_data_cinsert > thread_1_data;
	boost::shared_ptr< protocol_module_cinsert::session_thread_data_cinsert > thread_1_read_data(new protocol_module_cinsert::session_thread_data_cinsert);
	thread_1_data.first = test_obj.thread_id_01;
	thread_1_data.second = thread_1_read_data;
	thread_1_read_data->thread_division = protocol_module_cinsert::THREAD_DIVISION_UP_STREAM;
	thread_1_read_data->accept_end_flag = protocol_module_cinsert::ACCEPT_END_FLAG_OFF;
	thread_1_read_data->sorry_flag = protocol_module_cinsert::SORRY_FLAG_ON;
	session_thread_data_map.insert(thread_1_data);
	
	// thread 2 data
	// thread_data->thread_division = THREAD_DIVISION_UP_STREAM
	// thread_data->accept_end_flag = ACCEPT_END_FLAG_ON
	// thread_data->sorry_flag = SORRY_FLAG_OFF
	// thread_data->recive_data_map.find( thread_data->client_endpoint_tcp ) fond
	// recive_data in status = SEND_OK fond
	
	std::pair< boost::thread::id, protocol_module_cinsert::t_session_thread_data_cinsert > thread_2_data;
	boost::shared_ptr< protocol_module_cinsert::session_thread_data_cinsert > thread_2_read_data(new protocol_module_cinsert::session_thread_data_cinsert);
	thread_2_data.first = test_obj.thread_id_02;
	thread_2_data.second = thread_2_read_data;
	
	thread_2_read_data->thread_division = protocol_module_cinsert::THREAD_DIVISION_UP_STREAM;
	thread_2_read_data->accept_end_flag = protocol_module_cinsert::ACCEPT_END_FLAG_ON;
	thread_2_read_data->sorry_flag = protocol_module_cinsert::SORRY_FLAG_OFF;
	thread_2_read_data->client_endpoint_tcp = cl_end;
	
	recive_data_pair.first = cl_end;
	snd_status.status = protocol_module_cinsert::SEND_NG;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	snd_status.status = protocol_module_cinsert::SEND_OK;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	snd_status.status = protocol_module_cinsert::SEND_NG;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	thread_2_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_1;
	recive_data_pair.second.send_status_list.clear();
	thread_2_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_2;
	recive_data_pair.second.send_status_list.clear();
	thread_2_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_3;
	recive_data_pair.second.send_status_list.clear();
	thread_2_read_data->recive_data_map.insert(recive_data_pair);
	
	session_thread_data_map.insert(thread_2_data);
	
	// thread 3 data
	// thread_data->thread_division = THREAD_DIVISION_UP_STREAM
	// thread_data->accept_end_flag = ACCEPT_END_FLAG_ON
	// thread_data->sorry_flag = SORRY_FLAG_OFF
	// thread_data->recive_data_map.find( thread_data->client_endpoint_tcp ) fond
	// recive_data in status = SEND_OK not fond
	
	std::pair< boost::thread::id, protocol_module_cinsert::t_session_thread_data_cinsert > thread_3_data;
	boost::shared_ptr< protocol_module_cinsert::session_thread_data_cinsert > thread_3_read_data(new protocol_module_cinsert::session_thread_data_cinsert);
	thread_3_data.first = test_obj.thread_id_03;
	thread_3_data.second = thread_3_read_data;
	
	thread_3_read_data->thread_division = protocol_module_cinsert::THREAD_DIVISION_UP_STREAM;
	thread_3_read_data->accept_end_flag = protocol_module_cinsert::ACCEPT_END_FLAG_ON;
	thread_3_read_data->sorry_flag = protocol_module_cinsert::SORRY_FLAG_OFF;
	thread_3_read_data->client_endpoint_tcp = cl_end;
	
	recive_data_pair.first = cl_end;
	snd_status.status = protocol_module_cinsert::SEND_NG;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	snd_status.status = protocol_module_cinsert::SEND_NG;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	snd_status.status = protocol_module_cinsert::SEND_NG;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	thread_3_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_1;
	recive_data_pair.second.send_status_list.clear();
	thread_3_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_2;
	recive_data_pair.second.send_status_list.clear();
	thread_3_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_3;
	recive_data_pair.second.send_status_list.clear();
	thread_3_read_data->recive_data_map.insert(recive_data_pair);
	
	session_thread_data_map.insert(thread_3_data);
	
	// thread 4 data
	// thread_data->thread_division = THREAD_DIVISION_UP_STREAM
	// thread_data->accept_end_flag = ACCEPT_END_FLAG_ON
	// thread_data->sorry_flag = SORRY_FLAG_OFF
	// thread_data->recive_data_map.find( thread_data->client_endpoint_tcp ) fond
	// recive_data in status = SEND_OK not fond
	std::pair< boost::thread::id, protocol_module_cinsert::t_session_thread_data_cinsert > thread_4_data;
	boost::shared_ptr< protocol_module_cinsert::session_thread_data_cinsert > thread_4_read_data(new protocol_module_cinsert::session_thread_data_cinsert);
	thread_4_data.first = test_obj.thread_id_04;
	thread_4_data.second = thread_4_read_data;
	
	thread_4_read_data->thread_division = protocol_module_cinsert::THREAD_DIVISION_UP_STREAM;
	thread_4_read_data->accept_end_flag = protocol_module_cinsert::ACCEPT_END_FLAG_ON;
	thread_4_read_data->sorry_flag = protocol_module_cinsert::SORRY_FLAG_OFF;
	thread_4_read_data->client_endpoint_tcp = cl_end;
	
	recive_data_pair.first = dummy_1;
	snd_status.status = protocol_module_cinsert::SEND_NG;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	snd_status.status = protocol_module_cinsert::SEND_NG;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	snd_status.status = protocol_module_cinsert::SEND_NG;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	thread_4_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_1;
	recive_data_pair.second.send_status_list.clear();
	thread_4_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_2;
	recive_data_pair.second.send_status_list.clear();
	thread_4_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_3;
	recive_data_pair.second.send_status_list.clear();
	thread_4_read_data->recive_data_map.insert(recive_data_pair);
	
	session_thread_data_map.insert(thread_4_data);
	
	
	// thread 5 data
	// thread_data->thread_division = THREAD_DIVISION_UP_STREAM
	// thread_data->accept_end_flag = ACCEPT_END_FLAG_ON
	// thread_data->sorry_flag = SORRY_FLAG_ON
	// thread_data->recive_data_map.find( thread_data->client_endpoint_tcp ) fond
	// thread_data->end_flag = END_FLAG_OFF
	// recive_data in status = SEND_CONTINUE fond
	
	std::pair< boost::thread::id, protocol_module_cinsert::t_session_thread_data_cinsert > thread_5_data;
	boost::shared_ptr< protocol_module_cinsert::session_thread_data_cinsert > thread_5_read_data(new protocol_module_cinsert::session_thread_data_cinsert);
	thread_5_data.first = test_obj.thread_id_05;
	thread_5_data.second = thread_5_read_data;
	
	thread_5_read_data->thread_division = protocol_module_cinsert::THREAD_DIVISION_UP_STREAM;
	thread_5_read_data->accept_end_flag = protocol_module_cinsert::ACCEPT_END_FLAG_ON;
	thread_5_read_data->sorry_flag = protocol_module_cinsert::SORRY_FLAG_ON;
	thread_5_read_data->client_endpoint_tcp = cl_end;
	thread_5_read_data->end_flag = protocol_module_cinsert::END_FLAG_OFF;
	thread_5_read_data->sorryserver_switch_flag = protocol_module_cinsert::SORRYSERVER_SWITCH_FLAG_OFF;
	
	recive_data_pair.first = cl_end;
	snd_status.status = protocol_module_cinsert::SEND_NG;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	snd_status.status = protocol_module_cinsert::SEND_CONTINUE;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	snd_status.status = protocol_module_cinsert::SEND_OK;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	thread_5_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_1;
	recive_data_pair.second.send_status_list.clear();
	thread_5_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_2;
	recive_data_pair.second.send_status_list.clear();
	thread_5_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_3;
	recive_data_pair.second.send_status_list.clear();
	thread_5_read_data->recive_data_map.insert(recive_data_pair);
	
	session_thread_data_map.insert(thread_5_data);
	
	// thread 6 data
	// thread_data->thread_division = THREAD_DIVISION_UP_STREAM
	// thread_data->accept_end_flag = ACCEPT_END_FLAG_ON
	// thread_data->sorry_flag = SORRY_FLAG_ON
	// thread_data->recive_data_map.find( thread_data->client_endpoint_tcp ) fond
	// thread_data->end_flag = END_FLAG_OFF
	// recive_data in status = SEND_CONTINUE not fond
	
	std::pair< boost::thread::id, protocol_module_cinsert::t_session_thread_data_cinsert > thread_6_data;
	boost::shared_ptr< protocol_module_cinsert::session_thread_data_cinsert > thread_6_read_data(new protocol_module_cinsert::session_thread_data_cinsert);
	thread_6_data.first = test_obj.thread_id_06;
	thread_6_data.second = thread_6_read_data;
	
	thread_6_read_data->thread_division = protocol_module_cinsert::THREAD_DIVISION_UP_STREAM;
	thread_6_read_data->accept_end_flag = protocol_module_cinsert::ACCEPT_END_FLAG_ON;
	thread_6_read_data->sorry_flag = protocol_module_cinsert::SORRY_FLAG_ON;
	thread_6_read_data->client_endpoint_tcp = cl_end;
	thread_6_read_data->end_flag = protocol_module_cinsert::END_FLAG_OFF;
	thread_6_read_data->sorryserver_switch_flag = protocol_module_cinsert::SORRYSERVER_SWITCH_FLAG_OFF;
	
	recive_data_pair.first = cl_end;
	snd_status.status = protocol_module_cinsert::SEND_NG;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	snd_status.status = protocol_module_cinsert::SEND_OK;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	snd_status.status = protocol_module_cinsert::SEND_NG;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	thread_6_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_1;
	recive_data_pair.second.send_status_list.clear();
	thread_6_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_2;
	recive_data_pair.second.send_status_list.clear();
	thread_6_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_3;
	recive_data_pair.second.send_status_list.clear();
	thread_6_read_data->recive_data_map.insert(recive_data_pair);
	
	session_thread_data_map.insert(thread_6_data);
	
	// thread 7 data
	// thread_data->thread_division = THREAD_DIVISION_UP_STREAM
	// thread_data->accept_end_flag = ACCEPT_END_FLAG_ON
	// thread_data->sorry_flag = SORRY_FLAG_ON
	// thread_data->recive_data_map.find( thread_data->client_endpoint_tcp ) not fond
	std::pair< boost::thread::id, protocol_module_cinsert::t_session_thread_data_cinsert > thread_7_data;
	boost::shared_ptr< protocol_module_cinsert::session_thread_data_cinsert > thread_7_read_data(new protocol_module_cinsert::session_thread_data_cinsert);
	thread_7_data.first = test_obj.thread_id_07;
	thread_7_data.second = thread_7_read_data;
	
	thread_7_read_data->thread_division = protocol_module_cinsert::THREAD_DIVISION_UP_STREAM;
	thread_7_read_data->accept_end_flag = protocol_module_cinsert::ACCEPT_END_FLAG_ON;
	thread_7_read_data->sorry_flag = protocol_module_cinsert::SORRY_FLAG_ON;
	thread_7_read_data->client_endpoint_tcp = cl_end;
	
	recive_data_pair.first = dummy_1;
	snd_status.status = protocol_module_cinsert::SEND_NG;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	snd_status.status = protocol_module_cinsert::SEND_NG;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	snd_status.status = protocol_module_cinsert::SEND_NG;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	thread_7_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_1;
	recive_data_pair.second.send_status_list.clear();
	thread_7_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_2;
	recive_data_pair.second.send_status_list.clear();
	thread_7_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_3;
	recive_data_pair.second.send_status_list.clear();
	thread_7_read_data->recive_data_map.insert(recive_data_pair);
	
	session_thread_data_map.insert(thread_7_data);
	
	// thread 8 data
	// thread_data->thread_division = THREAD_DIVISION_DOWN_STREAM
	// thread_data->sorry_flag = SORRY_FLAG_OFF
	// recive_data in status = SEND_OK fond
	std::pair< boost::thread::id, protocol_module_cinsert::t_session_thread_data_cinsert > thread_8_data;
	boost::shared_ptr< protocol_module_cinsert::session_thread_data_cinsert > thread_8_read_data(new protocol_module_cinsert::session_thread_data_cinsert);
	thread_8_data.first = test_obj.thread_id_08;
	thread_8_data.second = thread_8_read_data;
	
	thread_8_read_data->thread_division = protocol_module_cinsert::THREAD_DIVISION_DOWN_STREAM;
	thread_8_read_data->sorry_flag = protocol_module_cinsert::SORRY_FLAG_OFF;
	thread_8_read_data->client_endpoint_tcp = cl_end;
	
	recive_data_pair.first = dummy_1;
	snd_status.status = protocol_module_cinsert::SEND_NG;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	snd_status.status = protocol_module_cinsert::SEND_OK;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	snd_status.status = protocol_module_cinsert::SEND_NG;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	thread_8_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_1;
	recive_data_pair.second.send_status_list.clear();
	thread_8_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_2;
	recive_data_pair.second.send_status_list.clear();
	thread_8_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_3;
	recive_data_pair.second.send_status_list.clear();
	thread_8_read_data->recive_data_map.insert(recive_data_pair);
	
	session_thread_data_map.insert(thread_8_data);
	
	// thread 9 data
	// thread_data->thread_division = THREAD_DIVISION_DOWN_STREAM
	// thread_data->sorry_flag = SORRY_FLAG_OFF
	// recive_data in status = SEND_OK not fond
	
	std::pair< boost::thread::id, protocol_module_cinsert::t_session_thread_data_cinsert > thread_9_data;
	boost::shared_ptr< protocol_module_cinsert::session_thread_data_cinsert > thread_9_read_data(new protocol_module_cinsert::session_thread_data_cinsert);
	thread_9_data.first = test_obj.thread_id_09;
	thread_9_data.second = thread_9_read_data;
		
	thread_9_read_data->thread_division = protocol_module_cinsert::THREAD_DIVISION_DOWN_STREAM;
	thread_9_read_data->sorry_flag = protocol_module_cinsert::SORRY_FLAG_OFF;
	thread_9_read_data->client_endpoint_tcp = cl_end;
	
	recive_data_pair.first = dummy_1;
	snd_status.status = protocol_module_cinsert::SEND_NG;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	snd_status.status = protocol_module_cinsert::SEND_NG;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	snd_status.status = protocol_module_cinsert::SEND_NG;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	thread_9_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_1;
	recive_data_pair.second.send_status_list.clear();
	thread_9_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_2;
	recive_data_pair.second.send_status_list.clear();
	thread_9_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_3;
	recive_data_pair.second.send_status_list.clear();
	thread_9_read_data->recive_data_map.insert(recive_data_pair);
	
	session_thread_data_map.insert(thread_9_data);
	
	
	// thread 10 data
	// thread_data->thread_division = THREAD_DIVISION_DOWN_STREAM
	// thread_data->sorry_flag = SORRY_FLAG_ON
	// recive_data in ALL send_rest_size = 0 & status == SEND_NG fond
	// thread_data->end_flag = END_FLAG_ON
	
	std::pair< boost::thread::id, protocol_module_cinsert::t_session_thread_data_cinsert > thread_10_data;
	boost::shared_ptr< protocol_module_cinsert::session_thread_data_cinsert > thread_10_read_data(new protocol_module_cinsert::session_thread_data_cinsert);
	thread_10_data.first = test_obj.thread_id_10;
	thread_10_data.second = thread_10_read_data;
	
	thread_10_read_data->thread_division = protocol_module_cinsert::THREAD_DIVISION_DOWN_STREAM;
	thread_10_read_data->sorry_flag = protocol_module_cinsert::SORRY_FLAG_ON;
	thread_10_read_data->end_flag = protocol_module_cinsert::END_FLAG_ON;
	
	recive_data_pair.first = dummy_1;
	snd_status.status = protocol_module_cinsert::SEND_OK;
	snd_status.send_rest_size = 0;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	snd_status.status = protocol_module_cinsert::SEND_OK;
	snd_status.send_rest_size = 0;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	snd_status.status = protocol_module_cinsert::SEND_NG;
	snd_status.send_rest_size = 0;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	thread_10_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_1;
	recive_data_pair.second.send_status_list.clear();
	thread_10_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_2;
	recive_data_pair.second.send_status_list.clear();
	thread_10_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_3;
	recive_data_pair.second.send_status_list.clear();
	thread_10_read_data->recive_data_map.insert(recive_data_pair);
	
	session_thread_data_map.insert(thread_10_data);
	
	// thread 11 data
	// thread_data->thread_division = THREAD_DIVISION_DOWN_STREAM
	// thread_data->sorry_flag = SORRY_FLAG_ON
	// recive_data in ALL status != SEND_NG & send_rest_size > 0 fond
	// thread_data->end_flag = END_FLAG_OFF
	
	std::pair< boost::thread::id, protocol_module_cinsert::t_session_thread_data_cinsert > thread_11_data;
	boost::shared_ptr< protocol_module_cinsert::session_thread_data_cinsert > thread_11_read_data(new protocol_module_cinsert::session_thread_data_cinsert);
	thread_11_data.first = test_obj.thread_id_11;
	thread_11_data.second = thread_11_read_data;
	
	thread_11_read_data->thread_division = protocol_module_cinsert::THREAD_DIVISION_DOWN_STREAM;
	thread_11_read_data->sorry_flag = protocol_module_cinsert::SORRY_FLAG_ON;
	thread_11_read_data->end_flag = protocol_module_cinsert::END_FLAG_OFF;
			
	recive_data_pair.first = dummy_1;
	snd_status.status = protocol_module_cinsert::SEND_OK;
	snd_status.send_rest_size = 0;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	snd_status.status = protocol_module_cinsert::SEND_OK;
	snd_status.send_rest_size = 1;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	snd_status.status = protocol_module_cinsert::SEND_OK;
	snd_status.send_rest_size = 0;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	thread_11_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_1;
	recive_data_pair.second.send_status_list.clear();
	thread_11_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_2;
	recive_data_pair.second.send_status_list.clear();
	thread_11_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_3;
	recive_data_pair.second.send_status_list.clear();
	thread_11_read_data->recive_data_map.insert(recive_data_pair);
	
	session_thread_data_map.insert(thread_11_data);
	
	// thread 12 data
	// thread_data->thread_division = THREAD_DIVISION_DOWN_STREAM
	// thread_data->sorry_flag = SORRY_FLAG_ON
	// recive_data in ALL status != SEND_NG & ALL send_rest_size != 0  status = SEND_OK fond
	// thread_data->end_flag = END_FLAG_OFF
	std::pair< boost::thread::id, protocol_module_cinsert::t_session_thread_data_cinsert > thread_12_data;
	boost::shared_ptr< protocol_module_cinsert::session_thread_data_cinsert > thread_12_read_data(new protocol_module_cinsert::session_thread_data_cinsert);
	thread_12_data.first = test_obj.thread_id_12;
	thread_12_data.second = thread_12_read_data;
	
	thread_12_read_data->thread_division = protocol_module_cinsert::THREAD_DIVISION_DOWN_STREAM;
	thread_12_read_data->sorry_flag = protocol_module_cinsert::SORRY_FLAG_ON;
	thread_12_read_data->end_flag = protocol_module_cinsert::END_FLAG_OFF;
	
	recive_data_pair.first = dummy_1;
	snd_status.status = protocol_module_cinsert::SEND_CONTINUE;
	snd_status.send_rest_size = 0;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	snd_status.status = protocol_module_cinsert::SEND_OK;
	snd_status.send_rest_size = 0;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	snd_status.status = protocol_module_cinsert::SEND_CONTINUE;
	snd_status.send_rest_size = 0;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	thread_12_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_1;
	recive_data_pair.second.send_status_list.clear();
	thread_12_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_2;
	recive_data_pair.second.send_status_list.clear();
	thread_12_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_3;
	recive_data_pair.second.send_status_list.clear();
	thread_12_read_data->recive_data_map.insert(recive_data_pair);
	
	session_thread_data_map.insert(thread_12_data);
	
	// thread 13 data
	// thread_data->thread_division = THREAD_DIVISION_DOWN_STREAM
	// thread_data->sorry_flag = SORRY_FLAG_ON
	// recive_data in status = SEND_NG or status == SEND_OK or send_rest_size == 0 not fond
	// thread_data->end_flag = END_FLAG_OFF
	std::pair< boost::thread::id, protocol_module_cinsert::t_session_thread_data_cinsert > thread_13_data;
	boost::shared_ptr< protocol_module_cinsert::session_thread_data_cinsert > thread_13_read_data(new protocol_module_cinsert::session_thread_data_cinsert);
	thread_13_data.first = test_obj.thread_id_13;
	thread_13_data.second = thread_13_read_data;
	
	thread_13_read_data->thread_division = protocol_module_cinsert::THREAD_DIVISION_DOWN_STREAM;
	thread_13_read_data->sorry_flag = protocol_module_cinsert::SORRY_FLAG_ON;
	thread_13_read_data->end_flag = protocol_module_cinsert::END_FLAG_OFF;
	
	recive_data_pair.first = dummy_1;
	snd_status.status = protocol_module_cinsert::SEND_CONTINUE;
	snd_status.send_rest_size = 0;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	snd_status.status = protocol_module_cinsert::SEND_CONTINUE;
	snd_status.send_rest_size = 0;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	snd_status.status = protocol_module_cinsert::SEND_CONTINUE;
	snd_status.send_rest_size = 0;
	recive_data_pair.second.send_status_list.push_back(snd_status);
	thread_13_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_1;
	recive_data_pair.second.send_status_list.clear();
	thread_13_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_2;
	recive_data_pair.second.send_status_list.clear();
	thread_13_read_data->recive_data_map.insert(recive_data_pair);
	
	recive_data_pair.first = dummy_3;
	recive_data_pair.second.send_status_list.clear();
	thread_13_read_data->recive_data_map.insert(recive_data_pair);
	
	session_thread_data_map.insert(thread_13_data);
	
	test_obj.sync_condition.notify_all();
	
//	std::cout << "sync_condition.notify_all" << std::endl;
	
	test_thread_01.join();
//	std::cout << "test_thread_01 stop" << std::endl;
	test_thread_02.join();
//	std::cout << "test_thread_02 stop" << std::endl;
	test_thread_03.join();
//	std::cout << "test_thread_03 stop" << std::endl;
	test_thread_04.join();
//	std::cout << "test_thread_04 stop" << std::endl;
	test_thread_05.join();
//	std::cout << "test_thread_05 stop" << std::endl;
	test_thread_06.join();
//	std::cout << "test_thread_06 stop" << std::endl;
	test_thread_07.join();
//	std::cout << "test_thread_07 stop" << std::endl;
	test_thread_08.join();
//	std::cout << "test_thread_08 stop" << std::endl;
	test_thread_09.join();
//	std::cout << "test_thread_09 stop" << std::endl;
	test_thread_10.join();
//	std::cout << "test_thread_10 stop" << std::endl;
	test_thread_11.join();
//	std::cout << "test_thread_11 stop" << std::endl;
	test_thread_12.join();
//	std::cout << "test_thread_12 stop" << std::endl;
	test_thread_13.join();
//	std::cout << "test_thread_13 stop" << std::endl;*/
 	
	std::cout << "test_thread_01 start time [" << test_obj.start_time_01.nsec << "]" << std::endl;
	std::cout << "test_thread_02 start time [" << test_obj.start_time_02.nsec << "]" << std::endl;
	std::cout << "test_thread_03 start time [" << test_obj.start_time_03.nsec << "]" << std::endl;
	std::cout << "test_thread_04 start time [" << test_obj.start_time_04.nsec << "]" << std::endl;
	std::cout << "test_thread_05 start time [" << test_obj.start_time_05.nsec << "]" << std::endl;
	std::cout << "test_thread_06 start time [" << test_obj.start_time_06.nsec << "]" << std::endl;
	std::cout << "test_thread_07 start time [" << test_obj.start_time_07.nsec << "]" << std::endl;
	std::cout << "test_thread_08 start time [" << test_obj.start_time_08.nsec << "]" << std::endl;
	std::cout << "test_thread_09 start time [" << test_obj.start_time_09.nsec << "]" << std::endl;
	std::cout << "test_thread_10 start time [" << test_obj.start_time_10.nsec << "]" << std::endl;
	std::cout << "test_thread_11 start time [" << test_obj.start_time_11.nsec << "]" << std::endl;
	std::cout << "test_thread_12 start time [" << test_obj.start_time_12.nsec << "]" << std::endl;
	std::cout << "test_thread_13 start time [" << test_obj.start_time_13.nsec << "]" << std::endl;
	
	std::cout << "test_thread_01 end time [" << test_obj.end_time_01.nsec << "]" << std::endl;
	std::cout << "test_thread_02 end time [" << test_obj.end_time_02.nsec << "]" << std::endl;
	std::cout << "test_thread_03 end time [" << test_obj.end_time_03.nsec << "]" << std::endl;
	std::cout << "test_thread_04 end time [" << test_obj.end_time_04.nsec << "]" << std::endl;
	std::cout << "test_thread_05 end time [" << test_obj.end_time_05.nsec << "]" << std::endl;
	std::cout << "test_thread_06 end time [" << test_obj.end_time_06.nsec << "]" << std::endl;
	std::cout << "test_thread_07 end time [" << test_obj.end_time_07.nsec << "]" << std::endl;
	std::cout << "test_thread_08 end time [" << test_obj.end_time_08.nsec << "]" << std::endl;
	std::cout << "test_thread_09 end time [" << test_obj.end_time_09.nsec << "]" << std::endl;
	std::cout << "test_thread_10 end time [" << test_obj.end_time_10.nsec << "]" << std::endl;
	std::cout << "test_thread_11 end time [" << test_obj.end_time_11.nsec << "]" << std::endl;
	std::cout << "test_thread_12 end time [" << test_obj.end_time_12.nsec << "]" << std::endl;
	std::cout << "test_thread_13 end time [" << test_obj.end_time_13.nsec << "]" << std::endl;

	
	// thread 1
	// unit_test [2] handle_sorry_enable THREAD_DIVISION_UP_STREAM & ACCEPT_END_FLAG_OFF case check
	std::cout << "[2] handle_sorry_disable THREAD_DIVISION_UP_STREAM & ACCEPT_END_FLAG_OFF case check" << std::endl;
	// check1 thread_data->sorry_flag = SORRY_FLAG_ON;
	BOOST_CHECK( thread_1_read_data->sorry_flag == protocol_module_cinsert::SORRY_FLAG_OFF );
	// check2 return = ACCEPT;
	BOOST_CHECK_EQUAL(test_obj.status_01,protocol_module_cinsert::ACCEPT);
	
	// thread 2 
	// unit_test [3] handle_sorry_enable THREAD_DIVISION_UP_STREAM & ACCEPT_END_FLAG_ON & SORRY_FLAG_OFF & status SEND_OK fond case check
	std::cout << "[3] handle_sorry_disable THREAD_DIVISION_UP_STREAM & ACCEPT_END_FLAG_ON & SORRY_FLAG_OFF & status SEND_OK fond case check" << std::endl;
	// check1 return REALSERVER_SELECT
	BOOST_CHECK_EQUAL(test_obj.status_02,protocol_module_cinsert::REALSERVER_SELECT);
	
	// thread 3
	// unit_test [4] handle_sorry_enable THREAD_DIVISION_UP_STREAM & ACCEPT_END_FLAG_ON & SORRY_FLAG_OFF status SEND_OK not fond case check
	std::cout << "[4] handle_sorry_disable THREAD_DIVISION_UP_STREAM & ACCEPT_END_FLAG_ON & SORRY_FLAG_OFF status SEND_OK not fond case check" << std::endl;
	// check1 return CLIENT_RECV
	BOOST_CHECK_EQUAL(test_obj.status_03,protocol_module_cinsert::CLIENT_RECV);
	
	// thread 4 
	// unit_test [5] handle_sorry_enable THREAD_DIVISION_UP_STREAM & ACCEPT_END_FLAG_ON & SORRY_FLAG_OFF recive_data_map not fond client_endpoint_tcp case check
	std::cout << "[5] handle_sorry_disable THREAD_DIVISION_UP_STREAM & ACCEPT_END_FLAG_ON & SORRY_FLAG_OFF recive_data_map not fond client_endpoint_tcp case check" << std::endl;
	// check1 return STOP
	BOOST_CHECK_EQUAL(test_obj.status_04,protocol_module_cinsert::STOP);
	
	// thread 5
	// unit_test [6] handle_sorry_enable THREAD_DIVISION_UP_STREAM & ACCEPT_END_FLAG_ON & SORRY_FLAG_OFF & status SEND_CONTINUE not fond case check
	std::cout << "[6] handle_sorry_disable THREAD_DIVISION_UP_STREAM & ACCEPT_END_FLAG_ON & SORRY_FLAG_OFF & status SEND_CONTINUE not fond case check" << std::endl;
	// check1 thread_data->sorryserver_switch_flag = SORRYSERVER_SWITCH_FLAG_ON;
	BOOST_CHECK( thread_5_read_data->realserver_switch_flag == protocol_module_cinsert::REALSERVER_SWITCH_FLAG_ON );
	// check2 thread_data->sorry_flag = SORRY_FLAG_ON;
	BOOST_CHECK( thread_5_read_data->sorry_flag == protocol_module_cinsert::SORRY_FLAG_OFF );
	// check4 thread_data->end_flag = END_FLAG_ON
	BOOST_CHECK( thread_5_read_data->end_flag == protocol_module_cinsert::END_FLAG_ON );
	// check3 return = SORRYSERVER_DISCONNECT;	
	BOOST_CHECK_EQUAL(test_obj.status_05,protocol_module_cinsert::SORRYSERVER_DISCONNECT);
	
	// thread 6
	// unit_test [7] handle_sorry_enable THREAD_DIVISION_UP_STREAM & ACCEPT_END_FLAG_ON & SORRY_FLAG_ON & status SEND_CONTINUE fond case check
	std::cout << "[7] handle_sorry_disable THREAD_DIVISION_UP_STREAM & ACCEPT_END_FLAG_ON & SORRY_FLAG_ON & status SEND_CONTINUE fond case check" << std::endl;
	// check1 thread_data->sorryserver_switch_flag = SORRYSERVER_SWITCH_FLAG_ON;
	BOOST_CHECK( thread_6_read_data->realserver_switch_flag == protocol_module_cinsert::REALSERVER_SWITCH_FLAG_ON );
	// check2 thread_data->sorry_flag = SORRY_FLAG_ON;
	BOOST_CHECK( thread_6_read_data->sorry_flag == protocol_module_cinsert::SORRY_FLAG_OFF );
	// check4 thread_data->end_flag = END_FLAG_OFF
	BOOST_CHECK( thread_6_read_data->end_flag == protocol_module_cinsert::END_FLAG_OFF );
	// check3 return = REALSERVER_DISCONNECT;	
	BOOST_CHECK_EQUAL(test_obj.status_06,protocol_module_cinsert::SORRYSERVER_DISCONNECT);
	
	// thread 7 
	// unit_test [8] handle_sorry_enable THREAD_DIVISION_UP_STREAM & ACCEPT_END_FLAG_ON & SORRY_FLAG_OFF recive_data_map not fond client_endpoint_tcp case check
	std::cout << "[8] handle_sorry_disable THREAD_DIVISION_UP_STREAM & ACCEPT_END_FLAG_ON & SORRY_FLAG_OFF recive_data_map not fond client_endpoint_tcp case check" << std::endl;
	// check1 return STOP
	BOOST_CHECK_EQUAL(test_obj.status_07,protocol_module_cinsert::STOP);
	
	// thread 8 
	// unit_test [9] handle_sorry_enable THREAD_DIVISION_DOWN_STREAM & SORRY_FLAG_OFF & status SEND_OK fond case check
	std::cout << "[9] handle_sorry_disable THREAD_DIVISION_DOWN_STREAM & SORRY_FLAG_OFF & status SEND_OK fond case check" << std::endl;
	// check1 return CLIENT_CONNECTION_CHECK
	BOOST_CHECK_EQUAL(test_obj.status_08,protocol_module_cinsert::CLIENT_CONNECTION_CHECK);
	
	// thread 9 
	// unit_test [10] handle_sorry_enable THREAD_DIVISION_DOWN_STREAM & SORRY_FLAG_ON & status SEND_OK not fond case check
	std::cout << "[10] handle_sorry_disable THREAD_DIVISION_DOWN_STREAM & SORRY_FLAG_ON & status SEND_OK not fond case check" << std::endl;
	// check1 return SORRYSERVER_RECV
	BOOST_CHECK_EQUAL(test_obj.status_09,protocol_module_cinsert::REALSERVER_RECV);
	
	// thread 10 
	// unit_test [11] handle_sorry_enable THREAD_DIVISION_DOWN_STREAM & SORRY_FLAG_ON & ALL send_rest_size == 0 & status == SEND_NG fond case check
	std::cout << "[11] handle_sorry_disable THREAD_DIVISION_DOWN_STREAM & SORRY_FLAG_ON & ALL send_rest_size == 0 & status == SEND_NG fond case check" << std::endl;
	// check 1 thread_data->sorry_flag = SORRY_FLAG_OFF
	BOOST_CHECK( thread_10_read_data->sorry_flag == protocol_module_cinsert::SORRY_FLAG_OFF );
	// check 2 thread_data->end_flag = END_FLAG_ON
	BOOST_CHECK( thread_10_read_data->end_flag == protocol_module_cinsert::END_FLAG_ON );
	// check 3 status = SORRYSERVER_DISCONNECT
	BOOST_CHECK_EQUAL(test_obj.status_10,protocol_module_cinsert::SORRYSERVER_DISCONNECT);
	
	// thread 11 
	// unit_test [12] handle_sorry_enable THREAD_DIVISION_DOWN_STREAM & SORRY_FLAG_OFF & ALL status != SEND_NG & send_rest_size > 0 fond case check
	std::cout << "[12] handle_sorry_disable THREAD_DIVISION_DOWN_STREAM & SORRY_FLAG_OFF & ALL status != SEND_NG & send_rest_size > 0 fond case check" << std::endl;
	// check 1 thread_data->sorry_flag = SORRY_FLAG_OFF
	BOOST_CHECK( thread_11_read_data->sorry_flag == protocol_module_cinsert::SORRY_FLAG_OFF );
	// check 2 thread_data->end_flag = END_FLAG_ON
	BOOST_CHECK( thread_11_read_data->end_flag == protocol_module_cinsert::END_FLAG_ON );
	// check 3 status = SORRYSERVER_DISCONNECT
	BOOST_CHECK_EQUAL(test_obj.status_11,protocol_module_cinsert::SORRYSERVER_DISCONNECT);
	
	// thread 12
	// unit_test [13] handle_sorry_enable THREAD_DIVISION_DOWN_STREAM & SORRY_FLAG_ON & ALL status != SEND_NG & send_rest_size > 0 fond case check
	std::cout << "[13] handle_sorry_disable THREAD_DIVISION_DOWN_STREAM & SORRY_FLAG_ON & ALL status != SEND_NG & ALL send_rest_size != 0  status = SEND_OK fond case check" << std::endl;
	// check 1 thread_data->sorry_flag = SORRY_FLAG_OFF
	BOOST_CHECK( thread_13_read_data->sorry_flag == protocol_module_cinsert::SORRY_FLAG_OFF );
	// check 2 thread_data->end_flag = END_FLAG_OFF not update
	BOOST_CHECK( thread_13_read_data->end_flag == protocol_module_cinsert::END_FLAG_OFF );
	// check 3 status = CLIENT_CONNECTION_CHECK
	BOOST_CHECK_EQUAL(test_obj.status_12,protocol_module_cinsert::CLIENT_CONNECTION_CHECK);
	
	// thread 13
	// unit_test [14] handle_sorry_enable THREAD_DIVISION_DOWN_STREAM & SORRY_FLAG_ON & ALL status != SEND_NG & send_rest_size > 0 fond case check
	std::cout << "[14] handle_sorry_disable THREAD_DIVISION_DOWN_STREAM & SORRY_FLAG_ON & status == SEND_NG or status == SEND_OK or send_rest_size == 0 not fond case check" << std::endl;
	// check 1 thread_data->sorry_flag = SORRY_FLAG_OFF  not update
	BOOST_CHECK( thread_13_read_data->end_flag == protocol_module_cinsert::SORRY_FLAG_OFF );
	// check 2 thread_data->end_flag = END_FLAG_OFF not update
	BOOST_CHECK( thread_13_read_data->end_flag == protocol_module_cinsert::END_FLAG_OFF );
	// check 3 status = SORRYSERVER_RECV
	BOOST_CHECK_EQUAL(test_obj.status_13,protocol_module_cinsert::REALSERVER_RECV);
	
	
	BOOST_MESSAGE( "----- handle_sorry_disable test end -----" );
}





//--handle_realserver_connection_fail_test_class --
class	handle_realserver_connection_fail_test_class : public protocol_module_cinsert
{
	public:
		handle_realserver_connection_fail_test_class() : protocol_module_cinsert()
		{
			getloglevel	= &stb_getloglevel;
			putLogFatal	= &stb_putLogFatal;
			putLogError	= &stb_putLogError;
			putLogWarn	= &stb_putLogWarn;
			putLogInfo	= &stb_putLogInfo;
			putLogDebug	= &stb_putLogDebug;
		};
		~handle_realserver_connection_fail_test_class(){};
		
		
		boost::condition	sync_condition;
		
		boost::thread::id thread_id_01;
		boost::asio::ip::tcp::endpoint rs_end_01;
		boost::mutex		sync_mutex_01;
		protocol_module_cinsert::EVENT_TAG	status_01;
		boost::xtime	start_time_01;
		boost::xtime	end_time_01;
		
		boost::thread::id thread_id_02;
		boost::asio::ip::tcp::endpoint rs_end_02;
		boost::mutex		sync_mutex_02;
		protocol_module_cinsert::EVENT_TAG	status_02;
		boost::xtime	start_time_02;
		boost::xtime	end_time_02;
		
		boost::thread::id thread_id_03;
		boost::asio::ip::tcp::endpoint rs_end_03;
		boost::mutex		sync_mutex_03;
		protocol_module_cinsert::EVENT_TAG	status_03;
		boost::xtime	start_time_03;
		boost::xtime	end_time_03;
		
		boost::thread::id thread_id_04;
		boost::asio::ip::tcp::endpoint rs_end_04;
		boost::mutex		sync_mutex_04;
		protocol_module_cinsert::EVENT_TAG	status_04;
		boost::xtime	start_time_04;
		boost::xtime	end_time_04;
		
		boost::thread::id thread_id_05;
		boost::asio::ip::tcp::endpoint rs_end_05;
		boost::mutex		sync_mutex_05;
		protocol_module_cinsert::EVENT_TAG	status_05;
		boost::xtime	start_time_05;
		boost::xtime	end_time_05;
		
		boost::thread::id thread_id_06;
		boost::asio::ip::tcp::endpoint rs_end_06;
		boost::mutex		sync_mutex_06;
		protocol_module_cinsert::EVENT_TAG	status_06;
		boost::xtime	start_time_06;
		boost::xtime	end_time_06;
		
		boost::thread::id thread_id_07;
		boost::asio::ip::tcp::endpoint rs_end_07;
		boost::mutex		sync_mutex_07;
		protocol_module_cinsert::EVENT_TAG	status_07;
		boost::xtime	start_time_07;
		boost::xtime	end_time_07;
		
		boost::thread::id thread_id_08;
		boost::asio::ip::tcp::endpoint rs_end_08;
		boost::mutex		sync_mutex_08;
		protocol_module_cinsert::EVENT_TAG	status_08;
		boost::xtime	start_time_08;
		boost::xtime	end_time_08;
		
		boost::thread::id thread_id_09;
		boost::asio::ip::tcp::endpoint rs_end_09;
		boost::mutex		sync_mutex_09;
		protocol_module_cinsert::EVENT_TAG	status_09;
		boost::xtime	start_time_09;
		boost::xtime	end_time_09;
		
		boost::thread::id thread_id_10;
		boost::asio::ip::tcp::endpoint rs_end_10;
		boost::mutex		sync_mutex_10;
		protocol_module_cinsert::EVENT_TAG	status_10;
		boost::xtime	start_time_10;
		boost::xtime	end_time_10;
		
		void test_thread_01_run(){
			thread_id_01 = boost::this_thread::get_id();
			boost::mutex::scoped_lock	lk( sync_mutex_01 );
			sync_condition.wait( lk );
			boost::xtime_get(&start_time_01, boost::TIME_UTC);
			status_01 = handle_realserver_connection_fail(thread_id_01,rs_end_01);
			boost::xtime_get(&end_time_01, boost::TIME_UTC);
		};
		
		void test_thread_02_run(){
			thread_id_02 = boost::this_thread::get_id();
			boost::mutex::scoped_lock	lk( sync_mutex_02 );
			sync_condition.wait( lk );
			boost::xtime_get(&start_time_02, boost::TIME_UTC);
			status_02 = handle_realserver_connection_fail(thread_id_02,rs_end_02);
			boost::xtime_get(&end_time_02, boost::TIME_UTC);
		};
		
		void test_thread_03_run(){
			thread_id_03 = boost::this_thread::get_id();
			boost::mutex::scoped_lock	lk( sync_mutex_03 );
			sync_condition.wait( lk );
			boost::xtime_get(&start_time_03, boost::TIME_UTC);
			status_03 = handle_realserver_connection_fail(thread_id_03,rs_end_03);
			boost::xtime_get(&end_time_03, boost::TIME_UTC);
		};
		
		void test_thread_04_run(){
			thread_id_04 = boost::this_thread::get_id();
			boost::mutex::scoped_lock	lk( sync_mutex_04 );
			sync_condition.wait( lk );
			boost::xtime_get(&start_time_04, boost::TIME_UTC);
			status_04 = handle_realserver_connection_fail(thread_id_04,rs_end_04);
			boost::xtime_get(&end_time_04, boost::TIME_UTC);
		};
		
		void test_thread_05_run(){
			thread_id_05 = boost::this_thread::get_id();
			boost::mutex::scoped_lock	lk( sync_mutex_05 );
			sync_condition.wait( lk );
			boost::xtime_get(&start_time_05, boost::TIME_UTC);
			status_05 = handle_realserver_connection_fail(thread_id_05,rs_end_05);
			boost::xtime_get(&end_time_05, boost::TIME_UTC);
		};
		
		void test_thread_06_run(){
			thread_id_06 = boost::this_thread::get_id();
			boost::mutex::scoped_lock	lk( sync_mutex_06 );
			sync_condition.wait( lk );
			boost::xtime_get(&start_time_06, boost::TIME_UTC);
			status_06 = handle_realserver_connection_fail(thread_id_06,rs_end_06);
			boost::xtime_get(&end_time_06, boost::TIME_UTC);
		};
		
		void test_thread_07_run(){
			thread_id_07 = boost::this_thread::get_id();
			boost::mutex::scoped_lock	lk( sync_mutex_07 );
			sync_condition.wait( lk );
			boost::xtime_get(&start_time_07, boost::TIME_UTC);
			status_07 = handle_realserver_connection_fail(thread_id_07,rs_end_07);
			boost::xtime_get(&end_time_07, boost::TIME_UTC);
		};
		
		void test_thread_08_run(){
			thread_id_07 = boost::this_thread::get_id();
			boost::mutex::scoped_lock	lk( sync_mutex_08);
			sync_condition.wait( lk );
			boost::xtime_get(&start_time_08, boost::TIME_UTC);
			status_08 = handle_realserver_connection_fail(thread_id_08,rs_end_08);
			boost::xtime_get(&end_time_08, boost::TIME_UTC);
		};
		
		void test_thread_09_run(){
			thread_id_09 = boost::this_thread::get_id();
			boost::mutex::scoped_lock	lk( sync_mutex_09);
			sync_condition.wait( lk );
			boost::xtime_get(&start_time_09, boost::TIME_UTC);
			status_09 = handle_realserver_connection_fail(thread_id_09,rs_end_09);
			boost::xtime_get(&end_time_09, boost::TIME_UTC);
		};
		
		void test_thread_10_run(){
			thread_id_10 = boost::this_thread::get_id();
			boost::mutex::scoped_lock	lk( sync_mutex_10);
			sync_condition.wait( lk );
			boost::xtime_get(&start_time_10, boost::TIME_UTC);
			status_10 = handle_realserver_connection_fail(thread_id_10,rs_end_10);
			boost::xtime_get(&end_time_10, boost::TIME_UTC);
		};
		
		
		t_session_thread_data_map& get_session_thread_data_map(){
			return session_thread_data_map;
		};
		
};

void handle_realserver_connection_fail_test(){
	BOOST_MESSAGE( "----- handle_realserver_connection_fail test end -----" );
	
	handle_realserver_connection_fail_test_class	test_obj;
	protocol_module_cinsert::t_session_thread_data_map& session_thread_data_map = test_obj.get_session_thread_data_map();
	
	// session_thread_data_map empty
	session_thread_data_map.clear();
	
	// unit_test [1] handle_realserver_connection_fail session_thread_data_map empty return STOP
	std::cout << "[1] handle_realserver_connection_fail session_thread_data_map empty return STOP" << std::endl;
	protocol_module_cinsert::EVENT_TAG status = test_obj.handle_realserver_connection_fail(boost::this_thread::get_id(),boost::asio::ip::tcp::endpoint());
	// check1 return STOP 
	BOOST_CHECK_EQUAL(status,protocol_module_cinsert::STOP);
	
	boost::thread test_thread_01(boost::bind(&handle_realserver_connection_fail_test_class::test_thread_01_run,&test_obj));
	boost::thread test_thread_02(boost::bind(&handle_realserver_connection_fail_test_class::test_thread_02_run,&test_obj));
	boost::thread test_thread_03(boost::bind(&handle_realserver_connection_fail_test_class::test_thread_03_run,&test_obj));
	boost::thread test_thread_04(boost::bind(&handle_realserver_connection_fail_test_class::test_thread_04_run,&test_obj));
	boost::thread test_thread_05(boost::bind(&handle_realserver_connection_fail_test_class::test_thread_05_run,&test_obj));
	boost::thread test_thread_06(boost::bind(&handle_realserver_connection_fail_test_class::test_thread_06_run,&test_obj));
	boost::thread test_thread_07(boost::bind(&handle_realserver_connection_fail_test_class::test_thread_07_run,&test_obj));
	boost::thread test_thread_08(boost::bind(&handle_realserver_connection_fail_test_class::test_thread_08_run,&test_obj));
	boost::thread test_thread_09(boost::bind(&handle_realserver_connection_fail_test_class::test_thread_09_run,&test_obj));
	boost::thread test_thread_10(boost::bind(&handle_realserver_connection_fail_test_class::test_thread_10_run,&test_obj));

	// sync_condition wait
	sleep(2);
	
	// set session_thread_data_map
	
	// thread 1
	// session_thread_data_map fond thread id
	// thread_data is NULL
	std::pair< boost::thread::id, protocol_module_cinsert::t_session_thread_data_cinsert > thread_1_data;
	boost::shared_ptr< protocol_module_cinsert::session_thread_data_cinsert > thread_1_read_data(new protocol_module_cinsert::session_thread_data_cinsert);
	thread_1_data.first = test_obj.thread_id_01;
	session_thread_data_map.insert(thread_1_data);
	
	// thread 2
	// session_thread_data_map fond thread id
	// thread_data is not NULL
	// thread_data->end_flag = END_FLAG_OFF
	std::pair< boost::thread::id, protocol_module_cinsert::t_session_thread_data_cinsert > thread_2_data;
	boost::shared_ptr< protocol_module_cinsert::session_thread_data_cinsert > thread_2_read_data(new protocol_module_cinsert::session_thread_data_cinsert);
	thread_2_data.first = test_obj.thread_id_02;
	thread_2_data.second = thread_2_read_data;
	thread_2_read_data->end_flag = protocol_module_cinsert::END_FLAG_OFF;
	session_thread_data_map.insert(thread_2_data);
	
	// thread 3
	// session_thread_data_map fond thread id
	// thread_data is NULL
	std::pair< boost::thread::id, protocol_module_cinsert::t_session_thread_data_cinsert > thread_3_data;
	boost::shared_ptr< protocol_module_cinsert::session_thread_data_cinsert > thread_3_read_data(new protocol_module_cinsert::session_thread_data_cinsert);
	thread_3_data.first = test_obj.thread_id_03;
	session_thread_data_map.insert(thread_3_data);
	
	// thread 4
	// session_thread_data_map fond thread id
	// thread_data is not NULL
	// thread_data->end_flag = END_FLAG_OFF
	std::pair< boost::thread::id, protocol_module_cinsert::t_session_thread_data_cinsert > thread_4_data;
	boost::shared_ptr< protocol_module_cinsert::session_thread_data_cinsert > thread_4_read_data(new protocol_module_cinsert::session_thread_data_cinsert);
	thread_4_data.first = test_obj.thread_id_04;
	thread_4_data.second = thread_4_read_data;
	thread_4_read_data->end_flag = protocol_module_cinsert::END_FLAG_OFF;
	session_thread_data_map.insert(thread_4_data);
	
	// thread 5
	// session_thread_data_map fond thread id
	// thread_data is NULL
	std::pair< boost::thread::id, protocol_module_cinsert::t_session_thread_data_cinsert > thread_5_data;
	boost::shared_ptr< protocol_module_cinsert::session_thread_data_cinsert > thread_5_read_data(new protocol_module_cinsert::session_thread_data_cinsert);
	thread_5_data.first = test_obj.thread_id_05;
	session_thread_data_map.insert(thread_5_data);
	
	// thread 6
	// session_thread_data_map fond thread id
	// thread_data is not NULL
	// thread_data->end_flag = END_FLAG_OFF
	std::pair< boost::thread::id, protocol_module_cinsert::t_session_thread_data_cinsert > thread_6_data;
	boost::shared_ptr< protocol_module_cinsert::session_thread_data_cinsert > thread_6_read_data(new protocol_module_cinsert::session_thread_data_cinsert);
	thread_6_data.first = test_obj.thread_id_06;
	thread_6_data.second = thread_6_read_data;
	thread_6_read_data->end_flag = protocol_module_cinsert::END_FLAG_OFF;
	session_thread_data_map.insert(thread_6_data);
	
	// thread 7
	// session_thread_data_map fond thread id
	// thread_data is NULL
	std::pair< boost::thread::id, protocol_module_cinsert::t_session_thread_data_cinsert > thread_7_data;
	boost::shared_ptr< protocol_module_cinsert::session_thread_data_cinsert > thread_7_read_data(new protocol_module_cinsert::session_thread_data_cinsert);
	thread_7_data.first = test_obj.thread_id_07;
	session_thread_data_map.insert(thread_7_data);
	
	// thread 8
	// session_thread_data_map fond thread id
	// thread_data is not NULL
	// thread_data->end_flag = END_FLAG_OFF
	std::pair< boost::thread::id, protocol_module_cinsert::t_session_thread_data_cinsert > thread_8_data;
	boost::shared_ptr< protocol_module_cinsert::session_thread_data_cinsert > thread_8_read_data(new protocol_module_cinsert::session_thread_data_cinsert);
	thread_8_data.first = test_obj.thread_id_08;
	thread_8_data.second = thread_8_read_data;
	thread_8_read_data->end_flag = protocol_module_cinsert::END_FLAG_OFF;
	session_thread_data_map.insert(thread_8_data);
	
	// thread 9
	// session_thread_data_map fond thread id
	// thread_data is NULL
	std::pair< boost::thread::id, protocol_module_cinsert::t_session_thread_data_cinsert > thread_9_data;
	boost::shared_ptr< protocol_module_cinsert::session_thread_data_cinsert > thread_9_read_data(new protocol_module_cinsert::session_thread_data_cinsert);
	thread_9_data.first = test_obj.thread_id_09;
	session_thread_data_map.insert(thread_9_data);
	
	// thread 10
	// session_thread_data_map fond thread id
	// thread_data is not NULL
	// thread_data->end_flag = END_FLAG_OFF
	std::pair< boost::thread::id, protocol_module_cinsert::t_session_thread_data_cinsert > thread_10_data;
	boost::shared_ptr< protocol_module_cinsert::session_thread_data_cinsert > thread_10_read_data(new protocol_module_cinsert::session_thread_data_cinsert);
	thread_10_data.first = test_obj.thread_id_10;
	thread_10_data.second = thread_10_read_data;
	thread_10_read_data->end_flag = protocol_module_cinsert::END_FLAG_OFF;
	session_thread_data_map.insert(thread_10_data);
	
	
	test_obj.sync_condition.notify_all();
	
//	std::cout << "sync_condition.notify_all" << std::endl;
	
	test_thread_01.join();
//	std::cout << "test_thread_01 stop" << std::endl;
	test_thread_02.join();
//	std::cout << "test_thread_02 stop" << std::endl;
	test_thread_03.join();
//	std::cout << "test_thread_03 stop" << std::endl;
	test_thread_04.join();
//	std::cout << "test_thread_04 stop" << std::endl;
	test_thread_05.join();
//	std::cout << "test_thread_05 stop" << std::endl;
	test_thread_06.join();
//	std::cout << "test_thread_06 stop" << std::endl;
	test_thread_07.join();
//	std::cout << "test_thread_07 stop" << std::endl;
	test_thread_08.join();
//	std::cout << "test_thread_08 stop" << std::endl;
	test_thread_09.join();
//	std::cout << "test_thread_09 stop" << std::endl;
	test_thread_10.join();
//	std::cout << "test_thread_10 stop" << std::endl;
 	
	std::cout << "test_thread_01 start time [" << test_obj.start_time_01.nsec << "]" << std::endl;
	std::cout << "test_thread_02 start time [" << test_obj.start_time_02.nsec << "]" << std::endl;
	std::cout << "test_thread_03 start time [" << test_obj.start_time_03.nsec << "]" << std::endl;
	std::cout << "test_thread_04 start time [" << test_obj.start_time_04.nsec << "]" << std::endl;
	std::cout << "test_thread_05 start time [" << test_obj.start_time_05.nsec << "]" << std::endl;
	std::cout << "test_thread_06 start time [" << test_obj.start_time_06.nsec << "]" << std::endl;
	std::cout << "test_thread_07 start time [" << test_obj.start_time_07.nsec << "]" << std::endl;
	std::cout << "test_thread_08 start time [" << test_obj.start_time_08.nsec << "]" << std::endl;
	std::cout << "test_thread_09 start time [" << test_obj.start_time_09.nsec << "]" << std::endl;
	std::cout << "test_thread_10 start time [" << test_obj.start_time_10.nsec << "]" << std::endl;
	
	std::cout << "test_thread_01 end time [" << test_obj.end_time_01.nsec << "]" << std::endl;
	std::cout << "test_thread_02 end time [" << test_obj.end_time_02.nsec << "]" << std::endl;
	std::cout << "test_thread_03 end time [" << test_obj.end_time_03.nsec << "]" << std::endl;
	std::cout << "test_thread_04 end time [" << test_obj.end_time_04.nsec << "]" << std::endl;
	std::cout << "test_thread_05 end time [" << test_obj.end_time_05.nsec << "]" << std::endl;
	std::cout << "test_thread_06 end time [" << test_obj.end_time_06.nsec << "]" << std::endl;
	std::cout << "test_thread_07 end time [" << test_obj.end_time_07.nsec << "]" << std::endl;
	std::cout << "test_thread_08 end time [" << test_obj.end_time_08.nsec << "]" << std::endl;
	std::cout << "test_thread_09 end time [" << test_obj.end_time_09.nsec << "]" << std::endl;
	std::cout << "test_thread_10 end time [" << test_obj.end_time_10.nsec << "]" << std::endl;
	
	// unit_test [2] handle_realserver_connection_fail thread_data is NULL return STOP
	std::cout << "[2] handle_realserver_connection_fail thread_data is NULL return STOP" << std::endl;
	
	// thread 1
	// check1 return STOP
	BOOST_CHECK_EQUAL(test_obj.status_01,protocol_module_cinsert::STOP);
	
	// thread 3
	// check1 return STOP
	BOOST_CHECK_EQUAL(test_obj.status_03,protocol_module_cinsert::STOP);
	
	// thread 5
	// check1 return STOP
	BOOST_CHECK_EQUAL(test_obj.status_05,protocol_module_cinsert::STOP);
	
	// thread 7
	// check1 return STOP
	BOOST_CHECK_EQUAL(test_obj.status_07,protocol_module_cinsert::STOP);
	
	// thread 9
	// check1 return STOP
	BOOST_CHECK_EQUAL(test_obj.status_09,protocol_module_cinsert::STOP);

	// unit_test [3] handle_realserver_connection_fail thread_data is not NULL return REALSERVER_DISCONNECT
	std::cout << "[3] handle_realserver_connection_fail thread_data is not NULL return REALSERVER_DISCONNECT" << std::endl;
	// thread 2
	// check1 thread_data->end_flag = END_FLAG_ON
	BOOST_CHECK( thread_2_read_data->end_flag == protocol_module_cinsert::END_FLAG_ON );
	// check2 return REALSERVER_DISCONNECT
	BOOST_CHECK_EQUAL(test_obj.status_02,protocol_module_cinsert::CLIENT_DISCONNECT);
		
	// thread 4
	// check1 thread_data->end_flag = END_FLAG_ON
	BOOST_CHECK( thread_4_read_data->end_flag == protocol_module_cinsert::END_FLAG_ON );
	// check2 return REALSERVER_DISCONNECT
	BOOST_CHECK_EQUAL(test_obj.status_04,protocol_module_cinsert::CLIENT_DISCONNECT);

	// thread 6
	// check1 thread_data->end_flag = END_FLAG_ON
	BOOST_CHECK( thread_6_read_data->end_flag == protocol_module_cinsert::END_FLAG_ON );
	// check2 return REALSERVER_DISCONNECT
	BOOST_CHECK_EQUAL(test_obj.status_06,protocol_module_cinsert::CLIENT_DISCONNECT);
	
	// thread 8
	// check1 thread_data->end_flag = END_FLAG_ON
	BOOST_CHECK( thread_8_read_data->end_flag == protocol_module_cinsert::END_FLAG_ON );
	// check2 return REALSERVER_DISCONNECT
	BOOST_CHECK_EQUAL(test_obj.status_08,protocol_module_cinsert::CLIENT_DISCONNECT);
	
	// thread 10
	// check1 thread_data->end_flag = END_FLAG_ON
	BOOST_CHECK( thread_10_read_data->end_flag == protocol_module_cinsert::END_FLAG_ON );
	// check2 return REALSERVER_DISCONNECT
	BOOST_CHECK_EQUAL(test_obj.status_10,protocol_module_cinsert::CLIENT_DISCONNECT);

	BOOST_MESSAGE( "----- handle_realserver_connection_fail test end -----" );
}

//--handle_sorryserver_connection_fail_test_class --
class	handle_sorryserver_connection_fail_test_class : public protocol_module_cinsert
{
	public:
		handle_sorryserver_connection_fail_test_class() : protocol_module_cinsert()
		{
			getloglevel	= &stb_getloglevel;
			putLogFatal	= &stb_putLogFatal;
			putLogError	= &stb_putLogError;
			putLogWarn	= &stb_putLogWarn;
			putLogInfo	= &stb_putLogInfo;
			putLogDebug	= &stb_putLogDebug;
		};
		~handle_sorryserver_connection_fail_test_class(){};
		
		
		boost::condition	sync_condition;
		
		boost::thread::id thread_id_01;
		boost::asio::ip::tcp::endpoint rs_end_01;
		boost::mutex		sync_mutex_01;
		protocol_module_cinsert::EVENT_TAG	status_01;
		boost::xtime	start_time_01;
		boost::xtime	end_time_01;
		
		boost::thread::id thread_id_02;
		boost::asio::ip::tcp::endpoint rs_end_02;
		boost::mutex		sync_mutex_02;
		protocol_module_cinsert::EVENT_TAG	status_02;
		boost::xtime	start_time_02;
		boost::xtime	end_time_02;
		
		boost::thread::id thread_id_03;
		boost::asio::ip::tcp::endpoint rs_end_03;
		boost::mutex		sync_mutex_03;
		protocol_module_cinsert::EVENT_TAG	status_03;
		boost::xtime	start_time_03;
		boost::xtime	end_time_03;
		
		boost::thread::id thread_id_04;
		boost::asio::ip::tcp::endpoint rs_end_04;
		boost::mutex		sync_mutex_04;
		protocol_module_cinsert::EVENT_TAG	status_04;
		boost::xtime	start_time_04;
		boost::xtime	end_time_04;
		
		boost::thread::id thread_id_05;
		boost::asio::ip::tcp::endpoint rs_end_05;
		boost::mutex		sync_mutex_05;
		protocol_module_cinsert::EVENT_TAG	status_05;
		boost::xtime	start_time_05;
		boost::xtime	end_time_05;
		
		boost::thread::id thread_id_06;
		boost::asio::ip::tcp::endpoint rs_end_06;
		boost::mutex		sync_mutex_06;
		protocol_module_cinsert::EVENT_TAG	status_06;
		boost::xtime	start_time_06;
		boost::xtime	end_time_06;
		
		boost::thread::id thread_id_07;
		boost::asio::ip::tcp::endpoint rs_end_07;
		boost::mutex		sync_mutex_07;
		protocol_module_cinsert::EVENT_TAG	status_07;
		boost::xtime	start_time_07;
		boost::xtime	end_time_07;
		
		boost::thread::id thread_id_08;
		boost::asio::ip::tcp::endpoint rs_end_08;
		boost::mutex		sync_mutex_08;
		protocol_module_cinsert::EVENT_TAG	status_08;
		boost::xtime	start_time_08;
		boost::xtime	end_time_08;
		
		boost::thread::id thread_id_09;
		boost::asio::ip::tcp::endpoint rs_end_09;
		boost::mutex		sync_mutex_09;
		protocol_module_cinsert::EVENT_TAG	status_09;
		boost::xtime	start_time_09;
		boost::xtime	end_time_09;
		
		boost::thread::id thread_id_10;
		boost::asio::ip::tcp::endpoint rs_end_10;
		boost::mutex		sync_mutex_10;
		protocol_module_cinsert::EVENT_TAG	status_10;
		boost::xtime	start_time_10;
		boost::xtime	end_time_10;
		
		void test_thread_01_run(){
			thread_id_01 = boost::this_thread::get_id();
			boost::mutex::scoped_lock	lk( sync_mutex_01 );
			sync_condition.wait( lk );
			boost::xtime_get(&start_time_01, boost::TIME_UTC);
			status_01 = handle_sorryserver_connection_fail(thread_id_01,rs_end_01);
			boost::xtime_get(&end_time_01, boost::TIME_UTC);
		};
		
		void test_thread_02_run(){
			thread_id_02 = boost::this_thread::get_id();
			boost::mutex::scoped_lock	lk( sync_mutex_02 );
			sync_condition.wait( lk );
			boost::xtime_get(&start_time_02, boost::TIME_UTC);
			status_02 = handle_sorryserver_connection_fail(thread_id_02,rs_end_02);
			boost::xtime_get(&end_time_02, boost::TIME_UTC);
		};
		
		void test_thread_03_run(){
			thread_id_03 = boost::this_thread::get_id();
			boost::mutex::scoped_lock	lk( sync_mutex_03 );
			sync_condition.wait( lk );
			boost::xtime_get(&start_time_03, boost::TIME_UTC);
			status_03 = handle_sorryserver_connection_fail(thread_id_03,rs_end_03);
			boost::xtime_get(&end_time_03, boost::TIME_UTC);
		};
		
		void test_thread_04_run(){
			thread_id_04 = boost::this_thread::get_id();
			boost::mutex::scoped_lock	lk( sync_mutex_04 );
			sync_condition.wait( lk );
			boost::xtime_get(&start_time_04, boost::TIME_UTC);
			status_04 = handle_sorryserver_connection_fail(thread_id_04,rs_end_04);
			boost::xtime_get(&end_time_04, boost::TIME_UTC);
		};
		
		void test_thread_05_run(){
			thread_id_05 = boost::this_thread::get_id();
			boost::mutex::scoped_lock	lk( sync_mutex_05 );
			sync_condition.wait( lk );
			boost::xtime_get(&start_time_05, boost::TIME_UTC);
			status_05 = handle_sorryserver_connection_fail(thread_id_05,rs_end_05);
			boost::xtime_get(&end_time_05, boost::TIME_UTC);
		};
		
		void test_thread_06_run(){
			thread_id_06 = boost::this_thread::get_id();
			boost::mutex::scoped_lock	lk( sync_mutex_06 );
			sync_condition.wait( lk );
			boost::xtime_get(&start_time_06, boost::TIME_UTC);
			status_06 = handle_sorryserver_connection_fail(thread_id_06,rs_end_06);
			boost::xtime_get(&end_time_06, boost::TIME_UTC);
		};
		
		void test_thread_07_run(){
			thread_id_07 = boost::this_thread::get_id();
			boost::mutex::scoped_lock	lk( sync_mutex_07 );
			sync_condition.wait( lk );
			boost::xtime_get(&start_time_07, boost::TIME_UTC);
			status_07 = handle_sorryserver_connection_fail(thread_id_07,rs_end_07);
			boost::xtime_get(&end_time_07, boost::TIME_UTC);
		};
		
		void test_thread_08_run(){
			thread_id_07 = boost::this_thread::get_id();
			boost::mutex::scoped_lock	lk( sync_mutex_08);
			sync_condition.wait( lk );
			boost::xtime_get(&start_time_08, boost::TIME_UTC);
			status_08 = handle_sorryserver_connection_fail(thread_id_08,rs_end_08);
			boost::xtime_get(&end_time_08, boost::TIME_UTC);
		};
		
		void test_thread_09_run(){
			thread_id_09 = boost::this_thread::get_id();
			boost::mutex::scoped_lock	lk( sync_mutex_09);
			sync_condition.wait( lk );
			boost::xtime_get(&start_time_09, boost::TIME_UTC);
			status_09 = handle_sorryserver_connection_fail(thread_id_09,rs_end_09);
			boost::xtime_get(&end_time_09, boost::TIME_UTC);
		};
		
		void test_thread_10_run(){
			thread_id_10 = boost::this_thread::get_id();
			boost::mutex::scoped_lock	lk( sync_mutex_10);
			sync_condition.wait( lk );
			boost::xtime_get(&start_time_10, boost::TIME_UTC);
			status_10 = handle_sorryserver_connection_fail(thread_id_10,rs_end_10);
			boost::xtime_get(&end_time_10, boost::TIME_UTC);
		};
		
		
		t_session_thread_data_map& get_session_thread_data_map(){
			return session_thread_data_map;
		};
		
};

void handle_sorryserver_connection_fail_test(){
	BOOST_MESSAGE( "----- handle_sorryserver_connection_fail test end -----" );
	
	handle_sorryserver_connection_fail_test_class	test_obj;
	protocol_module_cinsert::t_session_thread_data_map& session_thread_data_map = test_obj.get_session_thread_data_map();
	
	// session_thread_data_map empty
	session_thread_data_map.clear();
	
	// unit_test [1] handle_sorryserver_connection_fail session_thread_data_map empty return STOP
	std::cout << "[1] handle_sorryserver_connection_fail session_thread_data_map empty return STOP" << std::endl;
	protocol_module_cinsert::EVENT_TAG status = test_obj.handle_realserver_connection_fail(boost::this_thread::get_id(),boost::asio::ip::tcp::endpoint());
	// check1 return STOP 
	BOOST_CHECK_EQUAL(status,protocol_module_cinsert::STOP);
	
	boost::thread test_thread_01(boost::bind(&handle_sorryserver_connection_fail_test_class::test_thread_01_run,&test_obj));
	boost::thread test_thread_02(boost::bind(&handle_sorryserver_connection_fail_test_class::test_thread_02_run,&test_obj));
	boost::thread test_thread_03(boost::bind(&handle_sorryserver_connection_fail_test_class::test_thread_03_run,&test_obj));
	boost::thread test_thread_04(boost::bind(&handle_sorryserver_connection_fail_test_class::test_thread_04_run,&test_obj));
	boost::thread test_thread_05(boost::bind(&handle_sorryserver_connection_fail_test_class::test_thread_05_run,&test_obj));
	boost::thread test_thread_06(boost::bind(&handle_sorryserver_connection_fail_test_class::test_thread_06_run,&test_obj));
	boost::thread test_thread_07(boost::bind(&handle_sorryserver_connection_fail_test_class::test_thread_07_run,&test_obj));
	boost::thread test_thread_08(boost::bind(&handle_sorryserver_connection_fail_test_class::test_thread_08_run,&test_obj));
	boost::thread test_thread_09(boost::bind(&handle_sorryserver_connection_fail_test_class::test_thread_09_run,&test_obj));
	boost::thread test_thread_10(boost::bind(&handle_sorryserver_connection_fail_test_class::test_thread_10_run,&test_obj));

	// sync_condition wait
	sleep(2);
	
	// set session_thread_data_map
	
	// thread 1
	// session_thread_data_map fond thread id
	// thread_data is NULL
	std::pair< boost::thread::id, protocol_module_cinsert::t_session_thread_data_cinsert > thread_1_data;
	boost::shared_ptr< protocol_module_cinsert::session_thread_data_cinsert > thread_1_read_data(new protocol_module_cinsert::session_thread_data_cinsert);
	thread_1_data.first = test_obj.thread_id_01;
	session_thread_data_map.insert(thread_1_data);
	
	// thread 2
	// session_thread_data_map fond thread id
	// thread_data is not NULL
	// thread_data->end_flag = END_FLAG_OFF
	std::pair< boost::thread::id, protocol_module_cinsert::t_session_thread_data_cinsert > thread_2_data;
	boost::shared_ptr< protocol_module_cinsert::session_thread_data_cinsert > thread_2_read_data(new protocol_module_cinsert::session_thread_data_cinsert);
	thread_2_data.first = test_obj.thread_id_02;
	thread_2_data.second = thread_2_read_data;
	thread_2_read_data->end_flag = protocol_module_cinsert::END_FLAG_OFF;
	session_thread_data_map.insert(thread_2_data);
	
	// thread 3
	// session_thread_data_map fond thread id
	// thread_data is NULL
	std::pair< boost::thread::id, protocol_module_cinsert::t_session_thread_data_cinsert > thread_3_data;
	boost::shared_ptr< protocol_module_cinsert::session_thread_data_cinsert > thread_3_read_data(new protocol_module_cinsert::session_thread_data_cinsert);
	thread_3_data.first = test_obj.thread_id_03;
	session_thread_data_map.insert(thread_3_data);
	
	// thread 4
	// session_thread_data_map fond thread id
	// thread_data is not NULL
	// thread_data->end_flag = END_FLAG_OFF
	std::pair< boost::thread::id, protocol_module_cinsert::t_session_thread_data_cinsert > thread_4_data;
	boost::shared_ptr< protocol_module_cinsert::session_thread_data_cinsert > thread_4_read_data(new protocol_module_cinsert::session_thread_data_cinsert);
	thread_4_data.first = test_obj.thread_id_04;
	thread_4_data.second = thread_4_read_data;
	thread_4_read_data->end_flag = protocol_module_cinsert::END_FLAG_OFF;
	session_thread_data_map.insert(thread_4_data);
	
	// thread 5
	// session_thread_data_map fond thread id
	// thread_data is NULL
	std::pair< boost::thread::id, protocol_module_cinsert::t_session_thread_data_cinsert > thread_5_data;
	boost::shared_ptr< protocol_module_cinsert::session_thread_data_cinsert > thread_5_read_data(new protocol_module_cinsert::session_thread_data_cinsert);
	thread_5_data.first = test_obj.thread_id_05;
	session_thread_data_map.insert(thread_5_data);
	
	// thread 6
	// session_thread_data_map fond thread id
	// thread_data is not NULL
	// thread_data->end_flag = END_FLAG_OFF
	std::pair< boost::thread::id, protocol_module_cinsert::t_session_thread_data_cinsert > thread_6_data;
	boost::shared_ptr< protocol_module_cinsert::session_thread_data_cinsert > thread_6_read_data(new protocol_module_cinsert::session_thread_data_cinsert);
	thread_6_data.first = test_obj.thread_id_06;
	thread_6_data.second = thread_6_read_data;
	thread_6_read_data->end_flag = protocol_module_cinsert::END_FLAG_OFF;
	session_thread_data_map.insert(thread_6_data);
	
	// thread 7
	// session_thread_data_map fond thread id
	// thread_data is NULL
	std::pair< boost::thread::id, protocol_module_cinsert::t_session_thread_data_cinsert > thread_7_data;
	boost::shared_ptr< protocol_module_cinsert::session_thread_data_cinsert > thread_7_read_data(new protocol_module_cinsert::session_thread_data_cinsert);
	thread_7_data.first = test_obj.thread_id_07;
	session_thread_data_map.insert(thread_7_data);
	
	// thread 8
	// session_thread_data_map fond thread id
	// thread_data is not NULL
	// thread_data->end_flag = END_FLAG_OFF
	std::pair< boost::thread::id, protocol_module_cinsert::t_session_thread_data_cinsert > thread_8_data;
	boost::shared_ptr< protocol_module_cinsert::session_thread_data_cinsert > thread_8_read_data(new protocol_module_cinsert::session_thread_data_cinsert);
	thread_8_data.first = test_obj.thread_id_08;
	thread_8_data.second = thread_8_read_data;
	thread_8_read_data->end_flag = protocol_module_cinsert::END_FLAG_OFF;
	session_thread_data_map.insert(thread_8_data);
	
	// thread 9
	// session_thread_data_map fond thread id
	// thread_data is NULL
	std::pair< boost::thread::id, protocol_module_cinsert::t_session_thread_data_cinsert > thread_9_data;
	boost::shared_ptr< protocol_module_cinsert::session_thread_data_cinsert > thread_9_read_data(new protocol_module_cinsert::session_thread_data_cinsert);
	thread_9_data.first = test_obj.thread_id_09;
	session_thread_data_map.insert(thread_9_data);
	
	// thread 10
	// session_thread_data_map fond thread id
	// thread_data is not NULL
	// thread_data->end_flag = END_FLAG_OFF
	std::pair< boost::thread::id, protocol_module_cinsert::t_session_thread_data_cinsert > thread_10_data;
	boost::shared_ptr< protocol_module_cinsert::session_thread_data_cinsert > thread_10_read_data(new protocol_module_cinsert::session_thread_data_cinsert);
	thread_10_data.first = test_obj.thread_id_10;
	thread_10_data.second = thread_10_read_data;
	thread_10_read_data->end_flag = protocol_module_cinsert::END_FLAG_OFF;
	session_thread_data_map.insert(thread_10_data);
	
	
	test_obj.sync_condition.notify_all();
	
//	std::cout << "sync_condition.notify_all" << std::endl;
	
	test_thread_01.join();
//	std::cout << "test_thread_01 stop" << std::endl;
	test_thread_02.join();
//	std::cout << "test_thread_02 stop" << std::endl;
	test_thread_03.join();
//	std::cout << "test_thread_03 stop" << std::endl;
	test_thread_04.join();
//	std::cout << "test_thread_04 stop" << std::endl;
	test_thread_05.join();
//	std::cout << "test_thread_05 stop" << std::endl;
	test_thread_06.join();
//	std::cout << "test_thread_06 stop" << std::endl;
	test_thread_07.join();
//	std::cout << "test_thread_07 stop" << std::endl;
	test_thread_08.join();
//	std::cout << "test_thread_08 stop" << std::endl;
	test_thread_09.join();
//	std::cout << "test_thread_09 stop" << std::endl;
	test_thread_10.join();
//	std::cout << "test_thread_10 stop" << std::endl;
 	
	std::cout << "test_thread_01 start time [" << test_obj.start_time_01.nsec << "]" << std::endl;
	std::cout << "test_thread_02 start time [" << test_obj.start_time_02.nsec << "]" << std::endl;
	std::cout << "test_thread_03 start time [" << test_obj.start_time_03.nsec << "]" << std::endl;
	std::cout << "test_thread_04 start time [" << test_obj.start_time_04.nsec << "]" << std::endl;
	std::cout << "test_thread_05 start time [" << test_obj.start_time_05.nsec << "]" << std::endl;
	std::cout << "test_thread_06 start time [" << test_obj.start_time_06.nsec << "]" << std::endl;
	std::cout << "test_thread_07 start time [" << test_obj.start_time_07.nsec << "]" << std::endl;
	std::cout << "test_thread_08 start time [" << test_obj.start_time_08.nsec << "]" << std::endl;
	std::cout << "test_thread_09 start time [" << test_obj.start_time_09.nsec << "]" << std::endl;
	std::cout << "test_thread_10 start time [" << test_obj.start_time_10.nsec << "]" << std::endl;
	
	std::cout << "test_thread_01 end time [" << test_obj.end_time_01.nsec << "]" << std::endl;
	std::cout << "test_thread_02 end time [" << test_obj.end_time_02.nsec << "]" << std::endl;
	std::cout << "test_thread_03 end time [" << test_obj.end_time_03.nsec << "]" << std::endl;
	std::cout << "test_thread_04 end time [" << test_obj.end_time_04.nsec << "]" << std::endl;
	std::cout << "test_thread_05 end time [" << test_obj.end_time_05.nsec << "]" << std::endl;
	std::cout << "test_thread_06 end time [" << test_obj.end_time_06.nsec << "]" << std::endl;
	std::cout << "test_thread_07 end time [" << test_obj.end_time_07.nsec << "]" << std::endl;
	std::cout << "test_thread_08 end time [" << test_obj.end_time_08.nsec << "]" << std::endl;
	std::cout << "test_thread_09 end time [" << test_obj.end_time_09.nsec << "]" << std::endl;
	std::cout << "test_thread_10 end time [" << test_obj.end_time_10.nsec << "]" << std::endl;
	
	// unit_test [2] handle_sorryserver_connection_fail thread_data is NULL return STOP
	std::cout << "[2] handle_sorryserver_connection_fail thread_data is NULL return STOP" << std::endl;
	
	// thread 1
	// check1 return STOP
	BOOST_CHECK_EQUAL(test_obj.status_01,protocol_module_cinsert::STOP);
	
	// thread 3
	// check1 return STOP
	BOOST_CHECK_EQUAL(test_obj.status_03,protocol_module_cinsert::STOP);
	
	// thread 5
	// check1 return STOP
	BOOST_CHECK_EQUAL(test_obj.status_05,protocol_module_cinsert::STOP);
	
	// thread 7
	// check1 return STOP
	BOOST_CHECK_EQUAL(test_obj.status_07,protocol_module_cinsert::STOP);
	
	// thread 9
	// check1 return STOP
	BOOST_CHECK_EQUAL(test_obj.status_09,protocol_module_cinsert::STOP);
	
	// unit_test [3] handle_sorryserver_connection_fail thread_data is not NULL return SORRYSERVER_DISCONNECT
	std::cout << "[3] handle_sorryserver_connection_fail thread_data is not NULL return SORRYSERVER_DISCONNECT" << std::endl;
	
	// thread 2
	// check1 thread_data->end_flag = END_FLAG_ON
	BOOST_CHECK( thread_2_read_data->end_flag == protocol_module_cinsert::END_FLAG_ON );
	// check2 return REALSERVER_DISCONNECT
	BOOST_CHECK_EQUAL(test_obj.status_02,protocol_module_cinsert::CLIENT_DISCONNECT);
		
	// thread 4
	// check1 thread_data->end_flag = END_FLAG_ON
	BOOST_CHECK( thread_4_read_data->end_flag == protocol_module_cinsert::END_FLAG_ON );
	// check2 return REALSERVER_DISCONNECT
	BOOST_CHECK_EQUAL(test_obj.status_04,protocol_module_cinsert::CLIENT_DISCONNECT);

	// thread 6
	// check1 thread_data->end_flag = END_FLAG_ON
	BOOST_CHECK( thread_6_read_data->end_flag == protocol_module_cinsert::END_FLAG_ON );
	// check2 return REALSERVER_DISCONNECT
	BOOST_CHECK_EQUAL(test_obj.status_06,protocol_module_cinsert::CLIENT_DISCONNECT);
	
	// thread 8
	// check1 thread_data->end_flag = END_FLAG_ON
	BOOST_CHECK( thread_8_read_data->end_flag == protocol_module_cinsert::END_FLAG_ON );
	// check2 return REALSERVER_DISCONNECT
	BOOST_CHECK_EQUAL(test_obj.status_08,protocol_module_cinsert::CLIENT_DISCONNECT);
	
	// thread 10
	// check1 thread_data->end_flag = END_FLAG_ON
	BOOST_CHECK( thread_10_read_data->end_flag == protocol_module_cinsert::END_FLAG_ON );
	// check2 return REALSERVER_DISCONNECT
	BOOST_CHECK_EQUAL(test_obj.status_10,protocol_module_cinsert::CLIENT_DISCONNECT);

	BOOST_MESSAGE( "----- handle_sorryserver_connection_fail test end -----" );
}


//-------------------------------------------------------------------
//-------------------------------------------------------------------
test_suite*	init_unit_test_suite( int argc, char* argv[] ){

	test_suite* ts = BOOST_TEST_SUITE( "protocol_module_cinsert class test" );

	ts->add( BOOST_TEST_CASE( &handle_sorry_enable_test ) );
	ts->add( BOOST_TEST_CASE( &handle_sorry_disable_test ) );
	ts->add( BOOST_TEST_CASE( &handle_realserver_connection_fail_test ) );
	ts->add( BOOST_TEST_CASE( &handle_sorryserver_connection_fail_test ) );
	
	framework::master_test_suite().add( ts );

	return 0;
}
