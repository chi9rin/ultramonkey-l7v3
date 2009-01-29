#include <boost/test/included/unit_test.hpp>
#include "../../module/protocol/protocol_module_sessionless.cpp"

using namespace std;
using namespace boost;
using namespace boost::unit_test;
using namespace l7vs;

typedef l7vs::protocol_module_sessionless::check_message_result
		check_message_result;

//protocol_module_sessionless �g�Ə�
void protocol_module_sessionless_test(){

	//unit_test[1] ���W���[�����i"sessionless"�j
    protocol_module_sessionless obj1;
    BOOST_CHECK_EQUAL("sessionless",obj1.name);

    //unit_test[2] new
    protocol_module_sessionless *p1=new protocol_module_sessionless();
    BOOST_CHECK_EQUAL("sessionless", p1->name);
    delete p1;
}

//~protocol_module_sessionless �g�Ə�

//is_tcp �g�Ə�
void is_tcp_test(){

	//unit_test[4] TRUE��ԋp����
    protocol_module_sessionless obj;
    BOOST_CHECK(obj.is_tcp());
}

//is_udp �g�Ə�
void is_udp_test(){

	//unit_test[5] FALSE��ԋp����
    protocol_module_sessionless obj;
    BOOST_CHECK(!obj.is_udp());
}

//get_name �g�Ə�
void get_name_test(){

	//unit_test[5] ���W���[�����i"sessionless"�j��ԋp����
    protocol_module_sessionless obj;
    BOOST_CHECK_EQUAL("sesslonless", obj.get_name());
}

//initialize �����
typedef protocol_module_sessionless::realserverlist_type real_server_list;
typedef	protocol_module_sessionless::rs_list_itr_func_type rs_list_itr_func_type;
real_server_list real_list;
real_server_list::iterator real_begin() {return real_list.begin();}
real_server_list::iterator real_end() {return real_list.end();}
real_server_list::iterator real_next() {return ++real_list.begin();}
void list_lock_for_test() {}
void list_unlock_for_test() {}
void initialize_test() {

	//unit_test[1] RealServer���X�g�̊e����֐����\�b�h�̃e�X�g
	//unit_test[1] test data:RealServer���X�g�擪�擾�֐�,RealServer���X�g���[�擾�֐�,RealServer���X�g���v�f�擾�֐�
	//unit_test[1] test data:RealServer���X�g���b�N�֐�,RealServer���X�g�A�����b�N�֐�
	rs_list_itr_func_type inrslist_begin = real_begin;
	rs_list_itr_func_type inrslist_end = real_end;
	rs_list_itr_func_type inrslist_next = real_next;
	function<void(void)> inlist_lock = list_lock_for_test;
	function<void(void)> inlist_unlock = list_unlock_for_test;

	protocol_module_sessionless sessionless;
	sessionless.initialize(inrslist_begin,inrslist_end,inrslist_next,inlist_lock,inlist_unlock);

	BOOST_CHECK_EQUAL(sessionless.rs_list_begin, real_begin);
	BOOST_CHECK_EQUAL(sessionless.rs_list_end, real_end);
	BOOST_CHECK_EQUAL(sessionless.rs_list_next, real_next);
	BOOST_CHECK_EQUAL(sessionless.rs_list_lock, list_lock_for_test);
	BOOST_CHECK_EQUAL(sessionless.rs_list_unlock, list_unlock_for_test);

};

//finalize �����
void finalize_test() {

	//unit_test[2] �e����֐�������������
	//unit_test[2] test data
	protocol_module_sessionless sessionless;
	sessionless.finalize();

	BOOST_CHECK(!sessionless.getloglevel);
	BOOST_CHECK(!sessionless.putLogFatal);
	BOOST_CHECK(!sessionless.putLogError);
	BOOST_CHECK(!sessionless.putLogWarn);
	BOOST_CHECK(!sessionless.putLogInfo);
	BOOST_CHECK(!sessionless.putLogDebug);

	BOOST_CHECK(!sessionless.rs_list_begin);
	BOOST_CHECK(!sessionless.rs_list_end);
	BOOST_CHECK(!sessionless.rs_list_next);
	BOOST_CHECK(!sessionless.rs_list_lock);
	BOOST_CHECK(!sessionless.rs_list_unlock);

	BOOST_CHECK(!sessionless.replication_pay_memory);
	BOOST_CHECK(!sessionless.replication_area_lock);
	BOOST_CHECK(!sessionless.replication_area_unlock);

	BOOST_CHECK(!sessionless.schedule_tcp);
	BOOST_CHECK(!sessionless.schedule_udp);

	BOOST_CHECK_EQUAL(sessionless.forwarded_for, 0);
    BOOST_CHECK_EQUAL( '\0', sessionless.sorry_uri[0] );

}

//is_use_sorry �n����
void is_use_sorry_test() {
	protocol_module_sessionless sessionless;

	// unit_test[1]  is_use_sorry���\�b�h�̃e�X�g
	bool is_return = sessionless.is_use_sorry();
	BOOST_CHECK_EQUAL(is_return,true);
}

//check_parameter �n����
void check_parameter_test() {
	protocol_module_sessionless sessionless;
	check_message_result result;
	vector<string> args(5);

	// unit_test[2] �`�F�b�N���ʃt���O��TRUE��ݒ肷��
	// unit_test[2] test data:�I�v�V����������Ƀf�[�^�Ȃ�
	result = sessionless.check_parameter(args);
	BOOST_CHECK_EQUAL(result.flag,true);

	// unit_test[3] ���M���ݒ�w���ݒ�t���O��ON,�`�F�b�N���ʃt���O��TRUE��ݒ肷��
	// unit_test[3] test data:�I�v�V���������� = "-F"
	args[0] = "l7vsadmin";
	args[1] = "-F";
	result = sessionless.check_parameter(args);
	BOOST_CHECK_EQUAL(result.flag,true);

	// unit_test[4] ���M���ݒ�w���ݒ�t���O��ON, �`�F�b�N���ʃt���O��TRUE��ݒ肷��
	// unit_test[4] test data:�I�v�V���������� = "-F"
	args[0] = "l7vsadmin";
	args[1] = "--forwarded-for";
	sessionless.check_parameter(args);
	result = sessionless.check_parameter(args);
	BOOST_CHECK_EQUAL(result.flag,true);

	// unit_test[5] �`�F�b�N���ʃt���O��FALSE��ݒ肷��,�`�F�b�N���ʃ��b�Z�[�W��"'-S/--sorryURI' option value '%s' is too long."�i%s�͎��v�f�j��ݒ肷��
	// unit_test[5] test data:�I�v�V���������� = "-S" sorryURI�ݒ�t���O = OFF ���v�f�̕����� > 127
	args[0] = "l7vsadmin";
	args[1] = "-S";
	args[2] = "http://abcde.com/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/";
	result = sessionless.check_parameter(args);
	BOOST_CHECK_EQUAL(result.flag,false);
	BOOST_CHECK_EQUAL(result.message,"'-S/--sorryURI' option value 'http://abcde.com/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/' is too long.");

	// unit_test[6] sorryURI�ݒ�t���O��ON �`�F�b�N���ʃ��b�Z�[�W��"Cannot set multiple option ''-S/--sorryURI'."��ݒ肷��
	// unit_test[6] test data:�I�v�V���������� = "-S" sorryURI�ݒ�t���O = OFF ���v�f�̕����� = 127 �`�F�b�NOK�̏ꍇ
	args[0] = "l7vsadmin";
	args[1] = "-S";
	args[2] = "http://abcde.com/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/12345678/";
	args[3] = "----sorryURI";
	args[4] = "http://abcde.com/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/12345678/";
	result = sessionless.check_parameter(args);
	BOOST_CHECK_EQUAL(result.message,"Cannot set multiple option ''-S/--sorryURI'.");
	BOOST_CHECK_EQUAL(result.flag,false);

	// unit_test[7] �`�F�b�N���ʃt���O��FALSE��ݒ肷��,�`�F�b�N���ʃ��b�Z�[�W��"'-S/--sorryURI' option value '%s' is not a valid URI."�i%s�͎��v�f�j��ݒ肷��
	// unit_test[7] test data:�I�v�V���������� = "-S" sorryURI�ݒ�t���O = OFF ���v�f�̕����� = 127 �`�F�b�NNG�̏ꍇ
	args[0] = "l7vsadmin";
	args[1] = "-S";
	args[2] = "http://abcde.com/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/1234567+/";
	result = sessionless.check_parameter(args);
	BOOST_CHECK_EQUAL(result.flag,false);
	BOOST_CHECK_EQUAL(result.message,"'-S/--sorryURI' option value 'http://abcde.com/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/1234567+/' is not a valid URI.");

	// unit_test[8] sorryURI�ݒ�t���O��ON �`�F�b�N���ʃ��b�Z�[�W��"Cannot set multiple option ''-S/--sorryURI'."��ݒ肷��
	// unit_test[8] test data:�I�v�V���������� = "-S" sorryURI�ݒ�t���O = OFF ���v�f�̕����� < 127 �`�F�b�NOK�̏ꍇ
	args[0] = "l7vsadmin";
	args[1] = "-S";
	args[2] = "http://abcd.com/";
	args[3] = "----sorryURI";
	args[4] = "http://abcd.com/";
	result = sessionless.check_parameter(args);
	BOOST_CHECK_EQUAL(result.message,"Cannot set multiple option ''-S/--sorryURI'.");
	BOOST_CHECK_EQUAL(result.flag,false);

	// unit_test[9] �`�F�b�N���ʃt���O��FALSE��ݒ肷��,�`�F�b�N���ʃ��b�Z�[�W��"'-S/--sorryURI' option value '%s' is not a valid URI."�i%s�͎��v�f�j��ݒ肷��
	// unit_test[9] test data:�I�v�V���������� = "-S" sorryURI�ݒ�t���O = OFF ���v�f�̕����� < 127 �`�F�b�NNG�̏ꍇ
	args[0] = "l7vsadmin";
	args[1] = "-S";
	args[2] = "http://abcd.com/aa+";
	result = sessionless.check_parameter(args);
	BOOST_CHECK_EQUAL(result.flag,false);
	BOOST_CHECK_EQUAL(result.message,"'-S/--sorryURI' option value 'http://abcd.com/aa+' is not a valid URI.");

	// unit_test[10] �`�F�b�N���ʃt���O��FALSE��ݒ肷��,�`�F�b�N���ʃ��b�Z�[�W��"You have to set  option value ''-S/--sorryURI'."��ݒ肷��
	// unit_test[10] test data:�I�v�V���������� = "-S" sorryURI�ݒ�t���O = OFF ���v�f�����݂��Ȃ��ꍇ
	args[0] = "l7vsadmin";
	args[1] = "-S";
	result = sessionless.check_parameter(args);
	BOOST_CHECK_EQUAL(result.flag,false);
	BOOST_CHECK_EQUAL(result.message,"You have to set  option value ''-S/--sorryURI'.");

	// unit_test[11] �`�F�b�N���ʃt���O��FALSE��ݒ肷��,�`�F�b�N���ʃ��b�Z�[�W��"Cannot set multiple option ''-S/--sorryURI'."��ݒ肷��
	// unit_test[11] test data:�I�v�V���������� = "-S" sorryURI�ݒ�t���O = ON
	args[0] = "l7vsadmin";
	args[1] = "-S";
	args[2] = "http://abcd.com/";
	args[3] = "-S";
	args[4] = "http://www.xyz.com/";
	result = sessionless.check_parameter(args);
	BOOST_CHECK_EQUAL(result.flag,false);
	BOOST_CHECK_EQUAL(result.message,"Cannot set multiple option ''-S/--sorryURI");

	// unit_test[12] �`�F�b�N���ʃt���O��FALSE��ݒ肷��,�`�F�b�N���ʃ��b�Z�[�W��"Option error."��ݒ肷��
	// unit_test[12] test data:�I�v�V����������"-F","-S" �ȊO�̏ꍇ
	args[0] = "l7vsadmin";
	args[1] = "-D";
	result = sessionless.check_parameter(args);
	BOOST_CHECK_EQUAL(result.flag,false);
	BOOST_CHECK_EQUAL(result.message,"Option error.");

}

