/*
 * @file  strict_time_based_rolling_policy.cpp
 * @brief log4cxx's rolling policy class. (time)
 *
 * L7VSD: Linux Virtual Server for Layer7 Load Balancing
 * Copyright (C) 2008  NTT COMWARE Corporation.
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
#include <log4cxx/rolling/filerenameaction.h>
#include <log4cxx/helpers/loglog.h>
#include <log4cxx/helpers/exception.h>
#include <log4cxx/helpers/stringhelper.h>
#include <log4cxx/helpers/optionconverter.h>
#include <time.h>
#include "strict_time_based_rolling_policy.h"
#include "lexical_cast.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace log4cxx;
using namespace log4cxx::rolling;
using namespace log4cxx::helpers;
using namespace log4cxx::pattern;

#define TIME_BUF_LEN (256)
#define LOG_DATE_FORMAT "%Y%m%d%H%M"
#define LOG_DATE_FORMAT_WEEK "%Y%m%d%w%H%M"

IMPLEMENT_LOG4CXX_OBJECT(StrictTimeBasedRollingPolicy)

/*!
 * default constructor.
 * initialize member valiables
 * @param   void
 * @return  void
 */
StrictTimeBasedRollingPolicy::StrictTimeBasedRollingPolicy() :
	nextCheck(0), rotationTiming(LOG_TIM_YEAR), rotationTimingValue("")
{
}

/*!
 * increase reffernce count
 * @param   void
 * @return  void
 */
void StrictTimeBasedRollingPolicy::addRef() const
{
	TriggeringPolicy::addRef();
}

/*!
 * decrease reffernce count
 * @param   void
 * @return  void
 */
void StrictTimeBasedRollingPolicy::releaseRef() const
{
	TriggeringPolicy::releaseRef();
}

/*!
 * evaluate and activate options
 * @param   memory pool
 * @return  void
 */
void StrictTimeBasedRollingPolicy::activateOptions(log4cxx::helpers::Pool& pool)
{
	// check for rotationTimingValue
	if (0 >= rotationTimingValue.length()) {
		LogLog::warn(
		LOG4CXX_STR("The RotationTimingValue option must be set before using StrictTimeBasedRollingPolicy. "));
		throw IllegalStateException();
	}

	// call super class's activateOptions
	FixedWindowRollingPolicy::activateOptions(pool);
}

/*!
 * rotationTimingValue getter
 * @param   void
 * @return  rotationTimingValue
 */
std::string StrictTimeBasedRollingPolicy::getRotationTimingValue()
{
	return rotationTimingValue;
}
 
/*!
 * rotationTimingValue setter
 * @param   rotationTimingValue
 * @return  void
 */
void StrictTimeBasedRollingPolicy::setRotationTimingValue(const std::string& val)
{
	rotationTimingValue = val;
}

/*!
 * rotationTiming getter
 * @param   void
 * @return  rotationTiming
 */
LOG_ROTATION_TIMING_TAG StrictTimeBasedRollingPolicy::getRotationTiming()
{
	return rotationTiming;
}
 
/*!
 * rotationTiming setter
 * @param   rotationTiming
 * @return  void
 */
void StrictTimeBasedRollingPolicy::setRotationTiming(const LOG_ROTATION_TIMING_TAG val)
{
	rotationTiming = val;
}

/*!
 * option setter
 * @param   option name
 * @param   value
 * @return  void
 */
void StrictTimeBasedRollingPolicy::setOption(const LogString& option, const LogString& value)
{
	if (StringHelper::equalsIgnoreCase(option,
		LOG4CXX_STR("ROTATIONTIMINGVALUE"),
		LOG4CXX_STR("rotationtimingvalue"))) {
		rotationTimingValue = value;
	}
	else if (StringHelper::equalsIgnoreCase(option,
		LOG4CXX_STR("ROTATIONTIMING"),
		LOG4CXX_STR("rotationtiming")))	{
		rotationTiming = (LOG_ROTATION_TIMING_TAG)OptionConverter::toInt(value, 0);
	}
	else {
		FixedWindowRollingPolicy::setOption(option, value);
	}
}

