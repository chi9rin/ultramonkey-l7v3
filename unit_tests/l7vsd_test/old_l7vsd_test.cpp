#include "../../include/l7vsd.h"
#include "../../src/l7vsd.cpp"
#include <iostream>
#include <boost/test/unit_test.hpp>
#include <dlfcn.h>

using namespace std;
using namespace boost::unit_test_framework;

int l7vs_snmpbridge_initialize() {return 1;}
int l7vs_replication_init() {return 1;}
void l7vs_replication_fini() {}

void l7vs_snmpbridge_reload_config() {}
void l7vs_replication_reset() {}
int l7vs_iomux_poll(timeval* t, int i) {return 1;}

//char l7vs_module_path[MAXPATHLEN];
char l7vs_module_path[1024];

extern "C" {

//glibcの関数置換え
static int		(*org_sigaction)(int signum, const struct sigaction *act, struct sigaction *oldact) = NULL;
bool			sigaction_hook = false;
int			sigaction_count = 0;

static int		(*org_daemon)(int nochdir, int noclose) = NULL;
bool			daemon_hook = false;
int			daemon_count = 0;
int			daemon_ret = 0;

//static void		(*org__exit)(int exitstatus) = NULL;
//bool			_exit_hook = false;
//int			_exit_count = 0;
}

//test_handler
void			test_handler(int sig);

//debug variable
int			call_count_iomux_init = 0;			//l7vs_iomux_initの呼出回数
int			ret_iomux_init = 0;				//l7vs_iomux_initの戻り値

int			call_count_config_init = 0;			//l7vs_config_initの呼出回数
int			ret_config_init = 0;				//l7vs_config_initの戻り値

int			call_count_module_init = 0;			//l7vs_module_initの呼出回数
char*			arg_modpath_module_init = 0;			//l7vs_module_initの引数(modpath)
int			ret_module_init = 0;				//l7vs_module_initの戻り値

int			call_count_lsock_init = 0;			//l7vs_lsock_initの呼出回数
int			ret_lsock_init = 0;				//l7vs_lsock_initの戻り値

int			call_count_conn_init = 0;			//l7vs_conn_initの呼出回数
int			ret_conn_init = 0;				//l7vs_conn_initの戻り値

int			call_count_iomux_poll = 0;			//l7vs_iomux_pollの呼出回数
struct timeval*		arg_timo_iomux_poll = 0;			//l7vs_iomux_pollの引数(timo)
int			ret_iomux_poll = 0;				//l7vs_iomux_pollの戻り値
int			errno_iomux_poll = 0;				//l7vs_iomux_pollで設定されるerrno

int			call_count_iomux_fini = 0;			//l7vs_iomux_finiの呼出回数
int			call_count_config_fini = 0;			//l7vs_config_finiの呼出回数
int			call_count_module_fini = 0;			//l7vs_module_finiの呼出回数
int			call_count_lsock_fini = 0;			//l7vs_lsock_finiの呼出回数

int			call_count_test_handler = 0;			//test_handlerの呼出回数
int			arg_sig_test_handler = 0;			//test_handlerの引数(sig)


//--tests--
void
sig_exit_handler_test()
{
	BOOST_MESSAGE( "----- sig_exit_handler test start -----" );


	//通常値で試験
	received_sig = 0;
	exit_requested = 0;
	//sig_exit_handler実行
	sig_exit_handler(15);
	//＊received_sigがセットされているか
	cout << "1----------------------------------------" << endl;
	BOOST_CHECK_EQUAL(received_sig, 15);
	//＊exit_requestedがインクリメントされているか
	cout << "2----------------------------------------" << endl;
	BOOST_CHECK_EQUAL(exit_requested, 1);

	//境界値で試験
	received_sig = 0;
	exit_requested = 0;
	//sig_exit_handler実行
	sig_exit_handler(INT_MAX);
	//＊received_sigがセットされているか
	cout << "3----------------------------------------" << endl;
	BOOST_CHECK_EQUAL(received_sig, INT_MAX);
	//＊exit_requestedがインクリメントされているか
	cout << "4----------------------------------------" << endl;
	BOOST_CHECK_EQUAL(exit_requested, 1);

	//限界値で試験
	received_sig = 0;
	exit_requested = 0;
	//sig_exit_handler実行
	sig_exit_handler(INT_MAX + 1);
	//＊received_sigがセットされているか
	cout << "5----------------------------------------" << endl;
	BOOST_CHECK_EQUAL(received_sig, INT_MAX + 1);
	//＊exit_requestedがインクリメントされているか
	cout << "6----------------------------------------" << endl;
	BOOST_CHECK_EQUAL(exit_requested, 1);


	BOOST_MESSAGE( "----- sig_exit_handler test end -----" );
}