//set_parameter �n����
void set_parameter_test() {
	protocol_module_sessionless sessionless;
	check_message_result result;
	vector<string> args(5);

	// unit_test[13] �`�F�b�N���ʃt���O��TRUE��ݒ肷��
	// unit_test[13] test data:�I�v�V����������Ƀf�[�^�Ȃ�
	result = sessionless.check_parameter(args);
	BOOST_CHECK_EQUAL(result.flag,true);

	// unit_test[14] ���M���ݒ�w����1��ݒ肷��
	args[0] = "l7vsadmin";
	sessionless.check_parameter(args);
	BOOST_CHECK_EQUAL(sessionless.forwarded_for,1);

	// unit_test[15] ���M���ݒ�w���ݒ�t���O��ON,�`�F�b�N���ʃt���O��TRUE��ݒ肷��
	// unit_test[15] test data:�I�v�V���������� = "-F"
	args[0] = "l7vsadmin";
	args[1] = "-F";
	result = sessionless.check_parameter(args);
	BOOST_CHECK_EQUAL(sessionless.forwarded_for,1);
	BOOST_CHECK_EQUAL(result.flag,true);

	// unit_test[16] ���M���ݒ�w���ݒ�t���O��ON,�`�F�b�N���ʃt���O��TRUE��ݒ肷��
	// unit_test[16] test data:�I�v�V���������� = "-F"
	args[0] = "l7vsadmin";
	args[1] = "--forwarded-for";
	result = sessionless.check_parameter(args);
	BOOST_CHECK_EQUAL(sessionless.forwarded_for,1);
	BOOST_CHECK_EQUAL(result.flag,true);

	// unit_test[17] �`�F�b�N���ʃt���O��FALSE��ݒ肷��,�`�F�b�N���ʃ��b�Z�[�W��"'-S/--sorryURI' option value '%s' is too long."�i%s�͎��v�f�j��ݒ肷��
	// unit_test[17] test data:�I�v�V���������� = "-S" sorryURI�ݒ�t���O = OFF ���v�f�̕����� > 127
	args[0] = "l7vsadmin";
	args[1] = "-S";
	args[2] = "http://abcde.com/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/";
	result = sessionless.check_parameter(args);
	BOOST_CHECK_EQUAL(result.flag,false);
	BOOST_CHECK_EQUAL(result.message,"'-S/--sorryURI' option value 'http://abcde.com/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/' is too long.");

	// unit_test[18] sorryURI�ݒ�t���O��ON �`�F�b�N���ʃ��b�Z�[�W��"Cannot set multiple option ''-S/--sorryURI'."��ݒ肷��
	// unit_test[18] test data:�I�v�V���������� = "-S" sorryURI�ݒ�t���O = OFF ���v�f�̕����� = 127 �`�F�b�NOK�̏ꍇ
	args[0] = "l7vsadmin";
	args[1] = "-S";
	args[2] = "http://abcde.com/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/12345678/";
	args[3] = "--sorryURI";
	args[4] = "http://abcde.com/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/12345678/";
	result = sessionless.check_parameter(args);
	BOOST_CHECK_EQUAL(result.message,"Cannot set multiple option ''-S/--sorryURI'.");
	BOOST_CHECK_EQUAL(result.flag,false);

	// unit_test[19] �`�F�b�N���ʃt���O��FALSE��ݒ肷��,�`�F�b�N���ʃ��b�Z�[�W��"'-S/--sorryURI' option value '%s' is not a valid URI."�i%s�͎��v�f�j��ݒ肷��
	// unit_test[19] test data:�I�v�V���������� = "-S" sorryURI�ݒ�t���O = OFF ���v�f�̕����� = 127 �`�F�b�NNG�̏ꍇ
	args[0] = "l7vsadmin";
	args[1] = "-S";
	args[2] = "http://abcde.com/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/1234567+/";
	result = sessionless.check_parameter(args);
	BOOST_CHECK_EQUAL(result.flag,false);
	BOOST_CHECK_EQUAL(result.message,"'-S/--sorryURI' option value 'http://abcde.com/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/1234567+/' is not a valid URI.");

	// unit_test[20] sorryURI�ݒ�t���O��ON �`�F�b�N���ʃ��b�Z�[�W��"Cannot set multiple option ''-S/--sorryURI'."��ݒ肷��
	// unit_test[20] test data:�I�v�V���������� = "-S" sorryURI�ݒ�t���O = OFF ���v�f�̕����� < 127 �`�F�b�NOK�̏ꍇ
	args[0] = "l7vsadmin";
	args[1] = "-S";
	args[2] = "http://abcd.com/";
	args[3] = "--sorryURI";
	args[4] = "http://abcd.com/";
	result = sessionless.check_parameter(args);
	BOOST_CHECK_EQUAL(result.message,"Cannot set multiple option ''-S/--sorryURI'.");
	BOOST_CHECK_EQUAL(result.flag,false);

	// unit_test[21] �`�F�b�N���ʃt���O��FALSE��ݒ肷��,�`�F�b�N���ʃ��b�Z�[�W��"'-S/--sorryURI' option value '%s' is not a valid URI."�i%s�͎��v�f�j��ݒ肷��
	// unit_test[21] test data:���v�f�̕����� < 127 �`�F�b�NNG�̏ꍇ
	args[0] = "l7vsadmin";
	args[1] = "-S";
	args[2] = "http://abcd.com/aa+";
	result = sessionless.check_parameter(args);
	BOOST_CHECK_EQUAL(result.flag,false);
	BOOST_CHECK_EQUAL(result.message,"'-S/--sorryURI' option value 'http://abcd.com/aa+' is not a valid URI.");

	// unit_test[22] �`�F�b�N���ʃt���O��FALSE��ݒ肷��,�`�F�b�N���ʃ��b�Z�[�W��"You have to set  option value ''-S/--sorryURI'."��ݒ肷��
	// unit_test[22] test data:�I�v�V���������� = "-S" sorryURI�ݒ�t���O = OFF ���v�f�����݂��Ȃ��ꍇ
	args[0] = "l7vsadmin";
	args[1] = "-S";
	result = sessionless.check_parameter(args);
	BOOST_CHECK_EQUAL(result.flag,false);
	BOOST_CHECK_EQUAL(result.message,"You have to set  option value ''-S/--sorryURI'.");

	// unit_test[23] �`�F�b�N���ʃt���O��FALSE��ݒ肷��,�`�F�b�N���ʃ��b�Z�[�W��"Cannot set multiple option ''-S/--sorryURI'."��ݒ肷��
	// unit_test[23] test data:�I�v�V���������� = "-S" sorryURI�ݒ�t���O = ON
	args[0] = "l7vsadmin";
	args[1] = "-S";
	args[2] = "http://abcd.com/";
	args[3] = "-S";
	args[4] = "http://www.xyz.com/";
	result = sessionless.check_parameter(args);
	BOOST_CHECK_EQUAL(result.flag,false);
	BOOST_CHECK_EQUAL(result.message,"Cannot set multiple option ''-S/--sorryURI");

	// unit_test[24] �`�F�b�N���ʃt���O��FALSE��ݒ肷��,�`�F�b�N���ʃ��b�Z�[�W��"Option error."��ݒ肷��
	// unit_test[24] test data:�I�v�V����������"-F","-S" �ȊO�̏ꍇ
	args[0] = "l7vsadmin";
	args[1] = "-D";
	result = sessionless.check_parameter(args);
	BOOST_CHECK_EQUAL(result.flag,false);
	BOOST_CHECK_EQUAL(result.message,"Option error.");

	// unit_test[25] ���M���ݒ�w����0��ݒ肷��
	// unit_test[25] test data:�`�F�b�N���ʃt���O = TRUE�̏ꍇ�A���M���ݒ�w���ݒ�t���O = OFF
	args[0] = "l7vsadmin";
	args[1] = "-S";
	args[2] = "http://abcd.com/";
	sessionless.check_parameter(args);
	BOOST_CHECK_EQUAL(sessionless.forwarded_for,0);

}

//add_parameter �n����
void add_parameter_test() {
	protocol_module_sessionless sessionless;
	check_message_result result;
	vector<string> args(2);

	// unit_test[58] �`�F�b�N���ʃt���O��TRUE��ݒ肷��
	// unit_test[58] test data:�I�v�V���������񂪑��݂��Ȃ��ꍇ
	result = sessionless.add_parameter(args);
	BOOST_CHECK_EQUAL(result.flag,true);

	// unit_test[59] �`�F�b�N���ʃt���O��FALSE��ݒ肷��, �`�F�b�N���ʃ��b�Z�[�W��"Cannot add option."��ݒ肷��
	// unit_test[59] test data:�I�v�V���������񂪑��݂���ꍇ
	args[0] = "-F";
	result = sessionless.add_parameter(args);
	BOOST_CHECK_EQUAL(result.flag,false);
	BOOST_CHECK_EQUAL(result.message,"Cannot add option.");
}

//handle_rslist_update �n����

//register_schedule(tcp) �����
void schedule_tcp( const boost::thread::id id,
		           rs_list_itr_func_type func_type1,
		           rs_list_itr_func_type func_type2,
		           rs_list_itr_func_type func_type3,
		           boost::asio::ip::tcp::endpoint& ) {};
