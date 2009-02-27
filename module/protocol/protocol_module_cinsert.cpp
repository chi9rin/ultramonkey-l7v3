#include <vector>
#include <list>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/xpressive/xpressive.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/regex.hpp>
#include <boost/format.hpp>
#include "protocol_module_cinsert.h"

namespace l7vs
{
//! constractor
protocol_module_cinsert::protocol_module_cinsert() : http_protocol_module_base( "cinsert" )
{
}

//! destractor
protocol_module_cinsert::~protocol_module_cinsert()
{
}

//! tcp protocol support check
//! @return tcp support is true
//! @return tcp not-support is false
bool	protocol_module_cinsert::is_tcp()
{
	//---------- DEBUG LOG START ------------------------------ 
	if( LOG_LV_DEBUG == getloglevel())
	{
		putLogDebug(	0,
						"function in  : [is_tcp]",
						__FILE__,
						__LINE__ );
		putLogDebug(	0,
						"function out : [is_tcp] : return = [true]",
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------
	return	true;
}

bool	protocol_module_cinsert::is_udp()
{
	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		putLogDebug(	0,
						"function in  : [is_udp]",
						__FILE__,
						__LINE__ );
		putLogDebug(	0,
						"function out : [is_udp] : return = [false]",
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------
	return	false;
}

void	protocol_module_cinsert::replication_interrupt()
{
	// NOP
	return;
}

void	protocol_module_cinsert::initialize(
				rs_list_itr_func_type			inlist_begin,
				rs_list_itr_func_type			inlist_end,
				rs_list_itr_next_func_type		inlist_next,
				boost::function< void( void ) >	inlist_lock,
				boost::function< void( void ) >	inlist_unlock )
{
	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function in  : [initialize] : "
									"inlist_begin = [%p], "
									"inlist_end = [%p], "
									"inlist_next = [%p], "
									"inlist_lock = [%p], "
									"inlist_unlock = [%p]");

		outform % &inlist_begin % &inlist_end % &inlist_next % &inlist_lock % &inlist_unlock;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	rs_list_begin	= inlist_begin;
	rs_list_end		= inlist_end;
	rs_list_next	= inlist_next;
	rs_list_lock	= inlist_lock;
	rs_list_unlock	= inlist_unlock;

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function out : [initialize] : "
									"rs_list_begin = [%p], "
									"rs_list_end = [%p], "
									"rs_list_next = [%p], "
									"rs_list_lock = [%p], "
									"rs_list_unlock = [%p]");

		outform % &rs_list_begin % &rs_list_end % &rs_list_next % &rs_list_lock % &rs_list_unlock;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

}

void	protocol_module_cinsert::finalize()
{
	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		putLogDebug(	0,
						"function in  : [finalize]",
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	rs_list_begin.clear();
	rs_list_end.clear();
	rs_list_next.clear();
	rs_list_lock.clear();
	rs_list_unlock.clear();

	replication_pay_memory.clear();
	replication_area_lock.clear();
	replication_area_unlock.clear();

	schedule_tcp.clear();
	schedule_udp.clear();

	cookie_expire	= 0;
	forwarded_for	= 0;
	reschedule		= 0;
	cookie_name.assign( '\0' );
	sorry_uri.assign( '\0' );

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		putLogDebug(	0,
						"function out : [finalize]",
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	getloglevel.clear();
	putLogFatal.clear();
	putLogError.clear();
	putLogWarn.clear();
	putLogInfo.clear();
	putLogDebug.clear();

}

//---------------------------------------------------------------------------------
bool	protocol_module_cinsert::is_use_sorry()
{
	return	true;
}

//---------------------------------------------------------------------------------
protocol_module_cinsert::check_message_result
protocol_module_cinsert::check_parameter( const std::vector< std::string >& args )
{

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function in  : [check_parameter] : "
									"args.size = [%d], "
									"args = [%s] ");

		std::string	str = "";

		for( unsigned int i = 0; i < args.size(); i++ )
		{
			if( i != 0 )
			{
				str += ", ";
			}
			str += "{";
			str += args[i];
			str += "}";
		}

		outform % args.size() % str;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	using namespace boost::xpressive;

	check_message_result check_result;

	bool cookie_name_set_flag	= false;
	bool cookie_expire_set_flag	= false;
	bool forwarded_for_set_flag	= false;
	bool reschedule_set_flag	= false;
	bool no_reschedule_set_flag	= false;
	bool sorry_uri_set_flag		= false;

	std::vector<std::string>::const_iterator args_itr;

	bool	match_result;

	sregex	cookie_name_regex = +( alpha | digit );
	sregex	cookie_expire_regex = +digit;
	sregex	sorry_uri_regex
				=	+(	'/' >>
						*(	alpha |
							digit |
							( set = ';', ':', '@', '&', '=' ) |
							( set = '$', '-', '_', '.', '+' ) |
							( set = '!', '*', '\'', '\(', ')', ',' ) |
							'%' >> repeat<2>(xdigit)));

	int	cookie_expire_tmp;

	try
	{

		check_result.flag = true;

		args_itr = args.begin();

		while( args_itr != args.end())
		{

			if( *args_itr == "-C" || *args_itr == "--cookie-name" )
			{

				if( cookie_name_set_flag == false )
				{

					args_itr++;

					if( args_itr  != args.end())
					{

						if( args_itr->length() > 1 && args_itr->substr(0,1) == "-" )
						{
							check_result.flag = false;

							check_result.message = "You have to set option value '-C/--cookie-name'.";

							break;
						}
						else
						{

							if( args_itr->length() > MAX_OPTION_SIZE - 1 )
							{
								check_result.flag = false;
	
								check_result.message =	"'-C/--cookie-name' option value '";
								check_result.message +=	*args_itr;
								check_result.message +=	"' is too long.";
	
								break;
							}
							else
							{
	
								match_result = regex_match(	*args_itr, cookie_name_regex );
		
								if( match_result == false )
								{
									check_result.flag = false;
		
									check_result.message =	"'-C/--cookie-name' option value '";
									check_result.message +=	*args_itr;
									check_result.message +=	"' is not alphanumeric character.";
		
									break;
								}
								else
								{
									cookie_name_set_flag = true;
								}
							}
						}
					}
					else
					{
						check_result.flag = false;

						check_result.message = "You have to set option value '-C/--cookie-name'.";

						break;
					}
				}
				else
				{
					check_result.flag = false;

					check_result.message = "Cannot set multiple option '-C/--cookie-name'.";

					break;
				}
			}
			else if( *args_itr == "-E" || *args_itr == "--cookie-expire" )
			{
				if( cookie_expire_set_flag == false )
				{

					args_itr++;

					if( args_itr  != args.end())
					{

						if( args_itr->length() > 1 && args_itr->substr(0,1) == "-" )
						{
							check_result.flag = false;

							check_result.message = "You have to set option value '-E/--cookie-expire'.";

							break;
						}
						else
						{
	
							match_result = regex_match(	*args_itr, cookie_expire_regex );
	
							if( match_result == false )
							{
								check_result.flag = false;
	
								check_result.message =	"'-E/--cookie-expire' option value '";
								check_result.message +=	*args_itr;
								check_result.message +=	"' is not numeric character.";
	
								break;
							}
							else
							{
								try
								{
									cookie_expire_tmp = boost::lexical_cast<int>( *args_itr );
								} catch ( const boost::bad_lexical_cast& ex )
								{
									check_result.flag = false;
		
									check_result.message =	"'-E/--cookie-expire' option value '";
									check_result.message +=	*args_itr;
									check_result.message +=	"' is too large.";
		
									break;
								}

								cookie_expire_set_flag = true;
							}
						}
					}
					else
					{
						check_result.flag = false;

						check_result.message = "You have to set option value '-E/--cookie-expire'.";

						break;
					}
				}
				else
				{
					check_result.flag = false;

					check_result.message = "Cannot set multiple option '-E/--cookie-expire'.";

					break;
				}
			}
			else if( *args_itr == "-F" || *args_itr == "--forwarded-for" )
			{
				forwarded_for_set_flag = true;
			}
			else if( *args_itr == "-R" || *args_itr == "--reschedule" )
			{
				if( no_reschedule_set_flag == false )
				{

					reschedule_set_flag = true;

				}
				else
				{
					check_result.flag = false;

					check_result.message = "You have to choose either of reschedule or no-reschedule.";

					break;
				}

			}
			else if( *args_itr == "-N" || *args_itr == "--no-reschedule" )
			{
				if( reschedule_set_flag == false )
				{

					no_reschedule_set_flag = true;

				}
				else
				{
					check_result.flag = false;

					check_result.message = "You have to choose either of reschedule or no-reschedule.";

					break;
				}

			}
			else if( *args_itr == "-S" || *args_itr == "--sorry-uri" )
			{

				if( sorry_uri_set_flag == false )
				{

					args_itr++;

					if( args_itr  != args.end())
					{

						if( args_itr->length() > 1 && args_itr->substr(0,1) == "-" )
						{
							check_result.flag = false;

							check_result.message = "You have to set option value '-S/--sorry-uri'.";

							break;
						}
						else
						{

							if( args_itr->length() > MAX_OPTION_SIZE - 1 )
							{
								check_result.flag = false;
	
								check_result.message =	"'-S/--sorry-uri' option value '";
								check_result.message +=	*args_itr;
								check_result.message +=	"' is too long.";
	
								break;
							}
							else
							{
	
								match_result = regex_match(	*args_itr, sorry_uri_regex );
		
								if( match_result == false )
								{
									check_result.flag = false;
		
									check_result.message =	"'-S/--sorry-uri' option value '";
									check_result.message +=	*args_itr;
									check_result.message +=	"' is not a valid URI.";
		
									break;
								}
								else
								{
									sorry_uri_set_flag = true;
								}
							}
						}
					}
					else
					{
						check_result.flag = false;

						check_result.message = "You have to set option value '-S/--sorry-uri'.";

						break;
					}
				}
				else
				{
					check_result.flag = false;

					check_result.message = "Cannot set multiple option '-S/--sorry-uri'.";

					break;
				}
			}
			else
			{
					check_result.flag = false;

					check_result.message = "Option error.";

					break;
			}

			args_itr++;

		}

	} catch (...)
	{
		check_result.flag = false;
		check_result.message = "Option error.";
	}

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function out : [check_parameter] : "
									"check_result.flag = [%d], "
									"check_result.message = [%s] ");

		outform % check_result.flag % check_result.message;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	return	check_result;
}

//---------------------------------------------------------------------------------
protocol_module_cinsert::check_message_result
protocol_module_cinsert::set_parameter( const std::vector< std::string >& args )
{

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function in  : [set_parameter] : "
									"args.size = [%d], "
									"args = [%s] ");

		std::string	str = "";

		for( unsigned int i = 0; i < args.size(); i++ )
		{
			if( i != 0 )
			{
				str += ", ";
			}
			str += "{";
			str += args[i];
			str += "}";
		}

		outform % args.size() % str;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	using namespace boost::xpressive;

	check_message_result check_result;

	bool cookie_name_set_flag	= false;
	bool cookie_expire_set_flag	= false;
	bool forwarded_for_set_flag	= false;
	bool reschedule_set_flag	= false;
	bool no_reschedule_set_flag	= false;
	bool sorry_uri_set_flag		= false;

	std::vector<std::string>::const_iterator args_itr;

	bool	match_result;

	sregex	cookie_name_regex = +( alpha | digit );
	sregex	cookie_expire_regex = +digit;
	sregex	sorry_uri_regex
				=	+(	'/' >>
						*(	alpha |
							digit |
							( set = ';', ':', '@', '&', '=' ) |
							( set = '$', '-', '_', '.', '+' ) |
							( set = '!', '*', '\'', '\(', ')', ',' ) |
							'%' >> repeat<2>(xdigit)));

	try
	{

		check_result.flag = true;

		args_itr = args.begin();

		while( args_itr != args.end())
		{

			if( *args_itr == "-C" || *args_itr == "--cookie-name" )
			{

				if( cookie_name_set_flag == false )
				{

					args_itr++;

					if( args_itr  != args.end())
					{

						if( args_itr->length() > 1 && args_itr->substr(0,1) == "-" )
						{
							check_result.flag = false;

							check_result.message = "You have to set option value '-C/--cookie-name'.";

							break;
						}
						else
						{

							if( args_itr->length() > MAX_OPTION_SIZE - 1 )
							{
								check_result.flag = false;
	
								check_result.message =	"'-C/--cookie-name' option value '";
								check_result.message +=	*args_itr;
								check_result.message +=	"' is too long.";
	
								break;
							}
							else
							{
	
								match_result = regex_match(	*args_itr, cookie_name_regex );
		
								if( match_result == false )
								{
									check_result.flag = false;
		
									check_result.message =	"'-C/--cookie-name' option value '";
									check_result.message +=	*args_itr;
									check_result.message +=	"' is not alphanumeric character.";
		
									break;
								}
								else
								{

									cookie_name.assign('\0');
									memcpy( cookie_name.data(), args_itr->c_str(), args_itr->length()); 
			
									cookie_name_set_flag = true;
								}
							}
						}
					}
					else
					{
						check_result.flag = false;

						check_result.message = "You have to set option value '-C/--cookie-name'.";

						break;
					}
				}
				else
				{
					check_result.flag = false;

					check_result.message = "Cannot set multiple option '-C/--cookie-name'.";

					break;
				}
			}
			else if( *args_itr == "-E" || *args_itr == "--cookie-expire" )
			{
				if( cookie_expire_set_flag == false )
				{

					args_itr++;

					if( args_itr  != args.end())
					{

						if( args_itr->length() > 1 && args_itr->substr(0,1) == "-" )
						{
							check_result.flag = false;

							check_result.message = "You have to set option value '-E/--cookie-expire'.";

							break;
						}
						else
						{
	
							match_result = regex_match(	*args_itr, cookie_expire_regex );
	
							if( match_result == false )
							{
								check_result.flag = false;
	
								check_result.message =	"'-E/--cookie-expire' option value '";
								check_result.message +=	*args_itr;
								check_result.message +=	"' is not numeric character.";
	
								break;
							}
							else
							{
								try
								{
									cookie_expire = boost::lexical_cast<int>( *args_itr );
								} catch ( const boost::bad_lexical_cast& ex )
								{
									check_result.flag = false;
		
									check_result.message =	"'-E/--cookie-expire' option value '";
									check_result.message +=	*args_itr;
									check_result.message +=	"' is too large.";
		
									break;
								}

								cookie_expire_set_flag = true;
							}
						}
					}
					else
					{
						check_result.flag = false;

						check_result.message = "You have to set option value '-E/--cookie-expire'.";

						break;
					}
				}
				else
				{
					check_result.flag = false;

					check_result.message = "Cannot set multiple option '-E/--cookie-expire'.";

					break;
				}
			}
			else if( *args_itr == "-F" || *args_itr == "--forwarded-for" )
			{
				forwarded_for = 1;
				forwarded_for_set_flag = true;
			}
			else if( *args_itr == "-R" || *args_itr == "--reschedule" )
			{
				if( no_reschedule_set_flag == false )
				{
					reschedule = 1;

					reschedule_set_flag = true;

				}
				else
				{
					check_result.flag = false;

					check_result.message = "You have to choose either of reschedule or no-reschedule.";

					break;
				}

			}
			else if( *args_itr == "-N" || *args_itr == "--no-reschedule" )
			{
				if( reschedule_set_flag == false )
				{
					reschedule = 0;

					no_reschedule_set_flag = true;

				}
				else
				{
					check_result.flag = false;

					check_result.message = "You have to choose either of reschedule or no-reschedule.";

					break;
				}

			}
			else if( *args_itr == "-S" || *args_itr == "--sorry-uri" )
			{

				if( sorry_uri_set_flag == false )
				{

					args_itr++;

					if( args_itr  != args.end())
					{

						if( args_itr->length() > 1 && args_itr->substr(0,1) == "-" )
						{
							check_result.flag = false;

							check_result.message = "You have to set option value '-S/--sorry-uri'.";

							break;
						}
						else
						{

							if( args_itr->length() > MAX_OPTION_SIZE - 1 )
							{
								check_result.flag = false;
	
								check_result.message =	"'-S/--sorry-uri' option value '";
								check_result.message +=	*args_itr;
								check_result.message +=	"' is too long.";
	
								break;
							}
							else
							{
	
								match_result = regex_match(	*args_itr, sorry_uri_regex );
		
								if( match_result == false )
								{
									check_result.flag = false;
		
									check_result.message =	"'-S/--sorry-uri' option value '";
									check_result.message +=	*args_itr;
									check_result.message +=	"' is not a valid URI.";
		
									break;
								}
								else
								{

									sorry_uri.assign('\0');
									memcpy( sorry_uri.data(), args_itr->c_str(), args_itr->length()); 
			
									sorry_uri_set_flag = true;
								}
							}
						}
					}
					else
					{
						check_result.flag = false;

						check_result.message = "You have to set option value '-S/--sorry-uri'.";

						break;
					}
				}
				else
				{
					check_result.flag = false;

					check_result.message = "Cannot set multiple option '-S/--sorry-uri'.";

					break;
				}
			}
			else
			{
					check_result.flag = false;

					check_result.message = "Option error.";

					break;
			}

			args_itr++;

		}

		if( check_result.flag == true )
		{

			if( cookie_name_set_flag == false )
			{
				memcpy( cookie_name.data(), "CookieName\0", 12);
			}

			if( cookie_expire_set_flag == false )
			{
				cookie_expire = 86400; 
			}

			if( forwarded_for_set_flag == false )
			{
				forwarded_for = 0; 
			}

			if( reschedule_set_flag == false )
			{
				reschedule = 0; 
			}

			if( sorry_uri_set_flag == false )
			{
				sorry_uri.assign('\0');
			}

		}

	} catch (...)
	{
		check_result.flag = false;
		check_result.message = "Option error.";
	}

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function out : [set_parameter] : "
									"check_result.flag = [%d], "
									"check_result.message = [%s] ");

