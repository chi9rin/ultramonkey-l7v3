/*
 * @file  sslid_ut.cpp
 * @brief sslid test main file.
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

#include <boost/test/included/unit_test.hpp>

#include "protocol_module_sslid_test.cpp"
#include "ssl_protocol_module_base_test.cpp"
#include "sslid_replication_data_processor_test.cpp"
#include "sslid_session_data_processor_test.cpp"

test_suite*	init_unit_test_suite( int argc, char* argv[] ){

	protocol_module_sslid_test_main();
	ssl_protocol_module_base_test_main();
	sslid_replication_data_processor_test_main();
	sslid_session_data_processor_test_main();

	return 0;
}

