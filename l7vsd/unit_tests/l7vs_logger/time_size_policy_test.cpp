#include "../../l7vsd/logger/time_and_size_based_rolling_policy.cpp"
#include "../../l7vsd/logger/strict_time_based_rolling_policy.cpp"

#include <iostream>
#include <fstream>
#include <dlfcn.h>
#include <time.h>

#include <boost/test/unit_test.hpp>
#include <log4cxx/pattern/datepatternconverter.h>
#include <log4cxx/pattern/filedatepatternconverter.h>

#define LOG_DATE_FORMAT "%Y%m%d%H%M"

using namespace boost::unit_test_framework;

int triggeringPolicyAddRefCalled;

int triggeringPolicyReleaseRefCalled;

int parseFileNamePatternCalled;

int getDatePatternConverterCalled;
bool getDatePatternConverterReturnNull = false;

int formatFileNameCalled = 0;
LogString formatFileNameArgBuf = "";
ObjectPtr formatFileNameArgObj = 0;

int localtime_rCalled = 0;
time_t localtime_rArg = 0;
bool localtime_rFail = false;

int equalsIgnoreCaseCalled = 0;
bool equalsIgnoreCaseReturns = false;

int timeCalled = 0;
time_t timeArg = 0;
time_t timeReturns = 0;
bool timeReturnNow = true;

int strftimeCalled = 0;
bool strftimeFail = false;
std::string strftimeArgFormat = "";

int mktimeCalled = 0;
bool mktimeFail = false;

int fileListCalled = 0;
std::vector<LogString> fileListReturns;

int fileDeleteFileCalled = 0;
std::vector<std::string> fileDeleteFiles;

int fixedPolicySetOptionCalled = 0;
std::string fixedPolicySetOptionArgOption = "";
std::string fixedPolicySetOptionArgValue = "";

int fixedPolicyActivateOptionsCalled = 0;

int fixedPolicyInitializeCalled = 0;
std::string fixedPolicyInitializeArgFile = "";
bool fixedPolicyInitializeArgAppend = false;

int fixedPolicyRolloverCalled = 0;
std::string fixedPolicyRolloverArgActiveFile = "";


extern "C" {
void triggeringPolicyAddRef_(log4cxx::rolling::TriggeringPolicy* ts) {
	++triggeringPolicyAddRefCalled;
}

void triggeringPolicyReleaseRef_(log4cxx::rolling::TriggeringPolicy* ts) {
	++triggeringPolicyReleaseRefCalled;
}

void parseFileNamePattern_(log4cxx::rolling::RollingPolicyBase* ts) {
	++parseFileNamePatternCalled;
}

log4cxx::pattern::PatternConverterPtr getDatePatternConverter_(log4cxx::rolling::RollingPolicyBase* ts) {
	++getDatePatternConverterCalled;
	if (getDatePatternConverterReturnNull) {
		return 0;
	}

	std::vector<LogString> altOptions;
	altOptions.push_back(LOG4CXX_STR("yyyy-MM-dd"));
	return log4cxx::pattern::DatePatternConverter::newInstance(altOptions);
}

struct tm* localtime_r_(const time_t* timep, struct tm* result) {
	++localtime_rCalled;
	localtime_rArg = *timep;
	if (localtime_rFail) {
		return 0;
	}
	return ((struct tm* (*)(const time_t*, struct tm*)) dlsym(RTLD_NEXT, "localtime_r"))(timep, result);
}

bool equalsIgnoreCase_(const LogString& s1, const LogString& upper, const LogString& lower) {
	++equalsIgnoreCaseCalled;
	if ("rotationtimingvalue" == lower) return true;
	if ("rotationtiming" == lower) return true;
	if ("imaxbackupindex" == lower) return true;

	return false;
}

void formatFileName_(log4cxx::rolling::RollingPolicyBase* ts, log4cxx::helpers::ObjectPtr& obj, LogString& buf, log4cxx::helpers::Pool& p) {
	++formatFileNameCalled;
	formatFileNameArgObj = obj;
	buf = formatFileNameArgBuf;
}



time_t time_(time_t* t) {
	++timeCalled;
	if (timeReturnNow) {
		return ((time_t (*)(time_t*)) dlsym(RTLD_NEXT, "time"))(t);
	}
	*t = timeArg;
	return timeReturns;
}

size_t strftime_(char* s, size_t max, const char* format, const struct tm* tm) {
	++strftimeCalled;
	strftimeArgFormat = format;
	if (strftimeFail) {
		return -1;
	}
	return ((size_t (*)(char*, size_t, const char*, const struct tm*)) dlsym(RTLD_NEXT, "strftime"))(s, max, format, tm);
}

time_t mktime_(struct tm* tm) {
	++mktimeCalled;
	if (mktimeFail) {
		return -1;
	}
	return ((time_t (*)(struct tm*)) dlsym(RTLD_NEXT, "mktime"))(tm);
}

std::vector<LogString> fileList_(log4cxx::File* ts, log4cxx::helpers::Pool& p) {
	++fileListCalled;
	return fileListReturns;
}

bool fileDeleteFile_(log4cxx::File* ts, log4cxx::helpers::Pool& p) {
	++fileDeleteFileCalled;
	LogString filename = ts->getName();
	fileDeleteFiles.push_back(filename);
	return true;
}

void fixedPolicySetOption_(log4cxx::rolling::FixedWindowRollingPolicy* ts, const LogString&option, const LogString& value) {
	++fixedPolicySetOptionCalled;
	fixedPolicySetOptionArgOption = option;
	fixedPolicySetOptionArgValue = value;
}

void fixedPolicyActivateOptions_(log4cxx::rolling::FixedWindowRollingPolicy* ts, log4cxx::helpers::Pool& p) {
	++fixedPolicyActivateOptionsCalled;
}

log4cxx::rolling::RolloverDescriptionPtr fixedPolicyInitialize_(log4cxx::rolling::FixedWindowRollingPolicy* ts, const LogString& file, const bool append, log4cxx::helpers::Pool& p) {
	++fixedPolicyInitializeCalled;
	fixedPolicyInitializeArgFile = file;
	fixedPolicyInitializeArgAppend = append;
	log4cxx::rolling::RolloverDescriptionPtr desc;
	return desc;
}

log4cxx::rolling::RolloverDescriptionPtr fixedPolicyRollover_(log4cxx::rolling::FixedWindowRollingPolicy* ts, const LogString& activeFile, log4cxx::helpers::Pool& p) {
	++fixedPolicyRolloverCalled;
	fixedPolicyRolloverArgActiveFile = activeFile;
	log4cxx::rolling::RolloverDescriptionPtr desc;
	return desc;
}

void _ZNK7log4cxx7rolling16TriggeringPolicy6addRefEv(log4cxx::rolling::TriggeringPolicy* ts) __attribute__((weak,alias("triggeringPolicyAddRef_")));

void _ZNK7log4cxx7rolling16TriggeringPolicy10releaseRefEv(log4cxx::rolling::TriggeringPolicy* ts) __attribute__((weak,alias("triggeringPolicyReleaseRef_")));

void _ZN7log4cxx7rolling17RollingPolicyBase20parseFileNamePatternEv(log4cxx::rolling::RollingPolicyBase* ts) __attribute__((weak,alias("parseFileNamePattern_")));

log4cxx::pattern::PatternConverterPtr _ZNK7log4cxx7rolling17RollingPolicyBase23getDatePatternConverterEv(log4cxx::rolling::RollingPolicyBase* ts) __attribute__((weak,alias("getDatePatternConverter_")));

void _ZNK7log4cxx7rolling17RollingPolicyBase14formatFileNameERNS_7helpers10ObjectPtrTINS2_6ObjectEEERSsRNS2_4PoolE(log4cxx::rolling::RollingPolicyBase* ts, log4cxx::helpers::ObjectPtr& obj,LogString& buf, log4cxx::helpers::Pool& p) __attribute__((weak,alias("formatFileName_")));

struct tm* localtime_r(const time_t* timep, struct tm* result) __attribute__((weak,alias("localtime_r_")));

bool _ZN7log4cxx7helpers12StringHelper16equalsIgnoreCaseERKSsS3_S3_(LogString& s1, LogString& upper, LogString& lower) __attribute__((weak,alias("equalsIgnoreCase_")));
 
time_t time(time_t* t) __attribute__((weak,alias("time_")));

size_t strftime(char* s, size_t max, const char* format, const struct tm* tm) __attribute__((weak,alias("strftime_")));

time_t mktime(struct tm* tm) __attribute__((weak,alias("mktime_")));

std::vector<LogString> _ZNK7log4cxx4File4listERNS_7helpers4PoolE(log4cxx::File* ts, log4cxx::helpers::Pool& p) __attribute__((weak,alias("fileList_")));

bool _ZNK7log4cxx4File10deleteFileERNS_7helpers4PoolE(log4cxx::File* ts, log4cxx::helpers::Pool& p) __attribute__((weak,alias("fileDeleteFile_")));

void _ZN7log4cxx7rolling24FixedWindowRollingPolicy9setOptionERKSsS3_(log4cxx::rolling::FixedWindowRollingPolicy* ts, const LogString& option, const LogString& value) __attribute__((weak,alias("fixedPolicySetOption_")));

void _ZN7log4cxx7rolling24FixedWindowRollingPolicy15activateOptionsERNS_7helpers4PoolE(log4cxx::rolling::FixedWindowRollingPolicy* ts, log4cxx::helpers::Pool& p) __attribute__((weak,alias("fixedPolicyActivateOptions_")));

log4cxx::rolling::RolloverDescriptionPtr _ZN7log4cxx7rolling24FixedWindowRollingPolicy10initializeERKSsbRNS_7helpers4PoolE(log4cxx::rolling::FixedWindowRollingPolicy* ts, const LogString& file, const bool append, log4cxx::helpers::Pool& p) __attribute__((weak,alias("fixedPolicyInitialize_")));

log4cxx::rolling::RolloverDescriptionPtr _ZN7log4cxx7rolling24FixedWindowRollingPolicy8rolloverERKSsRNS_7helpers4PoolE(log4cxx::rolling::FixedWindowRollingPolicy* ts, const LogString& activeFile, log4cxx::helpers::Pool& p) __attribute__((weak,alias("fixedPolicyRollover_")));

}

