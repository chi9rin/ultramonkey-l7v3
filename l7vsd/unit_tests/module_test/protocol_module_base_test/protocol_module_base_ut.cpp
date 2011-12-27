#include <boost/test/included/unit_test.hpp>
#include "../../../include/protocol_module_base.h"

using namespace l7vs;
using namespace boost;
using namespace boost::unit_test;

class protocol_module_base_test_class: public protocol_module_base
{
public:
        protocol_module_base_test_class() : protocol_module_base("test") {
        }

        ~protocol_module_base_test_class() {
        }

        bool is_tcp() {
                return true;
        }

        bool is_udp() {
                return false;
        }

        void replication_interrupt() {
        }

        void initialize(rs_list_itr_func_type inlist_begin,
                        rs_list_itr_func_type inlist_end,
                        rs_list_itr_next_func_type inlist_next,
                        boost::function<void(void)> inlist_lock,
                        boost::function<void(void)> inlist_unlock) {
        }

        void finalize() {
        }

        bool is_use_sorry() {
                return false;
        }

        void handle_rslist_update() {
        }

        check_message_result check_parameter(const std::vector<std::string>& args) {
                check_message_result result;
                return result;
        }

        check_message_result set_parameter(const std::vector<std::string>& args) {
                check_message_result result;
                return result;
        }

        check_message_result add_parameter(const std::vector<std::string>& args) {
                check_message_result result;
                return result;
        }

        void get_option_info(std::string &option) {
        }
        void register_schedule(tcp_schedule_func_type inschedule) {
        }

        void register_schedule(udp_schedule_func_type inschedule) {
        }

        EVENT_TAG handle_session_initialize(const boost::thread::id up_thread_id,
                                            const boost::thread::id down_thread_id,
                                            const boost::asio::ip::tcp::endpoint &client_endpoint_tcp,
                                            const boost::asio::ip::udp::endpoint &client_endpoint_udp) {
                return STOP;
        }

        EVENT_TAG handle_session_finalize(const boost::thread::id up_thread_id,
                                          const boost::thread::id down_thread_id) {
                return STOP;
        }

        EVENT_TAG handle_accept(const boost::thread::id thread_id) {
                return STOP;
        }

        EVENT_TAG handle_client_recv(const boost::thread::id thread_id,
                                     const boost::array<char, MAX_BUFFER_SIZE>& recvbuffer,
                                     const size_t recvlen) {
                return STOP;
        }

        EVENT_TAG handle_realserver_select(const boost::thread::id thread_id,
                                           boost::asio::ip::tcp::endpoint &rs_endpoint) {
                return STOP;
        }

        EVENT_TAG handle_realserver_select(const boost::thread::id thread_id,
                                           boost::asio::ip::udp::endpoint &rs_endpoint,
                                           boost::array<char, MAX_BUFFER_SIZE>& sendbuffer,
                                           size_t &datalen) {
                return STOP;
        }

        EVENT_TAG handle_realserver_connect(const boost::thread::id thread_id,
                                            boost::array<char, MAX_BUFFER_SIZE>& sendbuffer,
                                            size_t &datalen) {
                return STOP;
        }

        EVENT_TAG handle_realserver_connection_fail(const boost::thread::id thread_id,
                        const boost::asio::ip::tcp::endpoint &rs_endpoint) {
                return STOP;
        }

        EVENT_TAG handle_realserver_send(const boost::thread::id thread_id) {
                return STOP;
        }

        EVENT_TAG handle_sorryserver_select(const boost::thread::id thread_id,
                                            boost::asio::ip::tcp::endpoint &sorry_endpoint) {
                return STOP;
        }

        EVENT_TAG handle_sorryserver_connect(const boost::thread::id thread_id,
                                             boost::array<char, MAX_BUFFER_SIZE>& sendbuffer,
                                             size_t &datalen) {
                return STOP;
        }

        EVENT_TAG handle_sorryserver_connection_fail(const boost::thread::id thread_id,
                        const boost::asio::ip::tcp::endpoint &sorry_endpoint) {
                return STOP;
        }

        EVENT_TAG handle_sorryserver_send(const boost::thread::id thread_id) {
                return STOP;
        }