void register_schedule_tcp_test() {

	// unit_test[3]  schedule_tcp���\�b�h�̃e�X�g
	// unit_test[3] test data:Client�U�蕪�������֐�
	protocol_module_sessionless::tcp_schedule_func_type  func = schedule_tcp;

	protocol_module_sessionless sessionless;
	sessionless.register_schedule(func);

	BOOST_CHECK_EQUAL(sessionless.schedule_tcp,schedule_tcp);
}

//register_schedule(udp) �����


//handle_session_initialize �����
typedef protocol_module_sessionless::session_thread_data_sessionless session_thread_data;
typedef std::map< thread::id, session_thread_data*> thread_map;
void up_thread_func(){}
void down_thread_func(){}
void handle_session_initialize_test() {

	// unit_test[5] �J�ڐ�X�e�[�^�X��ݒ肷�� status = ACCEPT
	// unit_test[5] test data:�X���b�h
	boost::thread up_thread(up_thread_func);
	boost::thread down_thread(down_thread_func);

	protocol_module_sessionless sessionless;
	protocol_module_base::EVENT_TAG ret =
		sessionless.handle_session_initialize(up_thread.get_id(),down_thread.get_id());

    BOOST_CHECK_EQUAL( protocol_module_base::ACCEPT, ret);

    thread_map::iterator thread_map_iterator;
    thread_map::iterator thread_map_iterator_end;

    //unit_test[6]  ���X���b�h�}�v��̃e�X�g
    //unit_test[6] test data:�}�v
    thread_map_iterator = sessionless.session_thread_data_map.find(up_thread.get_id());
    thread_map_iterator_end = sessionless.session_thread_data_map.end();
    BOOST_CHECK(thread_map_iterator != thread_map_iterator_end);

    //unit_test[7]  ���X���b�h�}�v�̃e�X�g
    thread_map_iterator = sessionless.session_thread_data_map.find(up_thread.get_id());
    session_thread_data* data = thread_map_iterator->second;

    BOOST_CHECK(!data);
    BOOST_CHECK_EQUAL(up_thread.get_id(), data->thread_id);
    BOOST_CHECK_EQUAL(0, data->thread_division);
    BOOST_CHECK_EQUAL(down_thread.get_id(), data->pair_thread_id);
    BOOST_CHECK(!(data->recive_data_map.empty()));

    BOOST_CHECK_EQUAL(data->end_flag, 0);
    BOOST_CHECK_EQUAL(data->accept_end_flag, 0);
    BOOST_CHECK_EQUAL(data->sorry_flag, 0);
    BOOST_CHECK_EQUAL(data->sorryserver_switch_flag, 0);
    BOOST_CHECK_EQUAL(data->realserver_switch_flag, 0);

    //unit_test[8]  ����X���b�h�}�v��̃e�X�g
    thread_map_iterator = sessionless.session_thread_data_map.find(down_thread.get_id());
    BOOST_CHECK(thread_map_iterator != thread_map_iterator_end);

	//unit_test[9]  ����X���b�h�}�v�̃e�X�g
    thread_map_iterator = sessionless.session_thread_data_map.find(down_thread.get_id());
    data = thread_map_iterator->second;

    BOOST_CHECK(!data);
    BOOST_CHECK_EQUAL(down_thread.get_id(), data->thread_id);
    BOOST_CHECK_EQUAL(data->thread_division, 1);
    BOOST_CHECK_EQUAL(up_thread.get_id(), data->pair_thread_id);
    BOOST_CHECK(!(data->recive_data_map.empty()));

    BOOST_CHECK_EQUAL(data->end_flag, 0);
    BOOST_CHECK_EQUAL(data->accept_end_flag, 0);
    BOOST_CHECK_EQUAL(data->sorry_flag, 0);
    BOOST_CHECK_EQUAL(data->sorryserver_switch_flag, 0);
    BOOST_CHECK_EQUAL(data->realserver_switch_flag, 0);
}

//handle_session_finalize �����
void handle_session_finalize_test() {

	//unit_test[1] �Z�b�V�����X���b�h�ɑΉ�����I������
	//unit_test[1] test data:�Z�b�V�����X���b�h������
	boost::thread up_thread(up_thread_func);
	boost::thread down_thread(down_thread_func);
	protocol_module_sessionless sessionless;
	sessionless.handle_session_initialize(up_thread.get_id(),down_thread.get_id());
	protocol_module_base::EVENT_TAG ret =
		sessionless.handle_session_finalize(up_thread.get_id(), down_thread.get_id());

    thread_map::iterator thread_map_iterator;
    thread_map::iterator thread_map_iterator_end;
    thread_map_iterator = sessionless.session_thread_data_map.find(up_thread.get_id());

    thread_map_iterator_end = sessionless.session_thread_data_map.end();
    BOOST_CHECK(thread_map_iterator == thread_map_iterator_end);

    thread_map_iterator = sessionless.session_thread_data_map.find(down_thread.get_id());
    BOOST_CHECK(thread_map_iterator == thread_map_iterator_end);

    BOOST_CHECK_EQUAL( protocol_module_base::STOP, ret);

}

//handle_accept �����
void thread_func_for_accept(){}
void handle_accept_test() {

	session_thread_data  data;
	boost::thread accept_thread(thread_func_for_accept);

	//unit_test[13] sorry��Ԃ̏ꍇ
	//unit_test[13] test data:accept_end_flag=0,sorry_flag=1
	data.accept_end_flag = 0;
	data.sorry_flag = 1;

	protocol_module_sessionless obj1;
	obj1.session_thread_data_map.insert(
			std::pair<boost::thread::id, session_thread_data*>(accept_thread.get_id(), &data));

	protocol_module_base::EVENT_TAG ret = obj1.handle_accept(accept_thread.get_id());

	BOOST_CHECK_EQUAL(data.accept_end_flag, 1);//accept�����t���O��ON
	BOOST_CHECK_EQUAL(ret, protocol_module_base::SORRYSERVER_SELECT);//�J�ڐ�X�e�[�^�X=SORRYSERVER_SELECT

	//unit_test[13] sorry��ԈȊO�̏ꍇ
	data.accept_end_flag = 0;
	data.sorry_flag = 0;

	protocol_module_sessionless obj2;
	obj2.session_thread_data_map.insert(
				std::pair<boost::thread::id, session_thread_data*>(accept_thread.get_id(), &data));

	ret = obj2.handle_accept(accept_thread.get_id());

	BOOST_CHECK_EQUAL(data.accept_end_flag, 1);//accept�����t���O��ON
	BOOST_CHECK_EQUAL(ret, protocol_module_base::REALSERVER_SELECT);//�J�ڐ�X�e�[�^�X=REALSERVER_SELECT
}

//handle_client_recv

//handle_realserver_select(tcp) �g�Ə�
void upthread_func_for_handle_realserver_select_test(){}
void downthread_func_for_handle_realserver_select_test(){}

void schedule_tcp_determinate(//endpoint = ����
		const boost::thread::id thread_id,
		protocol_module_base::rs_list_itr_func_type rs_list_begin,
		protocol_module_base::rs_list_itr_func_type rs_list_end,
		protocol_module_base::rs_list_itr_func_type rs_list_next,
		boost::asio::ip::tcp::endpoint &rs_endpoint){
	rs_endpoint.resize(1234);
}

void schedule_tcp_nodeterminate(//endpoint = ������
		const boost::thread::id thread_id,
		protocol_module_base::rs_list_itr_func_type rs_list_begin,
		protocol_module_base::rs_list_itr_func_type rs_list_end,
		protocol_module_base::rs_list_itr_func_type rs_list_next,
		boost::asio::ip::tcp::endpoint &rs_endpoint){
}
void handle_realserver_select_test(){

	//unit_test[6] endpoint = ����
	protocol_module_sessionless obj1;

	boost::thread upt1(upthread_func_for_handle_realserver_select_test);
	boost::thread downt1(downthread_func_for_handle_realserver_select_test);
	session_thread_data dataup;
	session_thread_data datadown;
	obj1.session_thread_data_map[upt1.get_id()] = &dataup;
	obj1.session_thread_data_map[downt1.get_id()] = &datadown;
	dataup.thread_division = 0;
	dataup.pair_thread_id = downt1.get_id();
	datadown.thread_division = 1;
	datadown.pair_thread_id = upt1.get_id();
	datadown.recive_data_map.clear();
	protocol_module_sessionless::tcp_schedule_func_type func1 = &schedule_tcp_determinate;

	obj1.register_schedule(func1);

	boost::asio::ip::tcp::endpoint ep1;
	protocol_module_base::EVENT_TAG ret=obj1.handle_realserver_select(upt1.get_id(), ep1);

	BOOST_CHECK_EQUAL(ep1.size(), static_cast<size_t>(1234));
	BOOST_CHECK(datadown.recive_data_map.find(ep1) != datadown.recive_data_map.end());
	BOOST_CHECK_EQUAL(ret, protocol_module_base::CLIENT_RECV);

	//unit_test[7] endpoint = ������
	protocol_module_sessionless obj2;

	boost::thread upt2(upthread_func_for_handle_realserver_select_test);
	session_thread_data dataup2;
	obj2.session_thread_data_map[upt2.get_id()] = &dataup2;
	dataup2.end_flag = 0;
	protocol_module_sessionless::tcp_schedule_func_type func2 = &schedule_tcp_nodeterminate;

	obj2.register_schedule(func2);

	boost::asio::ip::tcp::endpoint ep2;
	ret=obj2.handle_realserver_select(upt2.get_id(), ep2);

	BOOST_CHECK_EQUAL(dataup2.end_flag, 1);
	BOOST_CHECK_EQUAL(ret, protocol_module_base::CLIENT_DISCONNECT);
}

//handle_realserver_select(udp)

//handle_realserver_connect

//handle_realserver_connection_fail �g�Ə�
void handle_realserver_connection_fail_test(){
	//unit_test[8] �I���t���O,�J�ڐ�X�e�[�^�X
	protocol_module_sessionless obj;

	boost::thread t;
	boost::asio::ip::tcp::endpoint ep;
	protocol_module_sessionless::EVENT_TAG ret;

	protocol_module_sessionless::session_thread_data_sessionless data;
	data.thread_id = t.get_id();
	obj.session_thread_data_map[t.get_id()] = &data;

	ret = obj.handle_realserver_connection_fail(t.get_id(),ep);

	BOOST_CHECK_EQUAL(data.end_flag, 1);		//�I���t���O��ON
	BOOST_CHECK_EQUAL(protocol_module_base::CLIENT_DISCONNECT, ret);		//status = CLIENT_DISCONNECT
}

//handle_realserver_send �����

