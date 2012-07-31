#define    TEST_CASE

#include <boost/test/included/unit_test.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread.hpp>
#include "http_utility.h"

using namespace boost::unit_test_framework;
using namespace l7vs;

#define    CHECK_METHOD_OK_STRING_NUM    (16)
#define    CHECK_METHOD_NG_STRING_NUM    (8)
#define    CHECK_METHOD_IMPOSSIBLE_STRING_NUM    (3)

#define    CHECK_VERSION_OK_STRING_NUM    (10)
#define    CHECK_VERSION_NG_STRING_NUM    (16)
#define    CHECK_VERSION_IMPOSSIBLE_STRING_NUM    (4)

#define    CHECK_STATUS_CODE_OK_STRING_NUM    (6)
#define    CHECK_STATUS_CODE_NG_STRING_NUM    (9)
#define    CHECK_STATUS_CODE_IMPOSSIBLE_STRING_NUM    (2)

#define    FIND_URI_OK_STRING_NUM    (4)
#define    FIND_URI_NG_STRING_NUM    (4)

#define    FIND_STATUS_CODE_OK_STRING_NUM    (2)
#define    FIND_STATUS_CODE_NG_STRING_NUM    (4)

#define    THREAD_LOOP_NUM    (1)


//--test class--
class    http_utility_test : public http_utility
{
public:
        boost::mutex        sync_mutex;
        boost::condition    sync_condition;
        http_utility_test() {}
        ~http_utility_test() {}

        void    find_uri_test() {

                int count    = 1;

                char    disp_uri[256];

                char    buffer_ok[FIND_URI_OK_STRING_NUM][256]
                =    {
// unit_test[01]  find_uri( FindData = "GET /abc/def/ HTTP/1.0" / Result = true )
// unit_test[02]  find_uri( FindData = "GET /abc/def/ghi HTTP/1.0" / Result = true )
// unit_test[03]  find_uri( FindData = "HEAD abcdef HTTP/1.0" / Result = true )
// unit_test[04]  find_uri( FindData = "HEAD /abc/def/ghi/jkl/mno/pqr/stu/vwx/yz0/123/456/789/ HTTP/1.0" / Result = true )
                        "GET /abc/def/ HTTP/1.0",
                        "GET /abc/def/ghi HTTP/1.0",
                        "HEAD abcdef HTTP/1.0",
                        "HEAD /abc/def/ghi/jkl/mno/pqr/stu/vwx/yz0/123/456/789/ HTTP/1.0",
                };

                char    buffer_ng[FIND_URI_NG_STRING_NUM][256]
                =    {
// unit_test[05]  find_uri( FindData = "GET/abc/def/ HTTP/1.0" / Result = false )
// unit_test[06]  find_uri( FindData = "GET /abc/def/HTTP/1.0" / Result = false )
// unit_test[07]  find_uri( FindData = "HEAD/abc/def/HTTP/1.0" / Result = false )
// unit_test[08]  find_uri( FindData = "" / Result = false )
                        "GET/abc/def/ HTTP/1.0",
                        "GET /abc/def/HTTP/1.0",
                        "HEAD/abc/def/HTTP/1.0",
                        "",
                };

                size_t    buffer_len    = 0;
                size_t    uri_offset    = 0;
                size_t    uri_len        = 0;

                for (int i = 0; i < FIND_URI_OK_STRING_NUM; i++, count++) {
                        std::cout.width(2);
                        std::cout.fill('0');
                        std::cout << count << "---------------------------------------" << std::endl;
                        memset(disp_uri, '\0', 256);
                        uri_offset    = 0;
                        uri_len        = 0;
                        std::cout << "String = [" << buffer_ok[i] << "] + [CR]" << std::endl;
                        buffer_ok[i][strlen(buffer_ok[i])] = '\r';
                        buffer_len = strlen(buffer_ok[i]);
                        std::cout << "Length = [" << buffer_len << "]" << std::endl;
                        // [01] - [04]
                        BOOST_CHECK(find_uri((const char *)buffer_ok[i], buffer_len, uri_offset, uri_len) == true);
                        memcpy(disp_uri, buffer_ok[i] + uri_offset, uri_len);
                        std::cout << "URI Offset = [" << uri_offset << "]" << std::endl;
                        std::cout << "URI Length = [" << uri_len << "]" << std::endl;
                        std::cout << "URI String = [" << disp_uri << "]" << std::endl;
                }

                for (int i = 0; i < FIND_URI_NG_STRING_NUM; i++, count++) {
                        std::cout.width(2);
                        std::cout.fill('0');
                        std::cout << count << "---------------------------------------" << std::endl;
                        memset(disp_uri, '\0', 256);
                        uri_offset    = 0;
                        uri_len        = 0;
                        std::cout << "String = [" << buffer_ng[i] << "] + [CR]" << std::endl;
                        buffer_ng[i][strlen(buffer_ng[i])] = '\r';
                        buffer_len = strlen(buffer_ng[i]);
                        std::cout << "Length = [" << buffer_len << "]" << std::endl;
                        // [05] - [07]
                        BOOST_CHECK(find_uri((const char *)buffer_ng[i], buffer_len, uri_offset, uri_len) == false);
                        memcpy(disp_uri, buffer_ng[i] + uri_offset, uri_len);
                        std::cout << "URI Offset = [" << uri_offset << "]" << std::endl;
                        std::cout << "URI Length = [" << uri_len << "]" << std::endl;
                        std::cout << "URI String = [" << disp_uri << "]" << std::endl;
                }

                std::cout.width(2);
                std::cout.fill('0');
                std::cout << count << "---------------------------------------" << std::endl;
                memset(disp_uri, '\0', 256);
                uri_offset    = 0;
                uri_len        = 0;
                buffer_len    = 0;
                std::cout << "String = [NULL]" << std::endl;
                std::cout << "Length = [" << buffer_len << "]" << std::endl;
// unit_test[09]  find_uri( FindData = NULL, Length = 0 / Result = false )
                BOOST_CHECK(find_uri(NULL, buffer_len, uri_offset, uri_len) == false);
                std::cout << "URI Offset = [" << uri_offset << "]" << std::endl;
                std::cout << "URI Length = [" << uri_len << "]" << std::endl;
                std::cout << "URI String = [" << disp_uri << "]" << std::endl;
                count++;

                std::cout.width(2);
                std::cout.fill('0');
                std::cout << count << "---------------------------------------" << std::endl;
                memset(disp_uri, '\0', 256);
                uri_offset    = 0;
                uri_len        = 0;
                buffer_len    = 100;
                std::cout << "String = [NULL]" << std::endl;
                std::cout << "Length = [" << buffer_len << "]" << std::endl;
// unit_test[10]  find_uri( FindData = NULL, Length = 100 / Result = false )
                BOOST_CHECK(find_uri(NULL, buffer_len, uri_offset, uri_len) == false);
                std::cout << "URI Offset = [" << uri_offset << "]" << std::endl;
                std::cout << "URI Length = [" << uri_len << "]" << std::endl;
                std::cout << "URI String = [" << disp_uri << "]" << std::endl;
                count++;

        }

