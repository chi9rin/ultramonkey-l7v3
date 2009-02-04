#include <vector>
#include <list>
#include <sstream>
#include <algorithm>
#include <boost/thread/pthread/mutex.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/regex.hpp>
#include <boost/format.hpp>
#include "protocol_module_sessionless.h"

namespace l7vs
{

const std::string protocol_module_sessionless::MODULE_NAME = "sessionless";
//construct
protocol_module_sessionless::protocol_module_sessionless() :
	http_protocol_module_base(MODULE_NAME)
{
	//	/*-------- DEBUG LOG --------*/
	//	if (LOG_LV_DEBUG == getloglevel())
	//	{
	//		putLogDebug(010000, "in/out_function: protocol_module_sessionless::"
	//			"protocol_module_sessionless()", __FILE__, __LINE__ );
	//
	//		boost::format formatter("out_function: protocol_module_sessionless::protocol_module_sessionless(): "
	//			"modename=%s");
	//		formatter % MODULE_NAME;
	//		putLogDebug(010001, formatter.str(), __FILE__, __LINE__ );
	//	}
	//	/*------DEBUG LOG END------*/
}
//desconstruct
protocol_module_sessionless::~protocol_module_sessionless()
{
//	/*-------- DEBUG LOG --------*/
//	if (LOG_LV_DEBUG == getloglevel())
//	{
//		putLogDebug(010002, "in/out_function: protocol_module_sessionless::"
//			"~protocol_module_sessionless()", __FILE__, __LINE__ );
//	}
//	/*------DEBUG LOG END------*/
}
//! tcp protocol support check
//! @return tcp support is true
//! @return tcp not-support is false
bool protocol_module_sessionless::is_tcp()
{
	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		putLogDebug(010003, "in/out_function: bool protocol_module_sessionless::is_tcp(): "
			"return_value=true", __FILE__, __LINE__ );
	}
	/*------DEBUG LOG END------*/
	return true;
}

//! udp protocol support check
//! @return udp support is true
//! @return udp not-support is false
bool protocol_module_sessionless::is_udp()
{
	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		putLogDebug(010004, "in/out_function: bool protocol_module_sessionless::is_udp(): "
			"return_value=false", __FILE__, __LINE__ );
	}
	/*------DEBUG LOG END------*/
	return false;
}

//replication用インターフェイス
void protocol_module_sessionless::replication_interrupt()
{
	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		putLogDebug(010002, "in/out_function: protocol_module_sessionless::"
			"replication_interrupt()", __FILE__, __LINE__ );
	}
	/*------DEBUG LOG END------*/
}
/*!
 *initialize
 *
 *@param [in] inrslist_begin
 *@param [in] inrslist_end
 *@param [in] inrslist_next
 *@param [in] inlist_lock
 *@param [in] inlist_unlock
 */
void protocol_module_sessionless::initialize(rs_list_itr_func_type	inlist_begin,
		rs_list_itr_func_type	inlist_end,
		rs_list_itr_next_func_type	inlist_next,
		boost::function< void( void ) >	inlist_lock,
		boost::function< void( void ) >	inlist_unlock)
{
	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		putLogDebug(010005, "in_function: void protocol_module_sessionless::initialize("
			"rs_list_itr_func_type inlist_begin, rs_list_itr_func_type inlist_end,"
			"rs_list_itr_next_func_type inlist_next,boost::function< void( void ) >	"
			"inlist_lock,boost::function< void( void ) >	inlist_unlock )", __FILE__, __LINE__ );
	}
	/*------DEBUG LOG END------*/

	//RealServerリストの各操作関数を設定する
	//RealServerリスト先頭取得関数
	rs_list_begin = inlist_begin;
	//RealServerリスト末端取得関数
	rs_list_end = inlist_end;
	//RealServerリスト次要素取得関数
	rs_list_next = inlist_next;
	//RealServerリストロック関数
	rs_list_lock = inlist_lock;
	//RealServerリストアンロック関数
	rs_list_unlock = inlist_unlock;

	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		putLogDebug(010006, "out_function: void protocol_module_sessionless::initialize("
			"rs_list_itr_func_type inlist_begin, rs_list_itr_func_type inlist_end,"
			"rs_list_itr_next_func_type inlist_next,boost::function< void( void ) >	"
			"inlist_lock,boost::function< void( void ) >	inlist_unlock )", __FILE__, __LINE__ );
	}
	/*-------- DEBUG LOG --------*/
}

/*!
 *finalize
 *
 */
void protocol_module_sessionless::finalize()
{
	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		putLogDebug(010007, "int_function: void protocol_module_sessionless::finalize()", __FILE__,
		__LINE__ );
	}
	/*------DEBUG LOG END------*/

	//ログ出力の各操作関数を初期化する
	//ログレベル取得関数
	getloglevel.clear();
	//ログ出力関数（Fatal）
	putLogFatal.clear();
	//ログ出力関数（Error）
	putLogError.clear();
	//ログ出力関数（Warn）
	putLogWarn.clear();
	//ログ出力関数（Info）
	putLogInfo.clear();
	//ログ出力関数（Debug）
	putLogDebug.clear();

	//RealServerリストの各操作関数を初期化する
	//RealServerリスト先頭取得関数
	rs_list_begin.clear();
	//RealServerリスト末端取得関数
	rs_list_end.clear();
	//RealServerリスト次要素取得関数
	rs_list_next.clear();
	//RealServerリストロック関数
	rs_list_lock.clear();
	//RealServerリストアンロック関数
	rs_list_unlock.clear();

	//Replicationの各操作関数を初期化する
	//コンポーネント領域割り当て関数
	replication_pay_memory.clear();
	//コンポーネント領域ロック関数
	replication_area_lock.clear();
	//コンポーネント領域アンロック関数
	replication_area_unlock.clear();

	//ScheduleModuleの振分関数を初期化する
	//Client振り分け処理関数
	schedule_tcp.clear();
	schedule_udp.clear();

	//各モジュールオプションを初期化する
	//送信元設定指示
	forwarded_for = 0;
	//sorry-uri
	sorry_uri.assign('\0');

	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		putLogDebug(010007, "out_function: void protocol_module_sessionless::finalize()", __FILE__,
		__LINE__ );
	}
	/*------DEBUG LOG END------*/
}

/*!
 *is_use_sorry
 *@return always return true
 */
bool protocol_module_sessionless::is_use_sorry()
{
	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		putLogDebug(010007, "in/out_function: bool protocol_module_sessionless::is_use_sorry(): "
			"return_value=true", __FILE__, __LINE__ );
	}
	/*------DEBUG LOG END------*/
	return true;
}

/*!
 *check_parameter
 *
 *@param [in] args
 *@retval     check_message_result_return
 */
protocol_module_sessionless::check_message_result protocol_module_sessionless::check_parameter(const std::vector<
        std::string>& args)
{
	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		boost::format formatter("int_function: protocol_module_sessionless::check_message_result "
			"protocol_module_sessionless::check_parameter("
			"const std::vector<std::string>& args ): args=%s");
		std::string argsdump;
		for (std::vector<std::string>::const_iterator it = args.begin(); it != args.end(); ++it)
		{
			argsdump += *it;
		}
		formatter % argsdump;
		putLogDebug(010010, formatter.str(), __FILE__, __LINE__ );
	}
	/*------DEBUG LOG END------*/

	//チェック結果フラグにTRUEを設定する
	check_message_result check_result;
	check_result.flag = true;
	bool forward_checked = false;
	bool sorryuri_checked = false;
	const std::string regex_match_str = "([^?#]*)(\\?([^#]*))?(#(.*))?";

	//オプション文字列の要素数分繰り返し
	typedef std::vector<std::string>::const_iterator vec_str_it;

	try
	{
		vec_str_it it = args.begin();
		vec_str_it it_end = args.end();
		for (; it != it_end; ++it)
		{
			//オプション文字列 = "-F"
			if (*it == "-F" || *it == "--forwarded-for")
			{
				//送信元設定指示設定フラグをON
				forward_checked = true;
			}
			//オプション文字列 = "-S"
			else if (*it == "-S" || *it == "--sorry-uri")
			{
				//sorryURI設定フラグ = OFF
				if (!sorryuri_checked)
				{
					//次要素が存在する場合
					if (++it != it_end)
					{
						if (!it->empty() && (it->substr(0, 1) == "-" || it->substr(0, 2) == "--"))
						{
							//チェック結果フラグにFALSEを設定する
							check_result.flag = false;
							//チェック結果メッセージを設定する
							check_result.message = "You have to set option value '-S/--sorry-uri'.";
							//繰り返し処理を終了する
							break;
						}
						//次要素の文字列長 ＞ 127
						if (it->size() > MAX_OPTION_SIZE - 1)
						{
							std::ostringstream ostr;
							ostr << "'-S/--sorry-uri\' option value '" << *it << "' is too long.";

							//チェック結果フラグにFALSEを設定する
							check_result.flag = false;
							//チェック結果メッセージを設定する
							check_result.message = ostr.str();
							putLogError(17000, check_result.message, __FILE__, __LINE__ );
							//繰り返し処理を終了する
							break;
						}
						//次要素の文字列長 ≦ 127
						else
						{
							//正規表現でURI（httpからポートまでの部分を対象外）の妥当性をチェックする
							//正規表現は([^?#]*)(\?([^#]*))?(#(.*))?通り
							boost::regex uri(regex_match_str);
							boost::cmatch what;
							if (boost::regex_match(it->c_str(), what, uri))
							{
								//チェックOKの場合
								//sorryURI設定フラグをON
								sorryuri_checked = true;
							}
							//チェックNGの場合
							else
							{
								std::ostringstream ostr;
								ostr << "'-S/--sorry-uri' option value '" << *it << "' is not a valid URI.";

								//チェック結果フラグにFALSEを設定する
								check_result.flag = false;
								//チェック結果メッセージを設定する
								check_result.message = ostr.str();
								putLogError(17000, check_result.message, __FILE__, __LINE__ );
								//繰り返し処理を終了する
								break;
							}
						}
					}
					//次要素が存在しない場合
					else
					{
						//チェック結果フラグにFALSEを設定する
						check_result.flag = false;
						//チェック結果メッセージを設定する
						check_result.message = "You have to set option value '-S/--sorry-uri'.";
						putLogError(17000, check_result.message, __FILE__,
						__LINE__ );
						//繰り返し処理を終了する
						break;
					}
				}
				//sorryURI設定フラグ = ON
				else
				{
					//チェック結果フラグにFALSEを設定する
					check_result.flag = false;
					//チェック結果メッセージを設定する
					check_result.message = "Cannot set multiple option '-S/--sorry-uri'.";
					putLogError(17000, check_result.message, __FILE__,
					__LINE__ );
					//繰り返し処理を終了する
					break;
				}
			}
			//オプション文字列 = 上記以外
			else
			{
				//チェック結果フラグにFALSEを設定する
				check_result.flag = false;
				//チェック結果メッセージを設定する
				check_result.message = "Option error.";
				putLogError(17000, check_result.message, __FILE__, __LINE__ );
				//繰り返し処理を終了する
				break;
			}
		}
	} catch (const std::exception& ex)
	{
		check_result.flag = false;
		std::cerr << "check parameter exception: result=false, error=" << ex.what() << "." << std::endl;
		boost::format formatter("function protocol_module_sessionless::check_message_result "
			"protocol_module_sessionless::check_parameter() exception: "
			"result=false, error=%s.");
		formatter % ex.what();
		putLogError(17000, formatter.str(), __FILE__, __LINE__ );
	} catch (...)
	{
		check_result.flag = false;
		std::cerr << "Unkown exception." << std::endl;
		putLogError(17000, "function protocol_module_sessionless::check_message_result "
			"protocol_module_sessionless::check_parameter() :"
			"Unkown exception.", __FILE__, __LINE__ );
	}

	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		boost::format formatter("out_function: protocol_module_sessionless::check_message_result "
			"protocol_module_sessionless::check_parameter("
			"const std::vector<std::string>& args ): return_value=("
			"check_message_result.flag=%d, check_message_result.message=%s");
		formatter % check_result.flag % check_result.message;
		putLogDebug(010005, formatter.str(), __FILE__, __LINE__ );
	}
	/*------DEBUG LOG END------*/
	return check_result;
}

/*!
 *set_parameter
 *
 *@param [in] args
 *@retval
 */
protocol_module_sessionless::check_message_result protocol_module_sessionless::set_parameter(const std::vector<
        std::string>& args)
{
	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		boost::format formatter("int_function: protocol_module_sessionless::check_message_result "
			"protocol_module_sessionless::set_parameter("
			"const std::vector<std::string>& args ): args=%s");
		std::string argsdump;
		for (std::vector<std::string>::const_iterator it = args.begin(); it != args.end(); ++it)
		{
			argsdump += *it;
		}
		formatter % argsdump;
		putLogDebug(010010, formatter.str(), __FILE__, __LINE__ );
	}
	/*------DEBUG LOG END------*/
	//チェック結果フラグにTRUEを設定する
	check_message_result check_result;
	check_result.flag = true;
	bool forward_checked = false;
	bool sorryuri_checked = false;
	const std::string regex_match_str = "([^?#]*)(\\?([^#]*))?(#(.*))?";

	//オプション文字列の要素数分繰り返し
	typedef std::vector<std::string>::const_iterator vec_str_it;
	vec_str_it it = args.begin();
	vec_str_it it_end = args.end();

	//送信元設定指示に1を設定する
	forwarded_for = 1;

	try
	{
		for (; it != it_end; ++it)
		{
			//オプション文字列 = "-F"
			if (*it == "-F" || *it == "--forwarded-for")
			{
				//送信元設定指示設定フラグをON
				forward_checked = true;
			}
			//オプション文字列 = "-S"
			else if (*it == "-S" || *it == "--sorry-uri")
			{
				//sorryURI設定フラグ = OFF
				if (!sorryuri_checked)
				{
					//次要素が存在する場合
					if (++it != it_end)
					{
						if (!it->empty() && (it->substr(0, 1) == "-" || it->substr(0, 2) == "--"))
						{
							//チェック結果フラグにFALSEを設定する
							check_result.flag = false;
							//チェック結果メッセージを設定する
							check_result.message = "You have to set option value '-S/--sorry-uri'.";
							//繰り返し処理を終了する
							break;
						}
						//次要素の文字列長 ＞ 127
						if (it->size() > MAX_OPTION_SIZE - 1)
						{
							std::ostringstream ostr;
							ostr << "'-S/--sorry-uri\' option value '" << *it << "' is too long.";

							//チェック結果フラグにFALSEを設定する
							check_result.flag = false;
							//チェック結果メッセージを設定する
							check_result.message = ostr.str();
							//繰り返し処理を終了する
							break;
						}
						//次要素の文字列長 ≦ 127
						else
						{
							//正規表現でURI（httpからポートまでの部分を対象外）の妥当性をチェックする
							//正規表現は([^?#]*)(\?([^#]*))?(#(.*))?通り
							boost::regex uri(regex_match_str);
							boost::cmatch what;
							if (boost::regex_match(it->c_str(), what, uri))
							{
								//チェックOKの場合
								//sorryURI設定フラグをON
								sorryuri_checked = true;
							}
							//チェックNGの場合
							else
							{
								std::ostringstream ostr;
								ostr << "'-S/--sorry-uri' option value '" << *it << "' is not a valid URI.";

								//チェック結果フラグにFALSEを設定する
								check_result.flag = false;
								//チェック結果メッセージを設定する
								check_result.message = ostr.str();
								//繰り返し処理を終了する
								break;
							}
						}
					}
					//次要素が存在しない場合
					else
					{
						//チェック結果フラグにFALSEを設定する
						check_result.flag = false;
						//チェック結果メッセージを設定する
						check_result.message = "You have to set option value '-S/--sorry-uri'.";
						//繰り返し処理を終了する
						break;
					}
				}
				//sorryURI設定フラグ = ON
				else
				{
					//チェック結果フラグにFALSEを設定する
					check_result.flag = false;
					//チェック結果メッセージを設定する
					check_result.message = "Cannot set multiple option '-S/--sorry-uri\'.";
					//繰り返し処理を終了する
					break;
				}
			}
			//オプション文字列 = 上記以外
			else
			{
				//チェック結果フラグにFALSEを設定する
				check_result.flag = false;
				//チェック結果メッセージを設定する
				check_result.message = "Option error.";
				//繰り返し処理を終了する
				break;
			}
		}

		if (check_result.flag == true)
		{
			//送信元設定指示設定フラグ = OFF
			if (!forward_checked)
			{
				//送信元設定指示に0を設定する
				forwarded_for = 0;
			}
		}

	} catch (const std::exception& ex)
	{
		check_result.flag = false;
		std::cerr << "set parameter exception: result=false, error=" << ex.what() << "." << std::endl;
		boost::format formatter("function protocol_module_sessionless::check_message_result "
			"protocol_module_sessionless::set_parameter() exception: "
			"result=false, error=%s.");
		formatter % ex.what();
		putLogError(17000, formatter.str(), __FILE__, __LINE__ );
	} catch (...)
	{
		check_result.flag = false;
		std::cerr << "Unkown exception." << std::endl;
		putLogError(17000, "function protocol_module_sessionless::check_message_result "
			"protocol_module_sessionless::set_parameter() :"
			"Unkown exception.", __FILE__, __LINE__ );
	}

	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		boost::format formatter("out_function: protocol_module_sessionless::check_message_result "
			"protocol_module_sessionless::set_parameter("
			"const std::vector<std::string>& args ): return_value=("
			"check_message_result.flag=%d, check_message_result.message=%s");
		formatter % check_result.flag % check_result.message;
		putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
	}

	return check_result;
}

/*!
 *add_parameter
 *
 *@param [in] args
 *@retval
 */
protocol_module_sessionless::check_message_result protocol_module_sessionless::add_parameter(const std::vector<
        std::string>& args)
{
	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		boost::format formatter("in_function: protocol_module_sessionless::check_message_result "
			"protocol_module_sessionless::add_parameter("
			"const std::vector<std::string>& args ): args=%s");
		std::string argsdump;
		for (std::vector<std::string>::const_iterator it = args.begin(); it != args.end(); ++it)
		{
			argsdump += *it;
		}
		formatter % argsdump;
		putLogDebug(10010, formatter.str(), __FILE__, __LINE__ );
	}
	/*------DEBUG LOG END------*/
	check_message_result check_result;
	//チェック結果フラグにTRUEを設定する
	check_result.flag = true;

	//オプション文字列が存在する場合
	if (!args.empty())
	{
		//チェック結果フラグにFALSEを設定する
		check_result.flag = false;
		//チェック結果メッセージを設定する
		check_result.message = "Cannot add option.";
	}

	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		boost::format formatter("out_function: protocol_module_sessionless::add_parameter "
			"protocol_module_sessionless::set_parameter("
			"const std::vector<std::string>& args ): return_value=("
			"check_message_result.flag=%d, check_message_result.message=%s");
		formatter % check_result.flag % check_result.message;
		putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
	}

	return check_result;
}

/*!
 *handle_rslist_update
 *
 */
void protocol_module_sessionless::handle_rslist_update()
{
	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		putLogDebug(10005, "in/out_function: void protocol_module_sessionless::handle_rslist_update()", __FILE__,
		__LINE__ );
	}
	/*------DEBUG LOG END------*/
}

/*!
 *register_schedule
 *
 *@param [in] inschedule
 *@retval
 */
void protocol_module_sessionless::register_schedule(tcp_schedule_func_type inschedule)
{
	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		putLogDebug(10005, "in_function: void protocol_module_sessionless::register_schedule( "
			"tcp_schedule_func_type inschedule )", __FILE__, __LINE__ );
	}
	/*------DEBUG LOG END------*/
	schedule_tcp = inschedule;
	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		putLogDebug(10005, "out_function: void protocol_module_sessionless::register_schedule( "
			"tcp_schedule_func_type inschedule )", __FILE__, __LINE__ );
	}
	/*------DEBUG LOG END------*/
}

/*!
 *register_schedule
 *
 *@param [in] inschedule
 *@retval
 */
void protocol_module_sessionless::register_schedule(udp_schedule_func_type inschedule)
{
	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		putLogDebug(10002,
		        "in/out_function:void protocol_module_sessionless::register_schedule(udp_schedule_func_type inschedule): "
			        "return_value=void", __FILE__, __LINE__ );
	}
	/*------DEBUG LOG END------*/
}

/*!
 *handle_session_initialize
 *
 *@param [in] up_thread_id
 *@param [in] down_thread_id
 *@retval
 */
protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::handle_session_initialize(
        const boost::thread::id up_thread_id, const boost::thread::id down_thread_id,
        const boost::asio::ip::tcp::endpoint& client_endpoint_tcp,
        const boost::asio::ip::udp::endpoint& client_endpoint_udp)
{
	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		boost::format formatter("in_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_session_initialize( const boost::thread::id up_thread_id, "
			"const boost::thread::id down_thread_id , const boost::asio::ip::tcp::endpoint& client_endpoint_tcp,"
			"const boost::asio::ip::udp::endpoint& client_endpoint_udp): "
			"up_thread_id=%d, down_thread_id=%d");
		formatter % up_thread_id % down_thread_id;
		putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
	}
	/*------DEBUG LOG END------*/
	session_thread_data_sessionless* p_up = NULL;
	session_thread_data_sessionless* p_down = NULL;

	//セッションスレッドに対応する初期処理を行う
	try
	{
		p_up = new session_thread_data_sessionless;
		/*-------- DEBUG LOG --------*/
		if (LOG_LV_DEBUG == getloglevel())
		{
			boost::format formatter("new: address=&(%d), size=%lu");
			formatter % &p_up % sizeof(session_thread_data_sessionless);
			putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
		}
		/*------DEBUG LOG END------*/
		p_up->thread_id = up_thread_id;
		p_up->thread_division = THREAD_DIVISION_UP_STREAM;
		p_up->pair_thread_id = down_thread_id;
		p_up->accept_end_flag = ACCEPT_END_FLAG_OFF;
		p_up->end_flag = END_FLAG_OFF;
		p_up->sorry_flag = SORRY_FLAG_OFF;
		p_up->sorryserver_switch_flag = SORRYSERVER_SWITCH_FLAG_OFF;
		p_up->realserver_switch_flag = REALSERVER_SWITCH_FLAG_OFF;
		p_up->last_status = INITIALIZE;
		p_up->client_endpoint_tcp = client_endpoint_tcp;

		recive_data recv_data;
		recv_data.recive_buffer = NULL;
		recv_data.recive_buffer1 = NULL;
		recv_data.recive_buffer2 = NULL;
		recv_data.recive_buffer_max_size = 0;
		recv_data.recive_buffer_rest_size = 0;

		p_up->recive_data_map[client_endpoint_tcp] = recv_data;

		/*-------- DEBUG LOG --------*/
		if (LOG_LV_DEBUG == getloglevel())
		{
			// data dump
			boost::format
			        formatter("function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
				        "handle_session_initialize() : session_thread_data_sessionless(upthread) : "
				        "thread_id=%d, thread_division=%d, "
				        "pair_thread_id=%d, accept_end_flag=%d, end_flag=%d,"
				        "sorry_flag=%d, sorryserver_switch_flag=%d, realserver_switch_flag=%d, last_status=%d, client_endpoint_tcp=%s");
			formatter % p_up->thread_id % p_up->thread_division % p_up->pair_thread_id % p_up->accept_end_flag
			        % p_up->end_flag % p_up->sorry_flag % p_up->sorryserver_switch_flag % p_up->realserver_switch_flag
			        % p_up->last_status % client_endpoint_tcp.address().to_string();
			putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
		}
		/*------DEBUG LOG END------*/

		p_down = new session_thread_data_sessionless;
		/*-------- DEBUG LOG --------*/
		if (LOG_LV_DEBUG == getloglevel())
		{
			boost::format formatter("new: address=&(%d), size=%lu");
			formatter % &p_down % sizeof(session_thread_data_sessionless);
			putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
		}
		/*------DEBUG LOG END------*/
		p_down->thread_id = down_thread_id;
		p_down->thread_division = THREAD_DIVISION_DOWN_STREAM;
		p_down->pair_thread_id = up_thread_id;
		p_down->accept_end_flag = ACCEPT_END_FLAG_OFF;
		p_down->end_flag = END_FLAG_OFF;
		p_down->sorry_flag = SORRY_FLAG_OFF;
		p_down->sorryserver_switch_flag = SORRYSERVER_SWITCH_FLAG_OFF;
		p_down->realserver_switch_flag = REALSERVER_SWITCH_FLAG_OFF;
		p_down->last_status = INITIALIZE;
		p_down->client_endpoint_tcp = client_endpoint_tcp;
		/*-------- DEBUG LOG --------*/
		if (LOG_LV_DEBUG == getloglevel())
		{
			// data dump
			boost::format formatter("function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
				"handle_session_initialize() : session_thread_data_sessionless(down_thread_id) : "
				"accept_end_flag=%d, end_flag=%d, "
				"pair_thread_id=%d, realserver_switch_flag=%d, sorry_flag=%d,"
				"thread_division=%d, thread_id=%d, last_status=%d, client_endpoint_tcp=%s.");
			formatter % p_down->thread_id % p_down->thread_division % p_down->pair_thread_id % p_down->accept_end_flag
			        % p_down->end_flag % p_down->sorry_flag % p_down->sorryserver_switch_flag
			        % p_down->realserver_switch_flag % p_down->last_status % client_endpoint_tcp.address().to_string();
			putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
		}
		/*------DEBUG LOG END------*/

		session_thread_data_map[up_thread_id] = p_up;
		session_thread_data_map[down_thread_id] = p_down;
	} catch (const std::exception& ex)
	{
		std::cerr << "handle_session_initialize exception: error=%s" << ex.what() << "." << std::endl;
		boost::format formatter("function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_session_initialize() exception: result=%d, error=%s.");
		formatter % ex.what();
		putLogError(17000, formatter.str(), __FILE__, __LINE__ );
		return FINALIZE;
	} catch (...)
	{
		std::cerr << "Unkown exception." << std::endl;
		putLogError(17000, "function protocol_module_sessionless::check_message_result "
			"protocol_module_sessionless::handle_session_initialize() : "
			"Unkown exception.", __FILE__, __LINE__ );

		return FINALIZE;
	}
	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		boost::format formatter("out_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_session_initialize( const boost::thread::id up_thread_id, "
			"const boost::thread::id down_thread_id , const boost::asio::ip::tcp::endpoint& client_endpoint_tcp,"
			"const boost::asio::ip::udp::endpoint& client_endpoint_udp ): return_value=%d");
		formatter % ACCEPT;
		putLogDebug(030005, formatter.str(), __FILE__, __LINE__ );
	}
	/*------DEBUG LOG END------*/

	return ACCEPT;
}
/*!
 *handle_session_finalize
 *
 *@param [in] up_thread_id
 *@param [in] down_thread_id
 *@retval
 */
protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::handle_session_finalize(
        const boost::thread::id up_thread_id, const boost::thread::id down_thread_id)
{

	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		boost::format formatter("in_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_session_finalize( const boost::thread::id up_thread_id, "
			"const boost::thread::id down_thread_id ): "
			"up_thread_id=%d, down_thread_id=%d");
		formatter % up_thread_id % down_thread_id;
		putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
	}
	/*------DEBUG LOG END------*/
	session_thread_data_sessionless* p_up = NULL;
	session_thread_data_sessionless* p_down = NULL;
	boost::asio::ip::tcp::endpoint endpoint;
	//セッションスレッドに対応する終了処理を行う
	try
	{
		boost::mutex::scoped_lock slock(session_thread_data_map_mutex);
		if (session_thread_data_map.find(up_thread_id) != session_thread_data_map.end())
		{
			p_up = session_thread_data_map[up_thread_id];
			if (p_up->recive_data_map.find(endpoint) != p_up->recive_data_map.end())
			{
				char*& recive_buffer = p_up->recive_data_map[endpoint].recive_buffer;
				if (recive_buffer != NULL)
				{
					if (recive_buffer == p_up->recive_data_map[endpoint].recive_buffer1)
					{
						if (p_up->recive_data_map[endpoint].recive_buffer2 != NULL)
						{
							/*-------- DEBUG LOG --------*/
							if (LOG_LV_DEBUG == getloglevel())
							{
								boost::format formatter("delete: address=&(%d)");
								formatter % p_up->recive_data_map[endpoint].recive_buffer2;
								putLogDebug(10005, formatter.str(), __FILE__,
								__LINE__ );
							}
							/*------DEBUG LOG END------*/
							delete[] p_up->recive_data_map[endpoint].recive_buffer2;
							p_up->recive_data_map[endpoint].recive_buffer2 = NULL;
						}
						p_up->recive_data_map[endpoint].recive_buffer1 = NULL;
					}
					else
					{
						if (p_up->recive_data_map[endpoint].recive_buffer1 != NULL)
						{
							/*-------- DEBUG LOG --------*/
							if (LOG_LV_DEBUG == getloglevel())
							{
								boost::format formatter("delete: address=&(%d)");
								formatter % p_up->recive_data_map[endpoint].recive_buffer1;
								putLogDebug(10005, formatter.str(), __FILE__,
								__LINE__ );
							}
							/*------DEBUG LOG END------*/
							delete[] p_up->recive_data_map[endpoint].recive_buffer1;
							p_up->recive_data_map[endpoint].recive_buffer1 = NULL;
						}
						p_up->recive_data_map[endpoint].recive_buffer2 = NULL;
					}
					/*-------- DEBUG LOG --------*/
					if (LOG_LV_DEBUG == getloglevel())
					{
						boost::format formatter("delete: address=&(%d)");
						formatter % recive_buffer;
						putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
					}
					/*------DEBUG LOG END------*/
					delete[] recive_buffer;
					recive_buffer = NULL;
				}
			}

			/*-------- DEBUG LOG --------*/
			if (LOG_LV_DEBUG == getloglevel())
			{
				boost::format formatter("delete: address=&(%d)");
				formatter % p_up;
				putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
			}
			/*------DEBUG LOG END------*/
			delete p_up;
			session_thread_data_map.erase(up_thread_id);
		}

		if (session_thread_data_map.find(down_thread_id) != session_thread_data_map.end())
		{
			p_down = session_thread_data_map[down_thread_id];
			if (p_down->recive_data_map.find(p_down->target_endpoint) != p_down->recive_data_map.end())
			{
				char*& recive_buffer = p_down->recive_data_map[p_down->target_endpoint].recive_buffer;
				if (recive_buffer != NULL)
				{
					if (recive_buffer == p_down->recive_data_map[p_down->target_endpoint].recive_buffer1)
					{
						if (p_down->recive_data_map[p_down->target_endpoint].recive_buffer2 != NULL)
						{
							/*-------- DEBUG LOG --------*/
							if (LOG_LV_DEBUG == getloglevel())
							{
								boost::format formatter("delete: address=&(%d)");
								formatter % p_down->recive_data_map[p_down->target_endpoint].recive_buffer2;
								putLogDebug(10005, formatter.str(), __FILE__,
								__LINE__ );
							}
							/*------DEBUG LOG END------*/
							delete[] p_down->recive_data_map[p_down->target_endpoint].recive_buffer2;
							p_down->recive_data_map[p_down->target_endpoint].recive_buffer2 = NULL;
						}
						p_down->recive_data_map[p_down->target_endpoint].recive_buffer1 = NULL;
					}
					else
					{
						if (p_down->recive_data_map[p_down->target_endpoint].recive_buffer1 != NULL)
						{
							/*-------- DEBUG LOG --------*/
							if (LOG_LV_DEBUG == getloglevel())
							{
								boost::format formatter("delete: address=&(%d)");
								formatter % p_down->recive_data_map[p_down->target_endpoint].recive_buffer1;
								putLogDebug(10005, formatter.str(), __FILE__,
								__LINE__ );
							}
							/*------DEBUG LOG END------*/
							delete[] p_down->recive_data_map[p_down->target_endpoint].recive_buffer1;
							p_down->recive_data_map[p_down->target_endpoint].recive_buffer1 = NULL;
						}
						p_down->recive_data_map[p_down->target_endpoint].recive_buffer2 = NULL;
					}
					/*-------- DEBUG LOG --------*/
					if (LOG_LV_DEBUG == getloglevel())
					{
						boost::format formatter("delete: address=&(%d)");
						formatter % recive_buffer;
						putLogDebug(10005, formatter.str(), __FILE__,
						__LINE__ );
					}
					/*------DEBUG LOG END------*/
					delete[] recive_buffer;
					recive_buffer = NULL;
				}
			}

			p_down = session_thread_data_map[down_thread_id];
			/*-------- DEBUG LOG --------*/
			if (LOG_LV_DEBUG == getloglevel())
			{
				boost::format formatter("delete: address=&(%d)");
				formatter % p_down;
				putLogDebug(10005, formatter.str(), __FILE__,
				__LINE__ );
			}
			/*------DEBUG LOG END------*/
			delete p_down;
			session_thread_data_map.erase(down_thread_id);
		}
	} catch (const std::exception& ex)
	{
		std::cerr << "handle_session_finalize exception: error=%s" << ex.what() << "." << std::endl;
		boost::format formatter("function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_session_finalize() exception: result=%d, error=%s.");
		formatter % STOP % ex.what();
		putLogError(17000, formatter.str(), __FILE__, __LINE__ );
	} catch (...)
	{
		std::cerr << "Unkown exception." << std::endl;
		boost::format formatter("function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_session_finalize() exception: result=%d.");
		formatter % STOP;
		putLogError(17000, formatter.str(), __FILE__, __LINE__ );
	}

	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		boost::format formatter("out_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_session_finalize( const boost::thread::id up_thread_id,"
			"const boost::thread::id down_thread_id ): return_value=%d");
		formatter % STOP;
		putLogDebug(030005, formatter.str(), __FILE__, __LINE__ );
	}
	/*------DEBUG LOG END------*/
	return STOP;
}

/*!
 *handle_accept
 *
 *@param [in] thread_id
 *@retval
 */
protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::handle_accept(const boost::thread::id thread_id)
{

	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		boost::format formatter("in_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_accept( const boost::thread::id thread_id ): thread_id=%d");
		formatter % thread_id;
		putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
	}
	/*------DEBUG LOG END------*/
	EVENT_TAG status;
	session_thread_data_sessionless* session_data = NULL;

	try
	{
		boost::mutex::scoped_lock slock(session_thread_data_map_mutex);
		if (session_thread_data_map.find(thread_id) == session_thread_data_map.end())
		{
			putLogError(17000, "Invalid thread id.", __FILE__, __LINE__ );
			return FINALIZE;
		}

		session_data = session_thread_data_map[thread_id];

		if (session_data == NULL)
		{
			putLogError(17000, "Invalid pointer.", __FILE__, __LINE__ );
			return FINALIZE;
		}

	} catch (const std::exception& ex)
	{
		std::cerr << "handle_accept exception: error=%s" << ex.what() << "." << std::endl;
		boost::format formatter("function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_accept() exception: result=%d, error=%s.");
		formatter % FINALIZE % ex.what();
		putLogError(17000, formatter.str(), __FILE__, __LINE__ );

		status = FINALIZE;
	} catch (...)
	{
		std::cerr << "Unkown exception." << std::endl;
		putLogError(17000, "function protocol_module_sessionless::check_message_result "
			"protocol_module_sessionless::handle_accept() : "
			"Unkown exception.", __FILE__, __LINE__ );
		status = FINALIZE;
	}

	//accept完了フラグをON
	session_data->accept_end_flag = ACCEPT_END_FLAG_ON;

	//sorry状態の場合
	if (session_data->sorry_flag == SORRY_FLAG_ON)
	{
		//遷移先ステータスを設定する
		status = SORRYSERVER_SELECT;
	}
	//sorry状態以外の場合
	else
	{
		//遷移先ステータスを設定する
		status = REALSERVER_SELECT;
	}

	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		boost::format formatter("out_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_accept( const boost::thread::id thread_id ): return_value=%d.");
		formatter % status;
		putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
	}
	/*------DEBUG LOG END------*/
	return status;
}

/*!
 *handle_client_recv
 *
 *@param [in] thread_id
 *@param [in] recvbuffer
 *@param [in] recvlen
 *@retval
 */
protocol_module_base::EVENT_TAG protocol_module_sessionless::handle_client_recv(const boost::thread::id thread_id,
        const boost::array<char, MAX_BUFFER_SIZE>& recvbuffer, const size_t recvlen)
{
	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		boost::format formatter("in_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_client_recv( const boost::thread::id thread_id,"
			"const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,"
			"const size_t recvlen ): thread_id=%d, recvbuffer=%s, recvlen=%d");
		formatter % thread_id % recvbuffer.data() % recvlen;
		putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
	}
	/*------DEBUG LOG END------*/

	EVENT_TAG status;
	typedef std::vector<protocol_module_sessionless::send_status_it> send_end_it_vec;
	send_end_it_vec send_end_vec;
	size_t data_remain_start = 0; //未送信データ開始位置
	size_t data_remain_size = 0; //未送信データサイズ
	size_t request_data_remain_size = 0; //リクエストデータ残サイズ
	bool bret = false;
	CHECK_RESULT_TAG check_result; //チェック結果
	size_t header_offset = 0;
	size_t header_offset_len = 0;
	size_t count_length_header_value = 0;
	size_t content_len_value = 0;
	std::string str_value;
	const std::string http_header = "";
	const std::string content_header = "Content-Length";
	boost::regex reg("Content-Length: +(\\d+)\r\n");
	boost::smatch what;
	session_thread_data_sessionless* session_data = NULL;

	char* buffer1 = NULL;
	char* buffer2 = NULL;
	//	size_t count = 0;

	//パラメータチェック
	if (recvlen > recvbuffer.size())
	{
		putLogError(17000, "Data size bigger than buffer size.", __FILE__,
		__LINE__ );
		/*-------- DEBUG LOG --------*/
		if (LOG_LV_DEBUG == getloglevel())
		{
			boost::format formatter(
			        "out_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
				        "handle_client_recv( const boost::thread::id thread_id,"
				        "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,"
				        "const size_t recvlen ): return_value=%d");
			formatter % FINALIZE;
			putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
		}
		/*------DEBUG LOG END------*/
		return FINALIZE;
	}

	try
	{
		{
			boost::mutex::scoped_lock slock(session_thread_data_map_mutex);

			if (session_thread_data_map.find(thread_id) == session_thread_data_map.end())
			{
				putLogError(17000, "Invalid thread id.", __FILE__, __LINE__ );
				/*-------- DEBUG LOG --------*/
				if (LOG_LV_DEBUG == getloglevel())
				{
					boost::format formatter(
					        "out_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
						        "handle_client_recv( const boost::thread::id thread_id,"
						        "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,"
						        "const size_t recvlen ): return_value=%d");
					formatter % FINALIZE;
					putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
				}
				/*------DEBUG LOG END------*/

				return FINALIZE;
			}

			session_data = session_thread_data_map[thread_id];
			if (session_data == NULL)
			{
				putLogError(17000, "Invalid thread id.", __FILE__, __LINE__ );
				/*-------- DEBUG LOG --------*/
				if (LOG_LV_DEBUG == getloglevel())
				{
					boost::format formatter(
					        "out_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
						        "handle_client_recv( const boost::thread::id thread_id,"
						        "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,"
						        "const size_t recvlen ): return_value=%d");
					formatter % FINALIZE;
					putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
				}
				/*------DEBUG LOG END------*/
				return FINALIZE;
			}
		}

		//終了フラグがONの場合
		if (session_data->end_flag == END_FLAG_ON)
		{
			/*-------- DEBUG LOG --------*/
			if (LOG_LV_DEBUG == getloglevel())
			{
				boost::format formatter(
				        "out_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
					        "handle_client_recv( const boost::thread::id thread_id,"
					        "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,"
					        "const size_t recvlen ): return_value=%d");
				formatter % CLIENT_RECV;
				putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
			}
			return CLIENT_RECV;
		}

		//終了フラグがOFFの場合
		if (session_data->recive_data_map.find(session_data->client_endpoint_tcp)
		        == session_data->recive_data_map.end())
		{
			putLogError(17000, "Invalid endpoint.", __FILE__, __LINE__ );
			/*-------- DEBUG LOG --------*/
			if (LOG_LV_DEBUG == getloglevel())
			{
				boost::format formatter(
				        "out_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
					        "handle_client_recv( const boost::thread::id thread_id,"
					        "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,"
					        "const size_t recvlen ): return_value=%d");
				formatter % FINALIZE;
				putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
			}
			/*------DEBUG LOG END------*/
			return FINALIZE;
		}

		recive_data& recv_data = session_data->recive_data_map[session_data->client_endpoint_tcp];

		send_status_it it = recv_data.send_status_list.begin();
		send_status_it it_end = recv_data.send_status_list.end();

		it = std::find_if(it, it_end, data_send_ok());
		if (it != it_end)
		{
			putLogError(17000, "send data is not correct.", __FILE__, __LINE__ );
			/*-------- DEBUG LOG --------*/
			if (LOG_LV_DEBUG == getloglevel())
			{
				boost::format formatter(
				        "out_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
					        "handle_client_recv( const boost::thread::id thread_id,"
					        "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,"
					        "const size_t recvlen ): return_value=%d");
				formatter % FINALIZE;
				putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
			}
			/*------DEBUG LOG END------*/
			return FINALIZE;
		}

		it = recv_data.send_status_list.begin();
		it = std::adjacent_find(it, it_end, data_send_repeated());
		if (it != it_end)
		{
			putLogError(17000, "send data is not correct.", __FILE__, __LINE__ );
			/*-------- DEBUG LOG --------*/
			if (LOG_LV_DEBUG == getloglevel())
			{
				boost::format formatter(
				        "out_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
					        "handle_client_recv( const boost::thread::id thread_id,"
					        "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,"
					        "const size_t recvlen ): return_value=%d");
				formatter % FINALIZE;
				putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
			}
			/*------DEBUG LOG END------*/
			return FINALIZE;
		}

		it = recv_data.send_status_list.begin();
		it_end = recv_data.send_status_list.end();

		//送信状態リストの要素数分繰り返し
		for (; it != it_end; ++it)
		{
			//送信状態が送信済の場合
			if (it->status == SEND_END)
			{
				//送信状態リストから削除する
				send_end_vec.push_back(it);
			}
			else if (it->status == SEND_CONTINUE) //送信状態が送信継続の場合
			{
				//送信データオフセットに送信済サイズを加算する
				it->send_offset += it->send_end_size;
				//未送信データ開始位置に送信データオフセットを設定する
				data_remain_start = it->send_offset;
				//繰り返し処理を終了する
				break;
			}
			else
			{ //送信状態が送信済以外の場合
				//未送信データ開始位置に送信データオフセットを設定する
				data_remain_start = it->send_offset;
				//未送信データサイズに未送信データサイズを設定する
				data_remain_size = it->unsend_size;
				//繰り返し処理を終了する
				break;
			}
		}

		for (send_end_it_vec::iterator it = send_end_vec.begin(); it != send_end_vec.end(); ++it)
		{
			//送信状態リストから削除する
			recv_data.send_status_list.erase(*it);
		}

		//データバッファ残サイズ　＜　リクエストデータサイズ
		if (recv_data.recive_buffer_rest_size < recvlen)
		{
			//データバッファMAXサイズ　＜　未送信データサイズ　＋　リクエストデータサイズ
			if (recv_data.recive_buffer_max_size < data_remain_size + recvlen)
			{
				if (recv_data.recive_buffer == NULL)
				{
					//データバッファ１をMAX_BUFFER_SIZE分、
					//新規に確保し、初期化する
					recv_data.recive_buffer1 = new char[MAX_BUFFER_SIZE];
					/*-------- DEBUG LOG --------*/
					if (LOG_LV_DEBUG == getloglevel())
					{
						boost::format formatter("new: address=&(%d), size=%lu");
						formatter % &recv_data.recive_buffer1 % (MAX_BUFFER_SIZE);
						putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
					}
					/*-------- DEBUG LOG --------*/
					memset(recv_data.recive_buffer1, 0, MAX_BUFFER_SIZE);
					//データバッファ２をMAX_BUFFER_SIZE分、
					//新規に確保し、初期化する
					recv_data.recive_buffer2 = new char[MAX_BUFFER_SIZE];
					/*-------- DEBUG LOG --------*/
					if (LOG_LV_DEBUG == getloglevel())
					{
						boost::format formatter("new: address=&(%d), size=%lu");
						formatter % &recv_data.recive_buffer2 % (MAX_BUFFER_SIZE);
						putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
					}
					/*-------- DEBUG LOG --------*/
					memset(recv_data.recive_buffer2, 0, MAX_BUFFER_SIZE);
					recv_data.recive_buffer = recv_data.recive_buffer1;
					//データバッファMAXサイズにMAX_BUFFER_SIZEを設定する
					recv_data.recive_buffer_max_size = MAX_BUFFER_SIZE;
				}
				else
				{
					//データバッファ１を「未送信データサイズ　＋　リクエストデータサイズ」分、
					//新規に確保し、初期化する
					buffer1 = new char[data_remain_size + recvlen];
					/*-------- DEBUG LOG --------*/
					if (LOG_LV_DEBUG == getloglevel())
					{
						boost::format formatter("new: address=&(%d), size=%lu");
						formatter % &buffer1 % (data_remain_size + recvlen);
						putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
					}
					/*-------- DEBUG LOG --------*/
					memset(buffer1, 0, data_remain_size + recvlen);
					//データバッファ２を「未送信データサイズ　＋　リクエストデータサイズ」分、
					//新規に確保し、初期化する
					buffer2 = new char[data_remain_size + recvlen];
					/*-------- DEBUG LOG --------*/
					if (LOG_LV_DEBUG == getloglevel())
					{
						boost::format formatter("new: address=&(%d), size=%lu");
						formatter % &buffer2 % (data_remain_size + recvlen);
						putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
					}
					/*-------- DEBUG LOG --------*/
					memset(buffer2, 0, data_remain_size + recvlen);

					/*-------- DEBUG LOG --------*/
					if (LOG_LV_DEBUG == getloglevel())
					{
						boost::format formatter("%02x");
						for (size_t i = 0; i < data_remain_size + recvlen; ++i)
						{
							formatter % static_cast<unsigned short> (static_cast<unsigned char> (buffer1[i]));
						}

						std::string datadump = formatter.str();
						formatter.parse(
						        "function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
							        "handle_client_recv( ): before memcpy (data dump):  "
							        "data begin(%d), data_size=%d,data=%s.");
						formatter % buffer1 % (data_remain_size + recvlen) % datadump;
						putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
					}
					/*------DEBUG LOG END------*/
					//データバッファ１に使用中データバッファの未送信データ開始位置から未送信データサイズ分設定する
					memcpy(buffer1, recv_data.recive_buffer + data_remain_start, data_remain_size);
					//データバッファ１にリクエストデータをサイズ分追加設定する
					memcpy(buffer1 + data_remain_size, recvbuffer.data(), recvlen);
					/*-------- DEBUG LOG --------*/
					if (LOG_LV_DEBUG == getloglevel())
					{
						boost::format formatter("%02x");
						for (size_t i = 0; i < data_remain_size + recvlen; ++i)
						{
							formatter % static_cast<unsigned short> (static_cast<unsigned char> (buffer2[i]));
						}

						std::string datadump = formatter.str();
						formatter.parse(
						        "function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
							        "handle_client_recv( ): after memcpy (data dump):  "
							        "data begin(%d), data_size=%d,data=%s.");
						formatter % buffer2 % (data_remain_size + recvlen) % datadump;
						putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
					}
					/*------DEBUG LOG END------*/
					//旧データバッファ１、旧データバッファ２を解放する
					if (recv_data.recive_buffer1 != NULL)
					{
						/*-------- DEBUG LOG --------*/
						if (LOG_LV_DEBUG == getloglevel())
						{
							boost::format formatter("delete: address=&(%d)");
							formatter % recv_data.recive_buffer1;
							putLogDebug(10005, formatter.str(), __FILE__,
							__LINE__ );
						}
						/*------DEBUG LOG END------*/
						delete[] recv_data.recive_buffer1;
						recv_data.recive_buffer1 = NULL;
					}

					if (recv_data.recive_buffer2 != NULL)
					{
						/*-------- DEBUG LOG --------*/
						if (LOG_LV_DEBUG == getloglevel())
						{
							boost::format formatter("delete: address=&(%d)");
							formatter % recv_data.recive_buffer2;
							putLogDebug(10005, formatter.str(), __FILE__,
							__LINE__ );
						}
						/*------DEBUG LOG END------*/
						delete[] recv_data.recive_buffer2;
						recv_data.recive_buffer2 = NULL;
					}

					//使用中データバッファにデータバッファ１を設定する
					recv_data.recive_buffer1 = buffer1;
					recv_data.recive_buffer2 = buffer2;
					recv_data.recive_buffer = recv_data.recive_buffer1;
					//データバッファMAXサイズに「未送信データサイズ　＋　リクエストデータサイズ」を設定する
					recv_data.recive_buffer_max_size = data_remain_size + recvlen;
				}
			}
			//データバッファMAXサイズ　≧　未送信データサイズ　＋　リクエストデータサイズ
			else
			{
				if (recv_data.recive_buffer1 == NULL || recv_data.recive_buffer2 == NULL)
				{
					putLogError(17000, "Invalid pointer.", __FILE__, __LINE__ );
					/*-------- DEBUG LOG --------*/
					if (LOG_LV_DEBUG == getloglevel())
					{
						boost::format formatter(
						        "out_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
							        "handle_client_recv( const boost::thread::id thread_id,"
							        "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,"
							        "const size_t recvlen ): return_value=%d");
						formatter % FINALIZE;
						putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
					}
					/*------DEBUG LOG END------*/
					return FINALIZE;
				}
				//使用中データバッファがデータバッファ１の場合
				if (recv_data.recive_buffer == recv_data.recive_buffer1)
				{
					//データバッファ２を初期化する
					memset(recv_data.recive_buffer2, 0, recv_data.recive_buffer_max_size);
					/*-------- DEBUG LOG --------*/
					if (LOG_LV_DEBUG == getloglevel())
					{
						boost::format formatter("%02x");
						for (size_t i = 0; i < recv_data.recive_buffer_max_size; ++i)
						{
							formatter
							        % static_cast<unsigned short> (static_cast<unsigned char> (recv_data.recive_buffer2[i]));
						}

						std::string datadump = formatter.str();
						formatter.parse(
						        "function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
							        "handle_client_recv( ): before memcpy (data dump):  "
							        "data begin(%d), data_size=%d,data=%s.");
						formatter % recv_data.recive_buffer2 % recv_data.recive_buffer_max_size % datadump;
						putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
					}
					/*------DEBUG LOG END------*/
					//データバッファ２に使用中データバッファの未送信データ開始位置から未送信データサイズ分設定する
					memcpy(recv_data.recive_buffer2, recv_data.recive_buffer + data_remain_start, data_remain_size);
					//データバッファ２にリクエストデータをサイズ分追加設定する
					memcpy(recv_data.recive_buffer2 + data_remain_size, recvbuffer.data(), recvlen);
					/*-------- DEBUG LOG --------*/
					if (LOG_LV_DEBUG == getloglevel())
					{
						boost::format formatter("%02x");
						for (size_t i = 0; i < recv_data.recive_buffer_max_size; ++i)
						{
							formatter
							        % static_cast<unsigned short> (static_cast<unsigned char> (recv_data.recive_buffer2[i]));
						}

						std::string datadump = formatter.str();
						formatter.parse(
						        "function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
							        "handle_client_recv( ): after memcpy (data dump):  "
							        "data begin(%d), data_size=%d,data=%s.");
						formatter % recv_data.recive_buffer2 % recv_data.recive_buffer_max_size % datadump;
						putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
					}
					/*------DEBUG LOG END------*/
					//使用中データバッファにデータバッファ２を設定する
					recv_data.recive_buffer = recv_data.recive_buffer2;
				}
				//使用中データバッファがデータバッファ２の場合
				else
				{
					//データバッファ１を初期化する
					memset(recv_data.recive_buffer1, 0, recv_data.recive_buffer_max_size);
					/*-------- DEBUG LOG --------*/
					if (LOG_LV_DEBUG == getloglevel())
					{
						boost::format formatter("%02x");
						for (size_t i = 0; i < recv_data.recive_buffer_max_size; ++i)
						{
							formatter
							        % static_cast<unsigned short> (static_cast<unsigned char> (recv_data.recive_buffer1[i]));
						}

						std::string datadump = formatter.str();
						formatter.parse(
						        "function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
							        "handle_client_recv( ): before memcpy (data dump):  "
							        "data begin(%d), data_size=%d,data=%s.");
						formatter % recv_data.recive_buffer1 % recv_data.recive_buffer_max_size % datadump;
						putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
					}
					/*------DEBUG LOG END------*/
					//データバッファ１に使用中データバッファの未送信データ開始位置から未送信データサイズ分設定する
					memcpy(recv_data.recive_buffer1, recv_data.recive_buffer + data_remain_start, data_remain_size);
					//データバッファ１にリクエストデータをサイズ分追加設定する
					memcpy(recv_data.recive_buffer1 + data_remain_size, recvbuffer.data(), recvlen);
					/*-------- DEBUG LOG --------*/
					if (LOG_LV_DEBUG == getloglevel())
					{
						boost::format formatter("%02x");
						for (size_t i = 0; i < recv_data.recive_buffer_max_size; ++i)
						{
							formatter
							        % static_cast<unsigned short> (static_cast<unsigned char> (recv_data.recive_buffer1[i]));
						}

						std::string datadump = formatter.str();
						formatter.parse(
						        "function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
							        "handle_client_recv( ): after memcpy (data dump):  "
							        "data begin(%d), data_size=%d,data=%s.");
						formatter % recv_data.recive_buffer1 % recv_data.recive_buffer_max_size % datadump;
						putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
					}
					/*------DEBUG LOG END------*/
					//使用中データバッファにデータバッファ１を設定する
					recv_data.recive_buffer = recv_data.recive_buffer1;
				}
			}

			//データバッファ残サイズに
			//「データバッファMAXサイズ　－　未送信データサイズ　－　リクエストデータサイズ」を設定する
			recv_data.recive_buffer_rest_size = recv_data.recive_buffer_max_size - data_remain_size - recvlen;

			//未送信データサイズに「未送信データサイズ　＋　リクエストデータサイズ」を設定する
			data_remain_size += recvlen;

			send_status_it it_begin = recv_data.send_status_list.begin();
			send_status_it it_end = recv_data.send_status_list.end();

			for (; it_begin != it_end; ++it_begin)
			{
				//送信データオフセットから未送信データ開始位置を減算
				it->send_offset -= data_remain_start;
			}
		}
		//データバッファ残サイズ　≧　リクエストデータサイズ
		else
		{
			if (recv_data.recive_buffer == NULL)
			{
				putLogError(17000, "Invalid thread id.", __FILE__, __LINE__ );
				/*-------- DEBUG LOG --------*/
				if (LOG_LV_DEBUG == getloglevel())
				{
					boost::format formatter(
					        "out_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
						        "handle_client_recv( const boost::thread::id thread_id,"
						        "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,"
						        "const size_t recvlen ): return_value=%d");
					formatter % FINALIZE;
					putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
				}
				/*------DEBUG LOG END------*/
				return FINALIZE;
			}
			/*-------- DEBUG LOG --------*/
			if (LOG_LV_DEBUG == getloglevel())
			{
				boost::format formatter("%02x");
				for (size_t i = 0; i < recv_data.recive_buffer_max_size; ++i)
				{
					formatter % static_cast<unsigned short> (static_cast<unsigned char> (recv_data.recive_buffer1[i]));
				}

				std::string datadump = formatter.str();
				formatter.parse("function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
					"handle_client_recv( ): before memcpy (data dump):  "
					"data begin(%d), data_size=%d,data=%s.");
				formatter % recv_data.recive_buffer1 % recv_data.recive_buffer_max_size % datadump;
				putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
			}
			/*------DEBUG LOG END------*/

			//使用中データバッファにリクエストデータをサイズ分追加設定する
			memcpy(recv_data.recive_buffer + recv_data.recive_buffer_max_size - recv_data.recive_buffer_rest_size,
			        recvbuffer.data(), recvlen);
			//データバッファ残サイズからリクエストデータサイズを減算する
			recv_data.recive_buffer_rest_size -= recvlen;
			//未送信データサイズに「未送信データサイズ　＋　リクエストデータサイズ」を設定する
			data_remain_size += recvlen;
		}

		it = recv_data.send_status_list.begin();
		it_end = recv_data.send_status_list.end();

		//リクエストデータ残サイズにリクエストデータサイズを設定する
		request_data_remain_size = recvlen;

		for (; it != it_end; ++it)
		{
			//送信状態が送信継続の場合
			if (it->status == SEND_CONTINUE)
			{
				//送信データ残サイズ　＞　リクエストデータ残サイズ
				if (it->send_rest_size > request_data_remain_size)
				{
					//送信可能データサイズにリクエストデータ残サイズを設定する
					it->send_possible_size = request_data_remain_size;
					//送信データ残サイズからリクエストデータ残サイズを減算する
					it->send_rest_size -= request_data_remain_size;
					//送信済データサイズに0を設定する
					it->send_end_size = 0;
					//リクエストデータ残サイズに0を設定する
					request_data_remain_size = 0;
				}
				//送信データ残サイズ　≦　リクエストデータ残サイズ
				else
				{
					//送信可能データサイズに送信データ残サイズを設定する
					it->send_possible_size = it->send_rest_size;
					//リクエストデータ残サイズから送信データ残サイズを減算する
					request_data_remain_size -= it->send_rest_size;
					//送信済データサイズに0を設定する
					it->send_end_size = 0;
					//送信データ残サイズに0を設定する
					it->send_rest_size = 0;
				}
				//編集区分に編集なしを設定する
				it->edit_division = EDIT_DIVISION_NO_EDIT;
				//送信状態に送信待を設定する
				it->status = SEND_OK;
			}
			//送信状態が送信不可の場合
			else if (it->status == SEND_NG)
			{
				//HTTPメソッドの妥当性をチェックする
				check_result = check_http_method(recv_data.recive_buffer + it->send_offset, data_remain_size);
				//チェック結果が正常の場合
				if (check_result == CHECK_OK)
				{
					//HTTPバージョンの妥当性をチェックする
					check_result = check_http_version(recv_data.recive_buffer + it->send_offset, data_remain_size);
				}
				//チェック結果が正常の場合
				if (check_result == CHECK_OK)
				{
					//送信可能データ内のHTTPヘッダの開始位置、サイズを取得する
					bret = find_http_header(recv_data.recive_buffer + it->send_offset, data_remain_size, http_header,
					        header_offset, header_offset_len);
					//検索結果が該当ありの場合
					if (bret)
					{
						//HTTPヘッダ（ContentLength）を検索する
						bret = find_http_header(recv_data.recive_buffer + it->send_offset, data_remain_size,
						        content_header, header_offset, count_length_header_value);
						//検索結果が該当ありの場合
						if (bret)
						{
							str_value.assign(recv_data.recive_buffer + it->send_offset + header_offset,
							        count_length_header_value);
							boost::regex_match(str_value, what, reg);
							str_value.assign(what[1].first, what[1].second);
							content_len_value = boost::lexical_cast<size_t>(str_value.c_str());
							//送信データ残サイズに
							//「HTTPヘッダサイズ　＋　ContentLength」を設定する
							it->send_rest_size = header_offset_len + content_len_value;
						}
						//検索結果が該当なしの場合
						else
						{
							//送信データ残サイズに未送信データサイズ + リクエストデータ残サイズを設定する
							it->send_rest_size = it->unsend_size + request_data_remain_size;
						}
						//編集区分に編集ありを設定する
						it->edit_division = EDIT_DIVISION_EDIT;
					}
					//検索結果が該当なしの場合
					else
					{
						//未送信データサイズにリクエストデータ残サイズを加算する
						it->unsend_size += request_data_remain_size;
						//リクエストデータ残サイズに0を設定する
						request_data_remain_size = 0;
						//繰り返し処理を終了する
						break;
					}
				}
				//チェック結果が異常の場合
				else if (check_result == CHECK_NG)
				{
					//編集区分に編集なしを設定する
					it->edit_division = EDIT_DIVISION_NO_EDIT;
					//送信データ残サイズに未送信データサイズ　＋リクエストデータ残サイズを設定する
					it->send_rest_size = it->unsend_size + request_data_remain_size;
				}
				//チェック結果が判定不能の場合
				//check_result == CHECK_INPOSSIBLE
				else
				{
					//未送信データサイズにリクエストデータ残サイズを加算する
					it->unsend_size += request_data_remain_size;
					//リクエストデータ残サイズに0を設定する
					request_data_remain_size = 0;
					//繰り返し処理を終了する
					break;
				}

				//送信データ残サイズ　＞　未送信データサイズ+リクエストデータ残サイズ
				if (it->send_rest_size > it->unsend_size + request_data_remain_size)
				{
					//送信可能データサイズに未送信データサイズ＋リクエストデータ残サイズを設定する
					it->send_possible_size = it->unsend_size + request_data_remain_size;
					//送信データ残サイズから未送信データサイズ　＋　リクエストデータ残サイズを減算する
					it->send_rest_size -= (it->unsend_size + request_data_remain_size);
					//送信済データサイズに0を設定する
					it->send_end_size = 0;
					//未送信データサイズに0を設定する
					it->unsend_size = 0;
					//リクエストデータ残サイズに0を設定する
					request_data_remain_size = 0;
				}
				//送信データ残サイズ　≦　未送信データサイズ　＋　リクエストデータ残サイズ
				else
				{
					//送信可能データサイズに送信データ残サイズを設定する
					it->send_possible_size = it->send_rest_size;
					//リクエストデータ残サイズに
					//「未送信データサイズ＋リクエストデータ残サイズ－送信データ残サイズ」を設定する
					request_data_remain_size = it->unsend_size + request_data_remain_size - it->send_rest_size;
					//送信済データサイズに0を設定する
					it->send_end_size = 0;
					//未送信データサイズに0を設定する
					it->unsend_size = 0;
					//送信データ残サイズに0を設定する
					it->send_rest_size = 0;
				}

				//送信状態に送信待を設定する
				it->status = SEND_OK;
			}
			//リクエストデータ残サイズ　≦　0
			if (request_data_remain_size <= 0)
			{
				break;
			}
		}

		//リクエストデータ残サイズ　＞　0
		while (request_data_remain_size > 0)
		{
			//送信状態を生成する
			send_status new_send_state;
			//送信状態に送信不可を設定する
			new_send_state.status = SEND_NG;
			//送信状態リストに送信状態を設定する
			recv_data.send_status_list.push_back(new_send_state);
			std::list<send_status>::reverse_iterator new_send_it = recv_data.send_status_list.rbegin();
			//送信データオフセットを設定する
			new_send_it->send_offset = recv_data.recive_buffer_max_size - recv_data.recive_buffer_rest_size
			        - request_data_remain_size;

			//HTTPメソッドの妥当性をチェックする
			check_result = check_http_method(recv_data.recive_buffer + new_send_it->send_offset,
			        request_data_remain_size);
			//チェック結果が正常の場合
			if (check_result == CHECK_OK)
			{
				//HTTPバージョンの妥当性をチェックする
				check_result = check_http_version(recv_data.recive_buffer + new_send_it->send_offset,
				        request_data_remain_size);
			}
			//チェック結果が正常の場合
			if (check_result == CHECK_OK)
			{
				//送信可能データ内のHTTPヘッダの開始位置、サイズを取得する
				bret = find_http_header(recv_data.recive_buffer + new_send_it->send_offset, request_data_remain_size,
				        http_header, header_offset, header_offset_len);
				//検索結果が該当ありの場合
				if (bret)
				{
					//HTTPヘッダ（ContentLength）を検索する
					bret = find_http_header(recv_data.recive_buffer + new_send_it->send_offset,
					        request_data_remain_size, content_header, header_offset, count_length_header_value);

					//検索結果が該当ありの場合
					if (bret)
					{
						str_value.assign(recv_data.recive_buffer + new_send_it->send_offset + header_offset,
						        count_length_header_value);
						boost::regex_match(str_value, what, reg);
						str_value.assign(what[1].first, what[1].second);
						content_len_value = boost::lexical_cast<size_t>(str_value.c_str());
						//送信データ残サイズに
						//「HTTPヘッダサイズ　＋　ContentLength」を設定する
						new_send_it->send_rest_size = header_offset_len + content_len_value;
					}
					//検索結果が該当なしの場合
					else
					{
						//送信データ残サイズにリクエストデータ残サイズを設定する
						new_send_it->send_rest_size = request_data_remain_size;
					}
					//編集区分に編集ありを設定する
					new_send_it->edit_division = EDIT_DIVISION_EDIT;
				}
				//検索結果が該当なしの場合
				else
				{
					//未送信データサイズにリクエストデータ残サイズを設定する
					new_send_it->unsend_size = request_data_remain_size;
					//リクエストデータ残サイズに0を設定する
					request_data_remain_size = 0;
					//繰り返し処理を終了する
					break;
				}
			}
			//チェック結果が異常の場合
			else if (check_result == CHECK_NG)
			{
				//編集区分に編集なしを設定する
				new_send_it->edit_division = EDIT_DIVISION_NO_EDIT;
				//送信データ残サイズにリクエストデータ残サイズを設定する
				new_send_it->send_rest_size = request_data_remain_size;
			}
			//チェック結果が判定不能の場合
			//check_result == CHECK_INPOSSIBLE
			else
			{
				//未送信データサイズにリクエストデータ残サイズを設定する
				new_send_it->unsend_size = request_data_remain_size;
				//リクエストデータ残サイズに0を設定する
				request_data_remain_size = 0;
				//繰り返し処理を終了する
				break;
			}

			//送信データ残サイズ　＞　リクエストデータ残サイズ
			if (new_send_it->send_rest_size > request_data_remain_size)
			{
				//送信可能データサイズにリクエストデータ残サイズを設定する
				new_send_it->send_possible_size = request_data_remain_size;
				//送信データ残サイズからリクエストデータ残サイズを減算する
				new_send_it->send_rest_size -= request_data_remain_size;
				//送信済データサイズに0を設定する
				new_send_it->send_end_size = 0;
				//リクエストデータ残サイズに0を設定する
				request_data_remain_size = 0;
			}
			//送信データ残サイズ　≦　リクエストデータ残サイズ
			else
			{
				//送信可能データサイズに送信データ残サイズを設定する
				new_send_it->send_possible_size = new_send_it->send_rest_size;
				//リクエストデータ残サイズから送信データ残サイズを減算する
				request_data_remain_size -= new_send_it->send_rest_size;
				//送信済データサイズに0を設定する
				new_send_it->send_end_size = 0;
				//送信データ残サイズに0を設定する
				new_send_it->send_rest_size = 0;
			}

			//送信状態に送信待を設定する
			new_send_it->status = SEND_OK;

		}

		//送信可能データありをチェック
		send_status_it it_find = find_if(recv_data.send_status_list.begin(), recv_data.send_status_list.end(),
		        data_send_possible());
		//送信可能データあり
		if (it_find != recv_data.send_status_list.end())
		{
			//sorry状態の場合
			if (session_data->sorry_flag == SORRY_FLAG_ON)
			{
				//遷移先ステータスを設定する
				status = SORRYSERVER_CONNECT;
			}
			//sorry状態以外の場合
			else
			{
				//遷移先ステータスを設定する
				status = REALSERVER_CONNECT;
			}
		}
		//送信可能データなし
		else
		{
			//遷移先ステータスを設定する
			status = CLIENT_RECV;
		}
	} catch (const std::bad_alloc&)
	{
		std::cerr << "handle_client_recv exception: Could not allocate memory." << std::endl;
		putLogError(17000, "Could not allocate memory.", __FILE__, __LINE__ );
		status = FINALIZE;
	} catch (const std::exception& ex)
	{
		std::cerr << "handle_client_recv exception: error=%s" << ex.what() << "." << std::endl;
		boost::format formatter("function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_client_recv() exception: result=%d, error=%s.");
		formatter % FINALIZE % ex.what();
		putLogError(17000, formatter.str(), __FILE__, __LINE__ );

		status = FINALIZE;
	} catch (...)
	{
		std::cerr << "Unkown exception." << std::endl;
		putLogError(17000, "function protocol_module_sessionless::check_message_result "
			"protocol_module_sessionless::handle_client_recv() : "
			"Unkown exception.", __FILE__, __LINE__ );
		status = FINALIZE;
	}

	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		boost::format formatter("out_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_client_recv( const boost::thread::id thread_id,"
			"const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,"
			"const size_t recvlen ): return_value=%d");
		formatter % status;
		putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
	}
	/*------DEBUG LOG END------*/

	return status;
}

