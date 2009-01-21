#define	TEST_CASE

#include <boost/test/included/unit_test.hpp>


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

//test_handler
void		test_handler(int sig);

int			call_count_test_handler = 0;	//test_handlerの呼出回数
int			arg_sig_test_handler = 0;		//test_handlerの引数(sig)

class	l7vsd_test	:public	l7vs::l7vsd {
public:
	vslist_type&	get_vslist(){ return vslist; }

};

//--tests--

void	list_virtual_service_test(){
	l7vsd_test	vsd_test;

	boost::shared_ptr< l7vs::virtualservice > vs1( new l7vs::virtualservice );
	boost::shared_ptr< l7vs::virtualservice > vs2( new l7vs::virtualservice );
	vs1->element.protocol_module_name = "cinsert";
	vs2->element.protocol_module_name = "url";

	vsd_test.get_vslist().push_back(vs1);
	vsd_test.get_vslist().push_back(vs2);

	//virtualservice_element elem1;
	//virtualservice_element elem2;

	l7vs::l7vsd::l7vsd_operation_result	res;
	l7vs::l7vsd::vsvec_type	vs_vector;

	res = vsd_test.list_virtual_service(vs_vector);
	// unit_test[1] list_virtual_service 正常系 戻り値確認
	BOOST_CHECK_EQUAL(res.flag, true);
	// unit_test[1] list_virtual_service 正常系 vs_vector確認
	BOOST_CHECK_EQUAL(vs_vector.size(), 2);
	BOOST_CHECK_EQUAL(vs_vector[0].protocol_module_name, "cinsert");
	BOOST_CHECK_EQUAL(vs_vector[1].protocol_module_name, "url");


	vsd_test.get_vslist().clear();
	vs_vector.clear();

	res = vsd_test.list_virtual_service(vs_vector);
	// unit_test[1] list_virtual_service 正常系２(vslistが空) 戻り値確認
	BOOST_CHECK_EQUAL(res.flag, true);
	// unit_test[1] list_virtual_service 正常系２(vslistが空) vs_vector確認
	BOOST_CHECK_EQUAL(vs_vector.size(), 0);


}


