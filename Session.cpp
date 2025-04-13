#include "Functions.hpp"
#include "Session.hpp"

void Session::_async_connect()
{
	socket.async_connect(ip::tcp::endpoint(ip::make_address_v4(server_ip), port),
		[this](error_code ec) {
			if (ec == error::connection_refused) {
				std::cerr << "Сервер не ответил. Повторная попытка...\n";
				std::this_thread::sleep_for(std::chrono::seconds(1));
				_async_connect();
			}
			else if (ec == error::eof) {
				_connect_repeat();
			}
			else if (ec) {
				std::cerr << "Ошибка подключения к серверу: " << ec.message() << std::endl;
				std::this_thread::sleep_for(std::chrono::seconds(1));
				_async_connect();
			}
			else {
				std::cout << "Подключение выполнено.\n";
				_write_command();
			}
		});
}

void Session::_async_read()
{
	async_read_until(socket, buf, '\f', [this](error_code ec, size_t bytes) {
		if (ec == error::eof) {
			_connect_repeat();
		}
		else if (ec == error::connection_reset) {
			std::cerr << "Сервер перестал отвечать. Переподключение...\n";
			socket.close();
			_async_connect();
		}
		else if (ec) {
			std::cerr << "Ошибка получения информации с сервера: " << ec.message() << std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(1));
			_async_read();
			return;
		}
		else {
			_read_buf();
		}
	});
}

void Session::_read_buf()
{
	int num_command;
	std::string command;

	istream >> num_command;
	istream.get();
	std::getline(istream, command, '\f');

	auto pair = commands.find(num_command);
	if (pair == commands.end()) {
		std::cerr << "Ошибка: полученная с сервера команда не распознана.\n";
		_write_command();
	}
	else {
		pair->second(command);
	}
}

void Session::_connect_repeat()
{
	std::cerr << "Сервер разорвал соединение.\n";
	char x = 'N';
	std::cout << "Подключиться повторно? [Y]: ";
	std::cin >> x;
	if (x == 'y' || x == 'Y') {
		_async_connect();
	}
	else {
		exit(0);
	}
}

void Session::_write_command()
{
	std::cout << "Введите команду:  ";
	std::string command;
	std::getline(std::cin, command);

	if (command.starts_with("/exit") || command.starts_with("exit")) {
		std::cout << "Соединение с сервером закрыто.\n";
		socket.close();
		exit(0);
	}

	command += '\f';

	try {
		_async_read();
		socket.send(buffer(command.data(), command.size()));
	}
	catch (...) {}
}

void Session::_commands_init()
{
	commands[(int)Com::command_not_found] = Funcs->command_not_found;
	commands[(int)Com::null] = Funcs->null;
	commands[(int)Com::message] = Funcs->message;
	commands[(int)Com::files_on_server] = Funcs->files_on_server;
	commands[(int)Com::load_file] = Funcs->load_file;
}

Session::Session(ip::tcp::socket& socket, const unsigned int port, const std::string server_ip)
	: socket{socket}, port{port}, server_ip{server_ip}, Funcs{new Functions(this)}, istream(&buf)
{
	_commands_init();
	_async_connect();
}
