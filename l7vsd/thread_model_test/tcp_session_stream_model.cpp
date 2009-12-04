//#define _DEBUG_MODE_

#include <boost/thread/thread.hpp>
#include "l7vsd_threadtest.h"
#include "tcp_session_stream_model.h"

namespace l7vs{

    //! construcor
    tcp_session_stream_model::tcp_session_stream_model(virtualservice_tcp* pService,boost::asio::io_service& io,boost::asio::ip::tcp::endpoint rs_endpoint):
                tcp_session_base(pService,io,rs_endpoint),
                threadA_state(0),
                threadB_state(0),
                exit_flag(false){
    }

    //! destructor
    tcp_session_stream_model::~tcp_session_stream_model(){
    }

        void tcp_session_stream_model::Run_main(){
                Run_up();
                pVs->release_session(this);
        };
        void tcp_session_stream_model::Run_sub(){
                Run_dw();
        };

        void tcp_session_stream_model::Run_up(){

#ifdef _DEBUG_MODE_
                std::cout << "tcp_session_stream_model::Run_up start\n";
#endif
                boost::array< char , 65535> buff;
                boost::system::error_code ec;

                {
                        rw_scoped_lock scope_lock(threadA_state_mutex);
                        threadA_state = 1;
                }

                //DWスレッド待機
                while(true){
                        {
                                rd_scoped_lock scope_lock(threadB_state_mutex);
                                if(threadB_state == 1){
                                        break;
                                }
                        }
                        boost::this_thread::yield();
                }

                // ノンブロッキングに設定
                boost::asio::socket_base::non_blocking_io cmd(true);
                cl_socket.io_control(cmd,ec);
                if(ec){
                        //ノンブロッキング設定失敗 -> 終了のお知らせ
#ifdef _DEBUG_MODE_
                        std::cout << "tcp_session_stream_model::Run_up cl_socket.io_control error[";
                        std::cout << ec.message();
                        std::cout << "]\n";
#endif
                        rw_scoped_lock scope_lock(exit_flag_update_mutex);
                        exit_flag = true;
                }

#ifdef _DEBUG_MODE_
                std::cout << "tcp_session_stream_model::Run_up Loop start\n";
#endif
                while(true){// UPスレッドメインループ
                        boost::this_thread::yield();
                        {
                rd_scoped_lock scope_lock(exit_flag_update_mutex);
                                if(exit_flag){
                                        //終了のお知らせ
                                        break;
                                }
                        }

                        // 受信
                        size_t recv_size = 0;
                        {
                                rd_scoped_lock scope_lock(cl_close_mutex);
                                recv_size = cl_socket.read_some(boost::asio::buffer(buff.data(),65535), ec);
                        }
                        if(ec){
                                // 受信エラー
                                if(ec == boost::asio::error::try_again){
                                        continue;
                                }else{
                                        //受信失敗終了のお知らせ
#ifdef _DEBUG_MODE_
                                        std::cout << "tcp_session_stream_model::Run_up cl_socket.read_some error[";
                                        std::cout << ec.message();
                                        std::cout << "]\n";
#endif
                                        rw_scoped_lock scope_lock(exit_flag_update_mutex);
                                        exit_flag = true;
                                        break;
                                }
                        }else{
#ifdef _DEBUG_MODE_
                                if(recv_size > 0){
                                        std::cout << "tcp_session_stream_model::Run_up cl_socket.read_some receive data size[";
                                        std::cout << recv_size;
                                        std::cout << "]\n";
                                }
#endif
                        }

                        // RS接続チェック
                        if(!rs_socket.is_open()){
                                //RS接続
                                {
                                        rw_scoped_lock scope_lock(rs_close_mutex);
                                        rs_socket.connect(realserver_endpoint,ec);
                                        if(ec){
                                                //接続失敗 -> 終了のお知らせ
#ifdef _DEBUG_MODE_
                                                std::cout << "tcp_session_stream_model::Run_up rs_socket.connect error[";
                                                std::cout << ec.message();
                                                std::cout << "]\n";
#endif
                                                rw_scoped_lock scope_lock(exit_flag_update_mutex);
                                                exit_flag = true;
                                                break;
                                        }else{
#ifdef _DEBUG_MODE_
                                                std::cout << "tcp_session_stream_model::Run_up rs_socket.connect OK\n";
#endif
                                        }
                                }
                                // ノンブロッキングに設定
                                boost::asio::socket_base::non_blocking_io cmd(true);
                                rs_socket.io_control(cmd,ec);
                                if(ec){
                                        //ノンブロッキング設定失敗 -> 終了のお知らせ
#ifdef _DEBUG_MODE_
                                        std::cout << "tcp_session_stream_model::Run_up rs_socket.io_control error[";
                                        std::cout << ec.message();
                                        std::cout << "]\n";
#endif
                                        rw_scoped_lock scope_lock(exit_flag_update_mutex);
                                        exit_flag = true;
                                        break;
                                }

                        }

                        // 送信
                        size_t send_size = 0;
                        while(send_size < recv_size){
                                size_t ret_size = 0;
                                {
                                        rd_scoped_lock scope_lock(rs_close_mutex);
                                        ret_size = rs_socket.write_some(boost::asio::buffer(buff.data() + send_size,recv_size - send_size), ec);
                                }
                                if(!ec){
                                // 送信成功
                                        send_size += ret_size;
#ifdef _DEBUG_MODE_
                                        if(ret_size > 0){
                                                std::cout << "tcp_session_stream_model::Run_up rs_socket.write_some send data size[";
                                                std::cout << ret_size;
                                                std::cout << "]\n";
                                        }
#endif

                                }else{
                                        if( ec != boost::asio::error::try_again ){
                                                //送信失敗 -> 終了のお知らせ
#ifdef _DEBUG_MODE_
                                                std::cout << "tcp_session_stream_model::Run_up rs_socket.write_some error[";
                                                std::cout << ec.message();
                                                std::cout << "]\n";
#endif
                                                rw_scoped_lock scope_lock(exit_flag_update_mutex);
                                                exit_flag = true;
                                                break;
                                        }
                                }
                                boost::this_thread::yield();
                        }
                }
#ifdef _DEBUG_MODE_
                std::cout << "tcp_session_stream_model::Run_up Loop end\n";
#endif


                // クローズ
                {
                        rw_scoped_lock scope_lock(cl_close_mutex);
                        cl_socket.close(ec);
                }

#ifdef _DEBUG_MODE_
                std::cout << "tcp_session_stream_model::Run_up cl_socket.close\n";
#endif

                //DWスレッド待機
               while(true){
                        {
                                rd_scoped_lock scope_lock(threadB_state_mutex);
                                if(threadB_state == 0){
                                        break;
                                }
                        }
                        boost::this_thread::yield();
                }
                exit_flag = false;

#ifdef _DEBUG_MODE_
                std::cout << "tcp_session_stream_model::Run_up end\n";
#endif
        }

