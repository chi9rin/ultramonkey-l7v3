#include <signal.h>

#include "logger_wrapper.h"
#include "parameter_wrapper.h"
#include "subagent.h"

using namespace l7vs;

Logger      Logger::instance;
Parameter   Parameter::instance;

l7ag_subagent* subagent;

/*!
 * sig handler
 */
static void
handler_sig_exit(int sig)
{
    subagent->stop();
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
    if( LOG_LV_DEBUG == logger_get_log_level( LOG_CAT_SNMPAGENT_START_STOP ) ){
        char debugstr[DEBUG_STR_LEN];
        sprintf( debugstr, "in_function set_sighandler()" );
        LOGGER_PUT_LOG_DEBUG( LOG_CAT_SNMPAGENT_START_STOP, 0, debugstr );
    }

    int ret;

#define SET_SIGHANDLER(sig, handler)                                            \
    do {                                                                        \
        ret = set_sighandler((sig), (handler));                                 \
        if (ret < 0) {                                                          \
            if (LOG_LV_DEBUG == logger_get_log_level(LOG_CAT_L7VSD_PROGRAM)) {  \
                LOGGER_PUT_LOG_DEBUG(LOG_CAT_L7VSD_PROGRAM, 0,                  \
                    "out_function: static int set_sighandlers(void) "           \
                    "return_value: %d",                                         \
                    ret);                                                       \
            }                                                                   \
            return ret;                                                         \
        }                                                                       \
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

    if( LOG_LV_DEBUG == logger_get_log_level( LOG_CAT_SNMPAGENT_START_STOP ) ){
        char debugstr[DEBUG_STR_LEN];
        sprintf( debugstr, "out_function set_sighandler()" );
        LOGGER_PUT_LOG_DEBUG( LOG_CAT_SNMPAGENT_START_STOP, 0, debugstr );
    }
    return 0;
}

/*!
 * l7snmpagent main
 * @param[in]   argc    number of argument
 * @param[in]   argv    array of argument
 */
int
main(int argc, char* argv[])
{
    LOGGER_PUT_LOG_INFO( LOG_CAT_SNMPAGENT_START_STOP, 0, "start l7snmpagent" );

    if( LOG_LV_DEBUG == logger_get_log_level( LOG_CAT_SNMPAGENT_START_STOP ) ){
        char debugstr[DEBUG_STR_LEN];
        // TODO set debugstr
        LOGGER_PUT_LOG_DEBUG( LOG_CAT_SNMPAGENT_START_STOP, 0, debugstr );
    }

    set_sighandlers();

    // TODO whats this comment?
//  parameter_register_function_pointer();

    subagent = new l7ag_subagent();

    // start subagent
    if(!subagent->start()){
        // TODO error log
    }

    LOGGER_PUT_LOG_INFO( LOG_CAT_SNMPAGENT_START_STOP, 0, "stop l7snmpagent" );

    delete subagent;
    subagent = NULL;
    return 0;
}

// TODO remove unit test code!!!
int
l7snmp_main(int argc, char* argv[])
{
    return main(argc, argv);
}
