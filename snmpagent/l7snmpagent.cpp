#include <signal.h>

#include "logger_wrapper.h"
#include "parameter_wrapper.h"

using namespace l7vs;

Logger	Logger::instance;
Parameter	Parameter::instance;

/*!
 *
 */
static void
handler_sig_exit(int sig)
{
}

/*!
 * setup signal handler
 */
static int
set_sighandler(int sig, void (*handler)(int))
{
	struct sigaction act;
	int ret;

	ret = sigaction(sig, NULL, &act);
	if (ret < 0) {
		return ret;
	}
	act.sa_flags &= ~SA_RESETHAND;
	act.sa_handler = handler;
	ret = sigaction(sig, &act, NULL);
	if (ret < 0) {
		return ret;
	}
	return 0;
}

/*!
 *esetup all signal handlers
 */
static int
set_sighandlers()
{
	//debug log out
	if( LOG_LV_DEBUG == logger_get_log_level( LOG_CAT_SNMPAGENT_START_STOP ) ){
		char debugstr[DEBUG_STR_LEN];
		sprintf( debugstr, "in_function set_sighandler()" );
		LOGGER_PUT_LOG_DEBUG( LOG_CAT_SNMPAGENT_START_STOP,1, debugstr );
	}

	int ret;

#define SET_SIGHANDLER(sig, handler)                    \
        do {                                            \
                ret = set_sighandler((sig), (handler)); \
                if (ret < 0) {                          \
			if (LOG_LV_DEBUG == logger_get_log_level(LOG_CAT_L7VSD_PROGRAM)) {	\
				LOGGER_PUT_LOG_DEBUG(LOG_CAT_L7VSD_PROGRAM, 0,			\
					"out_function: static int set_sighandlers(void) "	\
					"return_value: %d",					\
					ret);							\
			}									\
                        return ret;                     \
                }                                       \
        } while (0)

	SET_SIGHANDLER( SIGHUP,  handler_sig_exit );
	SET_SIGHANDLER( SIGINT,  handler_sig_exit );
	SET_SIGHANDLER( SIGQUIT, handler_sig_exit );
	SET_SIGHANDLER( SIGTERM, handler_sig_exit );
	SET_SIGHANDLER( SIGUSR1, SIG_IGN );
	SET_SIGHANDLER( SIGUSR2, SIG_IGN );
	SET_SIGHANDLER( SIGALRM, SIG_IGN );
	SET_SIGHANDLER( SIGCHLD, SIG_IGN );

#undef SET_SIGHANDLER

	//debug log out
	if( LOG_LV_DEBUG == logger_get_log_level( LOG_CAT_SNMPAGENT_START_STOP ) ){
		char debugstr[DEBUG_STR_LEN];
		sprintf( debugstr, "out_function set_sighandler()" );
		LOGGER_PUT_LOG_DEBUG( LOG_CAT_SNMPAGENT_START_STOP,2, debugstr );
	}
}

/*!
 * l7snmpagent main
 * @param[in]	argc	number of argument
 * @param[in]	argv	array of argument
 */
int main( int argc, char * argv[] )
{
	LOGGER_PUT_LOG_INFO( LOG_CAT_SNMPAGENT_START_STOP,1, "start l7snmpagent" );
	//debug log out
	if( LOG_LV_DEBUG == logger_get_log_level( LOG_CAT_SNMPAGENT_START_STOP ) ){
		char debugstr[DEBUG_STR_LEN];
		LOGGER_PUT_LOG_DEBUG( LOG_CAT_SNMPAGENT_START_STOP,3, debugstr );
	}

	set_sighandlers();

	LOGGER_PUT_LOG_INFO( LOG_CAT_SNMPAGENT_START_STOP,2, "stop l7snmpagent" );
}


