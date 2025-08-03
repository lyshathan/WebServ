#include "ServerConfig.hpp"
#include "LocationConfig.hpp"
#include "Token.hpp"


size_t	CountErrorCodes(std::vector<t_token> &tokenList, std::vector< t_token>::iterator it)
{
	size_t	count = 0;

	while (it != tokenList.end() && it->type != SEMICOLON)
	{
		it++;
		count++;
	}
	return (count);
}

ServerConfig::ServerConfig(std::vector<t_token> &tokenList, std::vector< t_token>::iterator &it) : _tokens(tokenList), _clientMaxBodySize(0), _currentLevel(it->level)
{
	it++;
	char *end;

	std::cout << PURPLE << "	-- CONFIGURING A NEW SERVER --" << RESET << std::endl;

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
		{
			double port = std::strtod((++it)->content.c_str(), &end);
			if (*end || port <= 0 || port > INT_MAX)
				throw std::invalid_argument("[ServerConfig] Invalid port");
			_listenPorts.push_back(port);
			it++;
			if (it == _tokens.end() || it->type != SEMICOLON)
				throw std::invalid_argument("[ServerConfig] Invalid port (semicolon)");
		}

		else if (it->type == DIRECTIVE && it->content == "server_name")	// server names
		{
			it++;
			while (it != _tokens.end() &&  it->type == VALUE)
			{
				_serverNames.push_back(it->content);
				it++;
			}
			if (it == _tokens.end() || it->type != SEMICOLON)
				throw std::invalid_argument("[ServerConfig] Invalid server_name (semicolon)");
		}

		else if (it->type == DIRECTIVE && it->content == "root")	// root
		{
			_root = (++it)->content;
			it++;
			if (it == _tokens.end() || it->type != SEMICOLON)
				throw std::invalid_argument("[ServerConfig] Invalid root (semicolon)");
		}

		else if (it->type == DIRECTIVE && it->content == "index")	// index files
		{
			it++;
			while (it != _tokens.end() &&  it->type == VALUE)
			{
				_indexFiles.push_back(it->content);
				it++;
			}
		}

		else if (it->type == DIRECTIVE && it->content == "client_max_body_size")	// client_max_body_size
		{
			double bodySize = std::strtod((++it)->content.c_str(), &end);
			if (bodySize <= 0 || bodySize > INT_MAX)
				throw std::invalid_argument("[ServerConfig] Invalid client_max_body_size");
			_clientMaxBodySize = bodySize;
			it++;
			if (it == _tokens.end() || it->type != SEMICOLON)
				throw std::invalid_argument("[ServerConfig] Invalid client_max_body_size (semicolon)");
		}

		else if (it->type == DIRECTIVE && it->content == "error_page")	// error_page
		{
			char *end;
			it++;
			size_t	nbOfErrorCode = CountErrorCodes(_tokens, it);
			std::vector< t_token>::iterator itErrorPage = it + nbOfErrorCode - 1;
			while (it != itErrorPage)
			{
				double code = std::strtod(it->content.c_str(), &end);
				if (*end || code < 0 || code > INT_MAX)					// Need to validate the rules here ----------------
					throw std::invalid_argument("[ServerConfig] Invalid error_page");
				_errorPages[code] = itErrorPage->content;
				it++;
			}
			it++;
			if (it == _tokens.end() || it->type != SEMICOLON)
				throw std::invalid_argument("[ServerConfig] error_page (semicolon)");
		}

		else if (it->type == DIRECTIVE && it->content == "location")	// location
		{
			LocationConfig newLocation(_tokens, it);
			_locations.push_back(newLocation);
			newLocation.PrintLocation();
		}

		else
			throw std::invalid_argument("[ServerConfig] unknown directive");
	}
}

ServerConfig::~ServerConfig(void)
{

}


void	ServerConfig::PrintServer(void)
{
	std::cout << PURPLE ;
	std::cout << "		Listen ports :" << std::endl;
	for (std::vector<int>::iterator it = _listenPorts.begin() ; it != _listenPorts.end() ; it++)
	{
		std::cout << "			- " << *it << std::endl;
	}
	std::cout << "		Server Names :" << std::endl;
	for (std::vector<std::string>::iterator it = _serverNames.begin() ; it != _serverNames.end() ; it++)
	{
		std::cout << "			- " << *it << std::endl;
	}
	std::cout << "		Root : " << _root << std::endl;
	std::cout << "		Index files : " << std::endl;
	for (std::vector<std::string>::iterator it = _indexFiles.begin() ; it != _indexFiles.end() ; it++)
	{
		std::cout << "			- " << *it << std::endl;
	}
	std::cout << "		Client max body size : " << _clientMaxBodySize << std::endl;
	std::cout << "		Error pages : " << std::endl;
	for (std::map< int , std::string >::iterator it = _errorPages.begin() ; it != _errorPages.end() ; it++)
	{
		std::cout << "			- " << it->first << " | " << it->second << std::endl;
	}
	std::cout << RESET ;

}