		outform % check_result.flag % check_result.message;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	return check_result;
}

//---------------------------------------------------------------------------------
protocol_module_cinsert::check_message_result
protocol_module_cinsert::add_parameter( const std::vector< std::string >& args )
{

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function in  : [add_parameter] : "
									"args.size = [%d], "
									"args = [%s] ");

		std::string	str = "";

		for( unsigned int i = 0; i < args.size(); i++ )
		{
			if( i != 0 )
			{
				str += ", ";
			}
			str += "{";
			str += args[i];
			str += "}";
		}

		outform % args.size() % str;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	check_message_result check_result;

	try
	{

		check_result.flag = true;

		if( args.empty() == false )
		{

			check_result.flag = false;

			check_result.message = "Cannot add option.";

		}

	} catch (...)
	{
		check_result.flag = false;
		check_result.message = "Cannot add option.";
	}

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function out : [add_parameter] : "
									"check_result.flag = [%d], "
									"check_result.message = [%s] ");

		outform % check_result.flag % check_result.message;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	return check_result;
}

//---------------------------------------------------------------------------------
void	protocol_module_cinsert::handle_rslist_update()
{
	// NOP
	return;
}

//---------------------------------------------------------------------------------
void	protocol_module_cinsert::register_schedule( tcp_schedule_func_type inschedule )
{

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function in  : [register_schedule] : "
									"inschedule = [%p]");

		outform % &inschedule;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	schedule_tcp = inschedule;

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function out : [register_schedule] : "
									"schedule_tcp = [%p]");

		outform % &schedule_tcp;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------
}

//---------------------------------------------------------------------------------
void	protocol_module_cinsert::register_schedule( udp_schedule_func_type inschedule )
{
	// NOP
	return;
}

//---------------------------------------------------------------------------------
protocol_module_cinsert::EVENT_TAG
protocol_module_cinsert::handle_session_initialize(
				const boost::thread::id up_thread_id,
				const boost::thread::id down_thread_id,
				const boost::asio::ip::tcp::endpoint& client_endpoint_tcp,
				const boost::asio::ip::udp::endpoint& client_endpoint_udp )
{

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function in  : [handle_session_initialize] : "
									"up_thread_id = [%s], "
									"down_thread_id = [%s], "
									"client_endpoint_tcp = [%s], "
									"client_endpoint_udp = [%s]");

		outform % up_thread_id % down_thread_id % client_endpoint_tcp % client_endpoint_udp;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	recive_data						client_recv_data;
	char*							buffer				= NULL;

	try
	{
		t_session_thread_data_cinsert	up_thread_data( new session_thread_data_cinsert );

		up_thread_data->thread_id				= up_thread_id;
		up_thread_data->thread_division			= THREAD_DIVISION_UP_STREAM;
		up_thread_data->pair_thread_id			= down_thread_id;
		up_thread_data->client_endpoint_tcp		= client_endpoint_tcp;

		buffer = new char[client_recv_data.recive_buffer_max_size];

		client_recv_data.recive_buffer		= buffer;
		client_recv_data.recive_buffer_1	= buffer;

		buffer = new char[client_recv_data.recive_buffer_max_size];

		client_recv_data.recive_buffer_2	= buffer;

		up_thread_data->recive_data_map[ client_endpoint_tcp ] = client_recv_data;

		client_recv_data.recive_buffer_1	= NULL;
		client_recv_data.recive_buffer_2	= NULL; 

		t_session_thread_data_cinsert	down_thread_data( new session_thread_data_cinsert );

		down_thread_data->thread_id					= down_thread_id;
		down_thread_data->thread_division			= THREAD_DIVISION_DOWN_STREAM;
		down_thread_data->pair_thread_id			= up_thread_id;
		down_thread_data->client_endpoint_tcp		= client_endpoint_tcp;

		{
			boost::mutex::scoped_lock	lock( session_thread_data_map_mutex );
			session_thread_data_map[ up_thread_id ]		= up_thread_data;
			session_thread_data_map[ down_thread_id ]	= down_thread_data;
		}

	} catch (...)
	{
		//---------- DEBUG LOG START ------------------------------
		if( LOG_LV_DEBUG == getloglevel())
		{
			boost::format	outform(	"function out : [handle_session_initialize] : "
										"up_thread_id = [%s], "
										"down_thread_id = [%s], "
										"next_status = [%d]");
	
			outform % up_thread_id % down_thread_id % FINALIZE;
	
			putLogDebug(	0,
							outform.str(),
							__FILE__,
							__LINE__ );
		}
		//---------- DEBUG LOG END ------------------------------

		return	FINALIZE;
	}

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function out : [handle_session_initialize] : "
									"up_thread_id = [%s], "
									"down_thread_id = [%s], "
									"next_status = [%d]");

		outform % up_thread_id % down_thread_id % ACCEPT;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	return	ACCEPT;

}

//---------------------------------------------------------------------------------
protocol_module_cinsert::EVENT_TAG
protocol_module_cinsert::handle_session_finalize(
				const boost::thread::id up_thread_id,
				const boost::thread::id down_thread_id )
{

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function in  : [handle_session_finalize] : "
									"up_thread_id = [%s], "
									"down_thread_id = [%s]");

		outform % up_thread_id % down_thread_id;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	t_session_thread_data_map_itr	thread_data_itr;

	try
	{

		boost::mutex::scoped_lock	lock( session_thread_data_map_mutex );

		thread_data_itr = session_thread_data_map.find( up_thread_id );

		if( thread_data_itr != session_thread_data_map.end() )
		{
			session_thread_data_map.erase( thread_data_itr );
		}

		thread_data_itr = session_thread_data_map.find( down_thread_id );

		if( thread_data_itr != session_thread_data_map.end() )
		{
			session_thread_data_map.erase( thread_data_itr );
		}

	} catch (...)
	{

		//---------- DEBUG LOG START ------------------------------
		if( LOG_LV_DEBUG == getloglevel())
		{
			boost::format	outform(	"function out : [handle_session_finalize] : "
										"up_thread_id = [%s], "
										"down_thread_id = [%s], "
										"next_status = [%d]");
	
			outform % up_thread_id % down_thread_id % STOP;
	
			putLogDebug(	0,
							outform.str(),
							__FILE__,
							__LINE__ );
		}
		//---------- DEBUG LOG END ------------------------------

		return	STOP;
	}

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function out : [handle_session_finalize] : "
									"up_thread_id = [%s], "
									"down_thread_id = [%s], "
									"next_status = [%d]");

		outform % up_thread_id % down_thread_id % STOP;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	return	STOP;
}

//---------------------------------------------------------------------------------
protocol_module_cinsert::EVENT_TAG
protocol_module_cinsert::handle_accept( const boost::thread::id thread_id )
{

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function in  : [handle_accept] : "
									"thread_id = [%s]");

		outform % thread_id;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	t_session_thread_data_cinsert	thread_data;
	t_session_thread_data_map_itr	thread_data_itr;

	EVENT_TAG	status = STOP;

	try
	{
		{
			boost::mutex::scoped_lock	lock( session_thread_data_map_mutex );

			thread_data_itr = session_thread_data_map.find( thread_id );

			if( thread_data_itr != session_thread_data_map.end() )
			{
				thread_data = thread_data_itr->second;
			}
		}

		if( thread_data != NULL )
		{

			thread_data->accept_end_flag = ACCEPT_END_FLAG_ON;

			status = CLIENT_RECV;

		}
	} catch (...)
	{

		//---------- DEBUG LOG START ------------------------------
		if( LOG_LV_DEBUG == getloglevel())
		{
			boost::format	outform(	"function out : [handle_accept] : "
										"thread_id = [%s], "
										"next_status = [%d]");
	
			outform % thread_id % FINALIZE;
	
			putLogDebug(	0,
							outform.str(),
							__FILE__,
							__LINE__ );
		}
		//---------- DEBUG LOG END ------------------------------

		return	FINALIZE;
	}

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function out : [handle_accept] : "
									"thread_id = [%s], "
									"next_status = [%d]");

		outform % thread_id % status;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	return	status;

}

//---------------------------------------------------------------------------------
protocol_module_cinsert::EVENT_TAG
protocol_module_cinsert::handle_client_recv(
				const boost::thread::id thread_id,
				const boost::array< char, MAX_BUFFER_SIZE >& recvbuffer,
				const size_t recvlen )
{

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function in  : [handle_client_recv] : "
									"thread_id = [%s], "
									"recvlen = [%d]");

		outform % thread_id % recvlen;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	using namespace boost::xpressive;

	t_session_thread_data_cinsert	thread_data;

	char*	buffer_1			= NULL;
	char*	buffer_2			= NULL;

	size_t	unsend_data_offset		= 0;
	size_t	unsend_data_size		= 0;
	size_t	rest_request_data_size	= 0;
	size_t	next_request_offset		= 0;

	send_status			send_status_add;

	CHECK_RESULT_TAG	check_result;
	bool				find_result;
	size_t				http_header_offset	= 0;
	size_t				http_header_len		= 0;
	std::string			http_header_name_blank = "";
	std::string			http_header_name_content_length = "Content-Length";
	std::string			content_length;
	match_results< const char* >	regex_result;
	cregex	content_length_regex = icase("Content-Length") >> ":" >> *~_d >> (s1 = +_d) >> *~_d;

	t_session_thread_data_map_itr	thread_data_itr;
	t_recive_data_map_itr			recive_data_itr;
	t_send_status_list_itr			send_status_itr;

	EVENT_TAG	status = STOP;

	try
	{
		{
			boost::mutex::scoped_lock	lock( session_thread_data_map_mutex );

			thread_data_itr = session_thread_data_map.find( thread_id );

			if( thread_data_itr != session_thread_data_map.end() )
			{
				thread_data = thread_data_itr->second;
			}
		}
		if( thread_data != NULL )
		{

			if( thread_data->end_flag == END_FLAG_ON )
			{
				status = CLIENT_RECV;
			}
			else
			{
				recive_data_itr = thread_data->recive_data_map.find( thread_data->client_endpoint_tcp );

				if( recive_data_itr != thread_data->recive_data_map.end() )
				{

					send_status_itr = recive_data_itr->second.send_status_list.begin();

					while( send_status_itr != recive_data_itr->second.send_status_list.end())
					{

						if( send_status_itr->status == SEND_END )
						{

							next_request_offset = send_status_itr->send_offset + send_status_itr->send_end_size;
							send_status_itr = recive_data_itr->second.send_status_list.erase( send_status_itr );
						}
						else if( send_status_itr->status == SEND_CONTINUE )
						{

							send_status_itr->send_offset += send_status_itr->send_end_size;
							unsend_data_offset = send_status_itr->send_offset;
							break;
						}
						else
						{

							unsend_data_offset = send_status_itr->send_offset;
							unsend_data_size = send_status_itr->unsend_size;
							break;
						}

					}

					if( recive_data_itr->second.recive_buffer_rest_size < recvlen )
					{

						if( recive_data_itr->second.recive_buffer_max_size < unsend_data_size + recvlen )
						{

							buffer_1 = new char[unsend_data_size + recvlen];
							buffer_2 = new char[unsend_data_size + recvlen];

							memset( buffer_1, '\0', unsend_data_size + recvlen );
							memset( buffer_2, '\0', unsend_data_size + recvlen );

							memcpy(	buffer_1,
									recive_data_itr->second.recive_buffer + unsend_data_offset,
									unsend_data_size );

							memcpy(	buffer_1 + unsend_data_size,
									recvbuffer.data(),
									recvlen );

							free( recive_data_itr->second.recive_buffer_1 );
							free( recive_data_itr->second.recive_buffer_2 );

							recive_data_itr->second.recive_buffer	= buffer_1;
							recive_data_itr->second.recive_buffer_1	= buffer_1;
							recive_data_itr->second.recive_buffer_2	= buffer_2;

							recive_data_itr->second.recive_buffer_max_size = unsend_data_size + recvlen;

						}
						else
						{

							if( recive_data_itr->second.recive_buffer == recive_data_itr->second.recive_buffer_1 )
							{

								memset( recive_data_itr->second.recive_buffer_2,
										'\0',
										recive_data_itr->second.recive_buffer_max_size );

								memcpy(	recive_data_itr->second.recive_buffer_2,
										recive_data_itr->second.recive_buffer + unsend_data_offset,
										unsend_data_size );

								memcpy(	recive_data_itr->second.recive_buffer_2 + unsend_data_size,
										recvbuffer.data(),
										recvlen );

								recive_data_itr->second.recive_buffer = recive_data_itr->second.recive_buffer_2;

							}
							else
							{

								memset( recive_data_itr->second.recive_buffer_1,
										'\0',
										recive_data_itr->second.recive_buffer_max_size );

								memcpy(	recive_data_itr->second.recive_buffer_1,
										recive_data_itr->second.recive_buffer + unsend_data_offset,
										unsend_data_size );

								memcpy(	recive_data_itr->second.recive_buffer_1 + unsend_data_size,
										recvbuffer.data(),
										recvlen );

								recive_data_itr->second.recive_buffer = recive_data_itr->second.recive_buffer_1;

							}

						}

						recive_data_itr->second.recive_buffer_rest_size
							= recive_data_itr->second.recive_buffer_max_size - unsend_data_size - recvlen;

						next_request_offset = 0;

						unsend_data_size += recvlen;

						send_status_itr = recive_data_itr->second.send_status_list.begin();

						while( send_status_itr != recive_data_itr->second.send_status_list.end())
						{

							send_status_itr->send_offset -= unsend_data_offset;
							send_status_itr++;
						}

					}
					else
					{

						memcpy(	recive_data_itr->second.recive_buffer + recive_data_itr->second.recive_buffer_max_size - recive_data_itr->second.recive_buffer_rest_size,
								recvbuffer.data(),
								recvlen );

						recive_data_itr->second.recive_buffer_rest_size -= recvlen; 

						unsend_data_size += recvlen;

					}

					send_status_itr = recive_data_itr->second.send_status_list.begin();

					rest_request_data_size = recvlen;

					while( send_status_itr != recive_data_itr->second.send_status_list.end())
					{

						if( send_status_itr->status == SEND_CONTINUE )
						{

							if( send_status_itr->send_rest_size > rest_request_data_size )
							{

								send_status_itr->send_possible_size = rest_request_data_size;

								send_status_itr->send_rest_size -= rest_request_data_size;

								send_status_itr->send_end_size = 0;

								rest_request_data_size = 0;

							}
							else
							{

								send_status_itr->send_possible_size = send_status_itr->send_rest_size;

								rest_request_data_size -= send_status_itr->send_rest_size;

								send_status_itr->send_rest_size = 0;

								send_status_itr->send_end_size = 0;

							}

							send_status_itr->edit_division = EDIT_DIVISION_NO_EDIT;

							send_status_itr->status = SEND_OK;

							next_request_offset	= send_status_itr->send_offset
												+ send_status_itr->send_possible_size; 

						}
						else if( send_status_itr->status == SEND_NG )
						{

							check_result = check_http_method(
												(const char*)recive_data_itr->second.recive_buffer
													+ send_status_itr->send_offset,
												unsend_data_size );

							if( check_result == CHECK_OK )
							{

								check_result = check_http_version(
													(const char*)recive_data_itr->second.recive_buffer
														+ send_status_itr->send_offset,
													unsend_data_size );
							}

							if( check_result == CHECK_OK )
							{

								find_result = find_http_header(
												(const char*)recive_data_itr->second.recive_buffer
													+ send_status_itr->send_offset,
												unsend_data_size,
												http_header_name_blank,
												http_header_offset,
												http_header_len );

								if( find_result == true )
								{

									if( http_header_len == 0 )
									{
										send_status_itr->send_rest_size
														= http_header_offset + http_header_len + 2;
									}
									else
									{
										send_status_itr->send_rest_size
														= http_header_offset + http_header_len + 4;
									}

									find_result = find_http_header(
													(const char*)recive_data_itr->second.recive_buffer
														+ send_status_itr->send_offset,
													send_status_itr->send_rest_size,
													http_header_name_content_length,
													http_header_offset,
													http_header_len );

									if( find_result == true )
									{

										content_length.assign(	recive_data_itr->second.recive_buffer
																	+ send_status_itr->send_offset
																	+ http_header_offset,
																http_header_len );

										find_result = regex_search(	content_length.c_str(),
																	regex_result,
																	content_length_regex );

										if( find_result == true )
										{

											content_length = content_length.substr(
																			regex_result.position(1),
																			regex_result.length(1));

											send_status_itr->send_rest_size
																+=	boost::lexical_cast<size_t>(
																		content_length);

 										}

									}

									send_status_itr->edit_division = EDIT_DIVISION_EDIT;

								}
								else
								{

									send_status_itr->unsend_size += rest_request_data_size;

									rest_request_data_size = 0;

									break;

								}

							}
							else if( check_result == CHECK_NG )
							{

								send_status_itr->edit_division = EDIT_DIVISION_NO_EDIT;

								send_status_itr->send_rest_size = send_status_itr->unsend_size
																+ rest_request_data_size; 

							}
							else
							{

								send_status_itr->unsend_size += rest_request_data_size;

								rest_request_data_size = 0;

								break;

							}

							if( send_status_itr->send_rest_size >
									send_status_itr->unsend_size + rest_request_data_size )
							{

								send_status_itr->send_possible_size
									= send_status_itr->unsend_size + rest_request_data_size;

								send_status_itr->send_rest_size
									-= (send_status_itr->unsend_size + rest_request_data_size);

								send_status_itr->send_end_size = 0;

								send_status_itr->unsend_size = 0;

								rest_request_data_size = 0;

							}
							else
							{

								send_status_itr->send_possible_size = send_status_itr->send_rest_size;

								rest_request_data_size	= send_status_itr->unsend_size
														+ rest_request_data_size
														- send_status_itr->send_rest_size;

								send_status_itr->send_rest_size = 0;

								send_status_itr->send_end_size = 0;

								send_status_itr->unsend_size = 0;

							}

							send_status_itr->status = SEND_OK;

							next_request_offset	= send_status_itr->send_offset
												+ send_status_itr->send_possible_size;

						}

						if( rest_request_data_size <= 0 )
						{
							break;
						}

						send_status_itr++;

					}

					while( rest_request_data_size > 0 )
					{

						send_status_add.status				= SEND_NG;
						send_status_add.send_end_size		= 0;
						send_status_add.send_rest_size		= 0;
						send_status_add.send_possible_size	= 0;
						send_status_add.unsend_size			= 0;
						send_status_add.edit_division		= 0;
						send_status_add.send_offset			= next_request_offset;

						check_result = check_http_method(
											(const char*)recive_data_itr->second.recive_buffer
												+ send_status_add.send_offset,
											rest_request_data_size );

						if( check_result == CHECK_OK )
						{

							check_result = check_http_version(
												(const char*)recive_data_itr->second.recive_buffer
													+ send_status_add.send_offset,
												rest_request_data_size );
						}

						if( check_result == CHECK_OK )
						{

							find_result = find_http_header(
											(const char*)recive_data_itr->second.recive_buffer
												+ send_status_add.send_offset,
											rest_request_data_size,
											http_header_name_blank,
											http_header_offset,
											http_header_len );

							if( find_result == true )
							{

								if( http_header_len == 0 )
								{
									send_status_add.send_rest_size
													= http_header_offset + http_header_len + 2;
								}
								else
								{
									send_status_add.send_rest_size
													= http_header_offset + http_header_len + 4;
								}

								find_result = find_http_header(
												(const char*)recive_data_itr->second.recive_buffer
													+ send_status_add.send_offset,
												send_status_add.send_rest_size,
												http_header_name_content_length,
												http_header_offset,
												http_header_len );

								if( find_result == true )
								{

									content_length.assign(	recive_data_itr->second.recive_buffer
																+ send_status_add.send_offset
																+ http_header_offset,
															http_header_len );

									find_result = regex_search(	content_length.c_str(),
																regex_result,
																content_length_regex );

									if( find_result == true )
									{

										content_length = content_length.substr(
																		regex_result.position(1),
																		regex_result.length(1));

										send_status_add.send_rest_size
															+=	boost::lexical_cast<size_t>(
																	content_length);

									}

								}

								send_status_add.edit_division = EDIT_DIVISION_EDIT;

							}
							else
							{

								send_status_add.unsend_size = rest_request_data_size;

								rest_request_data_size = 0;

								recive_data_itr->second.send_status_list.push_back( send_status_add );

								break;

							}

						}
						else if( check_result == CHECK_NG )
						{

							send_status_add.edit_division = EDIT_DIVISION_NO_EDIT;

							send_status_add.send_rest_size = rest_request_data_size;

						}
						else
						{

							send_status_add.unsend_size = rest_request_data_size;

							rest_request_data_size = 0;

							recive_data_itr->second.send_status_list.push_back( send_status_add );

							break;

						}

						if( send_status_add.send_rest_size > rest_request_data_size )
						{

							send_status_add.send_possible_size = rest_request_data_size;

							send_status_add.send_rest_size -= rest_request_data_size;

							send_status_add.send_end_size = 0;

							send_status_add.unsend_size = 0;

							rest_request_data_size = 0;

						}
						else
						{

							send_status_add.send_possible_size = send_status_add.send_rest_size;

							rest_request_data_size -= send_status_add.send_rest_size;

							send_status_add.send_rest_size = 0;

							send_status_add.send_end_size = 0;

							send_status_add.unsend_size = 0;

						}

						send_status_add.status = SEND_OK;

						next_request_offset	= send_status_add.send_offset
											+ send_status_add.send_possible_size;

						recive_data_itr->second.send_status_list.push_back( send_status_add );

					}

					send_status_itr = recive_data_itr->second.send_status_list.begin();

					status = CLIENT_RECV;

					while( send_status_itr != recive_data_itr->second.send_status_list.end())
					{

						if( send_status_itr->status == SEND_OK )
						{

							if( thread_data->sorry_flag == SORRY_FLAG_ON )
							{

								status = SORRYSERVER_SELECT;
							}
							else
							{

								status = REALSERVER_SELECT;
							}

							break;
						}

						send_status_itr++;

					}

				}
			}

		}
	} catch (...)
	{
		//---------- DEBUG LOG START ------------------------------
		if( LOG_LV_DEBUG == getloglevel())
		{
			boost::format	outform(	"function out : [handle_client_recv] : "
										"thread_id = [%s], "
										"next_status = [%d]");
	
			outform % thread_id % FINALIZE;
	
			putLogDebug(	0,
							outform.str(),
							__FILE__,
							__LINE__ );
		}
		//---------- DEBUG LOG END ------------------------------

		return	FINALIZE;
	}

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function out : [handle_client_recv] : "
									"thread_id = [%s], "
									"next_status = [%d]");

		outform % thread_id % status;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	return	status;
}

