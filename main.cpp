#include "Session.hpp"

int main(int args, char** argv) {
	setlocale(LC_ALL, "RU");
	const unsigned int port = 53888;
	const std::string server_ip = "192.168.251.31";
	io_context io_context;
	ip::tcp::socket socket{io_context};
	Session session(socket, port, server_ip);
	io_context.run();
}