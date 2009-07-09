#include <boost/thread/thread.hpp>

#include "tcp_session_stream_model.h"
 
namespace l7vs{

	//! construcor
	tcp_session_stream_model::tcp_session_stream_model(boost::asio::io_service& io,boost::asio::ip::tcp::endpoint rs_endpoint):
		cl_socket(io),
                rs_socket(io),
                threadA_state(0),
                threadB_state(0),
                realserver_endpoint(rs_endpoint),
                exit_flag(false){
	}

	//! destructor
	tcp_session_stream_model::~tcp_session_stream_model(){
	}

        boost::asio::ip::tcp::socket& tcp_session_stream_model::get_cl_socket(){
                return cl_socket;
        }

        void tcp_session_stream_model::Run_up(){
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
                        rw_scoped_lock scope_lock(exit_flag_update_mutex);
                        exit_flag = true;
                }

                while(true){// UPスレッドメインループ
                        {
				rd_scoped_lock scope_lock(exit_flag_update_mutex);
                                if(exit_flag){
                                        //終了のお知らせ
                                        break;
                                }
                        }

                        boost::this_thread::yield();
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
                                        rw_scoped_lock scope_lock(exit_flag_update_mutex);
                                        exit_flag = true;
                                        break;
                                }
                        }

                        // RS接続チェック
                        if(!rs_socket.is_open()){
                                //RS接続
                                {
                                        rw_scoped_lock scope_lock(rs_close_mutex);
                                        if(!rs_socket.connect(realserver_endpoint,ec)){
                                                //接続失敗 -> 終了のお知らせ
                                                rw_scoped_lock scope_lock(exit_flag_update_mutex);
                                                exit_flag = true;
                                                break;
                                        }
                                }
                                // ノンブロッキングに設定
                                boost::asio::socket_base::non_blocking_io cmd(true);
                                rs_socket.io_control(cmd,ec);
                                if(ec){
                                        //ノンブロッキング設定失敗 -> 終了のお知らせ
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
                                }else{
                                        if( ec != boost::asio::error::try_again ){
                                                //送信失敗 -> 終了のお知らせ
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
                        rw_scoped_lock scope_lock(cl_close_mutex);
                        cl_socket.close(ec);
                }

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
        }

        void tcp_session_stream_model::Run_dw(){
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
                        {
				rd_scoped_lock scope_lock(exit_flag_update_mutex);
                                if(exit_flag){
                                        //終了のお知らせ
                                        break;
                                }
                        }
                        boost::this_thread::yield();

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
                                        rw_scoped_lock scope_lock(exit_flag_update_mutex);
                                        exit_flag = true;
                                        break;
                                }
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
                                }else{
                                        if( ec != boost::asio::error::try_again ){
                                                //送信失敗 -> 終了のお知らせ
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
                {
                        rw_scoped_lock scope_lock(threadB_state_mutex);
                        threadB_state = 0;
                }

        }
}