//---------------------------------------------------------------------------------
protocol_module_cinsert::EVENT_TAG
protocol_module_cinsert::handle_realserver_select(
				const boost::thread::id thread_id,
				boost::asio::ip::tcp::endpoint & rs_endpoint )
{

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function in  : [handle_realserver_select] : "
									"thread_id = [%s]");

		outform % thread_id;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	using namespace boost::xpressive;

	t_session_thread_data_cinsert	thread_data;

	boost::asio::ip::tcp::endpoint	endpoint_init;

	bool	find_result;
	size_t	http_header_offset	= 0;
	size_t	http_header_len		= 0;
	std::string		http_header_name_cookie = "Cookie";
	std::string		cookie_name_str = cookie_name.data();
	std::string		cookie;
	std::string		cookie_address;
	std::string		cookie_port;
	match_results< const char* >	regex_result;
	cregex	cookie_regex
				=	icase("Cookie") >> ":" >> *_ >>
					cookie_name_str >> "=" >>
					( s1 = +_d >> "." >> +_d >> "." >> +_d >> "." >> +_d ) >>
					":" >> ( s2 = +_d );

	realserverlist_type::iterator	rs_list_itr;
	t_session_thread_data_map_itr	thread_data_itr;
	t_recive_data_map_itr			recive_data_itr;
	t_send_status_list_itr			send_status_itr;

	int		cookie_not_endpoint_flag	= 0;

	EVENT_TAG	status = STOP;

	try
	{
		{
			boost::mutex::scoped_lock	lock( session_thread_data_map_mutex );

			thread_data_itr = session_thread_data_map.find( thread_id );

			if( thread_data_itr != session_thread_data_map.end() )
			{
				thread_data = thread_data_itr->second;
			}
		}

		if( thread_data != NULL )
		{
			recive_data_itr = thread_data->recive_data_map.find( thread_data->client_endpoint_tcp );

			if( recive_data_itr != thread_data->recive_data_map.end() )
			{

				send_status_itr = recive_data_itr->second.send_status_list.begin();

				while( send_status_itr != recive_data_itr->second.send_status_list.end())
				{

					if( send_status_itr->status == SEND_OK )
					{

						break;

					}

					send_status_itr++;

				}

				rs_endpoint = endpoint_init;

				if( send_status_itr != recive_data_itr->second.send_status_list.end())
				{

					if( send_status_itr->send_endpoint != endpoint_init )
					{
						rs_endpoint = send_status_itr->send_endpoint;
						status = REALSERVER_CONNECT;

					}
					else
					{

						if( thread_data->last_endpoint_tcp != endpoint_init )
						{
							rs_endpoint = thread_data->last_endpoint_tcp;
						}
						else
						{

							find_result = find_http_header(
											(const char*)recive_data_itr->second.recive_buffer
												+ send_status_itr->send_offset,
											send_status_itr->send_possible_size,
											http_header_name_cookie,
											http_header_offset,
											http_header_len );
	
							if( find_result == true )
							{
	
								cookie.assign(	recive_data_itr->second.recive_buffer
													+ send_status_itr->send_offset
													+ http_header_offset,
													http_header_len );
	
								find_result = regex_search(	cookie.c_str(),
															regex_result,
															cookie_regex );
							}
	
							if( find_result == true )
							{
	
								cookie_address	= cookie.substr(	regex_result.position(1),
																	regex_result.length(1));
	
								cookie_port		= cookie.substr(	regex_result.position(2),
																	regex_result.length(2));
	
								try
								{
									boost::asio::ip::tcp::endpoint
										endpoint_tmp(	boost::asio::ip::address::from_string(cookie_address),
														boost::lexical_cast<unsigned short>(cookie_port));
		
									rs_list_itr = rs_list_begin();
		
									while( rs_list_itr != rs_list_end())
									{
		
										if( rs_list_itr->tcp_endpoint == endpoint_tmp )
										{
		
											rs_endpoint = endpoint_tmp;
											break;
										}
		
										rs_list_itr = rs_list_next( rs_list_itr );
									}
								} catch (...)
								{
									boost::format	outform(	"cookie value invalid endpoint : [handle_realserver_select] : "
																"cookie_address = [%s], "
																"cookie_port = [%s]");

									outform % cookie_address % cookie_port;

									putLogError(	0,
													outform.str(),
													__FILE__,
													__LINE__ );

									cookie_not_endpoint_flag = 1;

								}

								if(( rs_endpoint == endpoint_init && reschedule == 1 ) || cookie_not_endpoint_flag == 1 )
								{
	
									schedule_tcp( thread_id, rs_list_begin, rs_list_end, rs_list_next, rs_endpoint );
	
								}
	
							}
							else
							{
								schedule_tcp( thread_id, rs_list_begin, rs_list_end, rs_list_next, rs_endpoint );
							}
						}

						if( rs_endpoint != endpoint_init )
						{

							send_status_itr->send_endpoint = rs_endpoint;

							thread_data->last_endpoint_tcp = rs_endpoint;

							status = REALSERVER_CONNECT;

						}
						else
						{

							thread_data->end_flag = END_FLAG_ON;

							status = CLIENT_DISCONNECT;

						}
					}

				}

			}

		}
	} catch (...)
	{
		//---------- DEBUG LOG START ------------------------------
		if( LOG_LV_DEBUG == getloglevel())
		{
			boost::format	outform(	"function out : [handle_realserver_select] : "
										"thread_id = [%s], "
										"next_status = [%d]");
	
			outform % thread_id % FINALIZE;
	
			putLogDebug(	0,
							outform.str(),
							__FILE__,
							__LINE__ );
		}
		//---------- DEBUG LOG END ------------------------------

		return	FINALIZE;
	}

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function out : [handle_realserver_select] : "
									"thread_id = [%s], "
									"next_status = [%d], "
									"rs_endpoint = [%s]");

		outform % thread_id % status % rs_endpoint;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	return	status;

}

//---------------------------------------------------------------------------------
protocol_module_cinsert::EVENT_TAG
protocol_module_cinsert::handle_realserver_select(
				const boost::thread::id thread_id,
				boost::asio::ip::udp::endpoint& rs_endpoint,
				boost::array< char, MAX_BUFFER_SIZE >& sendbuffer,
				size_t& datalen )
{
	// NOP
	return	STOP;
}

//---------------------------------------------------------------------------------
protocol_module_cinsert::EVENT_TAG
protocol_module_cinsert::handle_realserver_connect(
				const boost::thread::id thread_id,
				boost::array< char, MAX_BUFFER_SIZE >& sendbuffer,
				size_t& datalen )
{

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function in  : [handle_realserver_connect] : "
									"thread_id = [%s]");

		outform % thread_id;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	t_session_thread_data_cinsert		thread_data;

	size_t	rest_datalen = sendbuffer.size();

	edit_data	edit_data_x_forwarded_for;

	bool			find_result			= false;
	size_t			http_header_offset	= 0;
	size_t			http_header_len		= 0;
	std::string		http_header_name_blank				= "";
	std::string		http_header_name_x_forwarded_for	= "X-Forwarded-For";

	t_session_thread_data_map_itr	thread_data_itr;
	t_recive_data_map_itr			recive_data_itr;
	t_send_status_list_itr			send_status_itr;
	t_edit_data_list_itr			edit_data_itr;
	t_edit_data_list_itr			min_insert_position_itr;

	EVENT_TAG	status = STOP;

	try
	{
		{
			boost::mutex::scoped_lock	lock( session_thread_data_map_mutex );

			thread_data_itr = session_thread_data_map.find( thread_id );

			if( thread_data_itr != session_thread_data_map.end() )
			{
				thread_data = thread_data_itr->second;
			}
		}

		datalen = 0;

		if( thread_data != NULL )
		{

			recive_data_itr = thread_data->recive_data_map.find( thread_data->client_endpoint_tcp );

			if( recive_data_itr != thread_data->recive_data_map.end() )
			{

				send_status_itr = recive_data_itr->second.send_status_list.begin();

				while( send_status_itr != recive_data_itr->second.send_status_list.end())
				{

					if( send_status_itr->status == SEND_OK )
					{

						break;

					}

					send_status_itr++;

				}

			}

			if( send_status_itr != recive_data_itr->second.send_status_list.end() )
			{

				if( send_status_itr->edit_division == EDIT_DIVISION_EDIT )
				{

					if( send_status_itr->edit_data_list.empty() == true )
					{

						if( forwarded_for == 1 )
						{

							find_result
								= find_http_header(	(const char*)recive_data_itr->second.recive_buffer
														+ send_status_itr->send_offset,
													send_status_itr->send_possible_size,
													http_header_name_x_forwarded_for,
													http_header_offset,
													http_header_len );
	
	
							if( find_result == true )
							{

								edit_data_x_forwarded_for.data.assign(
																recive_data_itr->second.recive_buffer
																	+ send_status_itr->send_offset
																	+ http_header_offset,
																http_header_len );
	
								edit_data_x_forwarded_for.data
									+=	str(boost::format(	", %1%" )
														% thread_data->client_endpoint_tcp.address());
	
								edit_data_x_forwarded_for.data_size			= edit_data_x_forwarded_for.data.length(); 
								edit_data_x_forwarded_for.insert_position	= http_header_offset;
								edit_data_x_forwarded_for.replace_size		= http_header_len;
		
							}
							else
							{

								find_result
									= find_http_header(	(const char*)recive_data_itr->second.recive_buffer
															+ send_status_itr->send_offset,
														send_status_itr->send_possible_size,
														http_header_name_blank,
														http_header_offset,
														http_header_len );
	
								if( find_result == true )
								{

									edit_data_x_forwarded_for.data
										+=	str(boost::format(	"%1%: %2%\r\n" )
															% http_header_name_x_forwarded_for
															% thread_data->client_endpoint_tcp.address());
		
									edit_data_x_forwarded_for.data_size			= edit_data_x_forwarded_for.data.length(); 
									edit_data_x_forwarded_for.insert_position	= http_header_offset;
									edit_data_x_forwarded_for.replace_size		= 0;
								}

							}

							send_status_itr->edit_data_list.push_back( edit_data_x_forwarded_for );
						}
					}

					while( 1 )
					{

						if( send_status_itr->edit_data_list.empty() == true )
						{

							send_status_itr->edit_division = EDIT_DIVISION_NO_EDIT;
	
							if( rest_datalen > 0 && send_status_itr->send_possible_size > 0 )
							{
	
								if( rest_datalen >= send_status_itr->send_possible_size )
								{

									memcpy(	sendbuffer.data()
												+	datalen,
											recive_data_itr->second.recive_buffer
												+	send_status_itr->send_offset
												+	send_status_itr->send_end_size,
											send_status_itr->send_possible_size );
	
									datalen += send_status_itr->send_possible_size;
	
									send_status_itr->send_end_size += send_status_itr->send_possible_size;

									rest_datalen -= send_status_itr->send_possible_size;
	
									send_status_itr->send_possible_size = 0;

								}
								else
								{

									memcpy(	sendbuffer.data()
												+	datalen,
											recive_data_itr->second.recive_buffer
												+	send_status_itr->send_offset
												+	send_status_itr->send_end_size,
											rest_datalen );
	
									datalen += rest_datalen;
	
									send_status_itr->send_end_size += rest_datalen;
	
									send_status_itr->send_possible_size -= rest_datalen;
	
									rest_datalen = 0;
	
								}

								break;

							}
	
						}
						else
						{

							edit_data_itr = send_status_itr->edit_data_list.begin();
	
							min_insert_position_itr = edit_data_itr;
	
							while( edit_data_itr != send_status_itr->edit_data_list.end() )
							{

								if( min_insert_position_itr->insert_position
										> edit_data_itr->insert_position )
								{

									min_insert_position_itr = edit_data_itr;
	
								}
	
								edit_data_itr++;
	
							}

							if( rest_datalen >= ( min_insert_position_itr->insert_position
													- send_status_itr->send_end_size ))
							{

								memcpy(	sendbuffer.data()
											+	datalen,
										recive_data_itr->second.recive_buffer
											+	send_status_itr->send_offset
											+	send_status_itr->send_end_size,
										min_insert_position_itr->insert_position
											-	send_status_itr->send_end_size );
	
								datalen
									+=	( min_insert_position_itr->insert_position
											- send_status_itr->send_end_size );
	
								send_status_itr->send_possible_size
									-=	( min_insert_position_itr->insert_position
											- send_status_itr->send_end_size );
	
								rest_datalen
									-=	( min_insert_position_itr->insert_position
											- send_status_itr->send_end_size );

								send_status_itr->send_end_size
									+=	( min_insert_position_itr->insert_position
											- send_status_itr->send_end_size );


								if( rest_datalen >= min_insert_position_itr->data_size )
								{

									memcpy(	sendbuffer.data()
												+	datalen,
											min_insert_position_itr->data.data(),
											min_insert_position_itr->data_size );
	
									datalen += min_insert_position_itr->data_size;

									send_status_itr->send_end_size
										+=	min_insert_position_itr->replace_size;

									send_status_itr->send_possible_size
										-=	min_insert_position_itr->replace_size;

									rest_datalen
										-=	min_insert_position_itr->data_size;

									send_status_itr->edit_data_list.erase( min_insert_position_itr );
	
								}
								else
								{

									break;
	
								}
	
							}
							else
							{

								memcpy(	sendbuffer.data()
											+	datalen,
										recive_data_itr->second.recive_buffer
											+	send_status_itr->send_offset
											+	send_status_itr->send_end_size,
										rest_datalen );
	
								datalen += rest_datalen;
	
								send_status_itr->send_end_size += rest_datalen;
	
								send_status_itr->send_possible_size -= rest_datalen;
	
								rest_datalen = 0;
	
								break;
	
							}
	
						}
	
					}
	
				}
				else
				{

					if( sendbuffer.size() >= send_status_itr->send_possible_size )
					{

						memcpy(	sendbuffer.data(),
								recive_data_itr->second.recive_buffer
									+	send_status_itr->send_offset,
								send_status_itr->send_possible_size );
	
						datalen = send_status_itr->send_possible_size;
	
						send_status_itr->send_end_size = send_status_itr->send_possible_size;
	
						send_status_itr->send_possible_size = 0;
	
					}
					else
					{

						memcpy(	sendbuffer.data(),
								recive_data_itr->second.recive_buffer
									+	send_status_itr->send_offset,
								sendbuffer.size());
	
						datalen = sendbuffer.size();
	
						send_status_itr->send_end_size = sendbuffer.size();
	
						send_status_itr->send_possible_size -= sendbuffer.size();
	
					}
	
				}

				status = REALSERVER_SEND;

			}

		}
	} catch (...)
	{
		//---------- DEBUG LOG START ------------------------------
		if( LOG_LV_DEBUG == getloglevel())
		{
			boost::format	outform(	"function out : [handle_realserver_connect] : "
										"thread_id = [%s], "
										"next_status = [%d]");
	
			outform % thread_id % FINALIZE;
	
			putLogDebug(	0,
							outform.str(),
							__FILE__,
							__LINE__ );
		}
		//---------- DEBUG LOG END ------------------------------

		return	FINALIZE;
	}

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function out : [handle_realserver_connect] : "
									"thread_id = [%s], "
									"next_status = [%d], "
									"datalen = [%d]");

		outform % thread_id % status % datalen;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	return	status;

}

