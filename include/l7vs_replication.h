/*
 * @file	l7vs_replication.h
 * @bref	The Replication of data between ACT and STB.
 *
 * L7VSD: Linux Virtual Server for Layer7 Load Balancing
 * Copyright (C) 2005  NTT COMWARE Corporation.
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
#ifndef L7VS_REPLICATION_H
#define L7VS_REPLICATION_H


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

#include <netdb.h>
#include <stdint.h>

//! Mode Type
enum REPLICATION_MODE_TAG{
	REPLICATION_OUT,		//!< Initial
	REPLICATION_SINGLE,		//!< Single
	REPLICATION_MASTER,		//!< Master isActive
	REPLICATION_SLAVE,		//!< Slave isActive
	REPLICATION_MASTER_STOP,	//!< Master isStop
	REPLICATION_SLAVE_STOP		//!< Slave isStop
};

//! Transfer data between active server and standby server.
struct l7vs_replication_data{
	unsigned short		id;		//!< ID of Replication Function
	char			pad1[6];
	unsigned long long	serial;		//!< Serial Number
	unsigned int		block_num;	//!< Block Number in Replication memory
	char			pad2[4];
	uint64_t		size;		//!< Data size for checking packet loss
	char			data[DATA_SIZE];//!< Raw data
};

//! Component information set to SG file.
struct l7vs_replication_component{
	char 			id[ID_LENGTH];		//!< ID distinguishes Component
	unsigned int		block_head;	//!< The first Block Number of Component's memory
	unsigned int		block_size;	//!< Number of Block allocated in component memory
};

//! State Infomation struct to execute Replication.
struct l7vs_replication_state{
	enum REPLICATION_MODE_TAG	service_status;		//!< States Type of Replication Function
	unsigned long long		send_time;		//!< Completion last time to send data
	unsigned int			last_send_block;	//!< Completion last Block Number to send data
	unsigned int			last_recv_block;	//!< Completion last Block Number to receive data
	unsigned int			total_block;		//!< The maximum Block Number of Components memory
	void *				replication_memory;	//!< Top address in Replication memory
	void *				component_memory;	//!< Top address in Component memory
	uint64_t			sarface_block_no;	//!< Serial number for respect switch
	uint64_t *			sarface_block_array_ptr;//!< Serial number in received respect at every block
};

//! Essential information set to SG file.
struct l7vs_replication_info{
	char 	ip_addr[NI_MAXHOST];		//!< Destination IP address (IPv4 or IPv6)
	char 	service_name[NI_MAXSERV];	//!< Destination service name or port number
	char 	nic[NIC_LENGTH];			//!< Network device (ex. eth0, bond0, hme0, etc.)
	unsigned short	interval;		//!< Interval when data of one block is sent
	int		component_num;		//!< Number of components read setting
	struct	l7vs_replication_component component_info[CMP_MAX];	//!< Information on individual component
};


// External function

// Initialize of Replication Function
extern int l7vs_replication_init();

// Finalize of Replication Function
extern void l7vs_replication_fini();

// Switches Slave to Master ( VirtualService is added )
extern void l7vs_replication_switch_to_master();

// Switches Master to Slave ( VirtualService is deleted )
extern void l7vs_replication_switch_to_slave();

// Allocate the memory of the component.
extern void *l7vs_replication_pay_memory( char* component_id, unsigned int* size );

// Replication Callback
extern int l7vs_replication_send_callback( struct l7vs_iomux *iom );

// Replication Callback
extern int l7vs_replication_recv_callback( struct l7vs_iomux *iom );

// Dump Replication info to the Log.
extern void l7vs_replication_dump_memory();

// Starting the Replication
extern void l7vs_replication_start();

// Stopping the Replication
extern void l7vs_replication_stop();

// Compulsorily executes the replication.
extern void l7vs_replication_compulsorily();

// Re-Sets the Interval
extern void l7vs_replication_reset();

// Acquires the state
extern enum REPLICATION_MODE_TAG l7vs_replication_get_status();

// Check the interval time
extern int l7vs_replication_checkinterval();


#endif
