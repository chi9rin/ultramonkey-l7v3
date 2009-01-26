#define	TEST_CASE

#include <boost/test/included/unit_test.hpp>
#include <boost/thread.hpp>

#include "error_code.h"
#include "logger_wrapper.h"

//#include "logger_enum.h"
//#include "logger_stub.h"
#include "logger.h"

//#include "parameter_enum.h"
//#include "parameter_stub.h"
#include "parameter.h"

#include "command_receiver_stub.h"
#include "virtualservice_stub.h"
//#include "virtualservice_element_stub.h"
#include "realserver_stub.h"
//#include "realserver_element_stub.h"
#include "replication_stub.h"
#include "snmpbridge_stub.h"
#include "protocol_module_control_stub.h"
#include "schedule_module_control_stub.h"

#include "../../src/l7vsd.cpp"

using namespace boost::unit_test_framework;

// test class
class	l7vsd_test	:public	l7vs::l7vsd {
public:
	vslist_type&	get_vslist(){ return vslist; }
	boost::thread_group&	get_tg(){ return vs_threads; }
	vslist_type::iterator	search_vslist( l7vs::virtualservice_element& in_elem ){
		return l7vsd::search_vslist( in_elem );
	}
	void	set_replication( boost::shared_ptr< l7vs::replication > inrep ){
		rep = inrep;
	}

};

//test_handler
void		test_handler(int sig);

int			call_count_test_handler = 0;	//test_handlerの呼出回数
int			arg_sig_test_handler = 0;		//test_handlerの引数(sig)


//util
template< typename InternetProtocol >
boost::asio::ip::basic_endpoint< InternetProtocol > string_to_endpoint( std::string str ){
	std::string::size_type pos = str.find( ":" );
	std::string	hostname = str.substr( 0, pos );
	std::string	portname = str.substr( pos+1, str.length() );
	boost::asio::io_service		io_service;
	typename InternetProtocol::resolver				resolver(io_service);
	typename InternetProtocol::resolver::query		query( hostname, portname );
	typename InternetProtocol::resolver::iterator	end;
	typename InternetProtocol::resolver::iterator	itr = resolver.resolve( query );
	if( itr == end ) return typename InternetProtocol::endpoint();
	return *itr;
}

//--tests--

void	list_virtual_service_test(){
	BOOST_MESSAGE( "----- list_virtual_service_test start -----" );

	l7vsd_test			vsd_test;

	boost::asio::io_service	io;
	l7vs::replication	rep(io);
	l7vs::virtualservice_element	e;

	boost::shared_ptr< l7vs::virtual_service > vs1( new l7vs::virtual_service( vsd_test, rep, e ) );
	boost::shared_ptr< l7vs::virtual_service > vs2( new l7vs::virtual_service( vsd_test, rep, e ) );
	vs1->element.protocol_module_name = "cinsert";
	vs2->element.protocol_module_name = "url";

	vsd_test.get_vslist().push_back(vs1);
	vsd_test.get_vslist().push_back(vs2);

	//virtualservice_element elem1;
	//virtualservice_element elem2;

	//正常系
	{
		l7vs::error_code	err;
		l7vs::l7vsd::vsvec_type	vs_vector;
	
		vsd_test.list_virtual_service( vs_vector, err );
		// unit_test[1] list_virtual_service 正常系 エラーコード確認
		BOOST_CHECK( !err );
		// unit_test[1] list_virtual_service 正常系 vs_vector数確認
		BOOST_CHECK_EQUAL( vs_vector.size(), 2U );
		// unit_test[1] list_virtual_service 正常系 vs_vector内容確認
		BOOST_CHECK_EQUAL( vs_vector[0].protocol_module_name, "cinsert" );
		BOOST_CHECK_EQUAL( vs_vector[1].protocol_module_name, "url" );
	}

	vsd_test.get_vslist().clear();

	//正常系２
	{
		l7vs::error_code	err;
		l7vs::l7vsd::vsvec_type	vs_vector;

		vsd_test.list_virtual_service( vs_vector, err );
		// unit_test[1] list_virtual_service 正常系２(vslistが空の場合) エラーコード確認
		BOOST_CHECK( !err );
		// unit_test[1] list_virtual_service 正常系２(vslistが空の場合) vs_vector数確認
		BOOST_CHECK_EQUAL( vs_vector.size(), 0U );
	}

	BOOST_MESSAGE( "----- list_virtual_service_test end -----" );

}