class TestTimeSizePolicy : public log4cxx::rolling::TimeAndSizeBasedRollingPolicy 
{
public:
	time_t getNextCheckProperty() { return log4cxx::rolling::StrictTimeBasedRollingPolicy::nextCheck; }
	void setNextCheckProperty(time_t val) { log4cxx::rolling::StrictTimeBasedRollingPolicy::nextCheck = val; }
	time_t getNextCheck(time_t now_time) { return log4cxx::rolling::StrictTimeBasedRollingPolicy::getNextCheck(now_time); }

	log4cxx::pattern::PatternMap getFormatSpecifiers() { return log4cxx::rolling::StrictTimeBasedRollingPolicy::getFormatSpecifiers(); };
	
};


void time_size_policy_constructor_test()
{
	BOOST_MESSAGE( "----- time_size_policy_constructor_test start -----" );

	TestTimeSizePolicy p;

	//＊nextCheckが初期化されていること
	BOOST_CHECK_EQUAL(p.getNextCheckProperty(), 0);

	//＊rotationTimingが初期化されているkと
	BOOST_CHECK_EQUAL(p.getRotationTiming(), LOG_TIM_YEAR);

	//＊rotationTimingValueが初期化されていること
	BOOST_CHECK_EQUAL(p.getRotationTimingValue(), "");

	//＊maxFileSizeが初期化されていること
	BOOST_CHECK_EQUAL(p.getMaxFileSize(), (unsigned long long)DEFAULT_MAX_FILE_SIZE);

	BOOST_MESSAGE( "----- time_size_policy_constructor_test end -----" );

}


void time_size_policy_addref_test()
{
	BOOST_MESSAGE( "----- time_size_policy_addref_test start -----" );

	TestTimeSizePolicy p;

	//＊triggerinPolicyのaddrefを呼んでいること
	triggeringPolicyAddRefCalled = 0;
	p.addRef();
	BOOST_CHECK_EQUAL(triggeringPolicyAddRefCalled, 1);
  
	BOOST_MESSAGE( "----- time_size_policy_addref_test end -----" );
}

void time_size_policy_releaseref_test()
{
	BOOST_MESSAGE( "----- time_size_policy_releaseref_test start -----" );

	TestTimeSizePolicy p;

	//＊triggeringPolicyのreleaserefを呼んでいること
	triggeringPolicyReleaseRefCalled = 0;
	p.releaseRef();
	BOOST_CHECK_EQUAL(triggeringPolicyReleaseRefCalled, 1);
  
  
	BOOST_MESSAGE( "----- time_size_policy_releaseref_test end -----" );
}

void time_size_policy_activateoptions_test()
{
	BOOST_MESSAGE( "----- time_size_policy_activateoptions_test start -----" );

	TestTimeSizePolicy p;
	log4cxx::helpers::Pool pool;
#define DATE_PATTERN "test.log.%d{%Y-%m-%d_%H-%M}"

	//＊rotationTimingValueが設定されていなかった時にIlligalStateExceptionを投げること
	p.setFileNamePattern(DATE_PATTERN);
	p.setRotationTimingValue("");
	BOOST_CHECK_THROW(p.activateOptions(pool), log4cxx::helpers::IllegalStateException);

	//＊FixedRollingPolicyのactivateOptionsを呼んでいること
	fixedPolicyActivateOptionsCalled = 0;
	p.setRotationTimingValue("02111615");
	p.activateOptions(pool);
	BOOST_CHECK_EQUAL(fixedPolicyActivateOptionsCalled, 1);

  
	BOOST_MESSAGE( "----- time_size_policy_activateoptions_test end -----" );
}

void time_size_policy_get_rotation_timing_value_test()
{
	BOOST_MESSAGE( "----- time_size_policy_get_rotation_timing_value_test start -----" );
	TestTimeSizePolicy p;

	//＊設定されているrotationTimingValueを取得すること
	p.setRotationTimingValue("2020");
	BOOST_CHECK_EQUAL(p.getRotationTimingValue(), "2020");	
  
	BOOST_MESSAGE( "----- time_size_policy_get_rotation_timing_value_test end -----" );
}

void time_size_policy_set_rotation_timing_value_test()
{
	BOOST_MESSAGE( "----- time_size_policy_set_rotation_timing_value_test start -----" );
	TestTimeSizePolicy p;

	//＊rotationTimingValueを設定すること
	p.setRotationTimingValue("12311152");
	BOOST_CHECK_EQUAL(p.getRotationTimingValue(), "12311152");	
	
  
	BOOST_MESSAGE( "----- time_size_policy_set_rotation_timing_value_test end -----" );
}

void time_size_policy_get_rotation_timing_test()
{
	BOOST_MESSAGE( "----- time_size_policy_get_rotation_timing_test start -----" );
	TestTimeSizePolicy p;

	//＊設定されているrotationTimingを取得すること
	p.setRotationTiming(LOG_TIM_MONTH);
	BOOST_CHECK_EQUAL(p.getRotationTiming(), LOG_TIM_MONTH);	
	
  
	BOOST_MESSAGE( "----- time_size_policy_get_rotation_timing_test end -----" );
}

