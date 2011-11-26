#include <log4cxx/logstring.h>
#include <log4cxx/pattern/filedatepatternconverter.h>
#include <log4cxx/helpers/date.h>
#include <log4cxx/rolling/filerenameaction.h>
#include <log4cxx/helpers/loglog.h>
#include <log4cxx/helpers/exception.h>
#include <log4cxx/helpers/stringhelper.h>
#include <log4cxx/helpers/optionconverter.h>
#include <time.h>
#include "strict_time_based_rolling_policy.h"
#include <boost/lexical_cast.hpp>
#include "logger_rotation_enum.h"

using namespace log4cxx;
using namespace log4cxx::rolling;
using namespace log4cxx::helpers;
using namespace log4cxx::pattern;

using namespace l7vs;
extern int strictPolicyCtorCalled;
extern void *strictPolicyCreated;


IMPLEMENT_LOG4CXX_OBJECT(StrictTimeBasedRollingPolicy)

StrictTimeBasedRollingPolicy::StrictTimeBasedRollingPolicy() :
        nextCheck(0), rotationTiming(LOG_TIM_YEAR), rotationTimingValue("")
{
        ++strictPolicyCtorCalled;
        strictPolicyCreated = this;
}

void StrictTimeBasedRollingPolicy::addRef() const
{
}

void StrictTimeBasedRollingPolicy::releaseRef() const
{
}

void StrictTimeBasedRollingPolicy::activateOptions(log4cxx::helpers::Pool &pool)
{
}

std::string StrictTimeBasedRollingPolicy::getRotationTimingValue()
{
        return rotationTimingValue;
}

void StrictTimeBasedRollingPolicy::setRotationTimingValue(const std::string &val)
{
        rotationTimingValue = val;
}

LOG_ROTATION_TIMING_TAG StrictTimeBasedRollingPolicy::getRotationTiming()
{
        return rotationTiming;
}

void StrictTimeBasedRollingPolicy::setRotationTiming(const LOG_ROTATION_TIMING_TAG val)
{
        rotationTiming = val;
}

void StrictTimeBasedRollingPolicy::setOption(const LogString &option, const LogString &value)
{
}

RolloverDescriptionPtr StrictTimeBasedRollingPolicy::initialize(
        const LogString &currentActiveFile,
        const bool append,
        Pool &pool)
{
        RolloverDescriptionPtr desc;
        return desc;
}

time_t StrictTimeBasedRollingPolicy::getNextCheck(time_t timeNow)
{
        return 0;
}

RolloverDescriptionPtr StrictTimeBasedRollingPolicy::rollover(
        const LogString &currentActiveFile,
        Pool &pool)
{
        RolloverDescriptionPtr desc;
        return desc;
}



bool StrictTimeBasedRollingPolicy::isTriggeringEvent(
        Appender* /* appender */,
        const log4cxx::spi::LoggingEventPtr& /* event */,
        const LogString& /* filename */,
        size_t /* fileLength */)
{
        return true;
}
