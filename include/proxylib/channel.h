#pragma once

#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <proxylib/connection.h>
#include <proxylib/handshake_handler.h>
#include <proxylib/shared_manager.h>

namespace proxylib {
namespace asio {
namespace socks4a {

class channel
	: public boost::enable_shared_from_this<channel>,
	  private boost::noncopyable
{
	public:
		typedef boost::shared_ptr<channel>				shared_ptr;
		typedef shared_manager<channel>					manager_type;
		typedef boost::asio::ip::tcp::socket			socket_type;
		typedef boost::system::error_code				error_code;
		typedef boost::function
			<void (const error_code&, const size_t&)>	handler_type;

		channel(manager_type& manager, boost::asio::io_service& io_service)
			: _manager(manager),
			  _io_service(io_service),
			  _client(io_service),
			  _remote(io_service),
			  _handler(*this) {
			DTRACE;
		}
		~channel() {
			DTRACE;
			stop();
		}
		void start() {
			DTRACE;
			_handler.start();
		}
		void stop() {
			DTRACE;
			_handler.stop();
		}
		void remove_from_manager() {
			DTRACE;
			_manager.remove(shared_from_this());
		}
		boost::asio::io_service& io_service() { return _io_service; }
		connection_t& client() { return _client; }
		connection_t& remote() { return _remote; }

	private:
		manager_type&				_manager;
		boost::asio::io_service&	_io_service;
		connection_t				_client;
		connection_t				_remote;
		handshake_handler			_handler;
};

} // namespace socks4a
} // namespace asio
} // namespace proxylib
