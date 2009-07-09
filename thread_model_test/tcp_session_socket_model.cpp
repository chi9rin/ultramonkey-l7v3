#include <boost/thread/thread.hpp>

#include "tcp_session_socket_model.h"
 
namespace l7vs{

	//! construcor
	tcp_session_socket_model::tcp_session_socket_model(boost::asio::io_service& io,boost::asio::ip::tcp::endpoint rs_endpoint):
		cl_socket(io),
                rs_socket(io),
                threadA_state(0),
                threadB_state(0),
                realserver_endpoint(rs_endpoint){
	}

	//! destructor
	tcp_session_socket_model::~tcp_session_socket_model(){
	}

        boost::asio::ip::tcp::socket& tcp_session_socket_model::get_cl_socket(){
                return cl_socket;
        }

        void tcp_session_socket_model::Run_cl(){
                socket_data* pdata = NULL;
                socket_data* pmsg = NULL;
                boost::system::error_code ec;

                {
                        rw_scoped_lock scope_lock(threadA_state_mutex);
                        threadA_state = 1;
                }

                //RSスレッド待機
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
                        if(pdata == NULL){
                                pdata = new socket_data();
                        }
                        pdata->data_size = 0;
                        pdata->msg_type = 0;
                        while(!cl2rs_msg.push(pdata)){}
                        pdata = NULL;
                }else{
                        while(true){// CLスレッドメインループ
                                boost::this_thread::yield();

                                // 受信
                                if(pdata == NULL){
                                        pdata = new socket_data();
                                }
                                size_t recv_size = cl_socket.read_some(boost::asio::buffer(pdata->buff.data(),65535), ec);
                                if(!ec){
                                         if(recv_size > 0){
                                                pdata->data_size = recv_size;
                                                pdata->msg_type = 1;
                                                while(!cl2rs_msg.push(pdata)){}
                                                pdata = NULL;
                                        }
                                }else{
                                        if(ec != boost::asio::error::try_again){
                                                //受信失敗 -> 終了のお知らせ
                                                pdata->data_size = 0;
                                                pdata->msg_type = 0;
                                                while(!cl2rs_msg.push(pdata)){}
                                                pdata = NULL;
                                                break;
                                        }else{
                                                ec.clear();
                                        }
                                }
                                //　CL要求チェック
                                while(true){
                                        boost::this_thread::yield();
                                        pmsg = rs2cl_msg.pop();
                                        if(pmsg == NULL){
                                                break;
                                        }
                                        if(pmsg->msg_type == 1){
                                                // 送信
                                                size_t send_size = 0;
                                                while(send_size < pmsg->data_size){
                                                        size_t ret_size = cl_socket.write_some(boost::asio::buffer(pmsg->buff.data() + send_size,pmsg->data_size - send_size), ec);
                                                        if(!ec){
                                                        // 送信成功
                                                                send_size += ret_size;
                                                        }else{
                                                        if( ec != boost::asio::error::try_again ){
                                                                break;
                                                        }
                                                        ec.clear();
                                                        }
                                                        boost::this_thread::yield();
                                                }
                                                delete pmsg;
                                                pmsg = NULL;
                                                if(ec){
                                                        // 送信失敗 
                                                        break;
                                                }
                                        }else{
                                                // 終了要求
                                                break;
                                        }
                                }
                                if(ec){
                                //送信失敗 -> 終了のお知らせ
                                        if(pdata == NULL){
                                                pdata = new socket_data();
                                        }
                                        pdata->data_size = 0;
                                        pdata->msg_type = 0;
                                        while(!cl2rs_msg.push(pdata)){}
                                        pdata = NULL;
                                        break;
                                }
                                if(pmsg != NULL){
                                        //　終了要求
                                        delete pmsg;
                                        pmsg = NULL;
                                        break;
                                }
                        }
                }

                // クローズ
                cl_socket.close(ec);

                // 受信バッファのお掃除
                if(pdata != NULL){
                        delete pdata;
                        pdata = NULL;
                }

                //RSスレッド待機
               while(true){ 
                        {
                                rd_scoped_lock scope_lock(threadB_state_mutex);
                                if(threadB_state == 0){
                                        break;
                                }
                        }
                        boost::this_thread::yield();
                }