        void    find_uri_test_thread(int thread_no, bool *ret) {

                boost::xtime    start_time;
                boost::xtime    end_time;

                int        count    = 1;

                char    disp_uri[256];

                char    buffer[5][256]
                =    {
// unit_test[11]  find_uri( FindData = "GET /abc/def/ HTTP/1.0" / Result = true / ThreadNo = 1 )
// unit_test[12]  find_uri( FindData = "HEAD /abc/def/ghi/jkl/mno/pqr/stu/vwx/yz0/123/456/789/ HTTP/1.0" / Result = true / ThreadNo = 2 )
// unit_test[13]  find_uri( FindData = "GET/abc/def/ HTTP/1.0" / Result = false / ThreadNo = 3 )
// unit_test[14]  find_uri( FindData = "HEAD/abc/def/HTTP/1.0" / Result = false / ThreadNo = 4 )
// unit_test[15]  find_uri( FindData = "" / Result = false / ThreadNo = 5 )
                        "GET /abc/def/ HTTP/1.0",
                        "HEAD /abc/def/ghi/jkl/mno/pqr/stu/vwx/yz0/123/456/789/ HTTP/1.0",
                        "GET/abc/def/ HTTP/1.0",
                        "HEAD/abc/def/HTTP/1.0",
                        "",
                };

                int        edit_flag[5]
                =    {
                        1,
                        1,
                        1,
                        1,
                        0,
                };

                bool    result[5]
                =    {
                        true,
                        true,
                        false,
                        false,
                        false,
                };

                bool    find_result;

                size_t    buffer_len    = 0;
                size_t    uri_offset    = 0;
                size_t    uri_len        = 0;

                std::cout << "[Thread_" << thread_no << "] ";
                std::cout.width(2);
                std::cout.fill('0');
                std::cout << count << "---------------------------------------" << std::endl;
                memset(disp_uri, '\0', 256);
                uri_offset    = 0;
                uri_len        = 0;
                std::cout << "[Thread_" << thread_no << "] " << "String = [" << buffer[thread_no-1] << "]" << std::endl;
                if (edit_flag[thread_no-1] == 1) {
                        buffer[thread_no-1][strlen(buffer[thread_no-1])] = '\r';
                }
                buffer_len = strlen(buffer[thread_no-1]);
                std::cout << "[Thread_" << thread_no << "] " << "Length = [" << buffer_len << "]" << std::endl;
                std::cout << "[Thread_" << thread_no << "] " << "Wait..." << std::endl;
                {
                        boost::mutex::scoped_lock    lk(sync_mutex);
                        sync_condition.wait(lk);
                }
                boost::xtime_get(&start_time, boost::TIME_UTC);
                // [11] - [15]
                find_result = find_uri((const char *)buffer[thread_no-1], buffer_len, uri_offset, uri_len);
                if (find_result == result[thread_no-1]) {
                        *ret = true;
                } else {
                        *ret = false;
                }
//    BOOST_CHECK( find_uri( (const char*)buffer[thread_no-1], buffer_len, uri_offset, uri_len ) == result[thread_no-1] );
                boost::xtime_get(&end_time, boost::TIME_UTC);
                memcpy(disp_uri, buffer[thread_no-1] + uri_offset, uri_len);
                std::cout << "[Thread_" << thread_no << "] " << "URI Offset = [" << uri_offset << "]" << std::endl;
                std::cout << "[Thread_" << thread_no << "] " << "URI Length = [" << uri_len << "]" << std::endl;
                std::cout << "[Thread_" << thread_no << "] " << "URI String = [" << disp_uri << "]" << std::endl;
                std::cout << "[Thread_" << thread_no << "] " << "StartTime = [" << start_time.nsec << "]" << std::endl;
                std::cout << "[Thread_" << thread_no << "] " << "EndTime   = [" << end_time.nsec << "]" << std::endl;
                std::cout << "[Thread_" << thread_no << "] " << "CheckEnd" << std::endl;

        }


        void    find_status_code_test() {

                int count    = 1;

                char    disp_status_code[256];

                char    buffer_ok[FIND_STATUS_CODE_OK_STRING_NUM][256]
                =    {
// unit_test[01]  find_status_code( FindData = "HTTP/1.0 000 abcdff" / Result = true )
// unit_test[02]  find_status_code( FindData = "HTTP/1.0 999 abcdff" / Result = true )
                        "HTTP/1.0 000 abcdff",
                        "HTTP/1.0 999 abcdff",
                };

                char    buffer_ng[FIND_STATUS_CODE_NG_STRING_NUM][256]
                =    {
// unit_test[03]  find_status_code( FindData = "HTTP/1.0 10 abcdff" / Result = false )
// unit_test[04]  find_status_code( FindData = "HTTP/1.0 1000 abcdff" / Result = false )
// unit_test[05]  find_status_code( FindData = "HTTP/1.0 aaa abcdff" / Result = false )
// unit_test[06]  find_status_code( FindData = "" / Result = false )
                        "HTTP/1.0 10 abcdff",
                        "HTTP/1.0 1000 abcdff",
                        "HTTP/1.0 aaa abcdff",
                        "",
                };

                size_t    buffer_len            = 0;
                size_t    status_code_offset    = 0;
                size_t    status_code_len        = 0;

                for (int i = 0; i < FIND_STATUS_CODE_OK_STRING_NUM; i++, count++) {
                        std::cout.width(2);
                        std::cout.fill('0');
                        std::cout << count << "---------------------------------------" << std::endl;
                        memset(disp_status_code, '\0', 256);
                        status_code_offset    = 0;
                        status_code_len        = 0;
                        std::cout << "String = [" << buffer_ok[i] << "] + [CR]" << std::endl;
                        buffer_ok[i][strlen(buffer_ok[i])] = '\r';
                        buffer_len = strlen(buffer_ok[i]);
                        std::cout << "Length = [" << buffer_len << "]" << std::endl;
                        // [01] - [02]
                        BOOST_CHECK(find_status_code((const char *)buffer_ok[i], buffer_len, status_code_offset, status_code_len) == true);
                        memcpy(disp_status_code, buffer_ok[i] + status_code_offset, status_code_len);
                        std::cout << "STATUS CODE Offset = [" << status_code_offset << "]" << std::endl;
                        std::cout << "STATUS CODE Length = [" << status_code_len << "]" << std::endl;
                        std::cout << "STATUS CODE String = [" << disp_status_code << "]" << std::endl;
                }

                for (int i = 0; i < FIND_STATUS_CODE_NG_STRING_NUM; i++, count++) {
                        std::cout.width(2);
                        std::cout.fill('0');
                        std::cout << count << "---------------------------------------" << std::endl;
                        memset(disp_status_code, '\0', 256);
                        status_code_offset    = 0;
                        status_code_len        = 0;
                        std::cout << "String = [" << buffer_ng[i] << "] + [CR]" << std::endl;
                        buffer_ng[i][strlen(buffer_ng[i])] = '\r';
                        buffer_len = strlen(buffer_ng[i]);
                        std::cout << "Length = [" << buffer_len << "]" << std::endl;
                        // [03] - [06]
                        BOOST_CHECK(find_status_code((const char *)buffer_ng[i], buffer_len, status_code_offset, status_code_len) == false);
                        memcpy(disp_status_code, buffer_ng[i] + status_code_offset, status_code_len);
                        std::cout << "STATUS CODE Offset = [" << status_code_offset << "]" << std::endl;
                        std::cout << "STATUS CODE Length = [" << status_code_len << "]" << std::endl;
                        std::cout << "STATUS CODE String = [" << disp_status_code << "]" << std::endl;
                }

                std::cout.width(2);
                std::cout.fill('0');
                std::cout << count << "---------------------------------------" << std::endl;
                memset(disp_status_code, '\0', 256);
                status_code_offset    = 0;
                status_code_len        = 0;
                buffer_len            = 0;
                std::cout << "String = [NULL]" << std::endl;
                std::cout << "Length = [" << buffer_len << "]" << std::endl;
// unit_test[07]  find_status_code( FindData = NULL, Length = 0 / Result = false )
                BOOST_CHECK(find_status_code(NULL, buffer_len, status_code_offset, status_code_len) == false);
                std::cout << "STATUS CODE Offset = [" << status_code_offset << "]" << std::endl;
                std::cout << "STATUS CODE Length = [" << status_code_len << "]" << std::endl;
                std::cout << "STATUS CODE String = [" << disp_status_code << "]" << std::endl;
                count++;

                std::cout.width(2);
                std::cout.fill('0');
                std::cout << count << "---------------------------------------" << std::endl;
                memset(disp_status_code, '\0', 256);
                status_code_offset    = 0;
                status_code_len        = 0;
                buffer_len            = 0;
                std::cout << "String = [NULL]" << std::endl;
                std::cout << "Length = [" << buffer_len << "]" << std::endl;
// unit_test[08]  find_status_code( FindData = NULL, Length = 100 / Result = false )
                BOOST_CHECK(find_status_code(NULL, buffer_len, status_code_offset, status_code_len) == false);
                std::cout << "STATUS CODE Offset = [" << status_code_offset << "]" << std::endl;
                std::cout << "STATUS CODE Length = [" << status_code_len << "]" << std::endl;
                std::cout << "STATUS CODE String = [" << disp_status_code << "]" << std::endl;
                count++;

        }

