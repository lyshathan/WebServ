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

void GlobalConfig::setClientMaxBodySize(std::string &size)
{
	char *end;

	double bodySize = std::strtod(size.c_str(), &end);
	if (bodySize <= 0 || bodySize > INT_MAX)
		throw std::invalid_argument("[GlobalConfig] Invalid client_max_body_size");
	this->_client_max_body_size = bodySize;
}

void GlobalConfig::printGlobal(void)
{
	std::cout << GREEN;
	std::cout << "Error log : " << _error_log << std::endl;
	std::cout << "Client max Body Size : " << _client_max_body_size << std::endl;
	std::cout << RESET;

}