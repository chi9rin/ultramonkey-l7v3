//
//	@file	replication.h
//	@brief	Replication class
//
//	copyright (c) xxx corporation. 2009
//	mail: 
//
//	Distributed under the Boost Software License, Version 1.0.(See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt
//
#include	<boost/lexical_cast.hpp>
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

	Parameter			param;
	error_code			ip_addr_ret, service_name_ret, nic_ret, interval_ret;
	std::stringstream	buf;

	// Check by continuous initialize.
	if ( REPLICATION_OUT != replication_state.service_status ){
		// Initialization has already been done.
		buf << "Initialization is a failure, because initialization has already been done. mode : " << replication_mode[(int)replication_state.service_status];
		Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 1, buf.str(), __FILE__, __LINE__ );
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
		Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 1, "IP Address is not set.", __FILE__, __LINE__ );
		// Status Set
		replication_state.service_status = REPLICATION_SINGLE;
		return -1;
	}
	// Port exists
	if ( service_name_ret ){
		Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 1, "Port is not set.", __FILE__, __LINE__ );
		// Status Set
		replication_state.service_status = REPLICATION_SINGLE;
		return -1;
	}
	// NIC exists
	if ( nic_ret ){
		Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 1, "NIC is not set.", __FILE__, __LINE__ );
		// Status Set
		replication_state.service_status = REPLICATION_SINGLE;
		return -1;
	}
	// Interval exists
	if ( interval_ret ){
		Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 1, "Interval is not set.", __FILE__, __LINE__ );
		// Status Set
		replication_state.service_status = REPLICATION_SINGLE;
		return -1;
	}

	// Failed in the acquisition of IP
	if ( replication_info.ip_addr == "" ){
		Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 1, "Could not get IP Address.", __FILE__, __LINE__ );
		// Status Set
		replication_state.service_status = REPLICATION_SINGLE;
		return -1;
	}

	// Failed in the acquisition of Port
	if ( replication_info.service_name == "" ){
		Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 1, "Could not get Port.", __FILE__, __LINE__ );
		// Status Set
		replication_state.service_status = REPLICATION_SINGLE;
		return -1;
	}

	// Failed in the acquisition of NIC
	if ( replication_info.nic == "" ){
		Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 1, "Could not get NIC.", __FILE__, __LINE__ );
		// Status Set
		replication_state.service_status = REPLICATION_SINGLE;
		return -1;
	}

	// Variable that sets ID
	char key_id[ID_LENGTH];
	char key_size[CMP_SIZE_LENGTH];
	error_code	id_ret, size_ret;
	mutex_ptr	component_mutex;

	replication_mutex.clear();
	// Conponent exists
	// Get Component infomation
	for ( int i=0; i<CMP_MAX; i++)
	{
		sprintf( key_id, "cmponent_id_%02d",i);
		sprintf( key_size, "cmponent_size_%02d",i);

		// ID and the Size exist
		replication_info.component_info[i].id = param.get_string( PARAM_COMP_REPLICATION, key_id, id_ret ); 
		replication_info.component_info[i].block_size = param.get_int( PARAM_COMP_REPLICATION, key_size, size_ret );
		if ( id_ret || size_ret ){
			break;
		}
		if ( replication_info.component_info[i].id == "" ){
			buf << "Could not get " << key_id << ".";
			Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 1, buf.str(), __FILE__, __LINE__ );
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
		Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 1, "Failed in parameter check.", __FILE__, __LINE__ );
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
		Logger::putLogError( LOG_CAT_L7VSD_SYSTEM_MEMORY, 1, "Components memory is NULL.", __FILE__, __LINE__ );
		// Status Set
		replication_state.service_status = REPLICATION_SINGLE;
		return -1;
	}

	// Memory for Switch Sarface Number
	replication_state.sarface_block_array_ptr = getsrf();
	if ( NULL == replication_state.sarface_block_array_ptr ){
		// free memory
		releaserpl();
		releasecmp();
		Logger::putLogError( LOG_CAT_L7VSD_SYSTEM_MEMORY, 1, "Surface infomation memory is NULL.", __FILE__, __LINE__ );
		// Status Set
		replication_state.service_status = REPLICATION_SINGLE;
		return -1;
	}

	// Status Set to Slave
	if ( 0 != set_slave() ){
//		replication_receive_socket.close();

		// free memory
		releasesrf();
		releaserpl();
		releasecmp();
		// Status Set
		replication_state.service_status = REPLICATION_SINGLE;
		return -1;
	}

	replication_state.service_status = REPLICATION_SLAVE;

	buf << "Initialized in " << replication_mode[(int)replication_state.service_status] << " mode.";
	Logger::putLogInfo( LOG_CAT_L7VSD_REPLICATION, 1, buf.str(), __FILE__, __LINE__ );

	return 0;
}