//---------------------------------------------------------------------------------
protocol_module_cinsert::EVENT_TAG
protocol_module_cinsert::handle_realserver_connection_fail(
				const boost::thread::id thread_id,
				const boost::asio::ip::tcp::endpoint & rs_endpoint )
{

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function in  : [handle_realserver_connection_fail] : "
									"thread_id = [%s], "
									"rs_endpoint = [%s]");

		outform % thread_id % rs_endpoint;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	t_session_thread_data_cinsert	thread_data;
	t_session_thread_data_map_itr	thread_data_itr;

	EVENT_TAG	status = STOP;

	try
	{
		{
			boost::mutex::scoped_lock	lock( session_thread_data_map_mutex );

			thread_data_itr = session_thread_data_map.find( thread_id );

			if( thread_data_itr != session_thread_data_map.end() )
			{
				thread_data = thread_data_itr->second;
			}
		}

		if( thread_data != NULL )
		{

			thread_data->end_flag = END_FLAG_ON;

			status = CLIENT_DISCONNECT;

		}
	} catch (...)
	{
		//---------- DEBUG LOG START ------------------------------
		if( LOG_LV_DEBUG == getloglevel())
		{
			boost::format	outform(	"function out : [handle_realserver_connection_fail] : "
										"thread_id = [%s], "
										"next_status = [%d]");
	
			outform % thread_id % FINALIZE;
	
			putLogDebug(	0,
							outform.str(),
							__FILE__,
							__LINE__ );
		}
		//---------- DEBUG LOG END ------------------------------

		return	FINALIZE;
	}

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function out : [handle_realserver_connection_fail] : "
									"thread_id = [%s], "
									"next_status = [%d]");

		outform % thread_id % status;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	return	status;
}

//---------------------------------------------------------------------------------
protocol_module_cinsert::EVENT_TAG
protocol_module_cinsert::handle_realserver_send( const boost::thread::id thread_id )
{

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function in  : [handle_realserver_send] : "
									"thread_id = [%s]");

		outform % thread_id;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	t_session_thread_data_cinsert	thread_data;

	t_session_thread_data_map_itr	thread_data_itr;
	t_recive_data_map_itr			recive_data_itr;
	t_send_status_list_itr			send_status_itr;
	t_edit_data_list_itr			edit_data_itr;

	EVENT_TAG	status = STOP;

	try
	{
		{
			boost::mutex::scoped_lock	lock( session_thread_data_map_mutex );

			thread_data_itr = session_thread_data_map.find( thread_id );

			if( thread_data_itr != session_thread_data_map.end() )
			{
				thread_data = thread_data_itr->second;
			}
		}

		if( thread_data != NULL )
		{
			recive_data_itr = thread_data->recive_data_map.find( thread_data->client_endpoint_tcp );

			if( recive_data_itr != thread_data->recive_data_map.end() )
			{

				send_status_itr = recive_data_itr->second.send_status_list.begin();

				while( send_status_itr != recive_data_itr->second.send_status_list.end())
				{

					if( send_status_itr->status == SEND_OK )
					{

						break;

					}

					send_status_itr++;

				}

				if( send_status_itr != recive_data_itr->second.send_status_list.end())
				{
					if( send_status_itr->send_possible_size > 0 )
					{
	
						send_status_itr->send_offset += send_status_itr->send_end_size;
	
						edit_data_itr = send_status_itr->edit_data_list.begin();
	
						while( edit_data_itr != send_status_itr->edit_data_list.end())
						{
	
							edit_data_itr->insert_position -= send_status_itr->send_end_size;
	
						}
	
						send_status_itr->send_end_size = 0;
	
					}
					else
					{
						if( send_status_itr->send_rest_size > 0 )
						{
							send_status_itr->status = SEND_CONTINUE;
						}
						else
						{
							send_status_itr->status = SEND_END;
						}
					}

				}

				status = CLIENT_RECV;

				while( send_status_itr != recive_data_itr->second.send_status_list.end())
				{
					if( send_status_itr->status == SEND_OK )
					{
	
						status = REALSERVER_SELECT;
						break;
	
					}
	
					send_status_itr++;

				}

			}

		}
	} catch (...)
	{
		//---------- DEBUG LOG START ------------------------------
		if( LOG_LV_DEBUG == getloglevel())
		{
			boost::format	outform(	"function out : [handle_realserver_send] : "
										"thread_id = [%s], "
										"next_status = [%d]");
	
			outform % thread_id % FINALIZE;
	
			putLogDebug(	0,
							outform.str(),
							__FILE__,
							__LINE__ );
		}
		//---------- DEBUG LOG END ------------------------------

		return	FINALIZE;
	}

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function out : [handle_realserver_send] : "
									"thread_id = [%s], "
									"next_status = [%d]");

		outform % thread_id % status;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	return	status;
}

//---------------------------------------------------------------------------------
protocol_module_cinsert::EVENT_TAG
protocol_module_cinsert::handle_sorryserver_select(
				const boost::thread::id thread_id,
				boost::asio::ip::tcp::endpoint & sorry_endpoint )
{

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function in  : [handle_sorryserver_select] : "
									"thread_id = [%s], "
									"sorry_endpoint = [%s]");

		outform % thread_id % sorry_endpoint;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	t_session_thread_data_cinsert	thread_data;

	boost::asio::ip::tcp::endpoint	endpoint_init;

	t_session_thread_data_map_itr	thread_data_itr;
	t_recive_data_map_itr			recive_data_itr;
	t_send_status_list_itr			send_status_itr;

	EVENT_TAG	status = STOP;

	try
	{
		{
			boost::mutex::scoped_lock	lock( session_thread_data_map_mutex );

			thread_data_itr = session_thread_data_map.find( thread_id );

			if( thread_data_itr != session_thread_data_map.end() )
			{
				thread_data = thread_data_itr->second;
			}
		}

		if( thread_data != NULL )
		{

			if( sorry_endpoint == endpoint_init )
			{

				thread_data->end_flag = END_FLAG_ON;

				status = CLIENT_DISCONNECT;

			}
			else
			{
				recive_data_itr = thread_data->recive_data_map.find( thread_data->client_endpoint_tcp );
	
				if( recive_data_itr != thread_data->recive_data_map.end() )
				{
	
					send_status_itr = recive_data_itr->second.send_status_list.begin();
	
					while( send_status_itr != recive_data_itr->second.send_status_list.end())
					{
	
						if( send_status_itr->status == SEND_OK )
						{
	
							break;
	
						}
	
						send_status_itr++;
	
					}
	
					if( send_status_itr != recive_data_itr->second.send_status_list.end())
					{
	
						send_status_itr->send_endpoint = sorry_endpoint;
	
						status = SORRYSERVER_CONNECT;
					}
				}
			}
		}
	} catch (...)
	{
		//---------- DEBUG LOG START ------------------------------
		if( LOG_LV_DEBUG == getloglevel())
		{
			boost::format	outform(	"function out : [handle_sorryserver_select] : "
										"thread_id = [%s], "
										"next_status = [%d]");
	
			outform % thread_id % FINALIZE;
	
			putLogDebug(	0,
							outform.str(),
							__FILE__,
							__LINE__ );
		}
		//---------- DEBUG LOG END ------------------------------

		return	FINALIZE;
	}

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function out : [handle_sorryserver_select] : "
									"thread_id = [%s], "
									"next_status = [%d], "
									"sorry_endpoint = [%s]");

		outform % thread_id % status % sorry_endpoint;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	return	status;
}

//---------------------------------------------------------------------------------
protocol_module_cinsert::EVENT_TAG
protocol_module_cinsert::handle_sorryserver_connect(
				const boost::thread::id thread_id,
				boost::array< char, MAX_BUFFER_SIZE >& sendbuffer,
				size_t& datalen )
{

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function in  : [handle_sorryserver_connect] : "
									"thread_id = [%s]");

		outform % thread_id;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	t_session_thread_data_cinsert	thread_data;

	edit_data	edit_data_uri;
	edit_data	edit_data_x_forwarded_for;

	size_t	rest_datalen = sendbuffer.size();

	bool			find_result			= false;
	size_t			uri_offset			= 0;
	size_t			uri_len				= 0;
	size_t			http_header_offset	= 0;
	size_t			http_header_len		= 0;
	std::string		http_header_name_blank				= "";
	std::string		http_header_name_x_forwarded_for	= "X-Forwarded-For";

	t_session_thread_data_map_itr	thread_data_itr;
	t_recive_data_map_itr			recive_data_itr;
	t_send_status_list_itr			send_status_itr;
	t_edit_data_list_itr			edit_data_itr;
	t_edit_data_list_itr			min_insert_position_itr;

	EVENT_TAG	status = STOP;

	try
	{
		{
			boost::mutex::scoped_lock	lock( session_thread_data_map_mutex );

			thread_data_itr = session_thread_data_map.find( thread_id );

			if( thread_data_itr != session_thread_data_map.end() )
			{
				thread_data = thread_data_itr->second;
			}
		}

		datalen = 0;

		if( thread_data != NULL )
		{
			recive_data_itr = thread_data->recive_data_map.find( thread_data->client_endpoint_tcp );

			if( recive_data_itr != thread_data->recive_data_map.end() )
			{

				send_status_itr = recive_data_itr->second.send_status_list.begin();

				while( send_status_itr != recive_data_itr->second.send_status_list.end())
				{

					if( send_status_itr->status == SEND_OK )
					{

						break;

					}

					send_status_itr++;

				}

			}

			if( send_status_itr != recive_data_itr->second.send_status_list.end() )
			{
				if( send_status_itr->edit_division == EDIT_DIVISION_EDIT )
				{
	
					if( send_status_itr->edit_data_list.empty() == true )
					{

						if( strlen(sorry_uri.data()) > 0 )
						{
							find_result
								= find_uri(	(const char*)recive_data_itr->second.recive_buffer
												+ send_status_itr->send_offset,
											send_status_itr->send_possible_size,
											uri_offset,
											uri_len );
	
							if( find_result == true )
							{
								edit_data_uri.data				= sorry_uri.data();
								edit_data_uri.data_size			= edit_data_uri.data.length(); 
								edit_data_uri.insert_position	= uri_offset;
								edit_data_uri.replace_size		= uri_len;
	
								send_status_itr->edit_data_list.push_back( edit_data_uri );
	
							}
						}

						if( forwarded_for == 1 )
						{
							find_result
								= find_http_header(	(const char*)recive_data_itr->second.recive_buffer
														+ send_status_itr->send_offset,
													send_status_itr->send_possible_size,
													http_header_name_x_forwarded_for,
													http_header_offset,
													http_header_len );
	
	
							if( find_result == true )
							{
	
								edit_data_x_forwarded_for.data.assign(
																recive_data_itr->second.recive_buffer
																	+ send_status_itr->send_offset
																	+ http_header_offset,
																http_header_len );
	
								edit_data_x_forwarded_for.data
									+=	str(boost::format(	", %1%" )
														% thread_data->client_endpoint_tcp.address());
	
								edit_data_x_forwarded_for.data_size			= edit_data_x_forwarded_for.data.length(); 
								edit_data_x_forwarded_for.insert_position	= http_header_offset;
								edit_data_x_forwarded_for.replace_size		= http_header_len;
		
							}
							else
							{
	
								find_result
									= find_http_header(	(const char*)recive_data_itr->second.recive_buffer
															+ send_status_itr->send_offset,
														send_status_itr->send_possible_size,
														http_header_name_blank,
														http_header_offset,
														http_header_len );
	
								if( find_result == true )
								{
		
									edit_data_x_forwarded_for.data
										+=	str(boost::format(	"%1%: %2%\r\n" )
															% http_header_name_x_forwarded_for
															% thread_data->client_endpoint_tcp.address());
		
									edit_data_x_forwarded_for.data_size			= edit_data_x_forwarded_for.data.length(); 
									edit_data_x_forwarded_for.insert_position	= http_header_offset;
									edit_data_x_forwarded_for.replace_size		= 0;
								}
	
							}
	
							send_status_itr->edit_data_list.push_back( edit_data_x_forwarded_for );

						}

					}
	
					while( 1 )
					{
	
						if( send_status_itr->edit_data_list.empty() == true )
						{
	
							send_status_itr->edit_division = EDIT_DIVISION_NO_EDIT;
	
							if( rest_datalen > 0 && send_status_itr->send_possible_size > 0 )
							{
	
								if( rest_datalen >= send_status_itr->send_possible_size )
								{
	
									memcpy(	sendbuffer.data()
												+	datalen,
											recive_data_itr->second.recive_buffer
												+	send_status_itr->send_offset
												+	send_status_itr->send_end_size,
											send_status_itr->send_possible_size );
	
									datalen += send_status_itr->send_possible_size; 
	
									send_status_itr->send_end_size += send_status_itr->send_possible_size;

									rest_datalen -= send_status_itr->send_possible_size;
	
									send_status_itr->send_possible_size = 0;

								}
								else
								{
	
									memcpy(	sendbuffer.data()
												+	datalen,
											recive_data_itr->second.recive_buffer
												+	send_status_itr->send_offset
												+	send_status_itr->send_end_size,
											rest_datalen );
	
									datalen += rest_datalen;
	
									send_status_itr->send_end_size += rest_datalen;
	
									send_status_itr->send_possible_size -= rest_datalen;
	
									rest_datalen = 0;
	
								}
	
								break;
	
							}
	
						}
						else
						{
	
							edit_data_itr = send_status_itr->edit_data_list.begin();
	
							min_insert_position_itr = edit_data_itr;
	
							while( edit_data_itr != send_status_itr->edit_data_list.end() )
							{
	
								if( min_insert_position_itr->insert_position
										> edit_data_itr->insert_position )
								{
	
									min_insert_position_itr = edit_data_itr;
	
								}
	
								edit_data_itr++;
	
							}
	
							if( rest_datalen >= ( min_insert_position_itr->insert_position
													- send_status_itr->send_end_size ))
							{
	
								memcpy(	sendbuffer.data()
											+	datalen,
										recive_data_itr->second.recive_buffer
											+	send_status_itr->send_offset
											+	send_status_itr->send_end_size,
										min_insert_position_itr->insert_position
											-	send_status_itr->send_end_size );
	
								datalen
									+=	( min_insert_position_itr->insert_position
											- send_status_itr->send_end_size );
	
								send_status_itr->send_possible_size
									-=	( min_insert_position_itr->insert_position
											- send_status_itr->send_end_size );
	
								rest_datalen
									-=	( min_insert_position_itr->insert_position
											- send_status_itr->send_end_size );

								send_status_itr->send_end_size
									+=	( min_insert_position_itr->insert_position
											- send_status_itr->send_end_size );
	
								if( rest_datalen >= min_insert_position_itr->data_size )
								{
	
									memcpy(	sendbuffer.data()
												+	datalen,
											min_insert_position_itr->data.data(),
											min_insert_position_itr->data_size );
	
									datalen += min_insert_position_itr->data_size;
	
									send_status_itr->send_end_size
										+=	min_insert_position_itr->replace_size;
		
									send_status_itr->send_possible_size
										-=	min_insert_position_itr->replace_size;
		
									rest_datalen
										-=	min_insert_position_itr->data_size;
	
									send_status_itr->edit_data_list.erase( min_insert_position_itr );
	
								}
								else
								{
	
									break;
	
								}
	
							}
							else
							{
	
								memcpy(	sendbuffer.data()
											+	datalen,
										recive_data_itr->second.recive_buffer
											+	send_status_itr->send_offset
											+	send_status_itr->send_end_size,
										rest_datalen );
	
								datalen += rest_datalen;
	
								send_status_itr->send_end_size += rest_datalen;
	
								send_status_itr->send_possible_size -= rest_datalen;
	
								rest_datalen = 0;
	
								break;
	
							}
	
						}
	
					}
	
				}
				else
				{
	
					if( sendbuffer.size() >= send_status_itr->send_possible_size )
					{
	
						memcpy(	sendbuffer.data(),
								recive_data_itr->second.recive_buffer
									+	send_status_itr->send_offset,
								send_status_itr->send_possible_size );
	
						datalen = send_status_itr->send_possible_size;
	
						send_status_itr->send_end_size = send_status_itr->send_possible_size;
	
						send_status_itr->send_possible_size = 0;
	
					}
					else
					{
	
						memcpy(	sendbuffer.data(),
								recive_data_itr->second.recive_buffer
									+	send_status_itr->send_offset,
								sendbuffer.size());
	
						datalen = sendbuffer.size();
	
						send_status_itr->send_end_size = sendbuffer.size();
	
						send_status_itr->send_possible_size -= sendbuffer.size();
	
					}
	
				}
	
				status = SORRYSERVER_SEND;

			}

		}
	} catch (...)
	{
		//---------- DEBUG LOG START ------------------------------
		if( LOG_LV_DEBUG == getloglevel())
		{
			boost::format	outform(	"function out : [handle_sorryserver_connect] : "
										"thread_id = [%s], "
										"next_status = [%d]");
	
			outform % thread_id % FINALIZE;
	
			putLogDebug(	0,
							outform.str(),
							__FILE__,
							__LINE__ );
		}
		//---------- DEBUG LOG END ------------------------------

		return	FINALIZE;
	}

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function out : [handle_sorryserver_connect] : "
									"thread_id = [%s], "
									"next_status = [%d], "
									"datalen = [%d]");

		outform % thread_id % status % datalen;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	return	status;
}