        void    find_status_code_test_thread(int thread_no, bool *ret) {

                boost::xtime    start_time;
                boost::xtime    end_time;

                int        count    = 1;

                char    disp_status_code[256];

                char    buffer[5][256]
                =    {
// unit_test[09]  find_status_code( FindData = "HTTP/1.0 000 abcdff" / Result = true / ThreadNo = 1 )
// unit_test[10]  find_status_code( FindData = "HTTP/1.0 999 abcdff" / Result = true / ThreadNo = 2 )
// unit_test[11]  find_status_code( FindData = "HTTP/1.0 10 abcdff" / Result = false / ThreadNo = 3 )
// unit_test[12]  find_status_code( FindData = "HTTP/1.0 1000 abcdff" / Result = false / ThreadNo = 4 )
// unit_test[13]  find_status_code( FindData = "" / Result = false / ThreadNo = 5 )
                        "HTTP/1.0 000 abcdff",
                        "HTTP/1.0 999 abcdff",
                        "HTTP/1.0 10 abcdff",
                        "HTTP/1.0 1000 abcdff",
                        "",
                };

                int        edit_flag[5]
                =    {
                        1,
                        1,
                        1,
                        1,
                        0,
                };

                bool    result[5]
                =    {
                        true,
                        true,
                        false,
                        false,
                        false,
                };

                bool    find_result;

                size_t    buffer_len            = 0;
                size_t    status_code_offset    = 0;
                size_t    status_code_len        = 0;

                std::cout << "[Thread_" << thread_no << "] ";
                std::cout.width(2);
                std::cout.fill('0');
                std::cout << count << "---------------------------------------" << std::endl;
                memset(disp_status_code, '\0', 256);
                status_code_offset    = 0;
                status_code_len        = 0;
                std::cout << "[Thread_" << thread_no << "] " << "String = [" << buffer[thread_no-1] << "]" << std::endl;
                if (edit_flag[thread_no-1] == 1) {
                        buffer[thread_no-1][strlen(buffer[thread_no-1])] = '\r';
                }
                buffer_len = strlen(buffer[thread_no-1]);
                std::cout << "[Thread_" << thread_no << "] " << "Length = [" << buffer_len << "]" << std::endl;
                std::cout << "[Thread_" << thread_no << "] " << "Wait..." << std::endl;
                {
                        boost::mutex::scoped_lock    lk(sync_mutex);
                        sync_condition.wait(lk);
                }
                boost::xtime_get(&start_time, boost::TIME_UTC);
                // [09] - [13]
                find_result = find_status_code((const char *)buffer[thread_no-1], buffer_len, status_code_offset, status_code_len);
                if (find_result == result[thread_no-1]) {
                        *ret = true;
                } else {
                        *ret = false;
                }
//    BOOST_CHECK( find_status_code( (const char*)buffer[thread_no-1], buffer_len, status_code_offset, status_code_len ) == result[thread_no-1] );
                boost::xtime_get(&end_time, boost::TIME_UTC);
                memcpy(disp_status_code, buffer[thread_no-1] + status_code_offset, status_code_len);
                std::cout << "[Thread_" << thread_no << "] " << "STATUS CODE Offset = [" << status_code_offset << "]" << std::endl;
                std::cout << "[Thread_" << thread_no << "] " << "STATUS CODE Length = [" << status_code_len << "]" << std::endl;
                std::cout << "[Thread_" << thread_no << "] " << "STATUS CODE String = [" << disp_status_code << "]" << std::endl;
                std::cout << "[Thread_" << thread_no << "] " << "StartTime = [" << start_time.nsec << "]" << std::endl;
                std::cout << "[Thread_" << thread_no << "] " << "EndTime   = [" << end_time.nsec << "]" << std::endl;
                std::cout << "[Thread_" << thread_no << "] " << "CheckEnd" << std::endl;

        }

