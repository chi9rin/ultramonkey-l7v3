#include <signal.h>
#include <sstream>
#include <string>

#include "logger.h"
#include "parameter.h"
#include "subagent.h"

using namespace l7vs;

l7ag_subagent *subagent;

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
        l7vs::Logger    logger;
        if (LOG_LV_DEBUG == logger.getLogLevel(l7vs::LOG_CAT_SNMPAGENT_START_STOP)) {
                std::string debugstr("in_function set_sighandler()");
                logger.putLogDebug(l7vs::LOG_CAT_SNMPAGENT_START_STOP, 0, debugstr, __FILE__, __LINE__);
        }

        int ret;

#define SET_SIGHANDLER(sig, handler)                                                                    \
    do {                                                                                                \
        ret = set_sighandler((sig), (handler));                                                         \
        if (ret < 0) {                                                                                  \
            if (LOG_LV_DEBUG == logger.getLogLevel(l7vs::LOG_CAT_SNMPAGENT_SYSTEM_SIGNAL)) {            \
                std::ostringstream str; \
                str << "out_function: static int set_sighandlers(void) return_value: " << ret; \
                logger.putLogDebug(l7vs::LOG_CAT_SNMPAGENT_SYSTEM_SIGNAL, 0, str.str(), __FILE__, __LINE__ );      \
            }                                                                                           \
            return ret;                                                                                 \
        }                                                                                               \
    } while (0)

        SET_SIGHANDLER(SIGHUP,  handler_sig_exit);
        SET_SIGHANDLER(SIGINT,  handler_sig_exit);
        SET_SIGHANDLER(SIGQUIT, handler_sig_exit);
        SET_SIGHANDLER(SIGTERM, handler_sig_exit);
        SET_SIGHANDLER(SIGUSR1, SIG_IGN);
        SET_SIGHANDLER(SIGUSR2, SIG_IGN);
        SET_SIGHANDLER(SIGALRM, SIG_IGN);
        SET_SIGHANDLER(SIGCHLD, SIG_IGN);

#undef SET_SIGHANDLER

        if (LOG_LV_DEBUG == logger.getLogLevel(l7vs::LOG_CAT_SNMPAGENT_START_STOP)) {
                std::string debugstr("out_function set_sighandler()");
                logger.putLogDebug(l7vs::LOG_CAT_SNMPAGENT_START_STOP, 0, debugstr, __FILE__, __LINE__);
        }
        return 0;
}

/*!
 * l7snmpagent main
 * @param[in]   argc    number of argument
 * @param[in]   argv    array of argument
 */
int
main(int argc, char *argv[])
{
        l7vs::Logger    logger;
        logger.putLogInfo(l7vs::LOG_CAT_SNMPAGENT_START_STOP, 0, "start l7snmpagent", __FILE__, __LINE__);

        if (LOG_LV_DEBUG == logger.getLogLevel(l7vs::LOG_CAT_SNMPAGENT_START_STOP)) {
                std::string debugstr;
                // TODO set debugstr
                logger.putLogDebug(l7vs::LOG_CAT_SNMPAGENT_START_STOP, 0, debugstr, __FILE__, __LINE__);
        }

        set_sighandlers();

        // TODO whats this comment?
//  parameter_register_function_pointer();

        subagent = new l7ag_subagent();

        // start subagent
        if (!subagent->start()) {
                // TODO error log
        }

        logger.putLogInfo(l7vs::LOG_CAT_SNMPAGENT_START_STOP, 0, "stop l7snmpagent", __FILE__, __LINE__);

        delete subagent;
        subagent = NULL;
        return 0;
}

// TODO remove unit test code!!!
int
l7snmp_main(int argc, char *argv[])
{
        return main(argc, argv);
}
