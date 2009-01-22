#ifndef	VIRTUALSERVICE_H
#define	VIRTUALSERVICE_H

#include "virtualservice_element.h"
#include "replication_stub.h"
#include "boost/thread.hpp"

namespace	l7vs{
class	l7vsd;

class	virtualservice_base : boost::noncopyable{
public:
	struct	vs_operation_result{
		bool			flag;
		std::string		message;
		vs_operation_result() : flag(true), message(""){}
		bool	operator==( const vs_operation_result& in )
					{ return ( ( flag == in.flag ) && ( message == in.message ) ); }
		bool	operator!=( const vs_operation_result& in )
					{ return ( ( flag != in.flag ) || ( message != in.message ) ); }
		bool	operator!() const
					{ return !flag; }
		typedef void (*unspecified_bool_type)();
		static void unspecified_bool_true() {}
		operator unspecified_bool_type() const { return flag == 0 ? 0 : unspecified_bool_true; }
	};
};

class	virtual_service{
public:

//variable
	virtualservice_element		element;

	//for stub
	bool	initialize_called;
	bool	set_virtualservice_called;
	bool	run_called;

//function
	virtual_service(	const l7vs::l7vsd& invsd,
						const l7vs::replication& inrep,
						const virtualservice_element& inelement )
				:	initialize_called(false),
					set_virtualservice_called(false),
					run_called(false)
				{}

	~virtual_service()	{}
	
	virtualservice_base::vs_operation_result
								initialize()
	{
		virtualservice_base::vs_operation_result res;
		initialize_called = true;
		return res;
	}

	virtualservice_base::vs_operation_result
								finalize()
	{
	}

// 	bool						operator==( const virtualservice_base& in );
// 	bool						operator!=( const virtualservice_base& in );

	virtualservice_base::vs_operation_result
								set_virtualservice( const virtualservice_element& in )
	{
		virtualservice_base::vs_operation_result res;
		element = in;
		set_virtualservice_called = true;
		return res;
	}

	virtualservice_base::vs_operation_result
								edit_virtualservice( const virtualservice_element& in )
	{}

	virtualservice_base::vs_operation_result
								add_realserver( const virtualservice_element& in )
	{}
	virtualservice_base::vs_operation_result
								edit_realserver( const virtualservice_element& in )
	{}
	virtualservice_base::vs_operation_result
								del_realserver( const virtualservice_element& in )
	{}

	virtualservice_element&		get_element()
	{ return element; }

	void						run()
	{ run_called = true; }
	void						stop()
	{}

	void		connection_active( const boost::asio::ip::tcp::endpoint& in );
	void		connection_inactive( const boost::asio::ip::tcp::endpoint& in );
	void		release_session( const boost::thread::id thread_id );

	unsigned long long			get_qos_upstream();
	unsigned long long			get_qos_downstream();
	unsigned long long			get_throughput_upstream();
	unsigned long long			get_throughput_downstream();

	void						update_up_recv_size( unsigned long long	datasize );
	void						update_up_send_size( unsigned long long	datasize );
	void						update_down_recv_size( unsigned long long	datasize );
	void						update_down_send_size( unsigned long long	datasize );
	
// 	boost::shared_ptr<protocol_module_base>
// 								get_protocol_module();
// 	boost::shared_ptr<schedule_module_base>
// 								get_schedule_module();

};
}	//namespace	l7vs
#endif	//VIRTUALSERVICE_H
