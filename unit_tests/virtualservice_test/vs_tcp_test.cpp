
#include <iostream>
#include <sstream>
#include <boost/test/included/unit_test.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include "stub.h"
#include "virtualservice.h"
#include "vs_tcp.h"

// #include "protocol_module_test1.h"
#include "schedule_module_test1.h"

using namespace boost::unit_test;

bool pmcontrol_error_flag;
bool smcontrol_error_flag;

bool pm_function_called;

//test case1  create,initialize,run,stop,finalize,destroy(normal case)
void	virtualservice_tcp_test1(){
	pmcontrol_error_flag	= false;
	smcontrol_error_flag	= false;

	pm_function_called		= false;

	session_pool_debugger::getInstance().set_exist_flag( false );

	l7vs::l7vsd					vsd;
	boost::asio::io_service		dispatcher;
	l7vs::replication			rep( dispatcher );
	l7vs::virtualservice_element	element;
	//set element value

	
	// unit_test[1]  object create
	BOOST_MESSAGE( "-------1" );
	l7vs::vs_tcp*	vs = new l7vs::vs_tcp( vsd, rep, element );

	BOOST_CHECK_EQUAL( &vsd, &(vs->get_vsd()) );
	BOOST_CHECK_EQUAL( &rep, &(vs->get_rep()) );
	BOOST_CHECK( element == vs->get_element() );
	l7vs::error_code	vs_err;
	// unit_test[2]  initialize method call
	BOOST_MESSAGE( "-------2" );
	BOOST_CHECK( NULL == vs->get_protocol_module() );
	BOOST_CHECK( NULL == vs->get_schedule_module() );

	vs->initialize( vs_err );
	BOOST_CHECK( vs_err == false );

	//protocol_module_controlのモジュールロードを呼んでいるか(モジュールロード正常終了)
	BOOST_CHECK( NULL != vs->get_protocol_module() );
	//schedule_module_controlのモジュールロードを呼んでいるか(モジュールロード正常終了)
	BOOST_CHECK( NULL != vs->get_schedule_module() );
	//session_poolを作成しているか(デフォルトサイズで作成)
	BOOST_CHECK( SESSION_POOL_NUM_DEFAULT == vs->get_pool_sessions().size() );

	// unit_test[3]  run method test
	BOOST_MESSAGE( "-------3" );
	boost::thread	vs_main( &l7vs::vs_tcp::run, vs );

	// unit_test[4]  stop method test
	BOOST_MESSAGE( "-------4" );
	vs->stop();

	vs_main.join();

	// unit_test[5]  finalize method call
	vs->finalize( vs_err );

	// unit_test[6]  object destroy
	BOOST_MESSAGE( "-------2" );
	delete vs;
}

//test case2
void	virtualservice_tcp_test2(){
	l7vs::error_code	vs_err;

	l7vs::l7vsd					vsd;
	boost::asio::io_service		dispatcher;
	l7vs::replication			rep( dispatcher );
	l7vs::virtualservice_element	element;
	//set element value



	l7vs::vs_tcp*	vs = new l7vs::vs_tcp( vsd, rep, element );
	// unit_test[]  initialize method call(poolsize from parameter file:value = 512)
	session_pool_debugger::getInstance().set_exist_flag( true );
	pmcontrol_error_flag	= false;
	smcontrol_error_flag	= false;
	vs->initialize( vs_err );
	BOOST_CHECK( SESSION_POOL_NUM_PARAM == vs->get_pool_sessions().size() );
	BOOST_CHECK( vs_err == false );
	delete vs;


	vs = new l7vs::vs_tcp( vsd, rep, element );
	// unit_test[]  initialize method call(procotol_module_control::module_load error case)
	pmcontrol_error_flag	= true;
	smcontrol_error_flag	= false;
	BOOST_CHECK( NULL == vs->get_protocol_module() );
	BOOST_CHECK( NULL == vs->get_schedule_module() );
	vs->initialize( vs_err );
	BOOST_CHECK( NULL == vs->get_protocol_module() );
	BOOST_CHECK( NULL == vs->get_schedule_module() );
	BOOST_CHECK( vs_err == true );
	BOOST_MESSAGE( vs_err.get_message() );
	BOOST_CHECK( PROTOMOD_LOAD_ERROR_MSG == vs_err.get_message() );

	delete vs;

	vs = new l7vs::vs_tcp( vsd, rep, element );
	// unit_test[]  initialize method call(procotol_module_control::module_load error case)
	pmcontrol_error_flag	= false;
	smcontrol_error_flag	= true;
	BOOST_CHECK( NULL == vs->get_protocol_module() );
	BOOST_CHECK( NULL == vs->get_schedule_module() );
	vs->initialize( vs_err );
	BOOST_CHECK( NULL != vs->get_protocol_module() );
	BOOST_CHECK( NULL == vs->get_schedule_module() );
	BOOST_CHECK( vs_err == true );
	BOOST_MESSAGE( vs_err.get_message() );
	BOOST_CHECK( SCHEDMOD_LOAD_ERROR_MSG == vs_err.get_message() );

	delete vs;
}

