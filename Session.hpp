#pragma once

#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

using namespace boost::asio;
using error_code = boost::system::error_code;

class Session
{
private:
	ip::tcp::socket& socket;
	error_code _ec;
	const unsigned int port;
	const std::string server_ip;
	streambuf buf;

	void _async_connect();
	void _async_read();
	void _read_buf();
	void _connect_repeat();
	void _write_command();

public:
	Session(ip::tcp::socket& socket, const unsigned int port, const std::string server_ip);
};

