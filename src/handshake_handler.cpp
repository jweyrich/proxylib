#include <proxylib/handshake_handler.h>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <proxylib/channel.h>
#include <proxylib/common.h>
#include <proxylib/shared_manager.h>

namespace proxylib {
namespace asio {
namespace socks4a {

handshake_handler::handshake_handler(channel& channel)
	: _channel(channel),
	  _resolver(channel.io_service()),
	  _relay(channel.client(), channel.remote()) {
	LOG_TRACE;
}

handshake_handler::~handshake_handler() {
	LOG_TRACE;
}

void handshake_handler::start() {
	LOG_TRACE;
	setup_handlers();
	read_request();
}

void handshake_handler::stop() {
	LOG_TRACE;
	_relay.stop();
	// Be sure to disconnect.
	_channel.client().disconnect();
	_channel.remote().disconnect();
}

handshake_handler::socket_type& handshake_handler::client_socket() {
	return _channel.client().socket();
}

void handshake_handler::setup_handlers() {
	_read_request_cb = boost::bind(
		&handshake_handler::handle_read_request,
		this,
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred
	);
	_resolve_cb = boost::bind(
		&handshake_handler::handle_resolve,
		this,
		boost::asio::placeholders::error,
		boost::asio::placeholders::iterator
	);
	_write_reply_cb = boost::bind(
		&handshake_handler::handle_write_reply,
		this,
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred
	);
	_relay.set_error_cb(boost::bind(
		&handshake_handler::handle_error,
		this,
		boost::asio::placeholders::error,
		_2 // endpoint
	));
	_relay.set_disconnect_cb(boost::bind(
		&handshake_handler::handle_disconnect,
		this,
		boost::asio::placeholders::error,
		_2 // endpoint
	));
}

void handshake_handler::read_request() {
	LOG_TRACE;
	async_read(
		_channel.client().socket(),
		boost::asio::buffer(_channel.client().read_buffer()),
		boost::asio::transfer_at_least(1),
		_read_request_cb
	);
}

void handshake_handler::handle_read_request(const error_code& error, const size_t& bytes_transferred) {
	LOG_TRACE;
	if (!error) {
		boost::tribool result;
		// TODO(jweyrich): use buffer + bytes_transferred instead of buffer.end() ?
		boost::tie(result, boost::tuples::ignore) = _request_parser.parse(
			_request, _channel.client().read_buffer().begin(), _channel.client().read_buffer().end());

		if (result) {
			LOG("bytes_transferred = %lu", bytes_transferred);
			// Swap endianess.
			//_request.swap_endianess(direction::incoming);
			if (_request.vn != version) {
				// Invalid version.
				handle_error(error, connection_relay::endpoint::client); // FIXME(jweyrich): should we pick a custom error?
				return;
			}
			switch (_request.cd) {
				case request::connect:
					if (IN_000X(_request.dstip)) {
						// Lookup host name.
						LOG("resolving %s", _request.hostname.c_str());
						_resolver.async_resolve(_request.query(), _resolve_cb);
					} else {
						endpoint_type endpoint = _request.endpoint();
						LOG("connecting to %s:%d",
							 endpoint.address().to_string().c_str(),
							 endpoint.port());
						_channel.remote().socket().async_connect(
							endpoint,
							boost::bind(
								&handshake_handler::handle_connect,
								this,
								boost::asio::placeholders::error,
								resolver_type::iterator()
							)
						);
					}
					break;
				case request::bind:
					// TODO(jweyrich): implement bind command
					break;
			}
		} else if (!result) {
			LOG("invalid request");
			// Invalid request.
			handle_error(error, connection_relay::endpoint::client); // FIXME(jweyrich): should we pick a custom error?
		} else {
			LOG("waiting more data...");
			// More data required.
			read_request();
		}
	} else if (error != boost::asio::error::operation_aborted) {
		handle_error(error, connection_relay::endpoint::client);
	}
}

void handshake_handler::handle_resolve(const error_code& error, resolver_type::iterator iter) {
	LOG_TRACE;
	if (!error) {
		endpoint_type endpoint = *iter;
		LOG("connecting to %s:%d",
			 endpoint.address().to_string().c_str(),
			 endpoint.port());
		_channel.remote().socket().async_connect(
			endpoint,
			boost::bind(
				&handshake_handler::handle_connect,
				this,
				boost::asio::placeholders::error,
				++iter
			)
		);
	} else {
		LOG("error: %s", error.message().c_str());
	}
}

void handshake_handler::handle_connect(const error_code& error, resolver_type::iterator iter) {
	LOG_TRACE;
	if (!error) {
		LOG("connected");
		_reply.cd = reply::request_granted;
	} else if (iter != resolver_type::iterator()) {
		// The connection failed. Try the next endpoint in the list.
		_channel.remote().disconnect();
		handle_resolve(error_code(), iter);
		return;
	} else {
		LOG("error: %s", error.message().c_str());
		_reply.cd = reply::request_failed;
		_channel.remote().disconnect();
	}
	// Send reply to client.
	async_write(
		_channel.client().socket(),
		_reply.buffer(),
		_write_reply_cb
	);
}

void handshake_handler::handle_write_reply(const error_code& error, const size_t& bytes_transferred) {
	LOG_TRACE;
	if (!error) {
		if (!_reply.success()) {
			LOG("!_reply.success()");
			_channel.client().disconnect();
			return;
		}
		_relay.start();
	} else {
		handle_error(error, connection_relay::endpoint::client);
	}
}

void handshake_handler::handle_error(const error_code& error, int endpoint) {
	LOG_TRACE;
	LOG("error: %s", error.message().c_str());
	_channel.stop();
	handle_disconnect(error, endpoint);
}

void handshake_handler::handle_disconnect(const error_code& error, int endpoint) {
	LOG_TRACE;
	_channel.remove_from_manager();
}

} } } // namespace proxylib::asio::socks4a
