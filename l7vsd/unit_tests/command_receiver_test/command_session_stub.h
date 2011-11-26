#ifndef    COMMAND_SESSION_H
#define    COMMAND_SESSION_H

#include <map>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include "error_code.h"
#include "l7vs_command.h"

namespace    l7vs
{
class    l7vsd;

class    command_session
{
public:
        //! unix domain socket typedef
        typedef    boost::asio::local::stream_protocol::socket    unixsocket_type;
        //! command_handler typedef
        typedef std::map<l7vsadm_request::COMMAND_CODE_TAG, boost::function<void(l7vs::error_code)> >    command_handler_map_type;
        //! command code to status code map typedef
        typedef    std::map<l7vsadm_request::COMMAND_CODE_TAG, l7vsd_response::COMMAND_RESPONSE_CODE>    command_status_map_type;
        //! command_session shared_ptr typedef
        typedef    boost::shared_ptr<command_session>    command_session_ptr;

        //! unix domain socket
        unixsocket_type            unixsocket;

        //!    l7vsd refferecne
        l7vsd                    &vsd;

        static    unsigned int    constructor_called;
        static    unsigned int    start_called;
        static    unsigned int    socket_called;

public:
        command_session(boost::asio::io_service &io_service, l7vsd &parent)
                :    unixsocket(io_service),
                     vsd(parent) {
                ++constructor_called;
        }

        //!    @brief        return unixsocket
        unixsocket_type    &socket() {
                ++socket_called;
                return unixsocket;
        }

        void    start() {
                ++start_called;
        }

};
}    //namespace    l7vs
#endif    //COMMAND_SESSION_H
