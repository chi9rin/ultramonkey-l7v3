error=
for path in ./command_receiver_test/command_receiver_ut \
	./command_session_test/command_session_ut \
	./command_thread_test/command_thread \
	./endpoint_test/endpoint_ut \
	./l7vs_command_test/l7vs_command_ut \
	./l7vs_logger/logger_access_manager_test \
	./l7vs_logger/logger_impl_test \
	./l7vs_logger/logger_ut_test \
	./l7vsadm_test/l7vsadm_ut \
	./l7vsd_test/l7vsd_thread \
	./l7vsd_test/l7vsd_ut \
	./logger_implement_access/accesslog_ut \
	./module_controls_test/protocol_module_control/module_control_thread_ut \
	./module_controls_test/protocol_module_control/module_control_ut \
	./module_controls_test/schedule_module_control/module_control_thread_ut \
	./module_controls_test/schedule_module_control/module_control_ut \
	./module_test/http_protocol_module_base_test/http_protocol_module_base_ut \
	./module_test/http_utility_test/http_utility_ut \
	./module_test/module_base_test/module_base_ut \
	./module_test/protocol_module_base_test/protocol_module_base_ut \
	./module_test/protocol_module_ip_test/ip_ut \
	./module_test/protocol_module_sessionless_test/sessionless_ut \
	./module_test/protocol_module_sslid_test/sslid_ut \
	./module_test/ssl_protocol_module_base_test/sslid_ut \
	./other/atomic_test \
	./parameter_test/parameter_impl_thread_ut \
	./parameter_test/parameter_impl_ut \
	./parameter_test/parameter_ut \
	./realserver_test/realserver_ut \
	./replication_test/replication_ut \
	./schedule_module_test/schedule_module_lc_ut \
	./schedule_module_test/schedule_module_rr_ut \
	./schedule_module_test/schedule_module_wrr_ut \
	./session_test/session_result_message/session_result_message_ut \
	./session_test/tcp_data_test/tcp_data_ut \
	./session_test/tcp_realserver_connect_socket_list/tcp_realserver_connect_socket_list_test_ut \
	./session_test/tcp_session/tcp_session_test_ut \
	./session_test/tcp_socket/tcp_socket_test_ut \
	./session_test/tcp_ssl_socket/tcp_ssl_socket_test_ut \
	./session_test/tcp_thread_message/tcp_thread_message_test_ut \
	./session_test/udp_data_test/udp_data_ut \
	./virtualservice_element_test/vselement_ut \
	./virtualservice_test/vs_base_ut \
	./virtualservice_test/vs_tcp_it \
	./virtualservice_test/vs_tcp_sorry_ut \
	./virtualservice_test/vs_tcp_thread_ut \
	./virtualservice_test/vs_tcp_thread_ut2 \
	./virtualservice_test/vs_tcp_ut 
do
	echo "$path" | grep "$1" > /dev/null
	if [ $? -ne 0 ]; then
		continue
	fi
	
	dir=`dirname $path`
	file=`basename $path`
	pushd $dir > /dev/null
	echo "exec $path"
	./$file 2>&1 | tail -1
	if [ "$?" -ne 0 ]; then
		error="${error}Error detected in $path\n"
	fi
	popd > /dev/null
done

if [ "$error" = "" ]; then
	exit 0
else
	echo -ne $error
	exit 1
fi
