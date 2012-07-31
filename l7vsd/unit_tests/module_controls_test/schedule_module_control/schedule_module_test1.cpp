#include    "schedule_module_base.h"
#include <iostream>

namespace l7vs
{

class    schedule_module_test1 : public schedule_module_base
{
protected:
//    boost::asio::ip::tcp::endpoint    tcp_endpoint ;
//    boost::asio::ip::udp::endpoint    udp_endpoint ;
public:
        schedule_module_test1();
        ~schedule_module_test1();

        void    initialize();

        bool    is_tcp();
        bool    is_udp();

        void    handle_schedule(
                boost::thread::id                    thread_id,
                rslist_iterator_begin_func_type        inlist_begin,
                rslist_iterator_end_func_type        inlist_end,
                rslist_iterator_next_func_type        inlist_next,
                boost::asio::ip::tcp::endpoint    &outendpoint);

        void    handle_schedule(
                boost::thread::id                    thread_id,
                rslist_iterator_begin_func_type        inlist_begin,
                rslist_iterator_end_func_type        inlist_end,
                rslist_iterator_next_func_type        inlist_next,
                boost::asio::ip::udp::endpoint    &outendpoint);

        void    replication_interrupt() {}
};

schedule_module_test1::schedule_module_test1() : schedule_module_base("schedule_module_test1")
{
}

schedule_module_test1::~schedule_module_test1() {}

void    schedule_module_test1::initialize()
{
}

bool    schedule_module_test1::is_tcp()
{
        return true;
}
bool    schedule_module_test1::is_udp()
{
        return true;
}

void    schedule_module_test1::handle_schedule(
        boost::thread::id                    thread_id,
        rslist_iterator_begin_func_type        inlist_begin,
        rslist_iterator_end_func_type        inlist_end,
        rslist_iterator_next_func_type        inlist_next,
        boost::asio::ip::tcp::endpoint    &outendpoint)
{
}

void    schedule_module_test1::handle_schedule(
        boost::thread::id                    thread_id,
        rslist_iterator_begin_func_type        inlist_begin,
        rslist_iterator_end_func_type        inlist_end,
        rslist_iterator_next_func_type        inlist_next,
        boost::asio::ip::udp::endpoint    &outendpoint)
{
}

}

extern "C" l7vs::schedule_module_base*
create_module()
{
        return dynamic_cast<l7vs::schedule_module_base *>(new l7vs::schedule_module_test1());
}

extern "C" void
destroy_module(l7vs::schedule_module_base *in)
{
        delete in;
}