//! Finalize Replication
void		replication::finalize(){
	Logger	logger( LOG_CAT_L7VSD_REPLICATION, 1, "replication::finalize", __FILE__, __LINE__ );

	// Socket finalaize
	replication_receive_socket.close();
	replication_send_socket.close();
	// Release replication memory
	releaserpl();
	// Release component memory
	releasecmp();
	// Release sarface block memory
	releasesrf();

	// reset of replication_state
	replication_state.send_time = 0;
	replication_state.last_send_block = 0;
	replication_state.last_recv_block = 0;
	replication_state.total_block = 0;
	replication_state.sarface_block_no = 0;

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

	// status change
	replication_state.service_status = REPLICATION_OUT;
}

//! Switch Slave to Master
void		replication::switch_to_master(){
	Logger	logger( LOG_CAT_L7VSD_REPLICATION, 1, "replication::switch_to_master", __FILE__, __LINE__ );

	std::stringstream buf;
	int ret;

	switch (replication_state.service_status){
		case REPLICATION_SLAVE:
		case REPLICATION_SLAVE_STOP:
			// Set Mastre Mode
			ret = set_master();
			if ( 0 != ret ){
				replication_state.service_status = REPLICATION_SINGLE;

				releaserpl();
				releasecmp();
				releasesrf();

				buf << "Switch to master NG. mode : " << replication_mode[(int)replication_state.service_status];
				Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 1, buf.str(), __FILE__, __LINE__ );
			}else{
				// Copy from component area to replication area.
				memcpy(replication_state.replication_memory, replication_state.component_memory, replication_state.total_block*DATA_SIZE);

				if ( REPLICATION_SLAVE == replication_state.service_status ){
					// Set mode.
					replication_state.service_status = REPLICATION_MASTER;
				} else {
				// Set mode.
					replication_state.service_status = REPLICATION_MASTER_STOP;
				}

				buf << "Switch to master OK. mode : " << replication_mode[(int)replication_state.service_status];
				Logger::putLogInfo( LOG_CAT_L7VSD_REPLICATION, 1, buf.str(), __FILE__, __LINE__ );
			}
			break;
		case REPLICATION_SINGLE:
			buf << "Starting by " << replication_mode[(int)replication_state.service_status] << ", doesn't shift to MASTER.";
			Logger::putLogInfo( LOG_CAT_L7VSD_REPLICATION, 1, buf.str(), __FILE__, __LINE__ );
			break;
		default:
			buf << "Can not switch to master. mode : " << replication_mode[(int)replication_state.service_status];
			Logger::putLogWarn( LOG_CAT_L7VSD_REPLICATION, 1, buf.str(), __FILE__, __LINE__ );
			break;
	}
}

//! Set Master mode
int		replication::set_master()
{
	std::stringstream buf;

	// close socket
	replication_receive_socket.close();

	// make send socket
	// Check by continuous initialize.
	if ( REPLICATION_SINGLE == replication_state.service_status || REPLICATION_MASTER == replication_state.service_status ||
		REPLICATION_MASTER_STOP == replication_state.service_status ){
		// Initialization has already been done.
		buf << "send-socket initialization has already been done. mode : " << replication_mode[(int)replication_state.service_status];
		Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 1, buf.str(), __FILE__, __LINE__ );
		return -1;
	}

	boost::asio::ip::udp::endpoint udp_endpoint( boost::asio::ip::address::from_string( replication_info.ip_addr ), boost::lexical_cast<unsigned short>( replication_info.service_name ) );

	replication_send_socket.connect( udp_endpoint );
//	if ( -1 == replication_send_ENDPOINT.connect( udp_endpoint ) ){
//		Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 1, "Failed in the initialization of the send socket.", __FILE__, __LINE__ );
//		return -1;
//	}









	Logger::putLogInfo( LOG_CAT_L7VSD_REPLICATION, 1, "Initialization of send socket is success.", __FILE__, __LINE__ );

	return 0;

}