void	sig_exit_handler_test(){
	BOOST_MESSAGE( "----- sig_exit_handler test start -----" );

	//通常値で試験
	received_sig = 0;
	exit_requested = 0;
	//sig_exit_handler実行
	sig_exit_handler(15);
	// unit_test[1] sig_exit_handler 通常値 received_sig確認
	std::cout << "1----------------------------------------" << std::endl;
	BOOST_CHECK_EQUAL(received_sig, 15);
	// unit_test[1] sig_exit_handler 通常値 exit_requested確認
	std::cout << "2----------------------------------------" << std::endl;
	BOOST_CHECK_EQUAL(exit_requested, 1);

	//境界値で試験
	received_sig = 0;
	exit_requested = 0;
	//sig_exit_handler実行
	sig_exit_handler(INT_MAX);
	// unit_test[1] sig_exit_handler 境界値 received_sig確認
	std::cout << "3----------------------------------------" << std::endl;
	BOOST_CHECK_EQUAL(received_sig, INT_MAX);
	// unit_test[1] sig_exit_handler 境界値 exit_requested確認
	std::cout << "4----------------------------------------" << std::endl;
	BOOST_CHECK_EQUAL(exit_requested, 1);

	//限界値で試験
	received_sig = 0;
	exit_requested = 0;
	//sig_exit_handler実行
	sig_exit_handler(INT_MAX + 1);
	// unit_test[1] sig_exit_handler 限界値 received_sig確認
	std::cout << "5----------------------------------------" << std::endl;
	BOOST_CHECK_EQUAL(received_sig, INT_MAX + 1);
	// unit_test[1] sig_exit_handler 限界値 exit_requested確認
	std::cout << "6----------------------------------------" << std::endl;
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
	std::cout << "1----------------------------------------" << std::endl;
	BOOST_CHECK_EQUAL(ret, 0);

	//SIGUSR1の設定を再度取得する
	ret = sigaction(SIGUSR1, NULL, &act);
	// unit_test[1] set_sighander 正常系 handler確認
	std::cout << "2----------------------------------------" << std::endl;
	BOOST_CHECK_EQUAL(act.sa_handler, test_handler);
	//　unit_test[1] set_sighander 正常系 flags確認
	std::cout << "3----------------------------------------" << std::endl;
	BOOST_CHECK_EQUAL(act.sa_flags | ~SA_RESETHAND, ~SA_RESETHAND);


	//正常系２(handler NULL)
	//set_sighandler実行
	ret = set_sighandler(SIGUSR1, NULL);
	// unit_test[1] set_sighandler 正常系２(handler NULL) 返り値確認
	std::cout << "4----------------------------------------" << std::endl;
	BOOST_CHECK_EQUAL(ret, 0);
	//SIGUSR1の設定を再度取得する
	ret = sigaction(SIGUSR1, NULL, &act);
	// unit_test[1] set_sighandler 正常系２(handler NULL) handler確認
	std::cout << "5----------------------------------------" << std::endl;
	BOOST_CHECK_EQUAL(act.sa_handler, (void (*)(int))NULL);
	//　unit_test[1] set_sighander 正常系２(handler NULL) flags確認
	std::cout << "6----------------------------------------" << std::endl;
	BOOST_CHECK_EQUAL(act.sa_flags | ~SA_RESETHAND, ~SA_RESETHAND);


	//異常系(signal 0)
	//set_sighandler実行
	ret = set_sighandler(0, test_handler);
	// unit_test[1] set_sighandler 異常系(signal 0) 返り値確認
	std::cout << "7----------------------------------------" << std::endl;
	BOOST_CHECK_EQUAL(ret, -1);

	
	//異常系(signal 境界値)
	//set_sighandler実行
	ret = set_sighandler(INT_MAX, test_handler);
	// unit_test[1] set_sighandler 異常系(signal 境界値) 返り値確認
	std::cout << "8----------------------------------------" << std::endl;
	BOOST_CHECK_EQUAL(ret, -1);


	//異常系(signal 限界値)
	//set_sighandler実行
	ret = set_sighandler(INT_MAX + 1, test_handler);
	// unit_test[1] set_sighandler 異常系(signal 限界値) 返り値確認
	std::cout << "9----------------------------------------" << std::endl;
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
	std::cout << "1----------------------------------------" << std::endl;
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
	std::cout << "2----------------------------------------" << std::endl;
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

void	usage_test(){
	BOOST_MESSAGE( "----- usage test start -----" );

	//正常系
	// unit_test[1] usage 正常系 stdout出力確認
	std::cout << "1----------------------------------------" << std::endl;
	usage(stdout);
	// unit_test[1] usage 正常系 stderr出力確認
	std::cout << "2----------------------------------------" << std::endl;
	usage(stderr);

	//異常系
	// unit_test[1] usage 異常系 null出力確認
	std::cout << "3----------------------------------------" << std::endl;
	usage(NULL);

	BOOST_MESSAGE( "----- usage test end -----" );
}

void	test_handler(int sig){
	std::cout << "test_handler called" << std::endl;
	++call_count_test_handler;
	arg_sig_test_handler = sig;
	return;
}


test_suite*	init_unit_test_suite( int argc, char* argv[] ){

	test_suite* ts = BOOST_TEST_SUITE( "l7vsd class test" );

	l7vs::Logger logger_instance;
	l7vs::Parameter param;
	logger_instance.loadConf();
	
	logger_instance.putLogFatal( l7vs::LOG_CAT_L7VSD_NETWORK, 1, "Parameter initialize failure", __FILE__, __LINE__ );

	ts->add( BOOST_TEST_CASE( &list_virtual_service_test ) );

	ts->add( BOOST_TEST_CASE( &sig_exit_handler_test ) );
	ts->add( BOOST_TEST_CASE( &set_sighandler_test ) );
	ts->add( BOOST_TEST_CASE( &set_sighandlers_test ) );
	ts->add( BOOST_TEST_CASE( &usage_test ) );


	framework::master_test_suite().add( ts );

	return 0;
}
