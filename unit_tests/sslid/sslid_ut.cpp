#include <boost/test/included/unit_test.hpp>

#include "protocol_module_sslid_test.cpp"
#include "ssl_protocol_module_base_test.cpp"
#include "sslid_replication_data_processor_test.cpp"
#include "sslid_session_data_processor_test.cpp"

test_suite*	init_unit_test_suite( int argc, char* argv[] ){

	protocol_module_sslid_test_main();
	ssl_protocol_module_base_test_main();
	sslid_replication_data_processor_test_main();
	sslid_session_data_processor_test_main();

	return 0;
}