        void tcp_session_stream_model::Run_dw(){
#ifdef _DEBUG_MODE_
                std::cout << "tcp_session_stream_model::Run_dw start\n";
#endif
                boost::array< char , 65535> buff;
                boost::system::error_code ec;

                {
                        rw_scoped_lock scope_lock(threadB_state_mutex);
                        threadB_state = 1;
                }
                //UPスレッド待機
                while(true){ 
                        {
                                rd_scoped_lock scope_lock(threadA_state_mutex);
                                if(threadA_state == 1){
                                        break;
                                }
                        }
                        boost::this_thread::yield();
                }
                while(true){// DWスレッドメインループ
                        boost::this_thread::yield();
                        {
                rd_scoped_lock scope_lock(exit_flag_update_mutex);
                                if(exit_flag == true){
                                        //終了のお知らせ
                                        break;
                                }
                        }

                        // RS接続チェック
                        if(!rs_socket.is_open()){
                                continue;
                        }
                        // RS受信
                        size_t recv_size = 0;
                        {
                                rd_scoped_lock scope_lock(rs_close_mutex);
                                recv_size = rs_socket.read_some(boost::asio::buffer(buff.data(),65535), ec);
                        }
                        if(ec){
                                // 受信エラー
                                if(ec == boost::asio::error::try_again){
                                        continue;
                                }else{
                                        //受信失敗終了のお知らせ
#ifdef _DEBUG_MODE_
                                        std::cout << "tcp_session_stream_model::Run_dw rs_socket.read_some error[";
                                        std::cout << ec.message();
                                        std::cout << "]\n";
#endif
                                        rw_scoped_lock scope_lock(exit_flag_update_mutex);
                                        exit_flag = true;
                                        break;
                                }
                        }else{
#ifdef _DEBUG_MODE_
                                if(recv_size > 0){
                                        std::cout << "tcp_session_stream_model::Run_dw rs_socket.read_some receive data size[";
                                        std::cout << recv_size;
                                        std::cout << "]\n";
                                }
#endif
                        }


                        // CL送信
                        size_t send_size = 0;
                        while(send_size < recv_size){
                                size_t ret_size = 0;
                                {
                                        rd_scoped_lock scope_lock(cl_close_mutex);
                                        ret_size = cl_socket.write_some(boost::asio::buffer(buff.data() + send_size,recv_size - send_size), ec);
                                }
                                if(!ec){
                                // 送信成功
                                        send_size += ret_size;
#ifdef _DEBUG_MODE_
                                        if(ret_size > 0){
                                                std::cout << "tcp_session_stream_model::Run_up cl_socket.write_some send data size[";
                                                std::cout << ret_size;
                                                std::cout << "]\n";
                                        }
#endif
                                }else{
                                        if( ec != boost::asio::error::try_again ){
                                                //送信失敗 -> 終了のお知らせ
#ifdef _DEBUG_MODE_
                                                std::cout << "tcp_session_stream_model::Run_dw cl_socket.write_some error[";
                                                std::cout << ec.message();
                                                std::cout << "]\n";
#endif
                                                rw_scoped_lock scope_lock(exit_flag_update_mutex);
                                                exit_flag = true;
                                                break;
                                        }
                                }
                                boost::this_thread::yield();
                        }
                }

                // クローズ
                {
                        rw_scoped_lock scope_lock(rs_close_mutex);
                        rs_socket.close(ec);
                }

#ifdef _DEBUG_MODE_
                std::cout << "tcp_session_stream_model::Run_dw rs_socket.close\n";
#endif

                {
                        rw_scoped_lock scope_lock(threadB_state_mutex);
                        threadB_state = 0;
                }

#ifdef _DEBUG_MODE_
                std::cout << "tcp_session_stream_model::Run_dw end\n";
#endif
        }
}
