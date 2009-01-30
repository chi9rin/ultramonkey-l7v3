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
        ~ssl_protocol_module_base();
//ZLQ modify for test:    protected:
        int get_ssl_session_id(const char* record_data, int recv_length, std::string& session_id);

        int check_ssl_record_sendable( bool is_message_form_client,
                                                    const char* record_data,
                                                    int recv_length,
                                                    int& all_length,
                                                    bool& is_hello_message);
};

}   // namespace l7vsd

#endif // SSL_PROTOCOL_MODULE_BASE_H
