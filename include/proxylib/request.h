#pragma once

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <string>
#include <proxylib/common.h>

namespace proxylib {
namespace asio {
namespace socks4a {

class request {
	public:
		enum command_type {
			connect	= 0x01,
			bind	= 0x02,
		};

		request() : vn(0), cd(0), dstport(0), dstip(0) {}
		request(
			command_type command,
			const boost::asio::ip::tcp::endpoint& endpoint,
			const std::string& userid,
			const std::string& hostname
		) : vn(::proxylib::asio::socks4a::version),
			cd(command),
			userid(userid),
			hostname(hostname)
		{
			// Accept only IPv4
			if (endpoint.protocol() != boost::asio::ip::tcp::v4()) {
				throw boost::system::system_error(
					boost::asio::error::address_family_not_supported);
			}
			dstport = endpoint.port();
			dstip = endpoint.address().to_v4().to_ulong();
		}
		~request() {}

		inline boost::asio::ip::address_v4 address() const {
			return boost::asio::ip::address_v4(dstip);
		}
		inline boost::asio::ip::tcp::endpoint endpoint() const {
			return boost::asio::ip::tcp::endpoint(address(), dstport);
		}
		inline boost::asio::ip::tcp::resolver::query query() {
			return boost::asio::ip::tcp::resolver::query(hostname, dstport);
		}
		boost::array<boost::asio::const_buffer, 8> buffer() const {
			boost::array<boost::asio::const_buffer, 8> bufs = {
				{
					boost::asio::buffer(&vn, 1),
					boost::asio::buffer(&cd, 1),
					boost::asio::buffer(&dstport, 2),
					boost::asio::buffer(&dstip, 4),
					boost::asio::buffer(userid),
					boost::asio::buffer(&null_byte, 1),
					boost::asio::buffer(hostname),
					boost::asio::buffer(&null_byte, 1)
				}
			};
			return bufs;
		}

	public:
		uint8 vn; // version
		uint8 cd; // command
		uint16 dstport;
		uint32 dstip;
		std::string userid;
		std::string hostname;
		byte null_byte;
};

} // namespace socks4a
} // namespace asio
} // namespace proxylib
