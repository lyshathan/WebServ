#include "GlobalConfig.hpp"

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
		throwErrorToken(" Invalid client_max_body_size", token);
	this->_clientMaxBodySize = bodySize;
	setSizeDefined(true);
}

