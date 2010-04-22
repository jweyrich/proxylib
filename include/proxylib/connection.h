#pragma once

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <proxylib/common.h>

namespace proxylib {
namespace asio {
namespace socks4a {

template<size_t _BufferSize>
class connection : private boost::noncopyable {
	public:
		typedef boost::array<byte, _BufferSize>			buffer_type;
		typedef boost::asio::ip::tcp::socket			socket_type;
		typedef boost::system::error_code				error_code;
		typedef boost::function
			<void (const error_code&, const size_t&)>	handler_type;

		connection(boost::asio::io_service& io_service)
			: _io_service(io_service),
			  _socket(io_service) {
		}
		~connection() {
			disconnect();
		}
		bool connected() const {
			return _socket.is_open();
		}
		void disconnect() {
			if (connected()) {
//				error_code error;
//				// FIXME(jweyrich): any better way to check whether the socket is really connected or not?
//				_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, error);
//				if (error.value() != boost::asio::error::not_connected)
//					boost::asio::detail::throw_error(error);
				_socket.close();
			}
		}
		void dump_read_buffer(FILE* file, const size_t& size) const {
			dump_hex(file, _buffer.data(), size, 0);
		}
		socket_type& socket() { return _socket; }
		buffer_type& read_buffer() { return _buffer; }
		handler_type& read_cb() { return _read_cb; }
		handler_type& write_cb() { return _write_cb; }
		void set_read_cb(handler_type func) { _read_cb = func; }
		void set_write_cb(handler_type func) { _write_cb = func; }

	private:
		boost::asio::io_service&	_io_service;
		handler_type				_read_cb;
		handler_type				_write_cb;
		socket_type					_socket;
		buffer_type					_buffer;
};

typedef connection<2048> connection_t;

} // namespace socks4a
} // namespace asio
} // namespace proxylib
