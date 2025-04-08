#include "Session.hpp"
#include "Functions.hpp"

Functions::Functions(Session* _obj) { obj = _obj; }

void Functions::unlock_write_command(std::string)
{
	obj->_write_command();
}

void Functions::command_not_found(std::string)
{
	std::cout << "Сервер: команда не распознана.\n";
	obj->_write_command();
}
