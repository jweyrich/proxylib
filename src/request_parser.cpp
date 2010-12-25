#include <proxylib/request_parser.h>
#include <proxylib/common.h>

namespace proxylib {
namespace asio {
namespace socks4a {

request_parser::request_parser() : _state(version) {
}

void request_parser::reset() {
	_state = version;
}

boost::tribool request_parser::consume(request& req, byte input) {
	switch (_state) {
		case version:
			if (input != ::proxylib::asio::socks4a::version) {
				return false;
			}
			_state = command;
			req.vn = input;
			LOG("vn = %02x", req.vn);
			return boost::indeterminate;
		case command:
			if (input != request::bind && input != request::connect) {
				return false;
			}
			_state = port_1;
			req.cd = input;
			LOG("cd = %02x", req.cd);
			return boost::indeterminate;
		case port_1:
			_state = port_2;
			req.dstport = (input << 8) & 0xff00;
			return boost::indeterminate;
		case port_2:
			_state = ip_1;
			req.dstport |= input & 0x00ff;
			LOG("port = %d", req.dstport);
			return boost::indeterminate;
		case ip_1:
			_state = ip_2;
			req.dstip = (input << 24) & 0xff000000;
			return boost::indeterminate;
		case ip_2:
			_state = ip_3;
			req.dstip |= (input << 16) & 0x00ff0000;
			return boost::indeterminate;
		case ip_3:
			_state = ip_4;
			req.dstip |= (input << 8) & 0x0000ff00;
			return boost::indeterminate;
		case ip_4:
			_state = userid;
			req.dstip |= input & 0x000000ff;
			LOG("dstip = %s", boost::asio::ip::address_v4(req.dstip).to_string().c_str());
			return boost::indeterminate;
		case userid:
			if (!is_valid_userid(input) || req.userid.length() >= 255) {
				return false;
			} else if (input == '\0') {
				LOG("userid = %s", req.userid.c_str());
				if (!IN_000X(req.dstip))
					return true;
				_state = hostname;
			} else {
				req.userid.push_back(input);
			}
			return boost::indeterminate;
		case hostname:
			if (!is_valid_hostname(input) || req.hostname.length() >= 255) {
				return false;
			} else if (input == '\0') {
				LOG("hostname = %s", req.hostname.c_str());
				return true;
			} else {
				req.hostname.push_back(input);
			}
			return boost::indeterminate;
		default:
			return false;
	}
}

bool request_parser::is_valid_userid(int c) {
	return std::isalnum(c) || c == '\0' || c == '.' || c == '-';
}

bool request_parser::is_valid_hostname(int c) {
	return std::isalnum(c) || c == '\0' || c == '.' || c == '-';
}

} } } // namespace proxylib::asio::socks4a
