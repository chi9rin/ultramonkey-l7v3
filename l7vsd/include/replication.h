/*
 *    @file    replication.h
 *    @brief    Replication class
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
#ifndef REPLICATION_H
#define REPLICATION_H

#include <string>
#include <map>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/thread/condition.hpp>
#include <boost/shared_ptr.hpp>
#include <stdint.h>

//! Max Number of Components
#define CMP_MAX    (100)

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

//! Dump log data's width
#define LOG_DATA_WIDTH (16)

//! Replication data ID
#define REPLICATION_ID (37)


namespace l7vs
{

class    replication
{
public:
        enum REPLICATION_MODE_TAG {
                REPLICATION_OUT = 0,
                REPLICATION_SINGLE,
                REPLICATION_MASTER,
                REPLICATION_SLAVE,
                REPLICATION_MASTER_STOP,
                REPLICATION_SLAVE_STOP
        };

        typedef    boost::shared_ptr<boost::mutex>        mutex_ptr;
        typedef    boost::shared_ptr<boost::thread>    thread_ptr;
protected:
        //! Transfer data between active server and standby server.
        struct replication_data_struct {
                unsigned short                  id;                             //!< ID of Replication Function
                char                            pad1[6];
                unsigned long long              serial;                         //!< Serial Number
                unsigned int                    block_num;                      //!< Block Number in Replication memory
                char                            pad2[4];
                uint64_t                        size;                           //!< Data size for checking packet loss
                char                            data[DATA_SIZE];                //!< Raw data
        };

        //! Component information set to SG file.
        struct replication_component {
                std::string                     id;                             //!< ID distinguishes Component
                unsigned int                    block_head;                     //!< The first Block Number of Component's memory
                unsigned int                    block_size;                     //!< Number of Block allocated in component memory

                replication_component() :       id(""),
                        block_head(0),
                        block_size(0) {}
        };

        //! State Information struct to execute Replication.
        struct replication_state_struct {
                enum REPLICATION_MODE_TAG       service_status;                 //!< States Type of Replication Function
                unsigned long long              send_time;                      //!< Completion last time to send data(no use)
                unsigned int                    last_send_block;                //!< Completion last Block Number to send data
                unsigned int                    last_recv_block;                //!< Completion last Block Number to receive data
                unsigned int                    total_block;                    //!< The maximum Block Number of Components memory
                void                           *replication_memory;             //!< Top address in Replication memory
                void                           *component_memory;               //!< Top address in Component memory
                uint64_t                        surface_block_no;               //!< Serial number for respect switch
                uint64_t                       *surface_block_array_ptr;        //!< Serial number in received respect at every block

                replication_state_struct() :    service_status(REPLICATION_OUT),
                        send_time(0),
                        last_send_block(0),
                        last_recv_block(0),
                        total_block(0),
                        replication_memory(NULL),
                        component_memory(NULL),
                        surface_block_no(1),
                        surface_block_array_ptr(NULL) {}
        };

        //! Essential information set to SG file.
        struct replication_info_struct {
                std::string                     ip_addr;                        //!< Destination IP address (IPv4 or IPv6)
                std::string                     service_name;                   //!< Destination service name or port number
                std::string                     recv_ip_addr;                   //!< Replication Data Receive IP address (IPv4 or IPv6)
                unsigned short                  interval;                       //!< Interval when data of one block is sent
                int                             component_num;                  //!< Number of components read setting
                struct replication_component    component_info[CMP_MAX];        //!< Information on individual component

                replication_info_struct() :     ip_addr(""),
                        service_name(""),
                        recv_ip_addr(""),
                        interval(0),
                        component_num(0) {}
        };

        std::map<std::string, mutex_ptr>    replication_mutex;
        boost::asio::io_service             service_io;
        boost::asio::ip::udp::endpoint      replication_endpoint;
        boost::asio::ip::udp::endpoint      bind_endpoint;
        boost::asio::ip::udp::socket        replication_receive_socket;
        boost::asio::ip::udp::socket        replication_send_socket;
        struct replication_state_struct     replication_state;
        struct replication_info_struct      replication_info;

        enum REPLICATION_THREAD_TAG {
                WAIT = 0,
                RUNNING,
                WAIT_REQ,
                EXIT
        };

        REPLICATION_THREAD_TAG              replication_flag;
        thread_ptr                          replication_thread_ptr;
        boost::mutex                        replication_thread_mutex;
        boost::condition                    replication_thread_condition;

        struct replication_data_struct      replication_data;

        REPLICATION_THREAD_TAG              service_flag;
        thread_ptr                          service_thread_ptr;
        boost::mutex                        service_thread_mutex;
        boost::condition                    service_thread_condition;

public:
        replication() : replication_receive_socket(service_io),
                replication_send_socket(service_io),
                replication_flag(EXIT),
                service_flag(EXIT) {
        } ;
        ~replication() {}

        int                         initialize();
        void                        finalize();
        void                        switch_to_master();
        void                        switch_to_slave();
        void                       *pay_memory(const std::string &inid, unsigned int &outsize);
        void                        dump_memory();
        void                        start();
        void                        stop();
        void                        force_replicate();
        void                        reset();
        REPLICATION_MODE_TAG        get_status();
        void                        handle_receive(const boost::system::error_code &err, size_t size);
        int                         lock(const std::string &inid);
        int                         unlock(const std::string &inid);
        int                         refer_lock_mutex(const std::string &inid, mutex_ptr &outmutex);
protected:
        int                         handle_send();
        int                         set_master();
        int                         set_slave();
        int                         check_parameter();
        void                       *getrpl();
        void                       *getcmp();
        uint64_t                   *getsrf();
        unsigned long long          make_serial();
        int                         send_data();
        int                         recv_data();
        void                        releaserpl();
        void                        releasecmp();
        void                        releasesrf();

        void                        send_thread();
        void                        service_thread();
};

}    //namespace l7vs

#endif    //REPLICATION_H
