#pragma once

#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <proxylib/reply.h>
#include <proxylib/request.h>
#include <proxylib/request_parser.h>
#include <proxylib/connection_relay.h>

namespace proxylib {
namespace asio {
namespace socks4a {

class channel;

class handshake_handler : private boost::noncopyable {
	private:
		typedef boost::asio::ip::tcp::endpoint	endpoint_type;
		typedef boost::asio::ip::tcp::socket	socket_type;
		typedef boost::asio::ip::tcp::resolver	resolver_type;
		typedef boost::system::error_code		error_code;
		typedef boost::function<void (const error_code&, const size_t&)>			handler_type_1;
		typedef boost::function<void (const error_code&, resolver_type::iterator)>	handler_type_2;

	public:
		explicit handshake_handler(channel& channel);
		~handshake_handler();
		void start();
		void stop();
		socket_type& client_socket();

	private:
		void setup_handlers();
		void read_request();
		void handle_read_request(const error_code& error, const size_t& bytes_transferred);
		void handle_resolve(const error_code& error, resolver_type::iterator iter);
		void handle_connect(const error_code& error, resolver_type::iterator iter);
		void handle_write_reply(const error_code& error, const size_t& bytes_transferred);
		void handle_error(const error_code& error, int endpoint);
		void handle_disconnect(const error_code& error, int endpoint);

	private:
		channel&			_channel;
		resolver_type		_resolver;
		connection_relay	_relay;
		request				_request;
		request_parser		_request_parser;
		reply				_reply;
		handler_type_1		_read_request_cb;
		handler_type_2		_resolve_cb;
		handler_type_1		_write_reply_cb;
};

} // namespace socks4a
} // namespace asio
} // namespace proxylib