/*!
 * rolling policy initialize
 * @param   filename of current use
 * @param   append or overWrite
 * @param   memory pool
 * @return  Rollover information
 */
RolloverDescriptionPtr StrictTimeBasedRollingPolicy::initialize(
	const LogString& currentActiveFile,
	const bool append,
	Pool& pool) 
{
	// get current time
	time_t now_time;
	time_t ret_time = time(&now_time);
	if (-1 == ret_time) {
		LogLog::warn(LOG4CXX_STR("Fail to get CurrentTime. "));
		RolloverDescriptionPtr desc;
		return desc;
	}

	// get next rotation timing
	nextCheck = getNextCheck(now_time);
	if (-1 == nextCheck) {
		LogLog::warn(LOG4CXX_STR("Fail to get nextCheck. "));
		RolloverDescriptionPtr desc;
		return desc;
	}	

	// call super class's initialize
	return FixedWindowRollingPolicy::initialize(currentActiveFile, append, pool);
}

/*!
 * calculate next rollover timing
 * @param   now time
 * @return  next rollover time
 */
time_t StrictTimeBasedRollingPolicy::getNextCheck(time_t now_time)
{
	struct tm now_tm;
	struct tm *ret_tm;
	ret_tm = localtime_r(&now_time, &now_tm);
	if (0 == ret_tm) {
		return -1;
	}
	
	char buf[TIME_BUF_LEN];
	size_t ret_sz = 0;
	if (LOG_TIM_WEEK == rotationTiming) {
		ret_sz = strftime(buf, sizeof(buf), LOG_DATE_FORMAT_WEEK, &now_tm);
	}
	else {
		ret_sz = strftime(buf, sizeof(buf), LOG_DATE_FORMAT, &now_tm);
	}
	if (0 == ret_sz) {
		return -1;
	}
	std::string now(buf);

	int dates[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};	
	unsigned long long numNowDate = 0;;
	unsigned long long numTimingDate = 0;

	int numYear = 0;
	int numMonth = 0;
	int numDate = 0;
	int numNowWeek = 0;
	int numTimingWeek = 0;
	int numHour = 0;
	int numMinute = 0;

	struct tm t;
	memset(&t, 0, sizeof(struct tm));

	time_t next = 0;

	try {
		switch (rotationTiming) {
		case LOG_TIM_YEAR:
			numNowDate = l7vs::lexical_cast<unsigned long long>(now.substr(4));
			numTimingDate = l7vs::lexical_cast<unsigned long long>(rotationTimingValue);
	
			numYear = l7vs::lexical_cast<int>(now.substr(0, 4));
			numMonth = l7vs::lexical_cast<int>(rotationTimingValue.substr(0, 2));
			numDate = l7vs::lexical_cast<int>(rotationTimingValue.substr(2, 2));
			numHour = l7vs::lexical_cast<int>(rotationTimingValue.substr(4, 2));
			numMinute = l7vs::lexical_cast<int>(rotationTimingValue.substr(6));
	
			if (numTimingDate > numNowDate) {
				t.tm_year = numYear - 1900;
			}
			else {
#if defined(LOGGER_PROCESS_ADM)
				t.tm_year = numYear - 1900;
#else	//LOGGER_PROCESS_VSD or LOGGER_PROCESS_SNM
				t.tm_year = numYear + 1 - 1900;
#endif
			}
			t.tm_mon = numMonth - 1;
			t.tm_mday = numDate;
			t.tm_hour = numHour;
			t.tm_min = numMinute;
	
			next = mktime(&t);
			if (-1 == next) {
				return -1;
			}
			break;
	
		case LOG_TIM_MONTH:
			numNowDate = l7vs::lexical_cast<unsigned long long>(now.substr(6));
			numTimingDate = l7vs::lexical_cast<unsigned long long>(rotationTimingValue);
	
			numYear = l7vs::lexical_cast<int>(now.substr(0, 4));
			numMonth = l7vs::lexical_cast<int>(now.substr(4, 2));
			numDate = l7vs::lexical_cast<int>(rotationTimingValue.substr(0, 2));
			numHour = l7vs::lexical_cast<int>(rotationTimingValue.substr(2, 2));
			numMinute = l7vs::lexical_cast<int>(rotationTimingValue.substr(4));
	
			if (numTimingDate > numNowDate) {
				t.tm_year = numYear - 1900;
				t.tm_mon = numMonth - 1;
			}
			else {
#if defined(LOGGER_PROCESS_ADM)
				t.tm_year = numYear - 1900;
				t.tm_mon = numMonth - 1;
#else	//LOGGER_PROCESS_VSD or LOGGER_PROCESS_SNM
				if (12 == numMonth) {
					t.tm_year = numYear + 1 - 1900;
					t.tm_mon = 0;
				}
				else {
					t.tm_year = numYear - 1900;
					t.tm_mon = numMonth + 1 - 1;
				}
#endif
			}
	
			if (numDate > dates[t.tm_mon]) {
				t.tm_mday = dates[t.tm_mon];
			}
			else {
				t.tm_mday = numDate;
			}

			t.tm_hour = numHour;
			t.tm_min = numMinute;
	
			next = mktime(&t);
			if (-1 == next) {
				return -1;
			}
	
			break;
		case LOG_TIM_WEEK:
			numNowDate = l7vs::lexical_cast<unsigned long long>(now.substr(8));
			numTimingDate = l7vs::lexical_cast<unsigned long long>(rotationTimingValue);
	
			numYear = l7vs::lexical_cast<int>(now.substr(0, 4));
			numMonth = l7vs::lexical_cast<int>(now.substr(4, 2));
			numDate = l7vs::lexical_cast<int>(now.substr(6, 2));
			numNowWeek = l7vs::lexical_cast<int>(now.substr(8, 1));
			numTimingWeek = l7vs::lexical_cast<int>(rotationTimingValue.substr(0, 1));
			numHour = l7vs::lexical_cast<int>(rotationTimingValue.substr(1, 2));
			numMinute = l7vs::lexical_cast<int>(rotationTimingValue.substr(3));
	
			t.tm_year = numYear - 1900;
			t.tm_mon = numMonth - 1;
			if (numTimingDate > numNowDate) {	
				t.tm_mday = numDate + (numTimingWeek - numNowWeek); 
			}
			else {
#if defined(LOGGER_PROCESS_ADM)
				t.tm_mday = numDate + (numTimingWeek - numNowWeek); 
#else	//LOGGER_PROCESS_VSD or LOGGER_PROCESS_SNM
				t.tm_mday = numDate + (7 - (numNowWeek - numTimingWeek));
#endif
			}
			t.tm_hour = numHour;
			t.tm_min = numMinute;
	
			next = mktime(&t);
			if (-1 == next) {
				return -1;
			}
	
			break;
	
		case LOG_TIM_DATE:
			numNowDate = l7vs::lexical_cast<unsigned long long>(now.substr(8));
			numTimingDate = l7vs::lexical_cast<unsigned long long>(rotationTimingValue);
	
			numYear = l7vs::lexical_cast<int>(now.substr(0, 4));
			numMonth = l7vs::lexical_cast<int>(now.substr(4, 2));
			numDate = l7vs::lexical_cast<int>(now.substr(6, 2));
			numHour = l7vs::lexical_cast<int>(rotationTimingValue.substr(0, 2));
			numMinute = l7vs::lexical_cast<int>(rotationTimingValue.substr(2));
	
			t.tm_year = numYear - 1900;
			t.tm_mon = numMonth - 1;
			if (numTimingDate > numNowDate) {	
				t.tm_mday = numDate;
			}
			else {
#if defined(LOGGER_PROCESS_ADM)
				t.tm_mday = numDate; 
#else	//LOGGER_PROCESS_VSD or LOGGER_PROCESS_SNM
				t.tm_mday = numDate + 1; 
#endif
			}
			t.tm_hour = numHour;
			t.tm_min = numMinute;
	
			next = mktime(&t);
			if (-1 == next) {
				return -1;
			}
			break;
	
		default:	//HOUR
			numNowDate = l7vs::lexical_cast<unsigned long long>(now.substr(10));
			numTimingDate = l7vs::lexical_cast<unsigned long long>(rotationTimingValue);
	
			numYear = l7vs::lexical_cast<int>(now.substr(0, 4));
			numMonth = l7vs::lexical_cast<int>(now.substr(4, 2));
			numDate = l7vs::lexical_cast<int>(now.substr(6, 2));
			numHour = l7vs::lexical_cast<int>(now.substr(8, 2));
			numMinute = l7vs::lexical_cast<int>(rotationTimingValue);
	
			t.tm_year = numYear - 1900;
			t.tm_mon = numMonth - 1;
			t.tm_mday = numDate;
			if (numTimingDate > numNowDate) {
				t.tm_hour = numHour;
			}
			else {
#if defined(LOGGER_PROCESS_ADM)
				t.tm_hour = numHour;
#else	//LOGGER_PROCESS_VSD or LOGGER_PROCESS_SNM
				t.tm_hour = numHour + 1;
#endif
			}
			t.tm_min = numMinute;
	
			next = mktime(&t);
			if (-1 == next) {
				return -1;
			}
		}
	}
	catch (const std::exception& ex) {
		return -1;
	}
	return next;

}