void	add_virtual_service_test(){
	BOOST_MESSAGE( "----- add_virtual_service_test start -----" );

	l7vsd_test			vsd_test;

	boost::asio::io_service			io;
	boost::shared_ptr< l7vs::replication >
									rep( new l7vs::replication(io) );
	vsd_test.set_replication( rep );

// 正常系
	{
		l7vs::virtualservice_element	elem;
		elem.tcp_accept_endpoint = string_to_endpoint<boost::asio::ip::tcp>( "10.10.10.10:9999" );

		l7vs::error_code err;
		vsd_test.add_virtual_service( elem, err );
		// unit_test[1] add_virtual_service 正常系 エラーコード確認
		BOOST_CHECK( !err );
		// unit_test[1] add_virtual_service 正常系 vslist数確認
		BOOST_CHECK_EQUAL( vsd_test.get_vslist().size(), 1U );
		// unit_test[1] add_virtual_service 正常系 vslist内容確認
		l7vs::l7vsd::vslist_type::iterator itr = vsd_test.get_vslist().begin();
		BOOST_CHECK( (*itr)->get_element() == elem );
		// unit_test[1] add_virtual_service 正常系 initialize確認
		BOOST_CHECK_EQUAL( (*itr)->initialize_called, true );
		// unit_test[1] add_virtual_service 正常系 set_virtualservice確認
		BOOST_CHECK_EQUAL( (*itr)->set_virtualservice_called, true );
		// unit_test[1] add_virtual_service 正常系 run確認
		vsd_test.get_tg().join_all();
		BOOST_CHECK_EQUAL( (*itr)->run_called, true );
		// unit_test[1] add_virtual_service 正常系 replication switch_to_master確認
		BOOST_CHECK_EQUAL( rep->switch_to_master_called, true );
	}

	rep->switch_to_master_called = false;

// 正常系２
	{
		l7vs::virtualservice_element	elem2;
		elem2.tcp_accept_endpoint = string_to_endpoint<boost::asio::ip::tcp>( "20.20.20.20:8888" );

		l7vs::error_code err;
		vsd_test.add_virtual_service( elem2, err );
		// unit_test[1] add_virtual_service 正常系２(vs2個め) エラーコード確認
		BOOST_CHECK( !err );
		// unit_test[1] add_virtual_service 正常系２(vs2個め) vslist数確認
		BOOST_CHECK_EQUAL( vsd_test.get_vslist().size(), 2U );
		// unit_test[1] add_virtual_service 正常系２(vs2個め) vslist内容確認
		l7vs::l7vsd::vslist_type::iterator itr = vsd_test.get_vslist().begin();
		++itr;
		BOOST_CHECK( (*itr)->get_element() == elem2 );
		// unit_test[1] add_virtual_service 正常系２(vs2個め) initialize確認
		BOOST_CHECK_EQUAL( (*itr)->initialize_called, true );
		// unit_test[1] add_virtual_service 正常系２(vs2個め) set_virtualservice確認
		BOOST_CHECK_EQUAL( (*itr)->set_virtualservice_called, true );
		// unit_test[1] add_virtual_service 正常系２(vs2個め) run確認
		vsd_test.get_tg().join_all();
		BOOST_CHECK_EQUAL( (*itr)->run_called, true );
		// unit_test[1] add_virtual_service 正常系２(vs2個め) replication switch_to_master確認
		BOOST_CHECK_EQUAL( rep->switch_to_master_called, false );
	}

//異常系
	// unit_test[1] add_virtual_service 異常系(既に同じvsがある場合) エラーコード確認
	{
		l7vs::virtualservice_element	elem3;
		elem3.tcp_accept_endpoint = string_to_endpoint<boost::asio::ip::tcp>( "20.20.20.20:8888" );

		l7vs::error_code err;
		vsd_test.add_virtual_service( elem3, err );
		BOOST_CHECK( err );
	}

	// unit_test[1] add_virtual_service 異常系２(replicaitonがnullの場合) 戻り値確認
	{
		l7vsd_test			vsd_test2;
		l7vs::virtualservice_element	elem;
		elem.tcp_accept_endpoint = string_to_endpoint<boost::asio::ip::tcp>( "10.10.10.10:9999" );

		l7vs::error_code err;
		vsd_test2.add_virtual_service( elem, err );
		BOOST_CHECK( err );
	}

	// unit_test[1] add_virtual_service 異常系３(initialize失敗) 戻り値確認
	{
		l7vs::virtualservice_element	elem4;
		elem4.tcp_accept_endpoint = string_to_endpoint<boost::asio::ip::tcp>( "30.30.30.30:7777" );
		l7vs::virtual_service::initialize_fail = true;

		l7vs::error_code err;
		vsd_test.add_virtual_service( elem4, err );
		BOOST_CHECK( err );

		l7vs::virtual_service::initialize_fail = false;
	}

	// unit_test[1] add_virtual_service 異常系４(set_virtualservice失敗) 戻り値確認
	{
		l7vs::virtualservice_element	elem5;
		elem5.tcp_accept_endpoint = string_to_endpoint<boost::asio::ip::tcp>( "40.40.40.40:6666" );
		l7vs::virtual_service::set_virtualservice_fail = true;

		l7vs::error_code err;
		vsd_test.add_virtual_service( elem5, err );
		BOOST_CHECK( err );

		l7vs::virtual_service::set_virtualservice_fail = false;
	}

	BOOST_MESSAGE( "----- add_virtual_service_test end -----" );
}