void
set_sighandler_test()
{
	BOOST_MESSAGE( "----- set_sighandler test start -----" );

	int ret = 0;
	struct sigaction oldact, act;

	//SIGUSR1の設定を保存する
	ret = sigaction(SIGUSR1, NULL, &oldact);

	//正常系のテスト
	//set_sighandler実行
	ret = set_sighandler(SIGUSR1, test_handler);
	//＊set_sighandlerは成功するか
	cout << "1----------------------------------------" << endl;
	BOOST_CHECK_EQUAL(ret, 0);
	//SIGUSR1の設定を再度取得する
	ret = sigaction(SIGUSR1, NULL, &act);
	//＊handlerはセットされているか
	cout << "2----------------------------------------" << endl;
	BOOST_CHECK_EQUAL(act.sa_handler, test_handler);
	//＊flagsにSA_RESETHANDがセットされていないか
	cout << "3----------------------------------------" << endl;
	BOOST_CHECK_EQUAL(act.sa_flags | ~SA_RESETHAND, ~SA_RESETHAND);


	//引数handlerがNULL
	//set_sighandler実行
	ret = set_sighandler(SIGUSR1, NULL);
	//＊set_sighandlerは成功するか
	cout << "4----------------------------------------" << endl;
	BOOST_CHECK_EQUAL(ret, 0);
	//SIGUSR1の設定を再度取得する
	ret = sigaction(SIGUSR1, NULL, &act);
	//＊handlerはセットされているか
	cout << "5----------------------------------------" << endl;
	BOOST_CHECK_EQUAL(act.sa_handler, (void (*)(int))NULL);
	//＊flagsにSA_RESETHANDがセットされていないか
	cout << "6----------------------------------------" << endl;
	BOOST_CHECK_EQUAL(act.sa_flags | ~SA_RESETHAND, ~SA_RESETHAND);


	//引数sigが0
	//set_sighandler実行
	ret = set_sighandler(0, test_handler);
	//＊set_sighandlerは失敗するか
	cout << "7----------------------------------------" << endl;
	BOOST_CHECK_EQUAL(ret, -1);

	
	//引数sigがINT_MAX
	//set_sighandler実行
	ret = set_sighandler(INT_MAX, test_handler);
	//＊set_sighandlerは失敗するか
	cout << "8----------------------------------------" << endl;
	BOOST_CHECK_EQUAL(ret, -1);


	//引数sigがINT_MAX+1
	//set_sighandler実行
	ret = set_sighandler(INT_MAX + 1, test_handler);
	//＊set_sighandlerは失敗するか
	cout << "9----------------------------------------" << endl;
	BOOST_CHECK_EQUAL(ret, -1);


	//SIGUSR1の設定を元に戻す
	ret = sigaction(SIGUSR1, &oldact, NULL);

	BOOST_MESSAGE( "----- set_sighandler test end -----" );
}

