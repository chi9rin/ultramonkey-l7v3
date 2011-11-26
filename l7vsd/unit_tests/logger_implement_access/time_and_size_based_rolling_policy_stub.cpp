#include <log4cxx/logstring.h>
#include <log4cxx/pattern/filedatepatternconverter.h>
#include <log4cxx/helpers/date.h>
#include <log4cxx/rolling/filerenameaction.h>
#include <log4cxx/helpers/loglog.h>
#include <log4cxx/helpers/exception.h>
#include <log4cxx/helpers/stringhelper.h>
#include <log4cxx/helpers/optionconverter.h>
#include <time.h>
#include "time_and_size_based_rolling_policy.h"
#include <boost/lexical_cast.hpp>
#define DEFAULT_MAX_FILE_SIZE (10 * 1024 * 1024)

using namespace log4cxx;
using namespace log4cxx::rolling;
using namespace log4cxx::helpers;
using namespace log4cxx::pattern;

extern int timeSizePolicyCtorCalled;
extern void *timeSizePolicyCreated;

IMPLEMENT_LOG4CXX_OBJECT(TimeAndSizeBasedRollingPolicy)

TimeAndSizeBasedRollingPolicy::TimeAndSizeBasedRollingPolicy() :
        maxFileSize(DEFAULT_MAX_FILE_SIZE)
{
        ++timeSizePolicyCtorCalled;
        timeSizePolicyCreated = this;
}

size_t TimeAndSizeBasedRollingPolicy::getMaxFileSize()
{
        return maxFileSize;
}

void TimeAndSizeBasedRollingPolicy::setMaxFileSize(size_t size)
{
        maxFileSize = size;
}

void TimeAndSizeBasedRollingPolicy::setOption(const LogString &option, const LogString &value)
{
}

bool TimeAndSizeBasedRollingPolicy::isTriggeringEvent(
        Appender *appender,
        const log4cxx::spi::LoggingEventPtr &event,
        const LogString &filename,
        size_t fileLength)
{
        return true;
}