void	del_virtual_service_test(){
	BOOST_MESSAGE( "----- del_virtual_service_test start -----" );

	l7vsd_test			vsd_test;

	boost::asio::io_service			io;
	boost::shared_ptr< l7vs::replication >
									rep( new l7vs::replication(io) );
	vsd_test.set_replication( rep );

	l7vs::virtualservice_element	elem;
	elem.tcp_accept_endpoint = string_to_endpoint<boost::asio::ip::tcp>( "10.10.10.10:9999" );
	l7vs::virtualservice_element	elem2;
	elem2.tcp_accept_endpoint = string_to_endpoint<boost::asio::ip::tcp>( "20.20.20.20:8888" );

// prepare vslist
	{
		l7vs::error_code err;
		vsd_test.add_virtual_service( elem, err );
		vsd_test.add_virtual_service( elem2, err );
	}

// normal case
	{
		l7vs::virtual_service::stop_called = false;
		l7vs::virtual_service::finalize_called = false;
		l7vs::virtual_service::finalize_fail = false;

		l7vs::error_code err;
		vsd_test.del_virtual_service( elem, err );
		// unit_test[1] del_virtual_service normal case error_code check
		BOOST_CHECK( !err );
		// unit_test[1] del_virtual_service normal case vslist num check
		BOOST_CHECK_EQUAL( vsd_test.get_vslist().size(), 1U );
		// unit_test[1] del_virtual_service normal case stop call check
		BOOST_CHECK_EQUAL( l7vs::virtual_service::stop_called, true );
		// unit_test[1] del_virtual_service normal case finalize call check
		BOOST_CHECK_EQUAL( l7vs::virtual_service::finalize_called, true );
		// unit_test[1] del_virtual_service normal case replication switch_to_slave call check
		BOOST_CHECK_EQUAL( rep->switch_to_slave_called, false );

		l7vs::virtual_service::stop_called = false;
		l7vs::virtual_service::finalize_called = false;
		l7vs::virtual_service::finalize_fail = false;

		vsd_test.del_virtual_service( elem2, err );
		// unit_test[1] del_virtual_service normal case 2 (delete last vs) error_code check
		BOOST_CHECK( !err );
		// unit_test[1] del_virtual_service normal case 2 (delete last vs) vslist num check
		BOOST_CHECK_EQUAL( vsd_test.get_vslist().size(), 0U );
		// unit_test[1] del_virtual_service normal case 2 (delete last vs) stop call check
		BOOST_CHECK_EQUAL( l7vs::virtual_service::stop_called, true );
		// unit_test[1] del_virtual_service normal case 2 (delete last vs) finalize call check
		BOOST_CHECK_EQUAL( l7vs::virtual_service::finalize_called, true );
		// unit_test[1] del_virtual_service normal case 2 (delete last vs) replication switch_to_slave call check
		BOOST_CHECK_EQUAL( rep->switch_to_slave_called, true );

		l7vs::virtual_service::stop_called = false;
		l7vs::virtual_service::finalize_called = false;
		l7vs::virtual_service::finalize_fail = false;
	}

// error case 1
	{
		l7vs::error_code err;
		vsd_test.del_virtual_service( elem, err );

		// unit_test[1] del_virtual_service error_case 1(vslist empty) error_code check
		BOOST_CHECK( err );
		// unit_test[1] del_virtual_service error_case 1(vslist empty) vslist num check
		BOOST_CHECK_EQUAL( vsd_test.get_vslist().size(), 0U );

		l7vs::virtual_service::stop_called = false;
		l7vs::virtual_service::finalize_called = false;
		l7vs::virtual_service::finalize_fail = false;
	}

// vslist prepare
	{
		l7vs::error_code err;
		vsd_test.add_virtual_service( elem, err );
	}

// error case 2
	{
		l7vs::virtualservice_element	elem3;
		elem3.tcp_accept_endpoint = string_to_endpoint<boost::asio::ip::tcp>( "30.30.30.30:7777" );

		l7vs::error_code err;
		vsd_test.del_virtual_service( elem3, err );
		// unit_test[1] del_virtual_service error case 2(no target vs) error code check
		BOOST_CHECK( err );
		// unit_test[1] del_virtual_service error case 2(no target vs) vslist num check
		BOOST_CHECK_EQUAL( vsd_test.get_vslist().size(), 1U );

		l7vs::virtual_service::stop_called = false;
		l7vs::virtual_service::finalize_called = false;
		l7vs::virtual_service::finalize_fail = false;
	}

// error case 3
	{
		l7vs::virtual_service::finalize_fail = true;

		l7vs::error_code err;
		vsd_test.del_virtual_service( elem, err );
		// unit_test[1] del_virtual_service error case 3(error in finalize) error_code check
		BOOST_CHECK( err );
		// unit_test[1] del_virtual_service error case 3(error in finalize) vslist num check
		BOOST_CHECK_EQUAL( vsd_test.get_vslist().size(), 0U );

		l7vs::virtual_service::stop_called = false;
		l7vs::virtual_service::finalize_called = false;
		l7vs::virtual_service::finalize_fail = false;
	}

	BOOST_MESSAGE( "----- del_virtual_service_test end -----" );

}

