/*!
 * @file  logger_rotation_enum.h
 * @brief logger module rotation constants enumeration.
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

#ifndef LOGGER_ROTATION_ENUM_H
#define LOGGER_ROTATION_ENUM_H

namespace l7vs{

//!	@enum	LOG_ROTATION_TAG
//!	@brief	Rotation kind enumeration
enum LOG_ROTATION_TAG {
	LOG_ROT_SIZE,		//!< rotate by filesize
	LOG_ROT_DATE,		//!< rotate by date
	LOG_ROT_DATESIZE	//!< rotate by date and filesize
};

//!	@enum	LOG_ROTATION_TAG
//!	@brief	RotationTiming enumeration
enum LOG_ROTATION_TIMING_TAG {
	LOG_TIM_YEAR,		//!< rotate yearly
	LOG_TIM_MONTH,		//!< rotate monthly
	LOG_TIM_WEEK,		//!< rotate weekly
	LOG_TIM_DATE,		//!< rotate daily
	LOG_TIM_HOUR		//!< rotate hourly
};

}	//namespace l7vs
#endif	//__LOGGER_ROTATION_ENUM_H__
