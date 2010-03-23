#pragma once

#include <proxylib/request.h>
#include <boost/logic/tribool.hpp>
#include <boost/tuple/tuple.hpp>

namespace proxylib {
namespace asio {
namespace socks4a {

class request_parser {
	public:
		// Construct ready to parse the request method.
		request_parser();

		// Reset to initial parser state.
		void reset();

		// Parse some data. The tribool return value is true when a complete request
		// has been parsed, false if the data is invalid, indeterminate when more
		// data is required. The InputIterator return value indicates how much of the
		// input has been consumed.
		template <typename InputIterator>
		boost::tuple<boost::tribool, InputIterator> parse(request& req,
			InputIterator begin, InputIterator end)
		{
			while (begin != end) {
				boost::tribool result = consume(req, *begin++);
				if (result || !result)
					return boost::make_tuple(result, begin);
			}
			boost::tribool result = boost::indeterminate;
			return boost::make_tuple(result, begin);
		}

	private:
		// Handle the next byte of input.
		boost::tribool consume(request& req, byte input);

		// Check if a byte is part of a valid userid.
		static bool is_valid_userid(int c);

		// Check if a byte is part of a valid hostname.
		static bool is_valid_hostname(int c);

		// The current state of the parser.
		enum {
			version,
			command,
			port_1, port_2,
			ip_1, ip_2, ip_3, ip_4,
			userid,
			hostname,
		} _state;
};

} // namespace socks4a
} // namespace asio
} // namespace proxylib