               //キューのお掃除
               while(true){
                        pmsg = cl2rs_msg.pop();
                        if(pmsg == NULL){
                                break;
                        }
                        delete pmsg;
                        pmsg = NULL;
               }
               while(true){
                        pmsg = rs2cl_msg.pop();
                        if(pmsg == NULL){
                                break;
                        }
                        delete pmsg;
                        pmsg = NULL;
               }
        }

        void tcp_session_socket_model::Run_rs(){
                socket_data* pdata = NULL;
                socket_data* pmsg = NULL;
                boost::system::error_code ec;

                {
                        rw_scoped_lock scope_lock(threadB_state_mutex);
                        threadB_state = 1;
                }
                //CLスレッド待機
                while(true){ 
                        {
                                rd_scoped_lock scope_lock(threadA_state_mutex);
                                if(threadA_state == 1){
                                        break;
                                }
                        }
                        boost::this_thread::yield();
                }

                // 接続
                rs_socket.connect(realserver_endpoint,ec);
                if(ec){
                       //接続失敗 -> 終了のお知らせ
                        if(pdata == NULL){
                                pdata = new socket_data();
                        }
                        pdata->data_size = 0;
                        pdata->msg_type = 0;
                        while(!rs2cl_msg.push(pdata)){}
                        pdata = NULL;
                }else{
                        // ノンブロッキングに設定
                        boost::asio::socket_base::non_blocking_io cmd(true);
                        rs_socket.io_control(cmd,ec);
                        if(ec){
                                //ノンブロッキング設定失敗 -> 終了のお知らせ
                                if(pdata == NULL){
                                        pdata = new socket_data();
                                }
                                pdata->data_size = 0;
                                pdata->msg_type = 0;
                                while(!rs2cl_msg.push(pdata)){}
                                pdata = NULL;
                        }else{
                                while(true){// RSスレッドメインループ
                                        if(pdata == NULL){
                                                pdata = new socket_data();
                                        }
                                        boost::this_thread::yield();
                                        // 受信
                                        size_t recv_size = rs_socket.read_some(boost::asio::buffer(pdata->buff.data(),65535), ec);
                                        if(!ec){
                                                if(recv_size > 0){
                                                        pdata->data_size = recv_size;
                                                        pdata->msg_type = 1;
                                                        while(!rs2cl_msg.push(pdata)){}
                                                        pdata = NULL;
                                                }
                                        }else{
                                                if(ec != boost::asio::error::try_again){
                                                        //受信失敗 -> 終了のお知らせ
                                                        pdata->data_size = 0;
                                                        pdata->msg_type = 0;
                                                        while(!rs2cl_msg.push(pdata)){}
                                                        pdata = NULL;
                                                        break;
                                                }else{
                                                ec.clear(); 
                                                }
                                        }
                                        //　RS要求チェック
                                        while(true){
                                                boost::this_thread::yield();
                                                pmsg = cl2rs_msg.pop();
                                                if(pmsg == NULL){
                                                        break;
                                                }
                                                if(pmsg->msg_type == 1){
                                                        // 送信
                                                        size_t send_size = 0;
                                                        while(send_size < pmsg->data_size){
                                                                size_t ret_size = rs_socket.write_some(boost::asio::buffer(pmsg->buff.data() + send_size,pmsg->data_size - send_size), ec);
                                                                if(!ec){
                                                                // 送信成功
                                                                        send_size += ret_size;
                                                                }else{
                                                                        if( ec != boost::asio::error::try_again ){
                                                                        //送信失敗
                                                                                break;
                                                                        }
                                                                        ec.clear();
                                                                }
                                                                boost::this_thread::yield();
                                                        }
                                                        delete pmsg;
                                                        pmsg = NULL;
                                                        if(ec){
                                                                // 送信失敗 
                                                                break;
                                                        }
                                                }else{
                                                        // 終了要求
                                                        break;
                                                }
                                        }
                                        if(ec){
                                        //送信失敗 -> 終了のお知らせ
                                                if(pdata == NULL){
                                                        pdata = new socket_data();
                                                }
                                                pdata->data_size = 0;
                                                pdata->msg_type = 0;
                                                while(!rs2cl_msg.push(pdata)){}
                                                pdata = NULL;
                                                break;
                                        }
                                        if(pmsg != NULL){
                                                //　終了要求
                                                delete pmsg;
                                                pmsg = NULL;
                                                break;
                                        }
                                }
                        }

                        // クローズ
                        rs_socket.close(ec);

                        // 受信バッファのお掃除
                        if(pdata != NULL){
                                delete pdata;
                                pdata = NULL;
                        }
                }

                {
                        rw_scoped_lock scope_lock(threadB_state_mutex);
                        threadB_state = 0;
                }

        }
}