void time_size_policy_set_rotation_timing_test()
{
	BOOST_MESSAGE( "----- time_size_policy_set_rotation_timing_test start -----" );
	TestTimeSizePolicy p;

	//＊rotationTimingを設定すること
	p.setRotationTiming(LOG_TIM_HOUR);
	BOOST_CHECK_EQUAL(p.getRotationTiming(), LOG_TIM_HOUR);	
	
  
	BOOST_MESSAGE( "----- time_size_policy_set_rotation_timing_test end -----" );
}

void time_size_policy_get_max_file_size_test()
{
	BOOST_MESSAGE( "----- time_size_policy_get_max_file_size_test start -----" );
	TestTimeSizePolicy p;

	//＊設定されているmaxFileSizeを取得すること
	p.setMaxFileSize(12345);
	BOOST_CHECK_EQUAL(p.getMaxFileSize(), 12345U);
	
  
	BOOST_MESSAGE( "----- time_size_policy_get_max_file_size_test end -----" );
}

void time_size_policy_set_max_file_size_test()
{
	BOOST_MESSAGE( "----- time_size_policy_set_max_file_size_test start -----" );
	TestTimeSizePolicy p;

	//＊maxFileSizeを設定すること
	p.setMaxFileSize(564565);
	BOOST_CHECK_EQUAL(p.getMaxFileSize(), 564565U);
	
  
	BOOST_MESSAGE( "----- time_size_policy_set_max_file_size_test end -----" );
}

void time_size_policy_set_option_test()
{
	BOOST_MESSAGE( "----- time_size_policy_set_option_test start -----" );
	TestTimeSizePolicy p;

	//＊rotationTimingを設定すること
	p.setRotationTimingValue("");
	p.setOption("rotationtimingvalue", "05050101");
	BOOST_CHECK_EQUAL(p.getRotationTimingValue(), "05050101");
	
	//＊rotationTimingValueを設定すること
	p.setRotationTiming(LOG_TIM_YEAR);
	p.setOption("rotationtiming", "1");
	BOOST_CHECK_EQUAL(p.getRotationTiming(), LOG_TIM_MONTH);

	//＊FixedWindowRollingPolicyのsetOptionを呼んでいること	
	p.setMaxIndex(0);
	fixedPolicySetOptionCalled = 0;
	fixedPolicySetOptionArgOption = "";
	fixedPolicySetOptionArgValue = "";
	p.setOption("maxindex", "123");
	BOOST_CHECK_EQUAL(fixedPolicySetOptionCalled, 1);
	BOOST_CHECK_EQUAL(fixedPolicySetOptionArgOption, "maxindex");
	BOOST_CHECK_EQUAL(fixedPolicySetOptionArgValue, "123");

	//＊maxFileSizeを設定すること
	p.setMaxFileSize(0);
	p.setOption("maxfilesize", "55555");
	BOOST_CHECK_EQUAL(p.getMaxFileSize(), 55555U);

  	BOOST_MESSAGE( "----- time_size_policy_set_option_index_test end -----" );
}

void time_size_policy_initialize_test()
{
	BOOST_MESSAGE( "----- time_size_policy_initialize_test start -----" );
	TestTimeSizePolicy p;
	log4cxx::helpers::Pool pool;

	log4cxx::rolling::RolloverDescriptionPtr ret;

#define CURRENT_ACTIVE_FILE "test_current_active_file"

	//＊timeを呼び出していること
	timeCalled = 0;
	p.setRotationTiming(LOG_TIM_YEAR);
	p.setRotationTimingValue("02111615");
	ret = p.initialize(CURRENT_ACTIVE_FILE, true, pool);
	BOOST_CHECK_EQUAL(timeCalled, 1);
	BOOST_CHECK_EQUAL(ret, (log4cxx::rolling::RolloverDescriptionPtr)0);

	//＊timeが失敗した時に空のRolloverDescriptionが帰ってくること
	timeCalled = 0;
	timeReturnNow = false;
	timeReturns = -1;
	p.setRotationTiming(LOG_TIM_YEAR);
	p.setRotationTimingValue("02111615");
	ret = p.initialize(CURRENT_ACTIVE_FILE, true, pool);
	BOOST_CHECK_EQUAL(timeCalled, 1);
	BOOST_CHECK_EQUAL(ret, (log4cxx::rolling::RolloverDescriptionPtr)0);
	timeReturnNow = true;
	timeReturns = 0;	

	//＊getNextCheckを呼び出していること(nextCheckが変更されていること)
	struct tm test_time;
	memset(&test_time, 0, sizeof(struct tm));
	test_time.tm_year = 108;
	test_time.tm_mon = 1;
	test_time.tm_mday = 12;
	test_time.tm_hour = 16;
	test_time.tm_min = 4;
	time_t test_now = mktime(&test_time);
	test_time.tm_year = 109;
	test_time.tm_mon = 0;
	test_time.tm_mday = 15;
	test_time.tm_hour = 12;
	test_time.tm_min = 0;
	time_t test_next = mktime(&test_time);
	timeReturnNow = false;
	timeArg = test_now;
	p.setRotationTiming(LOG_TIM_YEAR);
	p.setRotationTimingValue("01151200");
	p.setNextCheckProperty(0);
	ret = p.initialize(CURRENT_ACTIVE_FILE, true, pool);
	BOOST_CHECK_EQUAL(ret, (log4cxx::rolling::RolloverDescriptionPtr)0);
	BOOST_CHECK_EQUAL(p.getNextCheckProperty(), test_next);
	timeReturnNow = true;
	timeArg = 0;

	//＊getNextCheckが失敗した時に空のRolloverDescriptionが帰ってくること
	p.setRotationTiming(LOG_TIM_YEAR);
	p.setRotationTimingValue("AAAAAA");
	ret = p.initialize(CURRENT_ACTIVE_FILE, true, pool);
	BOOST_CHECK_EQUAL(ret, (log4cxx::rolling::RolloverDescriptionPtr)0);

	//＊FixedWindowRollingPolicyのinitializeを呼んでいること
	fixedPolicyInitializeCalled = 0;
	fixedPolicyInitializeArgFile = "";
	fixedPolicyInitializeArgAppend = false;
	p.setRotationTiming(LOG_TIM_YEAR);
	p.setRotationTimingValue("02111615");
	ret = p.initialize(CURRENT_ACTIVE_FILE, true, pool);
	BOOST_CHECK_EQUAL(fixedPolicyInitializeCalled, 1);
	BOOST_CHECK_EQUAL(ret, (log4cxx::rolling::RolloverDescriptionPtr)0);
	BOOST_CHECK_EQUAL(fixedPolicyInitializeArgFile, CURRENT_ACTIVE_FILE);
	BOOST_CHECK_EQUAL(fixedPolicyInitializeArgAppend, true);

#undef CURRENT_ACTIVE_FILE
  
	BOOST_MESSAGE( "----- time_size_policy_initialize_test end -----" );
}

