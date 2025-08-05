#include "Config.hpp"
#include "ServerConfig.hpp"
#include "Utils.hpp"

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


void	Config::Parser()
{
	GlobalConfig global;
	for (std::vector< t_token>::iterator it = _tokens.begin() ; it != _tokens.end() ; it++)
	{
		if (it->level == GLOBAL && it->type == DIRECTIVE)
		{
			if (it->content == "server")
			{
				ServerConfig newServer(_tokens, it, _serversConfig);
				_serversConfig.push_back(newServer);
				// newServer.PrintServer();
			}
			else if (it->content == "error_log")
			{
				global.setErrorLog((++it)->content);
				it++;
				if (it == _tokens.end() || it->type != SEMICOLON)
					ThrowError(" Invalid error_log", *it);
			}
			else if (it->content == "client_max_body_size")
			{
				global.setClientMaxBodySize((++it)->content, *it);
				it++;
				if (it == _tokens.end() || it->type != SEMICOLON)
					ThrowError(" Invalid client_max_body_size", *it);
			}
		}
		else
			ThrowError(" Invalid configuration file", *it);
	}
	_globalConfig = global;
	PrintConfig();
}


void	Config::PrintConfig(void)
{
	std::cout << "================================================================" << std::endl;
	_globalConfig.printGlobal();
	for (std::vector< ServerConfig >::iterator itServer = _serversConfig.begin() ; itServer != _serversConfig.end() ; itServer++)
	{
		itServer->PrintServer();
	}
	std::cout << "================================================================" << std::endl << std::endl;
}
