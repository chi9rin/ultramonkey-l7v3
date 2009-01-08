//
//	@file	virtualservice.h
//	@brief	VirtualService class
//
//	copyright (c) sdy corporation. 2008
//	mail: h dot okada at sdy dot co dot jp
//
//	Distributed under the Boost Software License, Version 1.0.(See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt
//
#ifndef	VIRTUALSERVICE_H
#define	VIRTUALSERVICE_H

#include <string>
#include <vector>
#include <map>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "virtualservice_element.h"
#include "replication.h"

namespace l7vs{

struct	vs_element_result{
	bool			flag;
	std::string		message;
	vs_element_result() : flag(true), message(""){}
	bool	operator==( const vs_element_result& in )
				{ return ( ( flag == in.flag ) && ( message == in.message ) ); }
	bool	operator!=( const vs_element_result& in )
				{ return ( ( flag != in.flag ) || ( message != in.message ) ); }
};

class	virtualservice_base{
protected:
	boost::asio::io_service		dispatcher;

	virtualservice_element		element;

	protocol_module_base*		protomod;
	schedule_module_base*		schedmod;

	std::vector<realserver>		rs_list;
	boost::mutex				rs_list_mutex;
	unsigned int				rs_list_ref_count;
	virtual	void				rs_list_lock();
	virtual	void				rs_list_unlock();

	virtual	void				write_replicate_data();

	virtualservice_base(){};
	virtual	~virtualservice_base(){};
public:
	virtual	bool				operator==( const virtualservice_element& ); 

	virtual	void				initialize();
	virtual	void				finalize();

	virtual	vs_element_result	set_element( virtualservice_element& );
	virtual	vs_element_result	mod_element( virtualservice_element& );
	virtualservice_element&		get_element(){ return element; }

	virtual	void				run();
	virtual	void				stop();
//	virtual	void				pause();
};

class	virtualservice_tcp : private boost::noncopyable, public virtualservice_base{
protected:
	boost::asio::local::stream_protocol::acceptor
								acceptor_;

	void						rs_list_lock();
	void						rs_list_unlock();

	std::map<boost::thread::id,session_thread_control>
								pool_sessions;
	std::map<boost::thread::id,session_thread_control>
								active_sessions;

	void						write_replicate_data();

public:

	bool						operator==( const virtualservice_tcp& );
	bool						operator==( const virtualservice_element& ); 

	void						initialize();
	void						finalize();

	void						set_element( virtualservice_element& );
	void						mod_element( virtualservice_element& );

	void						run();
	void						stop();
//	void						pause();
};

}

#endif//VIRTUALSERVICE_H
