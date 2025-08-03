#include "ConfigIncludes.hpp"
#include "Config.hpp"
#include "GlobalConfig.hpp"
#include "ServerConfig.hpp"

Config::Config(std::string filename): _braceLevel(0), _lineNumber(0), _level(GLOBAL), _expectedToken(DIRECTIVE), _configFileName(filename)
{
	std::ifstream	configFile(filename.c_str());

	std::cout << BLUE << "Reading from config file : " << filename << RESET <<std::endl;

	InitValidDirectives();
	InitToken(configFile);

	Parser();
}

Config::~Config(void)
{
	std::cout << RED << "Conf destructor called" << RESET <<std::endl;
}

const char* Config::ConfigException::what() const throw()
{
	return "Error: Invalid configuration file at line : ";
}