/*!
 *handle_realserver_select
 *
 *@param [in] thread_id
 *@param [out] rs_endpoint
 */
protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::handle_realserver_select(
        const boost::thread::id thread_id, boost::asio::ip::tcp::endpoint & rs_endpoint)
{
	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		boost::format formatter("in_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_realserver_select( const boost::thread::id thread_id,"
			"boost::asio::ip::tcp::endpoint & rs_endpoint): thread_id=%d, rs_endpoint=%s");
		formatter % thread_id % rs_endpoint.address().to_string();
		putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
	}
	/*------DEBUG LOG END------*/
	EVENT_TAG status;
	boost::asio::ip::tcp::endpoint tmp_endpoint;
	session_thread_data_sessionless* session_data = NULL;
	session_thread_data_sessionless* session_down_thread_data = NULL;

	if (schedule_tcp.empty())
	{
		/*-------- DEBUG LOG --------*/
		if (LOG_LV_DEBUG == getloglevel())
		{
			boost::format formatter(
			        "out_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
				        "handle_realserver_select( const boost::thread::id thread_id,"
				        "boost::asio::ip::tcp::endpoint & rs_endpoint)"
				        ": return_value=%d");
			formatter % FINALIZE;
			putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
		}
		/*------DEBUG LOG END------*/
		return FINALIZE;
	}

	try
	{
		{
			boost::mutex::scoped_lock slock(session_thread_data_map_mutex);

			if (session_thread_data_map.find(thread_id) == session_thread_data_map.end())
			{
				putLogError(17000, "Invalid thread id.", __FILE__, __LINE__ );
				return FINALIZE;
			}

			session_data = session_thread_data_map[thread_id];
			if (session_data == NULL)
			{
				putLogError(17000, "Invalid pointer.", __FILE__, __LINE__ );
				return FINALIZE;
			}
		}

		//schedule_moduleにて振り分け先realserverを決定する
		schedule_tcp(thread_id, rs_list_begin, rs_list_end, rs_list_next, rs_endpoint);

		//endpoint = 決定
		if (rs_endpoint != tmp_endpoint)
		{
			recive_data recv_data;
			recv_data.recive_buffer = NULL;
			recv_data.recive_buffer1 = NULL;
			recv_data.recive_buffer2 = NULL;
			recv_data.recive_buffer_max_size = 0;
			recv_data.recive_buffer_rest_size = 0;
			std::map<boost::asio::ip::tcp::endpoint, recive_data> recive_map;
			recive_map[rs_endpoint] = recv_data;

			//endpointを設定し、realserver受信バッファを確保する
			//thread_id's memory is not allocated
			if (session_thread_data_map.find(session_data->pair_thread_id) == session_thread_data_map.end())
			{
				session_down_thread_data = new session_thread_data_sessionless;
				session_down_thread_data->thread_id = session_data->pair_thread_id;
				session_down_thread_data->thread_division = THREAD_DIVISION_DOWN_STREAM;
				session_down_thread_data->pair_thread_id = thread_id;
				session_down_thread_data->accept_end_flag = ACCEPT_END_FLAG_OFF;
				session_down_thread_data->end_flag = END_FLAG_OFF;
				session_down_thread_data->sorry_flag = SORRY_FLAG_OFF;
				session_down_thread_data->sorryserver_switch_flag = SORRYSERVER_SWITCH_FLAG_OFF;
				session_down_thread_data->realserver_switch_flag = REALSERVER_SWITCH_FLAG_OFF;
				session_down_thread_data->recive_data_map = recive_map;

				session_thread_data_map[session_data->pair_thread_id] = session_down_thread_data;
			}
			else
			{
				session_down_thread_data = session_thread_data_map[session_data->pair_thread_id];
				if (session_down_thread_data == NULL)
				{
					session_down_thread_data = new session_thread_data_sessionless;
					session_down_thread_data->thread_id = session_data->pair_thread_id;
					session_down_thread_data->thread_division = THREAD_DIVISION_DOWN_STREAM;
					session_down_thread_data->pair_thread_id = thread_id;
					session_down_thread_data->accept_end_flag = ACCEPT_END_FLAG_OFF;
					session_down_thread_data->end_flag = END_FLAG_OFF;
					session_down_thread_data->sorry_flag = SORRY_FLAG_OFF;
					session_down_thread_data->sorryserver_switch_flag = SORRYSERVER_SWITCH_FLAG_OFF;
					session_down_thread_data->realserver_switch_flag = REALSERVER_SWITCH_FLAG_OFF;
					session_down_thread_data->recive_data_map = recive_map;
				}
				else
				{
					//thread_id's memory is allocated, check endpoint's memory
					session_down_thread_data->thread_id = session_data->pair_thread_id;
					session_down_thread_data->thread_division = THREAD_DIVISION_DOWN_STREAM;
					session_down_thread_data->pair_thread_id = thread_id;
					session_down_thread_data->accept_end_flag = ACCEPT_END_FLAG_OFF;
					session_down_thread_data->end_flag = END_FLAG_OFF;
					session_down_thread_data->sorry_flag = SORRY_FLAG_OFF;
					session_down_thread_data->sorryserver_switch_flag = SORRYSERVER_SWITCH_FLAG_OFF;
					session_down_thread_data->realserver_switch_flag = REALSERVER_SWITCH_FLAG_OFF;

					if (session_down_thread_data->recive_data_map.find(rs_endpoint)
					        != session_down_thread_data->recive_data_map.end())
					{
						//
						recive_data & recv_data = session_down_thread_data->recive_data_map[rs_endpoint];
						delete[] recv_data.recive_buffer1;
						delete[] recv_data.recive_buffer2;
						recv_data.recive_buffer1 = NULL;
						recv_data.recive_buffer2 = NULL;
						recv_data.recive_buffer = NULL;
						recv_data.recive_buffer_max_size = 0;
						recv_data.recive_buffer_rest_size = 0;
						recv_data.send_status_list.clear();
					}
					else
					{
						session_down_thread_data->recive_data_map[rs_endpoint] = recv_data;
					}
				}
			}

			//遷移先ステータスを設定する
			status = CLIENT_RECV;

		}
		else
		{
			//終了フラグをONにする
			session_data->end_flag = END_FLAG_ON;
			status = CLIENT_DISCONNECT;
		}
	} catch (const std::exception& ex)
	{
		std::cerr << "handle_realserver_select exception: error=%s" << ex.what() << "." << std::endl;
		boost::format formatter("function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_realserver_select() exception: result=%d, error=%s.");
		formatter % FINALIZE % ex.what();
		putLogError(17000, formatter.str(), __FILE__, __LINE__ );
		status = FINALIZE;
	} catch (...)
	{
		std::cerr << "Unkown exception." << std::endl;
		putLogError(17000, "function protocol_module_sessionless::check_message_result "
			"protocol_module_sessionless::handle_realserver_select() : "
			"Unkown exception.", __FILE__, __LINE__ );
		status = FINALIZE;
	}

	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		boost::format formatter("out_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_realserver_select( const boost::thread::id thread_id,"
			"boost::asio::ip::tcp::endpoint & rs_endpoint)"
			": return_value=%d");
		formatter % status;
		putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
	}
	/*------DEBUG LOG END------*/

	return status;
}

/*!
 *handle_realserver_select
 *
 *@param [in] thread_id
 *@param [in] sendbuffer
 *@param [out] datalen
 */
protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::handle_realserver_select(
        const boost::thread::id thread_id, boost::asio::ip::udp::endpoint& rs_endpoint, boost::array<char,
                MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen)
{
	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		boost::format formatter("in/out_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_realserver_select( const boost::thread::id thread_id,"
			"boost::asio::ip::udp::endpoint& rs_endpoint,"
			"size_t& datalen): "
			"return_value=%d.");
		formatter % STOP;
		putLogDebug(10000, formatter.str(), __FILE__, __LINE__ );
	}
	/*------DEBUG LOG END------*/
	return STOP;
}
/*!
 *handle_realserver_connect
 *
 *@param [in] thread_id
 *@param [in] sendbuffer
 *@param [out] datalen
 */
protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::handle_realserver_connect(
        const boost::thread::id thread_id, boost::array<char, MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen)
{
	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		boost::format formatter("in_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_realserver_connect( const boost::thread::id up_thread_id, "
			"boost::array<char, MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen ): "
			"thread_id=%d, sendbuffer=%d datalen=%d");
		formatter % thread_id % sendbuffer.data() % datalen;
		putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
	}
	/*------DEBUG LOG END------*/

	EVENT_TAG status;

	bool ret = false;
	size_t header_offset = 0;
	size_t header_offset_len = 0;
	const std::string http_header = "";
	const std::string str_forword_for = "X-Forwarded-For";
	size_t send_buffer_remian_size = 0;
	const int send_buffer_end_size = sendbuffer.max_size();
	session_thread_data_sessionless* session_data = NULL;
	size_t copy_size = 0;

	try
	{
		{
			boost::mutex::scoped_lock slock(session_thread_data_map_mutex);
			if (session_thread_data_map.find(thread_id) == session_thread_data_map.end())
			{
				putLogError(17000, "Invalid thread id.", __FILE__, __LINE__ );
				return FINALIZE;
			}

			session_data = session_thread_data_map[thread_id];

			if (session_data == NULL)
			{
				putLogError(17000, "Invalid pointer.", __FILE__, __LINE__ );
				return FINALIZE;
			}
		}

		//
		if (session_data->recive_data_map.find(session_data->target_endpoint) == session_data->recive_data_map.end())
		{
			putLogError(17000, "Invalid endpoint.", __FILE__, __LINE__ );
			return FINALIZE;
		}

		recive_data& recv_data = session_data->recive_data_map[session_data->target_endpoint];

		if (recv_data.recive_buffer == NULL)
		{
			putLogError(17000, "Invalid recive_buffer.", __FILE__, __LINE__ );
			return FINALIZE;
		}

		//送信可能データありをチェック
		send_status_it it = recv_data.send_status_list.begin();
		send_status_it it_end = recv_data.send_status_list.end();
		it = find_if(it, it_end, data_send_possible());
		if (it == it_end)
		{
			putLogError(17000, "sending possible data is not existed .", __FILE__, __LINE__ );
			return FINALIZE;
		}

		//送信バッファ残サイズに送信バッファサイズを設定する
		send_buffer_remian_size = send_buffer_end_size;

		//編集区分 = 編集あり
		if (it->edit_division == EDIT_DIVISION_EDIT)
		{
			//編集データリストが空の場合
			if (it->edit_data_list.empty())
			{
				//編集データ情報を生成する
				edit_data edata;
				//送信可能データからHTTPヘッダ（X-Forwarded-For）を検索する
				ret = find_http_header(recv_data.recive_buffer + it->send_offset, it->send_possible_size,
				        str_forword_for, header_offset, header_offset_len);
				//検索結果 = 該当あり
				if (ret)
				{
					//編集データ情報の編集データにX-Forwarded-Forを編集して設定する
					edata.data.assign(recv_data.recive_buffer + it->send_offset + header_offset, header_offset_len);
					edata.data += ", ";
					edata.data += session_data->client_endpoint_tcp.address().to_string();
					//編集データ情報の編集データ設定位置にX-Forwarded-Forの開始位置を設定する
					edata.insert_posission = header_offset;
					//編集データ情報の編集データサイズに編集後のX-Forwarded-Forのサイズを設定する
					edata.data_size = edata.data.size();
					//編集データ情報の上書きサイズに編集前のX-Forwarded-Forのサイズを設定する
					edata.replace_size = header_offset_len;
				}
				//検索結果 = 該当なし
				else
				{
					//送信可能データ内のHTTPヘッダの開始位置、サイズを取得する
					find_http_header(recv_data.recive_buffer + it->send_offset, it->send_possible_size, "",
					        header_offset, header_offset_len);
					//編集データ情報の編集データにX-Forwarded-Forを編集して設定する
					edata.data = str_forword_for;
					edata.data += ": ";
					edata.data += session_data->client_endpoint_tcp.address().to_string();
					edata.data += "\r\n";
					//編集データ情報の編集データ設定位置にHTTPヘッダの開始位置を設定する
					edata.insert_posission = header_offset;
					//編集データ情報の編集データサイズに編集後のX-Forwarded-Forのサイズを設定する
					edata.data_size = edata.data.size();
					//編集データ情報の上書きサイズに0を設定する
					edata.replace_size = 0;
				}

				//編集データリストに編集データ情報を設定する
				it->edit_data_list.push_back(edata);
			}

			//
			while (true)
			{
				//編集データリストが空の場合
				if (it->edit_data_list.empty())
				{
					//編集区分に編集なしを設定する
					it->edit_division = EDIT_DIVISION_NO_EDIT;
					//送信バッファ残サイズ　＞　0　かつ　送信可能データサイズ　＞　0
					if (send_buffer_remian_size > 0 && it->send_possible_size > 0)
					{
						//送信バッファ残サイズ　≧　送信可能データサイズ
						if (send_buffer_remian_size > it->send_possible_size)
						{
							//送信可能データを送信データオフセット＋送信済サイズの位置から
							//送信可能データサイズ分、送信バッファにコピーする
							copy_size = it->send_possible_size;
							memcpy(sendbuffer.data() + send_buffer_end_size - send_buffer_remian_size,
							        recv_data.recive_buffer + it->send_offset + it->send_end_size,
							        it->send_possible_size);
							//送信済サイズにコピーしたサイズを加算する
							it->send_end_size += copy_size;
							//送信可能データサイズからコピーしたサイズを減算する
							it->send_possible_size = 0;
							//送信バッファ残サイズからコピーしたサイズを減算する
							send_buffer_remian_size -= copy_size;
						}
						//送信バッファ残サイズ　＜　送信可能データサイズ
						else
						{
							//送信可能データを送信データオフセット＋送信済サイズの位置から
							//送信バッファ残サイズ分、送信バッファにコピーする
							copy_size = send_buffer_remian_size;
							memcpy(sendbuffer.data() + send_buffer_end_size - send_buffer_remian_size,
							        recv_data.recive_buffer + it->send_offset + it->send_end_size, copy_size);
							//送信済サイズにコピーしたサイズを加算する
							it->send_end_size += copy_size;
							//送信可能データサイズからコピーしたサイズを減算する
							it->send_possible_size -= copy_size;
							//送信バッファ残サイズからコピーしたサイズを減算する
							send_buffer_remian_size = 0;
						}
					}
					break;
				}
				//編集データリストが空でない場合
				else
				{
					//編集データリストから最小の編集データ設定位置を持つ要素を検索する
					std::list<edit_data>::iterator edit_min = std::min_element(it->edit_data_list.begin(),
					        it->edit_data_list.end());
					//送信バッファ残サイズ　≧　編集データ設定位置　－　送信済サイズ
					if (send_buffer_remian_size >= edit_min->insert_posission - it->send_end_size)
					{
						//送信可能データを送信データオフセット＋送信済サイズの位置から
						//編集データ設定位置－送信済サイズ分、送信バッファにコピーする
						copy_size = edit_min->insert_posission - it->send_end_size;
						memcpy(sendbuffer.data() + send_buffer_end_size - send_buffer_remian_size,
						        recv_data.recive_buffer + it->send_offset + it->send_end_size, copy_size);
						//送信済サイズにコピーしたサイズを加算する
						it->send_end_size += copy_size;
						//送信可能データサイズからコピーしたサイズを減算する
						it->send_possible_size -= copy_size;
						//送信バッファ残サイズからコピーしたサイズを減算する
						send_buffer_remian_size -= copy_size;

						//送信バッファ残サイズ　≧　編集データサイズ
						if (send_buffer_remian_size >= edit_min->data_size)
						{
							//編集データを編集データサイズ分、送信バッファにコピーする
							memcpy(sendbuffer.data() + send_buffer_end_size - send_buffer_remian_size,
							        edit_min->data.c_str(), edit_min->data_size);
							//送信済サイズに上書きサイズを加算する
							it->send_end_size += edit_min->replace_size;
							//送信可能データサイズから上書きサイズを減算する
							it->send_possible_size -= edit_min->replace_size;
							//送信バッファ残サイズから編集データサイズを減算する
							send_buffer_remian_size -= edit_min->data_size;
							//編集データリストから該当要素を削除する
							it->edit_data_list.erase(edit_min);
						}
						//送信バッファ残サイズ　＜　編集データサイズ
						else
						{
							break;
						}
					}
					//送信バッファ残サイズ　＜　編集データ設定位置　－　送信済サイズ
					else
					{
						//送信可能データを送信データオフセット＋送信済サイズの位置から
						//送信バッファ残サイズ分、送信バッファにコピーする
						copy_size = send_buffer_remian_size;
						memcpy(sendbuffer.data() + send_buffer_end_size - send_buffer_remian_size,
						        recv_data.recive_buffer + it->send_offset + it->send_end_size, copy_size);

						//送信済サイズにコピーしたサイズを加算する
						it->send_end_size += copy_size;
						//送信可能データサイズからコピーしたサイズを減算する
						it->send_possible_size -= copy_size;
						//送信バッファ残サイズからコピーしたサイズを減算する
						send_buffer_remian_size -= copy_size;
						//繰り返し処理を終了する
						break;
					}
				}
			}
		}
		//編集区分 = 編集なし
		else
		{
			//送信バッファサイズ　≧　送信可能データサイズ
			if (send_buffer_remian_size >= it->send_possible_size)
			{
				copy_size = it->send_possible_size;
				//送信可能データを先頭から送信可能データサイズ分、送信バッファにコピーする
				memcpy(sendbuffer.data(), recv_data.recive_buffer + it->send_offset, copy_size);
				//送信済データサイズに送信可能データサイズを設定する
				it->send_end_size = copy_size;
				//送信可能データサイズに0を設定する
				it->send_possible_size = 0;
				//
				send_buffer_remian_size -= copy_size;
			}
			//送信バッファサイズ　＜　送信可能データサイズ
			else
			{
				//送信可能データを先頭から送信バッファサイズ分、送信バッファにコピーする
				memcpy(sendbuffer.data(), recv_data.recive_buffer + it->send_offset, send_buffer_remian_size);
				//送信済データサイズに送信バッファサイズを設定する
				it->send_end_size = send_buffer_remian_size;
				//送信可能データサイズから送信バッファサイズを減算する
				it->send_possible_size -= send_buffer_remian_size;
				//
				send_buffer_remian_size = 0;
			}
		}

		datalen = send_buffer_end_size - send_buffer_remian_size;
		status = REALSERVER_SEND;

	} catch (const std::exception& ex)
	{
		std::cerr << "handle_realserver_connect exception: error=%s" << ex.what() << "." << std::endl;
		boost::format formatter("function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_realserver_connect() exception: result=%d, error=%s.");
		formatter % FINALIZE % ex.what();
		putLogError(17000, formatter.str(), __FILE__, __LINE__ );
		status = FINALIZE;
	} catch (...)
	{
		std::cerr << "Unkown exception." << std::endl;
		boost::format formatter("function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_realserver_connect() exception: result=%d.");
		formatter % FINALIZE;
		putLogError(17000, formatter.str(), __FILE__, __LINE__ );
		status = FINALIZE;
	}

	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		boost::format formatter("out_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_realserver_connect( const boost::thread::id thread_id,"
			"const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,"
			"const size_t recvlen ): return_value=%d");
		formatter % status;
		putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
	}
	/*------DEBUG LOG END------*/

	return status;
}

/*!
 *handle_realserver_connection_fail
 *
 *@param [in] thread_id
 *@param [in] rs_endpoint
 */
protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::handle_realserver_connection_fail(
        const boost::thread::id thread_id, const boost::asio::ip::tcp::endpoint & rs_endpoint)
{
	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		boost::format formatter("in_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_realserver_connection_fail( const boost::thread::id thread_id, "
			"const boost::asio::ip::tcp::endpoint & rs_endpoint): "
			"thread_id=%d, rs_endpoint=%s ");
		formatter % thread_id % rs_endpoint.address().to_string();
		putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
	}
	/*------DEBUG LOG END------*/

	EVENT_TAG status;
	session_thread_data_sessionless* session_data = NULL;

	try
	{
		boost::mutex::scoped_lock slock(session_thread_data_map_mutex);
		if (session_thread_data_map.find(thread_id) == session_thread_data_map.end())
		{
			putLogError(17000, "Invalid thread id.", __FILE__, __LINE__ );
			return FINALIZE;
		}

		session_data = session_thread_data_map[thread_id];
		if (session_data == NULL)
		{
			putLogError(17000, "Invalid pointer.", __FILE__, __LINE__ );
			return FINALIZE;
		}

		//終了フラグをON
		session_data->end_flag = END_FLAG_ON;
		//遷移先ステータスを設定する
		status = CLIENT_DISCONNECT;
	} catch (std::exception& ex)
	{
		std::cerr << "handle_realserver_connection_fail exception: error=%s" << ex.what() << "." << std::endl;
		boost::format formatter("function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_realserver_connection_fail() exception: result=%d, error=%s.");
		formatter % FINALIZE % ex.what();
		putLogError(17000, formatter.str(), __FILE__, __LINE__ );
		status = FINALIZE;
	} catch (...)
	{
		std::cerr << "Unkown exception." << std::endl;
		boost::format formatter("function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_realserver_connection_fail() exception: result=%d.");
		formatter % FINALIZE;
		putLogError(17000, formatter.str(), __FILE__, __LINE__ );
		status = FINALIZE;
	}

	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		boost::format formatter("out_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_realserver_connection_fail( const boost::thread::id thread_id,"
			"const boost::asio::ip::tcp::endpoint & rs_endpoint ): return_value=%d");
		formatter % status;
		putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
	}
	/*------DEBUG LOG END------*/
	return status;
}
/*!
 *handle_realserver_send
 *
 *@param [in] thread_id
 */
protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::handle_realserver_send(
        const boost::thread::id thread_id)
{
	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		boost::format formatter("in_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_realserver_send( const boost::thread::id thread_id): "
			"thread_id=%d ");
		formatter % thread_id;
		putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
	}
	/*------DEBUG LOG END------*/
	EVENT_TAG status;
	session_thread_data_sessionless* session_data = NULL;

	try
	{
		{
			boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
			if (session_thread_data_map.find(thread_id) == session_thread_data_map.end())
			{
				putLogError(17000, "Invalid thread id.", __FILE__, __LINE__ );
				return FINALIZE;
			}

			session_data = session_thread_data_map[thread_id];
			if (session_data == NULL)
			{
				putLogError(17000, "Invalid pointer.", __FILE__, __LINE__ );
				return FINALIZE;
			}
		}

		if (session_data->recive_data_map.find(session_data->client_endpoint_tcp)
		        == session_data->recive_data_map.end())
		{
			putLogError(17000, "Invalid endpoint.", __FILE__, __LINE__ );
			return FINALIZE;
		}

		recive_data& recv_data = session_data->recive_data_map[session_data->client_endpoint_tcp];

		send_status_it it = recv_data.send_status_list.begin();
		send_status_it it_end = recv_data.send_status_list.end();

		it = find_if(it, it_end, data_send_ok());
		if (it == it_end)
		{
			putLogError(17000, "sending possible data is not existed.", __FILE__, __LINE__ );
			return FINALIZE;
		}

		//送信可能データサイズ　＞　0
		if (it->send_possible_size > 0)
		{
			//送信状態に送信待を設定する
			it->status = SEND_OK;
			//送信データオフセットに送信済データサイズを加算する
			it->send_offset += it->send_end_size;

			//編集データリスト要素数分
			for (std::list<edit_data>::iterator list_it = it->edit_data_list.begin(); list_it
			        != it->edit_data_list.end(); ++list_it)
			{
				//編集データ設定位置から送信済データサイズを減算する。
				list_it->insert_posission -= it->send_end_size;
			}

			//送信済データサイズに0を設定する
			it->send_end_size = 0;
		}
		//送信可能データサイズ　≦　0
		else
		{
			//送信データ残サイズ　＞　0
			if (it->send_rest_size > 0)
			{
				//送信状態に送信継続を設定する
				it->status = SEND_CONTINUE;
			}
			//送信データ残サイズ　≦　0
			else
			{
				//送信状態に送信済を設定する
				it->status = SEND_END;
			}
		}

		it = recv_data.send_status_list.begin();
		it_end = recv_data.send_status_list.end();

		it = find_if(it, it_end, data_send_possible());
		if (it != it_end)
		{
			//送信可能データあり
			//遷移先ステータスを設定する
			status = REALSERVER_CONNECT;
		}
		else
		{
			//送信可能データなし
			//遷移先ステータスを設定する
			status = CLIENT_RECV;
		}

	} catch (const std::exception& ex)
	{
		std::cerr << "handle_realserver_send exception: error=%s" << ex.what() << "." << std::endl;
		boost::format formatter("function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_realserver_send() exception: result=%d, error=%s.");
		formatter % FINALIZE % ex.what();
		putLogError(17000, formatter.str(), __FILE__, __LINE__ );

		status = FINALIZE;
	} catch (...)
	{
		std::cerr << "Unkown exception." << std::endl;
		boost::format formatter("function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_realserver_send() exception: result=%d.");
		formatter % FINALIZE;
		putLogError(17000, formatter.str(), __FILE__, __LINE__ );
		status = FINALIZE;
	}

	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		boost::format formatter("out_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_realserver_send( const boost::thread::id thread_id): return_value=%d");
		formatter % status;
		putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
	}
	/*------DEBUG LOG END------*/

	return status;
}

/*!
 *handle_sorryserver_select
 *
 *@param [in] thread_id
 *@param [in]sorry_endpoint
 */
protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::handle_sorryserver_select(
        const boost::thread::id thread_id, boost::asio::ip::tcp::endpoint & sorry_endpoint)
{
	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		boost::format formatter("in_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_sorryserver_select( const boost::thread::id thread_id, "
			"boost::asio::ip::tcp::endpoint & sorry_endpoint): "
			"thread_id=%d, sorry_endpoint=%s ");
		formatter % thread_id % sorry_endpoint.address().to_string();
		putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
	}
	/*------DEBUG LOG END------*/
	EVENT_TAG status;
	boost::asio::ip::tcp::endpoint client_endpoint;

	session_thread_data_sessionless* session_data = NULL;
	session_thread_data_sessionless* session_down_thread_data = NULL;

	try
	{
		boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
		if (session_thread_data_map.find(thread_id) == session_thread_data_map.end())
		{
			putLogError(17000, "Invalid thread id.", __FILE__, __LINE__ );
			return FINALIZE;
		}

		session_data = session_thread_data_map[thread_id];
		if (session_data == NULL)
		{
			putLogError(17000, "Invalid pointer.", __FILE__, __LINE__ );
			return FINALIZE;
		}

		recive_data recv_data;
		recv_data.recive_buffer = NULL;
		recv_data.recive_buffer1 = NULL;
		recv_data.recive_buffer2 = NULL;
		recv_data.recive_buffer_max_size = 0;
		recv_data.recive_buffer_rest_size = 0;
		std::map<boost::asio::ip::tcp::endpoint, recive_data> recive_map;
		recive_map[sorry_endpoint] = recv_data;
		//下りスレッドにsorryserver受信バッファを確保する
		if (session_thread_data_map.find(session_data->pair_thread_id) == session_thread_data_map.end())
		{
			session_down_thread_data = new session_thread_data_sessionless;
			session_down_thread_data->thread_id = session_data->pair_thread_id;
			session_down_thread_data->thread_division = THREAD_DIVISION_DOWN_STREAM;
			session_down_thread_data->pair_thread_id = thread_id;
			session_down_thread_data->accept_end_flag = ACCEPT_END_FLAG_OFF;
			session_down_thread_data->end_flag = END_FLAG_OFF;
			session_down_thread_data->sorry_flag = SORRY_FLAG_OFF;
			session_down_thread_data->sorryserver_switch_flag = SORRYSERVER_SWITCH_FLAG_OFF;
			session_down_thread_data->realserver_switch_flag = REALSERVER_SWITCH_FLAG_OFF;
			session_down_thread_data->recive_data_map = recive_map;

			session_thread_data_map[session_data->pair_thread_id] = session_down_thread_data;
		}
		else
		{
			session_down_thread_data = session_thread_data_map[session_data->pair_thread_id];
			if (session_down_thread_data == NULL)
			{
				session_down_thread_data = new session_thread_data_sessionless;
				session_down_thread_data->thread_id = session_data->pair_thread_id;
				session_down_thread_data->thread_division = THREAD_DIVISION_DOWN_STREAM;
				session_down_thread_data->pair_thread_id = thread_id;
				session_down_thread_data->accept_end_flag = ACCEPT_END_FLAG_OFF;
				session_down_thread_data->end_flag = END_FLAG_OFF;
				session_down_thread_data->sorry_flag = SORRY_FLAG_OFF;
				session_down_thread_data->sorryserver_switch_flag = SORRYSERVER_SWITCH_FLAG_OFF;
				session_down_thread_data->realserver_switch_flag = REALSERVER_SWITCH_FLAG_OFF;
				session_down_thread_data->recive_data_map = recive_map;
			}
			else
			{
				//thread_id's memory is allocated, check endpoint's memory
				session_down_thread_data->thread_id = session_data->pair_thread_id;
				session_down_thread_data->thread_division = THREAD_DIVISION_DOWN_STREAM;
				session_down_thread_data->pair_thread_id = thread_id;
				session_down_thread_data->accept_end_flag = ACCEPT_END_FLAG_OFF;
				session_down_thread_data->end_flag = END_FLAG_OFF;
				session_down_thread_data->sorry_flag = SORRY_FLAG_OFF;
				session_down_thread_data->sorryserver_switch_flag = SORRYSERVER_SWITCH_FLAG_OFF;
				session_down_thread_data->realserver_switch_flag = REALSERVER_SWITCH_FLAG_OFF;

				if (session_down_thread_data->recive_data_map.find(sorry_endpoint)
				        != session_down_thread_data->recive_data_map.end())
				{
					//
					recive_data & recv_data = session_down_thread_data->recive_data_map[sorry_endpoint];
					delete[] recv_data.recive_buffer1;
					delete[] recv_data.recive_buffer2;
					recv_data.recive_buffer1 = NULL;
					recv_data.recive_buffer2 = NULL;
					recv_data.recive_buffer = NULL;
					recv_data.recive_buffer_max_size = 0;
					recv_data.recive_buffer_rest_size = 0;
					recv_data.send_status_list.clear();
				}
				else
				{
					session_down_thread_data->recive_data_map[sorry_endpoint] = recv_data;
				}
			}
		}

		//送信先endpointにendpointを設定する
		session_data->target_endpoint = sorry_endpoint;
		//遷移先ステータスを設定する
		status = SORRYSERVER_CONNECT;
	} catch (const std::bad_alloc& ex)
	{
		std::cerr << "handle_client_recv exception: Could not allocate memory." << std::endl;
		putLogError(17000, "Could not allocate memory.", __FILE__, __LINE__ );
		status = FINALIZE;
		status = FINALIZE;
	} catch (const std::exception& ex)
	{
		std::cerr << "handle_sorryserver_select exception: error=%s" << ex.what() << "." << std::endl;
		boost::format formatter("function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_sorryserver_select() exception: result=%d, error=%s.");
		formatter % FINALIZE % ex.what();
		putLogError(17000, formatter.str(), __FILE__, __LINE__ );
		status = FINALIZE;
	} catch (...)
	{
		std::cerr << "Unkown exception." << std::endl;
		boost::format formatter("function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_sorryserver_select() exception: result=%d.");
		formatter % FINALIZE;
		putLogError(17000, formatter.str(), __FILE__, __LINE__ );
		status = FINALIZE;
	}

	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		boost::format
		        formatter(
		                "out_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			                "handle_sorryserver_select( const boost::thread::id thread_id, boost::asio::ip::tcp::endpoint & sorry_endpoint): return_value=%d");
		formatter % status;
		putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
	}
	/*------DEBUG LOG END------*/
	return status;
}

/*!
 *handle_sorryserver_connect
 *
 *@param [in] thread_id
 *@param [out]sendbuffer
 *@param [out]datalen
 */
protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::handle_sorryserver_connect(
        const boost::thread::id thread_id, boost::array<char, MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen)
{
	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		boost::format formatter("in_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_sorryserver_connect( const boost::thread::id thread_id, "
			"boost::array<char, MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen): "
			"thread_id=%d, sendbuffer=%d, datalen=%d");
		formatter % thread_id % sendbuffer.data() % datalen;
		putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
	}
	/*------DEBUG LOG END------*/
	EVENT_TAG status;

	bool ret = false;
	size_t url_offset = 0;
	size_t url_offset_len = 0;
	size_t header_offset = 0;
	size_t header_offset_len = 0;

	const std::string http_header = "";
	const std::string str_forword_for = "X-Forwarded-For";
	size_t send_buffer_remian_size = 0;
	const int send_buffer_end_size = sendbuffer.max_size();
	size_t copy_size = 0;

	session_thread_data_sessionless* session_data = NULL;

	try
	{
		{
			boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
			if (session_thread_data_map.find(thread_id) == session_thread_data_map.end())
			{
				putLogError(17000, "Invalid thread id.", __FILE__, __LINE__ );
				return FINALIZE;
			}

			session_data = session_thread_data_map[thread_id];
			if (session_data == NULL)
			{
				putLogError(17000, "Invalid pointer.", __FILE__, __LINE__ );
				return FINALIZE;
			}
		}

		//
		if (session_data->recive_data_map.find(session_data->client_endpoint_tcp)
		        == session_data->recive_data_map.end())
		{
			putLogError(17000, "Invalid endpoint.", __FILE__, __LINE__ );
			return FINALIZE;
		}

		recive_data& recv_data = session_data->recive_data_map[session_data->client_endpoint_tcp];

		//送信可能データありをチェック
		send_status_it it = recv_data.send_status_list.begin();
		send_status_it it_end = recv_data.send_status_list.end();
		it = find_if(it, it_end, data_send_possible());
		if (it == it_end)
		{
			putLogError(17000, "sending possible is not existed.", __FILE__, __LINE__ );
			return FINALIZE;
		}

		//送信バッファ残サイズに送信バッファサイズを設定する
		send_buffer_remian_size = send_buffer_end_size;

		//編集区分 = 編集あり
		if (it->edit_division == EDIT_DIVISION_EDIT)
		{
			//編集データリストが空の場合
			if (it->edit_data_list.empty())
			{
				//編集データ情報を生成する
				edit_data edata;
				//送信可能データからURIを検索する
				ret = find_uri(recv_data.recive_buffer + it->send_offset, it->send_possible_size, url_offset,
				        url_offset_len);
				if (ret)
				{
					//編集データ情報の編集データにsorryURIを設定する
					edata.data = sorry_uri.data();
					//編集データ情報の編集データ設定位置にURIの開始位置を設定する
					edata.insert_posission = url_offset;
					//編集データ情報の編集データサイズにsorryURIのサイズを設定する
					edata.data_size = edata.data.size();
					//編集データ情報の上書きサイズにURIのサイズを設定する
					edata.replace_size = url_offset_len;
					//編集データリストに編集データ情報を設定する
					it->edit_data_list.push_back(edata);
				}
				//
				else
				{
				}
				//送信可能データからHTTPヘッダ（X-Forwarded-For）を検索する
				ret = find_http_header(recv_data.recive_buffer + it->send_offset, it->send_possible_size,
				        str_forword_for.c_str(), header_offset, header_offset_len);

				//検索結果 = 該当あり
				if (ret)
				{
					//編集データ情報の編集データにX-Forwarded-Forを編集して設定する
					edata.data.assign(recv_data.recive_buffer + it->send_offset + header_offset, header_offset_len
					        - strlen("\r\n"));
					edata.data += ", ";
					edata.data += session_data->client_endpoint_tcp.address().to_string();
					edata.data += "\r\n";
					//編集データ情報の編集データ設定位置にX-Forwarded-Forの開始位置を設定する
					edata.insert_posission = header_offset;
					//編集データ情報の編集データサイズに編集後のX-Forwarded-Forのサイズを設定する
					edata.data_size = edata.data.size();
					//編集データ情報の上書きサイズに編集前のX-Forwarded-Forのサイズを設定する
					edata.replace_size = header_offset_len;
				}
				//検索結果 = 該当なし
				else
				{
					//送信可能データ内のHTTPヘッダの開始位置、サイズを取得する
					find_http_header(recv_data.recive_buffer + it->send_offset, it->send_possible_size, "",
					        header_offset, header_offset_len);
					//編集データ情報の編集データにX-Forwarded-Forを編集して設定する
					edata.data = str_forword_for;
					edata.data += ": ";
					edata.data += session_data->client_endpoint_tcp.address().to_string();
					edata.data += "\r\n";
					//編集データ情報の編集データ設定位置にHTTPヘッダの開始位置を設定する
					edata.insert_posission = header_offset;
					//編集データ情報の編集データサイズに編集後のX-Forwarded-Forのサイズを設定する
					edata.data_size = edata.data.size();
					//編集データ情報の上書きサイズに0を設定する
					edata.replace_size = 0;
				}

				//編集データリストに編集データ情報を設定する
				it->edit_data_list.push_back(edata);
			}

			//
			while (true)
			{
				//編集データリストが空の場合
				if (it->edit_data_list.empty())
				{
					//編集区分に編集なしを設定する
					it->edit_division = EDIT_DIVISION_NO_EDIT;
					//送信バッファ残サイズ　＞　0　かつ　送信可能データサイズ　＞　0
					if (send_buffer_remian_size > 0 && it->send_possible_size > 0)
					{
						//送信バッファ残サイズ　≧　送信可能データサイズ
						if (send_buffer_remian_size > it->send_possible_size)
						{
							//送信可能データを送信データオフセット＋送信済サイズの位置から
							//送信可能データサイズ分、送信バッファにコピーする
							copy_size = it->send_possible_size;
							memcpy(sendbuffer.data() + send_buffer_end_size - send_buffer_remian_size,
							        recv_data.recive_buffer + it->send_offset + it->send_end_size, copy_size);
							//送信済サイズにコピーしたサイズを加算する
							it->send_end_size += copy_size;
							//送信可能データサイズからコピーしたサイズを減算する
							it->send_possible_size = 0;
							//送信バッファ残サイズからコピーしたサイズを減算する
							send_buffer_remian_size -= copy_size;
						}
						//送信バッファ残サイズ　＜　送信可能データサイズ
						else
						{
							//送信可能データを送信データオフセット＋送信済サイズの位置から
							//送信バッファ残サイズ分、送信バッファにコピーする
							memcpy(sendbuffer.data() + send_buffer_end_size - send_buffer_remian_size,
							        recv_data.recive_buffer + it->send_offset + it->send_end_size,
							        send_buffer_remian_size);
							//送信済サイズにコピーしたサイズを加算する
							it->send_end_size += send_buffer_remian_size;
							//送信可能データサイズからコピーしたサイズを減算する
							it->send_possible_size -= send_buffer_remian_size;
							//送信バッファ残サイズからコピーしたサイズを減算する
							send_buffer_remian_size = 0;
						}
					}
					//繰り返し処理を終了する
					break;
				}
				//編集データリストが空でない場合
				else
				{
					//編集データリストから最小の編集データ設定位置を持つ要素を検索する
					std::list<edit_data>::iterator edit_min = std::min_element(it->edit_data_list.begin(),
					        it->edit_data_list.end());
					//送信バッファ残サイズ　≧　編集データ設定位置　－　送信済サイズ
					if (send_buffer_remian_size >= edit_min->insert_posission - it->send_end_size)
					{
						//送信可能データを送信データオフセット＋送信済サイズの位置から
						//編集データ設定位置－送信済サイズ分、送信バッファにコピーする
						copy_size = edit_min->insert_posission - it->send_end_size;
						memcpy(sendbuffer.data() + send_buffer_end_size - send_buffer_remian_size,
						        recv_data.recive_buffer + it->send_offset + it->send_end_size, copy_size);
						//送信済サイズにコピーしたサイズを加算する
						it->send_end_size += copy_size;
						//送信可能データサイズからコピーしたサイズを減算する
						it->send_possible_size -= copy_size;
						//送信バッファ残サイズからコピーしたサイズを減算する
						send_buffer_remian_size -= copy_size;

						//送信バッファ残サイズ　≧　編集データサイズ
						if (send_buffer_remian_size >= edit_min->data_size)
						{
							//編集データを編集データサイズ分、送信バッファにコピーする
							memcpy(sendbuffer.data() + send_buffer_end_size - send_buffer_remian_size,
							        recv_data.recive_buffer + it->send_offset, edit_min->data_size);
							//送信済サイズに上書きサイズを加算する
							it->send_end_size += edit_min->replace_size;
							//送信可能データサイズから上書きサイズを減算する
							it->send_possible_size += edit_min->replace_size;
							//送信バッファ残サイズから編集データサイズを減算する
							send_buffer_remian_size -= edit_min->data_size;
							//編集データリストから該当要素を削除する
							it->edit_data_list.erase(edit_min);
						}
						//送信バッファ残サイズ　＜　編集データサイズ
						else
						{
							break;
						}
					}
					//送信バッファ残サイズ　＜　編集データ設定位置　－　送信済サイズ
					else
					{
						//送信可能データを送信データオフセット＋送信済サイズの位置から
						//送信バッファ残サイズ分、送信バッファにコピーする
						memcpy(sendbuffer.data() + send_buffer_end_size - send_buffer_remian_size,
						        recv_data.recive_buffer + it->send_offset + it->send_end_size, send_buffer_remian_size);
						//送信済サイズにコピーしたサイズを加算する
						it->send_end_size += send_buffer_remian_size;
						//送信可能データサイズからコピーしたサイズを減算する
						it->send_possible_size -= send_buffer_remian_size;
						//送信バッファ残サイズからコピーしたサイズを減算する
						send_buffer_remian_size -= 0;
						//繰り返し処理を終了する
						break;
					}
				}
			}
		}
		//編集区分 = 編集なし
		else
		{
			//送信バッファサイズ　≧　送信可能データサイズ
			if (send_buffer_remian_size >= it->send_possible_size)
			{
				//送信可能データを先頭から送信可能データサイズ分、送信バッファにコピーする
				memcpy(sendbuffer.data() + send_buffer_end_size - send_buffer_remian_size, recv_data.recive_buffer
				        + it->send_offset, it->send_possible_size);
				//送信済データサイズに送信可能データサイズを設定する
				it->send_end_size = it->send_possible_size;
				//送信可能データサイズに0を設定する
				it->send_possible_size = 0;
			}
			//送信バッファサイズ　＜　送信可能データサイズ
			else
			{
				//送信可能データを先頭から送信バッファサイズ分、送信バッファにコピーする
				memcpy(sendbuffer.data() + send_buffer_end_size - send_buffer_remian_size, recv_data.recive_buffer
				        + it->send_offset, send_buffer_remian_size);
				//送信済データサイズに送信バッファサイズを設定する
				it->send_end_size = send_buffer_remian_size;
				//送信可能データサイズから送信バッファサイズを減算する
				it->send_possible_size -= send_buffer_remian_size;
			}
		}

		datalen = send_buffer_end_size - send_buffer_remian_size;

		status = SORRYSERVER_SEND;

	} catch (const std::exception& ex)
	{
		std::cerr << "handle_sorryserver_connect exception: error=%s" << ex.what() << "." << std::endl;
		boost::format formatter("function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_sorryserver_connect() exception: result=%d, error=%s.");
		formatter % FINALIZE % ex.what();
		putLogError(17000, formatter.str(), __FILE__, __LINE__ );
		status = FINALIZE;
	} catch (...)
	{
		std::cerr << "Unkown exception." << std::endl;
		boost::format formatter("function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_sorryserver_connect() exception: result=%d.");
		formatter % FINALIZE;
		putLogError(17000, formatter.str(), __FILE__, __LINE__ );
		status = FINALIZE;
	}

	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		boost::format
		        formatter(
		                "out_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			                "handle_sorryserver_connect( const boost::thread::id thread_id, boost::array<char, MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen): return_value=%d");
		formatter % status;
		putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
	}
	/*------DEBUG LOG END------*/

	return status;
}