//---------------------------------------------------------------------------------
protocol_module_cinsert::EVENT_TAG
protocol_module_cinsert::handle_sorryserver_connection_fail(
				const boost::thread::id thread_id,
				const boost::asio::ip::tcp::endpoint & sorry_endpoint )
{

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function in  : [handle_sorryserver_connection_fail] : "
									"thread_id = [%s], "
									"sorry_endpoint = [%s]");

		outform % thread_id % sorry_endpoint;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	t_session_thread_data_cinsert	thread_data;
	t_session_thread_data_map_itr	thread_data_itr;

	EVENT_TAG	status = STOP;

	try
	{
		{
			boost::mutex::scoped_lock	lock( session_thread_data_map_mutex );

			thread_data_itr = session_thread_data_map.find( thread_id );

			if( thread_data_itr != session_thread_data_map.end() )
			{
				thread_data = thread_data_itr->second;
			}
		}

		if( thread_data != NULL )
		{

			thread_data->end_flag = END_FLAG_ON;

			status = CLIENT_DISCONNECT;

		}
	} catch (...)
	{
		//---------- DEBUG LOG START ------------------------------
		if( LOG_LV_DEBUG == getloglevel())
		{
			boost::format	outform(	"function out : [handle_sorryserver_connection_fail] : "
										"thread_id = [%s], "
										"next_status = [%d]");
	
			outform % thread_id % FINALIZE;
	
			putLogDebug(	0,
							outform.str(),
							__FILE__,
							__LINE__ );
		}
		//---------- DEBUG LOG END ------------------------------

		return	FINALIZE;
	}

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function out : [handle_sorryserver_connection_fail] : "
									"thread_id = [%s], "
									"next_status = [%d]");

		outform % thread_id % status;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	return	status;
}

//---------------------------------------------------------------------------------
protocol_module_cinsert::EVENT_TAG
protocol_module_cinsert::handle_sorryserver_send( const boost::thread::id thread_id )
{

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function in  : [handle_sorryserver_send] : "
									"thread_id = [%s]");

		outform % thread_id;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	t_session_thread_data_cinsert	thread_data;

	t_session_thread_data_map_itr	thread_data_itr;
	t_recive_data_map_itr			recive_data_itr;
	t_send_status_list_itr			send_status_itr;
	t_edit_data_list_itr			edit_data_itr;

	EVENT_TAG	status = STOP;

	try
	{
		{
			boost::mutex::scoped_lock	lock( session_thread_data_map_mutex );

			thread_data_itr = session_thread_data_map.find( thread_id );

			if( thread_data_itr != session_thread_data_map.end() )
			{
				thread_data = thread_data_itr->second;
			}
		}

		if( thread_data != NULL )
		{
			recive_data_itr = thread_data->recive_data_map.find( thread_data->client_endpoint_tcp );

			if( recive_data_itr != thread_data->recive_data_map.end() )
			{

				send_status_itr = recive_data_itr->second.send_status_list.begin();

				while( send_status_itr != recive_data_itr->second.send_status_list.end())
				{

					if( send_status_itr->status == SEND_OK )
					{

						break;

					}

					send_status_itr++;

				}

				if( send_status_itr != recive_data_itr->second.send_status_list.end())
				{
					if( send_status_itr->send_possible_size > 0 )
					{
	
						send_status_itr->send_offset += send_status_itr->send_end_size;
	
						edit_data_itr = send_status_itr->edit_data_list.begin();
	
						while( edit_data_itr != send_status_itr->edit_data_list.end())
						{
	
							edit_data_itr->insert_position -= send_status_itr->send_end_size;
	
						}
	
						send_status_itr->send_end_size = 0;
	
					}
					else
					{
						if( send_status_itr->send_rest_size > 0 )
						{
							send_status_itr->status = SEND_CONTINUE;
						}
						else
						{
							send_status_itr->status = SEND_END;
						}
					}

				}

				status = CLIENT_RECV;

				while( send_status_itr != recive_data_itr->second.send_status_list.end())
				{
					if( send_status_itr->status == SEND_OK )
					{
	
						status = SORRYSERVER_SELECT;
						break;
	
					}
	
					send_status_itr++;

				}

			}

		}
	} catch (...)
	{
		//---------- DEBUG LOG START ------------------------------
		if( LOG_LV_DEBUG == getloglevel())
		{
			boost::format	outform(	"function out : [handle_sorryserver_send] : "
										"thread_id = [%s], "
										"next_status = [%d]");
	
			outform % thread_id % FINALIZE;
	
			putLogDebug(	0,
							outform.str(),
							__FILE__,
							__LINE__ );
		}
		//---------- DEBUG LOG END ------------------------------

		return	FINALIZE;
	}

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function out : [handle_sorryserver_send] : "
									"thread_id = [%s], "
									"next_status = [%d]");

		outform % thread_id % status;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	return	status;
}

//---------------------------------------------------------------------------------
protocol_module_cinsert::EVENT_TAG
protocol_module_cinsert::handle_realserver_recv(
				const boost::thread::id thread_id,
				const boost::asio::ip::tcp::endpoint& rs_endpoint,
				const boost::array< char, MAX_BUFFER_SIZE >& recvbuffer,
				const size_t recvlen )
{

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function in  : [handle_realserver_recv] : "
									"thread_id = [%s], "
									"rs_endpoint = [%s], "
									"recvlen = [%d]");

		outform % thread_id % rs_endpoint % recvlen;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	using namespace boost::xpressive;

	t_session_thread_data_cinsert	thread_data;

	char*	buffer				= NULL;
	char*	buffer_1			= NULL;
	char*	buffer_2			= NULL;

	size_t	unsend_data_offset		= 0;
	size_t	unsend_data_size		= 0;
	size_t	rest_response_data_size	= 0;
	size_t	next_response_offset	= 0;

	send_status			send_status_add;

	recive_data		realserver_recv_data;

	CHECK_RESULT_TAG	check_result;
	bool				find_result	= false;
	size_t				http_header_offset	= 0;
	size_t				http_header_len		= 0;
	std::string			http_header_name_blank			= "";
	std::string			http_header_name_content_length	= "Content-Length";
	std::string			content_length;
	match_results< const char* >	regex_result;
	cregex	content_length_regex = icase("Content-Length") >> ":" >> *~_d >> (s1 = +_d) >> *~_d;

	t_session_thread_data_map_itr	thread_data_itr;
	t_recive_data_map_itr			recive_data_itr;
	t_send_status_list_itr			send_status_itr;

	EVENT_TAG	status = STOP;

	try
	{
		{
			boost::mutex::scoped_lock	lock( session_thread_data_map_mutex );

			thread_data_itr = session_thread_data_map.find( thread_id );

			if( thread_data_itr != session_thread_data_map.end() )
			{
				thread_data = thread_data_itr->second;
			}
		}

		if( thread_data != NULL )
		{

			recive_data_itr = thread_data->recive_data_map.find( rs_endpoint );

			if( recive_data_itr == thread_data->recive_data_map.end() )
			{

				buffer = new char[realserver_recv_data.recive_buffer_max_size];
		
				realserver_recv_data.recive_buffer		= buffer;
				realserver_recv_data.recive_buffer_1	= buffer;
		
				buffer = new char[realserver_recv_data.recive_buffer_max_size];
		
				realserver_recv_data.recive_buffer_2	= buffer;
		
				thread_data->recive_data_map[ rs_endpoint ] = realserver_recv_data;

				realserver_recv_data.recive_buffer_1 = NULL;
				realserver_recv_data.recive_buffer_2 = NULL;

				recive_data_itr = thread_data->recive_data_map.find( rs_endpoint );

			}

			if( recive_data_itr != thread_data->recive_data_map.end() )
			{

				send_status_itr = recive_data_itr->second.send_status_list.begin();

				while( send_status_itr != recive_data_itr->second.send_status_list.end())
				{

					if( send_status_itr->status == SEND_END )
					{

						next_response_offset = send_status_itr->send_offset + send_status_itr->send_end_size;
						send_status_itr = recive_data_itr->second.send_status_list.erase( send_status_itr );
					}
					else if( send_status_itr->status == SEND_CONTINUE )
					{

						send_status_itr->send_offset += send_status_itr->send_end_size;
						unsend_data_offset = send_status_itr->send_offset;
						break;
					}
					else
					{

						unsend_data_offset = send_status_itr->send_offset;
						unsend_data_size = send_status_itr->unsend_size;
						break;
					}
				}

				if( recive_data_itr->second.recive_buffer_rest_size < recvlen )
				{

					if( recive_data_itr->second.recive_buffer_max_size < unsend_data_size + recvlen )
					{

						buffer_1 = new char[unsend_data_size + recvlen];
						buffer_2 = new char[unsend_data_size + recvlen];

						memset( buffer_1, '\0', unsend_data_size + recvlen );
						memset( buffer_2, '\0', unsend_data_size + recvlen );

						memcpy(	buffer_1,
								recive_data_itr->second.recive_buffer + unsend_data_offset,
								unsend_data_size );

						memcpy(	buffer_1 + unsend_data_size,
								recvbuffer.data(),
								recvlen );

						free( recive_data_itr->second.recive_buffer_1 );
						free( recive_data_itr->second.recive_buffer_2 );

						recive_data_itr->second.recive_buffer	= buffer_1;
						recive_data_itr->second.recive_buffer_1	= buffer_1;
						recive_data_itr->second.recive_buffer_2	= buffer_2;

						recive_data_itr->second.recive_buffer_max_size = unsend_data_size + recvlen;

					}
					else
					{

						if( recive_data_itr->second.recive_buffer == recive_data_itr->second.recive_buffer_1 )
						{

							memset( recive_data_itr->second.recive_buffer_2,
									'\0',
									recive_data_itr->second.recive_buffer_max_size );

							memcpy(	recive_data_itr->second.recive_buffer_2,
									recive_data_itr->second.recive_buffer + unsend_data_offset,
									unsend_data_size );

							memcpy(	recive_data_itr->second.recive_buffer_2 + unsend_data_size,
									recvbuffer.data(),
									recvlen );

							recive_data_itr->second.recive_buffer = recive_data_itr->second.recive_buffer_2;
						}
						else
						{

							memset( recive_data_itr->second.recive_buffer_1,
									'\0',
									recive_data_itr->second.recive_buffer_max_size );

							memcpy(	recive_data_itr->second.recive_buffer_1,
									recive_data_itr->second.recive_buffer + unsend_data_offset,
									unsend_data_size );

							memcpy(	recive_data_itr->second.recive_buffer_1 + unsend_data_size,
									recvbuffer.data(),
									recvlen );

							recive_data_itr->second.recive_buffer = recive_data_itr->second.recive_buffer_1;
						}

					}

					recive_data_itr->second.recive_buffer_rest_size
						= recive_data_itr->second.recive_buffer_max_size - unsend_data_size - recvlen;

					next_response_offset = 0;

					unsend_data_size += recvlen;

					send_status_itr = recive_data_itr->second.send_status_list.begin();

					while( send_status_itr != recive_data_itr->second.send_status_list.end())
					{

						send_status_itr->send_offset -= unsend_data_offset;
						send_status_itr++;

					}

				}
				else
				{

					memcpy(	recive_data_itr->second.recive_buffer + recive_data_itr->second.recive_buffer_max_size - recive_data_itr->second.recive_buffer_rest_size,
							recvbuffer.data(),
							recvlen );

					recive_data_itr->second.recive_buffer_rest_size -= recvlen; 

					unsend_data_size += recvlen;

				}

				send_status_itr = recive_data_itr->second.send_status_list.begin();

				rest_response_data_size = recvlen;

				while( send_status_itr != recive_data_itr->second.send_status_list.end())
				{

					if( send_status_itr->status == SEND_CONTINUE )
					{

						if( send_status_itr->send_rest_size > rest_response_data_size )
						{

							send_status_itr->send_possible_size = rest_response_data_size;

							send_status_itr->send_rest_size -= rest_response_data_size;

							send_status_itr->send_end_size = 0;

							rest_response_data_size = 0;

						}
						else
						{

							send_status_itr->send_possible_size = send_status_itr->send_rest_size;

							rest_response_data_size -= send_status_itr->send_rest_size;

							send_status_itr->send_rest_size = 0;

							send_status_itr->send_end_size = 0;

						}

						send_status_itr->edit_division = EDIT_DIVISION_NO_EDIT;

						send_status_itr->status = SEND_OK;

						next_response_offset	= send_status_itr->send_offset
												+ send_status_itr->send_possible_size; 

					}
					else if( send_status_itr->status == SEND_NG )
					{

						check_result = check_status_code(
											(const char*)recive_data_itr->second.recive_buffer
												+ send_status_itr->send_offset,
											unsend_data_size );

						if( check_result == CHECK_OK )
						{

							check_result = check_http_version(
												(const char*)recive_data_itr->second.recive_buffer
													+ send_status_itr->send_offset,
												unsend_data_size );
						}

						if( check_result == CHECK_OK )
						{

							find_result = find_http_header(
											(const char*)recive_data_itr->second.recive_buffer
												+ send_status_itr->send_offset,
											unsend_data_size,
											http_header_name_blank,
											http_header_offset,
											http_header_len );

							if( find_result == true )
							{

								if( http_header_len == 0 )
								{
									send_status_itr->send_rest_size
													= http_header_offset + http_header_len + 2;
								}
								else
								{
									send_status_itr->send_rest_size
													= http_header_offset + http_header_len + 4;
								}

								find_result = find_http_header(
												(const char*)recive_data_itr->second.recive_buffer
													+ send_status_itr->send_offset,
												send_status_itr->send_rest_size,
												http_header_name_content_length,
												http_header_offset,
												http_header_len );

								if( find_result == true )
								{

									content_length.assign(	recive_data_itr->second.recive_buffer
																+ send_status_itr->send_offset
																+ http_header_offset,
															http_header_len );

									find_result = regex_search(	content_length.c_str(),
																regex_result,
																content_length_regex );

									if( find_result == true )
									{

										content_length = content_length.substr(
																		regex_result.position(1),
																		regex_result.length(1));

										send_status_itr->send_rest_size
															+=	boost::lexical_cast<size_t>(
																	content_length);

									}

								}

								send_status_itr->edit_division = EDIT_DIVISION_EDIT;

							}
							else
							{

								send_status_itr->unsend_size += rest_response_data_size;

								rest_response_data_size = 0;

								break;

							}

						}
						else if( check_result == CHECK_NG )
						{

							send_status_itr->edit_division = EDIT_DIVISION_NO_EDIT;

							send_status_itr->send_rest_size = send_status_itr->unsend_size
															+ rest_response_data_size; 

						}
						else
						{

							send_status_itr->unsend_size += rest_response_data_size;

							rest_response_data_size = 0;

							break;

						}

						if( send_status_itr->send_rest_size >
								send_status_itr->unsend_size + rest_response_data_size )
						{


							send_status_itr->send_possible_size
								= send_status_itr->unsend_size + rest_response_data_size;

							send_status_itr->send_rest_size
								-= ( send_status_itr->unsend_size + rest_response_data_size );

							send_status_itr->send_end_size = 0;

							send_status_itr->unsend_size = 0;

							rest_response_data_size = 0;

						}
						else
						{

							send_status_itr->send_possible_size = send_status_itr->send_rest_size;

							rest_response_data_size	= send_status_itr->unsend_size
													+ rest_response_data_size
													- send_status_itr->send_rest_size;

							send_status_itr->send_rest_size = 0;

							send_status_itr->send_end_size = 0;

							send_status_itr->unsend_size = 0;

						}

						send_status_itr->status = SEND_OK;

						next_response_offset	= send_status_itr->send_offset
												+ send_status_itr->send_possible_size;

					}

					if( rest_response_data_size <= 0 )
					{

						break;
					}

					send_status_itr++;

				}

				while( rest_response_data_size > 0 )
				{

					send_status_add.status				= SEND_NG;
					send_status_add.send_end_size		= 0;
					send_status_add.send_rest_size		= 0;
					send_status_add.send_possible_size	= 0;
					send_status_add.unsend_size			= 0;
					send_status_add.edit_division		= 0;
					send_status_add.send_offset			= next_response_offset;

					check_result = check_status_code(
										(const char*)recive_data_itr->second.recive_buffer
											+ send_status_add.send_offset,
										rest_response_data_size );

					if( check_result == CHECK_OK )
					{

						check_result = check_http_version(
											(const char*)recive_data_itr->second.recive_buffer
												+ send_status_add.send_offset,
											rest_response_data_size );
					}

					if( check_result == CHECK_OK )
					{

						find_result = find_http_header(
										(const char*)recive_data_itr->second.recive_buffer
											+ send_status_add.send_offset,
										rest_response_data_size,
										http_header_name_blank,
										http_header_offset,
										http_header_len );

						if( find_result == true )
						{

							if( http_header_len == 0 )
							{
								send_status_add.send_rest_size
												= http_header_offset + http_header_len + 2;
							}
							else
							{
								send_status_add.send_rest_size
												= http_header_offset + http_header_len + 4;
							}

							find_result = find_http_header(
											(const char*)recive_data_itr->second.recive_buffer
												+ send_status_add.send_offset,
											send_status_add.send_rest_size,
											http_header_name_content_length,
											http_header_offset,
											http_header_len );

							if( find_result == true )
							{

								content_length.assign(	recive_data_itr->second.recive_buffer
															+ send_status_add.send_offset
															+ http_header_offset,
														http_header_len );

								find_result = regex_search(	content_length.c_str(),
															regex_result,
															content_length_regex );

								if( find_result == true )
								{

									content_length = content_length.substr(
																	regex_result.position(1),
																	regex_result.length(1));

									send_status_add.send_rest_size
														+=	boost::lexical_cast<size_t>(
																content_length);

								}

							}

							send_status_add.edit_division = EDIT_DIVISION_EDIT;

						}
						else
						{

							send_status_add.unsend_size = rest_response_data_size;

							rest_response_data_size = 0;

							recive_data_itr->second.send_status_list.push_back( send_status_add );

							break;

						}

					}
					else if( check_result == CHECK_NG )
					{

						send_status_add.edit_division = EDIT_DIVISION_NO_EDIT;

						send_status_add.send_rest_size = rest_response_data_size;

					}
					else
					{

						send_status_add.unsend_size = rest_response_data_size;

						rest_response_data_size = 0;

						recive_data_itr->second.send_status_list.push_back( send_status_add );

						break;

					}

					if( send_status_add.send_rest_size > rest_response_data_size )
					{

						send_status_add.send_possible_size = rest_response_data_size;

						send_status_add.send_rest_size -= rest_response_data_size;

						send_status_add.send_end_size = 0;

						send_status_add.unsend_size = 0;

						rest_response_data_size = 0;

					}
					else
					{

						send_status_add.send_possible_size = send_status_add.send_rest_size;

						rest_response_data_size -= send_status_add.send_rest_size;

						send_status_add.send_rest_size = 0;

						send_status_add.send_end_size = 0;

						send_status_add.unsend_size = 0;

					}

					send_status_add.status = SEND_OK;

					next_response_offset	= send_status_add.send_offset
											+ send_status_add.send_possible_size;

					recive_data_itr->second.send_status_list.push_back( send_status_add );

				}

				send_status_itr = recive_data_itr->second.send_status_list.begin();

				status = REALSERVER_RECV;

				while( send_status_itr != recive_data_itr->second.send_status_list.end())
				{

					if( send_status_itr->status == SEND_OK )
					{

						status = CLIENT_CONNECTION_CHECK;
						break;
					}

					send_status_itr++;

				}

			}

		}
	} catch (...)
	{
		//---------- DEBUG LOG START ------------------------------
		if( LOG_LV_DEBUG == getloglevel())
		{
			boost::format	outform(	"function out : [handle_realserver_recv] : "
										"thread_id = [%s], "
										"next_status = [%d]");
	
			outform % thread_id % FINALIZE;
	
			putLogDebug(	0,
							outform.str(),
							__FILE__,
							__LINE__ );
		}
		//---------- DEBUG LOG END ------------------------------

		return	FINALIZE;
	}

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function out : [handle_realserver_recv] : "
									"thread_id = [%s], "
									"next_status = [%d]");

		outform % thread_id % status;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	return	status;
}

