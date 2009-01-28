#ifndef	ENDPOINT_H
#define	ENDPOINT_H

#include <boost/serialization/nvp.hpp>
#include <boost/serialization/split_free.hpp>

namespace boost {
namespace serialization {

template<class Archive, class T>
inline void serialize(
	Archive & ar,
	boost::asio::ip::basic_endpoint<T> & t,
	const unsigned int file_version 
){
	boost::serialization::split_free( ar, t, file_version );
}

template<class Archive, class T>
inline void save(
	Archive & ar,
	boost::asio::ip::basic_endpoint<T> const& t,
	const unsigned int /* file_version */
){
	std::string		addr	= t.address().to_string();
	unsigned short	port	= t.port();
	ar << boost::serialization::make_nvp( "addr", addr );
	ar << boost::serialization::make_nvp( "port", port );
}

template<class Archive, class T>
inline void load(
	Archive & ar,
	boost::asio::ip::basic_endpoint<T>& t,
	const unsigned int /* file_version */ 
){
	std::string		addr;
	unsigned short	port;
	ar >> boost::serialization::make_nvp( "addr", addr );
	ar >> boost::serialization::make_nvp( "port", port );
	t = boost::asio::ip::basic_endpoint<T>( boost::asio::ip::address::from_string( addr ), port );
}

}	// namespace serialization
}	// namespace boost

#endif	// ENDPOINT_H
