/*
 * @file  strict_time_based_rolling_policy.h
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

#ifndef __STRICT_TIME_BASED_ROLLING_POLICY_H__
#define __STRICT_TIME_BASED_ROLLING_POLICY_H__

#include <log4cxx/portability.h>
#include <log4cxx/rolling/rollingpolicybase.h>
#include <log4cxx/rolling/triggeringpolicy.h>
#include <log4cxx/rolling/fixedwindowrollingpolicy.h>
#include "logger_enum.h"

namespace log4cxx {
	namespace rolling {
		/*!
		 *  log4cxx time based rolling policy class.
		 *  this policy class can specify rolling time more strictly
		 *  than normal TimeBasedRollingPolicy.
		 */
		class LOG4CXX_EXPORT StrictTimeBasedRollingPolicy :
			public FixedWindowRollingPolicy,
			public TriggeringPolicy
		{
			DECLARE_LOG4CXX_OBJECT(StrictTimeBasedRollingPolicy)
			BEGIN_LOG4CXX_CAST_MAP()
				LOG4CXX_CAST_ENTRY(StrictTimeBasedRollingPolicy)
				LOG4CXX_CAST_ENTRY_CHAIN(FixedWindowRollingPolicy)
				LOG4CXX_CAST_ENTRY_CHAIN(TriggeringPolicy)
			END_LOG4CXX_CAST_MAP()

		protected:
			//! time of next rollover
			time_t nextCheck;
			/*!
			 *  rotation timing
			 *  LOG_TIM_YEAR   = yearly
			 *  LOG_TIM_MONTH  = monthly
			 *  LOG_TIM_WEEK   = weekly
			 *  LOG_TIM_DATE   = daily
			 *  LOG_TIM_HOUR   = hourly
			 */
			l7vs::LOG_ROTATION_TIMING_TAG rotationTiming;
			/*!
			 *  rotation timing value 
			 *
			 *  rotation timing     value
			 *  -------------------------------------------------
			 *  LOG_TIM_YEAR	"03051500"	(3/5 15:00)
			 *  LOG_TIM_MONTH	"051100"	(5 11:00)
			 *  LOG_TIM_WEEK	"12000"		(mon 20:00) sun = 0, sat = 6
			 *  LOG_TIM_DATE	"1500"		(15:00)
			 *  LOG_TIM_HOUR	"45"		(45)
			 */
			std::string rotationTimingValue;
			//! calculate next rollover timing
			time_t getNextCheck(time_t now_time);
			
		public:
			//! default constructor
			StrictTimeBasedRollingPolicy();
			//! increase reffernce count
			void addRef() const;
			//! decrease reffernce count
			void releaseRef() const;
			//! option setter
			void setOption(const LogString&, const LogString&);
			//! evaluate and activate options
			void activateOptions(log4cxx::helpers::Pool& );

			//! rotationTiming getter
			l7vs::LOG_ROTATION_TIMING_TAG getRotationTiming();
			//! rotationTiming setter
			void setRotationTiming(const l7vs::LOG_ROTATION_TIMING_TAG);
			//! rotatioTimingValue getter
			std::string getRotationTimingValue();
			//! rotatioTimingValue setter
			void setRotationTimingValue(const std::string&);
			
			//! rolling policy initialize
			RolloverDescriptionPtr initialize(
				const LogString& file,
				const bool append,
				log4cxx::helpers::Pool& pool);
			//! invoke when rollover event
			RolloverDescriptionPtr rollover(const LogString& activeFile,
				log4cxx::helpers::Pool& pool);
			
			//! returns do rollover or not 
			virtual bool isTriggeringEvent(
				Appender* appender,
				const log4cxx::spi::LoggingEventPtr& event,
				const LogString& filename,
				size_t fileLength);
			
		};
		LOG4CXX_PTR_DEF(StrictTimeBasedRollingPolicy);
	}
}

#endif	//__STRICT_TIME_BASED_ROLLING_POLICY_H__

