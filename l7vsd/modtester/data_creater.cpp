#include "data_creater.h"
#include <boost/nondet_random.hpp>
#include <boost/function.hpp>
#include <vector>
#include <cstring>

typedef    boost::function< void ( boost::array<char, MAX_BUFFER_SIZE>&, size_t&, std::string& ) > create_data_func_type;
std::vector< create_data_func_type >    http_func_vector;
std::vector< create_data_func_type >    ssl_func_vector;

void    http_data_create_1( boost::array<char, MAX_BUFFER_SIZE>& buf, size_t&    length, std::string& data_name ){
    data_name = "nomal GET http request:GET http://www.hoge.org/index.html HTTP/1.1";
    char    src[] =
    "GET http://www.hoge.org/index.html HTTP/1.1\r\n"
    "Referer: http://www.hogehoge.jp\r\n"
    "Accept-Language: ja\r\n"
    "Accept-Encoding: *\r\n"
    "Accept-Charset: *\r\n"
    "User-Argetnt: modtester\r\rn"
    "Content-Type: \r\n"
//    "Cookie: \r\n"
//    "X-Forwarded-For:"
    "\r\n";
    std::memcpy( &(buf[0]), src, sizeof( src ) );
    length = sizeof( src );
}

void    http_data_create_2( boost::array<char, MAX_BUFFER_SIZE>& buf, size_t&    length, std::string& data_name ){
    data_name = "nomal GET http request:GET / HTTP/1.1";
    char    src[] =
    "GET / HTTP/1.1\r\n"
    "Referer: http://www.hogehoge.jp\r\n"
    "Accept-Language: ja\r\n"
    "Accept-Encoding: *\r\n"
    "Accept-Charset: *\r\n"
    "User-Argetnt: modtester\r\rn"
    "Content-Type: \r\n"
//    "Cookie: \r\n"
//    "X-Forwarded-For:"
    "\r\n";
    std::memcpy( &(buf[0]), src, sizeof( src ) );
    length = sizeof( src );
}

void    http_data_create_3( boost::array<char, MAX_BUFFER_SIZE>& buf, size_t&    length, std::string& data_name ){
    data_name = "nomal GET http request with option:GET / HTTP/1.1\r\nHost www.w3.org";
    char src[] =
    "GET / HTTP/1.1\r\n"
    "Host www.sample.jp\r\n"
    "Referer: http://www.hogehoge.jp\r\n"
    "Accept-Language: ja\r\n"
    "Accept-Encoding: *\r\n"
    "Accept-Charset: *\r\n"
    "User-Argetnt: modtester\r\rn"
    "Content-Type: \r\n"
//    "Cookie: \r\n"
//    "X-Forwarded-For:"
    "\r\n";
    std::memcpy( &(buf[0]), src, sizeof( src ) );
    length = sizeof( src );
}

void    http_data_create_4( boost::array<char, MAX_BUFFER_SIZE>& buf, size_t&    length, std::string& data_name ){
    data_name = "nomal GET http request:GET / HTTP/1.1";
    char    src[] =
    "GET / HTTP/1.1\r\n"
    "Referer: http://www.hogehoge.jp\r\n"
    "Host: www.sample.jp\r\n"
    "Accept-Language: ja\r\n"
    "Accept-Encoding: *\r\n"
    "Accept-Charset: *\r\n"
    "User-Argetnt: modtester\r\rn"
    "Content-Type: application/x-www-from-encoded\r\n"
//    "Cookie: monkey=\r\n"
//    "X-Forwarded-For:"
    "\r\n";
    std::memcpy( &(buf[0]), src, sizeof( src ) );
    length = sizeof( src );
}

void    http_data_create_5( boost::array<char, MAX_BUFFER_SIZE>& buf, size_t&    length, std::string& data_name ){
    data_name = "nomal GET http request:GET / HTTP/1.0";
    char src[] =
    "GET / HTTP/1.1\r\n\r\n";
    std::memcpy( &(buf[0]), src, sizeof( src ) );
    length = sizeof( src );
}

void    http_data_create_6( boost::array<char, MAX_BUFFER_SIZE>& buf, size_t&    length, std::string& data_name ){
    data_name = "nomal PUT http request: PUT Content-Length=1024";
    char src[] =
    "PUT / HTTP/1.1\r\n"
    "Host: www.sample.jp\r\n"
    "Referer: http://www.hogehoge.jp\r\n"
    "Accept-Language: ja\r\n"
    "Accept-Encoding: *\r\n"
    "Accept-Charset: *\r\n"
    "User-Argetnt: modtester\r\rn"
    "Content-Type: application/x-www-from-encoded\r\n"
    "Content-Length: 1024\r\n"
    "\r\n"
    "0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF"
    "0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF"
    "0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF"
    "0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF"
    "0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF"
    "0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF"
    "0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF"
    "0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF"
    "0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF"
    "0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF"
    "0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF"
    "0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF"
    "0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF"
    "0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF"
    "0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF"
    "0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF"
    "\r\n";
    std::memcpy( &(buf[0]), src, sizeof( src ) );
    length = sizeof( src );
}