//! Switch Master to Slave
void		replication::switch_to_slave(){
	Logger	logger( LOG_CAT_L7VSD_REPLICATION, 1, "replication::switch_to_slave", __FILE__, __LINE__ );

	std::stringstream buf;
	int ret;

	switch (replication_state.service_status){
		case REPLICATION_MASTER:
		case REPLICATION_MASTER_STOP:
			// Set Mastre Mode
			ret = set_slave();
			if ( 0 != ret ){
				replication_state.service_status = REPLICATION_SINGLE;

				releaserpl();
				releasecmp();
				releasesrf();

				buf << "Switch to slave NG. mode : " << replication_mode[(int)replication_state.service_status];
				Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 1, buf.str(), __FILE__, __LINE__ );
			}else{
				// initialize to replication area.
				memset( replication_state.replication_memory, '\0', replication_state.total_block * DATA_SIZE );

				if ( REPLICATION_MASTER == replication_state.service_status ){
					// Set mode.
					replication_state.service_status = REPLICATION_SLAVE;
				} else {
					// Set mode.
					replication_state.service_status = REPLICATION_SLAVE_STOP;
				}

				buf << "Switch to slave OK. mode : " << replication_mode[(int)replication_state.service_status];
				Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 1, buf.str(), __FILE__, __LINE__ );
			}
			break;
		case REPLICATION_SINGLE:
			buf << "Starting by " << replication_mode[(int)replication_state.service_status] << ", doesn't shift to SLAVE.";
			Logger::putLogInfo( LOG_CAT_L7VSD_REPLICATION, 1, buf.str(), __FILE__, __LINE__ );
			break;
		default:
			buf << "Can not switch to slave. mode : " << replication_mode[(int)replication_state.service_status];
			Logger::putLogWarn( LOG_CAT_L7VSD_REPLICATION, 1, buf.str(), __FILE__, __LINE__ );
			break;
	}
}

//! Set Slave mode
int		replication::set_slave()
{
	std::stringstream buf;

	// close socket
	replication_send_socket.close();

	// make receive socket
	// Check by continuous initialize.
	if ( REPLICATION_SINGLE == replication_state.service_status || REPLICATION_SLAVE == replication_state.service_status ||
		REPLICATION_SLAVE_STOP == replication_state.service_status )	{
		// Initialization has already been done.
		buf << "recieve-socket initialization has already been done. mode : " << replication_mode[(int)replication_state.service_status];
		Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 1, buf.str(), __FILE__, __LINE__ );
		return -1;
	}

	boost::asio::ip::udp::endpoint udp_endpoint( boost::asio::ip::address::from_string( replication_info.ip_addr ), boost::lexical_cast<unsigned short>( replication_info.service_name ) );

	replication_send_socket.connect( udp_endpoint );
//	if ( -1 == replication_send_ENDPOINT.connect( udp_endpoint ) ){
//	{
//		Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 1, "Failed in the initialization of the recv socket.", __FILE__, __LINE__ );
//		return -1;
//	}

	





	Logger::putLogInfo( LOG_CAT_L7VSD_REPLICATION, 1, "Initialization of receive socket is success.", __FILE__, __LINE__ );

	return 0;
}

//! Pay replication memory 
//! @param[in] component_id is the one to identify the component.
//! @param[out] size of component use blocks
//! @return Replication memory address
//! @retval nonnull Replication memory address
//! @retval NULL Error
void*		replication::pay_memory( const std::string& inid, unsigned int& outsize ){
	Logger	logger( LOG_CAT_L7VSD_REPLICATION, 1, "replication::pay_memory", __FILE__, __LINE__ );

	void *ret = NULL;
	std::stringstream buf;

	outsize = 0;
	// Check replication mode.
	if ( REPLICATION_OUT == replication_state.service_status || REPLICATION_SINGLE == replication_state.service_status){
		std::stringstream buf;
		// Check mode that can use the replication.
		buf << "Improper mode for Replication. mode : " << replication_mode[(int)replication_state.service_status];
		Logger::putLogInfo( LOG_CAT_L7VSD_REPLICATION, 1, buf.str(), __FILE__, __LINE__ );
		return NULL;
	}

	if ( NULL == replication_state.component_memory ){
		Logger::putLogError( LOG_CAT_L7VSD_SYSTEM_MEMORY, 1, "You can't get memory. Component memory is NULL.", __FILE__, __LINE__ );
		return NULL;
	}

	// search Component ID
	for ( int i = 0; i < replication_info.component_num; i++ ){
		// Demand ID is compared with Component ID
		if ( inid == replication_info.component_info[i].id ){
			// size check
			if ( 0 == replication_info.component_info[i].block_size ){
				Logger::putLogWarn( LOG_CAT_L7VSD_REPLICATION, 1, "Component block size is 0.", __FILE__, __LINE__ );
				return NULL;
			}

			// block_head check
			if ( replication_info.component_info[i].block_head < 0 || replication_info.component_info[i].block_head > replication_state.total_block ){
				buf << "Too many component block. Max is " << replication_state.total_block << ".";
				Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 1, buf.str(), __FILE__, __LINE__ );
				return NULL;
			}
			// Pay memory address
			ret = ( char * )replication_state.component_memory + replication_info.component_info[i].block_head * DATA_SIZE;
			// Nnumber of blocks of ID was returned.
			outsize = replication_info.component_info[i].block_size;

			// LOG INFO
			char str[256];
			sprintf( str, "Component Info ID : \"%s\". Block size : %d . Head Block No : %d/  Pay memory : %p ",
					replication_info.component_info[i].id.c_str(), 
					replication_info.component_info[i].block_size,
					replication_info.component_info[i].block_head,
					( char* )ret );
			Logger::putLogInfo( LOG_CAT_L7VSD_REPLICATION, 1, str, __FILE__, __LINE__ );
			return ret;
		}
	}

	buf << "Unknown component ID. Component ID : " << inid;
	Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 1, buf.str(), __FILE__, __LINE__ );

	return NULL;
}