        void    find_http_header_test() {

                int        count    = 1;
                int        i        = 0;
                int        j        = 0;

                char    disp_http_header[4096];

                std::string    http_header_name[10]
                =    {
                        "Cookie",
                        "X-Forwarded-For",
                        "Cookie2",
                        "X-Forwarded-For2",
                        "",
                        "cookie",
                        "COOKIE",
                        "content-length",
                        "Content-Length",
                        "CONTENT-LENGTH",
                };

                bool    result_1[10]
                =    {
// unit_test[01]  find_http_header( FindData = "...Cookie: 10.10.10.10:11111...BlankLine...", HeaderName = "Cookie", ReturnCode = "\r" / Result = true )
// unit_test[02]  find_http_header( FindData = "...X-Forwarded-For: 20.20.20.20...BlankLine...", HeaderName = "X-Forwarded-For", ReturnCode = "\r" / Result = true )
// unit_test[03]  find_http_header( FindData = "...BlankLine...Cookie2: 30.30.30.30:33333...", HeaderName = "Cookie2", ReturnCode = "\r" / Result = false )
// unit_test[04]  find_http_header( FindData = "...BlankLine...X-Forwarded-For2: 40.40.40.40...", HeaderName = "X-Forwarded-For2", ReturnCode = "\r" / Result = false )
// unit_test[05]  find_http_header( FindData = "...BlankLine...", HeaderName = "", ReturnCode = "\r" / Result = true )
// unit_test[06]  find_http_header( FindData = "...Cookie: 10.10.10.10:11111...BlankLine...", HeaderName = "cookie", ReturnCode = "\r" / Result = true )
// unit_test[07]  find_http_header( FindData = "...Cookie: 10.10.10.10:11111...BlankLine...", HeaderName = "COOKIE", ReturnCode = "\r" / Result = true )
// unit_test[08]  find_http_header( FindData = "...CONTENT-LENGTH: 1000...BlankLine...", HeaderName = "content-length", ReturnCode = "\r" / Result = true )
// unit_test[09]  find_http_header( FindData = "...CONTENT-LENGTH: 1000...BlankLine...", HeaderName = "Content-Length", ReturnCode = "\r" / Result = true )
// unit_test[10]  find_http_header( FindData = "...CONTENT-LENGTH: 1000...BlankLine...", HeaderName = "CONTENT-LENGTH", ReturnCode = "\r" / Result = true )

// unit_test[11]  find_http_header( FindData = "...Cookie: 10.10.10.10:11111...BlankLine...", HeaderName = "Cookie", ReturnCode = "\n" / Result = true )
// unit_test[12]  find_http_header( FindData = "...X-Forwarded-For: 20.20.20.20...BlankLine...", HeaderName = "X-Forwarded-For", ReturnCode = "\n" / Result = true )
// unit_test[13]  find_http_header( FindData = "...BlankLine...Cookie2: 30.30.30.30:33333...", HeaderName = "Cookie2", ReturnCode = "\n" / Result = false )
// unit_test[14]  find_http_header( FindData = "...BlankLine...X-Forwarded-For2: 40.40.40.40...", HeaderName = "X-Forwarded-For2", ReturnCode = "\n" / Result = false )
// unit_test[15]  find_http_header( FindData = "...BlankLine...", HeaderName = "", ReturnCode = "\n" / Result = true )
// unit_test[16]  find_http_header( FindData = "...Cookie: 10.10.10.10:11111...BlankLine...", HeaderName = "cookie", ReturnCode = "\n" / Result = true )
// unit_test[17]  find_http_header( FindData = "...Cookie: 10.10.10.10:11111...BlankLine...", HeaderName = "COOKIE", ReturnCode = "\n" / Result = true )
// unit_test[18]  find_http_header( FindData = "...CONTENT-LENGTH: 1000...BlankLine...", HeaderName = "content-length", ReturnCode = "\n" / Result = true )
// unit_test[19]  find_http_header( FindData = "...CONTENT-LENGTH: 1000...BlankLine...", HeaderName = "Content-Length", ReturnCode = "\n" / Result = true )
// unit_test[20]  find_http_header( FindData = "...CONTENT-LENGTH: 1000...BlankLine...", HeaderName = "CONTENT-LENGTH", ReturnCode = "\n" / Result = true )

// unit_test[21]  find_http_header( FindData = "...Cookie: 10.10.10.10:11111...BlankLine...", HeaderName = "Cookie", ReturnCode = "\r\n" / Result = true )
// unit_test[22]  find_http_header( FindData = "...X-Forwarded-For: 20.20.20.20...BlankLine...", HeaderName = "X-Forwarded-For", ReturnCode = "\r\n" / Result = true )
// unit_test[23]  find_http_header( FindData = "...BlankLine...Cookie2: 30.30.30.30:33333...", HeaderName = "Cookie2", ReturnCode = "\r\n" / Result = false )
// unit_test[24]  find_http_header( FindData = "...BlankLine...X-Forwarded-For2: 40.40.40.40...", HeaderName = "X-Forwarded-For2", ReturnCode = "\r\n" / Result = false )
// unit_test[25]  find_http_header( FindData = "...BlankLine...", HeaderName = "", ReturnCode = "\r\n" / Result = true )
// unit_test[26]  find_http_header( FindData = "...Cookie: 10.10.10.10:11111...BlankLine...", HeaderName = "cookie", ReturnCode = "\r\n" / Result = true )
// unit_test[27]  find_http_header( FindData = "...Cookie: 10.10.10.10:11111...BlankLine...", HeaderName = "COOKIE", ReturnCode = "\r\n" / Result = true )
// unit_test[28]  find_http_header( FindData = "...CONTENT-LENGTH: 1000...BlankLine...", HeaderName = "content-length", ReturnCode = "\r\n" / Result = true )
// unit_test[29]  find_http_header( FindData = "...CONTENT-LENGTH: 1000...BlankLine...", HeaderName = "Content-Length", ReturnCode = "\r\n" / Result = true )
// unit_test[30]  find_http_header( FindData = "...CONTENT-LENGTH: 1000...BlankLine...", HeaderName = "CONTENT-LENGTH", ReturnCode = "\r\n" / Result = true )
                        true,
                        true,
                        false,
                        false,
                        true,
                        true,
                        true,
                        true,
                        true,
                        true,
                };

                bool    result_2[10]
                =    {
// unit_test[31]  find_http_header( FindData = "BlankLine...Cookie: 10.10.10.10:11111...", HeaderName = "Cookie", ReturnCode = "\r" / Result = false )
// unit_test[32]  find_http_header( FindData = "BlankLine...X-Forwarded-For: 20.20.20.20...", HeaderName = "X-Forwarded-For", ReturnCode = "\r" / Result = false )
// unit_test[33]  find_http_header( FindData = "BlankLine...Cookie2: 30.30.30.30:33333...", HeaderName = "Cookie2", ReturnCode = "\r" / Result = false )
// unit_test[34]  find_http_header( FindData = "BlankLine...X-Forwarded-For2: 40.40.40.40...", HeaderName = "X-Forwarded-For2", ReturnCode = "\r" / Result = false )
// unit_test[35]  find_http_header( FindData = "BlankLine...", HeaderName = "", ReturnCode = "\r" / Result = true )
// unit_test[36]  find_http_header( FindData = "BlankLine...Cookie: 10.10.10.10:11111...", HeaderName = "cookie", ReturnCode = "\r" / Result = false )
// unit_test[37]  find_http_header( FindData = "BlankLine...Cookie: 10.10.10.10:11111...", HeaderName = "COOKIE", ReturnCode = "\r" / Result = false )
// unit_test[38]  find_http_header( FindData = "BlankLine...CONTENT-LENGTH: 1000...", HeaderName = "content-length", ReturnCode = "\r" / Result = false )
// unit_test[39]  find_http_header( FindData = "BlankLine...CONTENT-LENGTH: 1000...", HeaderName = "Content-Length", ReturnCode = "\r" / Result = false )
// unit_test[40]  find_http_header( FindData = "BlankLine...CONTENT-LENGTH: 1000...", HeaderName = "CONTENT-LENGTH", ReturnCode = "\r" / Result = false )

// unit_test[41]  find_http_header( FindData = "BlankLine...Cookie: 10.10.10.10:11111...", HeaderName = "Cookie", ReturnCode = "\n" / Result = false )
// unit_test[42]  find_http_header( FindData = "BlankLine...X-Forwarded-For: 20.20.20.20...", HeaderName = "X-Forwarded-For", ReturnCode = "\n" / Result = false )
// unit_test[43]  find_http_header( FindData = "BlankLine...Cookie2: 30.30.30.30:33333...", HeaderName = "Cookie2", ReturnCode = "\n" / Result = false )
// unit_test[44]  find_http_header( FindData = "BlankLine...X-Forwarded-For2: 40.40.40.40...", HeaderName = "X-Forwarded-For2", ReturnCode = "\n" / Result = false )
// unit_test[45]  find_http_header( FindData = "BlankLine...", HeaderName = "", ReturnCode = "\n" / Result = true )
// unit_test[46]  find_http_header( FindData = "BlankLine...Cookie: 10.10.10.10:11111...", HeaderName = "cookie", ReturnCode = "\n" / Result = false )
// unit_test[47]  find_http_header( FindData = "BlankLine...Cookie: 10.10.10.10:11111...", HeaderName = "COOKIE", ReturnCode = "\n" / Result = false )
// unit_test[48]  find_http_header( FindData = "BlankLine...CONTENT-LENGTH: 1000...", HeaderName = "content-length", ReturnCode = "\n" / Result = false )
// unit_test[49]  find_http_header( FindData = "BlankLine...CONTENT-LENGTH: 1000...", HeaderName = "Content-Length", ReturnCode = "\n" / Result = false )
// unit_test[50]  find_http_header( FindData = "BlankLine...CONTENT-LENGTH: 1000...", HeaderName = "CONTENT-LENGTH", ReturnCode = "\n" / Result = false )

// unit_test[51]  find_http_header( FindData = "BlankLine...Cookie: 10.10.10.10:11111...", HeaderName = "Cookie", ReturnCode = "\r\n" / Result = false )
// unit_test[52]  find_http_header( FindData = "BlankLine...X-Forwarded-For: 20.20.20.20...", HeaderName = "X-Forwarded-For", ReturnCode = "\r\n" / Result = false )
// unit_test[53]  find_http_header( FindData = "BlankLine...Cookie2: 30.30.30.30:33333...", HeaderName = "Cookie2", ReturnCode = "\r\n" / Result = false )
// unit_test[54]  find_http_header( FindData = "BlankLine...X-Forwarded-For2: 40.40.40.40...", HeaderName = "X-Forwarded-For2", ReturnCode = "\r\n" / Result = false )
// unit_test[55]  find_http_header( FindData = "BlankLine...", HeaderName = "", ReturnCode = "\r\n" / Result = true )
// unit_test[56]  find_http_header( FindData = "BlankLine...Cookie: 10.10.10.10:11111...", HeaderName = "cookie", ReturnCode = "\r\n" / Result = false )
// unit_test[57]  find_http_header( FindData = "BlankLine...Cookie: 10.10.10.10:11111...", HeaderName = "COOKIE", ReturnCode = "\r\n" / Result = false )
// unit_test[58]  find_http_header( FindData = "BlankLine...CONTENT-LENGTH: 1000...", HeaderName = "content-length", ReturnCode = "\r\n" / Result = false )
// unit_test[59]  find_http_header( FindData = "BlankLine...CONTENT-LENGTH: 1000...", HeaderName = "Content-Length", ReturnCode = "\r\n" / Result = false )
// unit_test[60]  find_http_header( FindData = "BlankLine...CONTENT-LENGTH: 1000...", HeaderName = "CONTENT-LENGTH", ReturnCode = "\r\n" / Result = false )
                        false,
                        false,
                        false,
                        false,
                        true,
                        false,
                        false,
                        false,
                        false,
                        false,
                };

                char    buffer_all_1[3][4096];
                char    buffer_all_2[3][4096];

                char    buffer_line_1[8][256]
                =    {
                        "GET /abc/def/ HTTP/1.0",
                        "Cookie: 10.10.10.10:11111",
                        "X-Forwarded-For: 20.20.20.20",
                        "CONTENT-LENGTH: 1000",
                        "",
                        "GET /abc/def/ HTTP/1.0",
                        "Cookie2: 30.30.30.30:33333",
                        "X-Forwarded-For2: 40.40.40.40",
                };


                char    buffer_line_2[8][256]
                =    {
                        "GET /abc/def/ HTTP/1.0",
                        "",
                        "Cookie: 10.10.10.10:11111",
                        "X-Forwarded-For: 20.20.20.20",
                        "CONTENT-LENGTH: 1000",
                        "GET /abc/def/ HTTP/1.0",
                        "Cookie2: 30.30.30.30:33333",
                        "X-Forwarded-For2: 40.40.40.40",
                };

                size_t    buffer_len            = 0;
                size_t    http_header_offset    = 0;
                size_t    http_header_len        = 0;

                memset(buffer_all_1, '\0', sizeof(buffer_all_1));
                memset(buffer_all_2, '\0', sizeof(buffer_all_2));
                for (i = 0; i < 8; i++) {
                        for (j = 0; j < 3; j++) {
                                memcpy(buffer_all_1[j] + strlen(buffer_all_1[j]), buffer_line_1[i], strlen(buffer_line_1[i]));
                                if (j == 0 || j == 2) {
                                        buffer_all_1[j][strlen(buffer_all_1[j])] = '\r';
                                }
                                if (j == 1 || j == 2) {
                                        buffer_all_1[j][strlen(buffer_all_1[j])] = '\n';
                                }

                        }
                }

                for (i = 0; i < 8; i++) {
                        for (j = 0; j < 3; j++) {
                                memcpy(buffer_all_2[j] + strlen(buffer_all_2[j]), buffer_line_2[i], strlen(buffer_line_1[i]));
                                if (j == 0 || j == 2) {
                                        buffer_all_2[j][strlen(buffer_all_2[j])] = '\r';
                                }
                                if (j == 1 || j == 2) {
                                        buffer_all_2[j][strlen(buffer_all_2[j])] = '\n';
                                }

                        }
                }


                for (i = 0; i < 3; i++) {
                        buffer_len = strlen(buffer_all_1[i]);
                        for (j = 0; j < 10; j++, count++) {
                                std::cout.width(2);
                                std::cout.fill('0');
                                std::cout << count << "---------------------------------------" << std::endl;
                                http_header_offset    = 0;
                                http_header_len        = 0;
                                std::cout << "Length = [" << buffer_len << "]" << std::endl;
                                std::cout << "Http Header Name = [" << http_header_name[j] << "]" << std::endl;
                                std::cout << "Http Header Name Length = [" << http_header_name[j].length() << "]" << std::endl;
                                // [01] - [30]
                                BOOST_CHECK(find_http_header((const char *)buffer_all_1[i], buffer_len, http_header_name[j], http_header_offset, http_header_len) == result_1[j]);
                                memset(disp_http_header, '\0', sizeof(disp_http_header));
                                memcpy(disp_http_header, buffer_all_1[i] + http_header_offset, http_header_len);
                                std::cout << "Http Header Offset = [" << http_header_offset << "]" << std::endl;
                                std::cout << "Http Header Length = [" << http_header_len << "]" << std::endl;
                                std::cout << "Http Header String = [" << disp_http_header << "]" << std::endl;
                        }
                }

                for (i = 0; i < 3; i++) {
                        buffer_len = strlen(buffer_all_2[i]);
                        for (j = 0; j < 10; j++, count++) {
                                std::cout.width(2);
                                std::cout.fill('0');
                                std::cout << count << "---------------------------------------" << std::endl;
                                http_header_offset    = 0;
                                http_header_len        = 0;
                                std::cout << "Length = [" << buffer_len << "]" << std::endl;
                                std::cout << "Http Header Name = [" << http_header_name[j] << "]" << std::endl;
                                std::cout << "Http Header Name Length = [" << http_header_name[j].length() << "]" << std::endl;
                                // [31] - [60]
                                BOOST_CHECK(find_http_header((const char *)buffer_all_2[i], buffer_len, http_header_name[j], http_header_offset, http_header_len) == result_2[j]);
                                memset(disp_http_header, '\0', sizeof(disp_http_header));
                                memcpy(disp_http_header, buffer_all_2[i] + http_header_offset, http_header_len);
                                std::cout << "Http Header Offset = [" << http_header_offset << "]" << std::endl;
                                std::cout << "Http Header Length = [" << http_header_len << "]" << std::endl;
                                std::cout << "Http Header String = [" << disp_http_header << "]" << std::endl;
                        }
                }

                std::cout.width(2);
                std::cout.fill('0');
                std::cout << count << "---------------------------------------" << std::endl;
                http_header_offset    = 0;
                http_header_len        = 0;
                buffer_len = 0;
                std::cout << "Length = [" << buffer_len << "]" << std::endl;
                std::cout << "Http Header Name = [" << http_header_name[0] << "]" << std::endl;
                std::cout << "Http Header Name Length = [" << http_header_name[0].length() << "]" << std::endl;
// unit_test[61]  find_http_header( FindData = NULL, HeaderName = "Cookie", Length = 0 / Result = false )
                BOOST_CHECK(find_http_header(NULL, buffer_len, http_header_name[0], http_header_offset, http_header_len) == false);
                memset(disp_http_header, '\0', sizeof(disp_http_header));
                std::cout << "Http Header Offset = [" << http_header_offset << "]" << std::endl;
                std::cout << "Http Header Length = [" << http_header_len << "]" << std::endl;
                std::cout << "Http Header String = [" << disp_http_header << "]" << std::endl;
                count++;

                std::cout.width(2);
                std::cout.fill('0');
                std::cout << count << "---------------------------------------" << std::endl;
                http_header_offset    = 0;
                http_header_len        = 0;
                buffer_len = 100;
                std::cout << "Length = [" << buffer_len << "]" << std::endl;
                std::cout << "Http Header Name = [" << http_header_name[0] << "]" << std::endl;
                std::cout << "Http Header Name Length = [" << http_header_name[0].length() << "]" << std::endl;
// unit_test[62]  find_http_header( FindData = NULL, HeaderName = "Cookie", Length = 100 / Result = false )
                BOOST_CHECK(find_http_header(NULL, buffer_len, http_header_name[0], http_header_offset, http_header_len) == false);
                memset(disp_http_header, '\0', sizeof(disp_http_header));
                std::cout << "Http Header Offset = [" << http_header_offset << "]" << std::endl;
                std::cout << "Http Header Length = [" << http_header_len << "]" << std::endl;
                std::cout << "Http Header String = [" << disp_http_header << "]" << std::endl;
                count++;

        }

