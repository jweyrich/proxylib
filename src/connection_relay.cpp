#include <proxylib/connection_relay.h>
#include <iostream>
#include <proxylib/common.h>
#include <proxylib/connection.h>

namespace proxylib {
namespace asio {
namespace socks4a {

connection_relay::connection_relay(connection_t& client, connection_t& remote)
	: _client(client),
	  _remote(remote) {
}

connection_relay::~connection_relay() {
	stop();
}

void connection_relay::start() {
	setup_handlers();
	// Read from client.
	_client.socket().async_read_some(
		boost::asio::buffer(_client.read_buffer()),
		_client.read_cb()
	);
	// Read from remote.
	_remote.socket().async_read_some(
		boost::asio::buffer(_remote.read_buffer()),
		_remote.read_cb()
	);
}

void connection_relay::stop() {
	if (_client.connected())
		_client.socket().cancel();
	if (_remote.connected())
		_remote.socket().cancel();
}

void connection_relay::setup_handlers() {
	_client.set_read_cb(boost::bind(
		&connection_relay::handle_client_read,
		this,
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred
	));
	_remote.set_read_cb(boost::bind(
		&connection_relay::handle_remote_read,
		this,
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred
	));
	_client.set_write_cb(boost::bind(
		&connection_relay::handle_client_write,
		this,
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred
	));
	_remote.set_write_cb(boost::bind(
		&connection_relay::handle_remote_write,
		this,
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred
	));
}

void connection_relay::handle_client_read(const error_code& error, const size_t& bytes_transferred) {
	LOG_TRACE;
	switch (error.value()) {
		case 0:
			_client.dump_read_buffer(stdout, bytes_transferred);
			async_write(
				_remote.socket(),
				boost::asio::buffer(_client.read_buffer(), bytes_transferred),
				_remote.write_cb()
			);
			break;
//		case boost::asio::error::eof:
//		case boost::asio::error::connection_reset:
//			if (!_disconnect_cb.empty())
//				_disconnect_cb(error, endpoint::client);
//			break;
		case boost::asio::error::operation_aborted:
			break;
		default:
			if (!_error_cb.empty())
				_error_cb(error, endpoint::client);
	}
}

void connection_relay::handle_remote_read(const error_code& error, const size_t& bytes_transferred) {
	LOG_TRACE;
	switch (error.value()) {
		case 0:
			_remote.dump_read_buffer(stdout, bytes_transferred);
			async_write(
				_client.socket(),
				boost::asio::buffer(_remote.read_buffer(), bytes_transferred),
				_client.write_cb()
			);
			break;
//		case boost::asio::error::eof:
//		case boost::asio::error::connection_reset:
//			if (!_disconnect_cb.empty())
//				_disconnect_cb(error, endpoint::remote);
//			break;
		case boost::asio::error::operation_aborted:
			break;
		default:
			if (!_error_cb.empty())
				_error_cb(error, endpoint::remote);
	}
}

void connection_relay::handle_client_write(const error_code& error, const size_t& bytes_transferred) {
	LOG_TRACE;
	switch (error.value()) {
		case 0:
			_remote.socket().async_read_some(
				boost::asio::buffer(_remote.read_buffer()),
				_remote.read_cb()
			);
			break;
		case boost::asio::error::operation_aborted:
			break;
		default:
			if (!_error_cb.empty())
				_error_cb(error, endpoint::client);
	}
}

void connection_relay::handle_remote_write(const error_code& error, const size_t& bytes_transferred) {
	LOG_TRACE;
	switch (error.value()) {
		case 0:
			_client.socket().async_read_some(
				boost::asio::buffer(_client.read_buffer()),
				_client.read_cb()
			);
			break;
		case boost::asio::error::operation_aborted:
			break;
		default:
			if (!_error_cb.empty())
				_error_cb(error, endpoint::remote);
	}
}

} } } // namespace proxylib::asio::socks4a
