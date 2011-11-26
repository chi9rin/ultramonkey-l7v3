#include <boost/test/included/unit_test.hpp>

#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>

#include "logger.h"
#include "parameter.h"

#include "l7vsd_stub.h"
#include "command_session_stub.h"

#include "../../src/command_receiver.cpp"

#define    SOCKFILE    "./l7vs"
#define    SOCKFILE2    "./l7vs2"

using namespace boost::unit_test_framework;

class    command_receiver_test    : public    l7vs::command_receiver
{
public:
        command_receiver_test(boost::asio::io_service &io,
                              const std::string &file,
                              l7vs::l7vsd &vsd)
                :    l7vs::command_receiver(io, file, vsd) {}
        std::string    get_sockfile() {
                return sockfile;
        }

};

// variable
boost::asio::io_service    io;

//--tests--

void    client_thread()
{
        using boost::asio::local::stream_protocol;

        boost::asio::io_service            client_io;
        stream_protocol::socket    s(client_io);
        s.connect(stream_protocol::endpoint(SOCKFILE));

        boost::xtime xt;
        xtime_get(&xt, boost::TIME_UTC);
        xt.sec += 1;
        boost::thread::sleep(xt);

        s.close();

}

void    server_thread()
{
        io.run();
}

void    receiver_test()
{
        l7vs::l7vsd                vsd;

        {
                // sockfile value check
                command_receiver_test    receiver(io, SOCKFILE, vsd);
                // unit_test[1] command_receiver_test normal case sockfile value check
                BOOST_CHECK_EQUAL(receiver.get_sockfile(), SOCKFILE);

                //start server
                boost::thread    sv_thd(&server_thread);

                // unit_test[2] command_receiver_test normal case create time session constructor call check
                BOOST_CHECK_EQUAL(l7vs::command_session::constructor_called, 1U);
                // unit_test[3] command_receiver_test normal case create time session::socket call check
                BOOST_CHECK_EQUAL(l7vs::command_session::socket_called, 1U);

                // accept and start called
                boost::thread thd1(&client_thread);
                thd1.join();

                // unit_test[4] command_receiver_test normal case 1st time accept session constructor call check
                BOOST_CHECK_EQUAL(l7vs::command_session::constructor_called, 2U);
                // unit_test[5] command_receiver_test normal case 1st time accept session::start call check
                BOOST_CHECK_EQUAL(l7vs::command_session::start_called, 1U);
                // unit_test[6] command_receiver_test normal case 1st time accept session::socket call check
                BOOST_CHECK_EQUAL(l7vs::command_session::socket_called, 2U);

                // second accept and start called
                boost::thread thd2(&client_thread);
                thd2.join();

                // unit_test[7] command_receiver_test normal case 2nd time accept session constructor call check
                BOOST_CHECK_EQUAL(l7vs::command_session::constructor_called, 3U);
                // unit_test[8] command_receiver_test normal case 2nd time accept session::start call check
                BOOST_CHECK_EQUAL(l7vs::command_session::start_called, 2U);
                // unit_test[9] command_receiver_test normal case 2nd time accept session::socket call check
                BOOST_CHECK_EQUAL(l7vs::command_session::socket_called, 3U);

                //stop server
                io.stop();
                sv_thd.join();
        }

        // unlink file
        boost::filesystem::path socketfile(SOCKFILE);
        // unit_test[10] command_receiver_test normal case socketfile unlink check
        BOOST_CHECK(!boost::filesystem::exists(socketfile));
}

test_suite    *init_unit_test_suite(int argc, char *argv[])
{

        test_suite *ts = BOOST_TEST_SUITE("command_receiver class test");

        l7vs::Logger    logger_instance;
        l7vs::Parameter    parameter_instance;
        logger_instance.loadConf();

        ts->add(BOOST_TEST_CASE(&receiver_test));

        framework::master_test_suite().add(ts);

        return 0;
}