        void    find_http_header_test_thread(int thread_no, bool *ret) {

                boost::xtime    start_time;
                boost::xtime    end_time;

                int        count    = 1;
                int        i        = 0;

                char    disp_http_header[4096];

                std::string    http_header_name[5]
                =    {
// unit_test[63]  find_http_header( FindData = "...Cookie: 10.10.10.10:11111...BlankLine...", HeaderName = "Cookie" / Result = true / ThreadNo = 1 )
// unit_test[64]  find_http_header( FindData = "...X-Forwarded-For: 20.20.20.20...BlankLine...", HeaderName = "X-Forwarded-For" / Result = true / ThreadNo = 2 )
// unit_test[65]  find_http_header( FindData = "...BlankLine...Cookie2: 30.30.30.30:33333...", HeaderName = "Cookie2" / Result = false / ThreadNo = 3 )
// unit_test[66]  find_http_header( FindData = "...BlankLine...X-Forwarded-For2: 40.40.40.40...", HeaderName = "X-Forwarded-For2" / Result = false / ThreadNo = 4 )
// unit_test[67]  find_http_header( FindData = "...BlankLine...", HeaderName = "" / Result = true / ThreadNo = 5 )
                        "Cookie",
                        "X-Forwarded-For",
                        "Cookie2",
                        "X-Forwarded-For2",
                        "",
                };

                bool    result[5]
                =    {
                        true,
                        true,
                        false,
                        false,
                        true,
                };

                char    buffer_all[4096];

                char    buffer_line[8][256]
                =    {
                        "GET /abc/def/ HTTP/1.0",
                        "Cookie: 10.10.10.10:11111",
                        "X-Forwarded-For: 20.20.20.20",
                        "CONTENT-LENGTH: 1000",
                        "",
                        "GET /abc/def/ HTTP/1.0",
                        "Cookie2: 30.30.30.30:33333",
                        "X-Forwarded-For2: 40.40.40.40",
                };

                bool    find_result;

                size_t    buffer_len            = 0;
                size_t    http_header_offset    = 0;
                size_t    http_header_len        = 0;

                memset(buffer_all, '\0', sizeof(buffer_all));
                for (i = 0; i < 8; i++) {
                        memcpy(buffer_all + strlen(buffer_all), buffer_line[i], strlen(buffer_line[i]));
                        buffer_all[strlen(buffer_all)] = '\r';
                }

                std::cout << "[Thread_" << thread_no << "] ";
                std::cout.width(2);
                std::cout.fill('0');
                std::cout << count << "---------------------------------------" << std::endl;
                memset(disp_http_header, '\0', sizeof(disp_http_header));
                http_header_offset    = 0;
                http_header_len        = 0;
                buffer_len = strlen(buffer_all);
                std::cout << "[Thread_" << thread_no << "] " << "Length = [" << buffer_len << "]" << std::endl;
                std::cout << "[Thread_" << thread_no << "] " << "Http Header Name = [" << http_header_name[thread_no-1] << "]" << std::endl;
                std::cout << "[Thread_" << thread_no << "] " << "Http Header Name Length = [" << http_header_name[thread_no-1].length() << "]" << std::endl;
                std::cout << "[Thread_" << thread_no << "] " << "Wait..." << std::endl;
                {
                        boost::mutex::scoped_lock    lk(sync_mutex);
                        sync_condition.wait(lk);
                }
                boost::xtime_get(&start_time, boost::TIME_UTC);
                // [63] - [67]
                find_result = find_http_header((const char *)buffer_all, buffer_len, http_header_name[thread_no-1], http_header_offset, http_header_len);
                if (find_result == result[thread_no-1]) {
                        *ret = true;
                } else {
                        *ret = false;
                }
//    BOOST_CHECK( find_http_header( (const char*)buffer_all, buffer_len, http_header_name[thread_no-1], http_header_offset, http_header_len ) == result[thread_no-1] );
                boost::xtime_get(&end_time, boost::TIME_UTC);
                memcpy(disp_http_header, buffer_all + http_header_offset, http_header_len);

                std::cout << "[Thread_" << thread_no << "] " << "Http Header Offset = [" << http_header_offset << "]" << std::endl;
                std::cout << "[Thread_" << thread_no << "] " << "Http Header Length = [" << http_header_len << "]" << std::endl;
                std::cout << "[Thread_" << thread_no << "] " << "Http Header String = [" << disp_http_header << "]" << std::endl;
                std::cout << "[Thread_" << thread_no << "] " << "StartTime = [" << start_time.nsec << "]" << std::endl;
                std::cout << "[Thread_" << thread_no << "] " << "EndTime   = [" << end_time.nsec << "]" << std::endl;
                std::cout << "[Thread_" << thread_no << "] " << "CheckEnd" << std::endl;

        }

