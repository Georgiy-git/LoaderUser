#include "Session.hpp"
#include "Functions.hpp"

#include <fstream>
#include <chrono>
#include <boost/asio.hpp>

using namespace boost::asio;

Functions::Functions(Session* _obj) { obj = _obj; }

void Functions::null(std::string)
{
	obj->_write_command();
}

void Functions::command_not_found(std::string)
{
	std::cout << "Сервер: команда не распознана.\n";
	obj->_write_command();
}

void Functions::message(std::string message)
{
	std::cout << message;
	obj->_write_command();
}

void Functions::files_on_server(std::string line)
{
	std::string str;
	for (const auto& i : line) {
		if (i != '&') { str += i; }
		else {
			if (!str.empty()) {
				std::cout << str << std::endl;
			}
			str.clear();
		}
	}
	obj->_write_command();
}

void Functions::load_file(std::string line)
{
	auto i = line.find('&');
	std::string _quantity = line.substr(0, i);
	std::string file_name = line.substr(i + 1);

	size_t quantity;
	try {
		quantity = std::stoi(_quantity);
	}
	catch (...) {
		std::cerr << "Не удалось распознать размер скачиваемого файла.\n";
		obj->_write_command();
		return;
	}

	std::ofstream file(file_name.c_str(), std::ios::binary);
	if (!file.is_open()) {
		std::cout << "Не удалось создать файл.\n";
		std::this_thread::sleep_for(std::chrono::seconds(1));
		obj->_write_command();
		return;
	}

	boost::system::error_code ec;

	while (quantity > 0) {
		size_t rd = read(obj->socket, obj->buf, transfer_at_least(20), ec);
		if (ec) {
			std::cerr << "Произошла ошибка при загрузке.\n";
			obj->_write_command();
			return;
		}

		quantity -= rd;

		file << &obj->buf;
		if (file.fail()) {
			std::cerr << "Произошла ошибка при загрузке.\n";
			obj->_write_command();
			return;
		}
	}

	file.close();
	obj->_write_command();
}