//handle_sorryserver_select �g�Ə�
void handle_sorryserver_select_test(){

	//unit_test[9] �Y���X���b�h�̑��M�\�f�[�^���擾����,����X���b�h��sorryserver��M�o�b�t�@���m�ۂ���,���M��endpoint,�J�ڐ�X�e�[�^�X
	protocol_module_sessionless obj;

	boost::thread upt,downt;
	boost::asio::ip::tcp::endpoint ep;
	protocol_module_sessionless::EVENT_TAG ret;

	//up,down thread map
	protocol_module_sessionless::session_thread_data_sessionless dataup,datadown;
	obj.session_thread_data_map[upt.get_id()] = &dataup;
	obj.session_thread_data_map[downt.get_id()] = &datadown;
	dataup.thread_id = upt.get_id();
	dataup.thread_division = 0;
	dataup.pair_thread_id = downt.get_id();
	datadown.thread_id = downt.get_id();
	datadown.thread_division = 1;
	datadown.pair_thread_id = upt.get_id();
	datadown.recive_data_map.clear();

	//send_status���M�\�f�[�^
	protocol_module_sessionless::send_status st;
	st.status = protocol_module_sessionless::SEND_OK;
	st.send_possible_size = 2;
	dataup.recive_data_map[ep].send_status_list.push_back(st);

	ret = obj.handle_sorryserver_select(upt.get_id(), ep);

	BOOST_CHECK(!datadown.recive_data_map.empty());		//����X���b�h��sorryserver��M�o�b�t�@���m�ۂ���
	//BOOST_CHECK_EQUAL(ep, dataup.sorry_endpoint);	// ���M��endpoint��endpoint��ݒ肷��
	BOOST_CHECK_EQUAL(protocol_module_base::SORRYSERVER_CONNECT, ret);	//status = SORRYSERVER_CONNECT
}

//handle_sorryserver_connect

//handle_sorryserver_connection_fail

//handle_sorryserver_send

//handle_realserver_recv(tcp)

//handle_realserver_recv(udp)

//handle_sorryserver_recv

//handle_response_send_inform

//handle_client_connection_check
void handle_client_connection_check_test(){
	//unit_test[10] ���M�o�b�t�@�T�C�Y�@>�@���M�\�f�[�^�T�C�Y
	protocol_module_sessionless obj1;

	boost::thread t1;
	boost::array<char,MAX_BUFFER_SIZE> sbf1;
	std::size_t d1;
	protocol_module_sessionless::EVENT_TAG ret;
	boost::asio::ip::tcp::endpoint ep1;

	//session_thread_data_sessionless
	protocol_module_sessionless::session_thread_data_sessionless data1;
	obj1.session_thread_data_map[t1.get_id()] = &data1;
	//send_status
	protocol_module_sessionless::send_status sd1;
	sd1.status = protocol_module_sessionless::SEND_OK;
	sd1.send_possible_size = 2;
	sd1.send_offset = 1;
	//recive_data
	protocol_module_sessionless::recive_data rd1;
	rd1.recive_buffer = "abcd";
	//send_status->recive_data
	rd1.send_status_list.push_back(sd1);
	//recive_data->session_thread_data_sessionless
	data1.recive_data_map[ep1] = rd1;

	ret = obj1.handle_client_connection_check(t1.get_id(),sbf1,d1);
	//���M�\�f�[�^��擪���瑗�M�\�f�[�^�T�C�Y���A���M�o�b�t�@�ɃR�s�[����
	std::string chk1 = "bc";
	BOOST_CHECK_EQUAL(chk1.c_str(),sbf1.data());
	//���M�σf�[�^�T�C�Y�ɑ��M�\�f�[�^�T�C�Y��ݒ肷��
	BOOST_CHECK_EQUAL(data1.recive_data_map[ep1].send_status_list.rbegin()->send_end_size, 2);
	//���M�\�f�[�^�T�C�Y��0��ݒ肷��
	BOOST_CHECK_EQUAL(data1.recive_data_map[ep1].send_status_list.rbegin()->send_possible_size, 0);
	//�J�ڐ�X�e�[�^�X��ݒ肷��
	BOOST_CHECK_EQUAL(protocol_module_base::CLIENT_SEND, ret);


	//unit_test[11] ���M�o�b�t�@�T�C�Y�@<�@���M�\�f�[�^�T�C�Y
	protocol_module_sessionless obj2;

	boost::thread t2;
	boost::array<char,MAX_BUFFER_SIZE> sbf2;
	std::size_t d2;
	boost::asio::ip::tcp::endpoint ep2;

	//session_thread_data_sessionless
	protocol_module_sessionless::session_thread_data_sessionless data2;
	obj2.session_thread_data_map[t2.get_id()] = &data2;
	//send_status
	protocol_module_sessionless::send_status sd2;
	sd2.status = protocol_module_sessionless::SEND_OK;
	sd2.send_possible_size = 5000;
	sd2.send_offset = 1;
	//recive_data
	protocol_module_sessionless::recive_data rd2;
	rd2.recive_buffer = "abcdefg";
	//send_status->recive_data
	rd2.send_status_list.push_back(sd2);
	//recive_data->session_thread_data_sessionless
	data2.recive_data_map[ep2] = rd2;

	ret = obj2.handle_client_connection_check(t2.get_id(),sbf2,d2);
	//���M�\�f�[�^��擪���瑗�M�o�b�t�@�T�C�Y���A���M�o�b�t�@�ɃR�s�[����
	std::string chk2 = "bcdef";
	BOOST_CHECK_EQUAL(chk2.c_str(),sbf2.data());
	//���M�σf�[�^�T�C�Y�ɑ��M�o�b�t�@�T�C�Y��ݒ肷��
	int sbfsize = sbf2.max_size();
	BOOST_CHECK_EQUAL(data2.recive_data_map[ep2].send_status_list.rbegin()->send_end_size, sbfsize);
	//���M�\�f�[�^�T�C�Y���瑗�M�o�b�t�@�T�C�Y�����Z����
	BOOST_CHECK_EQUAL(5000-sbfsize,data2.recive_data_map[ep2].send_status_list.rbegin()->send_possible_size);
	//�J�ڐ�X�e�[�^�X��ݒ肷��
	BOOST_CHECK_EQUAL(protocol_module_base::CLIENT_SEND,ret);

	//unit_test[10] ���M�o�b�t�@�T�C�Y�@=�@���M�\�f�[�^�T�C�Y
	protocol_module_sessionless obj3;

	boost::thread t3;
	boost::array<char,MAX_BUFFER_SIZE> sbf3;
	std::size_t d3;
	boost::asio::ip::tcp::endpoint ep3;

	//session_thread_data_sessionless
	protocol_module_sessionless::session_thread_data_sessionless data3;
	obj1.session_thread_data_map[t3.get_id()] = &data3;
	//send_status
	protocol_module_sessionless::send_status sd3;
	sd3.status = protocol_module_sessionless::SEND_OK;
	sd3.send_possible_size = 4096;
	sd3.send_offset = 1;
	//recive_data
	protocol_module_sessionless::recive_data rd3;
	memset(rd3.recive_buffer,96,4096);
	//send_status->recive_data
	rd3.send_status_list.push_back(sd3);
	//recive_data->session_thread_data_sessionless
	data3.recive_data_map[ep3] = rd3;

	ret = obj3.handle_client_connection_check(t3.get_id(),sbf3,d3);
	//���M�\�f�[�^��擪���瑗�M�\�f�[�^�T�C�Y���A���M�o�b�t�@�ɃR�s�[����
	char *chk3;
	memset(chk3,96,4095);
	BOOST_CHECK_EQUAL(chk3, sbf3.data());
	//���M�σf�[�^�T�C�Y�ɑ��M�\�f�[�^�T�C�Y��ݒ肷��
	BOOST_CHECK_EQUAL(data3.recive_data_map[ep3].send_status_list.rbegin()->send_end_size, 4096);
	//���M�\�f�[�^�T�C�Y��0��ݒ肷��
	BOOST_CHECK_EQUAL(data3.recive_data_map[ep3].send_status_list.rbegin()->send_possible_size, 0);
	//�J�ڐ�X�e�[�^�X��ݒ肷��
	BOOST_CHECK_EQUAL(protocol_module_base::CLIENT_SEND, ret);

}

//handle_client_select
void handle_client_select_test(){
	//unit_test[1] STOP
	protocol_module_sessionless obj;

	boost::thread t;
	boost::asio::ip::udp::endpoint ep;
	boost::array<char,MAX_BUFFER_SIZE> sbf;
	std::size_t d;
	protocol_module_sessionless::EVENT_TAG ret;
	ret = obj.handle_client_select(t.get_id(),ep,sbf,d);

	BOOST_CHECK_EQUAL(protocol_module_base::STOP, ret);
}

//handle_client_send

//handle_client_disconnect �g�Ə�
void handle_client_disconnect_test(){
	//unit_test[12] �J�ڐ�X�e�[�^�X��ݒ肷��
	protocol_module_sessionless obj;

	protocol_module_sessionless::EVENT_TAG ret;
	boost::thread t;

	ret=obj.handle_client_disconnect(t.get_id());
	BOOST_CHECK_EQUAL(protocol_module_base::FINALIZE,ret);
}