        void    check_http_method_and_version_test() {

                int        count    = 1;

                char    buffer_ok[30][256]
                =    {
                        "GET /abc/def/ HTTP/1.0",
                        "HEAD /abc/def/ HTTP/1.0",
                        "POST /abc/def/ HTTP/1.0",
                        "PUT /abc/def/ HTTP/1.0",
                        "PROPFIND /abc/def/ HTTP/1.0",
                        "PROPPATCH /abc/def/ HTTP/1.0",
                        "OPTIONS /abc/def/ HTTP/1.0",
                        "CONNECT /abc/def/ HTTP/1.0",
                        "COPY /abc/def/ HTTP/1.0",
                        "TRACE /abc/def/ HTTP/1.0",
                        "DELETE /abc/def/ HTTP/1.0",
                        "LOCK /abc/def/ HTTP/1.0",
                        "UNLOCK /abc/def/ HTTP/1.0",
                        "MOVE /abc/def/ HTTP/1.0",
                        "MKCOL /abc/def/ HTTP/1.0",
                        "GET /abc/def/ HTTP/1.1",
                        "HEAD /abc/def/ HTTP/1.1",
                        "POST /abc/def/ HTTP/1.1",
                        "PUT /abc/def/ HTTP/1.1",
                        "PROPFIND /abc/def/ HTTP/1.1",
                        "PROPPATCH /abc/def/ HTTP/1.1",
                        "OPTIONS /abc/def/ HTTP/1.1",
                        "CONNECT /abc/def/ HTTP/1.1",
                        "COPY /abc/def/ HTTP/1.1",
                        "TRACE /abc/def/ HTTP/1.1",
                        "DELETE /abc/def/ HTTP/1.1",
                        "LOCK /abc/def/ HTTP/1.1",
                        "UNLOCK /abc/def/ HTTP/1.1",
                        "MOVE /abc/def/ HTTP/1.1",
                        "MKCOL /abc/def/ HTTP/1.1",
                };
                char    buffer_ng[16][256]
                =    {
                        "get /abc/def/ HTTP/1.0",
                        "Get /abc/def/ HTTP/1.0",
                        "GET/abc/def/ HTTP/1.0",
                        "GGET /abc/def/ HTTP/1.0",
                        " GET /abc/def/ HTTP/1.0",
                        "get GET /abc/def/ HTTP/1.0",
                        "get /abc/GET /abc/def/ HTTP/1.0",
                        "GET /abc/def/ HTTP/1.0 GET /abc/def/ HTTP/1.0",
                        "GET /abc/def/ HTTP/0.0",
                        "GET /abc/def/ HTTP/0.9",
                        "GET /abc/def/ HTTP/1.2",
                        "GET /abc/def/ HTTP/2.0",
                        "GET /abc/def/ghiHTTP/1.0",
                        "GET/abc/def/ghi HTTP/1.1",
                        "/abc/def/ghi HTTP/1.0",
                        "GET /abc/def/ http/1.0",
                };
                /*    char    buffer_http_utility[5][256]
                                =    {
                                        "GET / HTTP/1.0",
                                        "GET / HTTP/1.1",
                                        "Get / HTTP/1.0",
                                        "GET / HTTP/1.2",
                                        "",
                                    };    メモリ確保用のエラーがなくなったため不要*/
                size_t    buffer_len    = 0;

                for (int i = 0; i < 30; i++, count++) {
                        std::cout.width(2);
                        std::cout.fill('0');
                        std::cout << count << "---------------------------------------" << std::endl;
                        std::cout << "String = [" << buffer_ok[i] << "] + [CR]" << std::endl;
                        buffer_ok[i][strlen(buffer_ok[i])] = '\r';
                        buffer_len = strlen(buffer_ok[i]);
                        std::cout << "Length = [" << buffer_len << "]" << std::endl;
                        BOOST_CHECK(check_http_method_and_version(
                                            (const char *)buffer_ok[i], buffer_len) == CHECK_OK);
                }

                for (int i = 0; i < 16; i++, count++) {
                        std::cout.width(2);
                        std::cout.fill('0');
                        std::cout << count << "---------------------------------------" << std::endl;
                        std::cout << "String = [" << buffer_ng[i] << "] + [CR]" << std::endl;
                        buffer_ng[i][strlen(buffer_ng[i])] = '\r';
                        buffer_len = strlen(buffer_ng[i]);
                        std::cout << "Length = [" << buffer_len << "]" << std::endl;
                        BOOST_CHECK(check_http_method_and_version(
                                            (const char *)buffer_ng[i], buffer_len) == CHECK_NG);
                }

                /*malloc errorがなくなったため、テスト不要
                    for( int i = 0; i < 5; i++, count++ ){
                        std::cout.width(2);
                        std::cout.fill('0');
                        std::cout << count << "---------------------------------------" << std::endl;
                        std::cout << "String = [" << buffer_http_utility[i] << "]" << std::endl;
                        buffer_len = strlen( buffer_http_utility[i] );
                        std::cout << "Length = [" << buffer_len << "]" << std::endl;
                        BOOST_CHECK( check_http_method_and_version(
                            (const char*)buffer_http_utility[i], buffer_len ) == CHECK_IMPOSSIBLE );
                    }
                */
                std::cout.width(2);
                std::cout.fill('0');
                std::cout << count << "---------------------------------------" << std::endl;
                buffer_len = 0;
                std::cout << "String = [NULL]" << std::endl;
                std::cout << "Length = [" << buffer_len << "]" << std::endl;
                BOOST_CHECK(check_http_method_and_version(NULL, buffer_len) == CHECK_NG);
                count++;

                std::cout.width(2);
                std::cout.fill('0');
                std::cout << count << "---------------------------------------" << std::endl;
                buffer_len = 100;
                std::cout << "String = [NULL]" << std::endl;
                std::cout << "Length = [" << buffer_len << "]" << std::endl;
                BOOST_CHECK(check_http_method_and_version(NULL, buffer_len) == CHECK_NG);
                count++;

        }

