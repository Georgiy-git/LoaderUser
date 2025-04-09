#include "Session.hpp"
#include "Functions.hpp"

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
