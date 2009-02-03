//
//	@file	replication.h
//	@brief	Replication class
//
//	copyright (c) xxx corporation. 2008
//	mail: 
//
//	Distributed under the Boost Software License, Version 1.0.(See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt
//
#ifndef REPLICATION_H
#define REPLICATION_H

#include <string>
#include <map>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/thread/condition.hpp>
#include <stdint.h>

//! Max Number of Components
#define CMP_MAX	(100)

//! Max Size of Components
#define CMP_BLOCK_MAX (100000)

//! Key's length of Component ID
#define ID_LENGTH (17)

//! REPLICATION_SLAVE of Component SIZE
#define CMP_SIZE_LENGTH (17)

//! Maximum size of input/output raw data
#define DATA_SIZE (480)

//! Maximum size of input/output raw data
#define SEND_DATA_SIZE (512)

//! Min Interval
#define MIN_INTERVAL (10)

//! Max Interval
#define MAX_INTERVAL (10000)

//! Min Compulsorily Interval
#define MIN_COMPULSORILRY_INTERVAL (4)

//! Max Compulsorily Interval
#define MAX_COMPULSORILRY_INTERVAL (400)

//! Set nic length
#define NIC_LENGTH (128)

//! Dump log data's width
#define LOG_DATA_WIDTH (16)

//! Replication data ID
#define REPLICATION_ID (37)


namespace l7vs{

//! Transfer data between active server and standby server.
struct replication_data{
	unsigned short					id;							//!< ID of Replication Function
	char							pad1[6];
	unsigned long long				serial;						//!< Serial Number
	unsigned int					block_num;					//!< Block Number in Replication memory
	char							pad2[4];
	uint64_t						size;						//!< Data size for checking packet loss
	char							data[DATA_SIZE];			//!< Raw data
};

class	replication{
public:
	enum REPLICATION_MODE_TAG{
		REPLICATION_OUT = 0,
		REPLICATION_SINGLE,
		REPLICATION_MASTER,
		REPLICATION_SLAVE,
		REPLICATION_MASTER_STOP,
		REPLICATION_SLAVE_STOP
	};

protected:
	//! Component information set to SG file.
	struct replication_component{
		std::string 					id;							//!< ID distinguishes Component
		unsigned int					block_head;					//!< The first Block Number of Component's memory
		unsigned int					block_size;					//!< Number of Block allocated in component memory

		replication_component() :		id(""),
										block_head(0),
										block_size(0) {}
	};

	//! State Infomation struct to execute Replication.
	struct replication_state_struct{
		enum REPLICATION_MODE_TAG		service_status;				//!< States Type of Replication Function
		unsigned long long				send_time;					//!< Completion last time to send data
		unsigned int					last_send_block;			//!< Completion last Block Number to send data
		unsigned int					last_recv_block;			//!< Completion last Block Number to receive data
		unsigned int					total_block;				//!< The maximum Block Number of Components memory
		void*							replication_memory;			//!< Top address in Replication memory
		void*							component_memory;			//!< Top address in Component memory
		uint64_t						sarface_block_no;			//!< Serial number for respect switch
		uint64_t*						sarface_block_array_ptr;	//!< Serial number in received respect at every block

		replication_state_struct() :	service_status(REPLICATION_OUT),
										send_time(0),
										last_send_block(0),
										last_recv_block(0),
										total_block(0),
										replication_memory(NULL),
										component_memory(NULL),
										sarface_block_no(0),
										sarface_block_array_ptr(NULL) {}
	};

	//! Essential information set to SG file.
	struct replication_info_struct{
		std::string						ip_addr;					//!< Destination IP address (IPv4 or IPv6)
		std::string 					service_name;				//!< Destination service name or port number
		std::string 					nic;						//!< Network device (ex. eth0, bond0, hme0, etc.)
		unsigned short					interval;					//!< Interval when data of one block is sent
		int								component_num;				//!< Number of components read setting
		struct replication_component	component_info[CMP_MAX];	//!< Information on individual component

		replication_info_struct() :		ip_addr(""),
										service_name(""),
										nic(""),
										interval(0),
										component_num(0) {}
	};

	std::map<std::string, boost::mutex>		replication_mutex;
	boost::asio::ip::udp::socket			replication_receive_socket;
	boost::asio::ip::udp::socket			replication_send_socket;
	struct replication_state_struct			replication_state;
	struct replication_info_struct			replication_info;
	boost::asio::io_service&				receive_io;
	boost::asio::io_service					send_io;
	boost::thread							replication_thread;
	boost::mutex							replication_thread_mutex;
	boost::condition						replication_thread_condition;
	int										replication_flag;

public:
	replication( boost::asio::io_service& inreceive_io ) :	replication_receive_socket( inreceive_io ),
															replication_send_socket( inreceive_io ),
															receive_io( inreceive_io ) {} ;
	~replication(){}

	int							initialize();
	void						finalize();
	void						switch_to_master();
	void						switch_to_slave();
	void*						pay_memory( std::string& inid, unsigned int& outsize );
	void						dump_memory();
	void						start();
	void						stop();
	void						force_replicate();
	void						reset();
	REPLICATION_MODE_TAG		get_status();
	int							check_interval();
	int							handle_send();
	int							handle_receive();
	int							lock( std::string& inid );
	int							unlock( std::string& inid );
	int							refer_lock_mutex( std::string& inid, boost::mutex& outmutex );
protected:
	int							set_master();
	int							set_slave();
	int							check_parameter();
	void*						getrpl();
	void*						getcmp();
	uint64_t*					getsrf();
	int							sock_init();
	void						sock_fini();
	unsigned long long			make_serial();
	int							send_data( struct replication_data* indata );
	int							recv_data( struct replication_data* outdata );
	void						releaserpl();
	void						releasecmp();
	void						releasesrf();

	void						send_thread();
};

}	//namespace l7vs

#endif	//REPLICATION_H