        void    check_http_version_and_status_code_test() {

                int        count    = 1;

                char    buffer_ok[4][256]
                =    {
                        "HTTP/1.0 100 abcdff",
                        "HTTP/1.1 100 abcdff",
                        "HTTP/1.0 399 abcdff",
                        "HTTP/1.1 399 abcdff",
                };
                char    buffer_ng[10][256]
                =    {
                        "HTTP/0.0 100 abcdff",
                        "HTTP/0.9 100 abcdff",
                        "HTTP/1.2 100 abcdff",
                        "HTTP/2.0 100 abcdff",
                        "HTTP/1.0 100abcdff",
                        "HTTP/1.1100 abcdff",
                        "HTTP/1.1100abcdff",
                        "http/1.0 100 abcdff",
                        "HTTP/1.0 099 abcdff",
                        "HTTP/1.0 400 abcdff",
                };
                char    buffer_http_utility[2][256]
                =    {
                        "HTTP/1.0 100 abcdff",
                        "",
                };
                size_t    buffer_len    = 0;

                for (int i = 0; i < 4; i++, count++) {
                        std::cout.width(2);
                        std::cout.fill('0');
                        std::cout << count << "---------------------------------------" << std::endl;
                        std::cout << "String = [" << buffer_ok[i] << "] + [CR]" << std::endl;
                        buffer_ok[i][strlen(buffer_ok[i])] = '\r';
                        buffer_len = strlen(buffer_ok[i]);
                        std::cout << "Length = [" << buffer_len << "]" << std::endl;
                        BOOST_CHECK(check_http_version_and_status_code(
                                            (const char *)buffer_ok[i], buffer_len) == CHECK_OK);
                }

                for (int i = 0; i < 10; i++, count++) {
                        std::cout.width(2);
                        std::cout.fill('0');
                        std::cout << count << "---------------------------------------" << std::endl;
                        std::cout << "String = [" << buffer_ng[i] << "] + [CR]" << std::endl;
                        buffer_ng[i][strlen(buffer_ng[i])] = '\r';
                        buffer_len = strlen(buffer_ng[i]);
                        std::cout << "Length = [" << buffer_len << "]" << std::endl;
                        BOOST_CHECK(check_http_version_and_status_code(
                                            (const char *)buffer_ng[i], buffer_len) == CHECK_NG);
                }

                for (int i = 0; i < 2; i++, count++) {
                        std::cout.width(2);
                        std::cout.fill('0');
                        std::cout << count << "---------------------------------------" << std::endl;
                        std::cout << "String = [" << buffer_http_utility[i] << "]" << std::endl;
                        buffer_len = strlen(buffer_http_utility[i]);
                        std::cout << "Length = [" << buffer_len << "]" << std::endl;
                        BOOST_CHECK(check_http_version_and_status_code(
                                            (const char *)buffer_http_utility[i], buffer_len) == CHECK_IMPOSSIBLE);
                }

                std::cout.width(2);
                std::cout.fill('0');
                std::cout << count << "---------------------------------------" << std::endl;
                buffer_len = 0;
                std::cout << "String = [NULL]" << std::endl;
                std::cout << "Length = [" << buffer_len << "]" << std::endl;
                BOOST_CHECK(check_http_version_and_status_code(NULL, buffer_len) == CHECK_NG);
                count++;

                std::cout.width(2);
                std::cout.fill('0');
                std::cout << count << "---------------------------------------" << std::endl;
                buffer_len = 100;
                std::cout << "String = [NULL]" << std::endl;
                std::cout << "Length = [" << buffer_len << "]" << std::endl;
                BOOST_CHECK(check_http_version_and_status_code(NULL, buffer_len) == CHECK_NG);
                count++;

        }

        void    find_http_header_others_test() {

                int        count    = 1;
                int        i        = 0;
                int        j        = 0;

                char    disp_http_header[4096];

                char    buffer_all_1[3][4096];
                char    buffer_all_2[3][4096];

                char    buffer_line_1[8][256]
                =    {
                        "GET /abc/def/ HTTP/1.0",
                        "Cookie: 10.10.10.10:11111",
                        "X-Forwarded-For: 20.20.20.20",
                        "CONTENT-LENGTH: 1000",
                        "",
                        "GET /abc/def/ HTTP/1.0",
                        "Cookie2: 30.30.30.30:33333",
                        "X-Forwarded-For2: 40.40.40.40",
                };


                char    buffer_line_2[8][256]
                =    {
                        "GET /abc/def/ HTTP/1.0",
                        "",
                        "Cookie: 10.10.10.10:11111",
                        "X-Forwarded-For: 20.20.20.20",
                        "CONTENT-LENGTH: 1000",
                        "GET /abc/def/ HTTP/1.0",
                        "Cookie2: 30.30.30.30:33333",
                        "X-Forwarded-For2: 40.40.40.40",
                };

                size_t    buffer_len            = 0;
                size_t    http_header_offset    = 0;
                size_t    http_header_len        = 0;

                memset(buffer_all_1, '\0', sizeof(buffer_all_1));
                memset(buffer_all_2, '\0', sizeof(buffer_all_2));
                for (i = 0; i < 8; i++) {
                        for (j = 0; j < 3; j++) {
                                memcpy(buffer_all_1[j] + strlen(buffer_all_1[j]), buffer_line_1[i], strlen(buffer_line_1[i]));
                                if (j == 0 || j == 2) {
                                        buffer_all_1[j][strlen(buffer_all_1[j])] = '\r';
                                }
                                if (j == 1 || j == 2) {
                                        buffer_all_1[j][strlen(buffer_all_1[j])] = '\n';
                                }

                        }
                }

                for (i = 0; i < 8; i++) {
                        for (j = 0; j < 3; j++) {
                                memcpy(buffer_all_2[j] + strlen(buffer_all_2[j]), buffer_line_2[i], strlen(buffer_line_1[i]));
                                if (j == 0 || j == 2) {
                                        buffer_all_2[j][strlen(buffer_all_2[j])] = '\r';
                                }
                                if (j == 1 || j == 2) {
                                        buffer_all_2[j][strlen(buffer_all_2[j])] = '\n';
                                }

                        }
                }

                for (i = 0; i < 3; i++) {
                        buffer_len = strlen(buffer_all_1[i]);
                        for (j = 0; j < 4; j++) {
                                std::cout.width(2);
                                std::cout.fill('0');
                                std::cout << count << "---------------------------------------" << std::endl;
                                count++;
                                http_header_offset    = 0;
                                http_header_len        = 0;
                                std::cout << "Length = [" << buffer_len << "]" << std::endl;

                                if (j == 0) {
                                        std::cout << "MethodName = [find_http_header_cookie]" << std::endl;
                                        BOOST_CHECK(find_http_header_cookie((const char *)buffer_all_1[i], buffer_len, http_header_offset, http_header_len) == true);
                                } else if (j == 1) {
                                        std::cout << "MethodName = [find_http_header_content_length]" << std::endl;
                                        BOOST_CHECK(find_http_header_content_length((const char *)buffer_all_1[i], buffer_len, http_header_offset, http_header_len) == true);
                                } else if (j == 2) {
                                        std::cout << "MethodName = [find_http_header_x_forwarded_for]" << std::endl;
                                        BOOST_CHECK(find_http_header_x_forwarded_for((const char *)buffer_all_1[i], buffer_len, http_header_offset, http_header_len) == true);
                                } else {
                                        std::cout << "MethodName = [find_http_header_all]" << std::endl;
                                        BOOST_CHECK(find_http_header_all((const char *)buffer_all_1[i], buffer_len, http_header_offset, http_header_len) == true);
                                }

                                memset(disp_http_header, '\0', sizeof(disp_http_header));
                                memcpy(disp_http_header, buffer_all_1[i] + http_header_offset, http_header_len);
                                std::cout << "Http Header Offset = [" << http_header_offset << "]" << std::endl;
                                std::cout << "Http Header Length = [" << http_header_len << "]" << std::endl;
                                std::cout << "Http Header String = [" << disp_http_header << "]" << std::endl;
                        }
                }

                for (i = 0; i < 3; i++) {
                        buffer_len = strlen(buffer_all_2[i]);
                        for (j = 0; j < 4; j++) {
                                std::cout.width(2);
                                std::cout.fill('0');
                                std::cout << count << "---------------------------------------" << std::endl;
                                count++;
                                http_header_offset    = 0;
                                http_header_len        = 0;
                                std::cout << "Length = [" << buffer_len << "]" << std::endl;

                                if (j == 0) {
                                        std::cout << "MethodName = [find_http_header_cookie]" << std::endl;
                                        BOOST_CHECK(find_http_header_cookie((const char *)buffer_all_2[i], buffer_len, http_header_offset, http_header_len) == false);
                                } else if (j == 1) {
                                        std::cout << "MethodName = [find_http_header_content_length]" << std::endl;
                                        BOOST_CHECK(find_http_header_content_length((const char *)buffer_all_2[i], buffer_len, http_header_offset, http_header_len) == false);
                                } else if (j == 2) {
                                        std::cout << "MethodName = [find_http_header_x_forwarded_for]" << std::endl;
                                        BOOST_CHECK(find_http_header_x_forwarded_for((const char *)buffer_all_2[i], buffer_len, http_header_offset, http_header_len) == false);
                                } else {
                                        std::cout << "MethodName = [find_http_header_all]" << std::endl;
                                        BOOST_CHECK(find_http_header_all((const char *)buffer_all_2[i], buffer_len, http_header_offset, http_header_len) == true);
                                }

                                memset(disp_http_header, '\0', sizeof(disp_http_header));
                                memcpy(disp_http_header, buffer_all_2[i] + http_header_offset, http_header_len);
                                std::cout << "Http Header Offset = [" << http_header_offset << "]" << std::endl;
                                std::cout << "Http Header Length = [" << http_header_len << "]" << std::endl;
                                std::cout << "Http Header String = [" << disp_http_header << "]" << std::endl;
                        }
                }
        }

};