//---------------------------------------------------------------------------------
protocol_module_cinsert::EVENT_TAG
protocol_module_cinsert::handle_realserver_recv(
				const boost::thread::id thread_id,
				const boost::asio::ip::udp::endpoint& rs_endpoint,
				const boost::array< char, MAX_BUFFER_SIZE >& recvbuffer,
				const size_t recvlen )
{
	return	STOP;
}

//---------------------------------------------------------------------------------
protocol_module_cinsert::EVENT_TAG
protocol_module_cinsert::handle_sorryserver_recv(
				const boost::thread::id thread_id,
				const boost::asio::ip::tcp::endpoint& sorry_endpoint,
				const boost::array< char, MAX_BUFFER_SIZE >& recvbuffer,
				const size_t recvlen )
{

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function in  : [handle_sorryserver_recv] : "
									"thread_id = [%s], "
									"sorry_endpoint = [%s], "
									"recvlen = [%d]");

		outform % thread_id % sorry_endpoint % recvlen;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	using namespace boost::xpressive;

	t_session_thread_data_cinsert	thread_data;

	recive_data		sorryserver_recv_data;

	send_status		send_status_add;

	size_t	unsend_data_offset		= 0;
	size_t	unsend_data_size		= 0;
	size_t	rest_response_data_size	= 0;
	size_t	next_response_offset	= 0;

	char*	buffer				= NULL;
	char*	buffer_1			= NULL;
	char*	buffer_2			= NULL;

	bool				find_result			= false;
	CHECK_RESULT_TAG	check_result;
	size_t				http_header_offset	= 0;
	size_t				http_header_len		= 0;
	std::string			http_header_name_blank			= "";
	std::string			http_header_name_content_length	= "Content-Length";
	std::string			content_length;
	match_results< const char* >	regex_result;
	cregex	content_length_regex = icase("Content-Length") >> ":" >> *~_d >> (s1 = +_d) >> *~_d;

	t_session_thread_data_map_itr	thread_data_itr;
	t_recive_data_map_itr			recive_data_itr;
	t_send_status_list_itr			send_status_itr;

	EVENT_TAG	status = STOP;

	try
	{
		{
			boost::mutex::scoped_lock	lock( session_thread_data_map_mutex );

			thread_data_itr = session_thread_data_map.find( thread_id );

			if( thread_data_itr != session_thread_data_map.end() )
			{
				thread_data = thread_data_itr->second;
			}
		}

		if( thread_data != NULL )
		{

			recive_data_itr = thread_data->recive_data_map.find( sorry_endpoint );

			if( recive_data_itr == thread_data->recive_data_map.end() )
			{
				buffer = new char[sorryserver_recv_data.recive_buffer_max_size];
		
				sorryserver_recv_data.recive_buffer		= buffer;
				sorryserver_recv_data.recive_buffer_1	= buffer;
		
				buffer = new char[sorryserver_recv_data.recive_buffer_max_size];
		
				sorryserver_recv_data.recive_buffer_2	= buffer;
		
				thread_data->recive_data_map[ sorry_endpoint ] = sorryserver_recv_data;

				sorryserver_recv_data.recive_buffer_1 = NULL;
				sorryserver_recv_data.recive_buffer_2 = NULL;

				recive_data_itr = thread_data->recive_data_map.find( sorry_endpoint );

			}

			if( recive_data_itr != thread_data->recive_data_map.end() )
			{
				send_status_itr = recive_data_itr->second.send_status_list.begin();

				while( send_status_itr != recive_data_itr->second.send_status_list.end())
				{

					if( send_status_itr->status == SEND_END )
					{

						next_response_offset = send_status_itr->send_offset + send_status_itr->send_end_size;
						send_status_itr = recive_data_itr->second.send_status_list.erase( send_status_itr );
					}
					else if( send_status_itr->status == SEND_CONTINUE )
					{

						send_status_itr->send_offset += send_status_itr->send_end_size;
						unsend_data_offset = send_status_itr->send_offset;
						break;
					}
					else
					{
						unsend_data_offset = send_status_itr->send_offset;
						unsend_data_size = send_status_itr->unsend_size;
						break;
					}

				}

				if( recive_data_itr->second.recive_buffer_rest_size < recvlen )
				{
					if( recive_data_itr->second.recive_buffer_max_size < unsend_data_size + recvlen )
					{

						buffer_1 = new char[unsend_data_size + recvlen];
						buffer_2 = new char[unsend_data_size + recvlen];

						memset( buffer_1, '\0', unsend_data_size + recvlen );
						memset( buffer_2, '\0', unsend_data_size + recvlen );

						memcpy(	buffer_1,
								recive_data_itr->second.recive_buffer + unsend_data_offset,
								unsend_data_size );

						memcpy(	buffer_1 + unsend_data_size,
								recvbuffer.data(),
								recvlen );

						free( recive_data_itr->second.recive_buffer_1 );
						free( recive_data_itr->second.recive_buffer_2 );

						recive_data_itr->second.recive_buffer	= buffer_1;
						recive_data_itr->second.recive_buffer_1	= buffer_1;
						recive_data_itr->second.recive_buffer_2	= buffer_2;

						recive_data_itr->second.recive_buffer_max_size = unsend_data_size + recvlen;

					}
					else
					{

						if( recive_data_itr->second.recive_buffer == recive_data_itr->second.recive_buffer_1 )
						{
							memset( recive_data_itr->second.recive_buffer_2,
									'\0',
									recive_data_itr->second.recive_buffer_max_size );

							memcpy(	recive_data_itr->second.recive_buffer_2,
									recive_data_itr->second.recive_buffer + unsend_data_offset,
									unsend_data_size );

							memcpy(	recive_data_itr->second.recive_buffer_2 + unsend_data_size,
									recvbuffer.data(),
									recvlen );

							recive_data_itr->second.recive_buffer = recive_data_itr->second.recive_buffer_2;
						}
						else
						{
							memset( recive_data_itr->second.recive_buffer_1,
									'\0',
									recive_data_itr->second.recive_buffer_max_size );

							memcpy(	recive_data_itr->second.recive_buffer_1,
									recive_data_itr->second.recive_buffer + unsend_data_offset,
									unsend_data_size );

							memcpy(	recive_data_itr->second.recive_buffer_1 + unsend_data_size,
									recvbuffer.data(),
									recvlen );

							recive_data_itr->second.recive_buffer = recive_data_itr->second.recive_buffer_1;
						}

					}

					recive_data_itr->second.recive_buffer_rest_size
						= recive_data_itr->second.recive_buffer_max_size - unsend_data_size - recvlen;

					next_response_offset = 0;

					unsend_data_size += recvlen;

					send_status_itr = recive_data_itr->second.send_status_list.begin();

					while( send_status_itr != recive_data_itr->second.send_status_list.end())
					{

						send_status_itr->send_offset -= unsend_data_offset;
						send_status_itr++;

					}

				}
				else
				{

					memcpy(	recive_data_itr->second.recive_buffer + recive_data_itr->second.recive_buffer_max_size - recive_data_itr->second.recive_buffer_rest_size,
							recvbuffer.data(),
							recvlen );

					recive_data_itr->second.recive_buffer_rest_size -= recvlen; 

					unsend_data_size += recvlen;

				}

				send_status_itr = recive_data_itr->second.send_status_list.begin();

				rest_response_data_size = recvlen;

				while( send_status_itr != recive_data_itr->second.send_status_list.end())
				{

					if( send_status_itr->status == SEND_CONTINUE )
					{

						if( send_status_itr->send_rest_size > rest_response_data_size )
						{

							send_status_itr->send_possible_size = rest_response_data_size;

							send_status_itr->send_rest_size -= rest_response_data_size;

							send_status_itr->send_end_size = 0;

							rest_response_data_size = 0;

						}
						else
						{

							send_status_itr->send_possible_size = send_status_itr->send_rest_size;

							rest_response_data_size -= send_status_itr->send_rest_size;

							send_status_itr->send_rest_size = 0;

							send_status_itr->send_end_size = 0;

						}

						send_status_itr->edit_division = EDIT_DIVISION_NO_EDIT;

						send_status_itr->status = SEND_OK;

						next_response_offset	= send_status_itr->send_offset
												+ send_status_itr->send_possible_size; 

					}
					else if( send_status_itr->status == SEND_NG )
					{

						check_result = check_status_code(
											(const char*)recive_data_itr->second.recive_buffer
												+ send_status_itr->send_offset,
											unsend_data_size );

						if( check_result == CHECK_OK )
						{
							check_result = check_http_version(
												(const char*)recive_data_itr->second.recive_buffer
													+ send_status_itr->send_offset,
												unsend_data_size );
						}

						if( check_result == CHECK_OK )
						{

							find_result = find_http_header(
											(const char*)recive_data_itr->second.recive_buffer
												+ send_status_itr->send_offset,
											unsend_data_size,
											http_header_name_blank,
											http_header_offset,
											http_header_len );

							if( find_result == true )
							{

								if( http_header_len == 0 )
								{
									send_status_itr->send_rest_size
														= http_header_offset + http_header_len + 2;
								}
								else
								{
									send_status_itr->send_rest_size
														= http_header_offset + http_header_len + 4;
								}

								find_result = find_http_header(
												(const char*)recive_data_itr->second.recive_buffer
													+ send_status_itr->send_offset,
												send_status_itr->send_rest_size,
												http_header_name_content_length,
												http_header_offset,
												http_header_len );

								if( find_result == true )
								{

									content_length.assign(	recive_data_itr->second.recive_buffer
																+ send_status_itr->send_offset
																+ http_header_offset,
															http_header_len );

									find_result = regex_search(	content_length.c_str(),
																regex_result,
																content_length_regex );

									if( find_result == true )
									{

										content_length = content_length.substr(
																		regex_result.position(1),
																		regex_result.length(1));

										send_status_itr->send_rest_size
															+=	boost::lexical_cast<size_t>(
																	content_length);

									}

								}

								send_status_itr->edit_division = EDIT_DIVISION_NO_EDIT;

							}
							else
							{

								send_status_itr->unsend_size += rest_response_data_size;

								rest_response_data_size = 0;

								break;

							}

						}
						else if( check_result == CHECK_NG )
						{

							send_status_itr->edit_division = EDIT_DIVISION_NO_EDIT;

							send_status_itr->send_rest_size = send_status_itr->unsend_size
															+ rest_response_data_size; 

						}
						else
						{

							send_status_itr->unsend_size += rest_response_data_size;

							rest_response_data_size = 0;

							break;

						}

						if( send_status_itr->send_rest_size >
								send_status_itr->unsend_size + rest_response_data_size )
						{

							send_status_itr->send_possible_size
								= send_status_itr->unsend_size + rest_response_data_size;

							send_status_itr->send_rest_size
								-= ( send_status_itr->unsend_size + rest_response_data_size );

							send_status_itr->send_end_size = 0;

							send_status_itr->unsend_size = 0;

							rest_response_data_size = 0;

						}
						else
						{

							send_status_itr->send_possible_size = send_status_itr->send_rest_size;

							rest_response_data_size	= send_status_itr->unsend_size
													+ rest_response_data_size
													- send_status_itr->send_rest_size;

							send_status_itr->send_rest_size = 0;

							send_status_itr->send_end_size = 0;

							send_status_itr->unsend_size = 0;

						}

						send_status_itr->status = SEND_OK;

						next_response_offset	= send_status_itr->send_offset
												+ send_status_itr->send_possible_size;

					}

					if( rest_response_data_size <= 0 )
					{
						break;
					}

					send_status_itr++;

				}

				while( rest_response_data_size > 0 )
				{

					send_status_add.status				= SEND_NG;
					send_status_add.send_end_size		= 0;
					send_status_add.send_rest_size		= 0;
					send_status_add.send_possible_size	= 0;
					send_status_add.unsend_size			= 0;
					send_status_add.edit_division		= 0;
					send_status_add.send_offset			= next_response_offset;

					check_result = check_status_code(
										(const char*)recive_data_itr->second.recive_buffer
											+ send_status_add.send_offset,
										rest_response_data_size );

					if( check_result == CHECK_OK )
					{
						check_result = check_http_version(
											(const char*)recive_data_itr->second.recive_buffer
												+ send_status_add.send_offset,
											rest_response_data_size );
					}

					if( check_result == CHECK_OK )
					{

						find_result = find_http_header(
										(const char*)recive_data_itr->second.recive_buffer
											+ send_status_add.send_offset,
										rest_response_data_size,
										http_header_name_blank,
										http_header_offset,
										http_header_len );

						if( find_result == true )
						{

							if( http_header_len == 0 )
							{
								send_status_add.send_rest_size
												= http_header_offset + http_header_len + 2;
							}
							else
							{
								send_status_add.send_rest_size
												= http_header_offset + http_header_len + 4;
							}

							find_result = find_http_header(
											(const char*)recive_data_itr->second.recive_buffer
												+ send_status_add.send_offset,
											send_status_add.send_rest_size,
											http_header_name_content_length,
											http_header_offset,
											http_header_len );

							if( find_result == true )
							{

								content_length.assign(	recive_data_itr->second.recive_buffer
															+ send_status_add.send_offset
															+ http_header_offset,
														http_header_len );

								find_result = regex_search(	content_length.c_str(),
															regex_result,
															content_length_regex );

								if( find_result == true )
								{

									content_length = content_length.substr(
																	regex_result.position(1),
																	regex_result.length(1));

									send_status_add.send_rest_size
														+=	boost::lexical_cast<size_t>(
																content_length);

								}

							}

							send_status_add.edit_division = EDIT_DIVISION_NO_EDIT;

						}
						else
						{

							send_status_add.unsend_size = rest_response_data_size;

							rest_response_data_size = 0;

							recive_data_itr->second.send_status_list.push_back( send_status_add );

							break;

						}

					}
					else if( check_result == CHECK_NG )
					{

						send_status_add.edit_division = EDIT_DIVISION_NO_EDIT;

						send_status_add.send_rest_size = rest_response_data_size;

					}
					else
					{

						send_status_add.unsend_size = rest_response_data_size;

						rest_response_data_size = 0;

						recive_data_itr->second.send_status_list.push_back( send_status_add );

						break;

					}

					if( send_status_add.send_rest_size > rest_response_data_size )
					{

						send_status_add.send_possible_size = rest_response_data_size;

						send_status_add.send_rest_size -= rest_response_data_size;

						send_status_add.send_end_size = 0;

						send_status_add.unsend_size = 0;

						rest_response_data_size = 0;

					}
					else
					{

						send_status_add.send_possible_size = send_status_add.send_rest_size;

						rest_response_data_size -= send_status_add.send_rest_size;

						send_status_add.send_rest_size = 0;

						send_status_add.send_end_size = 0;

						send_status_add.unsend_size = 0;

					}

					send_status_add.status = SEND_OK;

					next_response_offset	= send_status_add.send_offset
											+ send_status_add.send_possible_size;

					recive_data_itr->second.send_status_list.push_back( send_status_add );

				}

				send_status_itr = recive_data_itr->second.send_status_list.begin();

				status = SORRYSERVER_RECV;

				while( send_status_itr != recive_data_itr->second.send_status_list.end())
				{

					if( send_status_itr->status == SEND_OK )
					{
						status = CLIENT_CONNECTION_CHECK;
						break;
					}

					send_status_itr++;

				}

			}

		}
	} catch (...)
	{
		//---------- DEBUG LOG START ------------------------------
		if( LOG_LV_DEBUG == getloglevel())
		{
			boost::format	outform(	"function out : [handle_sorryserver_recv] : "
										"thread_id = [%s], "
										"next_status = [%d]");
	
			outform % thread_id % FINALIZE;
	
			putLogDebug(	0,
							outform.str(),
							__FILE__,
							__LINE__ );
		}
		//---------- DEBUG LOG END ------------------------------

		return	FINALIZE;
	}

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function out : [handle_sorryserver_recv] : "
									"thread_id = [%s], "
									"next_status = [%d]");

		outform % thread_id % status;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	return	status;
}

