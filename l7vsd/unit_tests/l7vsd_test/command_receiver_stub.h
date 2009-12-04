#ifndef    COMMAND_RECEIVER_H
#define    COMMAND_RECEIVER_H

#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>

namespace    l7vs{
class    l7vsd;
class    command_receiver : private boost::noncopyable{
public:
    //!    @brief        constructor
    //!    @param[in]    io_service
    //!    @param[in]    socket file
    //!    @param[in]    l7vsd refernce
    command_receiver(    boost::asio::io_service&, const std::string&, l7vsd& )
        {}

    //!    @brief        destructor
    ~command_receiver()
        {}

};
}    //namespace    l7vs
#endif    //COMMAND_RECEIVER_H
