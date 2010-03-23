#pragma once

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <proxylib/connection.h>

namespace proxylib {
namespace asio {
namespace socks4a {

class connection_relay : private boost::noncopyable {
	protected:
		typedef boost::asio::ip::tcp::socket	socket_type;
		typedef boost::system::error_code		error_code;
		typedef boost::function
			<void (const error_code&, int)>		handler_type;

	public:
		explicit connection_relay(connection_t& client, connection_t& remote);
		~connection_relay();

		void start();
		void stop();

		inline connection_t& client() { return _client; }
		inline connection_t& remote() { return _remote; }

		inline handler_type& error_cb() { return _error_cb; }
		inline void set_error_cb(handler_type func) { _error_cb = func; }
		inline handler_type& disconnect_cb() { return _disconnect_cb; }
		inline void set_disconnect_cb(handler_type func) { _disconnect_cb = func; }

		struct endpoint {
			enum { client, remote };
		};

	protected:
		void setup_handlers();
		void handle_client_read(const error_code& error, const size_t& bytes_transferred);
		void handle_remote_read(const error_code& error, const size_t& bytes_transferred);
		void handle_client_write(const error_code& error, const size_t& bytes_transferred);
		void handle_remote_write(const error_code& error, const size_t& bytes_transferred);

	protected:
		connection_t&	_client;
		connection_t&	_remote;
		handler_type	_error_cb;
		handler_type	_disconnect_cb;
};

} // namespace socks4a
} // namespace asio
} // namespace proxylib