/*!
 *handle_sorryserver_connection_fail
 *
 *@param [in] thread_id
 *@param [out]sendbuffer
 *@param [out]datalen
 */
protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::handle_sorryserver_connection_fail(
        const boost::thread::id thread_id, const boost::asio::ip::tcp::endpoint & sorry_endpoint)
{
	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		boost::format formatter("in_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_sorryserver_connection_fail( const boost::thread::id thread_id, "
			"const boost::asio::ip::tcp::endpoint & sorry_endpoint): "
			"thread_id=%d, sorry_endpoint=%s ");
		formatter % thread_id % sorry_endpoint.address().to_string();
		putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
	}
	/*------DEBUG LOG END------*/

	EVENT_TAG status;
	session_thread_data_sessionless* session_data = NULL;

	try
	{

		boost::mutex::scoped_lock slock(session_thread_data_map_mutex);
		if (session_thread_data_map.find(thread_id) == session_thread_data_map.end())
		{
			putLogError(17000, "Invalid thread id.", __FILE__, __LINE__ );
			return FINALIZE;
		}

		session_data = session_thread_data_map[thread_id];
		if (session_data == NULL)
		{
			putLogError(17000, "Invalid pointer.", __FILE__, __LINE__ );
			return FINALIZE;
		}

		//終了フラグをON
		session_data->end_flag = END_FLAG_ON;
		//遷移先ステータスを設定する
		status = CLIENT_DISCONNECT;
	} catch (const std::exception& ex)
	{
		std::cerr << "handle_sorryserver_connection_fail exception: error=%s" << ex.what() << "." << std::endl;
		boost::format formatter("function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_sorryserver_connection_fail() exception: result=%d, error=%s.");
		formatter % FINALIZE % ex.what();
		putLogError(17000, formatter.str(), __FILE__, __LINE__ );
		status = FINALIZE;
	} catch (...)
	{
		std::cerr << "Unkown exception." << std::endl;
		boost::format formatter("function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_sorryserver_connection_fail() exception: result=%d.");
		formatter % FINALIZE;
		putLogError(17000, formatter.str(), __FILE__, __LINE__ );
		status = FINALIZE;
	}

	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		boost::format
		        formatter(
		                "out_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			                "handle_sorryserver_connection_fail(  const boost::thread::id thread_id, const boost::asio::ip::tcp::endpoint & sorry_endpoint): return_value=%d");
		formatter % status;
		putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
	}
	/*------DEBUG LOG END------*/
	return status;
}

/*!
 *handle_sorryserver_send
 *
 *@param [in] thread_id
 */
protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::handle_sorryserver_send(
        const boost::thread::id thread_id)
{
	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		boost::format formatter("in_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_sorryserver_send( const boost::thread::id thread_id): "
			"thread_id=%d");
		formatter % thread_id;
		putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
	}
	/*------DEBUG LOG END------*/
	EVENT_TAG status;
	session_thread_data_sessionless* session_data = NULL;

	try
	{
		{
			boost::mutex::scoped_lock slock(session_thread_data_map_mutex);
			if (session_thread_data_map.find(thread_id) == session_thread_data_map.end())
			{
				putLogError(17000, "Invalid thread id.", __FILE__, __LINE__ );
				return FINALIZE;
			}

			session_data = session_thread_data_map[thread_id];
			if (session_data == NULL)
			{
				putLogError(17000, "Invalid pointer.", __FILE__, __LINE__ );
				return FINALIZE;
			}
		}

		//
		if (session_data->recive_data_map.find(session_data->client_endpoint_tcp)
		        == session_data->recive_data_map.end())
		{
			putLogError(17000, "Invalid endpoint.", __FILE__, __LINE__ );
			return FINALIZE;
		}

		recive_data& recv_data = session_data->recive_data_map[session_data->client_endpoint_tcp];

		send_status_it it = recv_data.send_status_list.begin();
		send_status_it it_end = recv_data.send_status_list.end();
		it = find_if(it, it_end, data_send_ok());
		if (it == it_end)
		{
			putLogError(17000, "sending possible data is not existed.", __FILE__, __LINE__ );
			return FINALIZE;
		}

		//送信可能データサイズ　＞　0
		if (it->send_possible_size > 0)
		{
			//送信状態に送信待を設定する
			it->status = SEND_OK;
			//送信データオフセットに送信済データサイズを加算する
			it->send_offset += it->send_end_size;

			for (std::list<edit_data>::iterator list_it = it->edit_data_list.begin(); list_it
			        != it->edit_data_list.end(); ++list_it)
			{
				//編集データ設定位置から送信済データサイズを減算する。
				list_it->insert_posission -= it->send_end_size;
			}

			//送信済データサイズに0を設定する
			it->send_end_size = 0;
		}
		//送信可能データサイズ　≦　0
		else
		{
			//送信データ残サイズ　＞　0
			if (it->send_rest_size > 0)
			{
				//送信状態に送信継続を設定する
				it->status = SEND_CONTINUE;
			}
			//送信データ残サイズ　≦　0
			else
			{
				//送信状態に送信済を設定する
				it->status = SEND_END;
			}
		}

		it = recv_data.send_status_list.begin();
		it = find_if(it, it_end, data_send_ok());
		//送信可能データあり
		if (it != it_end)
		{
			//遷移先ステータスを設定する
			status = SORRYSERVER_CONNECT;
		}
		//送信可能データなし
		else
		{
			//遷移先ステータスを設定する
			status = CLIENT_RECV;
		}

	} catch (const std::exception& ex)
	{
		std::cerr << "handle_sorryserver_send exception: error=%s" << ex.what() << "." << std::endl;
		boost::format formatter("function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_sorryserver_send() exception: result=%d, error=%s.");
		formatter % FINALIZE % ex.what();
		putLogError(17000, formatter.str(), __FILE__, __LINE__ );
		status = FINALIZE;
	} catch (...)
	{
		std::cerr << "Unkown exception." << std::endl;
		boost::format formatter("function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_sorryserver_send() exception: result=%d.");
		formatter % FINALIZE;
		putLogError(17000, formatter.str(), __FILE__, __LINE__ );
		status = FINALIZE;
	}

	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		boost::format formatter("out_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_sorryserver_send( const boost::thread::id thread_id): return_value=%d");
		formatter % status;
		putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
	}
	/*------DEBUG LOG END------*/

	return status;
}

/*!
 *handle_realserver_recv
 *
 *@param [in] thread_id
 *@param [in] rs_endpoint
 *@param [in] recvbuffer
 *@param [in] recvlen
 */
protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::handle_realserver_recv(
        const boost::thread::id thread_id, const boost::asio::ip::tcp::endpoint& rs_endpoint, const boost::array<char,
                MAX_BUFFER_SIZE>& recvbuffer, const size_t recvlen)
{
	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		boost::format formatter("in_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_realserver_recv( const boost::thread::id thread_id,"
			"const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,"
			"const size_t recvlen ): thread_id=%d, recvbuffer=%s, recvlen=%d");
		formatter % thread_id % recvbuffer.data() % recvlen;
		putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
	}
	/*------DEBUG LOG END------*/

	EVENT_TAG status;
	typedef std::vector<protocol_module_sessionless::send_status_it> send_end_it_vec;
	send_end_it_vec send_end_vec;
	size_t data_remain_start = 0; //未送信データ開始位置
	size_t data_remain_size = 0; //未送信データサイズ
	size_t request_data_remain_size = 0; //リクエストデータ残サイズ
	bool bret = false;
	CHECK_RESULT_TAG check_result; //チェック結果
	size_t header_offset = 0;
	size_t header_offset_len = 0;
	size_t count_length_header_value = 0;
	size_t value = 0;
	std::string str_value;
	const std::string http_header = "";
	const std::string content_header = "Content-Length";
	session_thread_data_sessionless* session_data = NULL;

	char* buffer1 = NULL;
	char* buffer2 = NULL;
	size_t count = 0;

	//パラメータチェック
	if (recvlen > recvbuffer.size())
	{
		putLogError(17000, "Data size bigger than buffer size.", __FILE__,
		__LINE__ );
		/*-------- DEBUG LOG --------*/
		if (LOG_LV_DEBUG == getloglevel())
		{
			boost::format formatter(
			        "out_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
				        "handle_realserver_recv( const boost::thread::id thread_id,"
				        "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,"
				        "const size_t recvlen ): return_value=%d");
			formatter % FINALIZE;
			putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
		}
		/*------DEBUG LOG END------*/
		return FINALIZE;
	}

	try
	{
		{
			boost::mutex::scoped_lock slock(session_thread_data_map_mutex);

			if (session_thread_data_map.find(thread_id) == session_thread_data_map.end())
			{
				putLogError(17000, "Invalid thread id.", __FILE__, __LINE__ );
				/*-------- DEBUG LOG --------*/
				if (LOG_LV_DEBUG == getloglevel())
				{
					boost::format formatter(
					        "out_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
						        "handle_realserver_recv( const boost::thread::id thread_id,"
						        "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,"
						        "const size_t recvlen ): return_value=%d");
					formatter % FINALIZE;
					putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
				}
				/*------DEBUG LOG END------*/

				return FINALIZE;
			}

			session_data = session_thread_data_map[thread_id];
			if (session_data == NULL)
			{
				putLogError(17000, "Invalid thread id.", __FILE__, __LINE__ );
				/*-------- DEBUG LOG --------*/
				if (LOG_LV_DEBUG == getloglevel())
				{
					boost::format formatter(
					        "out_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
						        "handle_realserver_recv( const boost::thread::id thread_id,"
						        "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,"
						        "const size_t recvlen ): return_value=%d");
					formatter % FINALIZE;
					putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
				}
				/*------DEBUG LOG END------*/
				return FINALIZE;
			}
		}

		//終了フラグがONの場合
		if (session_data->end_flag == END_FLAG_ON)
		{
			/*-------- DEBUG LOG --------*/
			if (LOG_LV_DEBUG == getloglevel())
			{
				boost::format formatter(
				        "out_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
					        "handle_realserver_recv( const boost::thread::id thread_id,"
					        "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,"
					        "const size_t recvlen ): return_value=%d");
				formatter % CLIENT_RECV;
				putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
			}
			return CLIENT_RECV;
		}

		//終了フラグがOFFの場合
		if (session_data->recive_data_map.find(session_data->target_endpoint) == session_data->recive_data_map.end())
		{
			putLogError(17000, "Invalid endpoint.", __FILE__, __LINE__ );
			/*-------- DEBUG LOG --------*/
			if (LOG_LV_DEBUG == getloglevel())
			{
				boost::format formatter(
				        "out_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
					        "handle_realserver_recv( const boost::thread::id thread_id,"
					        "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,"
					        "const size_t recvlen ): return_value=%d");
				formatter % FINALIZE;
				putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
			}
			/*------DEBUG LOG END------*/
			return FINALIZE;
		}

		recive_data& recv_data = session_data->recive_data_map[session_data->target_endpoint];

		send_status_it it = recv_data.send_status_list.begin();
		send_status_it it_end = recv_data.send_status_list.end();

		count = std::count_if(it, it_end, data_send_continue());
		if (count >= 2)
		{
			putLogError(17000, "send continue data >= 2.", __FILE__, __LINE__ );
			/*-------- DEBUG LOG --------*/
			if (LOG_LV_DEBUG == getloglevel())
			{
				boost::format formatter(
				        "out_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
					        "handle_realserver_recv( const boost::thread::id thread_id,"
					        "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,"
					        "const size_t recvlen ): return_value=%d");
				formatter % FINALIZE;
				putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
			}
			/*------DEBUG LOG END------*/
			return FINALIZE;
		}

		it = recv_data.send_status_list.begin();
		it_end = recv_data.send_status_list.end();

		count = std::count_if(it, it_end, data_send_ng());
		if (count >= 2)
		{
			putLogError(17000, "send ng data >= 2.", __FILE__, __LINE__ );
			/*-------- DEBUG LOG --------*/
			if (LOG_LV_DEBUG == getloglevel())
			{
				boost::format formatter(
				        "out_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
					        "handle_realserver_recv( const boost::thread::id thread_id,"
					        "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,"
					        "const size_t recvlen ): return_value=%d");
				formatter % FINALIZE;
				putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
			}
			/*------DEBUG LOG END------*/
			return FINALIZE;
		}

		it = recv_data.send_status_list.begin();
		it_end = recv_data.send_status_list.end();

		//送信状態リストの要素数分繰り返し
		for (; it != it_end; ++it)
		{
			//送信状態が送信済の場合
			if (it->status == SEND_END)
			{
				//送信状態リストから削除する
				send_end_vec.push_back(it);
			}
			else if (it->status == SEND_CONTINUE) //送信状態が送信継続の場合
			{
				//送信データオフセットに送信済サイズを加算する
				it->send_offset += it->send_end_size;
				//未送信データ開始位置に送信データオフセットを設定する
				data_remain_start = it->send_offset;
				//繰り返し処理を終了する
				break;
			}
			else
			{ //送信状態が送信済以外の場合
				//未送信データ開始位置に送信データオフセットを設定する
				data_remain_start = it->send_offset;
				//未送信データサイズに未送信データサイズを設定する
				data_remain_size = it->unsend_size;
				//繰り返し処理を終了する
				break;
			}
		}

		for (send_end_it_vec::iterator it = send_end_vec.begin(); it != send_end_vec.end(); ++it)
		{
			//送信状態リストから削除する
			recv_data.send_status_list.erase(*it);
		}

		//データバッファ残サイズ　＜　リクエストデータサイズ
		if (recv_data.recive_buffer_rest_size < recvlen)
		{
			//データバッファMAXサイズ　＜　未送信データサイズ　＋　リクエストデータサイズ
			if (recv_data.recive_buffer_max_size < data_remain_size + recvlen)
			{
				if (recv_data.recive_buffer == NULL)
				{
					//データバッファ１をMAX_BUFFER_SIZE分、
					//新規に確保し、初期化する
					recv_data.recive_buffer1 = new char[MAX_BUFFER_SIZE];
					/*-------- DEBUG LOG --------*/
					if (LOG_LV_DEBUG == getloglevel())
					{
						boost::format formatter("new: address=&(%d), size=%lu");
						formatter % &recv_data.recive_buffer1 % (MAX_BUFFER_SIZE);
						putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
					}
					/*-------- DEBUG LOG --------*/
					memset(recv_data.recive_buffer1, 0, MAX_BUFFER_SIZE);
					//データバッファ２をMAX_BUFFER_SIZE分、
					//新規に確保し、初期化する
					recv_data.recive_buffer2 = new char[MAX_BUFFER_SIZE];
					/*-------- DEBUG LOG --------*/
					if (LOG_LV_DEBUG == getloglevel())
					{
						boost::format formatter("new: address=&(%d), size=%lu");
						formatter % &recv_data.recive_buffer2 % (MAX_BUFFER_SIZE);
						putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
					}
					/*-------- DEBUG LOG --------*/
					memset(recv_data.recive_buffer2, 0, MAX_BUFFER_SIZE);
					recv_data.recive_buffer = recv_data.recive_buffer1;
					//データバッファMAXサイズにMAX_BUFFER_SIZEを設定する
					recv_data.recive_buffer_max_size = MAX_BUFFER_SIZE;
				}
				else
				{
					//データバッファ１を「未送信データサイズ　＋　リクエストデータサイズ」分、
					//新規に確保し、初期化する
					buffer1 = new char[data_remain_size + recvlen];
					/*-------- DEBUG LOG --------*/
					if (LOG_LV_DEBUG == getloglevel())
					{
						boost::format formatter("new: address=&(%d), size=%lu");
						formatter % &buffer1 % (data_remain_size + recvlen);
						putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
					}
					/*-------- DEBUG LOG --------*/
					memset(buffer1, 0, data_remain_size + recvlen);
					//データバッファ２を「未送信データサイズ　＋　リクエストデータサイズ」分、
					//新規に確保し、初期化する
					buffer2 = new char[data_remain_size + recvlen];
					/*-------- DEBUG LOG --------*/
					if (LOG_LV_DEBUG == getloglevel())
					{
						boost::format formatter("new: address=&(%d), size=%lu");
						formatter % &buffer2 % (data_remain_size + recvlen);
						putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
					}
					/*-------- DEBUG LOG --------*/
					memset(buffer2, 0, data_remain_size + recvlen);

					/*-------- DEBUG LOG --------*/
					if (LOG_LV_DEBUG == getloglevel())
					{
						boost::format formatter("%02x");
						for (size_t i = 0; i < data_remain_size + recvlen; ++i)
						{
							formatter % static_cast<unsigned short> (static_cast<unsigned char> (buffer1[i]));
						}

						std::string datadump = formatter.str();
						formatter.parse(
						        "function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
							        "handle_realserver_recv( ): before memcpy (data dump):  "
							        "data begin(%d), data_size=%d,data=%s.");
						formatter % buffer1 % (data_remain_size + recvlen) % datadump;
						putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
					}
					/*------DEBUG LOG END------*/
					//データバッファ１に使用中データバッファの未送信データ開始位置から未送信データサイズ分設定する
					memcpy(buffer1, recv_data.recive_buffer + data_remain_start, data_remain_size);
					//データバッファ１にリクエストデータをサイズ分追加設定する
					memcpy(buffer1 + data_remain_size, recvbuffer.data(), recvlen);
					/*-------- DEBUG LOG --------*/
					if (LOG_LV_DEBUG == getloglevel())
					{
						boost::format formatter("%02x");
						for (size_t i = 0; i < data_remain_size + recvlen; ++i)
						{
							formatter % static_cast<unsigned short> (static_cast<unsigned char> (buffer2[i]));
						}

						std::string datadump = formatter.str();
						formatter.parse(
						        "function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
							        "handle_realserver_recv( ): after memcpy (data dump):  "
							        "data begin(%d), data_size=%d,data=%s.");
						formatter % buffer2 % (data_remain_size + recvlen) % datadump;
						putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
					}
					/*------DEBUG LOG END------*/
					//旧データバッファ１、旧データバッファ２を解放する
					if (recv_data.recive_buffer1 != NULL)
					{
						/*-------- DEBUG LOG --------*/
						if (LOG_LV_DEBUG == getloglevel())
						{
							boost::format formatter("delete: address=&(%d)");
							formatter % recv_data.recive_buffer1;
							putLogDebug(10005, formatter.str(), __FILE__,
							__LINE__ );
						}
						/*------DEBUG LOG END------*/
						delete[] recv_data.recive_buffer1;
						recv_data.recive_buffer1 = NULL;
					}

					if (recv_data.recive_buffer2 != NULL)
					{
						/*-------- DEBUG LOG --------*/
						if (LOG_LV_DEBUG == getloglevel())
						{
							boost::format formatter("delete: address=&(%d)");
							formatter % recv_data.recive_buffer2;
							putLogDebug(10005, formatter.str(), __FILE__,
							__LINE__ );
						}
						/*------DEBUG LOG END------*/
						delete[] recv_data.recive_buffer2;
						recv_data.recive_buffer2 = NULL;
					}

					//使用中データバッファにデータバッファ１を設定する
					recv_data.recive_buffer1 = buffer1;
					recv_data.recive_buffer2 = buffer2;
					recv_data.recive_buffer = recv_data.recive_buffer1;
					//データバッファMAXサイズに「未送信データサイズ　＋　リクエストデータサイズ」を設定する
					recv_data.recive_buffer_max_size = data_remain_size + recvlen;
				}
			}
			//データバッファMAXサイズ　≧　未送信データサイズ　＋　リクエストデータサイズ
			else
			{
				if (recv_data.recive_buffer1 == NULL || recv_data.recive_buffer2 == NULL)
				{
					putLogError(17000, "Invalid pointer.", __FILE__, __LINE__ );
					/*-------- DEBUG LOG --------*/
					if (LOG_LV_DEBUG == getloglevel())
					{
						boost::format formatter(
						        "out_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
							        "handle_realserver_recv( const boost::thread::id thread_id,"
							        "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,"
							        "const size_t recvlen ): return_value=%d");
						formatter % FINALIZE;
						putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
					}
					/*------DEBUG LOG END------*/
					return FINALIZE;
				}
				//使用中データバッファがデータバッファ１の場合
				if (recv_data.recive_buffer == recv_data.recive_buffer1)
				{
					//データバッファ２を初期化する
					memset(recv_data.recive_buffer2, 0, recv_data.recive_buffer_max_size);
					/*-------- DEBUG LOG --------*/
					if (LOG_LV_DEBUG == getloglevel())
					{
						boost::format formatter("%02x");
						for (size_t i = 0; i < recv_data.recive_buffer_max_size; ++i)
						{
							formatter
							        % static_cast<unsigned short> (static_cast<unsigned char> (recv_data.recive_buffer2[i]));
						}

						std::string datadump = formatter.str();
						formatter.parse(
						        "function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
							        "handle_realserver_recv( ): before memcpy (data dump):  "
							        "data begin(%d), data_size=%d,data=%s.");
						formatter % recv_data.recive_buffer2 % recv_data.recive_buffer_max_size % datadump;
						putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
					}
					/*------DEBUG LOG END------*/
					//データバッファ２に使用中データバッファの未送信データ開始位置から未送信データサイズ分設定する
					memcpy(recv_data.recive_buffer2, recv_data.recive_buffer + data_remain_start, data_remain_size);
					//データバッファ２にリクエストデータをサイズ分追加設定する
					memcpy(recv_data.recive_buffer2 + data_remain_size, recvbuffer.data(), recvlen);
					/*-------- DEBUG LOG --------*/
					if (LOG_LV_DEBUG == getloglevel())
					{
						boost::format formatter("%02x");
						for (size_t i = 0; i < recv_data.recive_buffer_max_size; ++i)
						{
							formatter
							        % static_cast<unsigned short> (static_cast<unsigned char> (recv_data.recive_buffer2[i]));
						}

						std::string datadump = formatter.str();
						formatter.parse(
						        "function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
							        "handle_realserver_recv( ): after memcpy (data dump):  "
							        "data begin(%d), data_size=%d,data=%s.");
						formatter % recv_data.recive_buffer2 % recv_data.recive_buffer_max_size % datadump;
						putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
					}
					/*------DEBUG LOG END------*/
					//使用中データバッファにデータバッファ２を設定する
					recv_data.recive_buffer = recv_data.recive_buffer2;
				}
				//使用中データバッファがデータバッファ２の場合
				else
				{
					//データバッファ１を初期化する
					memset(recv_data.recive_buffer1, 0, recv_data.recive_buffer_max_size);
					/*-------- DEBUG LOG --------*/
					if (LOG_LV_DEBUG == getloglevel())
					{
						boost::format formatter("%02x");
						for (size_t i = 0; i < recv_data.recive_buffer_max_size; ++i)
						{
							formatter
							        % static_cast<unsigned short> (static_cast<unsigned char> (recv_data.recive_buffer1[i]));
						}

						std::string datadump = formatter.str();
						formatter.parse(
						        "function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
							        "handle_realserver_recv( ): before memcpy (data dump):  "
							        "data begin(%d), data_size=%d,data=%s.");
						formatter % recv_data.recive_buffer1 % recv_data.recive_buffer_max_size % datadump;
						putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
					}
					/*------DEBUG LOG END------*/
					//データバッファ１に使用中データバッファの未送信データ開始位置から未送信データサイズ分設定する
					memcpy(recv_data.recive_buffer1, recv_data.recive_buffer + data_remain_start, data_remain_size);
					//データバッファ１にリクエストデータをサイズ分追加設定する
					memcpy(recv_data.recive_buffer1 + data_remain_size, recvbuffer.data(), recvlen);
					/*-------- DEBUG LOG --------*/
					if (LOG_LV_DEBUG == getloglevel())
					{
						boost::format formatter("%02x");
						for (size_t i = 0; i < recv_data.recive_buffer_max_size; ++i)
						{
							formatter
							        % static_cast<unsigned short> (static_cast<unsigned char> (recv_data.recive_buffer1[i]));
						}

						std::string datadump = formatter.str();
						formatter.parse(
						        "function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
							        "handle_realserver_recv( ): after memcpy (data dump):  "
							        "data begin(%d), data_size=%d,data=%s.");
						formatter % recv_data.recive_buffer1 % recv_data.recive_buffer_max_size % datadump;
						putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
					}
					/*------DEBUG LOG END------*/
					//使用中データバッファにデータバッファ１を設定する
					recv_data.recive_buffer = recv_data.recive_buffer1;
				}
			}

			//データバッファ残サイズに
			//「データバッファMAXサイズ　－　未送信データサイズ　－　リクエストデータサイズ」を設定する
			recv_data.recive_buffer_rest_size = recv_data.recive_buffer_max_size - data_remain_size - recvlen;

			//未送信データサイズに「未送信データサイズ　＋　リクエストデータサイズ」を設定する
			data_remain_size += recvlen;

			send_status_it it_begin = recv_data.send_status_list.begin();
			send_status_it it_end = recv_data.send_status_list.end();

			for (; it_begin != it_end; ++it_begin)
			{
				//送信データオフセットから未送信データ開始位置を減算
				it->send_offset -= data_remain_start;
			}
		}
		//データバッファ残サイズ　≧　リクエストデータサイズ
		else
		{
			if (recv_data.recive_buffer == NULL)
			{
				putLogError(17000, "Invalid thread id.", __FILE__, __LINE__ );
				/*-------- DEBUG LOG --------*/
				if (LOG_LV_DEBUG == getloglevel())
				{
					boost::format formatter(
					        "out_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
						        "handle_realserver_recv( const boost::thread::id thread_id,"
						        "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,"
						        "const size_t recvlen ): return_value=%d");
					formatter % FINALIZE;
					putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
				}
				/*------DEBUG LOG END------*/
				return FINALIZE;
			}
			/*-------- DEBUG LOG --------*/
			if (LOG_LV_DEBUG == getloglevel())
			{
				boost::format formatter("%02x");
				for (size_t i = 0; i < recv_data.recive_buffer_max_size; ++i)
				{
					formatter % static_cast<unsigned short> (static_cast<unsigned char> (recv_data.recive_buffer1[i]));
				}

				std::string datadump = formatter.str();
				formatter.parse("function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
					"handle_realserver_recv( ): before memcpy (data dump):  "
					"data begin(%d), data_size=%d,data=%s.");
				formatter % recv_data.recive_buffer1 % recv_data.recive_buffer_max_size % datadump;
				putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
			}
			/*------DEBUG LOG END------*/

			//使用中データバッファにリクエストデータをサイズ分追加設定する
			memcpy(recv_data.recive_buffer + recv_data.recive_buffer_max_size - recv_data.recive_buffer_rest_size,
			        recvbuffer.data(), recvlen);
			//データバッファ残サイズからリクエストデータサイズを減算する
			recv_data.recive_buffer_rest_size -= recvlen;
			//未送信データサイズに「未送信データサイズ　＋　リクエストデータサイズ」を設定する
			data_remain_size += recvlen;
		}

		it = recv_data.send_status_list.begin();
		it_end = recv_data.send_status_list.end();

		//リクエストデータ残サイズにリクエストデータサイズを設定する
		request_data_remain_size = recvlen;

		for (; it != it_end; ++it)
		{
			//送信状態が送信継続の場合
			if (it->status == SEND_CONTINUE)
			{
				//送信データ残サイズ　＞　リクエストデータ残サイズ
				if (it->send_rest_size > request_data_remain_size)
				{
					//送信可能データサイズにリクエストデータ残サイズを設定する
					it->send_possible_size = request_data_remain_size;
					//送信データ残サイズからリクエストデータ残サイズを減算する
					it->send_rest_size -= request_data_remain_size;
					//送信済データサイズに0を設定する
					it->send_end_size = 0;
					//リクエストデータ残サイズに0を設定する
					request_data_remain_size = 0;
				}
				//送信データ残サイズ　≦　リクエストデータ残サイズ
				else
				{
					//送信可能データサイズに送信データ残サイズを設定する
					it->send_possible_size = it->send_rest_size;
					//リクエストデータ残サイズから送信データ残サイズを減算する
					request_data_remain_size -= it->send_rest_size;
					//送信済データサイズに0を設定する
					it->send_end_size = 0;
					//送信データ残サイズに0を設定する
					it->send_rest_size = 0;
				}
				//編集区分に編集なしを設定する
				it->edit_division = EDIT_DIVISION_NO_EDIT;
				//送信状態に送信待を設定する
				it->status = SEND_OK;
			}
			//送信状態が送信不可の場合
			else if (it->status == SEND_NG)
			{
				//HTTPメソッドの妥当性をチェックする
				check_result = check_status_code(recv_data.recive_buffer + it->send_offset, data_remain_size);
				//チェック結果が正常の場合
				if (check_result == CHECK_OK)
				{
					//HTTPバージョンの妥当性をチェックする
					check_result = check_http_version(recv_data.recive_buffer + it->send_offset, data_remain_size);
				}
				//チェック結果が正常の場合
				if (check_result == CHECK_OK)
				{
					//送信可能データ内のHTTPヘッダの開始位置、サイズを取得する
					bret = find_http_header(recv_data.recive_buffer + it->send_offset, data_remain_size, http_header,
					        header_offset, header_offset_len);
					//検索結果が該当ありの場合
					if (bret)
					{
						//HTTPヘッダ（ContentLength）を検索する
						bret = find_http_header(recv_data.recive_buffer + it->send_offset, data_remain_size,
						        content_header, header_offset, count_length_header_value);
						//検索結果が該当ありの場合
						if (bret)
						{
							str_value.assign(recv_data.recive_buffer + it->send_offset + header_offset,
							        count_length_header_value);
							value = boost::lexical_cast<size_t>(str_value.c_str());
							//送信データ残サイズに
							//「HTTPヘッダサイズ　＋　ContentLength」を設定する
							it->send_rest_size = header_offset_len + value;
						}
						//検索結果が該当なしの場合
						else
						{
							//送信データ残サイズに未送信データサイズ + リクエストデータ残サイズを設定する
							it->send_rest_size = it->unsend_size + request_data_remain_size;
						}
						//編集区分に編集ありを設定する
						it->edit_division = EDIT_DIVISION_EDIT;
					}
					//検索結果が該当なしの場合
					else
					{
						//未送信データサイズにリクエストデータ残サイズを加算する
						data_remain_size += request_data_remain_size;
						it->unsend_size = data_remain_size;
						//リクエストデータ残サイズに0を設定する
						request_data_remain_size = 0;
						//繰り返し処理を終了する
						break;
					}
				}
				//チェック結果が異常の場合
				else if (check_result == CHECK_NG)
				{
					//編集区分に編集なしを設定する
					it->edit_division = EDIT_DIVISION_NO_EDIT;
					//送信データ残サイズにリクエストデータ残サイズを設定する
					it->send_rest_size = request_data_remain_size;
				}
				//チェック結果が判定不能の場合
				//check_result == CHECK_INPOSSIBLE
				else
				{
					//未送信データサイズにリクエストデータ残サイズを加算する
					data_remain_size += request_data_remain_size;
					it->unsend_size = data_remain_size;
					//リクエストデータ残サイズに0を設定する
					request_data_remain_size = 0;
					//繰り返し処理を終了する
					break;
				}

				//送信データ残サイズ　＞　未送信データサイズ+リクエストデータ残サイズ
				if (it->send_rest_size > data_remain_size + request_data_remain_size)
				{
					//送信可能データサイズに未送信データサイズ＋リクエストデータ残サイズを設定する
					it->send_possible_size = data_remain_size + request_data_remain_size;
					//送信データ残サイズから未送信データサイズ　＋　リクエストデータ残サイズを減算する
					it->send_rest_size -= (data_remain_size + request_data_remain_size);
					//送信済データサイズに0を設定する
					it->send_end_size = 0;
					//未送信データサイズに0を設定する
					it->unsend_size = 0;
					//リクエストデータ残サイズに0を設定する
					request_data_remain_size = 0;
				}
				//送信データ残サイズ　≦　未送信データサイズ　＋　リクエストデータ残サイズ
				else
				{
					//送信可能データサイズに送信データ残サイズを設定する
					it->send_possible_size = it->send_rest_size;
					//リクエストデータ残サイズに
					//「未送信データサイズ＋リクエストデータ残サイズ－送信データ残サイズ」を設定する
					request_data_remain_size = data_remain_size + request_data_remain_size - it->send_rest_size;
					//送信済データサイズに0を設定する
					it->send_end_size = 0;
					//未送信データサイズに0を設定する
					it->unsend_size = 0;
					//送信データ残サイズに0を設定する
					it->send_rest_size = 0;
				}

				//送信状態に送信待を設定する
				it->status = SEND_OK;
			}
			//リクエストデータ残サイズ　≦　0
			if (request_data_remain_size <= 0)
			{
				break;
			}
		}

		//リクエストデータ残サイズ　＞　0
		while (request_data_remain_size > 0)
		{
			//送信状態を生成する
			send_status new_send_state;
			//送信状態に送信不可を設定する
			new_send_state.status = SEND_NG;
			//送信状態リストに送信状態を設定する
			recv_data.send_status_list.push_back(new_send_state);
			std::list<send_status>::reverse_iterator new_send_it = recv_data.send_status_list.rbegin();
			//送信データオフセットを設定する
			new_send_it->send_offset = recv_data.recive_buffer_max_size - recv_data.recive_buffer_rest_size
			        - request_data_remain_size;

			//HTTPメソッドの妥当性をチェックする
			check_result = check_status_code(recv_data.recive_buffer + new_send_it->send_offset,
			        request_data_remain_size);
			//チェック結果が正常の場合
			if (check_result == CHECK_OK)
			{
				//HTTPバージョンの妥当性をチェックする
				check_result = check_http_version(recv_data.recive_buffer + new_send_it->send_offset,
				        request_data_remain_size);
			}
			//チェック結果が正常の場合
			if (check_result == CHECK_OK)
			{
				//送信可能データ内のHTTPヘッダの開始位置、サイズを取得する
				bret = find_http_header(recv_data.recive_buffer + new_send_it->send_offset, request_data_remain_size,
				        http_header, header_offset, header_offset_len);
				//検索結果が該当ありの場合
				if (bret)
				{
					//HTTPヘッダ（ContentLength）を検索する
					bret = find_http_header(recv_data.recive_buffer + new_send_it->send_offset,
					        request_data_remain_size, content_header, header_offset, count_length_header_value);

					//検索結果が該当ありの場合
					if (bret)
					{
						str_value.assign(recv_data.recive_buffer + it->send_offset + header_offset,
						        count_length_header_value);
						value = boost::lexical_cast<size_t>(str_value.c_str());
						//送信データ残サイズに
						//「HTTPヘッダサイズ　＋　ContentLength」を設定する
						new_send_it->send_rest_size = header_offset_len + value;
					}
					//検索結果が該当なしの場合
					else
					{
						//送信データ残サイズにリクエストデータ残サイズを設定する
						new_send_it->send_rest_size = request_data_remain_size;
					}
					//編集区分に編集ありを設定する
					new_send_it->edit_division = EDIT_DIVISION_EDIT;
				}
				//検索結果が該当なしの場合
				else
				{
					//未送信データサイズにリクエストデータ残サイズを設定する
					new_send_it->unsend_size = request_data_remain_size;
					//リクエストデータ残サイズに0を設定する
					request_data_remain_size = 0;
					//繰り返し処理を終了する
					break;
				}
			}
			//チェック結果が異常の場合
			else if (check_result == CHECK_NG)
			{
				//編集区分に編集なしを設定する
				new_send_it->edit_division = EDIT_DIVISION_NO_EDIT;
				//送信データ残サイズにリクエストデータ残サイズを設定する
				new_send_it->send_rest_size = request_data_remain_size;
			}
			//チェック結果が判定不能の場合
			//check_result == CHECK_INPOSSIBLE
			else
			{
				//未送信データサイズにリクエストデータ残サイズを加算する
				new_send_it->unsend_size = request_data_remain_size;
				//リクエストデータ残サイズに0を設定する
				request_data_remain_size = 0;
				//繰り返し処理を終了する
				break;
			}

			//送信データ残サイズ　＞　リクエストデータ残サイズ
			if (new_send_it->send_rest_size > request_data_remain_size)
			{
				//送信可能データサイズにリクエストデータ残サイズを設定する
				new_send_it->send_possible_size = request_data_remain_size;
				//送信データ残サイズからリクエストデータ残サイズを減算する
				new_send_it->send_rest_size -= request_data_remain_size;
				//送信済データサイズに0を設定する
				new_send_it->send_end_size = 0;
				//リクエストデータ残サイズに0を設定する
				request_data_remain_size = 0;
			}
			//送信データ残サイズ　≦　リクエストデータ残サイズ
			else
			{
				//送信可能データサイズに送信データ残サイズを設定する
				new_send_it->send_possible_size = new_send_it->send_rest_size;
				//リクエストデータ残サイズから送信データ残サイズを減算する
				request_data_remain_size -= new_send_it->send_rest_size;
				//送信済データサイズに0を設定する
				new_send_it->send_end_size = 0;
				//送信データ残サイズに0を設定する
				new_send_it->send_rest_size = 0;
			}

			//送信状態に送信待を設定する
			new_send_it->status = SEND_OK;

		}

		//送信可能データありをチェック
		send_status_it it_find = find_if(recv_data.send_status_list.begin(), recv_data.send_status_list.end(),
		        data_send_possible());
		//送信可能データあり
		if (it_find != recv_data.send_status_list.end())
		{
			//遷移先ステータスを設定する
			status = CLIENT_CONNECTION_CHECK;
		}
		//送信可能データなし
		else
		{
			//遷移先ステータスを設定する
			status = REALSERVER_RECV;
		}
	} catch (const std::bad_alloc&)
	{
		std::cerr << "handle_realserver_recv exception: Could not allocate memory." << std::endl;
		putLogError(17000, "Could not allocate memory.", __FILE__, __LINE__ );
		status = FINALIZE;
	} catch (const std::exception& ex)
	{
		std::cerr << "handle_realserver_recv exception: error=%s" << ex.what() << "." << std::endl;
		boost::format formatter("function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_realserver_recv() exception: result=%d, error=%s.");
		formatter % FINALIZE % ex.what();
		putLogError(17000, formatter.str(), __FILE__, __LINE__ );

		status = FINALIZE;
	} catch (...)
	{
		std::cerr << "Unkown exception." << std::endl;
		putLogError(17000, "function protocol_module_sessionless::check_message_result "
			"protocol_module_sessionless::handle_realserver_recv() : "
			"Unkown exception.", __FILE__, __LINE__ );
		status = FINALIZE;
	}

	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		boost::format formatter("out_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_realserver_recv( const boost::thread::id thread_id,"
			"const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,"
			"const size_t recvlen ): return_value=%d");
		formatter % status;
		putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
	}
	/*------DEBUG LOG END------*/

	return status;
}

/*!
 *handle_realserver_recv
 *
 *@param [in] thread_id
 *@param [in] rs_endpoint
 *@param [in] recvbuffer
 *@param [in] recvlen
 */
protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::handle_realserver_recv(
        const boost::thread::id thread_id, const boost::asio::ip::udp::endpoint& rs_endpoint, const boost::array<char,
                MAX_BUFFER_SIZE>& recvbuffer, const size_t recvlen)
{
	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		boost::format formatter("in/out_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_realserver_recv( const boost::thread::id thread_id,"
			"const boost::asio::ip::udp::endpoint& rs_endpoint,"
			"const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,"
			"const size_t recvlen): "
			"return_value=%d.");
		formatter % STOP;
		putLogDebug(10000, formatter.str(), __FILE__, __LINE__ );
	}
	/*------DEBUG LOG END------*/
	return STOP;
}

/*!
 *handle_sorryserver_recv
 *
 *@param [in] thread_id
 *@param [in] sorry_endpoint
 *@param [in] recvbuffer
 *@param [in] recvlen
 */
protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::handle_sorryserver_recv(
        const boost::thread::id thread_id, const boost::asio::ip::tcp::endpoint& sorry_endpoint, const boost::array<
                char, MAX_BUFFER_SIZE>& recvbuffer, const size_t recvlen)
{
	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		boost::format formatter("in_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_sorryserver_recv( const boost::thread::id thread_id,"
			"const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,"
			"const size_t recvlen ): thread_id=%d, recvbuffer=%s, recvlen=%d");
		formatter % thread_id % recvbuffer.data() % recvlen;
		putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
	}
	/*------DEBUG LOG END------*/

	EVENT_TAG status;
	typedef std::vector<protocol_module_sessionless::send_status_it> send_end_it_vec;
	send_end_it_vec send_end_vec;
	size_t data_remain_start = 0; //未送信データ開始位置
	size_t data_remain_size = 0; //未送信データサイズ
	size_t request_data_remain_size = 0; //リクエストデータ残サイズ
	bool bret = false;
	CHECK_RESULT_TAG check_result; //チェック結果
	size_t header_offset = 0;
	size_t header_offset_len = 0;
	size_t count_length_header_value = 0;
	size_t value = 0;
	std::string str_value;
	const std::string http_header = "";
	const std::string content_header = "Content-Length";
	session_thread_data_sessionless* session_data = NULL;

	char* buffer1 = NULL;
	char* buffer2 = NULL;

	//パラメータチェック
	if (recvlen > recvbuffer.size())
	{
		putLogError(17000, "Data size bigger than buffer size.", __FILE__,
		__LINE__ );
		/*-------- DEBUG LOG --------*/
		if (LOG_LV_DEBUG == getloglevel())
		{
			boost::format formatter(
			        "out_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
				        "handle_sorryserver_recv( const boost::thread::id thread_id,"
				        "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,"
				        "const size_t recvlen ): return_value=%d");
			formatter % FINALIZE;
			putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
		}
		/*------DEBUG LOG END------*/
		return FINALIZE;
	}

	try
	{
		{
			boost::mutex::scoped_lock slock(session_thread_data_map_mutex);

			if (session_thread_data_map.find(thread_id) == session_thread_data_map.end())
			{
				putLogError(17000, "Invalid thread id.", __FILE__, __LINE__ );
				/*-------- DEBUG LOG --------*/
				if (LOG_LV_DEBUG == getloglevel())
				{
					boost::format formatter(
					        "out_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
						        "handle_sorryserver_recv( const boost::thread::id thread_id,"
						        "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,"
						        "const size_t recvlen ): return_value=%d");
					formatter % FINALIZE;
					putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
				}
				/*------DEBUG LOG END------*/

				return FINALIZE;
			}

			session_data = session_thread_data_map[thread_id];
			if (session_data == NULL)
			{
				putLogError(17000, "Invalid thread id.", __FILE__, __LINE__ );
				/*-------- DEBUG LOG --------*/
				if (LOG_LV_DEBUG == getloglevel())
				{
					boost::format formatter(
					        "out_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
						        "handle_sorryserver_recv( const boost::thread::id thread_id,"
						        "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,"
						        "const size_t recvlen ): return_value=%d");
					formatter % FINALIZE;
					putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
				}
				/*------DEBUG LOG END------*/
				return FINALIZE;
			}
		}

		//終了フラグがONの場合
		if (session_data->end_flag == END_FLAG_ON)
		{
			/*-------- DEBUG LOG --------*/
			if (LOG_LV_DEBUG == getloglevel())
			{
				boost::format formatter(
				        "out_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
					        "handle_sorryserver_recv( const boost::thread::id thread_id,"
					        "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,"
					        "const size_t recvlen ): return_value=%d");
				formatter % CLIENT_RECV;
				putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
			}
			/*-------- DEBUG LOG --------*/
			return CLIENT_RECV;
		}

		//終了フラグがOFFの場合
		if (session_data->recive_data_map.find(session_data->target_endpoint) == session_data->recive_data_map.end())
		{
			putLogError(17000, "Invalid endpoint.", __FILE__, __LINE__ );
			/*-------- DEBUG LOG --------*/
			if (LOG_LV_DEBUG == getloglevel())
			{
				boost::format formatter(
				        "out_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
					        "handle_sorryserver_recv( const boost::thread::id thread_id,"
					        "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,"
					        "const size_t recvlen ): return_value=%d");
				formatter % FINALIZE;
				putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
			}
			/*------DEBUG LOG END------*/
			return FINALIZE;
		}

		recive_data& recv_data = session_data->recive_data_map[session_data->target_endpoint];

		send_status_it it = recv_data.send_status_list.begin();
		send_status_it it_end = recv_data.send_status_list.end();

		it = std::find_if(it, it_end, data_send_ok());
		if (it != it_end)
		{
			putLogError(17000, "sending data is invalid.", __FILE__, __LINE__ );
			/*-------- DEBUG LOG --------*/
			if (LOG_LV_DEBUG == getloglevel())
			{
				boost::format formatter(
				        "out_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
					        "handle_sorryserver_recv( const boost::thread::id thread_id,"
					        "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,"
					        "const size_t recvlen ): return_value=%d");
				formatter % FINALIZE;
				putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
			}
			/*------DEBUG LOG END------*/
			return FINALIZE;
		}

		it = recv_data.send_status_list.begin();
		it = std::adjacent_find(it, it_end, data_send_repeated());
		if (it != it_end)
		{
			putLogError(17000, "sending data is invalid..", __FILE__, __LINE__ );
			/*-------- DEBUG LOG --------*/
			if (LOG_LV_DEBUG == getloglevel())
			{
				boost::format formatter(
				        "out_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
					        "handle_sorryserver_recv( const boost::thread::id thread_id,"
					        "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,"
					        "const size_t recvlen ): return_value=%d");
				formatter % FINALIZE;
				putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
			}
			/*------DEBUG LOG END------*/
			return FINALIZE;
		}

		it = recv_data.send_status_list.begin();
		//送信状態リストの要素数分繰り返し
		for (; it != it_end; ++it)
		{
			//送信状態が送信済の場合
			if (it->status == SEND_END)
			{
				//送信状態リストから削除する
				send_end_vec.push_back(it);
			}
			else if (it->status == SEND_CONTINUE) //送信状態が送信継続の場合
			{
				//送信データオフセットに送信済サイズを加算する
				it->send_offset += it->send_end_size;
				//未送信データ開始位置に送信データオフセットを設定する
				data_remain_start = it->send_offset;
				//繰り返し処理を終了する
				break;
			}
			else
			{ //送信状態が送信済以外の場合
				//未送信データ開始位置に送信データオフセットを設定する
				data_remain_start = it->send_offset;
				//未送信データサイズに未送信データサイズを設定する
				data_remain_size = it->unsend_size;
				//繰り返し処理を終了する
				break;
			}
		}

		for (send_end_it_vec::iterator it = send_end_vec.begin(); it != send_end_vec.end(); ++it)
		{
			//送信状態リストから削除する
			recv_data.send_status_list.erase(*it);
		}

		//データバッファ残サイズ　＜　リクエストデータサイズ
		if (recv_data.recive_buffer_rest_size < recvlen)
		{
			//データバッファMAXサイズ　＜　未送信データサイズ　＋　リクエストデータサイズ
			if (recv_data.recive_buffer_max_size < data_remain_size + recvlen)
			{
				if (recv_data.recive_buffer == NULL)
				{
					//データバッファ１をMAX_BUFFER_SIZE分、
					//新規に確保し、初期化する
					recv_data.recive_buffer1 = new char[MAX_BUFFER_SIZE];
					/*-------- DEBUG LOG --------*/
					if (LOG_LV_DEBUG == getloglevel())
					{
						boost::format formatter("new: address=&(%d), size=%lu");
						formatter % &recv_data.recive_buffer1 % (MAX_BUFFER_SIZE);
						putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
					}
					/*-------- DEBUG LOG --------*/
					memset(recv_data.recive_buffer1, 0, MAX_BUFFER_SIZE);
					//データバッファ２をMAX_BUFFER_SIZE分、
					//新規に確保し、初期化する
					recv_data.recive_buffer2 = new char[MAX_BUFFER_SIZE];
					/*-------- DEBUG LOG --------*/
					if (LOG_LV_DEBUG == getloglevel())
					{
						boost::format formatter("new: address=&(%d), size=%lu");
						formatter % &recv_data.recive_buffer2 % (MAX_BUFFER_SIZE);
						putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
					}
					/*-------- DEBUG LOG --------*/
					memset(recv_data.recive_buffer2, 0, MAX_BUFFER_SIZE);
					recv_data.recive_buffer = recv_data.recive_buffer1;
					//データバッファMAXサイズにMAX_BUFFER_SIZEを設定する
					recv_data.recive_buffer_max_size = MAX_BUFFER_SIZE;
				}
				else
				{
					//データバッファ１を「未送信データサイズ　＋　リクエストデータサイズ」分、
					//新規に確保し、初期化する
					buffer1 = new char[data_remain_size + recvlen];
					/*-------- DEBUG LOG --------*/
					if (LOG_LV_DEBUG == getloglevel())
					{
						boost::format formatter("new: address=&(%d), size=%lu");
						formatter % &buffer1 % (data_remain_size + recvlen);
						putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
					}
					/*-------- DEBUG LOG --------*/
					memset(buffer1, 0, data_remain_size + recvlen);
					//データバッファ２を「未送信データサイズ　＋　リクエストデータサイズ」分、
					//新規に確保し、初期化する
					buffer2 = new char[data_remain_size + recvlen];
					/*-------- DEBUG LOG --------*/
					if (LOG_LV_DEBUG == getloglevel())
					{
						boost::format formatter("new: address=&(%d), size=%lu");
						formatter % &buffer2 % (data_remain_size + recvlen);
						putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
					}
					/*-------- DEBUG LOG --------*/
					memset(buffer2, 0, data_remain_size + recvlen);

					/*-------- DEBUG LOG --------*/
					if (LOG_LV_DEBUG == getloglevel())
					{
						boost::format formatter("%02x");
						for (size_t i = 0; i < data_remain_size + recvlen; ++i)
						{
							formatter % static_cast<unsigned short> (static_cast<unsigned char> (buffer1[i]));
						}

						std::string datadump = formatter.str();
						formatter.parse(
						        "function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
							        "handle_sorryserver_recv( ): before memcpy (data dump):  "
							        "data begin(%d), data_size=%d,data=%s.");
						formatter % buffer1 % (data_remain_size + recvlen) % datadump;
						putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
					}
					/*------DEBUG LOG END------*/
					//データバッファ１に使用中データバッファの未送信データ開始位置から未送信データサイズ分設定する
					memcpy(buffer1, recv_data.recive_buffer + data_remain_start, data_remain_size);
					//データバッファ１にリクエストデータをサイズ分追加設定する
					memcpy(buffer1 + data_remain_size, recvbuffer.data(), recvlen);
					/*-------- DEBUG LOG --------*/
					if (LOG_LV_DEBUG == getloglevel())
					{
						boost::format formatter("%02x");
						for (size_t i = 0; i < data_remain_size + recvlen; ++i)
						{
							formatter % static_cast<unsigned short> (static_cast<unsigned char> (buffer2[i]));
						}

						std::string datadump = formatter.str();
						formatter.parse(
						        "function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
							        "handle_sorryserver_recv( ): after memcpy (data dump):  "
							        "data begin(%d), data_size=%d,data=%s.");
						formatter % buffer2 % (data_remain_size + recvlen) % datadump;
						putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
					}
					/*------DEBUG LOG END------*/
					//旧データバッファ１、旧データバッファ２を解放する
					if (recv_data.recive_buffer1 != NULL)
					{
						/*-------- DEBUG LOG --------*/
						if (LOG_LV_DEBUG == getloglevel())
						{
							boost::format formatter("delete: address=&(%d)");
							formatter % recv_data.recive_buffer1;
							putLogDebug(10005, formatter.str(), __FILE__,
							__LINE__ );
						}
						/*------DEBUG LOG END------*/
						delete[] recv_data.recive_buffer1;
						recv_data.recive_buffer1 = NULL;
					}

					if (recv_data.recive_buffer2 != NULL)
					{
						/*-------- DEBUG LOG --------*/
						if (LOG_LV_DEBUG == getloglevel())
						{
							boost::format formatter("delete: address=&(%d)");
							formatter % recv_data.recive_buffer2;
							putLogDebug(10005, formatter.str(), __FILE__,
							__LINE__ );
						}
						/*------DEBUG LOG END------*/
						delete[] recv_data.recive_buffer2;
						recv_data.recive_buffer2 = NULL;
					}

					//使用中データバッファにデータバッファ１を設定する
					recv_data.recive_buffer1 = buffer1;
					recv_data.recive_buffer2 = buffer2;
					recv_data.recive_buffer = recv_data.recive_buffer1;
					//データバッファMAXサイズに「未送信データサイズ　＋　リクエストデータサイズ」を設定する
					recv_data.recive_buffer_max_size = data_remain_size + recvlen;
				}
			}
			//データバッファMAXサイズ　≧　未送信データサイズ　＋　リクエストデータサイズ
			else
			{
				if (recv_data.recive_buffer1 == NULL || recv_data.recive_buffer2 == NULL)
				{
					putLogError(17000, "Invalid pointer.", __FILE__, __LINE__ );
					/*-------- DEBUG LOG --------*/
					if (LOG_LV_DEBUG == getloglevel())
					{
						boost::format formatter(
						        "out_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
							        "handle_sorryserver_recv( const boost::thread::id thread_id,"
							        "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,"
							        "const size_t recvlen ): return_value=%d");
						formatter % FINALIZE;
						putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
					}
					/*------DEBUG LOG END------*/
					return FINALIZE;
				}
				//使用中データバッファがデータバッファ１の場合
				if (recv_data.recive_buffer == recv_data.recive_buffer1)
				{
					//データバッファ２を初期化する
					memset(recv_data.recive_buffer2, 0, recv_data.recive_buffer_max_size);
					/*-------- DEBUG LOG --------*/
					if (LOG_LV_DEBUG == getloglevel())
					{
						boost::format formatter("%02x");
						for (size_t i = 0; i < recv_data.recive_buffer_max_size; ++i)
						{
							formatter
							        % static_cast<unsigned short> (static_cast<unsigned char> (recv_data.recive_buffer2[i]));
						}

						std::string datadump = formatter.str();
						formatter.parse(
						        "function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
							        "handle_sorryserver_recv( ): before memcpy (data dump):  "
							        "data begin(%d), data_size=%d,data=%s.");
						formatter % recv_data.recive_buffer2 % recv_data.recive_buffer_max_size % datadump;
						putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
					}
					/*------DEBUG LOG END------*/
					//データバッファ２に使用中データバッファの未送信データ開始位置から未送信データサイズ分設定する
					memcpy(recv_data.recive_buffer2, recv_data.recive_buffer + data_remain_start, data_remain_size);
					//データバッファ２にリクエストデータをサイズ分追加設定する
					memcpy(recv_data.recive_buffer2 + data_remain_size, recvbuffer.data(), recvlen);
					/*-------- DEBUG LOG --------*/
					if (LOG_LV_DEBUG == getloglevel())
					{
						boost::format formatter("%02x");
						for (size_t i = 0; i < recv_data.recive_buffer_max_size; ++i)
						{
							formatter
							        % static_cast<unsigned short> (static_cast<unsigned char> (recv_data.recive_buffer2[i]));
						}

						std::string datadump = formatter.str();
						formatter.parse(
						        "function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
							        "handle_sorryserver_recv( ): after memcpy (data dump):  "
							        "data begin(%d), data_size=%d,data=%s.");
						formatter % recv_data.recive_buffer2 % recv_data.recive_buffer_max_size % datadump;
						putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
					}
					/*------DEBUG LOG END------*/
					//使用中データバッファにデータバッファ２を設定する
					recv_data.recive_buffer = recv_data.recive_buffer2;
				}
				//使用中データバッファがデータバッファ２の場合
				else
				{
					//データバッファ１を初期化する
					memset(recv_data.recive_buffer1, 0, recv_data.recive_buffer_max_size);
					/*-------- DEBUG LOG --------*/
					if (LOG_LV_DEBUG == getloglevel())
					{
						boost::format formatter("%02x");
						for (size_t i = 0; i < recv_data.recive_buffer_max_size; ++i)
						{
							formatter
							        % static_cast<unsigned short> (static_cast<unsigned char> (recv_data.recive_buffer1[i]));
						}

						std::string datadump = formatter.str();
						formatter.parse(
						        "function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
							        "handle_sorryserver_recv( ): before memcpy (data dump):  "
							        "data begin(%d), data_size=%d,data=%s.");
						formatter % recv_data.recive_buffer1 % recv_data.recive_buffer_max_size % datadump;
						putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
					}
					/*------DEBUG LOG END------*/
					//データバッファ１に使用中データバッファの未送信データ開始位置から未送信データサイズ分設定する
					memcpy(recv_data.recive_buffer1, recv_data.recive_buffer + data_remain_start, data_remain_size);
					//データバッファ１にリクエストデータをサイズ分追加設定する
					memcpy(recv_data.recive_buffer1 + data_remain_size, recvbuffer.data(), recvlen);
					/*-------- DEBUG LOG --------*/
					if (LOG_LV_DEBUG == getloglevel())
					{
						boost::format formatter("%02x");
						for (size_t i = 0; i < recv_data.recive_buffer_max_size; ++i)
						{
							formatter
							        % static_cast<unsigned short> (static_cast<unsigned char> (recv_data.recive_buffer1[i]));
						}

						std::string datadump = formatter.str();
						formatter.parse(
						        "function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
							        "handle_sorryserver_recv( ): after memcpy (data dump):  "
							        "data begin(%d), data_size=%d,data=%s.");
						formatter % recv_data.recive_buffer1 % recv_data.recive_buffer_max_size % datadump;
						putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
					}
					/*------DEBUG LOG END------*/
					//使用中データバッファにデータバッファ１を設定する
					recv_data.recive_buffer = recv_data.recive_buffer1;
				}
			}

			//データバッファ残サイズに
			//「データバッファMAXサイズ　－　未送信データサイズ　－　リクエストデータサイズ」を設定する
			recv_data.recive_buffer_rest_size = recv_data.recive_buffer_max_size - data_remain_size - recvlen;

			//未送信データサイズに「未送信データサイズ　＋　リクエストデータサイズ」を設定する
			data_remain_size += recvlen;

			send_status_it it_begin = recv_data.send_status_list.begin();
			send_status_it it_end = recv_data.send_status_list.end();

			for (; it_begin != it_end; ++it_begin)
			{
				//送信データオフセットから未送信データ開始位置を減算
				it->send_offset -= data_remain_start;
			}
		}
		//データバッファ残サイズ　≧　リクエストデータサイズ
		else
		{
			if (recv_data.recive_buffer == NULL)
			{
				putLogError(17000, "Invalid thread id.", __FILE__, __LINE__ );
				/*-------- DEBUG LOG --------*/
				if (LOG_LV_DEBUG == getloglevel())
				{
					boost::format formatter(
					        "out_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
						        "handle_sorryserver_recv( const boost::thread::id thread_id,"
						        "const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,"
						        "const size_t recvlen ): return_value=%d");
					formatter % FINALIZE;
					putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
				}
				/*------DEBUG LOG END------*/
				return FINALIZE;
			}
			/*-------- DEBUG LOG --------*/
			if (LOG_LV_DEBUG == getloglevel())
			{
				boost::format formatter("%02x");
				for (size_t i = 0; i < recv_data.recive_buffer_max_size; ++i)
				{
					formatter % static_cast<unsigned short> (static_cast<unsigned char> (recv_data.recive_buffer1[i]));
				}

				std::string datadump = formatter.str();
				formatter.parse("function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
					"handle_sorryserver_recv( ): before memcpy (data dump):  "
					"data begin(%d), data_size=%d,data=%s.");
				formatter % recv_data.recive_buffer1 % recv_data.recive_buffer_max_size % datadump;
				putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
			}
			/*------DEBUG LOG END------*/

			//使用中データバッファにリクエストデータをサイズ分追加設定する
			memcpy(recv_data.recive_buffer + recv_data.recive_buffer_max_size - recv_data.recive_buffer_rest_size,
			        recvbuffer.data(), recvlen);
			//データバッファ残サイズからリクエストデータサイズを減算する
			recv_data.recive_buffer_rest_size -= recvlen;
			//未送信データサイズに「未送信データサイズ　＋　リクエストデータサイズ」を設定する
			data_remain_size += recvlen;
		}

		it = recv_data.send_status_list.begin();
		it_end = recv_data.send_status_list.end();

		//リクエストデータ残サイズにリクエストデータサイズを設定する
		request_data_remain_size = recvlen;

		for (; it != it_end; ++it)
		{
			//送信状態が送信継続の場合
			if (it->status == SEND_CONTINUE)
			{
				//送信データ残サイズ　＞　リクエストデータ残サイズ
				if (it->send_rest_size > request_data_remain_size)
				{
					//送信可能データサイズにリクエストデータ残サイズを設定する
					it->send_possible_size = request_data_remain_size;
					//送信データ残サイズからリクエストデータ残サイズを減算する
					it->send_rest_size -= request_data_remain_size;
					//送信済データサイズに0を設定する
					it->send_end_size = 0;
					//リクエストデータ残サイズに0を設定する
					request_data_remain_size = 0;
				}
				//送信データ残サイズ　≦　リクエストデータ残サイズ
				else
				{
					//送信可能データサイズに送信データ残サイズを設定する
					it->send_possible_size = it->send_rest_size;
					//リクエストデータ残サイズから送信データ残サイズを減算する
					request_data_remain_size -= it->send_rest_size;
					//送信済データサイズに0を設定する
					it->send_end_size = 0;
					//送信データ残サイズに0を設定する
					it->send_rest_size = 0;
				}
//				//編集区分に編集なしを設定する
//				it->edit_division = EDIT_DIVISION_NO_EDIT;
				//送信状態に送信待を設定する
				it->status = SEND_OK;
			}
			//送信状態が送信不可の場合
			else if (it->status == SEND_NG)
			{
				//HTTPメソッドの妥当性をチェックする
				check_result = check_status_code(recv_data.recive_buffer + it->send_offset, data_remain_size);
				//チェック結果が正常の場合
				if (check_result == CHECK_OK)
				{
					//HTTPバージョンの妥当性をチェックする
					check_result = check_http_version(recv_data.recive_buffer + it->send_offset, data_remain_size);
				}
				//チェック結果が正常の場合
				if (check_result == CHECK_OK)
				{
					//送信可能データ内のHTTPヘッダの開始位置、サイズを取得する
					bret = find_http_header(recv_data.recive_buffer + it->send_offset, data_remain_size, http_header,
					        header_offset, header_offset_len);
					//検索結果が該当ありの場合
					if (bret)
					{
						//HTTPヘッダ（ContentLength）を検索する
						bret = find_http_header(recv_data.recive_buffer + it->send_offset, data_remain_size,
						        content_header, header_offset, count_length_header_value);
						//検索結果が該当ありの場合
						if (bret)
						{
							str_value.assign(recv_data.recive_buffer + it->send_offset + header_offset,
							        count_length_header_value);
							value = boost::lexical_cast<size_t>(str_value.c_str());
							//送信データ残サイズに
							//「HTTPヘッダサイズ　＋　ContentLength」を設定する
							it->send_rest_size = header_offset_len + value;
						}
						//検索結果が該当なしの場合
						else
						{
							//送信データ残サイズに未送信データサイズ + リクエストデータ残サイズを設定する
							it->send_rest_size = it->unsend_size + request_data_remain_size;
						}
//						//編集区分に編集ありを設定する
//						it->edit_division = EDIT_DIVISION_EDIT;
					}
					//検索結果が該当なしの場合
					else
					{
						//未送信データサイズにリクエストデータ残サイズを加算する
						it->unsend_size += request_data_remain_size;
						//リクエストデータ残サイズに0を設定する
						request_data_remain_size = 0;
						//繰り返し処理を終了する
						break;
					}
				}
				//チェック結果が異常の場合
				else if (check_result == CHECK_NG)
				{
//					//編集区分に編集なしを設定する
//					it->edit_division = EDIT_DIVISION_NO_EDIT;
					//送信データ残サイズにリクエストデータ残サイズを設定する
					it->send_rest_size = request_data_remain_size;
				}
				//チェック結果が判定不能の場合
				//check_result == CHECK_INPOSSIBLE
				else
				{
					//未送信データサイズにリクエストデータ残サイズを加算する
					it->unsend_size += request_data_remain_size;
					//リクエストデータ残サイズに0を設定する
					request_data_remain_size = 0;
					//繰り返し処理を終了する
					break;
				}

				//送信データ残サイズ　＞　未送信データサイズ+リクエストデータ残サイズ
				if (it->send_rest_size > it->unsend_size + request_data_remain_size)
				{
					//送信可能データサイズに未送信データサイズ＋リクエストデータ残サイズを設定する
					it->send_possible_size = it->unsend_size + request_data_remain_size;
					//送信データ残サイズから未送信データサイズ　＋　リクエストデータ残サイズを減算する
					it->send_rest_size -= (it->unsend_size + request_data_remain_size);
					//送信済データサイズに0を設定する
					it->send_end_size = 0;
					//未送信データサイズに0を設定する
					it->unsend_size = 0;
					//リクエストデータ残サイズに0を設定する
					request_data_remain_size = 0;
				}
				//送信データ残サイズ　≦　未送信データサイズ　＋　リクエストデータ残サイズ
				else
				{
					//送信可能データサイズに送信データ残サイズを設定する
					it->send_possible_size = it->send_rest_size;
					//リクエストデータ残サイズに
					//「未送信データサイズ＋リクエストデータ残サイズ－送信データ残サイズ」を設定する
					request_data_remain_size = it->unsend_size + request_data_remain_size - it->send_rest_size;
					//送信済データサイズに0を設定する
					it->send_end_size = 0;
					//未送信データサイズに0を設定する
					it->unsend_size = 0;
					//送信データ残サイズに0を設定する
					it->send_rest_size = 0;
				}

				//送信状態に送信待を設定する
				it->status = SEND_OK;
			}
			//リクエストデータ残サイズ　≦　0
			if (request_data_remain_size <= 0)
			{
				break;
			}
		}

		//リクエストデータ残サイズ　＞　0
		while (request_data_remain_size > 0)
		{
			//送信状態を生成する
			send_status new_send_state;
			//送信状態に送信不可を設定する
			new_send_state.status = SEND_NG;
			//送信状態リストに送信状態を設定する
			recv_data.send_status_list.push_back(new_send_state);
			std::list<send_status>::reverse_iterator new_send_it = recv_data.send_status_list.rbegin();
			//送信データオフセットを設定する
			new_send_it->send_offset = recv_data.recive_buffer_max_size - recv_data.recive_buffer_rest_size
			        - request_data_remain_size;

			//HTTPメソッドの妥当性をチェックする
			check_result = check_status_code(recv_data.recive_buffer + new_send_it->send_offset,
			        request_data_remain_size);
			//チェック結果が正常の場合
			if (check_result == CHECK_OK)
			{
				//HTTPバージョンの妥当性をチェックする
				check_result = check_http_version(recv_data.recive_buffer + new_send_it->send_offset,
				        request_data_remain_size);
			}
			//チェック結果が正常の場合
			if (check_result == CHECK_OK)
			{
				//送信可能データ内のHTTPヘッダの開始位置、サイズを取得する
				bret = find_http_header(recv_data.recive_buffer + new_send_it->send_offset, request_data_remain_size,
				        http_header, header_offset, header_offset_len);
				//検索結果が該当ありの場合
				if (bret)
				{
					//HTTPヘッダ（ContentLength）を検索する
					bret = find_http_header(recv_data.recive_buffer + new_send_it->send_offset,
					        request_data_remain_size, content_header, header_offset, count_length_header_value);

					//検索結果が該当ありの場合
					if (bret)
					{
						str_value.assign(recv_data.recive_buffer + it->send_offset + header_offset,
						        count_length_header_value);
						value = boost::lexical_cast<size_t>(str_value.c_str());
						//送信データ残サイズに
						//「HTTPヘッダサイズ　＋　ContentLength」を設定する
						new_send_it->send_rest_size = header_offset_len + value;
					}
					//検索結果が該当なしの場合
					else
					{
						//送信データ残サイズにリクエストデータ残サイズを設定する
						new_send_it->send_rest_size = request_data_remain_size;
					}
//					//編集区分に編集ありを設定する
//					new_send_it->edit_division = EDIT_DIVISION_EDIT;
				}
				//検索結果が該当なしの場合
				else
				{
					//未送信データサイズにリクエストデータ残サイズを設定する
					new_send_it->unsend_size = request_data_remain_size;
					//リクエストデータ残サイズに0を設定する
					request_data_remain_size = 0;
					//繰り返し処理を終了する
					break;
				}
			}
			//チェック結果が異常の場合
			else if (check_result == CHECK_NG)
			{
//				//編集区分に編集なしを設定する
//				new_send_it->edit_division = EDIT_DIVISION_NO_EDIT;
				//送信データ残サイズにリクエストデータ残サイズを設定する
				new_send_it->send_rest_size = request_data_remain_size;
			}
			//チェック結果が判定不能の場合
			//check_result == CHECK_INPOSSIBLE
			else
			{
				//未送信データサイズにリクエストデータ残サイズを加算する
				new_send_it->unsend_size = request_data_remain_size;
				//リクエストデータ残サイズに0を設定する
				request_data_remain_size = 0;
				//繰り返し処理を終了する
				break;
			}

			//送信データ残サイズ　＞　リクエストデータ残サイズ
			if (new_send_it->send_rest_size > request_data_remain_size)
			{
				//送信可能データサイズにリクエストデータ残サイズを設定する
				new_send_it->send_possible_size = request_data_remain_size;
				//送信データ残サイズからリクエストデータ残サイズを減算する
				new_send_it->send_rest_size -= request_data_remain_size;
				//送信済データサイズに0を設定する
				new_send_it->send_end_size = 0;
				//リクエストデータ残サイズに0を設定する
				request_data_remain_size = 0;
			}
			//送信データ残サイズ　≦　リクエストデータ残サイズ
			else
			{
				//送信可能データサイズに送信データ残サイズを設定する
				new_send_it->send_possible_size = new_send_it->send_rest_size;
				//リクエストデータ残サイズから送信データ残サイズを減算する
				request_data_remain_size -= new_send_it->send_rest_size;
				//送信済データサイズに0を設定する
				new_send_it->send_end_size = 0;
				//送信データ残サイズに0を設定する
				new_send_it->send_rest_size = 0;
			}

			//送信状態に送信待を設定する
			new_send_it->status = SEND_OK;

		}

		//送信可能データありをチェック
		send_status_it it_find = find_if(recv_data.send_status_list.begin(), recv_data.send_status_list.end(),
		        data_send_possible());
		//送信可能データあり
		if (it_find != recv_data.send_status_list.end())
		{
			//遷移先ステータスを設定する
			status = CLIENT_CONNECTION_CHECK;
		}
		//送信可能データなし
		else
		{
			//遷移先ステータスを設定する
			status = SORRYSERVER_RECV;
		}
	} catch (const std::bad_alloc&)
	{
		std::cerr << "handle_sorryserver_recv exception: Could not allocate memory." << std::endl;
		putLogError(17000, "Could not allocate memory.", __FILE__, __LINE__ );
		status = FINALIZE;
	} catch (const std::exception& ex)
	{
		std::cerr << "handle_sorryserver_recv exception: error=%s" << ex.what() << "." << std::endl;
		boost::format formatter("function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_sorryserver_recv() exception: result=%d, error=%s.");
		formatter % FINALIZE % ex.what();
		putLogError(17000, formatter.str(), __FILE__, __LINE__ );

		status = FINALIZE;
	} catch (...)
	{
		std::cerr << "Unkown exception." << std::endl;
		putLogError(17000, "function protocol_module_sessionless::check_message_result "
			"protocol_module_sessionless::handle_sorryserver_recv() : "
			"Unkown exception.", __FILE__, __LINE__ );
		status = FINALIZE;
	}

	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		boost::format formatter("out_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_sorryserver_recv( const boost::thread::id thread_id,"
			"const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,"
			"const size_t recvlen ): return_value=%d");
		formatter % status;
		putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
	}
	/*------DEBUG LOG END------*/

	return status;
}

/*!
 *handle_response_send_inform
 *
 *@param [in] thread_id
 */
protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::handle_response_send_inform(
        const boost::thread::id thread_id)
{
	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		boost::format formatter("in/out_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_response_send_inform( const boost::thread::id thread_id):"
			"const boost::asio::ip::udp::endpoint& rs_endpoint,"
			"const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,"
			"const size_t recvlen): "
			"return_value=%d.");
		formatter % STOP;
		putLogDebug(10000, formatter.str(), __FILE__, __LINE__ );
	}
	/*------DEBUG LOG END------*/
	return STOP;
}

/*!
 *handle_client_connection_check
 *
 *@param [in] thread_id
 *@param [out]sendbuffer
 *@param [out]datalen
 */
protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::handle_client_connection_check(
        const boost::thread::id thread_id, boost::array<char, MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen)
{

	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		boost::format formatter("in_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_client_connection_check( const boost::thread::id thread_id, "
			"boost::array<char, MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen): "
			"thread_id=%d, sendbuffer=%d, datalen=%d");
		formatter % thread_id % sendbuffer.data() % datalen;
		putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
	}
	/*------DEBUG LOG END------*/
	EVENT_TAG status;
	size_t send_buffer_size = sendbuffer.max_size();
	session_thread_data_sessionless* session_data = NULL;

	try
	{
		{
			boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
			if (session_thread_data_map.find(thread_id) == session_thread_data_map.end())
			{
				putLogError(17000, "Invalid thread id.", __FILE__, __LINE__ );
				return FINALIZE;
			}

			session_data = session_thread_data_map[thread_id];
			if (session_data == NULL)
			{
				putLogError(17000, "Invalid pointer.", __FILE__, __LINE__ );
				return FINALIZE;
			}

			if (session_data->recive_data_map.find(session_data->target_endpoint) == session_data->recive_data_map.end())
			{
				putLogError(17000, "Invalid endpoint.", __FILE__, __LINE__ );
				return FINALIZE;
			}
		}

		recive_data& recv_data = session_data->recive_data_map[session_data->target_endpoint];

		//該当スレッドの送信可能データを取得する
		send_status_it it = find_if(recv_data.send_status_list.begin(), recv_data.send_status_list.end(),
		        data_send_possible());
		if (it == recv_data.send_status_list.end())
		{
			putLogError(17000, "sending possible data is not existed.", __FILE__, __LINE__ );
			return FINALIZE;
		}

		//送信バッファサイズ　≧　送信可能データサイズ
		if (send_buffer_size > it->send_possible_size)
		{
			//送信可能データを先頭から送信可能データサイズ分、送信バッファにコピーする
			memcpy(sendbuffer.data(), recv_data.recive_buffer + it->send_offset, it->send_possible_size);
			//送信済データサイズに送信可能データサイズを設定する
			it->send_end_size = it->send_possible_size;
			//送信可能データサイズに0を設定する
			it->send_possible_size = 0;
			//
			datalen = it->send_possible_size;
		}
		//送信バッファサイズ　＜　送信可能データサイズ
		else
		{
			//送信可能データを先頭から送信バッファサイズ分、送信バッファにコピーする
			memcpy(sendbuffer.data(), recv_data.recive_buffer + it->send_offset, send_buffer_size);
			//送信済データサイズに送信バッファサイズを設定する
			it->send_end_size = send_buffer_size;
			//送信可能データサイズから送信バッファサイズを減算する
			it->send_possible_size -= send_buffer_size;
			//
			datalen = send_buffer_size;
		}

		//遷移先ステータスを設定する
		status = CLIENT_SEND;
	} catch (const std::exception& ex)
	{
		std::cerr << "handle_client_connection_check exception: error=%s" << ex.what() << "." << std::endl;
		boost::format formatter("function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_client_connection_check() exception: result=%d, error=%s.");
		formatter % FINALIZE % ex.what();
		putLogError(17000, formatter.str(), __FILE__, __LINE__ );
		status = FINALIZE;
	} catch (...)
	{
		std::cerr << "Unkown exception." << std::endl;
		boost::format formatter("function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_client_connection_check() exception: result=%d.");
		formatter % FINALIZE;
		putLogError(17000, formatter.str(), __FILE__, __LINE__ );
		status = FINALIZE;
	}

	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		boost::format formatter("out_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_client_connection_check( const boost::thread::id thread_id): return_value=%d");
		formatter % status;
		putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
	}
	/*------DEBUG LOG END------*/
	return status;
}

