/*!
 * @file  time_and_size_based_rolling_policy.cpp
 * @brief log4cxx's rolling policy class. (time + size)
 *
 * L7VSD: Linux Virtual Server for Layer7 Load Balancing
 * Copyright (C) 2009  NTT COMWARE Corporation.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 **********************************************************************/

#include <log4cxx/logstring.h>
#include <log4cxx/pattern/filedatepatternconverter.h>
#include <log4cxx/helpers/date.h>
#include <log4cxx/rolling/filerenameaction.h>
#include <log4cxx/helpers/loglog.h>
#include <log4cxx/helpers/exception.h>
#include <log4cxx/helpers/stringhelper.h>
#include <log4cxx/helpers/optionconverter.h>
#include <time.h>
#include <boost/lexical_cast.hpp>
#include "time_and_size_based_rolling_policy.h"

#define DEFAULT_MAX_FILE_SIZE (10 * 1024 * 1024)

using namespace log4cxx;
using namespace log4cxx::rolling;
using namespace log4cxx::helpers;
using namespace log4cxx::pattern;

IMPLEMENT_LOG4CXX_OBJECT(TimeAndSizeBasedRollingPolicy)

/*!
 * default constructor.
 * initialize member valiables
 * @param   void
 * @return  void
 */
TimeAndSizeBasedRollingPolicy::TimeAndSizeBasedRollingPolicy() :
        maxFileSize(DEFAULT_MAX_FILE_SIZE)
{
}

/*!
 * maxFileSize getter
 * @param   void
 * @return  maxFileSize
 */
size_t TimeAndSizeBasedRollingPolicy::getMaxFileSize()
{
        return maxFileSize;
}

/*!
 * maxFileSize setter
 * @param   maxFileSize
 * @return  void
 */
void TimeAndSizeBasedRollingPolicy::setMaxFileSize(size_t size)
{
        maxFileSize = size;
}

/*!
 * option setter
 * @param   option name
 * @param   value
 * @return  void
 */
void TimeAndSizeBasedRollingPolicy::setOption(const LogString &option, const LogString &value)
{
        if (StringHelper::equalsIgnoreCase(option,
                                           LOG4CXX_STR("MAXFILESIZE"),
                                           LOG4CXX_STR("maxfilesize"))) {
                maxFileSize = OptionConverter::toFileSize(value, DEFAULT_MAX_FILE_SIZE);
        } else {
                StrictTimeBasedRollingPolicy::setOption(option, value);
        }
}

/*!
 * returns do rollover or not
 * @param   appender (not use)
 * @param   event (not use)
 * @param   filename (not use)
 * @param   fileLength
 * @retval  true do rollover
 * @retval  false not rollover yet
 */
bool TimeAndSizeBasedRollingPolicy::isTriggeringEvent(
        Appender *appender,
        const log4cxx::spi::LoggingEventPtr &event,
        const LogString &filename,
        size_t fileLength)
{

        return (fileLength >= maxFileSize ||
                StrictTimeBasedRollingPolicy::isTriggeringEvent(appender, event, filename, fileLength));
}