//handle_sorry_enable �n����
void thread_t(){}
void handle_sorry_enable_test() {
	protocol_module_sessionless sessionless;
	protocol_module_sessionless::EVENT_TAG status;
	protocol_module_sessionless::send_status sendstatus;
	protocol_module_sessionless::recive_data receivedata;
	session_thread_data *thread_data = new session_thread_data();
	boost::thread thread_up(&thread_t);
	boost::thread thread_down(&thread_t);
	boost::thread::id thread_id_up = thread_up.get_id();
	boost::thread::id thread_id_down = thread_down.get_id();
	boost::asio::ip::tcp::endpoint endpoint;
	boost::asio::ip::tcp::endpoint endpoint_down;

	// ���X���b�h�̏ꍇ
	thread_data->thread_division = 0;
	thread_data->thread_id = thread_id_up;
	sessionless.session_thread_data_map.insert(std::pair<boost::thread::id, session_thread_data*>(thread_id_up,thread_data));

	// unit_test[26] sorry��Ԃ�ON,status = protocol_module_sessionless::ACCEPT
	// unit_test[26] test data:accept�����t���OOFF�̏ꍇ
	sessionless.session_thread_data_map[thread_id_up]->accept_end_flag = 0;
	status = sessionless.handle_sorry_enable(thread_id_up);
	BOOST_CHECK_EQUAL(sessionless.session_thread_data_map[thread_id_up]->sorry_flag,1);
	BOOST_CHECK_EQUAL(status,protocol_module_sessionless::ACCEPT);

	// unit_test[27] status =SORRYSERVER_SELECT
	// unit_test[27] test data:accept�����t���OON�̏ꍇ,sorry��Ԃ̏ꍇ,���M�\�f�[�^����̏ꍇ list 1��
	sessionless.session_thread_data_map[thread_id_up]->accept_end_flag = 1;
	sessionless.session_thread_data_map[thread_id_up]->sorry_flag = 1;
	sendstatus.status = protocol_module_sessionless::SEND_OK;
	sendstatus.send_possible_size = 10;
	receivedata.send_status_list.push_back(sendstatus);
	sessionless.session_thread_data_map[thread_id_up]->recive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::recive_data>(endpoint,receivedata));
	status = sessionless.handle_sorry_enable(thread_id_up);
	BOOST_CHECK_EQUAL(status,protocol_module_sessionless::SORRYSERVER_SELECT);
	sessionless.session_thread_data_map[thread_id_up]->recive_data_map.clear();

	// unit_test[28] status =SORRYSERVER_SELECT
	// unit_test[28] test data:���M�\�f�[�^����̏ꍇ list 2�� 1������
	sendstatus.status = protocol_module_sessionless::SEND_OK;
	sendstatus.send_possible_size = 10;
	receivedata.send_status_list.push_back(sendstatus);
	sendstatus.status = protocol_module_sessionless::SEND_END;
	receivedata.send_status_list.push_back(sendstatus);
	sessionless.session_thread_data_map[thread_id_up]->recive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::recive_data>(endpoint,receivedata));
	status = sessionless.handle_sorry_enable(thread_id_up);
	BOOST_CHECK_EQUAL(status,protocol_module_sessionless::SORRYSERVER_SELECT);
	sessionless.session_thread_data_map[thread_id_up]->recive_data_map.clear();

	// unit_test[29] status =CLIENT_RECV
	// unit_test[29] test data:���M�\�f�[�^�Ȃ� list 3��
	sendstatus.status = protocol_module_sessionless::SEND_END;
	receivedata.send_status_list.push_back(sendstatus);
	sendstatus.status = protocol_module_sessionless::SEND_CONTINUE;
	receivedata.send_status_list.push_back(sendstatus);
	sendstatus.status = protocol_module_sessionless::SEND_NG;
	receivedata.send_status_list.push_back(sendstatus);
	sessionless.session_thread_data_map[thread_id_up]->recive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::recive_data>(endpoint,receivedata));
	status = sessionless.handle_sorry_enable(thread_id_up);
	BOOST_CHECK_EQUAL(status,protocol_module_sessionless::CLIENT_RECV);
	sessionless.session_thread_data_map[thread_id_up]->recive_data_map.clear();

	// unit_test[30] �I���t���O��ON,status = REALSERVER_DISCONNECT
	// unit_test[30] test data:sorry��ԈȊO�̏ꍇ,���M�p���f�[�^���� list 1��
	sessionless.session_thread_data_map[thread_id_up]->sorry_flag = 0;
	sendstatus.status = protocol_module_sessionless::SEND_CONTINUE;
	receivedata.send_status_list.push_back(sendstatus);
	sessionless.session_thread_data_map[thread_id_up]->recive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::recive_data>(endpoint,receivedata));
	status = sessionless.handle_sorry_enable(thread_id_up);
	BOOST_CHECK_EQUAL(sessionless.session_thread_data_map[thread_id_up]->end_flag,1);
	BOOST_CHECK_EQUAL(status,protocol_module_sessionless::REALSERVER_DISCONNECT);
	sessionless.session_thread_data_map[thread_id_up]->recive_data_map.clear();

	// unit_test[31] �I���t���O��ON,status = REALSERVER_DISCONNECT
	// unit_test[31] test data:���M�p���f�[�^���� list 2�� 1������
	sendstatus.status = protocol_module_sessionless::SEND_CONTINUE;
	receivedata.send_status_list.push_back(sendstatus);
	sendstatus.status = protocol_module_sessionless::SEND_NG;
	receivedata.send_status_list.push_back(sendstatus);
	sessionless.session_thread_data_map[thread_id_up]->recive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::recive_data>(endpoint,receivedata));
	status = sessionless.handle_sorry_enable(thread_id_up);
	BOOST_CHECK_EQUAL(sessionless.session_thread_data_map[thread_id_up]->end_flag,1);
	BOOST_CHECK_EQUAL(status,protocol_module_sessionless::REALSERVER_DISCONNECT);
	sessionless.session_thread_data_map[thread_id_up]->recive_data_map.clear();

	// unit_test[32] sorryserver�ؑ֒��t���OON,sorry��Ԃ�ON,status = REALSERVER_DISCONNECT
	// unit_test[32] test data:���M�p���f�[�^�Ȃ� list 3��
	sendstatus.status = protocol_module_sessionless::SEND_END;
	receivedata.send_status_list.push_back(sendstatus);
	sendstatus.status = protocol_module_sessionless::SEND_NG;
	receivedata.send_status_list.push_back(sendstatus);
	sendstatus.status = protocol_module_sessionless::SEND_OK;
	receivedata.send_status_list.push_back(sendstatus);
	sessionless.session_thread_data_map[thread_id_up]->recive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::recive_data>(endpoint,receivedata));
	status = sessionless.handle_sorry_enable(thread_id_up);
	BOOST_CHECK_EQUAL(sessionless.session_thread_data_map[thread_id_up]->sorryserver_switch_flag,1);
	BOOST_CHECK_EQUAL(sessionless.session_thread_data_map[thread_id_up]->sorry_flag,1);
	BOOST_CHECK_EQUAL(status,protocol_module_sessionless::REALSERVER_DISCONNECT);
	sessionless.session_thread_data_map[thread_id_up]->recive_data_map.clear();

	// ����X���b�h�̏ꍇ
	sessionless.session_thread_data_map.clear();
	thread_data->thread_division = 1;
	thread_data->thread_id = thread_id_down;
	sessionless.session_thread_data_map.insert(std::pair<boost::thread::id, session_thread_data*>(thread_id_down,thread_data));

	// unit_test[33] status =CLIENT_CONNECTION_CHECK
	// unit_test[33] test data:sorry��Ԃ̏ꍇ,���M�\�f�[�^���� list 1��
	sessionless.session_thread_data_map[thread_id_down]->sorry_flag = 1;
	sendstatus.status = protocol_module_sessionless::SEND_OK;
	sendstatus.send_possible_size = 10;
	receivedata.send_status_list.push_back(sendstatus);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::recive_data>(endpoint_down,receivedata));
	status = sessionless.handle_sorry_enable(thread_id_down);
	BOOST_CHECK_EQUAL(status,protocol_module_sessionless::CLIENT_CONNECTION_CHECK);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.clear();

	// unit_test[34] status =CLIENT_CONNECTION_CHECK
	// unit_test[34] test data:���M�\�f�[�^���� list 2�� 1������
	sendstatus.status = protocol_module_sessionless::SEND_END;
	receivedata.send_status_list.push_back(sendstatus);
	sendstatus.status = protocol_module_sessionless::SEND_OK;
	sendstatus.send_possible_size = 10;
	receivedata.send_status_list.push_back(sendstatus);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::recive_data>(endpoint_down,receivedata));
	status = sessionless.handle_sorry_enable(thread_id_down);
	BOOST_CHECK_EQUAL(status,protocol_module_sessionless::CLIENT_CONNECTION_CHECK);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.clear();

	// unit_test[35] status =SORRYSERVER_RECV
	// unit_test[35] test data:���M�\�f�[�^�Ȃ� list 3��
	sendstatus.status = protocol_module_sessionless::SEND_END;
	receivedata.send_status_list.push_back(sendstatus);
	sendstatus.status = protocol_module_sessionless::SEND_CONTINUE;
	receivedata.send_status_list.push_back(sendstatus);
	sendstatus.status = protocol_module_sessionless::SEND_NG;
	receivedata.send_status_list.push_back(sendstatus);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::recive_data>(endpoint_down,receivedata));
	status = sessionless.handle_sorry_enable(thread_id_down);
	BOOST_CHECK_EQUAL(status,protocol_module_sessionless::SORRYSERVER_RECV);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.clear();

	// unit_test[36] �I���t���O��ON,status = REALSERVER_DISCONNECT
	// unit_test[36] test data:sorry��ԈȊO�̏ꍇ,���M�s�f�[�^���� list 1��
	sessionless.session_thread_data_map[thread_id_down]->sorry_flag = 0;
	sendstatus.status = protocol_module_sessionless::SEND_NG;
	receivedata.send_status_list.push_back(sendstatus);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::recive_data>(endpoint_down,receivedata));
	status = sessionless.handle_sorry_enable(thread_id_down);
	BOOST_CHECK_EQUAL(sessionless.session_thread_data_map[thread_id_down]->end_flag , 1);
	BOOST_CHECK_EQUAL(status,protocol_module_sessionless::REALSERVER_DISCONNECT);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.clear();

	// unit_test[37] �I���t���O��ON,status = REALSERVER_DISCONNECT
	// unit_test[37] test data:���M�f�[�^�c�T�C�Y �� �O�@�����݂���ꍇ list 2��
	sendstatus.status = protocol_module_sessionless::SEND_END;
	sendstatus.send_rest_size = 10;
	receivedata.send_status_list.push_back(sendstatus);
	sendstatus.status = protocol_module_sessionless::SEND_END;
	sendstatus.send_rest_size = 20;
	receivedata.send_status_list.push_back(sendstatus);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::recive_data>(endpoint_down,receivedata));
	status = sessionless.handle_sorry_enable(thread_id_down);
	BOOST_CHECK_EQUAL(sessionless.session_thread_data_map[thread_id_down]->end_flag , 1);
	BOOST_CHECK_EQUAL(status,protocol_module_sessionless::REALSERVER_DISCONNECT);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.clear();

	// unit_test[38] sorry��Ԃ�ON
	// unit_test[38] test data:���M�s�f�[�^�Ȃ��@���@���M�f�[�^�c�T�C�Y �� �O�@�����݂��Ȃ��ꍇ list 1��
	sendstatus.status = protocol_module_sessionless::SEND_END;
	sendstatus.send_rest_size = 0;
	receivedata.send_status_list.push_back(sendstatus);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::recive_data>(endpoint_down,receivedata));
	status = sessionless.handle_sorry_enable(thread_id_down);
	BOOST_CHECK_EQUAL(sessionless.session_thread_data_map[thread_id_down]->sorry_flag,1);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.clear();

	// unit_test[39]  status = SORRYSERVER_RECV
	// unit_test[39] test data:���M�s�f�[�^�Ȃ��@���@���M�f�[�^�c�T�C�Y �� �O�@�����݂��Ȃ��ꍇ,���M�\�f�[�^�Ȃ� list 1��
	sendstatus.status = protocol_module_sessionless::SEND_END;
	sendstatus.send_rest_size = 0;
	receivedata.send_status_list.push_back(sendstatus);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::recive_data>(endpoint_down,receivedata));
	status = sessionless.handle_sorry_enable(thread_id_down);
	BOOST_CHECK_EQUAL(status,protocol_module_sessionless::SORRYSERVER_RECV);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.clear();

	// unit_test[40] ���M�\�f�[�^���� status = CLIENT_CONNECTION_CHECK
	// unit_test[40] test data:���M�s�f�[�^�Ȃ��@���@���M�f�[�^�c�T�C�Y �� �O�@�����݂��Ȃ��ꍇ,���M�\�f�[�^���� list 3��
	sendstatus.status = protocol_module_sessionless::SEND_OK;
	sendstatus.send_rest_size = 0;
	receivedata.send_status_list.push_back(sendstatus);
	sendstatus.status = protocol_module_sessionless::SEND_CONTINUE;
	sendstatus.send_rest_size = 0;
	receivedata.send_status_list.push_back(sendstatus);
	sendstatus.status = protocol_module_sessionless::SEND_END;
	sendstatus.send_rest_size = 0;
	receivedata.send_status_list.push_back(sendstatus);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::recive_data>(endpoint_down,receivedata));
	status = sessionless.handle_sorry_enable(thread_id_down);
	BOOST_CHECK_EQUAL(status,protocol_module_sessionless::CLIENT_CONNECTION_CHECK);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.clear();

	// unit_test[41] status =FINALIZE
	// unit_test[41] test data:thread_id�Ή���session_thread_data�Ȃ�
	sendstatus.status = protocol_module_sessionless::SEND_OK;
	receivedata.send_status_list.push_back(sendstatus);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::recive_data>(endpoint_down,receivedata));
	status = sessionless.handle_sorry_enable(thread_id_up);
	BOOST_CHECK_EQUAL(status,protocol_module_sessionless::FINALIZE);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.clear();

}