void    http_data_create_7( boost::array<char, MAX_BUFFER_SIZE>& buf, size_t&    length, std::string& data_name ){
    data_name = "nomal POST http request: PUT Content-Length=0";
    char src[] =
    "PUT / HTTP/1.1\r\n"
    "Host: www.sample.jp\r\n"
    "Referer: http://www.hogehoge.jp\r\n"
    "Accept-Language: ja\r\n"
    "Accept-Encoding: *\r\n"
    "Accept-Charset: *\r\n"
    "User-Argetnt: modtester\r\rn"
    "Content-Type: application/x-www-from-encoded\r\n"
    "Content-Length: 0\r\n"
    "\r\n";
    std::memcpy( &(buf[0]), src, sizeof( src ) );
    length = sizeof( src );
}

void    http_data_create_8( boost::array<char, MAX_BUFFER_SIZE>& buf, size_t&    length, std::string& data_name ){
    data_name = "nomal POST http request: POST Content-Length=1024";
    char src[] =
    "POST / HTTP/1.1\r\n"
    "Host: www.sample.jp"
    "Referer: http://www.hogehoge.jp\r\n"
    "Accept-Language: ja\r\n"
    "Accept-Encoding: *\r\n"
    "Accept-Charset: *\r\n"
    "User-Argetnt: modtester\r\rn"
    "Content-Type: application/x-www-from-encoded\r\n"
    "Content-Length: 1024\r\n"
    "\r\n"
    "0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF"
    "0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF"
    "0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF"
    "0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF"
    "0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF"
    "0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF"
    "0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF"
    "0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF"
    "0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF"
    "0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF"
    "0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF"
    "0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF"
    "0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF"
    "0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF"
    "0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF"
    "0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF"
    "\r\n";
    std::memcpy( &(buf[0]), src, sizeof( src ) );
    length = sizeof( src );
}

void    http_data_create_9( boost::array<char, MAX_BUFFER_SIZE>& buf, size_t&    length, std::string& data_name ){
    data_name = "nomal HEAD http request: HEAD / HTTP/1.1";
    char src[] =
    "HEAD / HTTP/1.1\r\n"
    "Host: www.sample.jp"
    "Referer: http://www.hogehoge.jp\r\n"
    "Accept-Language: ja\r\n"
    "Accept-Encoding: *\r\n"
    "Accept-Charset: *\r\n"
    "User-Argetnt: modtester\r\rn"
    "Content-Type: application/x-www-from-encoded\r\n"
    "\r\n";
    std::memcpy( &(buf[0]), src, sizeof( src ) );
    length = sizeof( src );
}

void    http_err_data_create_1( boost::array<char, MAX_BUFFER_SIZE>& buf, size_t&    length, std::string& data_name ){
    data_name = "error GET http request ( HTTP1.2 ) : GET / HTTP/1.2";
    char src[] =
    "GET / HTTP/1.2\r\n\r\n";
    std::memcpy( &(buf[0]), src, sizeof( src ) );
    length = sizeof( src );
}

void    http_err_data_create_2( boost::array<char, MAX_BUFFER_SIZE>& buf, size_t&    length, std::string& data_name ){
    data_name = "error GET http request ( not use URI ) : GET  HTTP/1.1";
    char src[] =
    "GET HTTP/1.1\r\n\r\n";
    std::memcpy( &(buf[0]), src, sizeof( src ) );
    length = sizeof( src );
}


void    ssl_data_create( boost::array<char, MAX_BUFFER_SIZE>& buf, size_t& length, std::string& data_name ){
    data_name = "nomal_sslclint hello request";
    char src[] = {    0x43,0x01,0x06,0x00,0x00,0x2a,0x00,0x00,0x00,0x10,0x00,0x00,0x04,
                    0x00,0x00,0x05,0x00,0x00,0x0a,0x01,0x00,0x80,0x07,0x00,0xc0,0x30 };
    std::memcpy( &(buf[0]), src, sizeof( src ) );
    length = sizeof( src );
}

void    http_err_data_create_3( boost::array<char, MAX_BUFFER_SIZE>& buf, size_t&    length, std::string& data_name ){
    data_name = "error GET http request ( not HTTP/1.1 ) : GET  HTTP/1.1";
    char src[] =
    "GET HTTP/\r\n\r\n";
    std::memcpy( &(buf[0]), src, sizeof( src ) );
    length = sizeof( src );
}

data_creater::data_creater( data_creater::mode_tag in ) : mode( in ) {
    http_func_vector.push_back( &http_data_create_1 );
    http_func_vector.push_back( &http_data_create_2 );
    http_func_vector.push_back( &http_data_create_3 );
    http_func_vector.push_back( &http_data_create_4 );
    http_func_vector.push_back( &http_data_create_5 );
    http_func_vector.push_back( &http_data_create_6 );
    http_func_vector.push_back( &http_data_create_7 );
    http_func_vector.push_back( &http_data_create_8 );
    http_func_vector.push_back( &http_data_create_9 );
    http_func_vector.push_back( &http_err_data_create_1 );
    http_func_vector.push_back( &http_err_data_create_2 );
    http_func_vector.push_back( &http_err_data_create_3 );
    ssl_func_vector.push_back( &ssl_data_create );
}

void    data_creater::create_data( boost::array<char, MAX_BUFFER_SIZE>& buf, size_t& size, std::string& name ){
    boost::random_device    dev;
    unsigned int val = dev();
    if( mode == HTTPDATA ){
        val %= http_func_vector.size();
        http_func_vector[val](buf, size, name );
    }
    else if( mode == SSL ){
        val %= ssl_func_vector.size();
        ssl_func_vector[val](buf, size, name );
    }
}

