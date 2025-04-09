#pragma once

#include <string>

class Session;

class Functions
{
public:
	Functions(Session* _obj);

	static void null(std::string);
	static void command_not_found(std::string);
	static void message(std::string);
	static void files_on_server(std::string);

private:
	static inline Session* obj;
};