//! Replication Dump
void		replication::dump_memory(){
	Logger	logger( LOG_CAT_L7VSD_REPLICATION, 1, "replication::dump_memory", __FILE__, __LINE__ );

	int size;
	char* p;
	char* head;
	int h = 0;
	int i = 0;
	char str[256];

	// Check replication mode.
	if ( REPLICATION_OUT == replication_state.service_status || REPLICATION_SINGLE == replication_state.service_status){
		sprintf( str, "Replication memory dump failure. mode : %s", replication_mode[(int)replication_state.service_status] );
		Logger::putLogWarn( LOG_CAT_L7VSD_REPLICATION, 1, str, __FILE__, __LINE__ );
		return;
	}

	if ( NULL == replication_state.replication_memory ){
		Logger::putLogWarn( LOG_CAT_L7VSD_SYSTEM_MEMORY, 1, "Replication memory is NULL.", __FILE__, __LINE__ );
		return;
	}

	// Dump size
	size = 480 * replication_state.total_block;
	if ( 0 == size ){
		Logger::putLogWarn( LOG_CAT_L7VSD_REPLICATION, 1, "Can not get Replication memory", __FILE__, __LINE__ );
		return;
	}

	// Memory Dump
	p = ( char * )replication_state.replication_memory;

	// Output mode
	Logger::putLogInfo( LOG_CAT_L7VSD_REPLICATION, 1, "Replication Dump Start ----------------------------", __FILE__, __LINE__ );
	sprintf( str, "Mode is [ %s ].", replication_mode[(int)replication_state.service_status] );
	Logger::putLogInfo( LOG_CAT_L7VSD_REPLICATION, 1, str, __FILE__, __LINE__ );
	sprintf( str, "Total Block is [ %u ]", replication_state.total_block );
	Logger::putLogInfo( LOG_CAT_L7VSD_REPLICATION, 1, str, __FILE__, __LINE__ );

	// Converts into the binary, and writes it to the file. 
	for ( h = 0; h < size / DATA_SIZE; h++ ){
		sprintf( str, "Block Number [ %d ]",h );
		Logger::putLogInfo( LOG_CAT_L7VSD_REPLICATION, 1, str, __FILE__, __LINE__ );

		for ( i = 0; i < DATA_SIZE / LOG_DATA_WIDTH; i++ ){
			head = p + h * DATA_SIZE + i * LOG_DATA_WIDTH;
			sprintf( str, "%02hhX %02hhX %02hhX %02hhX  %02hhX %02hhX %02hhX %02hhX  %02hhX %02hhX %02hhX %02hhX  %02hhX %02hhX %02hhX %02hhX",
							*head, *(head+1), *(head+2), *(head+3), *(head+4), *(head+5), *(head+6), *(head+7),
							*(head+8), *(head+9), *(head+10), *(head+11), *(head+12), *(head+13), *(head+14), *(head+15));
			Logger::putLogInfo( LOG_CAT_L7VSD_REPLICATION, 1, str, __FILE__, __LINE__ );
		}
	}
	Logger::putLogInfo( LOG_CAT_L7VSD_REPLICATION, 1, "Replication Dump End ------------------------------", __FILE__, __LINE__ );
}

