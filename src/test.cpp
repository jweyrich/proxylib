#include <cstdlib>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <proxylib/proxylib.h>

int main(int argc, char** argv) {
	try {
		if (argc != 2) {
			std::cerr << "Usage: proxy <port>" << std::endl;
			return EXIT_FAILURE;
		}

		namespace socks4a = proxylib::asio::socks4a;

		boost::asio::io_service	io_service;
		socks4a::server			server(io_service, std::atoi(argv[1]));
		boost::thread			server_thread;

		server_thread = boost::thread(boost::bind(&socks4a::server::run, &server));
		server_thread.join();
//		server.run();
	} catch (std::exception& ex) {
		std::cerr << "Exception: " << ex.what() << std::endl;
	}

	return EXIT_SUCCESS;
}
