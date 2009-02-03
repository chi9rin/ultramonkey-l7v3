#include	"schedule_module_base.h"
#include <iostream>

namespace l7vs{

class	schedule_module_test1 : public schedule_module_base{
protected:
//	boost::asio::ip::tcp::endpoint	tcp_endpoint ;
//	boost::asio::ip::udp::endpoint	udp_endpoint ;
public:
	schedule_module_test1() : schedule_module_base( "SMtest1" ) {
		std::cout << "SMtest1 create" << std::endl;
	}
	~schedule_module_test1(){
		std::cout << "SMtest1 destroy" << std::endl;
	}

	void	initialize(){}

	bool	is_tcp(){ return true; }
	bool	is_udp(){ return false; }

	void	handle_schedule(
							boost::thread::id					thread_id,
							rslist_iterator_begin_func_type		inlist_begin,
							rslist_iterator_end_func_type		inlist_end,
							rslist_iterator_next_func_type		inlist_next,
							boost::asio::ip::tcp::endpoint&	outendpoint ){}

	void	handle_schedule(
							boost::thread::id					thread_id,
							rslist_iterator_begin_func_type		inlist_begin,
							rslist_iterator_end_func_type		inlist_end,
							rslist_iterator_next_func_type		inlist_next,
							boost::asio::ip::udp::endpoint&	outendpoint ){}

	void	replication_interrupt(){
		std::cout << "Schedule Module Replication." << std::endl;
	}
};

}
