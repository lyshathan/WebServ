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


void	Config::Parser()
{
	GlobalConfig global;
	for (std::vector< t_token>::iterator it = _tokens.begin() ; it != _tokens.end() ; it++)
	{
		if (it->level == GLOBAL && it->type == DIRECTIVE)
		{
			if (it->content == "server")
			{
				ServerConfig newServer(_tokens, it);
				_serversConfig.push_back(newServer);
				newServer.PrintServer();
			}
			else if (it->content == "error_log")
			{
				global.setErrorLog((++it)->content);
				it++;
				if (it == _tokens.end() || it->type != SEMICOLON)
					throw std::invalid_argument("[GlobalConfig] Invalid error_log (semicolon)");
			}
			else if (it->content == "client_max_body_size")
			{
				global.setClientMaxBodySize((++it)->content);
				it++;
				if (it == _tokens.end() || it->type != SEMICOLON)
					throw std::invalid_argument("[GlobalConfig] Invalid client_max_body_size (semicolon)");
			}
		}
		else
			throw std::invalid_argument("[GlobalConfig] Invalid configuration file");
	}

	global.printGlobal();
}


