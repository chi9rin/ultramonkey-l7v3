#ifndef	TEST_DATA_H
#define	TEST_DATA_H

#include <boost/serialization/map.hpp>
#include "l7vs_command.h"

class	test_data{
public:
	class	test_method_data{
	public:
		test_method_data() : num_called(0) {}
		unsigned int num_called;
	private:
		friend class boost::serialization::access;
		template< class Archive > void serialize( Archive& ar, const unsigned int version ){
			ar & num_called;
		}
	};
	typedef	std::map< l7vs::l7vsadm_request::COMMAND_CODE_TAG, test_method_data >	test_method_data_type;

	test_method_data_type	method_map;
	test_data(){ method_map.clear(); }

private:
	friend class boost::serialization::access;
	template< class Archive > void serialize( Archive& ar, const unsigned int version ){
		ar & method_map;
	}
};

#endif	//TEST_DATA_H