/*!
 *handle_client_select
 *
 *@param [in] thread_id
 *@param [out] sendbuffer
 *@param [out] datalen
 */
protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::handle_client_select(
        const boost::thread::id thread_id, boost::asio::ip::udp::endpoint& cl_endpoint, boost::array<char,
                MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen)
{
	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		boost::format formatter("in/out_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_client_select( const boost::thread::id thread_id,"
			"boost::asio::ip::udp::endpoint& cl_endpoint,"
			"boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,"
			"size_t& datalen):"
			"return_value=%d.");
		formatter % STOP;
		putLogDebug(10000, formatter.str(), __FILE__, __LINE__ );
	}
	/*------DEBUG LOG END------*/
	return STOP;
}

/*!
 *handle_client_send
 *
 *@param [in] thread_id
 */
protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::handle_client_send(
        const boost::thread::id thread_id)
{
	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		boost::format formatter("in_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_client_send( const boost::thread::id thread_id): "
			"thread_id=%d");
		formatter % thread_id;
		putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
	}
	/*------DEBUG LOG END------*/
	EVENT_TAG status;
	session_thread_data_sessionless* session_data = NULL;

	try
	{
		boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
		if (session_thread_data_map.find(thread_id) == session_thread_data_map.end())
		{
			putLogError(17000, "Invalid thread id.", __FILE__, __LINE__ );
			return FINALIZE;
		}

		session_data = session_thread_data_map[thread_id];
		if (session_data == NULL)
		{
			putLogError(17000, "Invalid pointer.", __FILE__, __LINE__ );
			return FINALIZE;
		}

		if (session_data->recive_data_map.find(session_data->target_endpoint) == session_data->recive_data_map.end())
		{
			putLogError(17000, "Invalid endpoint.", __FILE__, __LINE__ );
			return FINALIZE;
		}

		recive_data& recv_data = session_data->recive_data_map[session_data->target_endpoint];

		send_status_it it = recv_data.send_status_list.begin();
		send_status_it it_end = recv_data.send_status_list.end();
		it = find_if(it, it_end, data_send_ok());
		if (it == it_end)
		{
			putLogError(17000, "sending possible data is not existed.", __FILE__, __LINE__ );
			return FINALIZE;
		}

		//送信可能データサイズ　＞　0
		if (it->send_possible_size > 0)
		{
			//送信状態に送信待を設定する
			it->status = SEND_OK;
			//送信データオフセットに送信済データサイズを加算する
			it->send_offset += it->send_end_size;

			for (std::list<edit_data>::iterator list_it = it->edit_data_list.begin(); list_it
			        != it->edit_data_list.end(); ++list_it)
			{
				//編集データ設定位置から送信済データサイズを減算する。
				list_it->insert_posission -= it->send_end_size;
			}

			//送信済データサイズに0を設定する
			it->send_end_size = 0;
		}
		//送信可能データサイズ　≦　0
		else
		{
			//送信データ残サイズ　＞　0
			if (it->send_rest_size > 0)
			{
				//送信状態に送信継続を設定する
				it->status = SEND_CONTINUE;
			}
			//送信データ残サイズ　≦　0
			else
			{
				//送信状態に送信済を設定する
				it->status = SEND_END;
			}
		}

		it = recv_data.send_status_list.begin();
		it = find_if(it, it_end, data_send_ok());
		//送信可能データあり
		if (it != it_end)
		{
			//遷移先ステータスを設定する
			status = REALSERVER_CONNECT;
		}
		//送信可能データなし
		else
		{
			//終了フラグがONの場合
			if (session_data->end_flag == END_FLAG_ON)
			{
				//遷移先ステータスを設定する
				status = CLIENT_DISCONNECT;
			}
			//終了フラグがOFFの場合
			else
			{
				//sorry状態の場合
				if (session_data->sorry_flag == SORRY_FLAG_ON)
				{
					//遷移先ステータスを設定する
					status = SORRYSERVER_RECV;
				}
				//sorry状態以外の場合
				else
				{
					//遷移先ステータスを設定する
					status = REALSERVER_RECV;
				}
			}
		}
	} catch (const std::exception& ex)
	{
		std::cerr << "handle_client_send exception: error=%s" << ex.what() << "." << std::endl;
		boost::format formatter("function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_client_send() exception: result=%d, error=%s.");
		formatter % FINALIZE % ex.what();
		putLogError(17000, formatter.str(), __FILE__, __LINE__ );
		status = FINALIZE;
	} catch (...)
	{
		std::cerr << "Unkown exception." << std::endl;
		boost::format formatter("function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_client_send() exception: result=%d.");
		formatter % FINALIZE;
		putLogError(17000, formatter.str(), __FILE__, __LINE__ );

		status = FINALIZE;
	}

	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		boost::format formatter("out_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_client_send( const boost::thread::id thread_id): return_value=%d");
		formatter % status;
		putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
	}
	/*------DEBUG LOG END------*/

	return status;
}

/*!
 *handle_client_disconnect
 *
 *@param [in] thread_id
 */
protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::handle_client_disconnect(
        const boost::thread::id thread_id)
{
	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		putLogDebug(10002, "in/out_function: protocol_module_sessionless::"
			"handle_client_disconnect()", __FILE__, __LINE__ );
	}
	/*------DEBUG LOG END------*/
	return FINALIZE;
}

/*!
 *handle_sorry_enable
 *
 *@param [in] thread_id
 */
protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::handle_sorry_enable(
        const boost::thread::id thread_id)
{
	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		boost::format formatter("in_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_sorry_enable( const boost::thread::id thread_id): "
			"thread_id=%d");
		formatter % thread_id;
		putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
	}
	/*------DEBUG LOG END------*/

	EVENT_TAG status;
	boost::asio::ip::tcp::endpoint endpoint;
	bool send_possible = false;
	bool send_continue = false;
	bool send_disable = false;
	session_thread_data_sessionless* session_data = NULL;

	try
	{
		{
			boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
			if (session_thread_data_map.find(thread_id) == session_thread_data_map.end())
			{
				putLogError(17000, "Invalid thread id.", __FILE__, __LINE__ );
				return FINALIZE;
			}

			session_data = session_thread_data_map[thread_id];

			if (session_data == NULL)
			{
				putLogError(17000, "Invalid pointer.", __FILE__, __LINE__ );
				return FINALIZE;
			}

			endpoint = session_data->thread_division == THREAD_DIVISION_UP_STREAM ? session_data->client_endpoint_tcp
			        : session_data->target_endpoint;
			if (session_data->recive_data_map.find(endpoint) == session_data->recive_data_map.end())
			{
				putLogError(17000, "Invalid endpoint.", __FILE__, __LINE__ );
				return FINALIZE;
			}
		}

		recive_data& recv_data = session_data->recive_data_map[endpoint];

		//該当スレッドの送信可能データを取得する
		send_status_it it = find_if(recv_data.send_status_list.begin(), recv_data.send_status_list.end(),
		        data_send_possible());
		if (it != recv_data.send_status_list.end())
		{
			send_possible = true;
		}

		it = find_if(recv_data.send_status_list.begin(), recv_data.send_status_list.end(), data_send_continue());
		if (it != recv_data.send_status_list.end())
		{
			send_continue = true;
		}

		it = find_if(recv_data.send_status_list.begin(), recv_data.send_status_list.end(), data_send_disable());
		if (it != recv_data.send_status_list.end())
		{
			send_disable = true;
		}

		//上りスレッドの場合
		if (session_data->thread_division == THREAD_DIVISION_UP_STREAM)
		{
			//accept完了フラグOFFの場合
			if (session_data->accept_end_flag == ACCEPT_END_FLAG_OFF)
			{
				//sorry状態をON
				session_data->sorry_flag = SORRY_FLAG_ON;
				//遷移先ステータスを設定する
				status = ACCEPT;
			}
			//accept完了フラグONの場合
			else
			{
				//sorry状態の場合
				if (session_data->sorry_flag == SORRY_FLAG_ON)
				{
					//送信可能データあり
					if (send_possible)
					{
						//遷移先ステータスを設定する
						status = SORRYSERVER_SELECT;
					}
					//送信可能データなし
					else
					{
						//遷移先ステータスを設定する
						status = CLIENT_RECV;
					}
				}
				//sorry状態以外の場合
				else
				{
					//送信継続データあり
					if (send_continue)
					{
						//終了フラグをON
						session_data->end_flag = END_FLAG_ON;
						//遷移先ステータスを設定する
						status = REALSERVER_DISCONNECT;
					}
					//送信継続データなし
					else
					{
						//sorryserver切替中フラグON
						session_data->sorryserver_switch_flag = SORRYSERVER_SWITCH_FLAG_ON;
						//sorry状態をON
						session_data->sorry_flag = SORRY_FLAG_ON;
						//遷移先ステータスを設定する
						status = REALSERVER_DISCONNECT;
					}
				}
			}
		}
		//下りスレッドの場合
		else
		{
			//sorry状態の場合
			if (session_data->sorry_flag == SORRY_FLAG_ON)
			{
				//送信可能データあり
				if (send_possible)
				{
					//遷移先ステータスを設定する
					status = CLIENT_CONNECTION_CHECK;
				}
				//送信可能データなし
				else
				{
					//遷移先ステータスを設定する
					status = SORRYSERVER_RECV;
				}
			}
			//sorry状態以外の場合
			else
			{
				//送信不可データあり　または　送信データ残サイズ ＞ ０　が存在する場合
				if (send_disable)
				{
					//終了フラグをON
					session_data->end_flag = END_FLAG_ON;
					//遷移先ステータスを設定する
					status = REALSERVER_DISCONNECT;
				}
				//
				else
				{
					//sorry状態をON
					session_data->sorry_flag = SORRY_FLAG_ON;
					//送信可能データあり
					if (send_possible)
					{
						//遷移先ステータスを設定する
						status = CLIENT_CONNECTION_CHECK;
					}
					//送信可能データなし
					else
					{
						//遷移先ステータスを設定する
						status = SORRYSERVER_RECV;
					}
				}
			}
		}
	} catch (const std::exception& ex)
	{
		std::cerr << "handle_sorry_enable exception: error=%s" << ex.what() << "." << std::endl;
		boost::format formatter("function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_sorry_enable() exception: result=%d, error=%s.");
		formatter % FINALIZE % ex.what();
		putLogError(17000, formatter.str(), __FILE__, __LINE__ );
		status = FINALIZE;
	} catch (...)
	{
		std::cerr << "Unkown exception." << std::endl;
		boost::format formatter("function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_sorry_enable() exception: result=%d.");
		formatter % FINALIZE;
		putLogError(17000, formatter.str(), __FILE__, __LINE__ );
		status = FINALIZE;
	}

	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		boost::format formatter("out_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_sorry_enable( const boost::thread::id thread_id): return_value=%d");
		formatter % status;
		putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
	}
	/*------DEBUG LOG END------*/

	return status;
}

/*!
 *handle_sorry_disable
 *
 *@param [in] thread_id
 */
protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::handle_sorry_disable(
        const boost::thread::id thread_id)
{
	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		boost::format formatter("in_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_sorry_disable( const boost::thread::id thread_id): "
			"thread_id=%d");
		formatter % thread_id;
		putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
	}
	/*------DEBUG LOG END------*/
	EVENT_TAG status;
	boost::asio::ip::tcp::endpoint endpoint;
	bool send_possible = false;
	bool send_disable = false;
	bool send_continue = false;
	session_thread_data_sessionless* session_data = NULL;

	try
	{
		{
			boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
			if (session_thread_data_map.find(thread_id) == session_thread_data_map.end())
			{
				putLogError(17000, "Invalid thread id.", __FILE__, __LINE__ );
				return FINALIZE;
			}

			session_data = session_thread_data_map[thread_id];
			if (session_data == NULL)
			{
				putLogError(17000, "Invalid pointer.", __FILE__, __LINE__ );
				return FINALIZE;
			}
		}

		endpoint = session_data->thread_division == THREAD_DIVISION_UP_STREAM ? session_data->client_endpoint_tcp
		        : session_data->target_endpoint;
		if (session_data->recive_data_map.find(endpoint) == session_data->recive_data_map.end())
		{
			putLogError(17000, "Invalid endpoint.", __FILE__, __LINE__ );
			return FINALIZE;
		}

		recive_data& recv_data = session_data->recive_data_map[endpoint];

		//送信可能データを検索
		send_status_it it = find_if(recv_data.send_status_list.begin(), recv_data.send_status_list.end(),
		        data_send_possible());
		if (it != recv_data.send_status_list.end())
		{
			send_possible = true;
		}

		//送信不可データあり　または　送信データ残サイズ ＞ ０　が存在する場合を検索
		it = find_if(recv_data.send_status_list.begin(), recv_data.send_status_list.end(), data_send_disable());
		if (it != recv_data.send_status_list.end())
		{
			send_disable = true;
		}

		//送信継続データを検索
		it = find_if(recv_data.send_status_list.begin(), recv_data.send_status_list.end(), data_send_continue());
		if (it != recv_data.send_status_list.end())
		{
			send_continue = true;
		}

		//上りスレッドの場合
		if (session_data->thread_division == THREAD_DIVISION_UP_STREAM)
		{
			//accept完了フラグOFFの場合
			if (session_data->accept_end_flag == ACCEPT_END_FLAG_OFF)
			{
				//sorry状態をOFF
				session_data->sorry_flag = SORRY_FLAG_OFF;
				//遷移先ステータスを設定する
				status = ACCEPT;
			}
			//accept完了フラグONの場合
			else
			{
				//sorry状態の場合
				if (session_data->sorry_flag == SORRY_FLAG_ON)
				{
					//送信継続データあり
					if (send_continue)
					{
						//終了フラグをON
						session_data->end_flag = END_FLAG_ON;
						//遷移先ステータスを設定する
						status = SORRYSERVER_DISCONNECT;
					}
					//送信継続データなし
					else
					{
						//realserver切替中
						session_data->realserver_switch_flag = REALSERVER_SWITCH_FLAG_ON;
						//sorry状態をOFF
						session_data->sorry_flag = SORRY_FLAG_OFF;
						//遷移先ステータスを設定する
						status = SORRYSERVER_DISCONNECT;
					}
				}
				//sorry状態以外の場合
				else
				{
					//送信可能データあり
					if (send_possible)
					{
						//遷移先ステータスを設定する
						status = REALSERVER_SELECT;
					}
					//送信可能データなし
					else
					{
						//遷移先ステータスを設定する
						status = CLIENT_RECV;
					}
				}
			}
		}
		//下りスレッドの場合
		else
		{
			//sorry状態の場合
			if (session_data->sorry_flag == SORRY_FLAG_ON)
			{
				//送信不可データあり　または　送信データ残サイズ ＞ ０　が存在する場合
				if (send_disable)
				{
					//終了フラグをON
					session_data->end_flag = END_FLAG_ON;
					//遷移先ステータスを設定する
					status = SORRYSERVER_DISCONNECT;
				}
				//送信不可データなし　かつ　送信データ残サイズ ＞ ０　が存在しない場合
				else
				{
					//sorry状態をOFF
					session_data->sorry_flag = SORRY_FLAG_OFF;
					//送信可能データあり
					if (send_possible)
					{
						//遷移先ステータスを設定する
						status = CLIENT_CONNECTION_CHECK;
					}
					//送信可能データなし
					else
					{
						//遷移先ステータスを設定する
						status = REALSERVER_RECV;
					}
				}
			}
			//sorry状態以外の場合
			else
			{
				//送信可能データあり
				if (send_possible)
				{
					//遷移先ステータスを設定する
					status = CLIENT_CONNECTION_CHECK;
				}
				//送信可能データなし
				else
				{
					//遷移先ステータスを設定する
					status = REALSERVER_RECV;
				}
			}
		}
	} catch (std::exception& ex)
	{
		std::cerr << "handle_sorry_disable exception: error=%s" << ex.what() << "." << std::endl;
		boost::format formatter("function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_sorry_disable() exception: result=%d, error=%s.");
		formatter % FINALIZE % ex.what();
		putLogError(17000, formatter.str(), __FILE__, __LINE__ );
		status = FINALIZE;
	} catch (...)
	{
		std::cerr << "Unkown exception." << std::endl;
		boost::format formatter("function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_sorry_disable() exception: result=%d.");
		formatter % FINALIZE;
		putLogError(17000, formatter.str(), __FILE__, __LINE__ );
		status = FINALIZE;
	}

	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		boost::format formatter("out_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_sorry_disable( const boost::thread::id thread_id): return_value=%d");
		formatter % status;
		putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
	}
	/*------DEBUG LOG END------*/

	return status;
}

/*!
 *handle_sorryserver_disconnect
 *
 *@param [in] thread_id
 *@param [in] rs_endpoint
 */
protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::handle_realserver_disconnect(
        const boost::thread::id thread_id, const boost::asio::ip::tcp::endpoint & rs_endpoint)
{
	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		boost::format formatter("in_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_sorry_disable( const boost::thread::id thread_id, const boost::asio::ip::tcp::endpoint & rs_endpoint): "
			"thread_id=%d, rs_endpoint=%s");
		formatter % thread_id % rs_endpoint.address().to_string();
		putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
	}
	/*------DEBUG LOG END------*/
	EVENT_TAG status;
	bool possible_flag = false;
	boost::asio::ip::tcp::endpoint endpoint;
	session_thread_data_sessionless* session_data = NULL;

	try
	{
		{
			boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
			if (session_thread_data_map.find(thread_id) == session_thread_data_map.end())
			{

				return FINALIZE;
			}

			session_data = session_thread_data_map[thread_id];
			if (session_data == NULL)
			{
				return FINALIZE;
			}
		}

		if (session_data->recive_data_map.find(endpoint) == session_data->recive_data_map.end())
		{
			return FINALIZE;
		}

		recive_data& recv_data = session_data->recive_data_map[endpoint];

		//送信可能データを検索
		send_status_it it = find_if(recv_data.send_status_list.begin(), recv_data.send_status_list.end(),
		        data_send_possible());
		if (it != recv_data.send_status_list.end())
		{
			possible_flag = true;
		}

		//上りスレッドの場合
		if (session_data->thread_division == THREAD_DIVISION_UP_STREAM)
		{
			//終了フラグがONの場合
			if (session_data->end_flag == END_FLAG_ON)
			{
				//遷移先ステータスを設定する
				status = CLIENT_RECV;
			}
			//終了フラグがOFFの場合
			else
			{
				//sorryserver切替中の場合
				if (session_data->sorryserver_switch_flag == SORRYSERVER_SWITCH_FLAG_ON)
				{
					//送信可能データあり
					if (possible_flag)
					{
						//遷移先ステータスを設定する
						status = SORRYSERVER_SELECT;
					}
					//送信可能データなし
					else
					{
						//遷移先ステータスを設定する
						status = CLIENT_RECV;
					}
				}
				//sorryserver切替中でない場合
				else
				{
					//終了フラグをON
					session_data->end_flag = END_FLAG_ON;
					status = CLIENT_RECV;
				}
			}
		}
		//下りスレッドの場合
		else
		{
			//終了フラグをON
			session_data->end_flag = END_FLAG_ON;
			//送信可能データあり
			if (possible_flag)
			{
				//遷移先ステータスを設定する
				status = CLIENT_CONNECTION_CHECK;
			}
			//送信可能データなし
			else
			{
				//遷移先ステータスを設定する
				status = CLIENT_DISCONNECT;
			}
		}
	} catch (std::exception& ex)
	{
		std::cerr << "handle_realserver_disconnect exception: error=%s" << ex.what() << "." << std::endl;
		boost::format formatter("function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_realserver_disconnect() exception: result=%d, error=%s.");
		formatter % FINALIZE % ex.what();
		putLogError(17000, formatter.str(), __FILE__, __LINE__ );
		status = FINALIZE;
	} catch (...)
	{
		std::cerr << "Unkown exception." << std::endl;
		boost::format formatter("function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_realserver_disconnect() exception: result=%d.");
		formatter % FINALIZE;
		putLogError(17000, formatter.str(), __FILE__, __LINE__ );
		status = FINALIZE;
	}

	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		boost::format formatter("out_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_realserver_disconnect( const boost::thread::id thread_id): return_value=%d");
		formatter % status;
		putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
	}
	/*------DEBUG LOG END------*/

	return status;
}

/*!
 *handle_sorryserver_disconnect
 *
 *@param [in] thread_id
 *@param [in] sorry_endpoint
 */
protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::handle_sorryserver_disconnect(
        const boost::thread::id thread_id, const boost::asio::ip::tcp::endpoint & sorry_endpoint)
{
	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		boost::format
		        formatter(
		                "in_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			                "handle_sorryserver_disconnect( const boost::thread::id thread_id, const boost::asio::ip::tcp::endpoint & sorry_endpoint): "
			                "thread_id=%d, sorry_endpoint=%s");
		formatter % thread_id % sorry_endpoint.address().to_string();
		putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
	}
	/*------DEBUG LOG END------*/
	EVENT_TAG status;
	bool possible_flag = false;
	boost::asio::ip::tcp::endpoint endpoint;
	session_thread_data_sessionless* session_data = NULL;

	try
	{
		if (session_thread_data_map.find(thread_id) == session_thread_data_map.end())
		{
			putLogError(17000, "Invalid thread id.", __FILE__, __LINE__ );
			return FINALIZE;
		}

		session_data = session_thread_data_map[thread_id];

		if (session_data == NULL)
		{
			putLogError(17000, "Invalid pointer.", __FILE__, __LINE__ );
			return FINALIZE;
		}

		if (session_data->recive_data_map.find(endpoint) == session_data->recive_data_map.end())
		{
			putLogError(17000, "Invalid endpoint.", __FILE__, __LINE__ );
			return FINALIZE;
		}

		recive_data& recv_data = session_data->recive_data_map[endpoint];

		//送信可能データを検索
		send_status_it it = find_if(recv_data.send_status_list.begin(), recv_data.send_status_list.end(),
		        data_send_possible());
		if (it != recv_data.send_status_list.end())
		{
			possible_flag = true;
		}

		//上りスレッドの場合
		if (session_data->thread_division == THREAD_DIVISION_UP_STREAM)
		{
			//終了フラグがONの場合
			if (session_data->end_flag == END_FLAG_ON)
			{
				//遷移先ステータスを設定する
				status = CLIENT_RECV;
			}
			//終了フラグがOFFの場合
			else
			{
				//realserver切替中の場合
				if (session_data->realserver_switch_flag == REALSERVER_SWITCH_FLAG_ON)
				{
					//送信可能データあり
					if (possible_flag)
					{
						//遷移先ステータスを設定する
						status = REALSERVER_SELECT;
					}
					//送信可能データなし
					else
					{
						//遷移先ステータスを設定する
						status = CLIENT_RECV;
					}
				}
				//realserver切替中でない場合
				else
				{
					//終了フラグをON
					session_data->end_flag = END_FLAG_ON;
					status = CLIENT_RECV;
				}
			}
		}
		//下りスレッドの場合
		else
		{
			//終了フラグをON
			session_data->end_flag = END_FLAG_ON;
			//送信可能データあり
			if (possible_flag)
			{
				//遷移先ステータスを設定する
				status = CLIENT_CONNECTION_CHECK;
			}
			//送信可能データなし
			else
			{
				//遷移先ステータスを設定する
				status = CLIENT_DISCONNECT;
			}
		}
	} catch (const std::exception& ex)
	{
		std::cerr << "handle_sorryserver_disconnect exception: error=%s" << ex.what() << "." << std::endl;
		boost::format formatter("function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_sorryserver_disconnect() exception: result=%d, error=%s.");
		formatter % FINALIZE % ex.what();
		putLogError(17000, formatter.str(), __FILE__, __LINE__ );
		status = FINALIZE;
	} catch (...)
	{
		std::cerr << "Unkown exception." << std::endl;
		boost::format formatter("function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_sorryserver_disconnect() exception: result=%d.");
		formatter % FINALIZE;
		putLogError(17000, formatter.str(), __FILE__, __LINE__ );
		status = FINALIZE;
	}

	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		boost::format
		        formatter(
		                "out_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			                "handle_sorryserver_disconnect( const boost::thread::id thread_id, const boost::asio::ip::tcp::endpoint & sorry_endpoint): return_value=%d");
		formatter % status;
		putLogDebug(10005, formatter.str(), __FILE__, __LINE__ );
	}
	/*------DEBUG LOG END------*/

	return status;
}

/*!
 *handle_realserver_close
 *
 *@param [in] thread_id
 *@param [in] rs_endpoint
 */
protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::handle_realserver_close(
        const boost::thread::id thread_id, const boost::asio::ip::udp::endpoint & rs_endpoint)
{
	/*-------- DEBUG LOG --------*/
	if (LOG_LV_DEBUG == getloglevel())
	{
		boost::format formatter("in/out_function: protocol_module_sessionless::EVENT_TAG protocol_module_sessionless::"
			"handle_realserver_close( const boost::thread::id thread_id,"
			"const boost::asio::ip::udp::endpoint & rs_endpoint ): "
			"return_value=%d.");
		formatter % STOP;
		putLogDebug(10000, formatter.str(), __FILE__, __LINE__ );
	}
	/*------DEBUG LOG END------*/
	return STOP;
}

}