//test case3
void	virtualservice_tcp_test3(){

	//private method call
// 	call_handle_replication_interrupt();
// 	call_read_replicationdata();
// 	call_handle_accept();

	//public method call
}

void	session_pool_access_test(){
	//sessionがActiveになるときに、release_sessionされるケース
	//sessionがActiveになるときにfinalize
	//release_sessionされるときにfinalize
}

void	rslist_access_test(){
	//rs追加
	//add_rsとrs_list_lock
	//add_rsとrs_list_unlock
	//add_rsとconnection_active
	//add_rsとconnection_inactive
	//rs変更
	//edit_rsとrs_list_lock
	//edit_rsとrs_list_unlock
	//edit_rsとconnection_active
	//edit_rsとconnection_inactive
	//rs削除
	//del_rsとrs_list_lock
	//del_rsとrs_list_unlock
	//del_rsとconnection_active
	//del_rsとconnection_inactive
	//rs_list_lockを複数スレッドから同時に呼ぶ
	//rs_list_unlockを複数スレッドから同時に呼ぶ
	//rs_list_lockとrs_list_unlockを複数スレッドから同時に呼ぶ
	//connection_activeを複数スレッドから同時に呼ぶ
	//connection_inactiveを複数スレッドから同時に呼ぶ
	//connection_activeとconnection_inactiveを複数スレッドから同時に呼ぶ
	//rs_list_lockとconnection_activeを複数スレッドから同時に呼ぶ
	//rs_list_unlockとrs_list_unlockを複数スレッドから同時に呼ぶ
	//rs_list_lockとconnection_inactiveを複数スレッドから同時に呼ぶ
	//rs_list_unlockとconnection_activeを複数スレッドから同時に呼ぶ
}

void	element_access_test(){
	//get_qos_upを複数スレッドから同時に呼ぶ
	//get_qos_downを複数スレッドから同時に呼ぶ
	//get_qos_upとget_qos_downを複数スレッドから同時に呼ぶ
}

void	datasize_access_test(){
	//update_up_recvを複数スレッドから同時に呼ぶ
	//update_up_sendを複数スレッドから同時に呼ぶ
	//update_down_recvを複数スレッドから同時に呼ぶ
	//update_down_sendを複数スレッドから同時に呼ぶ
	//update_throughputとupdate_up_recvを複数スレッドから同時に呼ぶ
	//update_throughputとupdate_up_sendを複数スレッドから同時に呼ぶ
	//update_throughputとupdate_down_recvを複数スレッドから同時に呼ぶ
	//update_throughputとupdate_down_sendを複数スレッドから同時に呼ぶ
	//update_throughputとget_throughput_upを複数スレッドから同時に呼ぶ
	//update_throughputとget_throughput_downを複数スレッドから同時に呼ぶ
}

