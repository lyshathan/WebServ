#include "ServerConfig.hpp"

////////////////////////////////////////////////////////////////////////////////////
//								Constructor & Destructor
////////////////////////////////////////////////////////////////////////////////////

ServerConfig::ServerConfig(std::vector<t_token> &tokenList, std::vector< t_token>::iterator &it, std::vector< ServerConfig > &serversConfig)
: _serversConfig(serversConfig), _tokens(tokenList), _clientMaxBodySize(0), _currentLevel(it->level)
{
	ParseServerConfig(it);
}

ServerConfig::~ServerConfig(void)
{

}

////////////////////////////////////////////////////////////////////////////////////
//										Methods
////////////////////////////////////////////////////////////////////////////////////



void	ServerConfig::ParseServerConfig(std::vector< t_token>::iterator &it)
{
	it++;
	if (it++->type != OPEN_BRACE)
		throw std::invalid_argument("[ServerConfig] Missing open brace");


	while (!(it->type == CLOSE_BRACE && it->level == _currentLevel))
	{
		// std::cout << "[ServerConfig] it->content : " << it->content << RESET << std::endl;

		if (it->type == SEMICOLON)
		{
			it++;
			continue;
		}
		else if (it->type == DIRECTIVE && it->content == "listen")	// listen port
			ParseListenPort(it);
		else if (it->type == DIRECTIVE && it->content == "server_name")	// server names
			AddToVector(_serverNames, it);
		else if (it->type == DIRECTIVE && it->content == "index")	// index files
			AddToVector(_indexFiles, it);
		else if (it->type == DIRECTIVE && it->content == "root")	// root
			ParseRoot(it);
		else if (it->type == DIRECTIVE && it->content == "client_max_body_size")	// client_max_body_size
			ParseClientMaxBodySize(it);
		else if (it->type == DIRECTIVE && it->content == "error_page")	// error_page
			ParseErrorPage(it);
		else if (it->type == DIRECTIVE && it->content == "location")	// location
		{
			LocationConfig newLocation(_tokens, it, _locations);
			_locations.push_back(newLocation);
			// newLocation.PrintLocation();
		}
		else
			throw std::invalid_argument("[ServerConfig] unknown directive");
	}
}

void	ServerConfig::PrintServer(void)
{
	std::string indent = "|	|___ ";
	std::string list = "|	|	- ";
	
	std::cout << "|" << std::endl;
	std::cout << "|" << PURPLE << "==== SERVER ========" << RESET << std::endl;
	std::cout << indent << "Listen ports :" << std::endl;
	for (std::vector<int>::iterator it = _listenPorts.begin() ; it != _listenPorts.end() ; it++)
	{
		std::cout << list << *it << std::endl;
	}
	std::cout << indent << "Server Names :" << std::endl;
	for (std::vector<std::string>::iterator it = _serverNames.begin() ; it != _serverNames.end() ; it++)
	{
		std::cout << list << *it << std::endl;
	}
	std::cout << indent << "Root : " << _root << std::endl;
	std::cout << indent << "Index files : " << std::endl;
	for (std::vector<std::string>::iterator it = _indexFiles.begin() ; it != _indexFiles.end() ; it++)
	{
		std::cout << list << *it << std::endl;
	}
	std::cout << indent << "Client max body size : " << _clientMaxBodySize << std::endl;
	std::cout << indent << "Error pages : " << std::endl;
	for (std::map< int , std::string >::iterator it = _errorPages.begin() ; it != _errorPages.end() ; it++)
	{
		std::cout << list << it->first << " | " << it->second << std::endl;
	}
	for (std::vector< LocationConfig >::iterator it = _locations.begin() ; it != _locations.end() ; it++)
	{
		it->PrintLocation();
	}
}