void time_size_policy_get_next_check_test()
{
	BOOST_MESSAGE( "----- time_size_policy_get_next_check_test start -----" );
	TestTimeSizePolicy p;

	time_t nc_ret;
	struct tm nc_time;
	struct tm test_time;
	memset(&test_time, 0, sizeof(struct tm));
	time_t test_now = 0;

	//＊localtime_rを呼び出していること
	test_time.tm_year = 108;
	test_time.tm_mon = 1;
	test_time.tm_mday = 12;
	test_time.tm_hour = 16;
	test_time.tm_min = 4;
	test_now = mktime(&test_time);
	p.setRotationTiming(LOG_TIM_YEAR);
	p.setRotationTimingValue("01151200");
	localtime_rCalled = 0;
	nc_ret = p.getNextCheck(test_now);
	BOOST_CHECK(!(nc_ret == -1));
	BOOST_CHECK_EQUAL(localtime_rCalled, 1);
	
	//＊localtime_rが失敗した時に-1を返すこと
	test_time.tm_year = 108;
	test_time.tm_mon = 1;
	test_time.tm_mday = 12;
	test_time.tm_hour = 16;
	test_time.tm_min = 4;
	test_now = mktime(&test_time);
	p.setRotationTiming(LOG_TIM_YEAR);
	p.setRotationTimingValue("01151200");
	localtime_rFail = true;
	nc_ret = p.getNextCheck(test_now);
	BOOST_CHECK(nc_ret == -1);
	localtime_rFail = false;
	
	//＊strftimeをLOG_DATE_FORMATを引数に呼び出していること
	test_time.tm_year = 108;
	test_time.tm_mon = 1;
	test_time.tm_mday = 12;
	test_time.tm_hour = 16;
	test_time.tm_min = 4;
	test_now = mktime(&test_time);
	p.setRotationTiming(LOG_TIM_YEAR);
	p.setRotationTimingValue("01151200");
	strftimeCalled = 0;
	strftimeArgFormat = "";
	nc_ret = p.getNextCheck(test_now);
	BOOST_CHECK(!(nc_ret == -1));
	BOOST_CHECK_EQUAL(strftimeCalled, 1);
	BOOST_CHECK_EQUAL(strftimeArgFormat.c_str(), LOG_DATE_FORMAT);

	//＊strftimeが失敗したときに-1を返すこと
	test_time.tm_year = 108;
	test_time.tm_mon = 1;
	test_time.tm_mday = 12;
	test_time.tm_hour = 16;
	test_time.tm_min = 4;
	test_now = mktime(&test_time);
	p.setRotationTiming(LOG_TIM_YEAR);
	p.setRotationTimingValue("01151200");
	strftimeFail = true;
	nc_ret = p.getNextCheck(test_now);
	BOOST_CHECK(nc_ret == -1);
	strftimeFail = false;

	//＊rotationTimingValueに不正な値が入っていた場合、-1を返すこと 年
	test_time.tm_year = 108;
	test_time.tm_mon = 1;
	test_time.tm_mday = 12;
	test_time.tm_hour = 16;
	test_time.tm_min = 4;
	test_now = mktime(&test_time);
	p.setRotationTiming(LOG_TIM_YEAR);
	p.setRotationTimingValue("AAAAAA");
	nc_ret = p.getNextCheck(test_now);
	BOOST_CHECK(nc_ret == -1);

	//＊mktimeを呼び出している事　年
	test_time.tm_year = 108;
	test_time.tm_mon = 1;
	test_time.tm_mday = 12;
	test_time.tm_hour = 16;
	test_time.tm_min = 4;
	test_now = mktime(&test_time);
	p.setRotationTiming(LOG_TIM_YEAR);
	p.setRotationTimingValue("01151200");
	mktimeCalled = 0;
	nc_ret = p.getNextCheck(test_now);
	BOOST_CHECK(!(nc_ret == -1));
	BOOST_CHECK_EQUAL(mktimeCalled, 1);

	//＊mktimeが失敗した時 -1を返すこと　年
	test_time.tm_year = 108;
	test_time.tm_mon = 1;
	test_time.tm_mday = 12;
	test_time.tm_hour = 16;
	test_time.tm_min = 4;
	test_now = mktime(&test_time);
	p.setRotationTiming(LOG_TIM_YEAR);
	p.setRotationTimingValue("01151200");
	mktimeFail = true;
	nc_ret = p.getNextCheck(test_now);
	BOOST_CHECK(nc_ret == -1);
	mktimeFail = false;	

	//＊nextCheckを取得すること　年　設定日時が未来の場合
	test_time.tm_year = 108;
	test_time.tm_mon = 1;
	test_time.tm_mday = 12;
	test_time.tm_hour = 16;
	test_time.tm_min = 4;
	test_now = mktime(&test_time);		//2008/02/12 16:04
	p.setRotationTiming(LOG_TIM_YEAR);
	p.setRotationTimingValue("03100945");	//03/10 09:45
	nc_ret = p.getNextCheck(test_now);
	BOOST_CHECK(!(nc_ret == -1));
	localtime_r(&nc_ret, &nc_time);		//2008/03/10 09:45
	BOOST_CHECK_EQUAL(nc_time.tm_year, 108);
	BOOST_CHECK_EQUAL(nc_time.tm_mon, 2);
	BOOST_CHECK_EQUAL(nc_time.tm_mday, 10);
	BOOST_CHECK_EQUAL(nc_time.tm_hour, 9);
	BOOST_CHECK_EQUAL(nc_time.tm_min, 45);

	//＊nextCheckを取得すること　年　設定日時が現在の場合(来年に設定)
	test_time.tm_year = 108;
	test_time.tm_mon = 1;
	test_time.tm_mday = 12;
	test_time.tm_hour = 16;
	test_time.tm_min = 4;
	test_now = mktime(&test_time);		//2008/02/12 16:04
	p.setRotationTiming(LOG_TIM_YEAR);
	p.setRotationTimingValue("02121604");	//02/12 16:04
	nc_ret = p.getNextCheck(test_now);
	BOOST_CHECK(!(nc_ret == -1));
	localtime_r(&nc_ret, &nc_time);		//2009/02/12 16:04
	BOOST_CHECK_EQUAL(nc_time.tm_year, 109);
	BOOST_CHECK_EQUAL(nc_time.tm_mon, 1);
	BOOST_CHECK_EQUAL(nc_time.tm_mday, 12);
	BOOST_CHECK_EQUAL(nc_time.tm_hour, 16);
	BOOST_CHECK_EQUAL(nc_time.tm_min, 4);

	//＊nextCheckを取得すること　年　設定日時が過去の場合(来年に設定)
	test_time.tm_year = 108;
	test_time.tm_mon = 1;
	test_time.tm_mday = 12;
	test_time.tm_hour = 16;
	test_time.tm_min = 4;
	test_now = mktime(&test_time);		//2008/02/12 16:04
	p.setRotationTiming(LOG_TIM_YEAR);
	p.setRotationTimingValue("01151500");	//01/15 15:00
	nc_ret = p.getNextCheck(test_now);
	BOOST_CHECK(!(nc_ret == -1));
	localtime_r(&nc_ret, &nc_time);		//2009/01/15 15:00
	BOOST_CHECK_EQUAL(nc_time.tm_year, 109);
	BOOST_CHECK_EQUAL(nc_time.tm_mon, 0);
	BOOST_CHECK_EQUAL(nc_time.tm_mday, 15);
	BOOST_CHECK_EQUAL(nc_time.tm_hour, 15);
	BOOST_CHECK_EQUAL(nc_time.tm_min, 0);



	//＊rotationTimingValueに不正な値が入っていた場合、-1を返すこと 月
	test_time.tm_year = 108;
	test_time.tm_mon = 1;
	test_time.tm_mday = 12;
	test_time.tm_hour = 16;
	test_time.tm_min = 4;
	test_now = mktime(&test_time);
	p.setRotationTiming(LOG_TIM_MONTH);
	p.setRotationTimingValue("ZZZZZ");
	nc_ret = p.getNextCheck(test_now);
	BOOST_CHECK(nc_ret == -1);

	//＊mktimeを呼び出している事　月
	test_time.tm_year = 108;
	test_time.tm_mon = 1;
	test_time.tm_mday = 12;
	test_time.tm_hour = 16;
	test_time.tm_min = 4;
	test_now = mktime(&test_time);
	p.setRotationTiming(LOG_TIM_MONTH);
	p.setRotationTimingValue("151200");
	mktimeCalled = 0;
	nc_ret = p.getNextCheck(test_now);
	BOOST_CHECK(!(nc_ret == -1));
	BOOST_CHECK_EQUAL(mktimeCalled, 1);

	//＊mktimeが失敗した時 -1を返すこと　月
	test_time.tm_year = 108;
	test_time.tm_mon = 1;
	test_time.tm_mday = 12;
	test_time.tm_hour = 16;
	test_time.tm_min = 4;
	test_now = mktime(&test_time);
	p.setRotationTiming(LOG_TIM_MONTH);
	p.setRotationTimingValue("151200");
	mktimeFail = true;
	nc_ret = p.getNextCheck(test_now);
	BOOST_CHECK(nc_ret == -1);
	mktimeFail = false;	

	//＊nextCheckを取得すること　月　設定日時が未来の場合
	test_time.tm_year = 108;
	test_time.tm_mon = 11;
	test_time.tm_mday = 15;
	test_time.tm_hour = 1;
	test_time.tm_min = 5;
	test_now = mktime(&test_time);		//2008/12/15 01:05
	p.setRotationTiming(LOG_TIM_MONTH);
	p.setRotationTimingValue("200945");	//20 09:45
	nc_ret = p.getNextCheck(test_now);
	BOOST_CHECK(!(nc_ret == -1));
	localtime_r(&nc_ret, &nc_time);		//2008/12/20 09:45
	BOOST_CHECK_EQUAL(nc_time.tm_year, 108);
	BOOST_CHECK_EQUAL(nc_time.tm_mon, 11);
	BOOST_CHECK_EQUAL(nc_time.tm_mday, 20);
	BOOST_CHECK_EQUAL(nc_time.tm_hour, 9);
	BOOST_CHECK_EQUAL(nc_time.tm_min, 45);

	//＊設定日付が存在しない日の場合、存在する日に修正すること
	test_time.tm_year = 108;
	test_time.tm_mon = 1;
	test_time.tm_mday = 15;
	test_time.tm_hour = 14;
	test_time.tm_min = 30;
	test_now = mktime(&test_time);		//2008/02/15 14:30
	p.setRotationTiming(LOG_TIM_MONTH);
	p.setRotationTimingValue("310945");	//31 09:45
	nc_ret = p.getNextCheck(test_now);
	BOOST_CHECK(!(nc_ret == -1));
	localtime_r(&nc_ret, &nc_time);		//2008/02/28 09:45
	BOOST_CHECK_EQUAL(nc_time.tm_year, 108);
	BOOST_CHECK_EQUAL(nc_time.tm_mon, 1);
	BOOST_CHECK_EQUAL(nc_time.tm_mday, 28);
	BOOST_CHECK_EQUAL(nc_time.tm_hour, 9);
	BOOST_CHECK_EQUAL(nc_time.tm_min, 45);

	//＊nextCheckを取得すること　月　設定日時が現在の場合(来月に設定)
	test_time.tm_year = 108;
	test_time.tm_mon = 11;
	test_time.tm_mday = 15;
	test_time.tm_hour = 1;
	test_time.tm_min = 5;
	test_now = mktime(&test_time);		//2008/12/15 01:05
	p.setRotationTiming(LOG_TIM_MONTH);
	p.setRotationTimingValue("150105");	//15 01:05
	nc_ret = p.getNextCheck(test_now);
	BOOST_CHECK(!(nc_ret == -1));
	localtime_r(&nc_ret, &nc_time);		//2009/01/15 01:05
	BOOST_CHECK_EQUAL(nc_time.tm_year, 109);
	BOOST_CHECK_EQUAL(nc_time.tm_mon, 0);
	BOOST_CHECK_EQUAL(nc_time.tm_mday, 15);
	BOOST_CHECK_EQUAL(nc_time.tm_hour, 1);
	BOOST_CHECK_EQUAL(nc_time.tm_min, 5);

	//＊設定日付が存在しない日の場合、存在する日に修正すること
	test_time.tm_year = 108;
	test_time.tm_mon = 9;
	test_time.tm_mday = 31;
	test_time.tm_hour = 14;
	test_time.tm_min = 30;
	test_now = mktime(&test_time);		//2008/10/31 14:30
	p.setRotationTiming(LOG_TIM_MONTH);
	p.setRotationTimingValue("311430");	//31 14:30
	nc_ret = p.getNextCheck(test_now);
	BOOST_CHECK(!(nc_ret == -1));
	localtime_r(&nc_ret, &nc_time);		//2008/11/30 14:30
	BOOST_CHECK_EQUAL(nc_time.tm_year, 108);
	BOOST_CHECK_EQUAL(nc_time.tm_mon, 10);
	BOOST_CHECK_EQUAL(nc_time.tm_mday, 30);
	BOOST_CHECK_EQUAL(nc_time.tm_hour, 14);
	BOOST_CHECK_EQUAL(nc_time.tm_min, 30);

	//＊nextCheckを取得すること　月　設定日時が過去の場合(来月に設定)
	test_time.tm_year = 108;
	test_time.tm_mon = 11;
	test_time.tm_mday = 15;
	test_time.tm_hour = 1;
	test_time.tm_min = 5;
	test_now = mktime(&test_time);		//2008/12/15 01:05
	p.setRotationTiming(LOG_TIM_MONTH);
	p.setRotationTimingValue("101200");	//10 12:00
	nc_ret = p.getNextCheck(test_now);
	BOOST_CHECK(!(nc_ret == -1));
	localtime_r(&nc_ret, &nc_time);		//2009/01/15 01:05
	BOOST_CHECK_EQUAL(nc_time.tm_year, 109);
	BOOST_CHECK_EQUAL(nc_time.tm_mon, 0);
	BOOST_CHECK_EQUAL(nc_time.tm_mday, 10);
	BOOST_CHECK_EQUAL(nc_time.tm_hour, 12);
	BOOST_CHECK_EQUAL(nc_time.tm_min, 0);

	//＊設定日付が存在しない日の場合、存在する日に修正すること
	test_time.tm_year = 108;
	test_time.tm_mon = 2;
	test_time.tm_mday = 31;
	test_time.tm_hour = 14;
	test_time.tm_min = 0;
	test_now = mktime(&test_time);		//2008/03/31 14:00
	p.setRotationTiming(LOG_TIM_MONTH);
	p.setRotationTimingValue("311200");	//31 12:00
	nc_ret = p.getNextCheck(test_now);
	BOOST_CHECK(!(nc_ret == -1));
	localtime_r(&nc_ret, &nc_time);		//2008/04/30 12:00
	BOOST_CHECK_EQUAL(nc_time.tm_year, 108);
	BOOST_CHECK_EQUAL(nc_time.tm_mon, 3);
	BOOST_CHECK_EQUAL(nc_time.tm_mday, 30);
	BOOST_CHECK_EQUAL(nc_time.tm_hour, 12);
	BOOST_CHECK_EQUAL(nc_time.tm_min, 0);



	//＊rotationTimingValueに不正な値が入っていた場合、-1を返すこと 週
	test_time.tm_year = 108;
	test_time.tm_mon = 1;
	test_time.tm_mday = 12;
	test_time.tm_hour = 16;
	test_time.tm_min = 4;
	test_now = mktime(&test_time);
	p.setRotationTiming(LOG_TIM_WEEK);
	p.setRotationTimingValue("ZZZZZ");
	nc_ret = p.getNextCheck(test_now);
	BOOST_CHECK(nc_ret == -1);

	//＊mktimeを呼び出している事　週
	test_time.tm_year = 108;
	test_time.tm_mon = 1;
	test_time.tm_mday = 12;
	test_time.tm_hour = 16;
	test_time.tm_min = 4;
	test_now = mktime(&test_time);
	p.setRotationTiming(LOG_TIM_WEEK);
	p.setRotationTimingValue("51200");
	mktimeCalled = 0;
	nc_ret = p.getNextCheck(test_now);
	BOOST_CHECK(!(nc_ret == -1));
	BOOST_CHECK_EQUAL(mktimeCalled, 1);

	//＊mktimeが失敗した時 -1を返すこと　週
	test_time.tm_year = 108;
	test_time.tm_mon = 1;
	test_time.tm_mday = 12;
	test_time.tm_hour = 16;
	test_time.tm_min = 4;
	test_now = mktime(&test_time);
	p.setRotationTiming(LOG_TIM_WEEK);
	p.setRotationTimingValue("51200");
	mktimeFail = true;
	nc_ret = p.getNextCheck(test_now);
	BOOST_CHECK(nc_ret == -1);
	mktimeFail = false;	
	
	//＊nextCheckを取得すること　週　設定日時が未来の場合
	test_time.tm_year = 108;
	test_time.tm_mon = 1;
	test_time.tm_mday = 13;
	test_time.tm_hour = 1;
	test_time.tm_min = 0;
	test_now = mktime(&test_time);		//2008/02/13 01:00(水)
	p.setRotationTiming(LOG_TIM_WEEK);
	p.setRotationTimingValue("41200");	//(木) 12:00
	nc_ret = p.getNextCheck(test_now);
	BOOST_CHECK(!(nc_ret == -1));
	localtime_r(&nc_ret, &nc_time);		//2008/02/14 12:00(木)
	BOOST_CHECK_EQUAL(nc_time.tm_year, 108);
	BOOST_CHECK_EQUAL(nc_time.tm_mon, 1);
	BOOST_CHECK_EQUAL(nc_time.tm_mday, 14);
	BOOST_CHECK_EQUAL(nc_time.tm_hour, 12);
	BOOST_CHECK_EQUAL(nc_time.tm_min, 0);

	//＊nextCheckを取得すること　週　設定日時が現在の場合(翌週に設定)
	test_time.tm_year = 108;
	test_time.tm_mon = 1;
	test_time.tm_mday = 13;
	test_time.tm_hour = 1;
	test_time.tm_min = 0;
	test_now = mktime(&test_time);		//2008/02/13 01:00(水)
	p.setRotationTiming(LOG_TIM_WEEK);
	p.setRotationTimingValue("30100");	//(水) 1:00
	nc_ret = p.getNextCheck(test_now);
	BOOST_CHECK(!(nc_ret == -1));
	localtime_r(&nc_ret, &nc_time);		//2008/02/20 01:00(水)
	BOOST_CHECK_EQUAL(nc_time.tm_year, 108);
	BOOST_CHECK_EQUAL(nc_time.tm_mon, 1);
	BOOST_CHECK_EQUAL(nc_time.tm_mday, 20);
	BOOST_CHECK_EQUAL(nc_time.tm_hour, 1);
	BOOST_CHECK_EQUAL(nc_time.tm_min, 0);

	//＊nextCheckを取得すること　月　設定日時が過去の場合(翌週に設定)
	test_time.tm_year = 108;
	test_time.tm_mon = 1;
	test_time.tm_mday = 13;
	test_time.tm_hour = 1;
	test_time.tm_min = 0;
	test_now = mktime(&test_time);		//2008/02/13 01:00(水)
	p.setRotationTiming(LOG_TIM_WEEK);
	p.setRotationTimingValue("20500");	//(火) 5:00
	nc_ret = p.getNextCheck(test_now);
	BOOST_CHECK(!(nc_ret == -1));
	localtime_r(&nc_ret, &nc_time);		//2008/02/19 01:00(火)
	BOOST_CHECK_EQUAL(nc_time.tm_year, 108);
	BOOST_CHECK_EQUAL(nc_time.tm_mon, 1);
	BOOST_CHECK_EQUAL(nc_time.tm_mday, 19);
	BOOST_CHECK_EQUAL(nc_time.tm_hour, 5);
	BOOST_CHECK_EQUAL(nc_time.tm_min, 0);
	

	//＊rotationTimingValueに不正な値が入っていた場合、-1を返すこと 日
	test_time.tm_year = 108;
	test_time.tm_mon = 1;
	test_time.tm_mday = 12;
	test_time.tm_hour = 16;
	test_time.tm_min = 4;
	test_now = mktime(&test_time);
	p.setRotationTiming(LOG_TIM_DATE);
	p.setRotationTimingValue("NNNN");
	nc_ret = p.getNextCheck(test_now);
	BOOST_CHECK(nc_ret == -1);

	//＊mktimeを呼び出している事　日
	test_time.tm_year = 108;
	test_time.tm_mon = 1;
	test_time.tm_mday = 12;
	test_time.tm_hour = 16;
	test_time.tm_min = 4;
	test_now = mktime(&test_time);
	p.setRotationTiming(LOG_TIM_DATE);
	p.setRotationTimingValue("1200");
	mktimeCalled = 0;
	nc_ret = p.getNextCheck(test_now);
	BOOST_CHECK(!(nc_ret == -1));
	BOOST_CHECK_EQUAL(mktimeCalled, 1);

	//＊mktimeが失敗した時 -1を返すこと　日
	test_time.tm_year = 108;
	test_time.tm_mon = 1;
	test_time.tm_mday = 12;
	test_time.tm_hour = 16;
	test_time.tm_min = 4;
	test_now = mktime(&test_time);
	p.setRotationTiming(LOG_TIM_DATE);
	p.setRotationTimingValue("1200");
	mktimeFail = true;
	nc_ret = p.getNextCheck(test_now);
	BOOST_CHECK(nc_ret == -1);
	mktimeFail = false;

	//＊nextCheckを取得すること　日　設定日時が未来の場合
	test_time.tm_year = 108;
	test_time.tm_mon = 1;
	test_time.tm_mday = 13;
	test_time.tm_hour = 1;
	test_time.tm_min = 0;
	test_now = mktime(&test_time);		//2008/02/13 01:00
	p.setRotationTiming(LOG_TIM_DATE);
	p.setRotationTimingValue("2359");	//23:59
	nc_ret = p.getNextCheck(test_now);
	BOOST_CHECK(!(nc_ret == -1));
	localtime_r(&nc_ret, &nc_time);		//2008/02/13 23:59
	BOOST_CHECK_EQUAL(nc_time.tm_year, 108);
	BOOST_CHECK_EQUAL(nc_time.tm_mon, 1);
	BOOST_CHECK_EQUAL(nc_time.tm_mday, 13);
	BOOST_CHECK_EQUAL(nc_time.tm_hour, 23);
	BOOST_CHECK_EQUAL(nc_time.tm_min, 59);
	
	//＊nextCheckを取得すること　日　設定日時が現在の場合(翌日に設定)
	test_time.tm_year = 108;
	test_time.tm_mon = 1;
	test_time.tm_mday = 13;
	test_time.tm_hour = 1;
	test_time.tm_min = 0;
	test_now = mktime(&test_time);		//2008/02/13 01:00
	p.setRotationTiming(LOG_TIM_DATE);
	p.setRotationTimingValue("0100");	//01:00
	nc_ret = p.getNextCheck(test_now);
	BOOST_CHECK(!(nc_ret == -1));
	localtime_r(&nc_ret, &nc_time);		//2008/02/14 01:00
	BOOST_CHECK_EQUAL(nc_time.tm_year, 108);
	BOOST_CHECK_EQUAL(nc_time.tm_mon, 1);
	BOOST_CHECK_EQUAL(nc_time.tm_mday, 14);
	BOOST_CHECK_EQUAL(nc_time.tm_hour, 1);
	BOOST_CHECK_EQUAL(nc_time.tm_min, 0);

	//＊nextCheckを取得すること　日　設定日時が過去の場合(翌日に設定)
	test_time.tm_year = 108;
	test_time.tm_mon = 1;
	test_time.tm_mday = 13;
	test_time.tm_hour = 1;
	test_time.tm_min = 0;
	test_now = mktime(&test_time);		//2008/02/13 01:00
	p.setRotationTiming(LOG_TIM_DATE);
	p.setRotationTimingValue("0000");	//00:00
	nc_ret = p.getNextCheck(test_now);
	BOOST_CHECK(!(nc_ret == -1));
	localtime_r(&nc_ret, &nc_time);		//2008/02/14 00:00
	BOOST_CHECK_EQUAL(nc_time.tm_year, 108);
	BOOST_CHECK_EQUAL(nc_time.tm_mon, 1);
	BOOST_CHECK_EQUAL(nc_time.tm_mday, 14);
	BOOST_CHECK_EQUAL(nc_time.tm_hour, 0);
	BOOST_CHECK_EQUAL(nc_time.tm_min, 0);


	
	//＊rotationTimingValueに不正な値が入っていた場合、-1を返すこと 日
	test_time.tm_year = 108;
	test_time.tm_mon = 1;
	test_time.tm_mday = 12;
	test_time.tm_hour = 16;
	test_time.tm_min = 4;
	test_now = mktime(&test_time);
	p.setRotationTiming(LOG_TIM_HOUR);
	p.setRotationTimingValue("SS");
	nc_ret = p.getNextCheck(test_now);
	BOOST_CHECK(nc_ret == -1);

	//＊mktimeを呼び出している事　日
	test_time.tm_year = 108;
	test_time.tm_mon = 1;
	test_time.tm_mday = 12;
	test_time.tm_hour = 16;
	test_time.tm_min = 4;
	test_now = mktime(&test_time);
	p.setRotationTiming(LOG_TIM_HOUR);
	p.setRotationTimingValue("15");
	mktimeCalled = 0;
	nc_ret = p.getNextCheck(test_now);
	BOOST_CHECK(!(nc_ret == -1));
	BOOST_CHECK_EQUAL(mktimeCalled, 1);

	//＊mktimeが失敗した時 -1を返すこと　日
	test_time.tm_year = 108;
	test_time.tm_mon = 1;
	test_time.tm_mday = 12;
	test_time.tm_hour = 16;
	test_time.tm_min = 4;
	test_now = mktime(&test_time);
	p.setRotationTiming(LOG_TIM_HOUR);
	p.setRotationTimingValue("15");
	mktimeFail = true;
	nc_ret = p.getNextCheck(test_now);
	BOOST_CHECK(nc_ret == -1);
	mktimeFail = false;

	//＊nextCheckを取得すること　時間　設定日時が未来の場合
	test_time.tm_year = 108;
	test_time.tm_mon = 1;
	test_time.tm_mday = 13;
	test_time.tm_hour = 1;
	test_time.tm_min = 15;
	test_now = mktime(&test_time);		//2008/02/13 01:15
	p.setRotationTiming(LOG_TIM_HOUR);
	p.setRotationTimingValue("30");		//30
	nc_ret = p.getNextCheck(test_now);
	BOOST_CHECK(!(nc_ret == -1));
	localtime_r(&nc_ret, &nc_time);		//2008/02/13 01:30
	BOOST_CHECK_EQUAL(nc_time.tm_year, 108);
	BOOST_CHECK_EQUAL(nc_time.tm_mon, 1);
	BOOST_CHECK_EQUAL(nc_time.tm_mday, 13);
	BOOST_CHECK_EQUAL(nc_time.tm_hour, 1);
	BOOST_CHECK_EQUAL(nc_time.tm_min, 30);

	//＊nextCheckを取得すること　時間　設定日時が現在の場合(次の時間に設定)
	test_time.tm_year = 108;
	test_time.tm_mon = 1;
	test_time.tm_mday = 13;
	test_time.tm_hour = 1;
	test_time.tm_min = 15;
	test_now = mktime(&test_time);		//2008/02/13 01:15
	p.setRotationTiming(LOG_TIM_HOUR);
	p.setRotationTimingValue("15");		//15
	nc_ret = p.getNextCheck(test_now);
	BOOST_CHECK(!(nc_ret == -1));
	localtime_r(&nc_ret, &nc_time);		//2008/02/13 02:15
	BOOST_CHECK_EQUAL(nc_time.tm_year, 108);
	BOOST_CHECK_EQUAL(nc_time.tm_mon, 1);
	BOOST_CHECK_EQUAL(nc_time.tm_mday, 13);
	BOOST_CHECK_EQUAL(nc_time.tm_hour, 2);
	BOOST_CHECK_EQUAL(nc_time.tm_min, 15);

	//＊nextCheckを取得すること　時間　設定日時が過去の場合(次の時間に設定)
	test_time.tm_year = 108;
	test_time.tm_mon = 3;
	test_time.tm_mday = 30;
	test_time.tm_hour = 23;
	test_time.tm_min = 45;
	test_now = mktime(&test_time);		//2008/04/30 23:45
	p.setRotationTiming(LOG_TIM_HOUR);
	p.setRotationTimingValue("30");		//30
	nc_ret = p.getNextCheck(test_now);
	BOOST_CHECK(!(nc_ret == -1));
	localtime_r(&nc_ret, &nc_time);		//2008/05/01 00:30
	BOOST_CHECK_EQUAL(nc_time.tm_year, 108);
	BOOST_CHECK_EQUAL(nc_time.tm_mon, 4);
	BOOST_CHECK_EQUAL(nc_time.tm_mday, 1);
	BOOST_CHECK_EQUAL(nc_time.tm_hour, 0);
	BOOST_CHECK_EQUAL(nc_time.tm_min, 30);
	
  
	BOOST_MESSAGE( "----- time_size_policy_set_next_check_test end -----" );
}