//! Chenge Status isActive
void		replication::start(){
	Logger	logger( LOG_CAT_L7VSD_REPLICATION, 1, "replication::start", __FILE__, __LINE__ );

	std::stringstream buf;

	switch (replication_state.service_status){
		case REPLICATION_MASTER_STOP:
			replication_state.service_status = REPLICATION_MASTER;
			buf << "Replication start. mode : " << replication_mode[(int)replication_state.service_status];
			Logger::putLogInfo( LOG_CAT_L7VSD_REPLICATION, 1, buf.str(), __FILE__, __LINE__ );
			break;
		case REPLICATION_SLAVE_STOP:
			replication_state.service_status = REPLICATION_SLAVE;
			buf << "Replication start. mode : " << replication_mode[(int)replication_state.service_status];
			Logger::putLogInfo( LOG_CAT_L7VSD_REPLICATION, 1, buf.str(), __FILE__, __LINE__ );
			break;
		case REPLICATION_MASTER:
			buf << "Could not MASTER start, because already start. mode : " << replication_mode[(int)replication_state.service_status];
			Logger::putLogWarn( LOG_CAT_L7VSD_REPLICATION, 1, buf.str(), __FILE__, __LINE__ );
			break;
		case REPLICATION_SLAVE:
			buf << "Could not SALVE start, because already start. mode : " << replication_mode[(int)replication_state.service_status];
			Logger::putLogWarn( LOG_CAT_L7VSD_REPLICATION, 1, buf.str(), __FILE__, __LINE__ );
			break;
		default:
			buf << "Could not start, because  mode is " << replication_mode[(int)replication_state.service_status];
			Logger::putLogWarn( LOG_CAT_L7VSD_REPLICATION, 1, buf.str(), __FILE__, __LINE__ );
			break;
	}
}

//! Chenge Status isStop
void		replication::stop(){
	Logger	logger( LOG_CAT_L7VSD_REPLICATION, 1, "replication::stop", __FILE__, __LINE__ );

	std::stringstream buf;

	switch (replication_state.service_status){
		case REPLICATION_MASTER:
			replication_state.service_status = REPLICATION_MASTER_STOP;
			buf << "Replication stop. mode : " << replication_mode[(int)replication_state.service_status];
			Logger::putLogInfo( LOG_CAT_L7VSD_REPLICATION, 1, buf.str(), __FILE__, __LINE__ );
			break;
		case REPLICATION_SLAVE:
			replication_state.service_status = REPLICATION_SLAVE_STOP;
			buf << "Replication stop. mode : " << replication_mode[(int)replication_state.service_status];
			Logger::putLogInfo( LOG_CAT_L7VSD_REPLICATION, 1, buf.str(), __FILE__, __LINE__ );
			break;
		case REPLICATION_MASTER_STOP:
			buf << "Could not MASTER stop, because already stop. mode : " << replication_mode[(int)replication_state.service_status];
			Logger::putLogWarn( LOG_CAT_L7VSD_REPLICATION, 1, buf.str(), __FILE__, __LINE__ );
			break;
		case REPLICATION_SLAVE_STOP:
			buf << "Could not SALVE stop, because already stop. mode : " << replication_mode[(int)replication_state.service_status];
			Logger::putLogWarn( LOG_CAT_L7VSD_REPLICATION, 1, buf.str(), __FILE__, __LINE__ );
			break;
		default:
			buf << "Could not start, because  mode is " << replication_mode[(int)replication_state.service_status];
			Logger::putLogWarn( LOG_CAT_L7VSD_REPLICATION, 1, buf.str(), __FILE__, __LINE__ );
			break;
	}
}

