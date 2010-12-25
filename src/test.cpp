#include <cstdlib>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <proxylib/proxylib.h>

void signal_handler(int signal) {
	std::cerr << "Received signal " << signal << std::endl << "Exiting..." << std::endl;
	exit(EXIT_SUCCESS);
}

void install_sighandlers() {
	signal(SIGHUP, SIG_IGN);
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);
	signal(SIGKILL, signal_handler);
	signal(SIGQUIT, signal_handler);
}

int main(int argc, char** argv) {
	try {
		if (argc != 2) {
			std::cerr << "Usage: proxy <port>" << std::endl;
			return EXIT_FAILURE;
		}

		install_sighandlers();

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