void time_size_policy_rollover_test()
{
	BOOST_MESSAGE( "----- time_size_policy_rollover_test start -----" );
	TestTimeSizePolicy p;
	log4cxx::helpers::Pool pool;
	log4cxx::rolling::RolloverDescriptionPtr ret;

#define TEST_FILE_NAME "test_file_name"
#define CURRENT_ACTIVE_FILE "current_active_file"
	//＊timeを呼び出していること
	timeCalled = 0;
	formatFileNameArgBuf = TEST_FILE_NAME;
	p.setRotationTiming(LOG_TIM_YEAR);
	p.setRotationTimingValue("02111615");
	ret = p.rollover(CURRENT_ACTIVE_FILE, pool);
	BOOST_CHECK_EQUAL(timeCalled, 1);
	BOOST_CHECK_EQUAL(ret, (log4cxx::rolling::RolloverDescriptionPtr)0);

	//＊timeが失敗した時に空のRolloverDescriptionが帰ってくること
	timeCalled = 0;
	timeReturnNow = false;
	timeReturns = -1;
	formatFileNameArgBuf = TEST_FILE_NAME;
	p.setRotationTiming(LOG_TIM_YEAR);
	p.setRotationTimingValue("02111615");
	ret = p.rollover(CURRENT_ACTIVE_FILE, pool);
	BOOST_CHECK_EQUAL(timeCalled, 1);
	BOOST_CHECK_EQUAL(ret, (log4cxx::rolling::RolloverDescriptionPtr)0);
	timeReturnNow = true;
	timeReturns = 0;

	//＊getNextCheckを呼び出していること(nextCheckが変更されていること)
	struct tm test_time;
	memset(&test_time, 0, sizeof(struct tm));
	test_time.tm_year = 108;
	test_time.tm_mon = 1;
	test_time.tm_mday = 12;
	test_time.tm_hour = 16;
	test_time.tm_min = 4;
	time_t test_now = mktime(&test_time);
	test_time.tm_year = 109;
	test_time.tm_mon = 0;
	test_time.tm_mday = 15;
	test_time.tm_hour = 12;
	test_time.tm_min = 0;
	time_t test_next = mktime(&test_time);
	timeReturnNow = false;
	timeArg = test_now;
	p.setRotationTiming(LOG_TIM_YEAR);
	p.setRotationTimingValue("01151200");
	p.setNextCheckProperty(0);
	ret = p.rollover(CURRENT_ACTIVE_FILE, pool);
	BOOST_CHECK_EQUAL(ret, (log4cxx::rolling::RolloverDescriptionPtr)0);
	BOOST_CHECK_EQUAL(p.getNextCheckProperty(), test_next);
	timeReturnNow = true;
	timeArg = 0;

	//＊getNextCheckが失敗した時に空のRolloverDescriptionが帰ってくること
	p.setRotationTiming(LOG_TIM_YEAR);
	p.setRotationTimingValue("AAAAAA");
	ret = p.rollover(CURRENT_ACTIVE_FILE, pool);
	BOOST_CHECK_EQUAL(ret, (log4cxx::rolling::RolloverDescriptionPtr)0);
	
	//＊FixedWindowRollingPolicyのrolloverを呼んでいること
	fixedPolicyRolloverCalled = 0;
	fixedPolicyRolloverArgActiveFile = "";
	p.setRotationTiming(LOG_TIM_YEAR);
	p.setRotationTimingValue("02111615");
	ret = p.rollover(CURRENT_ACTIVE_FILE, pool);
	BOOST_CHECK_EQUAL(fixedPolicyRolloverCalled, 1);
	BOOST_CHECK_EQUAL(fixedPolicyRolloverArgActiveFile, CURRENT_ACTIVE_FILE);
	BOOST_CHECK_EQUAL(ret, (log4cxx::rolling::RolloverDescriptionPtr)0);
#undef CURRENT_ACTIVE_FILE
#undef TEST_FILE_NAME
  
	BOOST_MESSAGE( "----- time_size_policy_rollover_test end -----" );
}

