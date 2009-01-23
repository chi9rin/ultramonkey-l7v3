#ifndef	VIRTUALSERVICE_H
#define	VIRTUALSERVICE_H

#include "virtualservice_element.h"
#include "replication_stub.h"
#include "boost/thread.hpp"

namespace	l7vs{
class	l7vsd;

class	virtual_service{
public:

//variable
	virtualservice_element		element;

	//for stub
	bool	initialize_called;
	bool	set_virtualservice_called;
	bool	run_called;

//function
	virtual_service(	const l7vs::l7vsd& ,
						const l7vs::replication& ,
						const virtualservice_element& )
				:	initialize_called(false),
					set_virtualservice_called(false),
					run_called(false)
				{}

	~virtual_service()	{}

	void						initialize( error_code& )
	{
		initialize_called = true;
	}
	void						finalize( error_code& )
	{
	}
	

//	bool						operator==( const virtualservice_base& );
//	bool						operator!=( const virtualservice_base& );


	void						set_virtualservice( const virtualservice_element& in, error_code& ec )
	{
		element = in;
		set_virtualservice_called = true;
	}

	void						edit_virtualserivce( const virtualservice_element& , error_code& )
	{
	}

	void						add_realserver( const virtualservice_element& , error_code& )
	{
	}

	void						edit_realserver( const virtualservice_element& , error_code& )
	{
	}

	void						del_realserver( const virtualservice_element& , error_code& )
	{
	}

	virtualservice_element&		get_element()
	{ return element; }

	void						run()
	{ run_called = true; }
	void						stop()
	{}

// 	void		connection_active( const boost::asio::ip::tcp::endpoint& in );
// 	void		connection_inactive( const boost::asio::ip::tcp::endpoint& in );
// 	void		release_session( const boost::thread::id thread_id );
// 
// 	void		connection_active( const boost::asio::ip::tcp::endpoint&  );
// 	void		connection_inactive( const boost::asio::ip::tcp::endpoint&  );
// 	void		release_session( const boost::thread::id );

	unsigned long long			get_qos_upstream();
	unsigned long long			get_qos_downstream();
	unsigned long long			get_throughput_upstream();
	unsigned long long			get_throughput_downstream();

	void						update_up_recv_size( unsigned long long );
	void						update_up_send_size( unsigned long long );
	void						update_down_recv_size( unsigned long long );
	void						update_down_send_size( unsigned long long );
	
// 	boost::shared_ptr<protocol_module_base>
// 								get_protocol_module();
// 	boost::shared_ptr<schedule_module_base>
// 								get_schedule_module();

};
}	//namespace	l7vs
#endif	//VIRTUALSERVICE_H
