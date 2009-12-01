/*!
 * @file  time_and_size_based_rolling_policy.h
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

#ifndef __TIME_AND_SIZE_BASED_ROLLING_POLICY_H__
#define __TIME_AND_SIZE_BASED_ROLLING_POLICY_H__

#include <log4cxx/portability.h>
#include "strict_time_based_rolling_policy.h"
#include "logger_rotation_enum.h"

namespace log4cxx {
	namespace rolling {
		/*!
		 *  log4cxx time and size based rolling policy class.
		 *  this policy class is delivered from StrictTimeBasedRollingPolicy.
		 *  so it can size specify in addition to strict time specify.
		 */
		class LOG4CXX_EXPORT TimeAndSizeBasedRollingPolicy :
			public StrictTimeBasedRollingPolicy
		{
			DECLARE_LOG4CXX_OBJECT(TimeAndSizeBasedRollingPolicy)
			BEGIN_LOG4CXX_CAST_MAP()
				LOG4CXX_CAST_ENTRY(TimeAndSizeBasedRollingPolicy)
				LOG4CXX_CAST_ENTRY_CHAIN(StrictTimeBasedRollingPolicy)
			END_LOG4CXX_CAST_MAP()

		protected:
			//! maximum size of file
			size_t maxFileSize;
			
		public:
			//! default constructor
			TimeAndSizeBasedRollingPolicy();
			//! option setter
			void setOption(const LogString&, const LogString&);
			//! maxFileSize getter
			size_t getMaxFileSize();
			//! maxFileSize setter
			void setMaxFileSize(size_t);
			
			//! returns do rollover or not
			virtual bool isTriggeringEvent(
				Appender* appender,
				const log4cxx::spi::LoggingEventPtr& event,
				const LogString& filename,
				size_t fileLength);
			
		};
		LOG4CXX_PTR_DEF(TimeAndSizeBasedRollingPolicy);
	}
}

#endif	//__TIME_AND_SIZE_BASED_ROLLING_POLICY_H__

