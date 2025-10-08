#include "GlobalConfig.hpp"

////////////////////////////////////////////////////////////////////////////////////
//								Constructor & Destructor
////////////////////////////////////////////////////////////////////////////////////

GlobalConfig::GlobalConfig(void) : _sizeDefined(false) {}

GlobalConfig::~GlobalConfig(void) {}

////////////////////////////////////////////////////////////////////////////////////
//										Methods
////////////////////////////////////////////////////////////////////////////////////

GlobalConfig &GlobalConfig::operator=(GlobalConfig const &otherGlobalConfig)
{
	this->_error_log = otherGlobalConfig._error_log;
	this->_clientMaxBodySize = otherGlobalConfig._clientMaxBodySize;
	this->_sizeDefined = otherGlobalConfig._sizeDefined;
	return (*this);
}

void GlobalConfig::check(void)
{
	if (_error_log.empty())
		_error_log = "./log/error.log";		// set default error log file  -  CHECK
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
