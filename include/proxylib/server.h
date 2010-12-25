#pragma once

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>
#include <proxylib/common.h>
#include <proxylib/channel.h>
#include <proxylib/shared_manager.h>

namespace proxylib {
namespace asio {
namespace socks4a {

class server : private boost::noncopyable {
private:
	typedef boost::asio::ip::tcp::endpoint	tcp_endpoint;
	typedef boost::system::error_code		error_code;
	typedef shared_manager<channel>			channel_manager;

public:
	explicit server(boost::asio::io_service& io_service, uint16 port)
		: _io_service(io_service),
		  _acceptor(io_service, tcp_endpoint(boost::asio::ip::tcp::v4(), port)),
		  _channel_manager() {
	}

	void run() {
		async_accept();
		// The io_service::run() call will block until all asynchronous operations
		// have finished. While the server is running, there is always at least one
		// asynchronous operation outstanding: the asynchronous accept call waiting
		// for new incoming connections.
		_io_service.run();
	}
	void stop() {
		// Post a call to the stop function so that server::stop() is safe to call
		// from any thread.
		_io_service.post(boost::bind(&server::handle_stop, this));
	}

private:
	void async_accept() {
		_new_channel.reset(new channel(_channel_manager, _io_service));
		_acceptor.async_accept(
			_new_channel->client().socket(),
			boost::bind(
				&server::handle_accept_connection,
				this,
				boost::asio::placeholders::error
			)
		);
	}

	void handle_accept_connection(const error_code& error) {
		if (!error) {
			_channel_manager.add(_new_channel);
			_new_channel->start();
			async_accept();
		}
	}

	void handle_stop() {
		// The server is stopped by cancelling all outstanding asynchronous
		// operations. Once all operations have finished the io_service::run() call
		// will exit.
		_acceptor.close();
		std::for_each(_channel_manager.begin(), _channel_manager.end(),
			boost::bind(&channel::stop, _1));
		_channel_manager.clear();
	}

private:
	boost::asio::io_service&		_io_service;
	boost::asio::ip::tcp::acceptor	_acceptor;
	channel_manager					_channel_manager;
	channel_manager::shared_type	_new_channel;
};

} } } // namespace proxylib::asio::socks4a
