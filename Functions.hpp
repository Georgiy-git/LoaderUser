#pragma once

#include <string>

class Session;

class Functions
{
public:
	Functions(Session* _obj);

	static void unlock_write_command(std::string);
	static void command_not_found(std::string);

private:
	static inline Session* obj;
};