void time_size_policy_is_triggering_event_test()
{
	BOOST_MESSAGE( "----- time_size_policy_is_triggering_event_test start -----" );
	TestTimeSizePolicy p;

	bool ret;
	log4cxx::AppenderPtr argAppender;
	log4cxx::spi::LoggingEventPtr argEvent;
	LogString filename;
	size_t filelength = 50000;

	//filelengthがmaxFileSizeに満たない場合

	//＊timeを呼び出していること
	timeCalled = 0;
	timeReturnNow = false;
	timeArg = 10;
	p.setNextCheckProperty(5);
	p.setMaxFileSize(100000);
	ret = p.isTriggeringEvent(argAppender, argEvent, filename, filelength);
	BOOST_CHECK_EQUAL(timeCalled, 1);
	BOOST_CHECK_EQUAL(ret, true);
	timeArg = 0;
	timeReturnNow = true;

	//＊timeが失敗した時に空のRolloverDescriptionが帰ってくること
	timeCalled = 0;
	timeReturnNow = false;
	timeReturns = -1;
	p.setNextCheckProperty(5);
	p.setMaxFileSize(100000);
	ret = p.isTriggeringEvent(argAppender, argEvent, filename, filelength);
	BOOST_CHECK_EQUAL(timeCalled, 1);
	BOOST_CHECK_EQUAL(ret, false);
	timeReturnNow = true;
	timeReturns = 0;

	//＊timeで取得した時間とnextCheckを比較してtimeで取得した時間が大きい時にtrueを返却すること
	timeCalled = 0;
	timeReturnNow = false;
	timeArg = 10;
	p.setNextCheckProperty(5);
	p.setMaxFileSize(100000);
	ret = p.isTriggeringEvent(argAppender, argEvent, filename, filelength);
	BOOST_CHECK_EQUAL(timeCalled, 1);
	BOOST_CHECK_EQUAL(ret, true);
	timeArg = 0;
	timeReturnNow = true;

	//＊nextCheckが大きい時にfalseを返すこと
	timeCalled = 0;
	timeReturnNow = false;
	timeArg = 5;
	p.setNextCheckProperty(10);
	p.setMaxFileSize(100000);
	ret = p.isTriggeringEvent(argAppender, argEvent, filename, filelength);
	BOOST_CHECK_EQUAL(timeCalled, 1);
	BOOST_CHECK_EQUAL(ret, false);
	timeArg = 0;
	timeReturnNow = true;


	//filelengthがmaxFileSizeに以上の場合
	filelength = 200000;

	//＊timeを呼び出していないこと
	timeCalled = 0;
	timeReturnNow = false;
	timeArg = 10;
	p.setNextCheckProperty(5);
	p.setMaxFileSize(100000);
	ret = p.isTriggeringEvent(argAppender, argEvent, filename, filelength);
	BOOST_CHECK_EQUAL(timeCalled, 0);
	BOOST_CHECK_EQUAL(ret, true);
	timeArg = 0;
	timeReturnNow = true;

	//＊timeで取得した時間とnextCheckを比較してtimeで取得した時間が大きい時かつ、ファイルサイズも超えている時ににtrueを返却すること
	timeCalled = 0;
	timeReturnNow = false;
	timeArg = 10;
	p.setNextCheckProperty(5);
	p.setMaxFileSize(100000);
	ret = p.isTriggeringEvent(argAppender, argEvent, filename, filelength);
	BOOST_CHECK_EQUAL(timeCalled, 0);
	BOOST_CHECK_EQUAL(ret, true);
	timeArg = 0;
	timeReturnNow = true;

	//＊nextCheckが大きい時でもファイルサイズが超えていたらtrueを返すこと
	timeCalled = 0;
	timeReturnNow = false;
	timeArg = 5;
	p.setNextCheckProperty(10);
	p.setMaxFileSize(100000);
	ret = p.isTriggeringEvent(argAppender, argEvent, filename, filelength);
	BOOST_CHECK_EQUAL(timeCalled, 0);
	BOOST_CHECK_EQUAL(ret, true);
	timeArg = 0;
	timeReturnNow = true;



	BOOST_MESSAGE( "----- time_size_policy_is_triggering_event_test end -----" );
}

