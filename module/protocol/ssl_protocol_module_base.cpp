#include "ssl_protocol_module_base.h"
#include <arpa/inet.h>

namespace l7vs
{
	ssl_protocol_module_base::ssl_protocol_module_base(std::string in_modulename)
	:protocol_module_base(in_modulename)
    {
        //ctor
    }

    ssl_protocol_module_base::~ssl_protocol_module_base()
    {
        //dtor
    }
    /*
     * get ssl session id
     * @param[in]  const char*   record_data             ssl record data pointer
     * @param[in]  int           recv_length             ssl record data length
     * @param[out] std::string&  session_id              ssl session id
	 * @return 0 get session id, 1 no session id, -1 recv data error 
     */
    int ssl_protocol_module_base::get_ssl_session_id(const char* record_data, size_t recv_length, std::string& session_id)
    {
    	//check the ssl record data length
    	if (recv_length >= HELLO_MSG_HEADER_LENGTH && record_data != NULL)
    	{
    		//check the length of ssl session id (session id length is 32 bytes)
    		if (record_data[SESSION_ID_BEGAIN_OFFSET - 1] == 0x20)
    		{
    			//get session id
    			session_id.assign(record_data + SESSION_ID_BEGAIN_OFFSET, record_data + HELLO_MSG_HEADER_LENGTH);
    			return 0;
    		}
    		else
    		{
    			//the length of ssl session id is 0, the ssl record data dose not have a session id.
    			return 1;
    		}
    	}
    	else
    	{
    		//the ssl record data length < HELLO_MSG_HEADER_LENGTH, error!
    		return -1;
    	}
    }
    /*
     * check the ssl record data whether is able to send, or not.
     * @param[in]  bool        is_message_form_client   is message from client
     * @param[in]  const char* record_data              ssl record data pointer
     * @param[out] int&        all_length               ssl record data length
     * @param[out] bool&       is_hello_message         is ssl client/server hello record data
	 * @return 0 送信可能, 1 送信不可, -1 異常
     */
    int ssl_protocol_module_base::check_ssl_record_sendable( bool is_message_form_client,
                                                    const char* record_data,
                                                    size_t recv_length,
                                                    size_t& all_length,
                                                    bool& is_hello_message)
    {
    	//check record_data pointer
    	if (record_data == NULL) {
    		return -1;
    	}
    	//is_hello_messageを FALSEで設定する
		is_hello_message = false;
		//data_size≧6(SSL record のminimalサイズ)
		if (recv_length >= SSL_RECORD_MIN_SIZE)
		{
			// SSL recordチェック
			// SSLレコードデータの1バイト目が「20」、「21」、「22」、「23」で、
			//  2バイト目が「0x03」で、3バイト目が「0x00」、又は「0x01」の場合、
			//  チェック結果がOKで、その以外の場合、チェック結果がNGとして処理を行う。
			if((record_data[0] == 0x14 ||
				record_data[0] == 0x15 ||
				record_data[0] == 0x16 ||
				record_data[0] == 0x17) && (
				record_data[1] == 0x03) && (
				record_data[2] == 0x00 || record_data[2] == 0x01))
			{//チェック結果 = OK
				//  handshake helloチェック
				//  SSLレコードデータの1バイト目が「22」、且つ6バイト目が「0x01」、「0x02」の場合、
				//  チェック結果がhandshake helloとして処理を行う。
				if(record_data[0] == 0x16 && (record_data[5] == 0x01 || record_data[5] == 0x02))
				{//handshake helloの場合
					if(recv_length >= HELLO_MSG_HEADER_LENGTH)
					{//data_size≧76の場合
						//SSLレコードデータの10バイト目が「0x03」、
						//且つ11バイト目が「0x00」、「0x01」をチェックする。
						//チェックが正常の場合、引数の「is_message_form_client」により下記の処理を行う。
						//・is_message_form_clientがTUREの場合：6バイト目が「1」(client hello)をチェックする。
						//・is_message_form_clientが FALSEの場合：6バイト目が「2」(server hello)をチェックする。
						if(record_data[9] == 0x03 &&
							(record_data[10] == 0x00 || record_data[10] == 0x01) &&
							((is_message_form_client && record_data[5] == 0x01) ||
							 (!is_message_form_client && record_data[5] == 0x02)))
						{//チェック結果 = TRUE
							//is_hello_messageを TRUEで設定する
							is_hello_message = true;
							//SSLレコードデータの4バイト目と5バイト目がSSL recordのmessagesのサイズで、
							//sSSL recordの全サイズを messagesのサイズ + 5で設定する
							unsigned short* message_size = reinterpret_cast<unsigned short*>(const_cast<char*>(record_data)+3);
							all_length = ntohs(*message_size) + 5;
							//SSL recordチェック結果を「送信可能」
							return 0;
						}
						else
						{//チェック結果 = FALSE
							//SSL recordチェック結果が「異常」を返す
							return -1;
						}
					}
					else
					{//data_size<HELLO_MSG_HEADER_LENGTH の場合
						//SSL recordチェック結果を「送信不可」
						return 1;
					}
				}
				else
				{//その他場合
					//SSLレコードデータの4バイト目と5バイト目がSSL recordのmessagesのサイズで、
					//sSSL recordの全サイズを messagesのサイズ + 5で設定する
					unsigned short high_length = 0;
					unsigned short low_length = 0;
					high_length = static_cast<unsigned short>(static_cast<unsigned char>(record_data[3]));
					high_length = high_length << 8;
					low_length = static_cast<unsigned short>(static_cast<unsigned char>(record_data[4]));
					all_length = (high_length | low_length) + 5;
					//SSL recordチェック結果を「送信可能」
					return 0;
				}
			}
			else
			{//チェック結果 = NG
				//SSL recordチェック結果が「異常」を返す
				return -1;
			}
		}
		else
		{//data_size<6(SSL record のminimalサイズ)
			//SSL recordチェック結果を「送信不可」
			return 1;
		}
    }

}