//handle_sorry_disable �n����
void handle_sorry_disable_test(){
	protocol_module_sessionless sessionless;
	protocol_module_sessionless::EVENT_TAG status;
	protocol_module_sessionless::send_status sendstatus;
	protocol_module_sessionless::recive_data receivedata;
	session_thread_data *thread_data = new session_thread_data();
	boost::thread thread_up(&thread_t);
	boost::thread thread_down(&thread_t);
	boost::thread::id thread_id_up = thread_up.get_id();
	boost::thread::id thread_id_down = thread_down.get_id();
	boost::asio::ip::tcp::endpoint endpoint;
	boost::asio::ip::tcp::endpoint endpoint_down;

	// ���X���b�h�̏ꍇ
	thread_data->thread_division = 0;
	thread_data->thread_id = thread_id_up;
	sessionless.session_thread_data_map.insert(std::pair<boost::thread::id, session_thread_data*>(thread_id_up,thread_data));

	// unit_test[42] sorry��Ԃ�OFF,status =ACCEPT
	// unit_test[42] test data:accept�����t���OOFF�̏ꍇ
	sessionless.session_thread_data_map[thread_id_up]->accept_end_flag = 0;
	status = sessionless.handle_sorry_disable(thread_id_up);
	BOOST_CHECK_EQUAL(sessionless.session_thread_data_map[thread_id_up]->sorry_flag,0);
	BOOST_CHECK_EQUAL(status,protocol_module_sessionless::ACCEPT);

	// unit_test[43] �I���t���O��ON,status = SORRYSERVER_DISCONNECT
	// unit_test[43] test data:accept�����t���OON�̏ꍇ,sorry��Ԃ̏ꍇ,���M�p���f�[�^���� list 1��
	sessionless.session_thread_data_map[thread_id_up]->accept_end_flag = 1;
	sessionless.session_thread_data_map[thread_id_up]->sorry_flag = 1;
	sendstatus.status = protocol_module_sessionless::SEND_CONTINUE;
	receivedata.send_status_list.push_back(sendstatus);
	sessionless.session_thread_data_map[thread_id_up]->recive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::recive_data>(endpoint,receivedata));
	status = sessionless.handle_sorry_disable(thread_id_up);
	BOOST_CHECK_EQUAL(sessionless.session_thread_data_map[thread_id_up]->end_flag,1);
	BOOST_CHECK_EQUAL(status,protocol_module_sessionless::SORRYSERVER_DISCONNECT);
	sessionless.session_thread_data_map[thread_id_up]->recive_data_map.clear();

	// unit_test[44] �I���t���O��ON,status = SORRYSERVER_DISCONNECT
	// unit_test[44] test data:���M�p���f�[�^���� list 2�� 1������
	sendstatus.status = protocol_module_sessionless::SEND_CONTINUE;
	receivedata.send_status_list.push_back(sendstatus);
	sendstatus.status = protocol_module_sessionless::SEND_NG;
	receivedata.send_status_list.push_back(sendstatus);
	sessionless.session_thread_data_map[thread_id_up]->recive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::recive_data>(endpoint,receivedata));
	status = sessionless.handle_sorry_disable(thread_id_up);
	BOOST_CHECK_EQUAL(sessionless.session_thread_data_map[thread_id_up]->end_flag,1);
	BOOST_CHECK_EQUAL(status,protocol_module_sessionless::SORRYSERVER_DISCONNECT);
	sessionless.session_thread_data_map[thread_id_up]->recive_data_map.clear();

	// unit_test[45] realserver�ؑ֒�,sorry��Ԃ�OFF,status = SORRYSERVER_DISCONNECT
	// unit_test[45] test data:���M�p���f�[�^�Ȃ� list 3��
	sendstatus.status = protocol_module_sessionless::SEND_END;
	receivedata.send_status_list.push_back(sendstatus);
	sendstatus.status = protocol_module_sessionless::SEND_NG;
	receivedata.send_status_list.push_back(sendstatus);
	sendstatus.status = protocol_module_sessionless::SEND_OK;
	receivedata.send_status_list.push_back(sendstatus);
	sessionless.session_thread_data_map[thread_id_up]->recive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::recive_data>(endpoint,receivedata));
	status = sessionless.handle_sorry_disable(thread_id_up);
	BOOST_CHECK_EQUAL(sessionless.session_thread_data_map[thread_id_up]->realserver_switch_flag,1);
	BOOST_CHECK_EQUAL(sessionless.session_thread_data_map[thread_id_up]->sorry_flag,0);
	BOOST_CHECK_EQUAL(status,protocol_module_sessionless::SORRYSERVER_DISCONNECT);
	sessionless.session_thread_data_map[thread_id_up]->recive_data_map.clear();

	// unit_test[46] status =REALSERVER_SELECT
	// unit_test[46] test data:sorry��ԈȊO�̏ꍇ,���M�\�f�[�^����̏ꍇ list 1��
	sessionless.session_thread_data_map[thread_id_up]->sorry_flag = 0;
	sendstatus.status = protocol_module_sessionless::SEND_OK;
	receivedata.send_status_list.push_back(sendstatus);
	sessionless.session_thread_data_map[thread_id_up]->recive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::recive_data>(endpoint,receivedata));
	status = sessionless.handle_sorry_disable(thread_id_up);
	BOOST_CHECK_EQUAL(status,protocol_module_sessionless::REALSERVER_SELECT);
	sessionless.session_thread_data_map[thread_id_up]->recive_data_map.clear();

	// unit_test[47] status =REALSERVER_SELECT
	// unit_test[47] test data:���M�\�f�[�^����̏ꍇ list 2�� 1������
	sendstatus.status = protocol_module_sessionless::SEND_OK;
	receivedata.send_status_list.push_back(sendstatus);
	sendstatus.status = protocol_module_sessionless::SEND_END;
	receivedata.send_status_list.push_back(sendstatus);
	sessionless.session_thread_data_map[thread_id_up]->recive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::recive_data>(endpoint,receivedata));
	status = sessionless.handle_sorry_disable(thread_id_up);
	BOOST_CHECK_EQUAL(status,protocol_module_sessionless::REALSERVER_SELECT);
	sessionless.session_thread_data_map[thread_id_up]->recive_data_map.clear();

	// unit_test[48] status =CLIENT_RECV
	// unit_test[48] test data:���M�\�f�[�^�Ȃ� list 3��
	sendstatus.status = protocol_module_sessionless::SEND_END;
	receivedata.send_status_list.push_back(sendstatus);
	sendstatus.status = protocol_module_sessionless::SEND_CONTINUE;
	receivedata.send_status_list.push_back(sendstatus);
	sendstatus.status = protocol_module_sessionless::SEND_NG;
	receivedata.send_status_list.push_back(sendstatus);
	sessionless.session_thread_data_map[thread_id_up]->recive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::recive_data>(endpoint,receivedata));
	status = sessionless.handle_sorry_disable(thread_id_up);
	BOOST_CHECK_EQUAL(status,protocol_module_sessionless::CLIENT_RECV);
	sessionless.session_thread_data_map[thread_id_up]->recive_data_map.clear();

	// ����X���b�h�̏ꍇ
	sessionless.session_thread_data_map.clear();
	thread_data->thread_division = 1;
	thread_data->thread_id = thread_id_down;
	sessionless.session_thread_data_map.insert(std::pair<boost::thread::id, session_thread_data*>(thread_id_down,thread_data));

	// unit_test[49] �I���t���O��ON,status = SORRYSERVER_DISCONNECT
	// unit_test[49] test data:sorry��Ԃ̏ꍇ,���M�s�f�[�^���� list 1��
	sessionless.session_thread_data_map[thread_id_down]->sorry_flag = 1;
	sendstatus.status = protocol_module_sessionless::SEND_NG;
	receivedata.send_status_list.push_back(sendstatus);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::recive_data>(endpoint_down,receivedata));
	status = sessionless.handle_sorry_disable(thread_id_down);
	BOOST_CHECK_EQUAL(sessionless.session_thread_data_map[thread_id_down]->end_flag , 1);
	BOOST_CHECK_EQUAL(status,protocol_module_sessionless::SORRYSERVER_DISCONNECT);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.clear();

	// unit_test[50] �I���t���O��ON,status = SORRYSERVER_DISCONNECT
	// unit_test[50] test data:���M�f�[�^�c�T�C�Y �� �O�@�����݂���ꍇ list 2��
	sendstatus.status = protocol_module_sessionless::SEND_END;
	sendstatus.send_rest_size = 10;
	receivedata.send_status_list.push_back(sendstatus);
	sendstatus.status = protocol_module_sessionless::SEND_END;
	sendstatus.send_rest_size = 20;
	receivedata.send_status_list.push_back(sendstatus);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::recive_data>(endpoint_down,receivedata));
	status = sessionless.handle_sorry_disable(thread_id_down);
	BOOST_CHECK_EQUAL(sessionless.session_thread_data_map[thread_id_down]->end_flag , 1);
	BOOST_CHECK_EQUAL(status,protocol_module_sessionless::SORRYSERVER_DISCONNECT);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.clear();

	// unit_test[51] sorry��Ԃ�OFF
	// unit_test[51] test data:���M�s�f�[�^�Ȃ��@���@���M�f�[�^�c�T�C�Y �� �O�@�����݂��Ȃ��ꍇ list 1��
	sendstatus.status = protocol_module_sessionless::SEND_END;
	sendstatus.send_rest_size = 0;
	receivedata.send_status_list.push_back(sendstatus);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::recive_data>(endpoint_down,receivedata));
	status = sessionless.handle_sorry_disable(thread_id_down);
	BOOST_CHECK_EQUAL(sessionless.session_thread_data_map[thread_id_down]->sorry_flag,0);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.clear();

	// unit_test[52] sorry��Ԃ�OFF,���M�\�f�[�^�Ȃ� status = REALSERVER_RECV
	// unit_test[52] test data:���M�s�f�[�^�Ȃ��@���@���M�f�[�^�c�T�C�Y �� �O�@�����݂��Ȃ��ꍇ,���M�\�f�[�^�Ȃ� list 1��
	sendstatus.status = protocol_module_sessionless::SEND_END;
	sendstatus.send_rest_size = 0;
	receivedata.send_status_list.push_back(sendstatus);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::recive_data>(endpoint_down,receivedata));
	status = sessionless.handle_sorry_disable(thread_id_down);
	BOOST_CHECK_EQUAL(status,protocol_module_sessionless::REALSERVER_RECV);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.clear();

	// unit_test[53] status = CLIENT_CONNECTION_CHECK
	// unit_test[53] test data:���M�s�f�[�^�Ȃ��@���@���M�f�[�^�c�T�C�Y �� �O�@�����݂��Ȃ��ꍇ,���M�\�f�[�^���� list 3��
	sendstatus.status = protocol_module_sessionless::SEND_OK;
	sendstatus.send_rest_size = 0;
	receivedata.send_status_list.push_back(sendstatus);
	sendstatus.status = protocol_module_sessionless::SEND_CONTINUE;
	sendstatus.send_rest_size = 0;
	receivedata.send_status_list.push_back(sendstatus);
	sendstatus.status = protocol_module_sessionless::SEND_END;
	sendstatus.send_rest_size = 0;
	receivedata.send_status_list.push_back(sendstatus);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::recive_data>(endpoint_down,receivedata));
	status = sessionless.handle_sorry_disable(thread_id_down);
	BOOST_CHECK_EQUAL(status,protocol_module_sessionless::CLIENT_CONNECTION_CHECK);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.clear();

	// unit_test[54] status =CLIENT_CONNECTION_CHECK
	// unit_test[54] test data:sorry��ԈȊO�̏ꍇ,���M�\�f�[�^���� list 1��
	sessionless.session_thread_data_map[thread_id_down]->sorry_flag = 0;
	sendstatus.status = protocol_module_sessionless::SEND_OK;
	receivedata.send_status_list.push_back(sendstatus);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::recive_data>(endpoint_down,receivedata));
	status = sessionless.handle_sorry_disable(thread_id_down);
	BOOST_CHECK_EQUAL(status,protocol_module_sessionless::CLIENT_CONNECTION_CHECK);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.clear();

	// unit_test[55] status =CLIENT_CONNECTION_CHECK
	// unit_test[55] test data:���M�\�f�[�^���� list 2�� 1������
	sendstatus.status = protocol_module_sessionless::SEND_END;
	receivedata.send_status_list.push_back(sendstatus);
	sendstatus.status = protocol_module_sessionless::SEND_OK;
	receivedata.send_status_list.push_back(sendstatus);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::recive_data>(endpoint_down,receivedata));
	status = sessionless.handle_sorry_disable(thread_id_down);
	BOOST_CHECK_EQUAL(status,protocol_module_sessionless::CLIENT_CONNECTION_CHECK);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.clear();

	// unit_test[56] status =REALSERVER_RECV
	// unit_test[56] test data:���M�\�f�[�^�Ȃ� list 3��
	sendstatus.status = protocol_module_sessionless::SEND_END;
	receivedata.send_status_list.push_back(sendstatus);
	sendstatus.status = protocol_module_sessionless::SEND_CONTINUE;
	receivedata.send_status_list.push_back(sendstatus);
	sendstatus.status = protocol_module_sessionless::SEND_NG;
	receivedata.send_status_list.push_back(sendstatus);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::recive_data>(endpoint_down,receivedata));
	status = sessionless.handle_sorry_disable(thread_id_down);
	BOOST_CHECK_EQUAL(status,protocol_module_sessionless::REALSERVER_RECV);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.clear();

	// unit_test[57] status =FINALIZE
	// unit_test[57] test data:thread_id�Ή���session_thread_data�Ȃ�
	sendstatus.status = protocol_module_sessionless::SEND_OK;
	receivedata.send_status_list.push_back(sendstatus);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::recive_data>(endpoint_down,receivedata));
	status = sessionless.handle_sorry_disable(thread_id_up);
	BOOST_CHECK_EQUAL(status,protocol_module_sessionless::FINALIZE);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.clear();
}

