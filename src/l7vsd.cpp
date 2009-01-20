#include <signal.h>
#include <stdlib.h>
#include <errno.h>
#include "logger.h"
#include "parameter.h"
#include "l7vsd.h"

static void	sig_exit_handler(int sig);
static int	set_sighandler(int sig, void (*handler)(int));
static int	set_sighandlers();
static void	usage(FILE* p);
static int	l7vsd_main( int, char* );

static bool	exit_requested = false;
static int	received_sig = 0;

namespace l7vs{

Logger logger_instance;
Parameter parameter_instance;

void l7vsd::run() {

}

};// namespace l7vsd

static void sig_exit_handler(int sig) {
	received_sig = sig;
	exit_requested = true;
}

static int set_sighandler(int sig, void (*handler)(int)) {
	struct sigaction act;
	int ret;

	ret = sigaction(sig, NULL, &act);
	if (ret < 0) {
		//LOGGER_PUT_LOG_ERROR(LOG_CAT_L7VSD_SYSTEM_SIGNAL,1, "sigaction on signal %d failed", sig);
		return ret;
	}
	act.sa_flags &= ~SA_RESETHAND;
	act.sa_handler = handler;

	ret = sigaction(sig, &act, NULL);
	if (ret < 0) {
		//LOGGER_PUT_LOG_ERROR(LOG_CAT_L7VSD_SYSTEM_SIGNAL,2, "sigaction on signal %d failed", sig);
		return ret;
	}

	return 0;
}

static int set_sighandlers() {
	int ret;

#define SET_SIGHANDLER(sig, handler)				\
	do {											\
		ret = set_sighandler((sig), (handler));		\
		if (ret < 0) {								\
			return ret;								\
		}											\
	} while (0)

	SET_SIGHANDLER(SIGHUP,	sig_exit_handler);
	SET_SIGHANDLER(SIGINT,	sig_exit_handler);
	SET_SIGHANDLER(SIGQUIT,	sig_exit_handler);
	SET_SIGHANDLER(SIGTERM,	sig_exit_handler);
	SET_SIGHANDLER(SIGUSR1,	SIG_IGN);
	SET_SIGHANDLER(SIGUSR2,	SIG_IGN);
	SET_SIGHANDLER(SIGALRM,	SIG_IGN);
	SET_SIGHANDLER(SIGCHLD,	SIG_IGN);

#undef SET_SIGHANDLER

    return 0;
}

static void usage(FILE* p) {

}

int l7vsd_main( int argc, char* argv[] ){
	try{
		if (0 > set_sighandlers()) {
			exit(-1);
		}

		l7vs::l7vsd vsd;
		vsd.run();


	}
	catch( ... ){

	}
	return 0;
}

#ifndef	TEST_CASE
int main( int argc, char* argv[] ){
	return l7vsd_main( argc, argv );
}
#endif	//TEST_CASE
