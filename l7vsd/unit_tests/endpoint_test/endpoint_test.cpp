#include <iostream>
#include <fstream>
#include <boost/test/included/unit_test.hpp>
#include <boost/asio.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include "../../include/endpoint.h"

using namespace boost::unit_test_framework;

//variable


//--tests--


void    save_load_test()
{

        // tcp ipv4 endpoint
        //save
        {
                boost::asio::ip::tcp::endpoint  ep(boost::asio::ip::address::from_string("192.168.0.1"), 22100);
                std::ofstream file("save.dat");
                boost::archive::text_oarchive oa(file);
                oa << (const boost::asio::ip::tcp::endpoint)ep;
        }

        //load
        {
                boost::asio::ip::tcp::endpoint ep;
                std::ifstream file("save.dat");
                boost::archive::text_iarchive ia(file);
                ia >> ep;

                std::stringstream   buf;
                buf << ep;
                //std::cout << ep << std::endl;
                //unit_test[1]
                BOOST_CHECK_EQUAL(buf.str(), "192.168.0.1:22100");
        }

        // udp ipv4 endpoint
        //save
        {
                boost::asio::ip::udp::endpoint  ep(boost::asio::ip::address::from_string("192.168.0.1"), 22100);
                std::ofstream file("save.dat");
                boost::archive::text_oarchive oa(file);
                oa << (const boost::asio::ip::udp::endpoint)ep;
        }

        //load
        {
                boost::asio::ip::udp::endpoint ep;
                std::ifstream file("save.dat");
                boost::archive::text_iarchive ia(file);
                ia >> ep;

                std::stringstream   buf;
                buf << ep;
                //std::cout << ep << std::endl;
                //unit_test[2]
                BOOST_CHECK_EQUAL(buf.str(), "192.168.0.1:22100");
        }

        // tcp ipv6 endpoint(global)
        //save
        {
                boost::asio::ip::tcp::endpoint  ep(boost::asio::ip::address::from_string("2001:db8::1"), 55100);
                std::ofstream file("save.dat");
                boost::archive::text_oarchive oa(file);
                oa << (const boost::asio::ip::tcp::endpoint)ep;
        }

        //load
        {
                boost::asio::ip::tcp::endpoint ep;
                std::ifstream file("save.dat");
                boost::archive::text_iarchive ia(file);
                ia >> ep;

                std::stringstream   buf;
                buf << ep;
                //std::cout << ep << std::endl;
                //unit_test[3]
                BOOST_CHECK_EQUAL(buf.str(), "[2001:db8::1]:55100");
        }

        // tcp ipv6 endpoint(lo)
        //save
        {
                boost::asio::ip::tcp::endpoint  ep(boost::asio::ip::address::from_string("::1"), 55100);
                std::ofstream file("save.dat");
                boost::archive::text_oarchive oa(file);
                oa << (const boost::asio::ip::tcp::endpoint)ep;
        }

        //load
        {
                boost::asio::ip::tcp::endpoint ep;
                std::ifstream file("save.dat");
                boost::archive::text_iarchive ia(file);
                ia >> ep;

                std::stringstream   buf;
                buf << ep;
                //std::cout << ep << std::endl;
                //unit_test[4]
                BOOST_CHECK_EQUAL(buf.str(), "[::1]:55100");
        }

        // tcp ipv6 endpoint(linklocal)
        //save
        {
                std::string addr = "fe80::213:21ff:feb5:95fb%eth0";
                std::string addr_ = addr.substr(0, addr.find_first_of("%"));
                std::string ifname = addr.substr(addr.find_first_of("%") + 1);

                boost::asio::ip::address_v6 v6addr = boost::asio::ip::address_v6::from_string(addr_);
                unsigned long scope_id = 0;
                scope_id = if_nametoindex(ifname.c_str());
                v6addr.scope_id(scope_id);

                boost::asio::ip::tcp::endpoint  ep(v6addr, 55100);

                std::ofstream file("save.dat");
                boost::archive::text_oarchive oa(file);
                oa << (const boost::asio::ip::tcp::endpoint)ep;
        }

        //load
        {
                boost::asio::ip::tcp::endpoint ep;
                std::ifstream file("save.dat");
                boost::archive::text_iarchive ia(file);
                ia >> ep;

                std::stringstream   buf;
                buf << ep;
                //std::cout << ep << std::endl;
                //unit_test[5]
                BOOST_CHECK_EQUAL(buf.str(), "[fe80::213:21ff:feb5:95fb%eth0]:55100");
        }

        // udp ipv6 endpoint
        //save
        {
                boost::asio::ip::udp::endpoint  ep(boost::asio::ip::address::from_string("2001:db8::2"), 55100);
                std::ofstream file("save.dat");
                boost::archive::text_oarchive oa(file);
                oa << (const boost::asio::ip::udp::endpoint)ep;
        }

        //load
        {
                boost::asio::ip::udp::endpoint ep;
                std::ifstream file("save.dat");
                boost::archive::text_iarchive ia(file);
                ia >> ep;

                std::stringstream   buf;
                buf << ep;
                //std::cout << ep << std::endl;
                //unit_test[6]
                BOOST_CHECK_EQUAL(buf.str(), "[2001:db8::2]:55100");
        }
}

test_suite    *init_unit_test_suite(int argc, char *argv[])
{
        test_suite *ts = BOOST_TEST_SUITE("endpoint class test");

        ts->add(BOOST_TEST_CASE(&save_load_test));

        framework::master_test_suite().add(ts);

        return 0;
}
