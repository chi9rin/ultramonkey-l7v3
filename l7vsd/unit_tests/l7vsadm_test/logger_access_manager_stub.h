#ifndef LOGGER_ACCESS_MANAGER_H
#define LOGGER_ACCESS_MANAGER_H

#include <sched.h>
#include <net/if.h>
#include <unistd.h>
#include <sched.h>
#include <error.h>
#include <string>
#include <map>
#include <vector>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

namespace l7vs
{
class logger_access_manager
{
public:
        static logger_access_manager &getInstance() {
                static logger_access_manager log_manager;
                return(log_manager);
        }

        bool access_log_logrotate_parameter_check(
                std::map<std::string, std::string>& rotatedata) {
                access_log_logrotate_parameter_check_called = true;
                return(!access_log_logrotate_parameter_check_fail);
        }
        static bool access_log_logrotate_parameter_check_called;
        static bool access_log_logrotate_parameter_check_fail;

protected:
        logger_access_manager() {
        }

        virtual ~logger_access_manager() {
        }
};

bool logger_access_manager::access_log_logrotate_parameter_check_called(false);
bool logger_access_manager::access_log_logrotate_parameter_check_fail(false);

};

#endif // LOGGER_ACCESS_MANAGER_H