//handle_realserver_disconnect(tcp)
void handle_realserver_disconnect_test(){

	//unit_test[13] ���X���b�h�̏ꍇ->�I���t���O��ON�̏ꍇ
	protocol_module_sessionless obj1;

	boost::thread t1;
	boost::asio::ip::tcp::endpoint ep1;
	protocol_module_sessionless::EVENT_TAG ret;

	protocol_module_sessionless::session_thread_data_sessionless data1;
	obj1.session_thread_data_map[t1.get_id()] = &data1;
	//make ���X���b�h�̏ꍇ 0->���X���b�h
	data1.thread_division = 0;
	//make �I���t���O��ON�̏ꍇ 1->on
	data1.end_flag = 1;

	ret = obj1.handle_realserver_disconnect(t1.get_id(), ep1);
	//�J�ڐ�X�e�[�^�X��ݒ肷�� status = CLIENT_RECV
	BOOST_CHECK_EQUAL(protocol_module_base::CLIENT_RECV, ret);


	//unit_test[14] ���X���b�h�̏ꍇ->�I���t���O��OFF�̏ꍇ->sorryserver�ؑ֒��̏ꍇ->���M�\�f�[�^����
	protocol_module_sessionless obj2;

	boost::thread t2;
	boost::asio::ip::tcp::endpoint ep2;

	protocol_module_sessionless::session_thread_data_sessionless data2;
	obj2.session_thread_data_map[t2.get_id()] = &data2;
	//make ���X���b�h�̏ꍇ 0->���X���b�h
	data2.thread_division = 0;
	//make �I���t���O��OFF�̏ꍇ 0->off
	data2.end_flag = 0;
	//make sorryserver�ؑ֒��̏ꍇ 1->�ؑ֒�
	data2.sorryserver_switch_flag = 1;
	//make ���M�\�f�[�^����
	protocol_module_sessionless::send_status st2;
	st2.status = protocol_module_sessionless::SEND_OK;
	st2.send_possible_size = 100;
	data2.recive_data_map[ep2].send_status_list.push_back(st2);


	ret = obj2.handle_realserver_disconnect(t2.get_id(),ep2);
	//�J�ڐ�X�e�[�^�X��ݒ肷�� status = SORRYSERVER_SELECT
	BOOST_CHECK_EQUAL(protocol_module_base::SORRYSERVER_SELECT, ret);


	//unit_test[15] ���X���b�h�̏ꍇ->�I���t���O��OFF�̏ꍇ->sorryserver�ؑ֒��̏ꍇ->���M�\�f�[�^�Ȃ�
	protocol_module_sessionless obj3;

	boost::thread t3;
	boost::asio::ip::tcp::endpoint ep3;

	protocol_module_sessionless::session_thread_data_sessionless data3;
	obj3.session_thread_data_map[t3.get_id()] = &data3;
	//make ���X���b�h�̏ꍇ 0->���X���b�h
	data3.thread_division = 0;
	//make �I���t���O��OFF�̏ꍇ 0->off
	data3.end_flag = 0;
	//make sorryserver�ؑ֒��̏ꍇ 1->�ؑ֒�
	data3.sorryserver_switch_flag = 1;
	//make ���M�\�f�[�^�Ȃ�
	protocol_module_sessionless::send_status st3;
	//send_possible=0
	st3.send_possible_size = 0;
	data3.recive_data_map[ep3].send_status_list.push_back(st3);

	ret = obj3.handle_realserver_disconnect(t3.get_id(), ep3);
	//�J�ڐ�X�e�[�^�X��ݒ肷�� status = CLIENT_RECV
	BOOST_CHECK_EQUAL(protocol_module_base::CLIENT_RECV, ret);


	//unit_test[16] ���X���b�h�̏ꍇ->�I���t���O��OFF�̏ꍇ->sorryserver�ؑ֒��łȂ��ꍇ
	protocol_module_sessionless obj4;

	boost::thread t4;
	boost::asio::ip::tcp::endpoint ep4;

	protocol_module_sessionless::session_thread_data_sessionless data4;
	obj4.session_thread_data_map[t4.get_id()] = &data4;
	//make ���X���b�h�̏ꍇ 0->���X���b�h
	data4.thread_division = 0;
	//make �I���t���O��OFF�̏ꍇ 0->off
	data4.end_flag = 0;
	//make sorryserver�ؑ֒��łȂ��ꍇ 0->�ؑ֒��łȂ�
	data4.sorryserver_switch_flag = 0;

	ret = obj4.handle_realserver_disconnect(t4.get_id(),ep4);
	//�I���t���O��ON
	BOOST_CHECK_EQUAL(data4.end_flag, 1);
	//�J�ڐ�X�e�[�^�X��ݒ肷�� status = CLIENT_RECV
	BOOST_CHECK_EQUAL(protocol_module_base::CLIENT_RECV, ret);


	//unit_test[17] ����X���b�h�̏ꍇ->���M�\�f�[�^����
	protocol_module_sessionless obj5;

	boost::thread t5;
	boost::asio::ip::tcp::endpoint ep5;

	protocol_module_sessionless::session_thread_data_sessionless data5;
	obj5.session_thread_data_map[t5.get_id()] = &data5;
	//make ����X���b�h�̏ꍇ 1->����X���b�h
	data5.thread_division = 1;
	//make ���M�\�f�[�^����
	protocol_module_sessionless::send_status st5;
	st5.status = protocol_module_sessionless::SEND_OK;
	//send_possible>0
	st5.send_possible_size = 100;
	data5.recive_data_map[ep5].send_status_list.push_back(st5);

	ret = obj5.handle_realserver_disconnect(t5.get_id(), ep5);
	//�I���t���O��ON
	BOOST_CHECK_EQUAL(data5.end_flag, 1);
	//�J�ڐ�X�e�[�^�X��ݒ肷�� status = CLIENT_CONNECTION_CHECK
	BOOST_CHECK_EQUAL(protocol_module_base::CLIENT_CONNECTION_CHECK, ret);


	//unit_test[18] ����X���b�h�̏ꍇ->���M�\�f�[�^�Ȃ�
	protocol_module_sessionless obj6;

	boost::thread t6;
	boost::asio::ip::tcp::endpoint ep6;

	protocol_module_sessionless::session_thread_data_sessionless data6;
	obj6.session_thread_data_map[t6.get_id()] = &data6;
	//make ����X���b�h�̏ꍇ 1->����X���b�h
	data6.thread_division = 1;
	//make ���M�\�f�[�^�Ȃ�
	protocol_module_sessionless::send_status st6;
	//send_possible>0
	st5.send_possible_size = 0;
	data6.recive_data_map[ep6].send_status_list.push_back(st6);

	ret = obj6.handle_realserver_disconnect(t6.get_id(), ep6);
	//�I���t���O��ON
	BOOST_CHECK_EQUAL(data6.end_flag, 1);
	//�J�ڐ�X�e�[�^�X��ݒ肷�� status = CLIENT_DISCONNECT
	BOOST_CHECK_EQUAL(protocol_module_base::CLIENT_DISCONNECT, ret);
}

