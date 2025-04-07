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
		else if (ec == boost::asio::error::connection_reset) {
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
	std::string answer;
	std::istream istream(&buf);
	std::getline(istream, answer, '\f');
	int x = std::stoi(answer);

	switch (x)
	{
	case 1: { _write_command(); break; }
	default:
		std::cerr << "Ошибка: не удалось распознать ответ, полученный с сервера.\nПереподключение...\n";
		socket.close();
		_async_connect();
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
	socket.send(buffer(command));
	_async_read();
}

Session::Session(ip::tcp::socket& socket, const unsigned int port, const std::string server_ip)
	: socket{socket}, port{port}, server_ip{server_ip}
{
	_async_connect();
}
