#pragma once

#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <unordered_map>
#include <functional>
#include <exception>

#include "Commands.hpp"

using namespace boost::asio;
using error_code = boost::system::error_code;

class Session
{
private:
	friend class Functions;

	ip::tcp::socket& socket;
	error_code _ec;
	const unsigned int port;
	const std::string server_ip;
	streambuf buf;
	std::unordered_map<int, std::function<void(std::string)>> commands;
	Functions* Funcs = nullptr;

	void _async_connect();
	void _async_read();
	void _read_buf();
	void _connect_repeat();
	void _write_command();
	void _commands_init();

public:
	Session(ip::tcp::socket& socket, const unsigned int port, const std::string server_ip);
};

