#include "GlobalConfig.hpp"

GlobalConfig::GlobalConfig(void) : _sizeDefined(false)
{

}

GlobalConfig::~GlobalConfig(void)
{

}

void GlobalConfig::setSizeDefined(bool status)
{
	this->_sizeDefined = status;
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
		ThrowErrorToken(" Invalid client_max_body_size", token);
	this->_clientMaxBodySize = bodySize;
	setSizeDefined(true);
}

size_t GlobalConfig::getClientMaxBodySize(void) const
{
	return (_clientMaxBodySize);
}


void GlobalConfig::Check(void)
{
	if (_error_log.empty())
		_error_log = "./log/error.log";		// set default error log file
	if (_sizeDefined == false)
		_clientMaxBodySize = 1000000;
}

void GlobalConfig::printGlobal(void)
{
	std::string indent = "|___ ";

	std::cout << BLUE << "GLOBAL" << RESET << std::endl;
	std::cout << indent << "Error log : " << _error_log << std::endl;
	std::cout << indent << "Client max Body Size : " << _clientMaxBodySize << std::endl;
	std::cout << RESET;

}