        EVENT_TAG handle_realserver_recv(const boost::thread::id thread_id,
                                         const boost::asio::ip::tcp::endpoint &rs_endpoint,
                                         const boost::array<char, MAX_BUFFER_SIZE>& recvbuffer,
                                         const size_t recvlen) {
                return STOP;
        }

        EVENT_TAG handle_realserver_recv(const boost::thread::id thread_id,
                                         const boost::asio::ip::udp::endpoint &rs_endpoint,
                                         const boost::array<char, MAX_BUFFER_SIZE>& recvbuffer,
                                         const size_t recvlen) {
                return STOP;
        }

        EVENT_TAG handle_sorryserver_recv(const boost::thread::id thread_id,
                                          const boost::asio::ip::tcp::endpoint &sorry_endpoint,
                                          const boost::array<char, MAX_BUFFER_SIZE>& recvbuffer,
                                          const size_t recvlen) {
                return STOP;
        }

        EVENT_TAG handle_response_send_inform(const boost::thread::id thread_id) {
                return STOP;
        }

        EVENT_TAG handle_client_connection_check(const boost::thread::id thread_id,
                        boost::array<char, MAX_BUFFER_SIZE>& sendbuffer,
                        size_t &datalen) {
                return STOP;
        }

        EVENT_TAG handle_client_select(const boost::thread::id thread_id,
                                       boost::asio::ip::udp::endpoint &cl_endpoint,
                                       boost::array<char, MAX_BUFFER_SIZE>& sendbuffer,
                                       size_t &datalen) {
                return STOP;
        }

        EVENT_TAG handle_client_send(const boost::thread::id thread_id) {
                return STOP;
        }

        EVENT_TAG handle_client_disconnect(const boost::thread::id thread_id) {
                return STOP;
        }

        EVENT_TAG handle_sorry_enable(const boost::thread::id thread_id) {
                return STOP;
        }

        EVENT_TAG handle_sorry_disable(const boost::thread::id thread_id) {
                return STOP;
        }

        EVENT_TAG handle_realserver_disconnect(const boost::thread::id thread_id,
                                               const boost::asio::ip::tcp::endpoint &rs_endpoint) {
                return STOP;
        }

        EVENT_TAG handle_sorryserver_disconnect(const boost::thread::id thread_id,
                                                const boost::asio::ip::tcp::endpoint &sorry_endpoint) {
                return STOP;
        }

        EVENT_TAG handle_realserver_close(const boost::thread::id thread_id,
                                          const boost::asio::ip::udp::endpoint &rs_endpoint) {
                return STOP;
        }

        void is_exec_OK_test() {
                std::cout << "[1]--------------------------------------------- " << std::endl;
                //unit_test[1] 正常系で必ずtrueを返す(SSL以外モード)
                BOOST_CHECK_EQUAL(this->is_exec_OK(VS_CONTACT_CLASS_NORMAL), true);

                //unit_test[2] 正常系で必ずtrueを返す(SSLモード)
                std::cout << "[2]--------------------------------------------- " << std::endl;
                BOOST_CHECK_EQUAL(this->is_exec_OK(VS_CONTACT_CLASS_SSL), true);
        }

};

