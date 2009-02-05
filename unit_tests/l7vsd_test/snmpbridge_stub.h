#ifndef	__SNMPBRIDGE_H__
#define	__SNMPBRIDGE_H__
namespace	l7vs{
class	snmpbridge{
public:

//variable
	static	bool	initialize_called;
	static	bool	initialize_fail;

	static	bool	finalize_called;

//function
	//! constractor
	//! @param[in]
	//! @param[in]
	snmpbridge( l7vsd& l7vsd_in, boost::asio::io_service& io_service_in )
	{}

	//! destractor
	~snmpbridge(){}

	//! initilalize function
	//! @return
	int				initialize()
	{
		initialize_called = true;
		if(initialize_fail)	return -1;
		return 0;
	}

	//! finalize function
	void			finalize()
	{ finalize_called = true; }

	//! trap send function
	//! @param[in]
	//! @return
	int				send_trap( const std::string& message )
	{ return 0; }

	//! param reload function
	void			reload_config()
	{}

	//! change log level function
	//! @param[in]
	//! @param[in]
	//! @return
	int				change_loglevel( const LOG_CATEGORY_TAG snmp_log_category, const LOG_LEVEL_TAG loglevel )
	{ return 0; }

	//! view all log level function
	//! @param[in]
	//! @return
	int				change_loglevel_allcategory( const LOG_LEVEL_TAG loglevel )
	{ return 0; }

	//! send mibcollection
	//! @param[in]
	//! @return
	int				send_mibcollection( struct l7ag_mibrequest_message* payload )
	{ return 0; }

	//! connection status function
	//! @return
	bool				get_connectionstate()
	{ return true; }

	//! loglevel getting function
	//! @param[in]
	//! @return
	LOG_LEVEL_TAG	get_loglevel( const LOG_CATEGORY_TAG snmp_log_category )
	{ return LOG_LV_DEBUG; }

	//! loglevel getting function
	//! @param[out]
	//! @return
	void	get_loglevel_allcategory( std::map<LOG_CATEGORY_TAG, LOG_LEVEL_TAG>&	loglevelmap )
	{
		loglevelmap.insert( std::pair< LOG_CATEGORY_TAG, LOG_LEVEL_TAG >( LOG_CAT_SNMPAGENT_START_STOP, LOG_LV_FATAL ) );
	}

};	//class

}	//namespace	l7vs
#endif	//__SNMPBRIDGE_H__