void	edit_virtual_service_test(){
	BOOST_MESSAGE( "----- edit_virtual_service_test start -----" );
	l7vsd_test			vsd_test;

	boost::asio::io_service			io;
	boost::shared_ptr< l7vs::replication >
									rep( new l7vs::replication(io) );
	vsd_test.set_replication( rep );

	l7vs::virtualservice_element	elem;
	elem.tcp_accept_endpoint = string_to_endpoint<boost::asio::ip::tcp>( "10.10.10.10:9999" );

// prepare vslist
	{
		l7vs::error_code err;
		vsd_test.add_virtual_service( elem, err );
	}

// normal case
	{
		l7vs::error_code err;
		vsd_test.edit_virtual_service( elem, err );
		// unit_test[1] edit_virtual_service normal_case error_code check
		BOOST_CHECK( !err );
		// unit_test[1] edit_virtual_service normal_case edit_virtual_service call check
		l7vs::l7vsd::vslist_type::iterator itr = vsd_test.get_vslist().begin();
		BOOST_CHECK_EQUAL( (*itr)->edit_virtualservice_called, true );
	}
// error case
	{
		l7vs::virtual_service::edit_virtualservice_fail = true;

		l7vs::error_code err;
		vsd_test.edit_virtual_service( elem, err );
		// unit_test[1] edit_virtual_service error_case(vs::edit_virtualservice fail) error_code check
		BOOST_CHECK( err );
		l7vs::virtual_service::edit_virtualservice_fail = false;
	}

	{
		l7vs::virtualservice_element	elem2;
		elem2.tcp_accept_endpoint = string_to_endpoint<boost::asio::ip::tcp>( "20.20.20.20:8888" );

		l7vs::error_code err;
		vsd_test.edit_virtual_service( elem2, err );
		// unit_test[1] edit_virtual_service error_case(no target vs found) error_code check
		BOOST_CHECK( err );
	}

	BOOST_MESSAGE( "----- edit_virtual_service_test end -----" );
}

void	add_real_server_test(){
	BOOST_MESSAGE( "----- add_real_server_test start -----" );
	l7vsd_test			vsd_test;

	boost::asio::io_service			io;
	boost::shared_ptr< l7vs::replication >
									rep( new l7vs::replication(io) );
	vsd_test.set_replication( rep );

	l7vs::virtualservice_element	elem;
	elem.tcp_accept_endpoint = string_to_endpoint<boost::asio::ip::tcp>( "10.10.10.10:9999" );

// prepare vslist
	{
		l7vs::error_code err;
		vsd_test.add_virtual_service( elem, err );
	}

// normal case
	{
		l7vs::error_code err;
		vsd_test.add_real_server( elem, err );
		// unit_test[1] add_real_server normal_case error_code check
		BOOST_CHECK( !err );
		// unit_test[1] add_real_server normal_case add_realserver call check
		l7vs::l7vsd::vslist_type::iterator itr = vsd_test.get_vslist().begin();
		BOOST_CHECK_EQUAL( (*itr)->add_realserver_called, true );
	}
// error case
	{
		l7vs::virtual_service::add_realserver_fail = true;

		l7vs::error_code err;
		vsd_test.add_real_server( elem, err );
		// unit_test[1] add_real_server error_case(vs::add_realserver fail) error_code check
		BOOST_CHECK( err );
		l7vs::virtual_service::add_realserver_fail = false;
	}

	{
		l7vs::virtualservice_element	elem2;
		elem2.tcp_accept_endpoint = string_to_endpoint<boost::asio::ip::tcp>( "20.20.20.20:8888" );

		l7vs::error_code err;
		vsd_test.add_real_server( elem2, err );
		// unit_test[1] add_real_server error_case(no target vs found) error_code check
		BOOST_CHECK( err );
	}

	BOOST_MESSAGE( "----- add_real_server_test end -----" );
}

void	del_real_server_test(){
	BOOST_MESSAGE( "----- del_real_server_test start -----" );
	l7vsd_test			vsd_test;

	boost::asio::io_service			io;
	boost::shared_ptr< l7vs::replication >
									rep( new l7vs::replication(io) );
	vsd_test.set_replication( rep );

	l7vs::virtualservice_element	elem;
	elem.tcp_accept_endpoint = string_to_endpoint<boost::asio::ip::tcp>( "10.10.10.10:9999" );

// prepare vslist
	{
		l7vs::error_code err;
		vsd_test.add_virtual_service( elem, err );
	}

// normal case
	{
		l7vs::error_code err;
		vsd_test.del_real_server( elem, err );
		// unit_test[1] del_real_server normal_case error_code check
		BOOST_CHECK( !err );
		// unit_test[1] del_real_server normal_case del_realserver call check
		l7vs::l7vsd::vslist_type::iterator itr = vsd_test.get_vslist().begin();
		BOOST_CHECK_EQUAL( (*itr)->del_realserver_called, true );
	}
// error case
	{
		l7vs::virtual_service::del_realserver_fail = true;

		l7vs::error_code err;
		vsd_test.del_real_server( elem, err );
		// unit_test[1] del_real_server error_case(vs::del_realserver fail) error_code check
		BOOST_CHECK( err );
		l7vs::virtual_service::del_realserver_fail = false;
	}

	{
		l7vs::virtualservice_element	elem2;
		elem2.tcp_accept_endpoint = string_to_endpoint<boost::asio::ip::tcp>( "20.20.20.20:8888" );

		l7vs::error_code err;
		vsd_test.del_real_server( elem2, err );
		// unit_test[1] del_real_server error_case(no target vs found) error_code check
		BOOST_CHECK( err );
	}

	BOOST_MESSAGE( "----- del_real_server_test end -----" );
}

