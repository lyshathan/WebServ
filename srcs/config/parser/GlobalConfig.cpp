#include "GlobalConfig.hpp"

GlobalConfig::GlobalConfig(void)
{

}

GlobalConfig::~GlobalConfig(void)
{

}

void GlobalConfig::setErrorLog(std::string &errorLog)
{
	this->_error_log = errorLog;
}

void GlobalConfig::setClientMaxBodySize(std::string &size, t_token &token)
{
	char *end;

	double bodySize = std::strtod(size.c_str(), &end);
	if (*end || bodySize <= 0 || bodySize > INT_MAX)
		ThrowError(" Invalid client_max_body_size", token);
	this->_client_max_body_size = bodySize;
}

void GlobalConfig::printGlobal(void)
{
	std::string indent = "|___ ";

	std::cout << BLUE << "GLOBAL" << RESET << std::endl;
	std::cout << indent << "Error log : " << _error_log << std::endl;
	std::cout << indent << "Client max Body Size : " << _client_max_body_size << std::endl;
	std::cout << RESET;

}