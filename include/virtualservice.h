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
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "realserver.h"
#include "virtualservice_element.h"
//#include "replication.h"
#include "protocol_module_base.h"
#include "schedule_module_base.h"
//#include "session_thread_control.h"

#define	SESSION_POOL_NUM	256

namespace l7vs{

struct	vs_operation_result{
	bool			flag;
	std::string		message;
	vs_operation_result() : flag(true), message(""){}
	bool	operator==( const vs_operation_result& in )
				{ return ( ( flag == in.flag ) && ( message == in.message ) ); }
	bool	operator!=( const vs_operation_result& in )
				{ return ( ( flag != in.flag ) || ( message != in.message ) ); }
};

class	virtualservice_base : boost::noncopyable{
protected:
	const	l7vs::l7vsd&		vsd;
	const	l7vs::replication&	rep;

	boost::asio::io_service		dispatcher;

	virtualservice_element		element;

	protocol_module_base*		protomod;
	schedule_module_base*		schedmod;

	std::vector<realserver>		rs_list;
	boost::mutex				rs_list_mutex;
	unsigned int				rs_list_ref_count;
	virtual	void				rs_list_lock();
	virtual	void				rs_list_unlock();

	virtual	void				replication_interrupt();

	virtualservice_base(	const l7vs::l7vsd& invsd,
							const l7vs::replication& inrep,
							const virtualservice_element& inelement)
												 : vsd( invsd ), rep( inrep ), element( inelement ) {};
public:
	virtual	~virtualservice_base(){};

	virtual	bool				operator==( const virtualservice_base& );
	virtual	bool				operator!=( const virtualservice_base& );

	virtual	vs_operation_result	set_virtualservce( virtualservice_element& );
	virtual	vs_operation_result	edit_virtualservce( virtualservice_element& );
	virtual	vs_operation_result	add_realserver( virtualservice_element& );
	virtual	vs_operation_result	edit_realserver( virtualservice_element& );
	virtual	vs_operation_result	del_realserver( virtualservice_element& );
	virtualservice_element&		get_element(){ return element; }

	virtual	void				run();
	virtual	void				stop();
//	virtual	void				pause();

	virtual	void				connection_active( const boost::asio::ip::tcp::endpoint& );
	virtual	void				connection_inactive( const boost::asio::ip::tcp::endpoint& );
	virtual	void				release_session( boost::thread::id thread_id );
};

class	virtualservice_tcp : public virtualservice_base{
protected:
	boost::shared_ptr<boost::asio::ip::tcp::acceptor>
								acceptor_;

	std::map<boost::thread::id,session_thread_control>
								pool_sessions;
	std::map<boost::thread::id,session_thread_control>
								active_sessions;

	void						rs_list_lock();
	void						rs_list_unlock();

	void						replication_interrupt();

	virtualservice_tcp&	operator=( const virtualservice_tcp& ){}
public:
	virtualservice_tcp(		const l7vs::l7vsd& invsd,
							const l7vs::replication& inrep,
							const virtualservice_element& inelement);
	~virtualservice_tcp();

	bool						operator==( const virtualservice_base& );
	bool						operator!=( const virtualservice_base& );

	vs_operation_result			set_virtualservce( virtualservice_element& );
	vs_operation_result			edit_virtualservce( virtualservice_element& );

	vs_operation_result			add_realserver( virtualservice_element& );
	vs_operation_result			edit_realserver( virtualservice_element& );
	vs_operation_result			del_realserver( virtualservice_element& );

	void						run();
	void						stop();
//	void						pause();

	void						connection_active( const boost::asio::ip::tcp::endpoint& );
	void						connection_inactive( const boost::asio::ip::tcp::endpoint& );
	void						release_session( boost::thread::id thread_id );
};

class	virtualservice_udp : public virtualservice_base{
protected:
	void						rs_list_lock();
	void						rs_list_unlock();

	void						replication_interrupt();

	virtualservice_udp&	operator=( const virtualservice_udp& ){}
public:
	virtualservice_udp(		const l7vs::l7vsd& invsd,
							const l7vs::replication& inrep,
							const virtualservice_element& inelement);
	~virtualservice_udp();

	bool						operator==( const virtualservice_base& );
	bool						operator!=( const virtualservice_base& );

	vs_operation_result			set_virtualservce( virtualservice_element& );
	vs_operation_result			edit_virtualservce( virtualservice_element& );

	vs_operation_result			add_realserver( virtualservice_element& );
	vs_operation_result			edit_realserver( virtualservice_element& );
	vs_operation_result			del_realserver( virtualservice_element& );

	void						run();
	void						stop();
//	void						pause();

	void		connection_active( const boost::asio::ip::tcp::endpoint& );
	void		connection_inactive( const boost::asio::ip::tcp::endpoint& );
	void		release_session( boost::thread::id thread_id );
};

class	virtual_service{
protected:
	boost::shared_ptr<virtualservice_base>	vs;
public:
	virtual_service(	const l7vs::l7vsd& invsd,
						const l7vs::replication& inrep,
						const virtualservice_element& inelement ){
		if( inelement.udpmode )
			vs = boost::shared_ptr<virtualservice_base>(
					dynamic_cast<virtualservice_base*>( new virtualservice_udp( invsd, inrep, inelement ) ) );
		else
			vs = boost::shared_ptr<virtualservice_base>(
					dynamic_cast<virtualservice_base*>( new virtualservice_tcp( invsd, inrep, inelement ) ) );
	}
	~virtual_service(){
	}
	
	bool		operator==( const virtualservice_base& in ){ return vs->operator==( in ); }

	void		set_virtualservce( virtualservice_element& in ){ vs->set_virtualservce( in ); }
	void		edit_virtualservce( virtualservice_element& in ){ vs->edit_virtualservce( in ); }
	virtualservice_element&		get_element(){ return vs->get_element(); }

	void		run(){ vs->run(); }
	void		stop(){ vs->stop(); }
//	void		pause(){ vs->pause(); }
};

}

#endif//VIRTUALSERVICE_H
