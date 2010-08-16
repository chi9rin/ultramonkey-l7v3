#include "logger_implement_access.h"


#define LOGGER_ACCESS_LAYOUT "%d{%Y/%m/%d %H:%M:%S} [%p] %c %m %t %F:%L%n"
#define LOGGER_ACCESS_DEFAULT_BUFFER_SIZE (8 * 1024)
#define LOGGER_ACCESS_SYSLOG_FACILITY "USER"
#define LOGGER_ACCESS_BACKUP_INDEX_LOWER_LIMIT (1)
#define LOGGER_ACCESS_BACKUP_INDEX_LIMIT (12)
#define LOGGER_ACCESS_FILESIZE_LOWER_LIMIT (65535)
#define LOGGER_ACCESS_FILE_PATTERN "%i"


l7vs::logger_implement_access::logger_implement_access(const std::string &acLogFileName)
{
}


