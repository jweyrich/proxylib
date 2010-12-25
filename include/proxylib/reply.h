#pragma once

#include <boost/asio.hpp>
#include <proxylib/common.h>

namespace proxylib {
namespace asio {
namespace socks4a {

class reply {
public:
	enum status_type {
		request_granted				= 0x5a,
		request_failed				= 0x5b,
		request_failed_no_identd	= 0x5c,
		request_failed_bad_user_id	= 0x5d,
	};

	reply() : vn(0), cd(0), dstport(0), dstip(0) {}
	reply(
		status_type status,
		const boost::asio::ip::tcp::endpoint& endpoint
	) : vn(0),
		cd(status)
	{
		// Accept only IPv4
		if (endpoint.protocol() != boost::asio::ip::tcp::v4()) {
			throw boost::system::system_error(
				boost::asio::error::address_family_not_supported);
		}
		dstport = endpoint.port();
		dstip = endpoint.address().to_v4().to_ulong();
	}
	~reply() {}

	boost::asio::ip::address_v4 address() const {
		return boost::asio::ip::address_v4(dstip);
	}
	boost::asio::ip::tcp::endpoint endpoint() const {
		return boost::asio::ip::tcp::endpoint(address(), dstport);
	}
	boost::array<boost::asio::const_buffer, 4> buffer() const {
		boost::array<boost::asio::const_buffer, 4> bufs = {
			{
				boost::asio::buffer(&vn, 1),
				boost::asio::buffer(&cd, 1),
				boost::asio::buffer(&dstport, 2),
				boost::asio::buffer(&dstip, 4)
			}
		};
		return bufs;
	}
	bool success() const {
		return vn == 0 && cd == request_granted;
	}

public:
	uint8 vn; // version
	uint8 cd; // command
	uint16 dstport;
	uint32 dstip;
};

} } } // namespace proxylib::asio::socks4a