test_suite*
init_unit_test_suite(int argc, char* argv[])
{
	test_suite* test = BOOST_TEST_SUITE("logger_test");

	test->add(BOOST_TEST_CASE(time_size_policy_constructor_test));
	test->add(BOOST_TEST_CASE(time_size_policy_addref_test));
	test->add(BOOST_TEST_CASE(time_size_policy_releaseref_test));
	test->add(BOOST_TEST_CASE(time_size_policy_activateoptions_test));
	test->add(BOOST_TEST_CASE(time_size_policy_get_rotation_timing_value_test));
	test->add(BOOST_TEST_CASE(time_size_policy_set_rotation_timing_value_test));
	test->add(BOOST_TEST_CASE(time_size_policy_get_rotation_timing_test));
	test->add(BOOST_TEST_CASE(time_size_policy_set_rotation_timing_test));
	test->add(BOOST_TEST_CASE(time_size_policy_get_max_file_size_test));
	test->add(BOOST_TEST_CASE(time_size_policy_set_max_file_size_test));
	test->add(BOOST_TEST_CASE(time_size_policy_set_option_test));
	test->add(BOOST_TEST_CASE(time_size_policy_initialize_test));
	test->add(BOOST_TEST_CASE(time_size_policy_get_next_check_test));
	test->add(BOOST_TEST_CASE(time_size_policy_rollover_test));
	test->add(BOOST_TEST_CASE(time_size_policy_is_triggering_event_test));

	return test;
};

