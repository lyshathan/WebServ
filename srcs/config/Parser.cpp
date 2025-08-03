#include "Config.hpp"
#include "ServerConfig.hpp"
#include "GlobalConfig.hpp"

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
			std::cout << "Invalid directive" << std::endl;
	}

	global.printGlobal();
}