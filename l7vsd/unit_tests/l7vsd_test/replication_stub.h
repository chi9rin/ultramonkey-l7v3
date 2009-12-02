#ifndef	REPLICATION_H
#define	REPLICATION_H

#include <boost/thread.hpp>

namespace	l7vs{
class	replication{
public:
	enum REPLICATION_MODE_TAG{
		REPLICATION_OUT = 0,
		REPLICATION_SINGLE,
		REPLICATION_MASTER,
		REPLICATION_SLAVE,
		REPLICATION_MASTER_STOP,
		REPLICATION_SLAVE_STOP
	};

//variable
//	boost::asio::io_service&				receive_io;
	bool	switch_to_master_called;
	bool	switch_to_slave_called;

	static	bool	initialize_called;
	static	bool	initialize_fail;

	static	bool	finalize_called;

	static	bool	dump_memory_called;
	static	bool	start_called;
	static	bool	stop_called;
	static	bool	force_replicate_called;
	static	bool	reset_called;

//function
	replication()
				:	switch_to_master_called(false),
					switch_to_slave_called(false)
	{};

	int							initialize()
	{
		initialize_called = true;
		if(initialize_fail)	return -1;
		return 0;
	}

	void						finalize()
	{ finalize_called = true; }

	void						switch_to_master()
	{ switch_to_master_called = true; }

	void						switch_to_slave()
	{ switch_to_slave_called = true; }

	void*						pay_memory( std::string& inid, unsigned int& outsize );
	void						dump_memory()
	{ dump_memory_called = true; }
	void						start()
	{ start_called = true; }
	void						stop()
	{ stop_called = true; }
	void						force_replicate()
	{ force_replicate_called = true; }
	void						reset()
	{ reset_called = true; }
	REPLICATION_MODE_TAG		get_status()
	{ return REPLICATION_MASTER; }

	int							check_interval();
	int							handle_send();
	int							handle_receive();
	int							lock( std::string& inid );
	int							unlock( std::string& inid );
	int							refer_lock_mutex( std::string& inid, boost::mutex& outmutex );
};	//class

}	//namespace	l7vs
#endif	//REPLICATION_H