//! Compulsion reproduction execution
void		replication::force_replicate(){
	Logger	logger( LOG_CAT_L7VSD_REPLICATION, 1, "replication::force_replicate", __FILE__, __LINE__ );

	struct	replication_data trans_data;
	int	send_ret = -1;
	error_code	interval_ret;
	int	ms_time = 0;
	struct timespec	time;
	Parameter			param;

	std::stringstream buf;

	// Check by continuous initialize.
	if ( REPLICATION_MASTER != replication_state.service_status &&
		REPLICATION_MASTER_STOP != replication_state.service_status ){
		// Initialization has already been done.
		buf << "Could not compulsion replication. Mode is different. mode : " << replication_mode[(int)replication_state.service_status];
		Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 1, buf.str(), __FILE__, __LINE__ );
		return;
	}else if ( REPLICATION_MASTER_STOP == replication_state.service_status ){
		buf << "Can not replication compulsorily, because mode is " << replication_mode[(int)replication_state.service_status] << " .";
		Logger::putLogInfo( LOG_CAT_L7VSD_REPLICATION, 1, buf.str(), __FILE__, __LINE__ );
		return;
	}

	// Replication memory is NULL
	if ( NULL == replication_state.replication_memory ){
		Logger::putLogError( LOG_CAT_L7VSD_SYSTEM_MEMORY, 1, "Replication memory is NULL.", __FILE__, __LINE__ );
		return;
	}

	// Component memory is NULL
	if ( NULL == replication_state.component_memory ){
		Logger::putLogError( LOG_CAT_L7VSD_SYSTEM_MEMORY, 1, "Component memory is NULL.", __FILE__, __LINE__ );
		return;
	}

	// set send interval
	time.tv_sec = 0;
	time.tv_nsec = 0;
	ms_time = param.get_int( PARAM_COMP_REPLICATION, "compulsorily_interval", interval_ret );

	if ( interval_ret ){
		Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 1, "Could not get interval value for replication compulsorily.", __FILE__, __LINE__ );
		return;
	}

	if ( ms_time < MIN_COMPULSORILRY_INTERVAL || MAX_COMPULSORILRY_INTERVAL < ms_time ){
		Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 1, "Invalid compulsorily interval.", __FILE__, __LINE__ );
		return;
	}

	time.tv_nsec =  (long)( ms_time * 1000000 );

	// set last send block is c maximum block
	replication_state.last_send_block = replication_state.total_block-1;

	for(unsigned int i = 0; i < replication_state.total_block; i++){
		// set compulsorily interval.
		nanosleep( &time, NULL );

		// Temporary preservation sturuct initialize
		memset( &trans_data, 0, sizeof(struct replication_data) );
		send_ret = send_data(&trans_data);

		if ( 0 != send_ret ){
			Logger::putLogError( LOG_CAT_L7VSD_SYSTEM, 1, "Send data is Failed.", __FILE__, __LINE__ );
			return;
		}

		// set last send block number
		if ( replication_state.last_send_block < replication_state.total_block-1 ){
			replication_state.last_send_block += 1;
		}else if(replication_state.last_send_block == replication_state.total_block-1){
			replication_state.last_send_block = 0;
		}else{
			Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 1, "Last send block number is illegal.", __FILE__, __LINE__ );
			return;
		}
		buf << "Data sending succeeded. Send block number : " << replication_state.last_send_block << "Version : " << (unsigned long long)replication_state.sarface_block_no;
		Logger::putLogInfo( LOG_CAT_L7VSD_REPLICATION, 1, buf.str(), __FILE__, __LINE__ );

		// sarface block number is change
		if(replication_state.total_block == replication_state.last_send_block + 1 ){
			// Synchronization is executed. 
			memcpy( replication_state.replication_memory, replication_state.component_memory, replication_state.total_block*DATA_SIZE );

			// serial initialize
			replication_state.sarface_block_no = 0;

			// make new serial
			replication_state.sarface_block_no = (uint64_t)make_serial();
			if ( 0 == replication_state.sarface_block_no ){
				Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 1, "Could not get serial number.", __FILE__, __LINE__ );
				return;
			}
		}
	}
	Logger::putLogInfo( LOG_CAT_L7VSD_REPLICATION, 1, "Replication compulsorily is success.", __FILE__, __LINE__ );
}

//! Interval Re-setting
void		replication::reset(){
	Logger	logger( LOG_CAT_L7VSD_REPLICATION, 1, "replication::reset", __FILE__, __LINE__ );

	error_code ret;
	unsigned short value;
	std::stringstream buf;
	Parameter			param;

	// Check Parameter exists
	value = param.get_int( PARAM_COMP_REPLICATION, "interval", ret );
	if ( ret ){
		Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 1, "Not chage re-setting value.", __FILE__, __LINE__ );
		return;
	}

	// Check interval
	if ( value < MIN_INTERVAL || MAX_INTERVAL < value ){
		buf << "Invalid Interval value. value : " << (int)value;
		Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 1, buf.str(), __FILE__, __LINE__ );
		return;
	}
	//set interval
	replication_info.interval =  value;
}

//! Get Status
//! @return REPLICATION_MODE_TAG enumration
replication::REPLICATION_MODE_TAG	replication::get_status(){
	Logger	logger( LOG_CAT_L7VSD_REPLICATION, 1, "replication::get_status", __FILE__, __LINE__ );

	return replication_state.service_status;
}