/*!
 * log file rollover opration
 * @param   name of current log file
 * @param   memory pool
 * @return  Rollover infomation
 */
RolloverDescriptionPtr StrictTimeBasedRollingPolicy::rollover(
	const LogString& currentActiveFile,
	Pool& pool) 
{
	// get current time
	time_t now_time;
	time_t ret_time = time(&now_time);
	if (-1 == ret_time) {
		LogLog::warn(LOG4CXX_STR("Fail to get CurrentTime. "));
		RolloverDescriptionPtr desc;
		return desc;
	}

	// get next rotation timing
	nextCheck = getNextCheck(now_time);
	if (-1 == nextCheck) {
		LogLog::warn(LOG4CXX_STR("Fail to get nextCheck. "));
		RolloverDescriptionPtr desc;
		return desc;
	}	

	// call super class's rollover
	return FixedWindowRollingPolicy::rollover(currentActiveFile, pool);

}

/*!
 * returns do rollover or not
 * @param   appender (not use)
 * @param   event (not use)
 * @param   filename
 * @param   fileLength (not use)
 * @retval  true do rollover
 * @retval  false not rollover yet
 */
bool StrictTimeBasedRollingPolicy::isTriggeringEvent(
	Appender* /* appender */,
	const log4cxx::spi::LoggingEventPtr& /* event */,
	const LogString& filename,
	size_t /* fileLength */)  
{
	time_t now_time;
	time_t ret_time = time(&now_time);
	if (-1 == ret_time) {
		LogLog::warn(LOG4CXX_STR("Fail to get time. "));
		return false;
	}
#if defined(LOGGER_PROCESS_ADM)
	struct stat sb;

	if (-1 == stat(filename.c_str(), &sb)) {
		LogLog::warn(LOG4CXX_STR("Fail to get logfile update time. "));
		return false;
	}
	if (now_time > nextCheck && nextCheck > sb.st_mtime) {
		// Do rollover.
		return true;
	} else {
		// Do not rollover.
		return false;
	}
#else	//LOGGER_PROCESS_VSD or LOGGER_PROCESS_SNM
	return now_time > nextCheck;
#endif
}