//-------------------------------------------------------------------
void    find_uri_test_thread(http_utility_test *http_utility_test_1, int thread_no, bool *ret)
{

        for (int i = 0; i < THREAD_LOOP_NUM; i++) {
                http_utility_test_1->find_uri_test_thread(thread_no, ret);
        }

}
void nn()
{
}
void    find_uri_test()
{

        bool    thread_1;
        bool    thread_2;
        bool    thread_3;
        bool    thread_4;
        bool    thread_5;

        http_utility_test    http_utility_test_1;

        BOOST_MESSAGE("----- find_uri test start -----");
        http_utility_test_1.find_uri_test();
        BOOST_MESSAGE("----- find_uri test end -----");

        BOOST_MESSAGE("----- find_uri test multi thread start -----");
        boost::thread    t1(boost::bind(&find_uri_test_thread,
                                        &http_utility_test_1, 1, &thread_1));
        boost::thread    t2(boost::bind(&find_uri_test_thread,
                                        &http_utility_test_1, 2, &thread_2));
        boost::thread    t3(boost::bind(&find_uri_test_thread,
                                        &http_utility_test_1, 3, &thread_3));
        boost::thread    t4(boost::bind(&find_uri_test_thread,
                                        &http_utility_test_1, 4, &thread_4));
        boost::thread    t5(boost::bind(&find_uri_test_thread,
                                        &http_utility_test_1, 5, &thread_5));

        sleep(1);

        http_utility_test_1.sync_condition.notify_all();

        t1.join();
        t2.join();
        t3.join();
        t4.join();
        t5.join();

        BOOST_CHECK(thread_1 == true);
        BOOST_CHECK(thread_2 == true);
        BOOST_CHECK(thread_3 == true);
        BOOST_CHECK(thread_4 == true);
        BOOST_CHECK(thread_5 == true);

        BOOST_MESSAGE("----- find_uri test multi thread end -----");
}
//-------------------------------------------------------------------
void    find_status_code_test_thread(http_utility_test *http_utility_test_1, int thread_no, bool *ret)
{

        for (int i = 0; i < THREAD_LOOP_NUM; i++) {
                http_utility_test_1->find_status_code_test_thread(thread_no, ret);
        }

}
void    find_status_code_test()
{

        bool    thread_1;
        bool    thread_2;
        bool    thread_3;
        bool    thread_4;
        bool    thread_5;

        http_utility_test    http_utility_test_1;

        BOOST_MESSAGE("----- find_status_code test start -----");
        http_utility_test_1.find_status_code_test();
        BOOST_MESSAGE("----- find_status_code test end -----");

        BOOST_MESSAGE("----- find_status_code test multi thread start -----");
        boost::thread    t1(boost::bind(&find_status_code_test_thread,
                                        &http_utility_test_1, 1, &thread_1));
        boost::thread    t2(boost::bind(&find_status_code_test_thread,
                                        &http_utility_test_1, 2, &thread_2));
        boost::thread    t3(boost::bind(&find_status_code_test_thread,
                                        &http_utility_test_1, 3, &thread_3));
        boost::thread    t4(boost::bind(&find_status_code_test_thread,
                                        &http_utility_test_1, 4, &thread_4));
        boost::thread    t5(boost::bind(&find_status_code_test_thread,
                                        &http_utility_test_1, 5, &thread_5));

        sleep(1);

        http_utility_test_1.sync_condition.notify_all();

        t1.join();
        t2.join();
        t3.join();
        t4.join();
        t5.join();

        BOOST_CHECK(thread_1 == true);
        BOOST_CHECK(thread_2 == true);
        BOOST_CHECK(thread_3 == true);
        BOOST_CHECK(thread_4 == true);
        BOOST_CHECK(thread_5 == true);

        BOOST_MESSAGE("----- find_status_code test multi thread end -----");
}
//-------------------------------------------------------------------
void    find_http_header_test_thread(http_utility_test *http_utility_test_1, int thread_no, bool *ret)
{

        for (int i = 0; i < THREAD_LOOP_NUM; i++) {
                http_utility_test_1->find_http_header_test_thread(thread_no, ret);
        }

}
void    find_http_header_test()
{

        bool    thread_1;
        bool    thread_2;
        bool    thread_3;
        bool    thread_4;
        bool    thread_5;

        http_utility_test    http_utility_test_1;

        BOOST_MESSAGE("----- find_http_header test start -----");
        http_utility_test_1.find_http_header_test();
        BOOST_MESSAGE("----- find_http_header test end -----");

        BOOST_MESSAGE("----- find_http_header test multi thread start -----");
        boost::thread    t1(boost::bind(&find_http_header_test_thread,
                                        &http_utility_test_1, 1, &thread_1));
        boost::thread    t2(boost::bind(&find_http_header_test_thread,
                                        &http_utility_test_1, 2, &thread_2));
        boost::thread    t3(boost::bind(&find_http_header_test_thread,
                                        &http_utility_test_1, 3, &thread_3));
        boost::thread    t4(boost::bind(&find_http_header_test_thread,
                                        &http_utility_test_1, 4, &thread_4));
        boost::thread    t5(boost::bind(&find_http_header_test_thread,
                                        &http_utility_test_1, 5, &thread_5));

        sleep(1);

        http_utility_test_1.sync_condition.notify_all();

        t1.join();
        t2.join();
        t3.join();
        t4.join();
        t5.join();

        BOOST_CHECK(thread_1 == true);
        BOOST_CHECK(thread_2 == true);
        BOOST_CHECK(thread_3 == true);
        BOOST_CHECK(thread_4 == true);
        BOOST_CHECK(thread_5 == true);

        BOOST_MESSAGE("----- find_http_header test multi thread end -----");
}

void    check_http_method_and_version_test()
{

        http_utility_test    http_utility_test_1;

        BOOST_MESSAGE("----- check_http_method_and_version test start -----");
        http_utility_test_1.check_http_method_and_version_test();
        BOOST_MESSAGE("----- check_http_method_and_version test end -----");

}

void    check_http_version_and_status_code_test()
{

        http_utility_test    http_utility_test_1;

        BOOST_MESSAGE("----- check_http_version_and_status_code test start -----");
        http_utility_test_1.check_http_version_and_status_code_test();
        BOOST_MESSAGE("----- check_http_version_and_status_code test end -----");

}

void    find_http_header_others_test()
{

        http_utility_test    http_utility_test_1;

        BOOST_MESSAGE("----- find_http_header others test start -----");
        http_utility_test_1.find_http_header_others_test();
        BOOST_MESSAGE("----- find_http_header others test end -----");

}

//-------------------------------------------------------------------
test_suite    *init_unit_test_suite(int argc, char *argv[])
{

        test_suite *ts = BOOST_TEST_SUITE("http_utility class test");

        ts->add(BOOST_TEST_CASE(&find_uri_test));
        ts->add(BOOST_TEST_CASE(&find_status_code_test));
        ts->add(BOOST_TEST_CASE(&find_http_header_test));
        ts->add(BOOST_TEST_CASE(&check_http_method_and_version_test));
        ts->add(BOOST_TEST_CASE(&check_http_version_and_status_code_test));
        ts->add(BOOST_TEST_CASE(&find_http_header_others_test));

        framework::master_test_suite().add(ts);

        return 0;
}
