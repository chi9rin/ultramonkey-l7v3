#ifndef	SCHEDULE_MODULE_STUB_H
#define	SCHEDULE_MODULE_STUB_H

#include "schedule_module_base.h"

namespace l7vs{

class schedule_module_stub : public schedule_module_base
{
public:
	//!	constractor
	schedule_module_stub() : schedule_module_base("stub"){}
	//! destractor
	~schedule_module_stub(){}

	//!	initialize function
	void	initialize() {}

	//! tcp protocol support check
	//! @return tcp support is true
	//! @return tcp not-support is false
	bool	is_tcp() { return true; }
	//! udp protocol support check
	//! @return udp support is true
	//! @return udp not-support is false
	bool	is_udp() { return false; }

	//! handle schedule called then schedule function for TCP/IP endpoint
	//! @param[in]	thread id
	//! @param[in]	list iterator first function object
	//!	@param[in]	list iterator last function object
	//!	@param[in]	list iterator next function object
	//! @param[out]	scheduled TCP/IP endpoint
	void	handle_schedule(
							boost::thread::id					thread_id,
							rslist_iterator_begin_func_type		inlist_begin,
							rslist_iterator_end_func_type		inlist_end,
							rslist_iterator_next_func_type		inlist_next,
							boost::asio::ip::tcp::endpoint&		outendpoint )
	{}

	//! handle schedule calles then schedule function for UDP endpoint
	//! @param[in]	thread id
	//! @param[in]	list iterator first function object
	//!	@param[in]	list iterator last function object
	//!	@param[in]	list iterator next function object
	//! @param[out]	scheduled UDP endpoint
	void	handle_schedule(
							boost::thread::id					thread_id,
							rslist_iterator_begin_func_type		inlist_begin,
							rslist_iterator_end_func_type		inlist_end,
							rslist_iterator_next_func_type		inlist_next,
							boost::asio::ip::udp::endpoint&		outendpoint )
	{}

	//! replication interval interrrupt
	//! timer thread call this function. from virtualservice.
	void	replication_interrupt()
	{}
};

}	//namespace l7vs

#endif //SCHEDULE_MODULE_LC_H