test_suite*	init_unit_test_suite( int argc, char* argv[] ){

	// create unit test suite
	test_suite* ts = BOOST_TEST_SUITE( "virtualservice_base_test" );

	// add test case to test suite
	ts->add( BOOST_TEST_CASE( &virtualservice_tcp_test1 ) );
	ts->add( BOOST_TEST_CASE( &virtualservice_tcp_test2 ) );
	ts->add( BOOST_TEST_CASE( &virtualservice_tcp_test3 ) );

	framework::master_test_suite().add( ts );

	return 0;
}




// protocol_module stub
namespace l7vs{

class	protocol_module_test1 : public protocol_module_base{
public:
	protocol_module_test1() : protocol_module_base( "PMtest1" ) {
		BOOST_MESSAGE( "PMtest1 create" );
	}
	~protocol_module_test1(){
		BOOST_MESSAGE( "PMtest1 destroy" );
	}

	void	initialize(
							rs_list_itr_func_type		inlist_begin,
							rs_list_itr_func_type		inlist_end,
							rs_list_itr_next_func_type	inlist_next,
							boost::function< void( void ) >	inlist_lock,
							boost::function< void( void ) >	inlist_unlock ){}

	void	finalize(){}

	bool	is_tcp(){ return true; }
	bool	is_udp(){ return false; }
	bool	is_use_sorry(){ return true; }
	check_message_result	check_parameter( const std::vector<std::string>& args ){
		protocol_module_base::check_message_result	result;
		result.flag = true;
		return result;
	}

	void	handle_rslist_update(){}

	check_message_result	set_parameter( const std::vector<std::string>& args ){
		protocol_module_base::check_message_result	result;
		result.flag = true;
		return result;
	}
	check_message_result	add_parameter( const std::vector<std::string>& args ){
		protocol_module_base::check_message_result	result;
		result.flag = true;
		return result;
	}

	void	register_schedule( tcp_schedule_func_type inschedule ){}

	void	register_replication_area_lock(
									boost::function< void( void ) > inlock_func ){}

	void	register_replication_area_unlock(
									boost::function< void( void ) > inunlock_func ){}

	void	replication_interrupt(){
		std::cout << "Protocol Module Replication." << std::endl;
	}

	//use in upstream_thread
	EVENT_TAG	handle_session_initialize(
									const boost::thread::id up_thread_id,
									const boost::thread::id down_thread_id,
									const boost::asio::ip::tcp::endpoint& client_endpoint_tcp,
									const boost::asio::ip::udp::endpoint& client_endpoint_udp ){return STOP;}

	EVENT_TAG	handle_session_finalize(
									const boost::thread::id up_thread_id,
									const boost::thread::id down_thread_id ){return STOP;}

	EVENT_TAG	handle_accept( const boost::thread::id thread_id ){return STOP;}

	EVENT_TAG	handle_client_recv(
									const boost::thread::id thread_id,
									const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,
									const size_t recvlen ){return STOP;}

	EVENT_TAG	handle_realserver_select(
									const boost::thread::id thread_id,
									boost::asio::ip::tcp::endpoint& rs_endpoint ){return STOP;}

	EVENT_TAG	handle_realserver_connect(
									const boost::thread::id thread_id,
									boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
									size_t& datalen ){return STOP;}
	
	EVENT_TAG	handle_realserver_connection_fail(
									const boost::thread::id thread_id,
									const boost::asio::ip::tcp::endpoint& rs_endpoint ){return STOP;}
	
	EVENT_TAG	handle_realserver_send( const boost::thread::id thread_id ){return STOP;}
	
	EVENT_TAG	handle_sorryserver_select(
									const boost::thread::id thread_id,
									boost::asio::ip::tcp::endpoint & sorry_endpoint ){return STOP;}

	EVENT_TAG	handle_sorryserver_connect(
									const boost::thread::id thread_id,
									boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
									size_t& datalen ){return STOP;}

	EVENT_TAG	handle_sorryserver_connection_fail(
									const boost::thread::id thread_id,
									const boost::asio::ip::tcp::endpoint& sorry_endpoint ){return STOP;}
	
	EVENT_TAG	handle_sorryserver_send( const boost::thread::id thread_id ){return STOP;}

