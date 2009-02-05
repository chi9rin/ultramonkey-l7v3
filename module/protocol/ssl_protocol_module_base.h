#ifndef SSL_PROTOCOL_MODULE_BASE_H
#define SSL_PROTOCOL_MODULE_BASE_H

#include "protocol_module_base.h"
#include <string>

namespace l7vs
{

class ssl_protocol_module_base : public protocol_module_base
{
    public:
        ssl_protocol_module_base(std::string in_modulename);
        virtual ~ssl_protocol_module_base();
    protected:
        int get_ssl_session_id(const char* record_data, size_t recv_length, std::string& session_id);

        int check_ssl_record_sendable( bool is_message_form_client,
                                                    const char* record_data,
                                                    size_t recv_length,
                                                    size_t& all_length,
                                                    bool& is_hello_message);
	protected:
		const static size_t HELLO_MSG_HEADER_LENGTH = 76;
		const static size_t SESSION_ID_BEGAIN_OFFSET = 44;
		const static size_t SSL_RECORD_MIN_SIZE = 6;
};

}   // namespace l7vsd

#endif // SSL_PROTOCOL_MODULE_BASE_H