void dump_memory_test()
{
        struct check_condition {
                char memory[256];
                size_t memory_size;
                std::string out_string;
        };

        // テスト条件（正常系）：
        check_condition condition_empty_string[12] = {
                {"a", 0, ""},
                {"a", 1, "\n61\n"},
                {"ab", 2, "\n6162\n"},
                {"abc", 3, "\n6162 63\n"},
                {"abcd", 4, "\n6162 6364\n"},
                {"abcde", 5, "\n6162 6364 65\n"},
                {
                        "abcdefghijklmnopqrstuvwxyzABCDEF", 32,
                        "\n6162 6364 6566 6768 696A 6B6C 6D6E 6F70 7172 7374 7576 7778 797A 4142 4344 4546\n"
                },
                {
                        "abcdefghijklmnopqrstuvwxyzABCDEFG", 33,
                        "\n6162 6364 6566 6768 696A 6B6C 6D6E 6F70 7172 7374 7576 7778 797A 4142 4344 4546\n47\n"
                },
                {
                        "abcdefghijklmnopqrstuvwxyzABCDEFGH", 34,
                        "\n6162 6364 6566 6768 696A 6B6C 6D6E 6F70 7172 7374 7576 7778 797A 4142 4344 4546\n4748\n"
                },
                {
                        "abcdefghijklmnopqrstuvwxyzABCDEFGHI", 35,
                        "\n6162 6364 6566 6768 696A 6B6C 6D6E 6F70 7172 7374 7576 7778 797A 4142 4344 4546\n4748 49\n"
                },
                {
                        "abcdefghijklmnopqrstuvwxyzABCDEFabcdefghijklmnopqrstuvwxyzABCDEF", 64,
                        "\n6162 6364 6566 6768 696A 6B6C 6D6E 6F70 7172 7374 7576 7778 797A 4142 4344 4546\n"
                        "6162 6364 6566 6768 696A 6B6C 6D6E 6F70 7172 7374 7576 7778 797A 4142 4344 4546\n"
                },
                {
                        "abcdefghijklmnopqrstuvwxyzABCDEFabcdefghijklmnopqrstuvwxyzABCDEFG", 65,
                        "\n6162 6364 6566 6768 696A 6B6C 6D6E 6F70 7172 7374 7576 7778 797A 4142 4344 4546\n"
                        "6162 6364 6566 6768 696A 6B6C 6D6E 6F70 7172 7374 7576 7778 797A 4142 4344 4546\n47\n"
                },
        };

        // unit_test[1]　ヌルポイントのテスト（異常系）
        std::string out_string = "123";
        std::string out_string_src(out_string);
        protocol_module_base::dump_memory(NULL, 100, out_string);
        BOOST_CHECK_EQUAL(out_string, out_string_src);
        std::cout << "out_string = \n" <<  out_string << std::endl;
        std::cout << "unit_test[1]-----------------------------\n" << std::endl;

        // unit_test[2]　ポイントの内容のサイズ　＜　データサイズのテスト（異常系）
        out_string = "";
        char *less_data = "abcdefghij";
        protocol_module_base::dump_memory(less_data, 65, out_string);
        BOOST_CHECK_EQUAL(out_string.size(), 164u);
        std::cout << "out_string = \n" <<  out_string << std::endl;
        std::cout << "unit_test[2]-----------------------------\n" << std::endl;

        // 正常系テスト
        for (size_t i = 0; i < 2; i++) {
                for (size_t j = 0; j < 12; j++) {
                        out_string_src = "";

                        if (i == 0) {
                                out_string = "";
                        } else {
                                out_string = "test";
                                out_string_src = out_string;
                        }
                        protocol_module_base::dump_memory(condition_empty_string[j].memory,
                                                          condition_empty_string[j].memory_size,
                                                          out_string);
                        BOOST_CHECK_EQUAL(out_string, out_string_src + condition_empty_string[j].out_string);

                        std::cout << "condition[" << j << "].memory = " <<  condition_empty_string[j].memory << std::endl;
                        std::cout << "condition[" << j << "].memory_size = " <<  condition_empty_string[j].memory_size << std::endl;
                        std::cout << "condition[" << j << "].out_string = " <<  condition_empty_string[j].out_string << std::endl;

                        std::cout << "out_string = " <<  out_string << std::endl;

                        if ((j + 12 * i) >= 10) {
                                std::cout << "unit_test[" << j + 12 * i + 3 << "]------------------------------------\n" << std::endl;
                        } else {
                                std::cout << "unit_test[" << j + 12 * i + 3 << "]-------------------------------------\n" << std::endl;
                        }
                }
        }
}

void is_exec_OK_test()
{
        protocol_module_base_test_class obj;
        obj.is_exec_OK_test();
}

test_suite    *init_unit_test_suite(int argc, char *argv[])
{

        test_suite *ts = BOOST_TEST_SUITE("protocol_module_base::dump_memory test");

        ts->add(BOOST_TEST_CASE(&dump_memory_test));
        ts->add(BOOST_TEST_CASE(&is_exec_OK_test));

        framework::master_test_suite().add(ts);
        return 0;
}