void	edit_real_server_test(){
	BOOST_MESSAGE( "----- edit_real_server_test start -----" );
	l7vsd_test			vsd_test;

	boost::asio::io_service			io;
	boost::shared_ptr< l7vs::replication >
									rep( new l7vs::replication(io) );
	vsd_test.set_replication( rep );

	l7vs::virtualservice_element	elem;
	elem.tcp_accept_endpoint = string_to_endpoint<boost::asio::ip::tcp>( "10.10.10.10:9999" );

// prepare vslist
	{
		l7vs::error_code err;
		vsd_test.add_virtual_service( elem, err );
	}

// normal case
	{
		l7vs::error_code err;
		vsd_test.edit_real_server( elem, err );
		// unit_test[1] edit_real_server normal_case error_code check
		BOOST_CHECK( !err );
		// unit_test[1] edit_real_server normal_case edit_realserver call check
		l7vs::l7vsd::vslist_type::iterator itr = vsd_test.get_vslist().begin();
		BOOST_CHECK_EQUAL( (*itr)->edit_realserver_called, true );
	}
// error case
	{
		l7vs::virtual_service::edit_realserver_fail = true;

		l7vs::error_code err;
		vsd_test.edit_real_server( elem, err );
		// unit_test[1] edit_real_server error_case(vs::edit_realserver fail) error_code check
		BOOST_CHECK( err );
		l7vs::virtual_service::edit_realserver_fail = false;
	}

	{
		l7vs::virtualservice_element	elem2;
		elem2.tcp_accept_endpoint = string_to_endpoint<boost::asio::ip::tcp>( "20.20.20.20:8888" );

		l7vs::error_code err;
		vsd_test.edit_real_server( elem2, err );
		// unit_test[1] edit_real_server error_case(no target vs found) error_code check
		BOOST_CHECK( err );
	}

	BOOST_MESSAGE( "----- edit_real_server_test end -----" );
}

void	flush_virtual_service_test(){
	BOOST_MESSAGE( "----- flush_virtual_service_test start -----" );

	l7vsd_test			vsd_test;

	boost::asio::io_service			io;
	boost::shared_ptr< l7vs::replication >
									rep( new l7vs::replication(io) );
	vsd_test.set_replication( rep );

	l7vs::virtualservice_element	elem;
	elem.tcp_accept_endpoint = string_to_endpoint<boost::asio::ip::tcp>( "10.10.10.10:9999" );
	l7vs::virtualservice_element	elem2;
	elem2.tcp_accept_endpoint = string_to_endpoint<boost::asio::ip::tcp>( "20.20.20.20:8888" );

// prepare vslist
	{
		l7vs::error_code err;
		vsd_test.add_virtual_service( elem, err );
		vsd_test.add_virtual_service( elem2, err );
	}

// normal case
	{
		l7vs::virtual_service::stop_called = false;
		l7vs::virtual_service::finalize_called = false;
		l7vs::virtual_service::finalize_fail = false;

		l7vs::error_code err;
		vsd_test.flush_virtual_service( err );
		// unit_test[1] flush_virtual_service normal case error_code check
		BOOST_CHECK( !err );
		// unit_test[1] flush_virtual_service normal case vslist num check
		BOOST_CHECK_EQUAL( vsd_test.get_vslist().size(), 0U );
		// unit_test[1] flush_virtual_service normal case stop call check
		BOOST_CHECK_EQUAL( l7vs::virtual_service::stop_called, true );
		// unit_test[1] flush_virtual_service normal case finalize call check
		BOOST_CHECK_EQUAL( l7vs::virtual_service::finalize_called, true );
		// unit_test[1] flush_virtual_service normal case replication switch_to_slave call check
		BOOST_CHECK_EQUAL( rep->switch_to_slave_called, true );

		l7vs::virtual_service::stop_called = false;
		l7vs::virtual_service::finalize_called = false;
		l7vs::virtual_service::finalize_fail = false;
		rep->switch_to_slave_called = false;

		vsd_test.flush_virtual_service( err );
		// unit_test[1] flush_virtual_service normal case 2(vslist empty) error_code check
		BOOST_CHECK( !err );
		// unit_test[1] flush_virtual_service normal case 2(vslist empty) vslist num check
		BOOST_CHECK_EQUAL( vsd_test.get_vslist().size(), 0U );
		// unit_test[1] flush_virtual_service normal case 2(vslist empty) stop call check
		BOOST_CHECK_EQUAL( l7vs::virtual_service::stop_called, false );
		// unit_test[1] flush_virtual_service normal case 2(vslist empty) finalize call check
		BOOST_CHECK_EQUAL( l7vs::virtual_service::finalize_called, false );
		// unit_test[1] flush_virtual_service normal case 2(vslist empty) replication switch_to_slave call check
		BOOST_CHECK_EQUAL( rep->switch_to_slave_called, true );

		l7vs::virtual_service::stop_called = false;
		l7vs::virtual_service::finalize_called = false;
		l7vs::virtual_service::finalize_fail = false;

		rep->switch_to_slave_called = false;
	}

// prepare vslist
	{
		l7vs::error_code err;
		vsd_test.add_virtual_service( elem, err );
	}

// error case 1
	{
		l7vs::virtual_service::finalize_fail = true;

		l7vs::error_code err;
		vsd_test.flush_virtual_service( err );

		// unit_test[1] flush_virtual_service error case 1(error in finalize) error_code check
		BOOST_CHECK( err );
		// unit_test[1] flush_virtual_service error case 1(error in finalize) vslist num check
		BOOST_CHECK_EQUAL( vsd_test.get_vslist().size(), 0U );
		// unit_test[1] flush_virtual_service error case 1(error in finalize) stop call check
		BOOST_CHECK_EQUAL( l7vs::virtual_service::stop_called, true );
		// unit_test[1] flush_virtual_service error case 1(error in finalize) finalize call check
		BOOST_CHECK_EQUAL( l7vs::virtual_service::finalize_called, true );
		// unit_test[1] flush_virtual_service error case 1(error in finalize) replication switch_to_slave call check
		BOOST_CHECK_EQUAL( rep->switch_to_slave_called, true );

		l7vs::virtual_service::stop_called = false;
		l7vs::virtual_service::finalize_called = false;
		l7vs::virtual_service::finalize_fail = false;
	}

	BOOST_MESSAGE( "----- flush_virtual_service_test end -----" );

}