	//use in downstream_thread
	EVENT_TAG	handle_realserver_recv(
									const boost::thread::id thread_id,
									const boost::asio::ip::tcp::endpoint & rs_endpoint,
									const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,
									const size_t recvlen ){return STOP;}
	
	EVENT_TAG	handle_sorryserver_recv(
									const boost::thread::id thread_id,
									const boost::asio::ip::tcp::endpoint & sorry_endpoint,
									const boost::array<char,MAX_BUFFER_SIZE>& readbuffer,
									const size_t recvlen ){return STOP;}
	
	EVENT_TAG	handle_response_send_inform( const boost::thread::id thread_id ){return STOP;}

	EVENT_TAG	handle_client_connection_check(
									const boost::thread::id thread_id,
									boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
									size_t& datalen ){return STOP;}

	EVENT_TAG	handle_client_send( const boost::thread::id thread_id ){return STOP;}

	//use in upstream/downstream thread
	EVENT_TAG	handle_client_disconnect( const boost::thread::id thread_id ){return STOP;}
	
	EVENT_TAG	handle_sorry_enable( const boost::thread::id upthread_id ){return STOP;}

	EVENT_TAG	handle_sorry_disable( const boost::thread::id upthread_id ){return STOP;}

	EVENT_TAG	handle_realserver_disconnect(
									const boost::thread::id thread_id,
									const boost::asio::ip::tcp::endpoint & rs_endpoint ){return STOP;}
	
	EVENT_TAG	handle_sorryserver_disconnect(
									const boost::thread::id thread_id,
									const boost::asio::ip::tcp::endpoint & sorry_endpoint ){return STOP;}

	//unuse method
	void	register_schedule( udp_schedule_func_type inschedule ){}
	EVENT_TAG	handle_realserver_select(
									const boost::thread::id thread_id,
									boost::asio::ip::udp::endpoint& rs_endpoint,
									boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
									size_t& datalen ){return STOP;}
	EVENT_TAG	handle_realserver_recv(
									const boost::thread::id thread_id,
									const boost::asio::ip::udp::endpoint & rs_endpoint,
									const boost::array<char,MAX_BUFFER_SIZE>& readbuffer,
									const size_t recvlen ){return STOP;}
	EVENT_TAG	handle_client_select(
									const boost::thread::id thread_id,
									boost::asio::ip::udp::endpoint& rs_endpoint,
									boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
									size_t& datalen ){return STOP;}
	EVENT_TAG	handle_realserver_close(
									const boost::thread::id thread_id,
									const boost::asio::ip::udp::endpoint & rs_endpoint ){return STOP;};
};

}



// protocol_module_control　STUB code
l7vs::protocol_module_control&	l7vs::protocol_module_control::getInstance(){
	static	protocol_module_control	instance;
	return	instance;
}
void	l7vs::protocol_module_control::initialize( const std::string& infile_path ){}
void	l7vs::protocol_module_control::finalize(){}
l7vs::protocol_module_base*	l7vs::protocol_module_control::load_module( const std::string& modulename ){
	l7vs::protocol_module_base* return_value = NULL;

	if( !pmcontrol_error_flag )
		return_value = new protocol_module_test1;

	return return_value;
}
void	l7vs::protocol_module_control::unload_module( protocol_module_base* module_ptr ){
	delete	module_ptr;
}

l7vs::schedule_module_control&	l7vs::schedule_module_control::getInstance(){
	static	schedule_module_control	instance;
	return	instance;
}
void	l7vs::schedule_module_control::initialize( const std::string& infile_path ){}
void	l7vs::schedule_module_control::finalize(){}
l7vs::schedule_module_base*	l7vs::schedule_module_control::load_module( const std::string& modulename ){
	l7vs::schedule_module_base* return_value = NULL;

	if( !smcontrol_error_flag )
		return_value = new schedule_module_test1;

	return return_value;
}
void	l7vs::schedule_module_control::unload_module( schedule_module_base* module_ptr ){
	delete module_ptr;
}