//! Send Interval Check
//! @retval 0 Send data
//! @retval -1 Not send data
int			replication::check_interval(){
	Logger	logger( LOG_CAT_L7VSD_REPLICATION, 1, "replication::check_interval", __FILE__, __LINE__ );

	struct timeval tv;
	struct timezone tz;
	unsigned long long sending_time = 0;
	unsigned long long last_send_time = 0;
	unsigned long long diff_time = 0;

	// last time
	last_send_time = replication_state.send_time;

	// now time
	gettimeofday(&tv,&tz);
	sending_time = (unsigned long long)(tv.tv_sec*1000000+tv.tv_usec);

	// Last send time is 0.
	if ( 0 == last_send_time ){
		replication_state.send_time = sending_time;
		Logger::putLogInfo( LOG_CAT_L7VSD_REPLICATION, 1, " Last send time is 0.", __FILE__, __LINE__ );
		return 0;
	}

	//Last send time is illegal. 
	if ( last_send_time > sending_time){
		replication_state.send_time = sending_time;
		Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 1, "Last send time illegal.", __FILE__, __LINE__ );
		return -1;
	}

	// interval
	diff_time = sending_time - last_send_time;

	if ( diff_time < replication_info.interval){
		return -1;
	}
	replication_state.send_time = sending_time;
	return 0;
}

int			replication::handle_send(){
	Logger	logger( LOG_CAT_L7VSD_REPLICATION, 1, "replication::handle_send", __FILE__, __LINE__ );

	return 0;
}

int			replication::handle_receive(){
	Logger	logger( LOG_CAT_L7VSD_REPLICATION, 1, "replication::handle_receive", __FILE__, __LINE__ );

	return 0;
}

int			replication::lock( const std::string& inid ){
	Logger	logger( LOG_CAT_L7VSD_REPLICATION, 1, "replication::lock", __FILE__, __LINE__ );

	std::map<std::string, mutex_ptr>::iterator	itr;
	std::stringstream buf;

	itr = replication_mutex.find( inid );
	if( itr == replication_mutex.end() ){
		buf << "Could not find " << inid << ".";
		Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 1, buf.str(), __FILE__, __LINE__ );
		return -1;
	}

	itr->second->lock();

	return 0;
}

int			replication::unlock( const std::string& inid ){
	Logger	logger( LOG_CAT_L7VSD_REPLICATION, 1, "replication::unlock", __FILE__, __LINE__ );

	std::map<std::string, mutex_ptr>::iterator	itr;
	std::stringstream buf;

	itr = replication_mutex.find( inid );
	if( itr == replication_mutex.end() ){
		buf << "Could not find " << inid << ".";
		Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 1, buf.str(), __FILE__, __LINE__ );
		return -1;
	}

	itr->second->unlock();

	return 0;
}

int			replication::refer_lock_mutex( const std::string& inid, mutex_ptr outmutex ){
	Logger	logger( LOG_CAT_L7VSD_REPLICATION, 1, "replication::refer_lock_mutex", __FILE__, __LINE__ );

	std::map<std::string, mutex_ptr>::iterator	itr;
	std::stringstream buf;

	itr = replication_mutex.find( inid );
	if( itr == replication_mutex.end() ){
		buf << "Could not find " << inid << ".";
		Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 1, buf.str(), __FILE__, __LINE__ );
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
//	int info_ret = -1;
//	int cmp_ret = -1;
//	struct addrinfo hints;
	size_t sum=0;
	std::stringstream buf;

	// initialize structure
//	memset(&hints, 0, sizeof(struct addrinfo));

	// set address hints
//	hints.ai_family = AF_UNSPEC;     // permit any protocol (IPv4, IPv6, etc.)
//	hints.ai_socktype = SOCK_DGRAM;  // UDP
	// Whether IP and the port are effective is confirmed.
//	info_ret = getaddrinfo(replication_info.ip_addr,replication_info.service_name,&hints,&internal_val.address_info);
//	if ( 0 != info_ret ){
//		buf << "Failed to get IP or Service Name.(" << ret << ")";
//		Logger::putLogError( LOG_CAT_L7VSD_SYSTEM_ENDPOINT, 1, buf.str(), __FILE__, __LINE__ );
//		goto END;
//	}
//	struct sockaddr_in *sin;
//	sin = (sockaddr_in *)internal_val.address_info->ai_addr;
	
	// Port check
//	if ( sin->sin_port == htons(0)){
//		Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 1, "You can't specify port number 0.", __FILE__, __LINE__ );
//		goto END;
//	}
	// Interval check
	if ((MIN_INTERVAL>replication_info.interval) || (MAX_INTERVAL<replication_info.interval)){
		Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 1, "Invalid Interval value", __FILE__, __LINE__ );
		goto END;
	}
	// Components ID check
	// Components Size check
	if (0 == replication_info.component_num){
		Logger::putLogInfo( LOG_CAT_L7VSD_REPLICATION, 1, "Can not get component, because component is 0.", __FILE__, __LINE__ );
		ret = 0;
		goto END;
	}
	for ( int i=0; i < replication_info.component_num; i++ ){
		sum += replication_info.component_info[i].block_size ;
		for ( int j=i+1; j<replication_info.component_num; j++)	{
			if ( replication_info.component_info[j].id == replication_info.component_info[i].id ){
				buf << "Too many component block. Max is " << replication_state.total_block << ".";
				Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 1, buf.str(), __FILE__, __LINE__ );
				goto END;
			}
		}
	}
	if ( sum > CMP_BLOCK_MAX ){
		Logger::putLogError( LOG_CAT_L7VSD_REPLICATION, 1, "Total component size is too large.", __FILE__, __LINE__ );
		goto END;
	}
	ret = 0;