void	search_vslist_test(){
	BOOST_MESSAGE( "----- search_vslist_test start -----" );
	l7vsd_test			vsd_test;

	boost::asio::io_service	io;
	l7vs::replication	rep( io );
	l7vs::virtualservice_element	e;

	boost::shared_ptr< l7vs::virtual_service > vs1( new l7vs::virtual_service( vsd_test, rep, e ) );
	boost::shared_ptr< l7vs::virtual_service > vs2( new l7vs::virtual_service( vsd_test, rep, e ) );

	vs1->element.tcp_accept_endpoint = string_to_endpoint<boost::asio::ip::tcp>( "10.10.10.10:9999" );
	vs2->element.tcp_accept_endpoint = string_to_endpoint<boost::asio::ip::tcp>( "20.20.20.20:8888" );

	vsd_test.get_vslist().push_back(vs1);
	vsd_test.get_vslist().push_back(vs2);

	// unit_test[1] search_vslist normal case return value check
	{
		l7vs::virtualservice_element	elem;
		elem.tcp_accept_endpoint = string_to_endpoint<boost::asio::ip::tcp>( "20.20.20.20:8888" );
		l7vs::l7vsd::vslist_type::iterator itr = vsd_test.search_vslist( elem );
		BOOST_CHECK( (*itr)->get_element() == elem );
	}

	// unit_test[1] search_vslist normal case 2(vs not found) return value check
	{
		l7vs::virtualservice_element	elem;
		elem.tcp_accept_endpoint = string_to_endpoint<boost::asio::ip::tcp>( "30.30.30.30:7777" );
		l7vs::l7vsd::vslist_type::iterator itr = vsd_test.search_vslist( elem );
		BOOST_CHECK( itr == vsd_test.get_vslist().end() );
	}

	// unit_test[1] search_vslist normal case 3(vslist empty) return value check
	vsd_test.get_vslist().clear();
	{
		l7vs::virtualservice_element	elem;
		elem.tcp_accept_endpoint = string_to_endpoint<boost::asio::ip::tcp>( "20.20.20.20:8888" );
		l7vs::l7vsd::vslist_type::iterator itr = vsd_test.search_vslist( elem );
		BOOST_CHECK( itr == vsd_test.get_vslist().end() );
	}

	//udpmode
	boost::shared_ptr< l7vs::virtual_service > vs3( new l7vs::virtual_service( vsd_test, rep, e ) );
	boost::shared_ptr< l7vs::virtual_service > vs4( new l7vs::virtual_service( vsd_test, rep, e ) );

	vs3->element.udpmode = true;
	vs4->element.udpmode = true;
	vs3->element.udp_recv_endpoint = string_to_endpoint<boost::asio::ip::udp>( "40.40.40.40:6666" );
	vs4->element.udp_recv_endpoint = string_to_endpoint<boost::asio::ip::udp>( "50.50.50.50:5555" );

	vsd_test.get_vslist().push_back(vs3);
	vsd_test.get_vslist().push_back(vs4);
	// unit_test[1] search_vslist normal case 4(udpmode) return value check
	{
		l7vs::virtualservice_element	elem;
		elem.udpmode = true;
		elem.udp_recv_endpoint = string_to_endpoint<boost::asio::ip::udp>( "40.40.40.40:6666" );
		l7vs::l7vsd::vslist_type::iterator itr = vsd_test.search_vslist( elem );
		BOOST_CHECK( (*itr)->get_element() == elem );
	}

	// unit_test[1] search_vslist normal case 5(vs not found in udpmode) return value check
	{
		l7vs::virtualservice_element	elem;
		elem.udpmode = true;
		elem.udp_recv_endpoint = string_to_endpoint<boost::asio::ip::udp>( "60.60.60.60:4444" );
		l7vs::l7vsd::vslist_type::iterator itr = vsd_test.search_vslist( elem );
		BOOST_CHECK( itr == vsd_test.get_vslist().end() );
	}

	// unit_test[1] search_vslist normal case 6(vslist empty in udpmode) return value check
	vsd_test.get_vslist().clear();
	{
		l7vs::virtualservice_element	elem;
		elem.udpmode = true;
		elem.udp_recv_endpoint = string_to_endpoint<boost::asio::ip::udp>( "40.40.40.40:6666" );
		l7vs::l7vsd::vslist_type::iterator itr = vsd_test.search_vslist( elem );
		BOOST_CHECK( itr == vsd_test.get_vslist().end() );
	}

	BOOST_MESSAGE( "----- search_vslist_test end -----" );

}