void
set_sighandlers_test()
{
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

	//正常系のテスト
	//set_sighandlers実行
	ret = set_sighandlers();
	//＊set_sighandlerは成功するか
	cout << "1----------------------------------------" << endl;
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
	//＊handlerはセットされているか
	cout << "2----------------------------------------" << endl;
	BOOST_CHECK_EQUAL(act[0].sa_handler, sig_exit_handler);
	BOOST_CHECK_EQUAL(act[1].sa_handler, sig_exit_handler);
	BOOST_CHECK_EQUAL(act[2].sa_handler, sig_exit_handler);
	BOOST_CHECK_EQUAL(act[3].sa_handler, sig_exit_handler);
	BOOST_CHECK_EQUAL(act[4].sa_handler, SIG_IGN);
	BOOST_CHECK_EQUAL(act[5].sa_handler, SIG_IGN);
	BOOST_CHECK_EQUAL(act[6].sa_handler, SIG_IGN);
	BOOST_CHECK_EQUAL(act[7].sa_handler, SIG_IGN);


	//sigactionが失敗した場合
	sigaction_hook = true;
	sigaction_count = 0;
	//set_sighandler実行
	ret = set_sighandler(SIGUSR1, NULL);
	//＊set_sighandlerは失敗するか
	cout << "3----------------------------------------" << endl;
	BOOST_CHECK_EQUAL(ret, -1);
	sigaction_hook = false;


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

void
usage_test()
{
	BOOST_MESSAGE( "----- usage test start -----" );

	char ident[] = "l7vsd_test";

	//正常系
	//＊stdoutに出力
	cout << "1----------------------------------------" << endl;
	usage(stdout, ident);
	//＊stderrに出力
	cout << "2----------------------------------------" << endl;
	usage(stderr, ident);


	//異常系
	//＊NULLに出力(何も出力されない)
	cout << "3----------------------------------------" << endl;
	usage(NULL, ident);
	//＊stdoutに出力, ident=null(何も表示されない)
	cout << "4----------------------------------------" << endl;
	usage(stdout, NULL);

	BOOST_MESSAGE( "----- usage test end -----" );
}

void
main_test()
{
	BOOST_MESSAGE( "----- main test start -----" );

	pid_t pid;
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


	//正常系のテスト
	//normalモード
	cout << "正常系_normal---" << endl;
	pid = fork();
	if (0 > pid) {
		BOOST_ERROR("fork failed");
	} else if (0 == pid) {
		//子プロセスでは親プロセスにSIGTERMを送る
		pid_t ppid = getppid();
		sleep(2);
		kill(ppid, SIGTERM);
		exit(0);
	} else {
		//デバッグ変数の初期化を行う
		optind = 1;
		opterr = 1;
		exit_requested = 0;
		received_sig =0;
		int test_argc = 1;
		char *test_argv[] = {"l7vsd_test"};
		daemon_hook = true;
		daemon_count = 0;
		daemon_ret = 0;
		memset(l7vs_module_path, 0, sizeof(l7vs_module_path));
		ret_iomux_poll = 0;
		call_count_iomux_poll = 0;
		ret_iomux_init = 0;
		call_count_iomux_init = 0;
		call_count_iomux_fini = 0;
		ret_config_init = 0;
		call_count_config_init = 0;
		call_count_config_fini = 0;
		ret_module_init = 0;
		call_count_module_init = 0;
		call_count_module_fini = 0;
		ret_lsock_init = 0;
		call_count_lsock_init = 0;
		call_count_lsock_fini = 0;
		ret_conn_init = 1;
		call_count_conn_init = 0;
		//関数を実行する
		l7vsd_main(test_argc, test_argv);
		daemon_hook = false;
		//各デバッグ変数のチェック
		//＊l7vs_module_pathはセットされているか
		cout << "1----------------------------------------" << endl;
		BOOST_CHECK(!strncmp(l7vs_module_path, L7VS_MODULE_PATH, sizeof(L7VS_MODULE_PATH)));
		//＊l7vs_iomux_poll関数は1回以上呼ばれているか
		cout << "2----------------------------------------" << endl;
		BOOST_CHECK(call_count_iomux_poll >= 1);
		//＊l7vs_iomux_init関数は1回呼ばれているか
		cout << "3----------------------------------------" << endl;
		BOOST_CHECK_EQUAL(call_count_iomux_init, 1);
		//＊l7vs_iomux_fini関数は1回呼ばれているか
		cout << "4----------------------------------------" << endl;
		BOOST_CHECK_EQUAL(call_count_iomux_fini, 1);
		//＊l7vs_config_init関数は1回呼ばれているか
		cout << "5----------------------------------------" << endl;
		BOOST_CHECK_EQUAL(call_count_config_init, 1);
		//＊l7vs_config_fini関数は1回呼ばれているか
		cout << "6----------------------------------------" << endl;
		BOOST_CHECK_EQUAL(call_count_config_fini, 1);
		//＊l7vs_module_init関数は1回呼ばれているか
		cout << "7----------------------------------------" << endl;
		BOOST_CHECK_EQUAL(call_count_module_init, 1);
		//＊l7vs_module_fini関数は1回呼ばれているか
		cout << "8----------------------------------------" << endl;
		BOOST_CHECK_EQUAL(call_count_module_fini, 1);
		//＊l7vs_lsock_init関数は1回呼ばれているか
		cout << "9----------------------------------------" << endl;
		BOOST_CHECK_EQUAL(call_count_lsock_init, 1);
		//＊l7vs_lsock_fini関数は1回呼ばれているか
		cout << "10---------------------------------------" << endl;
		BOOST_CHECK_EQUAL(call_count_lsock_fini, 1);
		//＊l7vs_conn_init関数は1回呼ばれているか
		cout << "23---------------------------------------" << endl;
		BOOST_CHECK_EQUAL(call_count_conn_init, 1);
		//signalの設定を再度取得する
		ret = sigaction(SIGHUP, NULL, &act[0]);
		ret = sigaction(SIGINT, NULL, &act[1]);
		ret = sigaction(SIGQUIT, NULL, &act[2]);
		ret = sigaction(SIGTERM, NULL, &act[3]);
		ret = sigaction(SIGUSR1, NULL, &act[4]);
		ret = sigaction(SIGUSR2, NULL, &act[5]);
		ret = sigaction(SIGALRM, NULL, &act[6]);
		ret = sigaction(SIGCHLD, NULL, &act[7]);
		//＊handlerはセットされているか
		cout << "11---------------------------------------" << endl;
		BOOST_CHECK_EQUAL(act[0].sa_handler, sig_exit_handler);
		BOOST_CHECK_EQUAL(act[1].sa_handler, sig_exit_handler);
		BOOST_CHECK_EQUAL(act[2].sa_handler, sig_exit_handler);
		BOOST_CHECK_EQUAL(act[3].sa_handler, sig_exit_handler);
		BOOST_CHECK_EQUAL(act[4].sa_handler, SIG_IGN);
		BOOST_CHECK_EQUAL(act[5].sa_handler, SIG_IGN);
		BOOST_CHECK_EQUAL(act[6].sa_handler, SIG_IGN);
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
//		//loggerの設定を元に戻す
//		vanessa_logger_t *vl;
//		vl = vanessa_logger_openlog_filehandle(stderr, "l7vsd_test", LOG_DEBUG, 0);
//		vanessa_logger_set(vl);
	}


	//正常系のテスト
	//debugモード(short)
	cout << "正常系_debug_short---" << endl;
	pid = fork();
	if (0 > pid) {
		BOOST_ERROR("fork failed");
	} else if (0 == pid) {
		//子プロセスでは親プロセスにSIGTERMを送る
		pid_t ppid = getppid();
		sleep(2);
		kill(ppid, SIGTERM);
		exit(0);
	} else {
		//デバッグ変数の初期化を行う
		optind = 1;
		opterr = 1;
		exit_requested = 0;
		received_sig =0;
		int test_argc = 2;
		char *test_argv[] = {"l7vsd_test", "-d"};
		daemon_hook = true;
		daemon_count = 0;
		daemon_ret = 0;
		memset(l7vs_module_path, 0, sizeof(l7vs_module_path));
		ret_iomux_poll = 0;
		call_count_iomux_poll = 0;
		ret_iomux_init = 0;
		call_count_iomux_init = 0;
		call_count_iomux_fini = 0;
		ret_config_init = 0;
		call_count_config_init = 0;
		call_count_config_fini = 0;
		ret_module_init = 0;
		call_count_module_init = 0;
		call_count_module_fini = 0;
		ret_lsock_init = 0;
		call_count_lsock_init = 0;
		call_count_lsock_fini = 0;
		ret_conn_init = 0;
		call_count_conn_init = 0;
		//関数を実行する
		l7vsd_main(test_argc, test_argv);
		daemon_hook = false;
		//各デバッグ変数のチェック
		//＊l7vs_module_pathはセットされているか
		cout << "12---------------------------------------" << endl;
		BOOST_CHECK(!strncmp(l7vs_module_path, L7VS_MODULE_PATH, sizeof(L7VS_MODULE_PATH)));
		//＊l7vs_iomux_poll関数は1回以上呼ばれているか
		cout << "13---------------------------------------" << endl;
		BOOST_CHECK(call_count_iomux_poll >= 1);
		//＊l7vs_iomux_init関数は1回呼ばれているか
		cout << "14---------------------------------------" << endl;
		BOOST_CHECK_EQUAL(call_count_iomux_init, 1);
		//＊l7vs_iomux_fini関数は1回呼ばれているか
		cout << "15---------------------------------------" << endl;
		BOOST_CHECK_EQUAL(call_count_iomux_fini, 1);
		//＊l7vs_config_init関数は1回呼ばれているか
		cout << "16---------------------------------------" << endl;
		BOOST_CHECK_EQUAL(call_count_config_init, 1);
		//＊l7vs_config_fini関数は1回呼ばれているか
		cout << "17---------------------------------------" << endl;
		BOOST_CHECK_EQUAL(call_count_config_fini, 1);
		//＊l7vs_module_init関数は1回呼ばれているか
		cout << "18---------------------------------------" << endl;
		BOOST_CHECK_EQUAL(call_count_module_init, 1);
		//＊l7vs_module_fini関数は1回呼ばれているか
		cout << "19---------------------------------------" << endl;
		BOOST_CHECK_EQUAL(call_count_module_fini, 1);
		//＊l7vs_lsock_init関数は1回呼ばれているか
		cout << "20---------------------------------------" << endl;
		BOOST_CHECK_EQUAL(call_count_lsock_init, 1);
		//＊l7vs_lsock_fini関数は1回呼ばれているか
		cout << "21---------------------------------------" << endl;
		BOOST_CHECK_EQUAL(call_count_lsock_fini, 1);
		//＊l7vs_conn_init関数は1回呼ばれているか
		cout << "24---------------------------------------" << endl;
		BOOST_CHECK_EQUAL(call_count_conn_init, 1);
		//signalの設定を再度取得する
		ret = sigaction(SIGHUP, NULL, &act[0]);
		ret = sigaction(SIGINT, NULL, &act[1]);
		ret = sigaction(SIGQUIT, NULL, &act[2]);
		ret = sigaction(SIGTERM, NULL, &act[3]);
		ret = sigaction(SIGUSR1, NULL, &act[4]);
		ret = sigaction(SIGUSR2, NULL, &act[5]);
		ret = sigaction(SIGALRM, NULL, &act[6]);
		ret = sigaction(SIGCHLD, NULL, &act[7]);
		//＊handlerはセットされているか
		cout << "22---------------------------------------" << endl;
		BOOST_CHECK_EQUAL(act[0].sa_handler, sig_exit_handler);
		BOOST_CHECK_EQUAL(act[1].sa_handler, sig_exit_handler);
		BOOST_CHECK_EQUAL(act[2].sa_handler, sig_exit_handler);
		BOOST_CHECK_EQUAL(act[3].sa_handler, sig_exit_handler);
		BOOST_CHECK_EQUAL(act[4].sa_handler, SIG_IGN);
		BOOST_CHECK_EQUAL(act[5].sa_handler, SIG_IGN);
		BOOST_CHECK_EQUAL(act[6].sa_handler, SIG_IGN);
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
//		//loggerの設定を元に戻す
//		vanessa_logger_t *vl;
//		vl = vanessa_logger_openlog_filehandle(stderr, "l7vsd_test", LOG_DEBUG, 0);
//		vanessa_logger_set(vl);
	}


	//正常系のテスト
	//debugモード(long)
	cout << "正常系_debug_long---" << endl;
	pid = fork();
	if (0 > pid) {
		BOOST_ERROR("fork failed");
	} else if (0 == pid) {
		//子プロセスでは親プロセスにSIGTERMを送る
		pid_t ppid = getppid();
		sleep(2);
		kill(ppid, SIGTERM);
		exit(0);
	} else {
		//デバッグ変数の初期化を行う
		optind = 1;
		opterr = 1;
		exit_requested = 0;
		received_sig =0;
		int test_argc = 2;
		char *test_argv[] = {"l7vsd_test", "--debug"};
		daemon_hook = true;
		daemon_count = 0;
		daemon_ret = 0;
		memset(l7vs_module_path, 0, sizeof(l7vs_module_path));
		ret_iomux_poll = 0;
		call_count_iomux_poll = 0;
		ret_iomux_init = 0;
		call_count_iomux_init = 0;
		call_count_iomux_fini = 0;
		ret_config_init = 0;
		call_count_config_init = 0;
		call_count_config_fini = 0;
		ret_module_init = 0;
		call_count_module_init = 0;
		call_count_module_fini = 0;
		ret_lsock_init = 0;
		call_count_lsock_init = 0;
		call_count_lsock_fini = 0;
		ret_conn_init = 0;
		call_count_conn_init = 0;
		//関数を実行する
		l7vsd_main(test_argc, test_argv);
		daemon_hook = false;
		//各デバッグ変数のチェック
		//＊l7vs_module_pathはセットされているか
		cout << "12---------------------------------------" << endl;
		BOOST_CHECK(!strncmp(l7vs_module_path, L7VS_MODULE_PATH, sizeof(L7VS_MODULE_PATH)));
		//＊l7vs_iomux_poll関数は1回以上呼ばれているか
		cout << "13---------------------------------------" << endl;
		BOOST_CHECK(call_count_iomux_poll >= 1);
		//＊l7vs_iomux_init関数は1回呼ばれているか
		cout << "14---------------------------------------" << endl;
		BOOST_CHECK_EQUAL(call_count_iomux_init, 1);
		//＊l7vs_iomux_fini関数は1回呼ばれているか
		cout << "15---------------------------------------" << endl;
		BOOST_CHECK_EQUAL(call_count_iomux_fini, 1);
		//＊l7vs_config_init関数は1回呼ばれているか
		cout << "16---------------------------------------" << endl;
		BOOST_CHECK_EQUAL(call_count_config_init, 1);
		//＊l7vs_config_fini関数は1回呼ばれているか
		cout << "17---------------------------------------" << endl;
		BOOST_CHECK_EQUAL(call_count_config_fini, 1);
		//＊l7vs_module_init関数は1回呼ばれているか
		cout << "18---------------------------------------" << endl;
		BOOST_CHECK_EQUAL(call_count_module_init, 1);
		//＊l7vs_module_fini関数は1回呼ばれているか
		cout << "19---------------------------------------" << endl;
		BOOST_CHECK_EQUAL(call_count_module_fini, 1);
		//＊l7vs_lsock_init関数は1回呼ばれているか
		cout << "20---------------------------------------" << endl;
		BOOST_CHECK_EQUAL(call_count_lsock_init, 1);
		//＊l7vs_lsock_fini関数は1回呼ばれているか
		cout << "21---------------------------------------" << endl;
		BOOST_CHECK_EQUAL(call_count_lsock_fini, 1);
		//＊l7vs_conn_init関数は1回呼ばれているか
		cout << "46---------------------------------------" << endl;
		BOOST_CHECK_EQUAL(call_count_conn_init, 1);
		//signalの設定を再度取得する
		ret = sigaction(SIGHUP, NULL, &act[0]);
		ret = sigaction(SIGINT, NULL, &act[1]);
		ret = sigaction(SIGQUIT, NULL, &act[2]);
		ret = sigaction(SIGTERM, NULL, &act[3]);
		ret = sigaction(SIGUSR1, NULL, &act[4]);
		ret = sigaction(SIGUSR2, NULL, &act[5]);
		ret = sigaction(SIGALRM, NULL, &act[6]);
		ret = sigaction(SIGCHLD, NULL, &act[7]);
		//＊handlerはセットされているか
		cout << "22---------------------------------------" << endl;
		BOOST_CHECK_EQUAL(act[0].sa_handler, sig_exit_handler);
		BOOST_CHECK_EQUAL(act[1].sa_handler, sig_exit_handler);
		BOOST_CHECK_EQUAL(act[2].sa_handler, sig_exit_handler);
		BOOST_CHECK_EQUAL(act[3].sa_handler, sig_exit_handler);
		BOOST_CHECK_EQUAL(act[4].sa_handler, SIG_IGN);
		BOOST_CHECK_EQUAL(act[5].sa_handler, SIG_IGN);
		BOOST_CHECK_EQUAL(act[6].sa_handler, SIG_IGN);
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
//		//loggerの設定を元に戻す
//		vanessa_logger_t *vl;
//		vl = vanessa_logger_openlog_filehandle(stderr, "l7vsd_test", LOG_DEBUG, 0);
//		vanessa_logger_set(vl);
	}


	//異常系のテスト
	//iomux_pollが-1を返した時(errno=EINTR)
	cout << "異常系_iomux_poll=-1_EINTR---" << endl;
	pid = fork();
	if (0 > pid) {
		BOOST_ERROR("fork failed");
	} else if (0 == pid) {
		//子プロセスでは親プロセスにSIGTERMを送る
		pid_t ppid = getppid();
		sleep(2);
		kill(ppid, SIGTERM);
		exit(0);
	} else {
		//デバッグ変数の初期化を行う
		optind = 1;
		opterr = 1;
		exit_requested = 0;
		received_sig =0;
		int test_argc = 2;
		char *test_argv[] = {"l7vsd_test", "--debug"};
		daemon_hook = true;
		daemon_count = 0;
		daemon_ret = 0;
		memset(l7vs_module_path, 0, sizeof(l7vs_module_path));
		ret_iomux_poll = -1;
		errno_iomux_poll = EINTR;
		call_count_iomux_poll = 0;
		ret_iomux_init = 0;
		call_count_iomux_init = 0;
		call_count_iomux_fini = 0;
		ret_config_init = 0;
		call_count_config_init = 0;
		call_count_config_fini = 0;
		ret_module_init = 0;
		call_count_module_init = 0;
		call_count_module_fini = 0;
		ret_lsock_init = 0;
		call_count_lsock_init = 0;
		call_count_lsock_fini = 0;
		ret_conn_init = 0;
		call_count_conn_init = 0;
		//関数を実行する
		l7vsd_main(test_argc, test_argv);
		daemon_hook = false;
		//各デバッグ変数のチェック
		//＊l7vs_module_pathはセットされているか
		cout << "23---------------------------------------" << endl;
		BOOST_CHECK(!strncmp(l7vs_module_path, L7VS_MODULE_PATH, sizeof(L7VS_MODULE_PATH)));
		//＊l7vs_iomux_poll関数は1回以上呼ばれているか
		cout << "24---------------------------------------" << endl;
		BOOST_CHECK(call_count_iomux_poll >= 1);
		//＊l7vs_iomux_init関数は1回呼ばれているか
		cout << "25---------------------------------------" << endl;
		BOOST_CHECK_EQUAL(call_count_iomux_init, 1);
		//＊l7vs_iomux_fini関数は1回呼ばれているか
		cout << "26---------------------------------------" << endl;
		BOOST_CHECK_EQUAL(call_count_iomux_fini, 1);
		//＊l7vs_config_init関数は1回呼ばれているか
		cout << "27---------------------------------------" << endl;
		BOOST_CHECK_EQUAL(call_count_config_init, 1);
		//＊l7vs_config_fini関数は1回呼ばれているか
		cout << "28---------------------------------------" << endl;
		BOOST_CHECK_EQUAL(call_count_config_fini, 1);
		//＊l7vs_module_init関数は1回呼ばれているか
		cout << "29---------------------------------------" << endl;
		BOOST_CHECK_EQUAL(call_count_module_init, 1);
		//＊l7vs_module_fini関数は1回呼ばれているか
		cout << "30---------------------------------------" << endl;
		BOOST_CHECK_EQUAL(call_count_module_fini, 1);
		//＊l7vs_lsock_init関数は1回呼ばれているか
		cout << "31---------------------------------------" << endl;
		BOOST_CHECK_EQUAL(call_count_lsock_init, 1);
		//＊l7vs_lsock_fini関数は1回呼ばれているか
		cout << "32---------------------------------------" << endl;
		BOOST_CHECK_EQUAL(call_count_lsock_fini, 1);
		//＊l7vs_conn_init関数は1回呼ばれているか
		cout << "47---------------------------------------" << endl;
		BOOST_CHECK_EQUAL(call_count_conn_init, 1);
		//signalの設定を再度取得する
		ret = sigaction(SIGHUP, NULL, &act[0]);
		ret = sigaction(SIGINT, NULL, &act[1]);
		ret = sigaction(SIGQUIT, NULL, &act[2]);
		ret = sigaction(SIGTERM, NULL, &act[3]);
		ret = sigaction(SIGUSR1, NULL, &act[4]);
		ret = sigaction(SIGUSR2, NULL, &act[5]);
		ret = sigaction(SIGALRM, NULL, &act[6]);
		ret = sigaction(SIGCHLD, NULL, &act[7]);
		//＊handlerはセットされているか
		cout << "33---------------------------------------" << endl;
		BOOST_CHECK_EQUAL(act[0].sa_handler, sig_exit_handler);
		BOOST_CHECK_EQUAL(act[1].sa_handler, sig_exit_handler);
		BOOST_CHECK_EQUAL(act[2].sa_handler, sig_exit_handler);
		BOOST_CHECK_EQUAL(act[3].sa_handler, sig_exit_handler);
		BOOST_CHECK_EQUAL(act[4].sa_handler, SIG_IGN);
		BOOST_CHECK_EQUAL(act[5].sa_handler, SIG_IGN);
		BOOST_CHECK_EQUAL(act[6].sa_handler, SIG_IGN);
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
//		//loggerの設定を元に戻す
//		vanessa_logger_t *vl;
//		vl = vanessa_logger_openlog_filehandle(stderr, "l7vsd_test", LOG_DEBUG, 0);
//		vanessa_logger_set(vl);

	}

	//異常系のテスト
	//iomux_pollが-1を返した時(errno=EBADF)
	cout << "異常系_iomux_poll=-1_BADF---" << endl;
	pid = fork();
	if (0 > pid) {
		BOOST_ERROR("fork failed");
	} else if (0 == pid) {
		//子プロセスでは親プロセスにSIGTERMを送る
		pid_t ppid = getppid();
		sleep(2);
		kill(ppid, SIGTERM);
		exit(0);
	} else {
		//デバッグ変数の初期化を行う
		optind = 1;
		opterr = 1;
		exit_requested = 0;
		received_sig =0;
		int test_argc = 2;
		char *test_argv[] = {"l7vsd_test", "--debug"};
		daemon_hook = true;
		daemon_count = 0;
		daemon_ret = 0;
		memset(l7vs_module_path, 0, sizeof(l7vs_module_path));
		ret_iomux_poll = -1;
		errno_iomux_poll = EBADF;
		call_count_iomux_poll = 0;
		ret_iomux_init = 0;
		call_count_iomux_init = 0;
		call_count_iomux_fini = 0;
		ret_config_init = 0;
		call_count_config_init = 0;
		call_count_config_fini = 0;
		ret_module_init = 0;
		call_count_module_init = 0;
		call_count_module_fini = 0;
		ret_lsock_init = 0;
		call_count_lsock_init = 0;
		call_count_lsock_fini = 0;
		ret_conn_init = 0;
		call_count_conn_init = 0;
		//関数を実行する
		l7vsd_main(test_argc, test_argv);
		daemon_hook = false;
		//各デバッグ変数のチェック
		//＊l7vs_module_pathはセットされているか
		cout << "34---------------------------------------" << endl;
		BOOST_CHECK(!strncmp(l7vs_module_path, L7VS_MODULE_PATH, sizeof(L7VS_MODULE_PATH)));
		//＊l7vs_iomux_poll関数は1回以上呼ばれているか
		cout << "35---------------------------------------" << endl;
		BOOST_CHECK(call_count_iomux_poll >= 1);
		//＊l7vs_iomux_init関数は1回呼ばれているか
		cout << "36---------------------------------------" << endl;
		BOOST_CHECK_EQUAL(call_count_iomux_init, 1);
		//＊l7vs_iomux_fini関数は1回呼ばれているか
		cout << "37---------------------------------------" << endl;
		BOOST_CHECK_EQUAL(call_count_iomux_fini, 1);
		//＊l7vs_config_init関数は1回呼ばれているか
		cout << "38---------------------------------------" << endl;
		BOOST_CHECK_EQUAL(call_count_config_init, 1);
		//＊l7vs_config_fini関数は1回呼ばれているか
		cout << "39---------------------------------------" << endl;
		BOOST_CHECK_EQUAL(call_count_config_fini, 1);
		//＊l7vs_module_init関数は1回呼ばれているか
		cout << "40---------------------------------------" << endl;
		BOOST_CHECK_EQUAL(call_count_module_init, 1);
		//＊l7vs_module_fini関数は1回呼ばれているか
		cout << "41---------------------------------------" << endl;
		BOOST_CHECK_EQUAL(call_count_module_fini, 1);
		//＊l7vs_lsock_init関数は1回呼ばれているか
		cout << "42---------------------------------------" << endl;
		BOOST_CHECK_EQUAL(call_count_lsock_init, 1);
		//＊l7vs_lsock_fini関数は1回呼ばれているか
		cout << "43---------------------------------------" << endl;
		BOOST_CHECK_EQUAL(call_count_lsock_fini, 1);
		//＊l7vs_conn_init関数は1回呼ばれているか
		cout << "48---------------------------------------" << endl;
		BOOST_CHECK_EQUAL(call_count_conn_init, 1);
		//signalの設定を再度取得する
		cout << "44---------------------------------------" << endl;
		ret = sigaction(SIGHUP, NULL, &act[0]);
		ret = sigaction(SIGINT, NULL, &act[1]);
		ret = sigaction(SIGQUIT, NULL, &act[2]);
		ret = sigaction(SIGTERM, NULL, &act[3]);
		ret = sigaction(SIGUSR1, NULL, &act[4]);
		ret = sigaction(SIGUSR2, NULL, &act[5]);
		ret = sigaction(SIGALRM, NULL, &act[6]);
		ret = sigaction(SIGCHLD, NULL, &act[7]);
		//＊handlerはセットされているか
		cout << "45---------------------------------------" << endl;
		BOOST_CHECK_EQUAL(act[0].sa_handler, sig_exit_handler);
		BOOST_CHECK_EQUAL(act[1].sa_handler, sig_exit_handler);
		BOOST_CHECK_EQUAL(act[2].sa_handler, sig_exit_handler);
		BOOST_CHECK_EQUAL(act[3].sa_handler, sig_exit_handler);
		BOOST_CHECK_EQUAL(act[4].sa_handler, SIG_IGN);
		BOOST_CHECK_EQUAL(act[5].sa_handler, SIG_IGN);
		BOOST_CHECK_EQUAL(act[6].sa_handler, SIG_IGN);
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
//		//loggerの設定を元に戻す
//		vanessa_logger_t *vl;
//		vl = vanessa_logger_openlog_filehandle(stderr, "l7vsd_test", LOG_DEBUG, 0);
//		vanessa_logger_set(vl);

	}

	BOOST_MESSAGE( "----- main test end -----" );
}

void
main_help_short_test()
{
	BOOST_MESSAGE( "----- main help_short test start -----" );

	//正常系のテスト
	//help出力
	//デバッグ変数の初期化を行う
	optind = 1;
	opterr = 1;
	int test_argc = 2;
	char *test_argv[] = {"l7vsd_test", "-h"};
	//関数を実行する
	l7vsd_main(test_argc, test_argv);
	//＊usageが表示されるか
	cout << "1----------------------------------------" << endl;
	//終了してしまうので変数チェックは出来ない

	BOOST_MESSAGE( "----- main help_short test end -----" );
}

void
main_help_long_test()
{
	BOOST_MESSAGE( "----- main help_long test start -----" );

	//正常系のテスト
	//help出力
	//デバッグ変数の初期化を行う
	optind = 1;
	opterr = 1;
	int test_argc = 2;
	char *test_argv[] = {"l7vsd_test", "--help"};
	//関数を実行する
	l7vsd_main(test_argc, test_argv);
	//＊usageが表示されるか
	cout << "1----------------------------------------" << endl;
	//終了してしまうので変数チェックは出来ない

	BOOST_MESSAGE( "----- main help_long test end -----" );
}

void
main_invalid_arg_test()
{
	BOOST_MESSAGE( "----- main invalid_arg test start -----" );

	//異常系のテスト
	//無効な引数
	//デバッグ変数の初期化を行う
	optind = 1;
	opterr = 1;
	int test_argc = 2;
	char *test_argv[] = {"l7vsd_test", "--test"};
	//関数を実行する
	l7vsd_main(test_argc, test_argv);
	//＊unknown optionとusageが表示されるか
	cout << "1----------------------------------------" << endl;
	//終了してしまうので変数チェックは出来ない

	BOOST_MESSAGE( "----- main invalid_arg test end -----" );
}

void
main_daemon_failed_test()
{
	BOOST_MESSAGE( "----- main daemon_failed test start -----" );

	//異常系のテスト
	//daemon関数失敗
	//デバッグ変数の初期化を行う
	optind = 1;
	opterr = 1;
	int test_argc = 1;
	char *test_argv[] = {"l7vsd_test"};
	daemon_hook = true;
	daemon_count = 0;
	daemon_ret = -1;
	//関数を実行する
	l7vsd_main(test_argc, test_argv);
	//＊daemonで失敗して終了するか
	cout << "1----------------------------------------" << endl;
	//終了してしまうので変数チェックは出来ない

	BOOST_MESSAGE( "----- main daemon_failed test end -----" );
}

void
main_iomux_init_failed_test()
{
	BOOST_MESSAGE( "----- main iomux_init_failed test start -----" );

	//異常系のテスト
	//iomux_init関数失敗
	//デバッグ変数の初期化を行う
	optind = 1;
	opterr = 1;
	int test_argc = 2;
	char *test_argv[] = {"l7vsd_test", "--debug"};
	daemon_hook = true;
	daemon_count = 0;
	daemon_ret = 0;
	ret_iomux_init = -1;
	call_count_iomux_init = 0;
	//関数を実行する
	l7vsd_main(test_argc, test_argv);
	//＊iomux_initで失敗して終了するか
	cout << "1----------------------------------------" << endl;
	//終了してしまうので変数チェックは出来ない

	BOOST_MESSAGE( "----- main iomux_init_failed test end -----" );
}

void
main_config_init_failed_test()
{
	BOOST_MESSAGE( "----- main config_init_failed test start -----" );

	//異常系のテスト
	//iomux_init関数失敗
	//デバッグ変数の初期化を行う
	optind = 1;
	opterr = 1;
	int test_argc = 2;
	char *test_argv[] = {"l7vsd_test", "--debug"};
	daemon_hook = true;
	daemon_count = 0;
	daemon_ret = 0;
	ret_iomux_init = 0;
	call_count_iomux_init = 0;
	ret_config_init = -1;
	call_count_config_init = 0;
	//関数を実行する
	l7vsd_main(test_argc, test_argv);
	//＊config_initで失敗して終了するか
	cout << "1----------------------------------------" << endl;
	//終了してしまうので変数チェックは出来ない

	BOOST_MESSAGE( "----- main config_init_failed test end -----" );
}
test_suite*
init_unit_test_suite(int argc, char* argv[])
{
//	vanessa_logger_t *vl;
//	vl = vanessa_logger_openlog_filehandle(stderr, argv[0], LOG_DEBUG, 0);
//	vanessa_logger_set(vl);

	//glibc関数の紐付け
	org_sigaction = (int (*)(int, const struct sigaction*, struct sigaction*)) dlsym(RTLD_NEXT, "sigaction");
	org_daemon = (int (*)(int, int)) dlsym(RTLD_NEXT, "daemon");
//	org__exit = (void (*)(int)) dlsym(RTLD_NEXT, "_exit");

	test_suite* test = BOOST_TEST_SUITE("l7vsd_test");
	test->add(BOOST_TEST_CASE(sig_exit_handler_test));
	test->add(BOOST_TEST_CASE(set_sighandler_test));
	test->add(BOOST_TEST_CASE(set_sighandlers_test));
	test->add(BOOST_TEST_CASE(usage_test));
	//これより下のテストはプログラムが終了してしまうため、排他的に実行する
	if (argc == 1) {
		test->add(BOOST_TEST_CASE(main_test));
	}
	if (argc > 1) {
		switch(atoi(argv[1])) {
		case 1:
			test->add(BOOST_TEST_CASE(main_help_short_test));
			break;
		case 2:
		test->add(BOOST_TEST_CASE(main_help_long_test));
			break;
		case 3:
		test->add(BOOST_TEST_CASE(main_invalid_arg_test));
			break;
		case 4:
		test->add(BOOST_TEST_CASE(main_daemon_failed_test));
			break;
		case 5:
		test->add(BOOST_TEST_CASE(main_iomux_init_failed_test));
			break;
		case 6:
		test->add(BOOST_TEST_CASE(main_config_init_failed_test));
			break;
		}
	}

	return test;
};


//--------------------------スタブ関数----------------------------
int
l7vs_iomux_init()
{
	cout << "l7vs_iomux_init called" << endl;
	++call_count_iomux_init;
	return ret_iomux_init;
}

int
l7vs_config_init()
{
	cout << "l7vs_config_init called" << endl;
	++call_count_config_init;
	return ret_config_init;
}

int
l7vs_module_init(char *modpath)
{
	cout << "l7vs_module_init called" << endl;
	++call_count_module_init;
	arg_modpath_module_init = modpath;
	return ret_module_init;
}

int
l7vs_lsock_init()
{
	cout << "l7vs_lsock_init called" << endl;
	++call_count_lsock_init;
	return ret_lsock_init;
}

int
l7vs_iomux_poll(struct timeval *timo)
{
	cout << "l7vs_iomux_poll called" << endl;
	++call_count_iomux_poll;
	arg_timo_iomux_poll = timo;
	sleep(1);
	if (0 > ret_iomux_poll) {
		errno = errno_iomux_poll;
	}
	return ret_iomux_poll;
}

void
l7vs_lsock_fini()
{
	cout << "l7vs_lsock_fini called" << endl;
	++call_count_lsock_fini;
	return;
}

void
l7vs_module_fini()
{
	cout << "l7vs_module_fini called" << endl;
	++call_count_module_fini;
	return;
}

void
l7vs_config_fini()
{
	cout << "l7vs_config_poll called" << endl;
	++call_count_config_fini;
	return;
}

void
l7vs_iomux_fini()
{
	cout << "l7vs_iomux_fini called" << endl;
	++call_count_iomux_fini;
	return;
}
int
l7vs_conn_init()
{
	cout << "l7vs_conn_init called" << endl;
	++call_count_conn_init;
	return 1;
}

void
test_handler(int sig)
{
	cout << "test_handler called" << endl;
	++call_count_test_handler;
	arg_sig_test_handler = sig;
	return;
}


//ライブラリ関数ラッパー
int
sigaction(int signum, const struct sigaction *act, struct sigaction *oldact)
{
	int ret;
	//printf("__sigaction__\n");

	if (sigaction_hook && 0 >= sigaction_count) {
		return -1;
	} else {
		if (0 < sigaction_count) --sigaction_count;
		ret = (*org_sigaction)(signum, act, oldact);
	}

	return ret;
}

int
daemon(int nochdir, int noclose)
{
	int ret;
	//printf("__daemon__\n");

	if (daemon_hook && 0 >= daemon_count) {
		return daemon_ret;
	} else {
		if (0 < daemon_count) --daemon_count;
		ret = (*org_daemon)(nochdir, noclose);
	}

	return ret;
}

/*
void
_exit(int exitstatus)
{
	if (_exit_hook && 0 >= _exit_count) {
	} else {
		if (0 < _exit_count) --_exit_count;
		(*org__exit)(exitstatus);
	}
}
*/