//handle_realserver_disconnect(udp)

//handle_sorryserver_disconnect �g�Ə�
void handle_sorryserver_disconnect_test(){

	//unit_test[19] ���X���b�h�̏ꍇ->�I���t���O��ON�̏ꍇ
	protocol_module_sessionless obj1;

	boost::thread t1;
	boost::asio::ip::tcp::endpoint ep1;
	protocol_module_sessionless::EVENT_TAG ret;

	protocol_module_sessionless::session_thread_data_sessionless data1;
	obj1.session_thread_data_map[t1.get_id()] = &data1;
	//make ���X���b�h�̏ꍇ 0->���X���b�h
	data1.thread_division = 0;
	//make �I���t���O��ON�̏ꍇ 1->on
	data1.end_flag = 1;

	ret = obj1.handle_realserver_disconnect(t1.get_id(), ep1);
	//�J�ڐ�X�e�[�^�X��ݒ肷�� status = CLIENT_RECV
	BOOST_CHECK_EQUAL(protocol_module_base::CLIENT_RECV, ret);


	//unit_test[20] ���X���b�h�̏ꍇ->�I���t���O��OFF�̏ꍇ->realserver�ؑ֒��̏ꍇ->���M�\�f�[�^����
	protocol_module_sessionless obj2;

	boost::thread t2;
	boost::asio::ip::tcp::endpoint ep2;

	protocol_module_sessionless::session_thread_data_sessionless data2;
	obj2.session_thread_data_map[t2.get_id()] = &data2;
	//make ���X���b�h�̏ꍇ 0->���X���b�h
	data2.thread_division = 0;
	//make �I���t���O��OFF�̏ꍇ 0->off
	data2.end_flag = 0;
	//make realserver�ؑ֒��̏ꍇ 1->�ؑ֒�
	data2.realserver_switch_flag = 1;
	//make ���M�\�f�[�^����
	protocol_module_sessionless::send_status st2;
	st2.status = protocol_module_sessionless::SEND_OK;
	//send_possible>0
	st2.send_possible_size = 100;
	data2.recive_data_map[ep2].send_status_list.push_back(st2);

	ret = obj2.handle_realserver_disconnect(t2.get_id(), ep2);
	//�J�ڐ�X�e�[�^�X��ݒ肷�� status = REALSERVER_SELECT
	BOOST_CHECK_EQUAL(protocol_module_base::REALSERVER_SELECT, ret);


	//unit_test[21] ���X���b�h�̏ꍇ->�I���t���O��OFF�̏ꍇ->realserver�ؑ֒��̏ꍇ->���M�\�f�[�^�Ȃ�
	protocol_module_sessionless obj3;

	boost::thread t3;
	boost::asio::ip::tcp::endpoint ep3;

	protocol_module_sessionless::session_thread_data_sessionless data3;
	obj3.session_thread_data_map[t3.get_id()] = &data3;
	//make ���X���b�h�̏ꍇ 0->���X���b�h
	data3.thread_division = 0;
	//make �I���t���O��OFF�̏ꍇ 0->off
	data3.end_flag = 0;
	//make realserver�ؑ֒��̏ꍇ 1->�ؑ֒�
	data3.realserver_switch_flag = 1;
	//make ���M�\�f�[�^�Ȃ�
	protocol_module_sessionless::send_status st3;
	//send_possible=0
	st3.send_possible_size = 0;
	data3.recive_data_map[ep3].send_status_list.push_back(st3);

	ret = obj3.handle_realserver_disconnect(t3.get_id(), ep3);
	//�J�ڐ�X�e�[�^�X��ݒ肷�� status = CLIENT_RECV
	BOOST_CHECK_EQUAL(protocol_module_base::CLIENT_RECV, ret);


	//unit_test[22] ���X���b�h�̏ꍇ->�I���t���O��OFF�̏ꍇ->realserver�ؑ֒��łȂ��ꍇ
	protocol_module_sessionless obj4;

	boost::thread t4;
	boost::asio::ip::tcp::endpoint ep4;

	protocol_module_sessionless::session_thread_data_sessionless data4;
	obj4.session_thread_data_map[t4.get_id()] = &data4;
	//make ���X���b�h�̏ꍇ 0->���X���b�h
	data4.thread_division = 0;
	//make �I���t���O��OFF�̏ꍇ 0->off
	data4.end_flag = 0;
	//make realserver�ؑ֒��łȂ��ꍇ 0->�ؑ֒��łȂ�
	data4.realserver_switch_flag = 0;

	ret = obj4.handle_realserver_disconnect(t4.get_id(),ep4);
	//�I���t���O��ON
	BOOST_CHECK_EQUAL(data4.end_flag,1);
	//�J�ڐ�X�e�[�^�X��ݒ肷�� status = CLIENT_RECV
	BOOST_CHECK_EQUAL(protocol_module_base::CLIENT_RECV, ret);


	//unit_test[23] ����X���b�h�̏ꍇ->���M�\�f�[�^����
	protocol_module_sessionless obj5;

	boost::thread t5;
	boost::asio::ip::tcp::endpoint ep5;

	protocol_module_sessionless::session_thread_data_sessionless data5;
	obj5.session_thread_data_map[t5.get_id()] = &data5;
	//make ����X���b�h�̏ꍇ 1->����X���b�h
	data5.thread_division = 1;
	//make ���M�\�f�[�^����
	protocol_module_sessionless::send_status st5;
	st5.status = protocol_module_sessionless::SEND_OK;
	//send_possible>0
	st5.send_possible_size = 100;
	data5.recive_data_map[ep5].send_status_list.push_back(st5);

	ret = obj5.handle_realserver_disconnect(t5.get_id(),ep5);
	//�I���t���O��ON
	BOOST_CHECK_EQUAL(data5.end_flag, 1);
	//�J�ڐ�X�e�[�^�X��ݒ肷�� status = CLIENT_CONNECTION_CHECK
	BOOST_CHECK_EQUAL(protocol_module_base::CLIENT_CONNECTION_CHECK, ret);


	//unit_test[24] ����X���b�h�̏ꍇ->���M�\�f�[�^�Ȃ�
	protocol_module_sessionless obj6;

	boost::thread t6;
	boost::asio::ip::tcp::endpoint ep6;

	protocol_module_sessionless::session_thread_data_sessionless data6;
	obj6.session_thread_data_map[t6.get_id()] = &data6;
	//make ����X���b�h�̏ꍇ 1->����X���b�h
	data6.thread_division = 1;
	//make ���M�\�f�[�^�Ȃ�
	protocol_module_sessionless::send_status st6;
	//send_possible=0
	st6.send_possible_size = 0;
	data6.recive_data_map[ep6].send_status_list.push_back(st6);

	ret = obj6.handle_realserver_disconnect(t6.get_id(),ep6);
	//�I���t���O��ON
	BOOST_CHECK_EQUAL(data6.end_flag,1);
	//�J�ڐ�X�e�[�^�X��ݒ肷�� status = CLIENT_DISCONNECT
	BOOST_CHECK_EQUAL(protocol_module_base::CLIENT_DISCONNECT,ret);
}


test_suite*	init_unit_test_suite( int argc, char* argv[] ){

	test_suite* ts = BOOST_TEST_SUITE( "sessionless test" );

	ts->add(BOOST_TEST_CASE(&protocol_module_sessionless_test));
	ts->add(BOOST_TEST_CASE(&is_tcp_test));
	ts->add(BOOST_TEST_CASE(&is_udp_test));
	ts->add(BOOST_TEST_CASE(&get_name_test));
	ts->add(BOOST_TEST_CASE(&initialize_test));
	ts->add(BOOST_TEST_CASE(&finalize_test));
	ts->add(BOOST_TEST_CASE(&is_use_sorry_test));
	ts->add(BOOST_TEST_CASE(&check_parameter_test));
	ts->add(BOOST_TEST_CASE(&set_parameter_test));
	ts->add(BOOST_TEST_CASE(&add_parameter_test));
	ts->add(BOOST_TEST_CASE(&register_schedule_tcp_test));
	ts->add(BOOST_TEST_CASE(&handle_session_initialize_test));
	ts->add(BOOST_TEST_CASE(&handle_session_finalize_test));
	ts->add(BOOST_TEST_CASE(&handle_accept_test));
	ts->add(BOOST_TEST_CASE(&handle_realserver_select_test));
	ts->add(BOOST_TEST_CASE(&handle_realserver_connection_fail_test));
	ts->add(BOOST_TEST_CASE(&handle_sorryserver_select_test));
	ts->add(BOOST_TEST_CASE(&handle_client_connection_check_test));
	ts->add(BOOST_TEST_CASE(&handle_client_select_test));
	ts->add(BOOST_TEST_CASE(&handle_client_disconnect_test));
	ts->add(BOOST_TEST_CASE(&handle_sorry_enable_test));
	ts->add(BOOST_TEST_CASE(&handle_sorry_disable_test));
	ts->add(BOOST_TEST_CASE(&handle_realserver_disconnect_test));
	ts->add(BOOST_TEST_CASE(&handle_sorryserver_disconnect_test));

	framework::master_test_suite().add(ts);
	return 0;
}