void	sig_exit_handler_test(){
	BOOST_MESSAGE( "----- sig_exit_handler test start -----" );

	//通常値で試験
	received_sig = 0;
	exit_requested = 0;
	//sig_exit_handler実行
	sig_exit_handler(15);
	// unit_test[1] sig_exit_handler 通常値 received_sig確認
	BOOST_CHECK_EQUAL(received_sig, 15);
	// unit_test[1] sig_exit_handler 通常値 exit_requested確認
	BOOST_CHECK_EQUAL(exit_requested, 1);

	//境界値で試験
	received_sig = 0;
	exit_requested = 0;
	//sig_exit_handler実行
	sig_exit_handler(INT_MAX);
	// unit_test[1] sig_exit_handler 境界値 received_sig確認
	BOOST_CHECK_EQUAL(received_sig, INT_MAX);
	// unit_test[1] sig_exit_handler 境界値 exit_requested確認
	BOOST_CHECK_EQUAL(exit_requested, 1);

	//限界値で試験
	received_sig = 0;
	exit_requested = 0;
	//sig_exit_handler実行
	sig_exit_handler(INT_MAX + 1);
	// unit_test[1] sig_exit_handler 限界値 received_sig確認
	BOOST_CHECK_EQUAL(received_sig, INT_MAX + 1);
	// unit_test[1] sig_exit_handler 限界値 exit_requested確認
	BOOST_CHECK_EQUAL(exit_requested, 1);

	BOOST_MESSAGE( "----- sig_exit_handler test end -----" );
}

void	set_sighandler_test(){
	BOOST_MESSAGE( "----- set_sighandler test start -----" );

	int ret = 0;
	struct sigaction oldact, act;

	//SIGUSR1の設定を保存する
	ret = sigaction(SIGUSR1, NULL, &oldact);

	//正常系
	//set_sighandler実行
	ret = set_sighandler(SIGUSR1, test_handler);
	// unit_test[1] set_sighandler 正常系 返り値確認
	BOOST_CHECK_EQUAL(ret, 0);

	//SIGUSR1の設定を再度取得する
	ret = sigaction(SIGUSR1, NULL, &act);
	// unit_test[1] set_sighander 正常系 handler確認
	BOOST_CHECK_EQUAL(act.sa_handler, test_handler);
	//　unit_test[1] set_sighander 正常系 flags確認
	BOOST_CHECK_EQUAL(act.sa_flags | ~SA_RESETHAND, ~SA_RESETHAND);


	//正常系２(handler NULL)
	//set_sighandler実行
	ret = set_sighandler(SIGUSR1, NULL);
	// unit_test[1] set_sighandler 正常系２(handler NULL) 返り値確認
	BOOST_CHECK_EQUAL(ret, 0);
	//SIGUSR1の設定を再度取得する
	ret = sigaction(SIGUSR1, NULL, &act);
	// unit_test[1] set_sighandler 正常系２(handler NULL) handler確認
	BOOST_CHECK_EQUAL(act.sa_handler, (void (*)(int))NULL);
	//　unit_test[1] set_sighander 正常系２(handler NULL) flags確認
	BOOST_CHECK_EQUAL(act.sa_flags | ~SA_RESETHAND, ~SA_RESETHAND);


	//異常系(signal 0)
	//set_sighandler実行
	ret = set_sighandler(0, test_handler);
	// unit_test[1] set_sighandler 異常系(signal 0) 返り値確認
	BOOST_CHECK_EQUAL(ret, -1);

	
	//異常系(signal 境界値)
	//set_sighandler実行
	ret = set_sighandler(INT_MAX, test_handler);
	// unit_test[1] set_sighandler 異常系(signal 境界値) 返り値確認
	BOOST_CHECK_EQUAL(ret, -1);


	//異常系(signal 限界値)
	//set_sighandler実行
	ret = set_sighandler(INT_MAX + 1, test_handler);
	// unit_test[1] set_sighandler 異常系(signal 限界値) 返り値確認
	BOOST_CHECK_EQUAL(ret, -1);


	//SIGUSR1の設定を元に戻す
	ret = sigaction(SIGUSR1, &oldact, NULL);

	BOOST_MESSAGE( "----- set_sighandler test end -----" );
}