//---------------------------------------------------------------------------------
protocol_module_cinsert::EVENT_TAG
protocol_module_cinsert::handle_response_send_inform( const boost::thread::id thread_id )
{
	// NOP
	return	STOP;
}

//---------------------------------------------------------------------------------
protocol_module_cinsert::EVENT_TAG
protocol_module_cinsert::handle_client_connection_check(
				const boost::thread::id thread_id,
				boost::array< char, MAX_BUFFER_SIZE >& sendbuffer,
				size_t& datalen )
{

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function in  : [handle_client_connection_check] : "
									"thread_id = [%s]");

		outform % thread_id;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	t_session_thread_data_cinsert	thread_data;

	size_t	rest_datalen = sendbuffer.size();

	edit_data	edit_data_cookie;

	int			send_ok_flag = 0;

	bool			find_result			= false;
	size_t			http_header_offset	= 0;
	size_t			http_header_len		= 0;
	std::string		http_header_name_blank = "";

	boost::posix_time::ptime			now;
	boost::posix_time::time_duration	expire(0,0,cookie_expire,0);

	t_session_thread_data_map_itr	thread_data_itr;
	t_recive_data_map_itr			recive_data_itr;
	t_send_status_list_itr			send_status_itr;
	t_edit_data_list_itr			edit_data_itr;
	t_edit_data_list_itr			min_insert_position_itr;

	EVENT_TAG	status = STOP;

	try
	{
		{
			boost::mutex::scoped_lock	lock( session_thread_data_map_mutex );

			thread_data_itr = session_thread_data_map.find( thread_id );

			if( thread_data_itr != session_thread_data_map.end() )
			{
				thread_data = thread_data_itr->second;
			}
		}

		datalen = 0;

		if( thread_data != NULL )
		{

			recive_data_itr = thread_data->recive_data_map.find( thread_data->last_endpoint_tcp );

			if( recive_data_itr == thread_data->recive_data_map.end())
			{

				recive_data_itr = thread_data->recive_data_map.begin();
				thread_data->last_endpoint_tcp = recive_data_itr->first;
			}

			while( recive_data_itr != thread_data->recive_data_map.end() )
			{

				send_status_itr = recive_data_itr->second.send_status_list.begin();

				while( send_status_itr != recive_data_itr->second.send_status_list.end())
				{

					if( send_status_itr->status == SEND_OK )
					{

						send_ok_flag = 1;
						break;

					}

					send_status_itr++;

				}

				if( send_ok_flag == 1 )
				{

					break;
				}

				recive_data_itr++;

				if( recive_data_itr == thread_data->recive_data_map.end())
				{

					recive_data_itr = thread_data->recive_data_map.begin();
				}

				if( recive_data_itr->first == thread_data->last_endpoint_tcp )
				{

					break;
				}
			}

			if( send_ok_flag == 1 )
			{

				thread_data->last_endpoint_tcp = recive_data_itr->first;

				if( send_status_itr->edit_division == EDIT_DIVISION_EDIT )
				{
	
					if( send_status_itr->edit_data_list.empty() == true )
					{

						find_result
							= find_http_header(	(const char*)recive_data_itr->second.recive_buffer
													+ send_status_itr->send_offset,
												send_status_itr->send_possible_size,
												http_header_name_blank,
												http_header_offset,
												http_header_len );


						if( find_result == true )
						{

							edit_data_cookie.data
								=	str(boost::format(	"Set-Cookie: %1%=%2%:%3%;" )
														% cookie_name.data()
														% recive_data_itr->first.address()
														% recive_data_itr->first.port());

							now = boost::posix_time::second_clock::universal_time();

							now += expire;

							edit_data_cookie.data
								+=	str(boost::format(	" expires=%1%, %2% %3% %4% %5% GMT;" )
														% now.date().day_of_week()
														% now.date().day()
														% now.date().month()
														% now.date().year()
														% now.time_of_day());

							edit_data_cookie.data
								+=	" path=/\r\n";

							edit_data_cookie.data_size			= edit_data_cookie.data.length(); 
							edit_data_cookie.insert_position	= http_header_offset;
							edit_data_cookie.replace_size		= 0;
	
							send_status_itr->edit_data_list.push_back( edit_data_cookie );
	
						}
	
					}
	
					while( 1 )
					{
	
						if( send_status_itr->edit_data_list.empty() == true )
						{
	
							send_status_itr->edit_division = EDIT_DIVISION_NO_EDIT;
	
							if( rest_datalen > 0 && send_status_itr->send_possible_size > 0 )
							{
	
								if( rest_datalen >= send_status_itr->send_possible_size )
								{
	
									memcpy(	sendbuffer.data()
												+	datalen,
											recive_data_itr->second.recive_buffer
												+	send_status_itr->send_offset
												+	send_status_itr->send_end_size,
											send_status_itr->send_possible_size );
	
									datalen += send_status_itr->send_possible_size; 
	
									send_status_itr->send_end_size += send_status_itr->send_possible_size;

									rest_datalen -= send_status_itr->send_possible_size;
	
									send_status_itr->send_possible_size = 0;

								}
								else
								{
	
									memcpy(	sendbuffer.data()
												+	datalen,
											recive_data_itr->second.recive_buffer
												+	send_status_itr->send_offset
												+	send_status_itr->send_end_size,
											rest_datalen );
	
									datalen += rest_datalen;
	
									send_status_itr->send_end_size += rest_datalen;
	
									send_status_itr->send_possible_size -= rest_datalen;
	
									rest_datalen = 0;
	
								}
	
								break;
	
							}
	
						}
						else
						{
	
							edit_data_itr = send_status_itr->edit_data_list.begin();
	
							min_insert_position_itr = edit_data_itr;
	
							while( edit_data_itr != send_status_itr->edit_data_list.end() )
							{
	
								if( min_insert_position_itr->insert_position
										> edit_data_itr->insert_position )
								{
	
									min_insert_position_itr = edit_data_itr;
	
								}
	
								edit_data_itr++;
	
							}
	
							if( rest_datalen >= ( min_insert_position_itr->insert_position
													- send_status_itr->send_end_size ))
							{
	
								memcpy(	sendbuffer.data()
											+	datalen,
										recive_data_itr->second.recive_buffer
											+	send_status_itr->send_offset
											+	send_status_itr->send_end_size,
										min_insert_position_itr->insert_position
											-	send_status_itr->send_end_size );
	
								datalen
									+=	( min_insert_position_itr->insert_position
											- send_status_itr->send_end_size );

								send_status_itr->send_possible_size
									-=	( min_insert_position_itr->insert_position
											- send_status_itr->send_end_size );
	
								rest_datalen
									-=	( min_insert_position_itr->insert_position
											- send_status_itr->send_end_size );

								send_status_itr->send_end_size
									+=	( min_insert_position_itr->insert_position
											- send_status_itr->send_end_size );

								if( rest_datalen >= min_insert_position_itr->data_size )
								{
	
									memcpy(	sendbuffer.data()
												+	datalen,
											min_insert_position_itr->data.data(),
											min_insert_position_itr->data_size );
	
									datalen += min_insert_position_itr->data_size;
	
									send_status_itr->send_end_size
										+=	min_insert_position_itr->replace_size;
		
									send_status_itr->send_possible_size
										-=	min_insert_position_itr->replace_size;
		
									rest_datalen
										-=	min_insert_position_itr->data_size;
	
									send_status_itr->edit_data_list.erase( min_insert_position_itr );
	
								}
								else
								{
	
									break;
	
								}
	
							}
							else
							{
	
								memcpy(	sendbuffer.data()
											+	datalen,
										recive_data_itr->second.recive_buffer
											+	send_status_itr->send_offset
											+	send_status_itr->send_end_size,
										rest_datalen );
	
								datalen += rest_datalen;
	
								send_status_itr->send_end_size += rest_datalen;
	
								send_status_itr->send_possible_size -= rest_datalen;
	
								rest_datalen = 0;
	
								break;
	
							}
	
						}
	
					}
	
				}
				else
				{
	
					if( sendbuffer.size() >= send_status_itr->send_possible_size )
					{
	
						memcpy(	sendbuffer.data(),
								recive_data_itr->second.recive_buffer
									+	send_status_itr->send_offset,
								send_status_itr->send_possible_size );
	
						datalen = send_status_itr->send_possible_size;
	
						send_status_itr->send_end_size = send_status_itr->send_possible_size;
	
						send_status_itr->send_possible_size = 0;
	
					}
					else
					{
	
						memcpy(	sendbuffer.data(),
								recive_data_itr->second.recive_buffer
									+	send_status_itr->send_offset,
								sendbuffer.size());
	
						datalen = sendbuffer.size();
	
						send_status_itr->send_end_size = sendbuffer.size();
	
						send_status_itr->send_possible_size -= sendbuffer.size();
	
					}
	
				}
	
				status = CLIENT_SEND;

			}

		}
	} catch (...)
	{
		//---------- DEBUG LOG START ------------------------------
		if( LOG_LV_DEBUG == getloglevel())
		{
			boost::format	outform(	"function out : [handle_client_connection_check] : "
										"thread_id = [%s], "
										"next_status = [%d]");
	
			outform % thread_id % FINALIZE;
	
			putLogDebug(	0,
							outform.str(),
							__FILE__,
							__LINE__ );
		}
		//---------- DEBUG LOG END ------------------------------

		return	FINALIZE;
	}

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function out : [handle_client_connection_check] : "
									"thread_id = [%s], "
									"next_status = [%d], "
									"datalen = [%d]");

		outform % thread_id % status % datalen;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	return	status;

}

//---------------------------------------------------------------------------------
protocol_module_cinsert::EVENT_TAG
protocol_module_cinsert::handle_client_select(
				const boost::thread::id thread_id,
				boost::asio::ip::udp::endpoint& cl_endpoint,
				boost::array< char, MAX_BUFFER_SIZE >& sendbuffer,
				size_t& datalen )
{
	// NOP
	return	STOP;
}

//---------------------------------------------------------------------------------
protocol_module_cinsert::EVENT_TAG
protocol_module_cinsert::handle_client_send( const boost::thread::id thread_id )
{

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function in  : [handle_client_send] : "
									"thread_id = [%s]");

		outform % thread_id;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	t_session_thread_data_cinsert	thread_data;

	t_session_thread_data_map_itr	thread_data_itr;
	t_recive_data_map_itr			recive_data_itr;
	t_send_status_list_itr			send_status_itr;
	t_edit_data_list_itr			edit_data_itr;

	EVENT_TAG	status = STOP;

	try
	{
		{
			boost::mutex::scoped_lock	lock( session_thread_data_map_mutex );

			thread_data_itr = session_thread_data_map.find( thread_id );

			if( thread_data_itr != session_thread_data_map.end() )
			{
				thread_data = thread_data_itr->second;
			}
		}

		if( thread_data != NULL )
		{
			recive_data_itr = thread_data->recive_data_map.find( thread_data->last_endpoint_tcp );

			if( recive_data_itr != thread_data->recive_data_map.end() )
			{

				send_status_itr = recive_data_itr->second.send_status_list.begin();

				while( send_status_itr != recive_data_itr->second.send_status_list.end())
				{

					if( send_status_itr->status == SEND_OK )
					{

						break;

					}

					send_status_itr++;

				}

				if( send_status_itr != recive_data_itr->second.send_status_list.end())
				{
					if( send_status_itr->send_possible_size > 0 )
					{
	
						send_status_itr->send_offset += send_status_itr->send_end_size;
	
						edit_data_itr = send_status_itr->edit_data_list.begin();
	
						while( edit_data_itr != send_status_itr->edit_data_list.end())
						{
	
							edit_data_itr->insert_position -= send_status_itr->send_end_size;
	
							edit_data_itr++;

						}
	
						send_status_itr->send_end_size = 0;
	
					}
					else
					{
						if( send_status_itr->send_rest_size > 0 )
						{
							send_status_itr->status = SEND_CONTINUE;
						}
						else
						{
							send_status_itr->status = SEND_END;
						}
					}

				}
			}

			while( recive_data_itr != thread_data->recive_data_map.end() )
			{

				send_status_itr = recive_data_itr->second.send_status_list.begin();

				while( send_status_itr != recive_data_itr->second.send_status_list.end())
				{

					if( send_status_itr->status == SEND_OK )
					{

						status = CLIENT_CONNECTION_CHECK;
						break;

					}

					send_status_itr++;

				}

				if( status == CLIENT_CONNECTION_CHECK )
				{
					break;
				}

				recive_data_itr++;

				if( recive_data_itr == thread_data->recive_data_map.end())
				{
					recive_data_itr == thread_data->recive_data_map.begin();
				}

				if( recive_data_itr->first == thread_data->last_endpoint_tcp )
				{
					break;
				}
			}

			if( status != CLIENT_CONNECTION_CHECK )
			{
				if( thread_data->end_flag == END_FLAG_ON )
				{
					status = CLIENT_DISCONNECT;
				}
				else
				{
					if( thread_data->sorry_flag == SORRY_FLAG_ON )
					{
						status = SORRYSERVER_RECV;
					}
					else
					{
						status = REALSERVER_RECV;
					}

				}

			}

		}
	} catch (...)
	{
		//---------- DEBUG LOG START ------------------------------
		if( LOG_LV_DEBUG == getloglevel())
		{
			boost::format	outform(	"function out : [handle_client_send] : "
										"thread_id = [%s], "
										"next_status = [%d]");
	
			outform % thread_id % FINALIZE;
	
			putLogDebug(	0,
							outform.str(),
							__FILE__,
							__LINE__ );
		}
		//---------- DEBUG LOG END ------------------------------

		return	FINALIZE;
	}

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function out : [handle_client_send] : "
									"thread_id = [%s], "
									"next_status = [%d]");

		outform % thread_id % status;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	return	status;

}

