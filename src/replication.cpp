/*
 *	@file	replication.cpp
 *	@brief	Replication class
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
#include	<boost/lexical_cast.hpp>
#include	<boost/format.hpp>
#include	"replication.h"
#include	"parameter.h"
#include	"logger.h"

namespace l7vs{

//! emun States Type string
static const char* replication_mode[] = {
	"REPLICATION_OUT",
	"REPLICATION_SINGLE",
	"REPLICATION_MASTER",
	"REPLICATION_SLAVE",
	"REPLICATION_MASTER_STOP",
	"REPLICATION_SLAVE_STOP"
};


//! Initialize Replication
//! @retval 0 Success
//! @retval -1 Error
int			replication::initialize(){
	Logger	logger( LOG_CAT_L7VSD_REPLICATION, 1, "replication::initialize", __FILE__, __LINE__ );

	Parameter	param;
	error_code	ip_addr_ret, service_name_ret, nic_ret, interval_ret;
	std::string	buf;

	// Check by continuous initialize.
	if ( REPLICATION_OUT != replication_state.service_status ){
		// Initialization has already been done.
		buf = boost::io::str( boost::format( "Initialization is a failure, because initialization has already been done. mode : %s" ) 
											% replication_mode[( int )replication_state.service_status] );
		Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 1, buf, __FILE__, __LINE__ );
		return -1;
	}

	// Check the Parameter exists
	replication_info.ip_addr = param.get_string( PARAM_COMP_REPLICATION, "ip_addr", ip_addr_ret );
	replication_info.service_name = param.get_string( PARAM_COMP_REPLICATION, "service_name", service_name_ret );
	replication_info.nic = param.get_string( PARAM_COMP_REPLICATION, "nic", nic_ret );
	replication_info.interval = param.get_int( PARAM_COMP_REPLICATION, "interval", interval_ret );

	// SG File not set
	if ( ip_addr_ret && service_name_ret && nic_ret && interval_ret ){
		Logger::putLogInfo( LOG_CAT_L7VSD_REPLICATION, 1, "Required item is not set in l7vs.", __FILE__, __LINE__ );
		// Status Set
		replication_state.service_status = REPLICATION_SINGLE;
		return 0;
	}

	// IP Address exists
	if ( ip_addr_ret  ){
		Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 2, "IP Address is not set.", __FILE__, __LINE__ );
		// Status Set
		replication_state.service_status = REPLICATION_SINGLE;
		return -1;
	}
	// Port exists
	if ( service_name_ret ){
		Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 3, "Port is not set.", __FILE__, __LINE__ );
		// Status Set
		replication_state.service_status = REPLICATION_SINGLE;
		return -1;
	}
	// NIC exists
	if ( nic_ret ){
		Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 4, "NIC is not set.", __FILE__, __LINE__ );
		// Status Set
		replication_state.service_status = REPLICATION_SINGLE;
		return -1;
	}
	// Interval exists
	if ( interval_ret ){
		Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 5, "Interval is not set.", __FILE__, __LINE__ );
		// Status Set
		replication_state.service_status = REPLICATION_SINGLE;
		return -1;
	}

	// Failed in the acquisition of IP
	if ( replication_info.ip_addr == "" ){
		Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 6, "Could not get IP Address.", __FILE__, __LINE__ );
		// Status Set
		replication_state.service_status = REPLICATION_SINGLE;
		return -1;
	}

	// Failed in the acquisition of Port
	if ( replication_info.service_name == "" ){
		Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 7, "Could not get Port.", __FILE__, __LINE__ );
		// Status Set
		replication_state.service_status = REPLICATION_SINGLE;
		return -1;
	}

	// Failed in the acquisition of NIC
	if ( replication_info.nic == "" ){
		Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 8, "Could not get NIC.", __FILE__, __LINE__ );
		// Status Set
		replication_state.service_status = REPLICATION_SINGLE;
		return -1;
	}

	// Variable that sets ID
	std::string key_id;
	std::string key_size;
	error_code	id_ret, size_ret;
	mutex_ptr	component_mutex;

	replication_mutex.clear();
	// Conponent exists
	// Get Component infomation
	for ( int i=0; i<CMP_MAX; i++){
		key_id = boost::io::str( boost::format( "cmponent_id_%02d" ) % i );
		key_size = boost::io::str( boost::format( "cmponent_size_%02d" ) % i );

		// ID and the Size exist
		replication_info.component_info[i].id = param.get_string( PARAM_COMP_REPLICATION, key_id, id_ret ); 
		replication_info.component_info[i].block_size = param.get_int( PARAM_COMP_REPLICATION, key_size, size_ret );
		if ( id_ret || size_ret ){
			break;
		}
		if ( replication_info.component_info[i].id == "" ){
			buf = boost::io::str( boost::format( "Could not get %s." ) % key_id );
			Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 9, buf, __FILE__, __LINE__ );
			// Status Set
			replication_state.service_status = REPLICATION_SINGLE;
			return -1;
		}

		// Count block head number
		replication_info.component_info[i].block_head = ( 0 == i ? 0 :
				( replication_info.component_info[i-1].block_head + replication_info.component_info[i-1].block_size ) );
		replication_state.total_block += replication_info.component_info[i].block_size;

		// Number of Component 
		replication_info.component_num++;

		component_mutex = mutex_ptr( new boost::mutex );
		replication_mutex.insert( std::pair<std::string, mutex_ptr>( replication_info.component_info[i].id, component_mutex ) );
	}

	// Check the Parameters value
	if ( 0 != check_parameter() ){
		Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 10, "Failed in parameter check.", __FILE__, __LINE__ );
		// Status Set
		replication_state.service_status = REPLICATION_SINGLE;
		return -1;
	}

	// The memory is 0 or less. 
	if ( 0 >= replication_state.total_block){
		// Status Set
		replication_state.service_status = REPLICATION_SINGLE;
		return 0;
	}

	replication_state.last_send_block = replication_state.total_block-1;

	// Get the Replication memory
	replication_state.replication_memory = getrpl();
	// Confirmation of Replication memory 
	if ( NULL == replication_state.replication_memory ){
		Logger::putLogError( LOG_CAT_L7VSD_SYSTEM_MEMORY, 1, "Replication memory is NULL.", __FILE__, __LINE__ );
		// Status Set
		replication_state.service_status = REPLICATION_SINGLE;
		return -1;
	}
	// Get the Components memory
	replication_state.component_memory = getcmp();
	// Confirmation of Components memory
	if ( NULL == replication_state.component_memory ){
		// free memory
		releaserpl();
		Logger::putLogError( LOG_CAT_L7VSD_SYSTEM_MEMORY, 2, "Components memory is NULL.", __FILE__, __LINE__ );
		// Status Set
		replication_state.service_status = REPLICATION_SINGLE;
		return -1;
	}

	// Memory for Switch Surface Number
	replication_state.surface_block_array_ptr = getsrf();
	if ( NULL == replication_state.surface_block_array_ptr ){
		// free memory
		releaserpl();
		releasecmp();
		Logger::putLogError( LOG_CAT_L7VSD_SYSTEM_MEMORY, 3, "Surface infomation memory is NULL.", __FILE__, __LINE__ );
		// Status Set
		replication_state.service_status = REPLICATION_SINGLE;
		return -1;
	}

	// Status Set to Slave
	if ( 0 != set_slave() ){
		// free memory
		releasesrf();
		releaserpl();
		releasecmp();
		// Status Set
		replication_state.service_status = REPLICATION_SINGLE;
		return -1;
	}

	replication_state.service_status = REPLICATION_SLAVE;

	{
		boost::mutex::scoped_lock	lock( replication_thread_mutex );

		replication_flag = WAIT;
	}
	replication_thread_ptr = thread_ptr( new boost::thread( boost::bind ( &replication::send_thread, this ) ) );

	{
		boost::mutex::scoped_lock	lock( service_thread_mutex );

		service_flag = RUNNING;
	}
	service_thread_ptr = thread_ptr( new boost::thread( boost::bind ( &replication::service_thread, this ) ) );

	buf = boost::io::str( boost::format( "Initialized in %s mode." ) % replication_mode[( int )replication_state.service_status] );
	Logger::putLogInfo( LOG_CAT_L7VSD_REPLICATION, 2, buf, __FILE__, __LINE__ );

	return 0;
}

//! Finalize Replication
void		replication::finalize(){
	Logger	logger( LOG_CAT_L7VSD_REPLICATION, 2, "replication::finalize", __FILE__, __LINE__ );

	{
		boost::mutex::scoped_lock	lock( replication_thread_mutex );

		replication_flag = EXIT;
		replication_thread_condition.notify_all();
	}
	if ( replication_thread_ptr ){
		replication_thread_ptr->join();
	}

	// Socket finalaize
	if ( replication_send_socket.is_open() ){
		replication_send_socket.close();
	}
	if ( replication_receive_socket.is_open() ){
		replication_receive_socket.close();
	}

	{
		boost::mutex::scoped_lock	lock( service_thread_mutex );

		service_flag = EXIT;
		service_thread_condition.notify_all();
	}
	if ( service_thread_ptr ){
		service_thread_ptr->join();
	}

	// Release replication memory
	releaserpl();
	// Release component memory
	releasecmp();
	// Release surface block memory
	releasesrf();

	// reset of replication_state
	replication_state.send_time = 0;
	replication_state.last_send_block = 0;
	replication_state.last_recv_block = 0;
	replication_state.total_block = 0;
	replication_state.surface_block_no = 1;

	// reset of replication_info
	replication_info.ip_addr = "";
	replication_info.service_name = "";
	replication_info.nic = "";
	replication_info.interval = 0;
	replication_info.component_num = 0;
	for ( int loop = 0; loop < CMP_MAX; loop++ ){
		replication_info.component_info[loop].id = "";
		replication_info.component_info[loop].block_head = 0;
		replication_info.component_info[loop].block_size = 0;
	}
	replication_mutex.clear();

	replication_endpoint = boost::asio::ip::udp::endpoint();
	bind_endpoint = boost::asio::ip::udp::endpoint();

	// status change
	replication_state.service_status = REPLICATION_OUT;
}

//! Switch Slave to Master
void		replication::switch_to_master(){
	Logger	logger( LOG_CAT_L7VSD_REPLICATION, 3, "replication::switch_to_master", __FILE__, __LINE__ );

	std::string buf;
	int ret;

	switch ( replication_state.service_status ){
		case REPLICATION_SLAVE:
		case REPLICATION_SLAVE_STOP:
			// Set Mastre Mode
			ret = set_master();
			if ( 0 != ret ){
				replication_state.service_status = REPLICATION_SINGLE;

				releaserpl();
				releasecmp();
				releasesrf();

				buf = boost::io::str( boost::format( "Switch to master NG. mode : %s" ) % replication_mode[( int )replication_state.service_status] );
				Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 11, buf, __FILE__, __LINE__ );
			} else {
				// Copy from compornent area to replication area.
				memcpy( replication_state.replication_memory, replication_state.component_memory, replication_state.total_block*DATA_SIZE );

				if ( REPLICATION_SLAVE == replication_state.service_status ){
					// Set mode.
					replication_state.service_status = REPLICATION_MASTER;

					{
						boost::mutex::scoped_lock	lock( replication_thread_mutex );
						if	( replication_flag != EXIT ){
							 replication_flag = RUNNING;
						}
						replication_thread_condition.notify_all();
					}
				} else {
				// Set mode.
					replication_state.service_status = REPLICATION_MASTER_STOP;
				}

				buf = boost::io::str( boost::format( "Switch to master OK. mode : %s" ) % replication_mode[( int )replication_state.service_status] );
				Logger::putLogInfo( LOG_CAT_L7VSD_REPLICATION, 3, buf, __FILE__, __LINE__ );
			}
			break;
		case REPLICATION_SINGLE:
			buf = boost::io::str( boost::format( "Starting by %s, doesn't shift to MASTER." ) % replication_mode[( int )replication_state.service_status] ); 
			Logger::putLogInfo( LOG_CAT_L7VSD_REPLICATION, 4, buf, __FILE__, __LINE__ );
			break;
		default:
			buf = boost::io::str( boost::format( "Can not switch to master. mode : %s" ) % replication_mode[( int )replication_state.service_status] );
			Logger::putLogWarn( LOG_CAT_L7VSD_REPLICATION, 1, buf, __FILE__, __LINE__ );
			break;
	}
}

//! Set Master mode
int		replication::set_master()
{
	std::string buf;
	boost::system::error_code err;

	// close socket
	if ( replication_send_socket.is_open() ){
		replication_send_socket.close();
	}
	if ( replication_receive_socket.is_open() ){
		replication_receive_socket.cancel();
		replication_receive_socket.close();
	}

	{
		boost::mutex::scoped_lock	lock( service_thread_mutex );

		if	( service_flag == RUNNING ){
			service_flag = WAIT_REQ;
			service_io.stop();
		}
	}

	while ( service_flag == WAIT_REQ ){
		usleep(	1 );
	}

	{
		boost::mutex::scoped_lock	lock( service_thread_mutex );

		if	( service_flag != EXIT ){
			service_io.reset();
			service_flag = RUNNING;

			service_thread_condition.notify_all();
		}
	}

	Logger::putLogInfo( LOG_CAT_L7VSD_REPLICATION, 5, "Initialization of send socket is success.", __FILE__, __LINE__ );

	return 0;

}

//! Switch Master to Slave
void		replication::switch_to_slave(){
	Logger	logger( LOG_CAT_L7VSD_REPLICATION, 4, "replication::switch_to_slave", __FILE__, __LINE__ );

	std::string buf;
	int ret;

	switch ( replication_state.service_status ){
		case REPLICATION_MASTER:
		case REPLICATION_MASTER_STOP:

			{
				boost::mutex::scoped_lock	lock( replication_thread_mutex );
				if	( replication_flag != EXIT ){
					 replication_flag = WAIT;
				}
			}

			// Set Mastre Mode
			ret = set_slave();
			if ( 0 != ret ){
				replication_state.service_status = REPLICATION_SINGLE;

				releaserpl();
				releasecmp();
				releasesrf();

				buf = boost::io::str( boost::format( "Switch to slave NG. mode : %s" ) % replication_mode[( int )replication_state.service_status] );
				Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 12, buf, __FILE__, __LINE__ );
			} else {
				// initialize to replication area.
				memset( replication_state.replication_memory, '\0', replication_state.total_block * DATA_SIZE );

				if ( REPLICATION_MASTER == replication_state.service_status ){
					// Set mode.
					replication_state.service_status = REPLICATION_SLAVE;
				} else {
					// Set mode.
					replication_state.service_status = REPLICATION_SLAVE_STOP;
				}

				buf = boost::io::str( boost::format( "Switch to slave OK. mode : %s" ) % replication_mode[( int )replication_state.service_status] );
				Logger::putLogInfo( LOG_CAT_L7VSD_REPLICATION, 6, buf, __FILE__, __LINE__ );
			}
			break;
		case REPLICATION_SINGLE:
			buf = boost::io::str( boost::format( "Starting by %s, doesn't shift to SLAVE." ) % replication_mode[( int )replication_state.service_status] );
			Logger::putLogInfo( LOG_CAT_L7VSD_REPLICATION, 7, buf, __FILE__, __LINE__ );
			break;
		default:
			buf = boost::io::str( boost::format( "Can not switch to slave. mode : %s" ) % replication_mode[( int )replication_state.service_status] );
			Logger::putLogWarn( LOG_CAT_L7VSD_REPLICATION, 2, buf, __FILE__, __LINE__ );
			break;
	}
}

//! Set Slave mode
int		replication::set_slave()
{
	std::string buf;
	boost::system::error_code err;

	// close socket
	if ( replication_send_socket.is_open() ){
		replication_send_socket.close();
	}
	if ( replication_receive_socket.is_open() ){
		replication_receive_socket.cancel();
		replication_receive_socket.close();
	}

	{
		boost::mutex::scoped_lock	lock( service_thread_mutex );

		if	( service_flag != EXIT ){
			service_flag = WAIT_REQ;
			service_io.stop();
		}
	}
	while ( service_flag == WAIT_REQ ){
		usleep(	1 );
	}

	// make receive socket
//std::cout << "slave " << bind_endpoint.address() << ":" << bind_endpoint.port() << "\n";
	replication_receive_socket.open( bind_endpoint.protocol(), err );
	if ( err ){
		Logger::putLogError( LOG_CAT_L7VSD_SYSTEM, 1, err.message(), __FILE__, __LINE__ );
		return -1;
	}
	replication_receive_socket.bind( bind_endpoint, err );
	if ( err ){
		Logger::putLogError( LOG_CAT_L7VSD_SYSTEM, 2, err.message(), __FILE__, __LINE__ );
		return -1;
	}

//std::cout << "slave " << bind_endpoint.address() << ":" << bind_endpoint.port() << "\n";
	replication_receive_socket.async_receive_from( boost::asio::buffer( &replication_data, sizeof( struct replication_data_struct ) ),
											bind_endpoint,
											boost::bind( &replication::handle_receive, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred ) );

	{
		boost::mutex::scoped_lock	lock( service_thread_mutex );

		if	( service_flag != EXIT ){
			service_io.reset();
			service_flag = RUNNING;

			service_thread_condition.notify_all();
		}
	}
	Logger::putLogInfo( LOG_CAT_L7VSD_REPLICATION, 8, "Initialization of receive socket is success.", __FILE__, __LINE__ );

	return 0;
}

//! Pay replication memory 
//! @param[in] component_id is the one to identify the component.
//! @param[out] size of component use blocks
//! @return Replication memory address
//! @retval nonnull Replication memory address
//! @retval NULL Error
void*		replication::pay_memory( const std::string& inid, unsigned int& outsize ){
	Logger	logger( LOG_CAT_L7VSD_REPLICATION, 5, "replication::pay_memory", __FILE__, __LINE__ );

	void *ret = NULL;
	std::string buf;

	outsize = 0;
	// Check replication mode.
	if ( REPLICATION_OUT == replication_state.service_status || REPLICATION_SINGLE == replication_state.service_status){
		// Check mode that can use the replication.
		buf = boost::io::str( boost::format( "Improper mode for Replication. mode : %s" ) % replication_mode[( int )replication_state.service_status] );
		Logger::putLogInfo( LOG_CAT_L7VSD_REPLICATION, 9, buf, __FILE__, __LINE__ );
		return NULL;
	}

	if ( NULL == replication_state.component_memory ){
		Logger::putLogError( LOG_CAT_L7VSD_SYSTEM_MEMORY, 4, "You can't get memory. Component memory is NULL.", __FILE__, __LINE__ );
		return NULL;
	}

	// search Component ID
	for ( int i = 0; i < replication_info.component_num; i++ ){
		// Demand ID is compared with Component ID
		if ( inid == replication_info.component_info[i].id ){
			// size check
			if ( 0 == replication_info.component_info[i].block_size ){
				Logger::putLogWarn( LOG_CAT_L7VSD_REPLICATION, 3, "Component block size is 0.", __FILE__, __LINE__ );
				return NULL;
			}

			// block_head check
			if ( replication_info.component_info[i].block_head < 0 || replication_info.component_info[i].block_head > replication_state.total_block ){
				buf = boost::io::str( boost::format( "Too many component block. Max is %d." ) % replication_state.total_block );
				Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 13, buf, __FILE__, __LINE__ );
				return NULL;
			}
			// Pay memory address
			ret = ( char * )replication_state.component_memory + replication_info.component_info[i].block_head * DATA_SIZE;
			// Nnumber of blocks of ID was returned.
			outsize = replication_info.component_info[i].block_size;

			// LOG INFO
			char	str[256];
			sprintf( str, "Component Info ID : \"%s\". Block size : %d . Head Block No : %d/  Pay memory : %p ",
												replication_info.component_info[i].id.c_str(),
												replication_info.component_info[i].block_size,
												replication_info.component_info[i].block_head,
												( char* )ret );
			Logger::putLogInfo( LOG_CAT_L7VSD_REPLICATION, 10, str, __FILE__, __LINE__ );
			return ret;
		}
	}

	buf = boost::io::str( boost::format( "Unknown component ID. Component ID : %s" ) % inid );
	Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 14, buf, __FILE__, __LINE__ );

	return NULL;
}

//! Replication Dump
void		replication::dump_memory(){
	Logger	logger( LOG_CAT_L7VSD_REPLICATION, 6, "replication::dump_memory", __FILE__, __LINE__ );

	int size;
	unsigned char* p;
	unsigned char* head;
	int h = 0;
	int i = 0;
	std::string	buf;

	// Check replication mode.
	if ( REPLICATION_OUT == replication_state.service_status || REPLICATION_SINGLE == replication_state.service_status){
		buf = boost::io::str( boost::format( "Replication memory dump failure. mode : %s" ) % replication_mode[( int )replication_state.service_status] );
		Logger::putLogWarn( LOG_CAT_L7VSD_REPLICATION, 4, buf, __FILE__, __LINE__ );
		return;
	}

	if ( NULL == replication_state.replication_memory ){
		Logger::putLogWarn( LOG_CAT_L7VSD_SYSTEM_MEMORY, 1, "Replication memory is NULL.", __FILE__, __LINE__ );
		return;
	}

	// Dump size
	size = DATA_SIZE * replication_state.total_block;
	if ( 0 == size ){
		Logger::putLogWarn( LOG_CAT_L7VSD_REPLICATION, 5, "Can not get Replication memory", __FILE__, __LINE__ );
		return;
	}

	// Memory Dump
	p = ( unsigned char* )replication_state.replication_memory;

	// Output mode
	Logger::putLogInfo( LOG_CAT_L7VSD_REPLICATION, 11, "Replication Dump Start ----------------------------", __FILE__, __LINE__ );
	buf = boost::io::str( boost::format( "Mode is [ %s ]." ) % replication_mode[( int )replication_state.service_status] );
	Logger::putLogInfo( LOG_CAT_L7VSD_REPLICATION, 12, buf, __FILE__, __LINE__ );
	buf = boost::io::str( boost::format( "Total Block is [ %u ]" ) % replication_state.total_block );
	Logger::putLogInfo( LOG_CAT_L7VSD_REPLICATION, 13, buf, __FILE__, __LINE__ );

	// Converts into the binary, and writes it to the file. 
	for ( h = 0; h < size / DATA_SIZE; h++ ){
		buf = boost::io::str( boost::format( "Block Number [ %d ]" ) % h );
		Logger::putLogInfo( LOG_CAT_L7VSD_REPLICATION, 14, buf, __FILE__, __LINE__ );

		for ( i = 0; i < DATA_SIZE / LOG_DATA_WIDTH; i++ ){
			head = p + h * DATA_SIZE + i * LOG_DATA_WIDTH;

			// have to cast char to int. because boost::format ignore char with appointed width.
			buf = boost::io::str( boost::format(	"%02hhX %02hhX %02hhX %02hhX  %02hhX %02hhX %02hhX %02hhX  "
													"%02hhX %02hhX %02hhX %02hhX  %02hhX %02hhX %02hhX %02hhX" )
													% ( int )*head % ( int )*(head+1) % ( int )*(head+2) % ( int )*(head+3)
													% ( int )*(head+4) % ( int )*(head+5) % ( int )*(head+6)% ( int )*(head+7)
													% ( int )*(head+8) % ( int )*(head+9) % ( int )*(head+10) % ( int )*(head+11)
													% ( int )*(head+12) % ( int )*(head+13) % ( int )*(head+14) % ( int )*(head+15) );
			Logger::putLogInfo( LOG_CAT_L7VSD_REPLICATION, 15, buf, __FILE__, __LINE__ );
		}
	}
	Logger::putLogInfo( LOG_CAT_L7VSD_REPLICATION, 16, "Replication Dump End ------------------------------", __FILE__, __LINE__ );
}

//! Chenge Status isActive
void		replication::start(){
	Logger	logger( LOG_CAT_L7VSD_REPLICATION, 7, "replication::start", __FILE__, __LINE__ );

	std::string buf;

	switch ( replication_state.service_status ){
		case REPLICATION_MASTER_STOP:
			replication_state.service_status = REPLICATION_MASTER;

			{
				boost::mutex::scoped_lock	lock( replication_thread_mutex );
				if	( replication_flag != EXIT ){
					 replication_flag = RUNNING;
				}
				replication_thread_condition.notify_all();
			}

			buf = boost::io::str( boost::format( "Replication start. mode : %s" ) % replication_mode[( int )replication_state.service_status] );
			Logger::putLogInfo( LOG_CAT_L7VSD_REPLICATION, 17, buf, __FILE__, __LINE__ );
			break;
		case REPLICATION_SLAVE_STOP:
			replication_state.service_status = REPLICATION_SLAVE;
			buf = boost::io::str( boost::format( "Replication start. mode : %s" ) % replication_mode[( int )replication_state.service_status] );
			Logger::putLogInfo( LOG_CAT_L7VSD_REPLICATION, 18, buf, __FILE__, __LINE__ );
			break;
		case REPLICATION_MASTER:
			buf = boost::io::str( boost::format( "Could not MASTER start, because already start. mode : %s" ) % replication_mode[( int )replication_state.service_status] );
			Logger::putLogWarn( LOG_CAT_L7VSD_REPLICATION, 6, buf, __FILE__, __LINE__ );
			break;
		case REPLICATION_SLAVE:
			buf = boost::io::str( boost::format( "Could not SALVE start, because already start. mode : %s" ) % replication_mode[( int )replication_state.service_status] );
			Logger::putLogWarn( LOG_CAT_L7VSD_REPLICATION, 7, buf, __FILE__, __LINE__ );
			break;
		default:
			buf = boost::io::str( boost::format( "Could not start, because mode is %s." ) % replication_mode[( int )replication_state.service_status] );
			Logger::putLogWarn( LOG_CAT_L7VSD_REPLICATION, 8, buf, __FILE__, __LINE__ );
			break;
	}
}

//! Chenge Status isStop
void		replication::stop(){
	Logger	logger( LOG_CAT_L7VSD_REPLICATION, 8, "replication::stop", __FILE__, __LINE__ );

	std::string buf;

	switch ( replication_state.service_status ){
		case REPLICATION_MASTER:

			{
				boost::mutex::scoped_lock	lock( replication_thread_mutex );
				if	( replication_flag != EXIT ){
					 replication_flag = WAIT;
				}
			}

			replication_state.service_status = REPLICATION_MASTER_STOP;
			buf = boost::io::str( boost::format( "Replication stop. mode : %s" ) % replication_mode[( int )replication_state.service_status] );
			Logger::putLogInfo( LOG_CAT_L7VSD_REPLICATION, 19, buf, __FILE__, __LINE__ );
			break;
		case REPLICATION_SLAVE:
			replication_state.service_status = REPLICATION_SLAVE_STOP;
			buf = boost::io::str( boost::format( "Replication stop. mode : %s" ) % replication_mode[( int )replication_state.service_status] );
			Logger::putLogInfo( LOG_CAT_L7VSD_REPLICATION, 20, buf, __FILE__, __LINE__ );
			break;
		case REPLICATION_MASTER_STOP:
			buf = boost::io::str( boost::format( "Could not MASTER stop, because already stop. mode : %s" ) % replication_mode[( int )replication_state.service_status] );
			Logger::putLogWarn( LOG_CAT_L7VSD_REPLICATION, 9, buf, __FILE__, __LINE__ );
			break;
		case REPLICATION_SLAVE_STOP:
			buf = boost::io::str( boost::format( "Could not SALVE stop, because already stop. mode : %s" ) % replication_mode[( int )replication_state.service_status] );
			Logger::putLogWarn( LOG_CAT_L7VSD_REPLICATION, 10, buf, __FILE__, __LINE__ );
			break;
		default:
			buf = boost::io::str( boost::format( "Could not start, because mode is %s." ) % replication_mode[( int )replication_state.service_status] );
			Logger::putLogWarn( LOG_CAT_L7VSD_REPLICATION, 11, buf, __FILE__, __LINE__ );
			break;
	}
}

//! Compulsion reproduction execution
void		replication::force_replicate(){
	Logger	logger( LOG_CAT_L7VSD_REPLICATION, 9, "replication::force_replicate", __FILE__, __LINE__ );

	int					send_ret = -1;
	error_code			interval_ret;
	int					ms_time = 0;
	struct timespec		time;
	Parameter			param;

	std::string	buf;

	// Check by continuous initialize.
	if ( REPLICATION_MASTER != replication_state.service_status && REPLICATION_MASTER_STOP != replication_state.service_status ){
		// Initialization has already been done.
					buf = boost::io::str( boost::format( "Could not compulsion replication. Mode is different. mode : %s" ) % replication_mode[( int )replication_state.service_status] );
		Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 15, buf, __FILE__, __LINE__ );
		return;
	} else if ( REPLICATION_MASTER_STOP == replication_state.service_status ){
					buf = boost::io::str( boost::format( "Can not replication compulsorily, because mode is %s." ) % replication_mode[( int )replication_state.service_status] );
		Logger::putLogInfo( LOG_CAT_L7VSD_REPLICATION, 21, buf, __FILE__, __LINE__ );
		return;
	}

	// Replication memory is NULL
	if ( NULL == replication_state.replication_memory ){
		Logger::putLogError( LOG_CAT_L7VSD_SYSTEM_MEMORY, 5, "Replication memory is NULL.", __FILE__, __LINE__ );
		return;
	}

	// Component memory is NULL
	if ( NULL == replication_state.component_memory ){
		Logger::putLogError( LOG_CAT_L7VSD_SYSTEM_MEMORY, 6, "Component memory is NULL.", __FILE__, __LINE__ );
		return;
	}

	// set send interval
	time.tv_sec = 0;
	time.tv_nsec = 0;
	ms_time = param.get_int( PARAM_COMP_REPLICATION, "compulsorily_interval", interval_ret );

	if ( interval_ret ){
		Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 16, "Could not get interval value for replication compulsorily.", __FILE__, __LINE__ );
		return;
	}

	if ( ms_time < MIN_COMPULSORILRY_INTERVAL || MAX_COMPULSORILRY_INTERVAL < ms_time ){
		Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 17, "Invalid compulsorily interval.", __FILE__, __LINE__ );
		return;
	}

	// Thread stop
	{
		boost::mutex::scoped_lock	lock( replication_thread_mutex );
		if	( replication_flag != EXIT ){
			 replication_flag = WAIT;
		}
	}

	time.tv_nsec =  ( long )( ms_time * 1000000 );

	// set last send block is c maximum block
	replication_state.last_send_block = replication_state.total_block-1;

	for ( unsigned int i = 0; i < replication_state.total_block; i++ ){
		// set compulsorily interval.
		nanosleep( &time, NULL );

		send_ret = send_data();

		if ( 0 != send_ret ){
			Logger::putLogError( LOG_CAT_L7VSD_SYSTEM, 3, "Send data is Failed.", __FILE__, __LINE__ );
			goto END;
		}

		// set last send block number
		if ( replication_state.last_send_block < replication_state.total_block-1 ){
			replication_state.last_send_block += 1;
		} else if ( replication_state.last_send_block == replication_state.total_block-1 ){
			replication_state.last_send_block = 0;
		} else {
			Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 18, "Last send block number is illegal.", __FILE__, __LINE__ );
			goto END;
		}
		buf = boost::io::str( boost::format( "Data sending succeeded. Send block number : %u Version : %llu" ) % replication_state.last_send_block % (unsigned long long)replication_state.surface_block_no );
		Logger::putLogInfo( LOG_CAT_L7VSD_REPLICATION, 22, buf, __FILE__, __LINE__ );

		// surface block number is change
		if ( replication_state.total_block == replication_state.last_send_block + 1 ){
			// Synchronization is executed. 
			memcpy( replication_state.replication_memory, replication_state.component_memory, replication_state.total_block*DATA_SIZE );

			// make new serial
			replication_state.surface_block_no = (uint64_t)make_serial();
			if ( 0 == replication_state.surface_block_no ){
				Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 19, "Could not get serial number.", __FILE__, __LINE__ );
				goto END;
			}
		}
	}

	Logger::putLogInfo( LOG_CAT_L7VSD_REPLICATION, 23, "Replication compulsorily is success.", __FILE__, __LINE__ );

END:
	// Thread rusume
	{
		boost::mutex::scoped_lock	lock( replication_thread_mutex );
		if	( replication_flag != EXIT ){
			 replication_flag = RUNNING;
		}
		replication_thread_condition.notify_all();
	}
}

//! Interval Re-setting
void		replication::reset(){
	Logger	logger( LOG_CAT_L7VSD_REPLICATION, 10, "replication::reset", __FILE__, __LINE__ );

	error_code 			ret;
	unsigned short		value;
	std::string			buf;
	Parameter			param;

	// Check Parameter exists
	value = param.get_int( PARAM_COMP_REPLICATION, "interval", ret );
	if ( ret ){
		Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 20, "Not change re-setting value.", __FILE__, __LINE__ );
		return;
	}

	// Check interval
	if ( value < MIN_INTERVAL || MAX_INTERVAL < value ){
		buf = boost::io::str( boost::format( "Invalid Interval value. value : %d" ) % ( int )value );
		Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 21, buf, __FILE__, __LINE__ );
		return;
	}
	//set interval
	replication_info.interval =  value;
}

//! Get Status
//! @return REPLICATION_MODE_TAG enumration
replication::REPLICATION_MODE_TAG	replication::get_status(){
	Logger	logger( LOG_CAT_L7VSD_REPLICATION, 11, "replication::get_status", __FILE__, __LINE__ );

	return replication_state.service_status;
}

//! Send function
int			replication::handle_send(){
	Logger	logger( LOG_CAT_L7VSD_REPLICATION, 12, "replication::handle_send", __FILE__, __LINE__ );

	int send_ret = -1;
	std::string buf;
	std::map<std::string, mutex_ptr>::iterator	itr;

	// Check by continuous initialize.
	if ( REPLICATION_MASTER != replication_state.service_status && REPLICATION_MASTER_STOP != replication_state.service_status ){
		// Initialization has already been done.
		buf = boost::io::str( boost::format( "Can not send_callback. Mode is different.  mode : %s" ) % replication_mode[( int )replication_state.service_status] );
		Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 22, buf, __FILE__, __LINE__ );
		return -1;
	} else if ( REPLICATION_MASTER_STOP == replication_state.service_status ){ 
		Logger::putLogInfo( LOG_CAT_L7VSD_REPLICATION, 24, "Can not send Replication data, because mode is MASTER_STOP.", __FILE__, __LINE__ );
		return 0;
	}

	// Replication memory is NULL
	if ( NULL == replication_state.replication_memory ){
		Logger::putLogError( LOG_CAT_L7VSD_SYSTEM_MEMORY, 7, "Replication memory is NULL.", __FILE__, __LINE__ );
		return -1;
	}

	// Component memory is NULL
	if ( NULL == replication_state.component_memory ){
		Logger::putLogError( LOG_CAT_L7VSD_SYSTEM_MEMORY, 8, "Component memory is NULL.", __FILE__, __LINE__ );
		return -1;
	}

	send_ret = send_data();
	if ( 0 != send_ret ){
		Logger::putLogError( LOG_CAT_L7VSD_SYSTEM, 4, "Send data is Failed.", __FILE__, __LINE__ );
		return -1;
	}

	// set last send block number
	if ( replication_state.last_send_block < replication_state.total_block-1 )	{
		replication_state.last_send_block += 1;
	} else if ( replication_state.last_send_block == replication_state.total_block-1 ){
		replication_state.last_send_block = 0;
	} else {
		Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 23, "Last send block number is illegal.", __FILE__, __LINE__ );
		return -1;
	}

	// surface block number is change
	if ( replication_state.total_block == replication_state.last_send_block + 1 ){
		// Synchronization is executed. 
		memcpy( replication_state.replication_memory, replication_state.component_memory, replication_state.total_block*DATA_SIZE );

		// make new serial
		replication_state.surface_block_no = (uint64_t)make_serial();
		if ( 0 == replication_state.surface_block_no ){
			Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 24, "Could not get serial number .", __FILE__, __LINE__ );
			return -1;
		}
	}

	return 0;
}

//! Callback function
void		replication::handle_receive( const boost::system::error_code& err, size_t size ){
	int recv_ret;
	std::string buf;

	if ( err ){
		if ( boost::system::errc::operation_canceled != err.value() ){
			Logger::putLogInfo( LOG_CAT_L7VSD_SYSTEM, 1, err.message(), __FILE__, __LINE__ );
		}
		return;
	}

	// Check by continuous initialize.
	if ( REPLICATION_SLAVE != replication_state.service_status && REPLICATION_SLAVE_STOP != replication_state.service_status ){
		// Initialization has already been done.
		buf = boost::io::str( boost::format( "Can not receive_callback. Mode is different.  mode : %s" ) % replication_mode[( int )replication_state.service_status] );
		Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 25, buf, __FILE__, __LINE__ );
		return;
	} else if ( REPLICATION_SLAVE_STOP == replication_state.service_status ){
		Logger::putLogInfo( LOG_CAT_L7VSD_REPLICATION, 25, "Can not receive Replication data, because mode is SLAVE_STOP.", __FILE__, __LINE__ );
		return;
	}

	// Replication memory is NULL
	if ( NULL == replication_state.replication_memory ){
		Logger::putLogError( LOG_CAT_L7VSD_SYSTEM_MEMORY, 9, "Replication memory is NULL.", __FILE__, __LINE__ );
		return;
	}

	// Component memory is NULL
	if ( NULL == replication_state.component_memory ){
		Logger::putLogError( LOG_CAT_L7VSD_SYSTEM_MEMORY, 10, "Component memory is NULL.", __FILE__, __LINE__ );
		return;
	}

	// Surface block array memory is NULL
	if ( NULL == replication_state.surface_block_array_ptr){
		Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 26, "Surface block array pointer is NULL.", __FILE__, __LINE__ );
		return;
	}

	if ( size != sizeof ( struct replication_data_struct ) ){
		Logger::putLogError( LOG_CAT_L7VSD_SYSTEM, 5, "Failed in the reception processing of data because of illegal receive size.", __FILE__, __LINE__ );
		return;
	}

	recv_ret = recv_data();
	if ( 0 != recv_ret ){
		Logger::putLogError( LOG_CAT_L7VSD_SYSTEM, 6, "Failed in the reception processing of data because of illegal receive data.", __FILE__, __LINE__ );
		return;
	}

	// set surface block
	replication_state.surface_block_array_ptr[replication_data.block_num] = replication_data.serial;

	// set last recv block number
	if ( replication_state.last_recv_block < replication_state.total_block-1 ){
		replication_state.last_recv_block += 1;
	} else if ( replication_state.last_recv_block == replication_state.total_block-1 ){
		replication_state.last_recv_block = 0;
	} else {
		Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 27, "Last receive block number is illegal.", __FILE__, __LINE__ );
		return;
	}

//std::cout << "slave " << bind_endpoint.address() << ":" << bind_endpoint.port() << "\n";
//	replication_receive_socket.async_receive( boost::asio::buffer( &replication_data, sizeof( struct replication_data_struct ) ),
//											boost::bind( &replication::handle_receive, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred ) );
	replication_receive_socket.async_receive_from( boost::asio::buffer( &replication_data, sizeof( struct replication_data_struct ) ),
											bind_endpoint,
											boost::bind( &replication::handle_receive, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred ) );
}

//! Lock replication memory
//! @param[in] component_id is the one to identify the component.
//! @retval 0 Success
//! @retval -1 Error
int			replication::lock( const std::string& inid ){
	Logger	logger( LOG_CAT_L7VSD_REPLICATION, 13, "replication::lock", __FILE__, __LINE__ );

	std::map<std::string, mutex_ptr>::iterator	itr;
	std::string buf;

	itr = replication_mutex.find( inid );
	if ( itr == replication_mutex.end() ){
		buf = boost::io::str( boost::format( "Could not find %s." ) % inid );
		Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 28, buf, __FILE__, __LINE__ );
		return -1;
	}

	itr->second->lock();

	return 0;
}

//! Unlock replication memory
//! @param[in] component_id is the one to identify the component.
//! @retval 0 Success
//! @retval -1 Error
int			replication::unlock( const std::string& inid ){
	Logger	logger( LOG_CAT_L7VSD_REPLICATION, 14, "replication::unlock", __FILE__, __LINE__ );

	std::map<std::string, mutex_ptr>::iterator	itr;
	std::string buf;

	itr = replication_mutex.find( inid );
	if ( itr == replication_mutex.end() ){
		buf = boost::io::str( boost::format( "Could not find %s." ) % inid );
		Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 29, buf, __FILE__, __LINE__ );
		return -1;
	}

	itr->second->try_lock();
	itr->second->unlock();

	return 0;
}

//! Refer replication memory mutex
//! @param[in] component_id is the one to identify the component.
//! @param[out] shared_ptr of mutex
//! @retval 0 Success
//! @retval -1 Error
int			replication::refer_lock_mutex( const std::string& inid, mutex_ptr& outmutex ){
	Logger	logger( LOG_CAT_L7VSD_REPLICATION, 15, "replication::refer_lock_mutex", __FILE__, __LINE__ );

	std::map<std::string, mutex_ptr>::iterator	itr;
	std::string buf;

	itr = replication_mutex.find( inid );
	if ( itr == replication_mutex.end() ){
		buf = boost::io::str( boost::format( "Could not find %s." ) % inid );
		Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 30, buf, __FILE__, __LINE__ );
		return -1;
	}

	outmutex = itr->second;

	return 0;
}

//! Parameter Check
//! @retval 0 Success
//! @retval -1 Error
int			replication::check_parameter(){

	int ret = -1;
	size_t sum=0;
	std::string buf;

//std::cout << "check1 " << replication_info.ip_addr << ":" << replication_info.service_name << "\n";
	// Whether IP and the port are effective is confirmed.
	try{
//		replication_endpoint = boost::asio::ip::udp::endpoint( boost::asio::ip::address::from_string( replication_info.ip_addr ), boost::lexical_cast<unsigned short>( replication_info.service_name ) );

		boost::asio::ip::udp::resolver				udp_resolver( service_io );
		boost::asio::ip::udp::resolver::query		udp_query( replication_info.ip_addr, replication_info.service_name );
		boost::asio::ip::udp::resolver::iterator	itr = udp_resolver.resolve( udp_query );
		replication_endpoint = *itr;
	}
	catch(...){
		buf = boost::io::str( boost::format( "Failed to get IP or Service Name.(%s:%s)" ) % replication_info.ip_addr % replication_info.service_name );
		Logger::putLogError( LOG_CAT_L7VSD_SYSTEM_ENDPOINT, 1, buf, __FILE__, __LINE__ );
		goto END;
	}
//std::cout << "check2 " << replication_endpoint.address() << ":" << replication_endpoint.port() << "\n";

	// get ip address from nic
	try{
		struct sockaddr_in addr;

		//Networkdevice struct define
		struct ifreq ifr;
		memset( &ifr, 0, sizeof( struct ifreq ) );

		//create socket
		int fd  = socket( AF_INET, SOCK_DGRAM, 0 );
		if ( fd >= 0 ){
			//get networkdevice struct for IPv4
			strncpy( ifr.ifr_name, replication_info.nic.c_str(), IFNAMSIZ-1 );
			ifr.ifr_addr.sa_family = AF_INET;

			if ( ioctl( fd, SIOCGIFADDR, &ifr ) >= 0 ){
				memcpy( &addr, &(ifr.ifr_addr), sizeof( struct sockaddr_in ) );
			}

			close( fd );
		}

		bind_endpoint = boost::asio::ip::udp::endpoint( boost::asio::ip::address::from_string( inet_ntoa( addr.sin_addr ) ), boost::lexical_cast<unsigned short>( replication_info.service_name ) );
	}
	catch(...){
		Logger::putLogError( LOG_CAT_L7VSD_SYSTEM_ENDPOINT, 2, "You can not get IP address from nic.", __FILE__, __LINE__ );
		goto END;
	}
//std::cout << "check3 " << bind_endpoint.address() << ":" << bind_endpoint.port() << "\n";

	// Interval check
	if ( ( MIN_INTERVAL>replication_info.interval ) || ( MAX_INTERVAL<replication_info.interval ) ){
		Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 31, "Invalid Interval value", __FILE__, __LINE__ );
		goto END;
	}
	// Components ID check
	// Components Size check
	if ( 0 == replication_info.component_num ){
		Logger::putLogInfo( LOG_CAT_L7VSD_REPLICATION, 26, "Can not get component, because component is 0.", __FILE__, __LINE__ );
		ret = 0;
		goto END;
	}
	for ( int i=0; i < replication_info.component_num; i++ ){
		sum += replication_info.component_info[i].block_size ;
		for ( int j=i+1; j<replication_info.component_num; j++ ){
			if ( replication_info.component_info[j].id == replication_info.component_info[i].id ){
				buf = boost::io::str( boost::format( "Component ID was repeated.(%s)" ) % replication_info.component_info[i].id );
				Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 32, buf, __FILE__, __LINE__ );
				goto END;
			}
		}
	}
	if ( sum > CMP_BLOCK_MAX ){
		Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 33, "Total component size is too large.", __FILE__, __LINE__ );
		goto END;
	}
	ret = 0;
END:

	Logger::putLogInfo( LOG_CAT_L7VSD_REPLICATION, 27, ( ( 0 == ret ) ? "Parameter Check OK." :  "Parameter Check NG." ), __FILE__, __LINE__ );
	return ret;
}



//! Get Replication Memory
//! @return memory Replication memory
//! @retval memory memory get Success
//! @retval NULL Error
void*		replication::getrpl(){
	unsigned int total_block = replication_state.total_block;
	void*	memory = NULL;

	// Get replication memory
	memory = malloc( total_block*DATA_SIZE );

	// malloc Error
	if ( ( void* )NULL == memory ){
		Logger::putLogError( LOG_CAT_L7VSD_SYSTEM_MEMORY, 11, "Replication memory is Malloc Error.", __FILE__, __LINE__ );
		return NULL;
	}
	memset(memory,0,total_block*DATA_SIZE);

	return memory;
}

//! Get Component Memory
//! @return memory Component memory
//! @retval memory memory get Success
//! @retval NULL Error
void*		replication::getcmp(){
	unsigned int total_block = replication_state.total_block;
	void*	memory = NULL ;

	// Get component memory
	memory = malloc( total_block*DATA_SIZE );

	// malloc Error
	if ( ( void* )NULL == memory ){
		Logger::putLogError( LOG_CAT_L7VSD_SYSTEM_MEMORY, 12, "Component memory is Malloc Error.", __FILE__, __LINE__ );
		return NULL;
	}
	memset(memory,0,total_block*DATA_SIZE);

	return memory;
}

//! Get Surface Number Memory
//! @return memory Component memory
//! @retval memory memory get Success
//! @retval NULL Error
uint64_t*	replication::getsrf(){
	unsigned int total_block = replication_state.total_block;
	uint64_t*	memory = NULL;

	// Get memory
	memory = ( uint64_t* )malloc( total_block*sizeof(uint64_t) );

	// malloc Error
	if ( ( uint64_t* )NULL == memory ){
		Logger::putLogError( LOG_CAT_L7VSD_SYSTEM_MEMORY, 13, "Surface info address is Malloc Error.", __FILE__, __LINE__ );
		return NULL;
	}
	memset( memory, 0, total_block*sizeof(uint64_t) );

	return memory;
}

//! Make serial number
//! @return Serial number
//! @retval nonzero Serial number
//! @retval 0 Error
unsigned long long		replication::make_serial(){
	unsigned long long int serial_num;
	struct timespec current_time;

	// get time by clock_gettime
	if ( clock_gettime(CLOCK_REALTIME, &current_time) == -1 ){
		// failre.
		Logger::putLogError( LOG_CAT_L7VSD_SYSTEM, 7, "You failed to get of time.", __FILE__, __LINE__ );
		serial_num = 0;
	} else {
		// make a serial succeeds.
		serial_num = ( unsigned long long int )current_time.tv_sec * 1000000 + ( unsigned long long int ) current_time.tv_nsec / 1000;
	}

	return serial_num;
}

//! Send transfer data to standby server
//! @param[in] data Points to input data from external program. This will be send to standby server.
//! @retval 0 Success
//! @retval -1 Error
int			replication::send_data(){
	char*	send_memory;
	size_t send_byte;

	// make replication data struct
	//initialize
	memset( &replication_data, 0, sizeof( struct replication_data_struct ) );
	// Set replication id
	replication_data.id = REPLICATION_ID;
	// set block_num (replication_state.last_send_block + 1) and Range check of memory
	if ( replication_state.last_send_block < replication_state.total_block - 1 ){
		replication_data.block_num = replication_state.last_send_block + 1;
	} else if ( replication_state.last_send_block == replication_state.total_block - 1 ){
		replication_data.block_num = 0;
	} else {
		Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 34, "Send block number is too large.", __FILE__, __LINE__ );
		return -1;
	}

	// set serial
	replication_data.serial = replication_state.surface_block_no;
	if ( 1 == replication_data.serial &&  0 == replication_data.block_num ){
		Logger::putLogInfo( LOG_CAT_L7VSD_REPLICATION, 28, "Serial number is 1, first send processing.", __FILE__, __LINE__ );
	}

	// set data size (sizeof(replication_data))
	replication_data.size = sizeof( struct replication_data_struct );

	if ( replication_data.size > SEND_DATA_SIZE ){
		Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 35, "Send block data size is too large.", __FILE__, __LINE__ );
		return -1;
	}

	// set replication data (1 block)
	send_memory = ( char* )replication_state.replication_memory + DATA_SIZE*replication_data.block_num;
	memcpy( replication_data.data, send_memory, DATA_SIZE );

#if	0
	// send to data
	send_byte = replication_send_socket.send_to( boost::asio::buffer( &replication_data, sizeof( struct replication_data_struct ) ), replication_endpoint );
#else
	boost::system::error_code err;
	std::string buf;

	// Whether IP and the port are effective is confirmed.
	try{
//		replication_endpoint = boost::asio::ip::udp::endpoint( boost::asio::ip::address::from_string( replication_info.ip_addr ), boost::lexical_cast<unsigned short>( replication_info.service_name ) );

		boost::asio::ip::udp::resolver				udp_resolver( service_io );
		boost::asio::ip::udp::resolver::query		udp_query( replication_info.ip_addr, replication_info.service_name );
		boost::asio::ip::udp::resolver::iterator	itr = udp_resolver.resolve( udp_query );
		replication_endpoint = *itr;
	}
	catch(...){
		buf = boost::io::str( boost::format( "Failed to get IP or Service Name.(%s:%s)" ) % replication_info.ip_addr % replication_info.service_name );
		Logger::putLogError( LOG_CAT_L7VSD_SYSTEM_ENDPOINT, 3, buf, __FILE__, __LINE__ );
		return -1;
	}

//std::cout << "master " << replication_endpoint.address() << ":" << replication_endpoint.port() << "\n";
	replication_send_socket.connect( replication_endpoint, err );
	if ( err ){
		Logger::putLogError( LOG_CAT_L7VSD_SYSTEM, 8, err.message(), __FILE__, __LINE__ );
		return -1;
	}

	// send to data
	send_byte = replication_send_socket.send( boost::asio::buffer( &replication_data, sizeof( struct replication_data_struct ) ) );
	replication_send_socket.close();
#endif
	if ( sizeof( struct replication_data_struct ) != send_byte ){
		Logger::putLogError( LOG_CAT_L7VSD_SYSTEM, 9, "Data send error.", __FILE__, __LINE__ );
		return -1;
	}

	return 0;
}

//! Receive transfer data from active server
//! @param[out]  recv_data Points to output data from external program.
//! @retval 0 Success
//! @retval -1 Error
int			replication::recv_data(){
	char	*recv_memory;
	std::map<std::string, mutex_ptr>::iterator	itr;

	// Check replication ID
	if ( replication_data.id != REPLICATION_ID ){
		Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 36, "Get invalid data.", __FILE__, __LINE__ );
		return -1;
	}

	// block number is over
	if ( replication_data.block_num > replication_state.total_block ){
		Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 37, "Recv block number is too large.", __FILE__, __LINE__ );
		return -1;
	}

	// Comparison of serial numbers
	if ( replication_data.serial < replication_state.surface_block_array_ptr[replication_data.block_num] ){
		Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 38, "Recv replication data is too old.", __FILE__, __LINE__ );
		return -1;
	}

	// Substitution of version
	replication_state.surface_block_array_ptr[replication_data.block_num] = replication_data.serial;

	// set recv data
	recv_memory = ( char* )replication_state.replication_memory + DATA_SIZE * replication_data.block_num;

	// received data.
	memcpy( recv_memory, &replication_data.data, DATA_SIZE );

	// set surface block
	replication_state.surface_block_array_ptr[replication_data.block_num] = replication_data.serial;

	// Surface numbers are compared.
	for ( unsigned int i = 0; i < replication_state.total_block-1; i++ ){
		if ( replication_state.surface_block_array_ptr[i] != replication_state.surface_block_array_ptr[i+1] ){
			break;
		}
		if ( i == replication_state.total_block-2 ){
			// Lock all compornent area
			for ( itr = replication_mutex.begin(); itr != replication_mutex.end(); itr++ ){
				itr->second->lock();
			}

			// Synchronization is executed.
			memcpy(replication_state.component_memory, replication_state.replication_memory, replication_state.total_block*DATA_SIZE );

			// Unlock all compornent area
			for ( itr = replication_mutex.begin(); itr != replication_mutex.end(); itr++ ){
				itr->second->unlock();
			}
		}
	}

	return 0;
}

//! Release Replication Memory
void		replication::releaserpl(){
	if ( NULL != replication_state.replication_memory ){
		free(replication_state.replication_memory);
	}
	replication_state.replication_memory = NULL;
}

//! Release Components Memory
void		replication::releasecmp(){
	if ( NULL != replication_state.component_memory){
		free(replication_state.component_memory);
	}
	replication_state.component_memory = NULL;
}

//! Release Surface Memory
void		replication::releasesrf(){
	if ( NULL != replication_state.surface_block_array_ptr ){
		free(replication_state.surface_block_array_ptr);
	}
	replication_state.surface_block_array_ptr=NULL;
}

//! Replication thread
void		replication::send_thread(){
	bool	mode = false;
	REPLICATION_THREAD_TAG	flag;
	{
		boost::mutex::scoped_lock lock( replication_thread_mutex );
		flag = replication_flag;
	}
	for ( ; ; ){
		if ( flag == WAIT ){
			boost::mutex::scoped_lock	lock( replication_thread_mutex );
			replication_thread_condition.wait( lock );
		} else if ( flag == EXIT ){
			break;
		} else {
			if ( false == mode ){
				usleep(	replication_info.interval );
			} else {
				if ( -1 == handle_send() ){
				}
			}
			mode = !mode;
		}
		{
			boost::mutex::scoped_lock	lock( replication_thread_mutex );
			flag = replication_flag;
		}
	}
}

//! io_service thread
void		replication::service_thread(){
	REPLICATION_THREAD_TAG	flag;
	{
		boost::mutex::scoped_lock lock( service_thread_mutex );
		flag = service_flag;
	}
	for ( ; ; ){
		if ( flag == WAIT || flag == WAIT_REQ ){
			boost::mutex::scoped_lock	lock( service_thread_mutex );
			service_flag = WAIT;
			service_thread_condition.wait( lock );
		} else if ( flag == EXIT ){
			break;
		} else {
			service_io.poll();
		}
		{
			boost::mutex::scoped_lock	lock( service_thread_mutex );
			flag = service_flag;
		}
	}
}

}	//namespace l7vs