void	set_sighandlers_test(){
	BOOST_MESSAGE( "----- set_sighandlers test start -----" );

	int ret = 0;
	struct sigaction oldact[8], act[8];

	//signalの設定を保存する
	ret = sigaction(SIGHUP, NULL, &oldact[0]);
	ret = sigaction(SIGINT, NULL, &oldact[1]);
	ret = sigaction(SIGQUIT, NULL, &oldact[2]);
	ret = sigaction(SIGTERM, NULL, &oldact[3]);
	ret = sigaction(SIGUSR1, NULL, &oldact[4]);
	ret = sigaction(SIGUSR2, NULL, &oldact[5]);
	ret = sigaction(SIGALRM, NULL, &oldact[6]);
	ret = sigaction(SIGCHLD, NULL, &oldact[7]);

	//正常系
	//set_sighandlers実行
	ret = set_sighandlers();
	// unit_test[1] set_sighandlers 正常系 返り値確認
	BOOST_CHECK_EQUAL(ret, 0);
	//signalの設定を再度取得する
	ret = sigaction(SIGHUP, NULL, &act[0]);
	ret = sigaction(SIGINT, NULL, &act[1]);
	ret = sigaction(SIGQUIT, NULL, &act[2]);
	ret = sigaction(SIGTERM, NULL, &act[3]);
	ret = sigaction(SIGUSR1, NULL, &act[4]);
	ret = sigaction(SIGUSR2, NULL, &act[5]);
	ret = sigaction(SIGALRM, NULL, &act[6]);
	ret = sigaction(SIGCHLD, NULL, &act[7]);
	// unit_test[1] set_sighandlers 正常系 SIGHUP handler確認
	BOOST_CHECK_EQUAL(act[0].sa_handler, sig_exit_handler);
	// unit_test[1] set_sighandlers 正常系 SIGINT handler確認
	BOOST_CHECK_EQUAL(act[1].sa_handler, sig_exit_handler);
	// unit_test[1] set_sighandlers 正常系 SIGQUIT handler確認
	BOOST_CHECK_EQUAL(act[2].sa_handler, sig_exit_handler);
	// unit_test[1] set_sighandlers 正常系 SIGTERM handler確認
	BOOST_CHECK_EQUAL(act[3].sa_handler, sig_exit_handler);
	// unit_test[1] set_sighandlers 正常系 SIGUSR1 handler確認
	BOOST_CHECK_EQUAL(act[4].sa_handler, SIG_IGN);
	// unit_test[1] set_sighandlers 正常系 SIGUSR2 handler確認
	BOOST_CHECK_EQUAL(act[5].sa_handler, SIG_IGN);
	// unit_test[1] set_sighandlers 正常系 SIGALRM handler確認
	BOOST_CHECK_EQUAL(act[6].sa_handler, SIG_IGN);
	// unit_test[1] set_sighandlers 正常系 SIGCHLD handler確認
	BOOST_CHECK_EQUAL(act[7].sa_handler, SIG_IGN);

	//signalの設定を元に戻す
	ret = sigaction(SIGHUP, &oldact[0], NULL);
	ret = sigaction(SIGINT, &oldact[1], NULL);
	ret = sigaction(SIGQUIT, &oldact[2], NULL);
	ret = sigaction(SIGTERM, &oldact[3], NULL);
	ret = sigaction(SIGUSR1, &oldact[4], NULL);
	ret = sigaction(SIGUSR2, &oldact[5], NULL);
	ret = sigaction(SIGALRM, &oldact[6], NULL);
	ret = sigaction(SIGCHLD, &oldact[7], NULL);

	BOOST_MESSAGE( "----- set_sighandlers test end -----" );
}

// void	usage_test(){
// 	BOOST_MESSAGE( "----- usage test start -----" );
// 
// 	//正常系
// 	// unit_test[1] usage 正常系 stdout出力確認
// 	usage(stdout);
// 	// unit_test[1] usage 正常系 stderr出力確認
// 	usage(stderr);
// 
// 	//異常系
// 	// unit_test[1] usage 異常系 null出力確認
// 	usage(NULL);
// 
// 	BOOST_MESSAGE( "----- usage test end -----" );
// }

void	test_handler(int sig){
	std::cout << "test_handler called" << std::endl;
	++call_count_test_handler;
	arg_sig_test_handler = sig;
	return;
}


test_suite*	init_unit_test_suite( int argc, char* argv[] ){

	test_suite* ts = BOOST_TEST_SUITE( "l7vsd class test" );

	l7vs::Logger	logger_instance;
	l7vs::Parameter	parameter_instance;
	logger_instance.loadConf();

	ts->add( BOOST_TEST_CASE( &list_virtual_service_test ) );
	ts->add( BOOST_TEST_CASE( &search_vslist_test ) );
	ts->add( BOOST_TEST_CASE( &add_virtual_service_test ) );
	ts->add( BOOST_TEST_CASE( &del_virtual_service_test ) );
	ts->add( BOOST_TEST_CASE( &edit_virtual_service_test ) );
	ts->add( BOOST_TEST_CASE( &add_real_server_test ) );
	ts->add( BOOST_TEST_CASE( &del_real_server_test ) );
	ts->add( BOOST_TEST_CASE( &edit_real_server_test ) );
	ts->add( BOOST_TEST_CASE( &flush_virtual_service_test ) );



	ts->add( BOOST_TEST_CASE( &sig_exit_handler_test ) );
	ts->add( BOOST_TEST_CASE( &set_sighandler_test ) );
	ts->add( BOOST_TEST_CASE( &set_sighandlers_test ) );
	//ts->add( BOOST_TEST_CASE( &usage_test ) );

	framework::master_test_suite().add( ts );

	return 0;
}