//---------------------------------------------------------------------------------
protocol_module_cinsert::EVENT_TAG
protocol_module_cinsert::handle_client_disconnect( const boost::thread::id thread_id )
{

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform_1(	"function in  : [handle_client_disconnect] : "
									"thread_id = [%s]");

		outform_1 % thread_id;

		putLogDebug(	0,
						outform_1.str(),
						__FILE__,
						__LINE__ );

		boost::format	outform_2(	"function out : [handle_client_disconnect] : "
									"thread_id = [%s], "
									"next_status = [%d]");

		outform_2 % thread_id % FINALIZE;

		putLogDebug(	0,
						outform_2.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	return	FINALIZE;
}

//---------------------------------------------------------------------------------
protocol_module_cinsert::EVENT_TAG
protocol_module_cinsert::handle_sorry_enable( const boost::thread::id thread_id )
{

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function in  : [handle_sorry_enable] : "
									"thread_id = [%s]");

		outform % thread_id;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	t_session_thread_data_cinsert	thread_data;

	t_session_thread_data_map_itr	thread_data_itr;
	t_recive_data_map_itr			recive_data_itr;
	t_send_status_list_itr			send_status_itr;

	EVENT_TAG	status = STOP;

	try
	{
		{
			boost::mutex::scoped_lock	lock( session_thread_data_map_mutex );

			thread_data_itr = session_thread_data_map.find( thread_id );

			if( thread_data_itr != session_thread_data_map.end() )
			{
				thread_data = thread_data_itr->second;
			}
		}

		if( thread_data != NULL )
		{
			if( thread_data->thread_division == THREAD_DIVISION_UP_STREAM )
			{
				if( thread_data->accept_end_flag == ACCEPT_END_FLAG_OFF )
				{

					thread_data->sorry_flag = SORRY_FLAG_ON;

					status = ACCEPT;

				}
				else
				{
					if( thread_data->sorry_flag == SORRY_FLAG_ON )
					{

						recive_data_itr = thread_data->recive_data_map.find( thread_data->client_endpoint_tcp );

						if( recive_data_itr != thread_data->recive_data_map.end() )
						{

							status = CLIENT_RECV;

							send_status_itr = recive_data_itr->second.send_status_list.begin();

							while( send_status_itr != recive_data_itr->second.send_status_list.end())
							{

								if( send_status_itr->status == SEND_OK )
								{

									status = SORRYSERVER_SELECT;
									break;

								}

								send_status_itr++;

							}
						}
					}
					else
					{

						recive_data_itr = thread_data->recive_data_map.find( thread_data->client_endpoint_tcp );

						if( recive_data_itr != thread_data->recive_data_map.end() )
						{

							thread_data->sorryserver_switch_flag = SORRYSERVER_SWITCH_FLAG_ON;
							thread_data->sorry_flag = SORRY_FLAG_ON;
							status = REALSERVER_DISCONNECT;

							send_status_itr = recive_data_itr->second.send_status_list.begin();

							while( send_status_itr != recive_data_itr->second.send_status_list.end())
							{

								if( send_status_itr->status == SEND_CONTINUE )
								{

									thread_data->end_flag = END_FLAG_ON;
									break;

								}

								send_status_itr++;

							}
						}
					}
				}
			}
			else
			{

				if( thread_data->sorry_flag == SORRY_FLAG_ON )
				{

					recive_data_itr = thread_data->recive_data_map.begin();

					status = SORRYSERVER_RECV;

					while( recive_data_itr != thread_data->recive_data_map.end() )
					{

						send_status_itr = recive_data_itr->second.send_status_list.begin();

						while( send_status_itr != recive_data_itr->second.send_status_list.end())
						{

							if( send_status_itr->status == SEND_OK )
							{

								status = CLIENT_CONNECTION_CHECK;
								break;

							}

							send_status_itr++;

						}

						if( status == CLIENT_CONNECTION_CHECK )
						{
							break;
						}

						recive_data_itr++;

					}

				}
				else
				{

					recive_data_itr = thread_data->recive_data_map.begin();

					thread_data->sorryserver_switch_flag = SORRYSERVER_SWITCH_FLAG_ON;

					thread_data->sorry_flag = SORRY_FLAG_ON;
					status = SORRYSERVER_RECV;

					while( recive_data_itr != thread_data->recive_data_map.end() )
					{

						send_status_itr = recive_data_itr->second.send_status_list.begin();

						while( send_status_itr != recive_data_itr->second.send_status_list.end())
						{

							if( send_status_itr->status == SEND_NG ||
								send_status_itr->send_rest_size > 0 )
							{

								thread_data->end_flag = END_FLAG_ON;
								status = REALSERVER_DISCONNECT;
								break;

							}
							else if( send_status_itr->status == SEND_OK )
							{

								status = CLIENT_CONNECTION_CHECK;

							}

							send_status_itr++;

						}

						if( status == REALSERVER_DISCONNECT )
						{
							break;
						}

						recive_data_itr++;

					}
				}
			}

		}
	} catch (...)
	{
		//---------- DEBUG LOG START ------------------------------
		if( LOG_LV_DEBUG == getloglevel())
		{
			boost::format	outform(	"function out : [handle_sorry_enable] : "
										"thread_id = [%s], "
										"next_status = [%d]");
	
			outform % thread_id % FINALIZE;
	
			putLogDebug(	0,
							outform.str(),
							__FILE__,
							__LINE__ );
		}
		//---------- DEBUG LOG END ------------------------------

		return	FINALIZE;
	}

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function out : [handle_sorry_enable] : "
									"thread_id = [%s], "
									"next_status = [%d]");

		outform % thread_id % status;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	return	status;

}

//---------------------------------------------------------------------------------
protocol_module_cinsert::EVENT_TAG
protocol_module_cinsert::handle_sorry_disable( const boost::thread::id thread_id )
{

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function in  : [handle_sorry_disable] : "
									"thread_id = [%s]");

		outform % thread_id;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	t_session_thread_data_cinsert	thread_data;

	t_session_thread_data_map_itr	thread_data_itr;
	t_recive_data_map_itr			recive_data_itr;
	t_send_status_list_itr			send_status_itr;

	EVENT_TAG	status = STOP;

	try
	{
		{
			boost::mutex::scoped_lock	lock( session_thread_data_map_mutex );

			thread_data_itr = session_thread_data_map.find( thread_id );

			if( thread_data_itr != session_thread_data_map.end() )
			{
				thread_data = thread_data_itr->second;
			}
		}

		if( thread_data != NULL )
		{
			if( thread_data->thread_division == THREAD_DIVISION_UP_STREAM )
			{
				if( thread_data->accept_end_flag == ACCEPT_END_FLAG_OFF )
				{

					thread_data->sorry_flag = SORRY_FLAG_OFF;

					status = ACCEPT;

				}
				else
				{
					if( thread_data->sorry_flag == SORRY_FLAG_OFF )
					{

						recive_data_itr = thread_data->recive_data_map.find( thread_data->client_endpoint_tcp );

						if( recive_data_itr != thread_data->recive_data_map.end() )
						{

							status = CLIENT_RECV;

							send_status_itr = recive_data_itr->second.send_status_list.begin();

							while( send_status_itr != recive_data_itr->second.send_status_list.end())
							{

								if( send_status_itr->status == SEND_OK )
								{

									status = REALSERVER_SELECT;
									break;

								}

								send_status_itr++;

							}
						}
					}
					else
					{

						recive_data_itr = thread_data->recive_data_map.find( thread_data->client_endpoint_tcp );

						if( recive_data_itr != thread_data->recive_data_map.end() )
						{

							thread_data->realserver_switch_flag = REALSERVER_SWITCH_FLAG_ON;
							thread_data->sorry_flag = SORRY_FLAG_OFF;
							status = SORRYSERVER_DISCONNECT;

							send_status_itr = recive_data_itr->second.send_status_list.begin();

							while( send_status_itr != recive_data_itr->second.send_status_list.end())
							{

								if( send_status_itr->status == SEND_CONTINUE )
								{

									thread_data->end_flag = END_FLAG_ON;
									break;

								}

								send_status_itr++;

							}
						}
					}
				}
			}
			else
			{

				if( thread_data->sorry_flag == SORRY_FLAG_OFF )
				{

					recive_data_itr = thread_data->recive_data_map.begin();

					status = REALSERVER_RECV;

					while( recive_data_itr != thread_data->recive_data_map.end() )
					{

						send_status_itr = recive_data_itr->second.send_status_list.begin();

						while( send_status_itr != recive_data_itr->second.send_status_list.end())
						{

							if( send_status_itr->status == SEND_OK )
							{

								status = CLIENT_CONNECTION_CHECK;
								break;

							}

							send_status_itr++;

						}

						if( status == CLIENT_CONNECTION_CHECK )
						{
							break;
						}

						recive_data_itr++;

					}

				}
				else
				{

					recive_data_itr = thread_data->recive_data_map.begin();

					thread_data->realserver_switch_flag = REALSERVER_SWITCH_FLAG_ON;

					thread_data->sorry_flag = SORRY_FLAG_OFF;
					status = REALSERVER_RECV;

					while( recive_data_itr != thread_data->recive_data_map.end() )
					{

						send_status_itr = recive_data_itr->second.send_status_list.begin();

						while( send_status_itr != recive_data_itr->second.send_status_list.end())
						{

							if( send_status_itr->status == SEND_NG ||
								send_status_itr->send_rest_size > 0 )
							{

								thread_data->end_flag = END_FLAG_ON;
								status = SORRYSERVER_DISCONNECT;
								break;

							}
							else if( send_status_itr->status == SEND_OK )
							{

								status = CLIENT_CONNECTION_CHECK;

							}

							send_status_itr++;

						}

						if( status == SORRYSERVER_DISCONNECT )
						{
							break;
						}

						recive_data_itr++;

					}
				}
			}

		}
	} catch (...)
	{
		//---------- DEBUG LOG START ------------------------------
		if( LOG_LV_DEBUG == getloglevel())
		{
			boost::format	outform(	"function out : [handle_sorry_disable] : "
										"thread_id = [%s], "
										"next_status = [%d]");
	
			outform % thread_id % FINALIZE;
	
			putLogDebug(	0,
							outform.str(),
							__FILE__,
							__LINE__ );
		}
		//---------- DEBUG LOG END ------------------------------

		return	FINALIZE;
	}

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function out : [handle_sorry_disable] : "
									"thread_id = [%s], "
									"next_status = [%d]");

		outform % thread_id % status;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	return	status;
}

//---------------------------------------------------------------------------------
protocol_module_cinsert::EVENT_TAG
protocol_module_cinsert::handle_realserver_disconnect(
				const boost::thread::id thread_id,
				const boost::asio::ip::tcp::endpoint & rs_endpoint )
{

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function in  : [handle_realserver_disconnect] : "
									"thread_id = [%s], "
									"rs_endpoint = [%s]");

		outform % thread_id % rs_endpoint;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	t_session_thread_data_cinsert	thread_data;

	t_session_thread_data_map_itr	thread_data_itr;
	t_recive_data_map_itr			recive_data_itr;
	t_send_status_list_itr			send_status_itr;

	EVENT_TAG	status = STOP;

	try
	{
		{
			boost::mutex::scoped_lock	lock( session_thread_data_map_mutex );

			thread_data_itr = session_thread_data_map.find( thread_id );

			if( thread_data_itr != session_thread_data_map.end() )
			{
				thread_data = thread_data_itr->second;
			}
		}

		if( thread_data != NULL )
		{
			if( thread_data->thread_division == THREAD_DIVISION_UP_STREAM )
			{
				if( thread_data->end_flag == END_FLAG_ON )
				{
					status = CLIENT_RECV;
				}
				else
				{
					if( thread_data->sorryserver_switch_flag == SORRYSERVER_SWITCH_FLAG_ON )
					{

						thread_data->sorryserver_switch_flag = SORRYSERVER_SWITCH_FLAG_OFF;

						recive_data_itr = thread_data->recive_data_map.find( thread_data->client_endpoint_tcp );

						if( recive_data_itr != thread_data->recive_data_map.end() )
						{

							status = CLIENT_RECV;

							send_status_itr = recive_data_itr->second.send_status_list.begin();

							while( send_status_itr != recive_data_itr->second.send_status_list.end())
							{

								if( send_status_itr->status == SEND_OK )
								{

									status = SORRYSERVER_SELECT;
									break;

								}

								send_status_itr++;

							}
						}

					}
					else
					{

						thread_data->end_flag = END_FLAG_ON;
						status = CLIENT_RECV;

					}
				}
			}
			else
			{

				if( thread_data->end_flag == END_FLAG_ON )
				{
					status = CLIENT_DISCONNECT;
				}
				else
				{
					if( thread_data->sorryserver_switch_flag == SORRYSERVER_SWITCH_FLAG_ON )
					{
					
						thread_data->sorryserver_switch_flag = SORRYSERVER_SWITCH_FLAG_OFF;
						status = SORRYSERVER_RECV;
					
					}
					else
					{
					
						thread_data->end_flag = END_FLAG_ON;
						status = CLIENT_DISCONNECT;
					
					}
				}

				recive_data_itr = thread_data->recive_data_map.begin();

				while( recive_data_itr != thread_data->recive_data_map.end() )
				{

					send_status_itr = recive_data_itr->second.send_status_list.begin();

					while( send_status_itr != recive_data_itr->second.send_status_list.end())
					{

						if( send_status_itr->status == SEND_OK )
						{

							status = CLIENT_CONNECTION_CHECK;
							break;

						}

						send_status_itr++;

					}

					if( status == CLIENT_CONNECTION_CHECK )
					{
						break;
					}

					recive_data_itr++;

				}

			}

		}
	} catch (...)
	{
		//---------- DEBUG LOG START ------------------------------
		if( LOG_LV_DEBUG == getloglevel())
		{
			boost::format	outform(	"function out : [handle_realserver_disconnect] : "
										"thread_id = [%s], "
										"next_status = [%d]");
	
			outform % thread_id % FINALIZE;
	
			putLogDebug(	0,
							outform.str(),
							__FILE__,
							__LINE__ );
		}
		//---------- DEBUG LOG END ------------------------------

		return	FINALIZE;
	}

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function out : [handle_realserver_disconnect] : "
									"thread_id = [%s], "
									"next_status = [%d]");

		outform % thread_id % status;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	return	status;
}

//---------------------------------------------------------------------------------
protocol_module_cinsert::EVENT_TAG
protocol_module_cinsert::handle_sorryserver_disconnect(
				const boost::thread::id thread_id,
				const boost::asio::ip::tcp::endpoint & sorry_endpoint )
{

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function in  : [handle_sorryserver_disconnect] : "
									"thread_id = [%s], "
									"sorry_endpoint = [%s]");

		outform % thread_id % sorry_endpoint;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	t_session_thread_data_cinsert	thread_data;

	t_session_thread_data_map_itr	thread_data_itr;
	t_recive_data_map_itr			recive_data_itr;
	t_send_status_list_itr			send_status_itr;

	EVENT_TAG	status = STOP;

	try
	{
		{
			boost::mutex::scoped_lock	lock( session_thread_data_map_mutex );

			thread_data_itr = session_thread_data_map.find( thread_id );

			if( thread_data_itr != session_thread_data_map.end() )
			{
				thread_data = thread_data_itr->second;
			}
		}

		if( thread_data != NULL )
		{
			if( thread_data->thread_division == THREAD_DIVISION_UP_STREAM )
			{
				if( thread_data->end_flag == END_FLAG_ON )
				{
					status = CLIENT_RECV;
				}
				else
				{

					if( thread_data->realserver_switch_flag == REALSERVER_SWITCH_FLAG_ON )
					{

						thread_data->realserver_switch_flag = REALSERVER_SWITCH_FLAG_OFF;

						recive_data_itr = thread_data->recive_data_map.find( thread_data->client_endpoint_tcp );

						if( recive_data_itr != thread_data->recive_data_map.end() )
						{

							status = CLIENT_RECV;

							send_status_itr = recive_data_itr->second.send_status_list.begin();

							while( send_status_itr != recive_data_itr->second.send_status_list.end())
							{

								if( send_status_itr->status == SEND_OK )
								{

									status = REALSERVER_SELECT;
									break;

								}

								send_status_itr++;

							}
						}

					}
					else
					{

						thread_data->end_flag = END_FLAG_ON;
						status = CLIENT_RECV;

					}
				}
			}
			else
			{

				if( thread_data->end_flag == END_FLAG_ON )
				{
					status = CLIENT_DISCONNECT;
				}
				else
				{
					if( thread_data->realserver_switch_flag == REALSERVER_SWITCH_FLAG_ON )
					{
					
						thread_data->realserver_switch_flag = REALSERVER_SWITCH_FLAG_OFF;
						status = REALSERVER_RECV;
					
					}
					else
					{
					
						thread_data->end_flag = END_FLAG_ON;
						status = CLIENT_DISCONNECT;
					
					}
				}

				recive_data_itr = thread_data->recive_data_map.begin();

				while( recive_data_itr != thread_data->recive_data_map.end() )
				{

					send_status_itr = recive_data_itr->second.send_status_list.begin();

					while( send_status_itr != recive_data_itr->second.send_status_list.end())
					{

						if( send_status_itr->status == SEND_OK )
						{

							status = CLIENT_CONNECTION_CHECK;
							break;

						}

						send_status_itr++;

					}

					if( status == CLIENT_CONNECTION_CHECK )
					{
						break;
					}

					recive_data_itr++;

				}

			}

		}
	} catch (...)
	{
		//---------- DEBUG LOG START ------------------------------
		if( LOG_LV_DEBUG == getloglevel())
		{
			boost::format	outform(	"function out : [handle_sorryserver_disconnect] : "
										"thread_id = [%s], "
										"next_status = [%d]");
	
			outform % thread_id % FINALIZE;
	
			putLogDebug(	0,
							outform.str(),
							__FILE__,
							__LINE__ );
		}
		//---------- DEBUG LOG END ------------------------------

		return	FINALIZE;
	}

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function out : [handle_sorryserver_disconnect] : "
									"thread_id = [%s], "
									"next_status = [%d]");

		outform % thread_id % status;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	return	status;

}

//---------------------------------------------------------------------------------
protocol_module_cinsert::EVENT_TAG
protocol_module_cinsert::handle_realserver_close(
				const boost::thread::id thread_id,
				const boost::asio::ip::udp::endpoint & rs_endpoint )
{
	// NOP
	return	STOP;
}

}
//---------------------------------------------------------------------------------
extern "C" l7vs::protocol_module_base*
create_module(){
	return dynamic_cast<l7vs::protocol_module_base*>(new l7vs::protocol_module_cinsert());
}

extern "C" void
destroy_module( l7vs::protocol_module_base* in ){
	delete in;
}