END:

	Logger::putLogInfo( LOG_CAT_L7VSD_REPLICATION, 1, ( ( 0 == ret ) ? "Parameter Check OK." :  "Parameter Check NG." ), __FILE__, __LINE__ );
	return ret;
}


//! Get Replication Memory
//! @return memory Replication memory
//! @retval memory memory get Success
//! @retval NULL Error
void*		replication::getrpl(){
	unsigned int total_block = replication_state.total_block;
	void *memory = NULL;

	// Get replication memory
	memory = malloc( total_block*DATA_SIZE );

	// malloc Error
	if ( (void *)NULL == memory ){
		Logger::putLogError( LOG_CAT_L7VSD_SYSTEM_MEMORY, 1, "Replication memory is Malloc Error.", __FILE__, __LINE__ );
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
	void *memory = NULL ;

	// Get component memory
	memory = malloc( total_block*DATA_SIZE );

	// malloc Error
	if ( (void *)NULL == memory ){
		Logger::putLogError( LOG_CAT_L7VSD_SYSTEM_MEMORY, 1, "Component memory is Malloc Error.", __FILE__, __LINE__ );
		return NULL;
	}
	memset(memory,0,total_block*DATA_SIZE);

	return memory;
}

//! Get Sarface Number Memory 
//! @return memory Component memory
//! @retval memory memory get Success
//! @retval NULL Error
uint64_t*	replication::getsrf(){
	unsigned int total_block = replication_state.total_block;
	uint64_t *memory = NULL;

	// Get memory
	memory = (uint64_t*)malloc( total_block*sizeof(uint64_t) );

	// malloc Error
	if ( (uint64_t *)NULL == memory ){
		Logger::putLogError( LOG_CAT_L7VSD_SYSTEM_MEMORY, 1, "Surface info address is Malloc Error.", __FILE__, __LINE__ );
		return NULL;
	}
	memset(memory,0,total_block*sizeof(uint64_t));

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
		Logger::putLogError( LOG_CAT_L7VSD_SYSTEM, 1, "You failed to get of time.", __FILE__, __LINE__ );
		serial_num = 0;
	} else {
		// make a serial succeeds.
		serial_num = (unsigned long long int) current_time.tv_sec * 1000000 + (unsigned long long int) current_time.tv_nsec / 1000;
	}

	return serial_num;
}

//! Send transfer data to standby server
//! @param[in] data Points to input data from external program. This will be send to standby server.
//! @retval 0 Success
//! @retval -1 Error
int			replication::send_data( struct replication_data* indata ){

	return 0;
}

//! Receive transfer data from active server
//! @param[out]  recv_data Points to output data from external program.
//! @retval 0 Success
//! @retval -1 Error
int			replication::recv_data( struct replication_data* outdata ){

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

//! Release Sarface Memory
void		replication::releasesrf(){
	if ( NULL != replication_state.sarface_block_array_ptr ){
		free(replication_state.sarface_block_array_ptr);
	}
	replication_state.sarface_block_array_ptr=NULL;
}

void		replication::send_thread(){
}

}	//namespace l7vs
