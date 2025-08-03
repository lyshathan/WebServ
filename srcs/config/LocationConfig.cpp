#include "ServerConfig.hpp"
#include "LocationConfig.hpp"
#include "Token.hpp"

LocationConfig::LocationConfig(std::vector<t_token> &tokenList, std::vector< t_token>::iterator &it): _tokens(tokenList), _currentLevel(it->level), _clientMaxBodySize(0)
{
	char *end;
	it++;

	std::cout << YELLOW << "		-- CONFIGURING A LOCATION --" << RESET << std::endl;

	_path = (it++)->content;

	if (it++->type != OPEN_BRACE)
		throw std::invalid_argument("[LocationConfig] Missing open brace");

	while (!(it->type == CLOSE_BRACE && it->level == _currentLevel))
	{
		// std::cout << "	[LocationConfig] it->content : " << it->content << RESET << std::endl;

		if (it->type == SEMICOLON)
		{
			it++;
			continue;
		}

		else if (it->type == DIRECTIVE && it->content == "allow_methods")	// allowed Method
		{
			it++;
			while (it != _tokens.end() &&  it->type == VALUE)
			{
				_allowMethod.push_back(it->content);
				it++;
			}
			if (it == _tokens.end() || it->type != SEMICOLON)
				throw std::invalid_argument("[LocationConfig] Invalid allow_methods (semicolon)");
		}

		else if (it->type == DIRECTIVE && it->content == "autoindex")	// auto index
		{
			it++;
			if (it->content == "on")
				_autoIndex = true;
			else if (it->content == "off")
				_autoIndex = false;
			else
				throw std::invalid_argument("[LocationConfig] Invalid autoindex");
			it++;
			if (it == _tokens.end() || it->type != SEMICOLON)
					throw std::invalid_argument("[LocationConfig] Invalid autoindex (semicolon)");
		}

		else if (it->type == DIRECTIVE				// paths
			&& (it->content == "upload_path"
				|| it->content == "cgi_extension"
				|| it->content == "cgi_path"
				|| it->content == "root"))
		{
			if (it->content == "upload_path")
				_uploadPath = (++it)->content;
			else if (it->content == "cgi_extension")
				_cgiExtension = (++it)->content;
			else if (it->content == "cgi_path")
				_cgiPath = (++it)->content;
			else if (it->content == "root")
				_root = (++it)->content;
			it++;
			if (it == _tokens.end() || it->type != SEMICOLON)
					throw std::invalid_argument("[LocationConfig] Invalid path (semicolon)");
		}

		else if (it->type == DIRECTIVE && it->content == "client_max_body_size")	// client_max_body_size
		{
			double bodySize = std::strtod((++it)->content.c_str(), &end);
			if (bodySize <= 0 || bodySize > INT_MAX)
				throw std::invalid_argument("[LocationConfig] Invalid client_max_body_size");
			_clientMaxBodySize = bodySize;
			it++;
			if (it == _tokens.end() || it->type != SEMICOLON)
				throw std::invalid_argument("[LocationConfig] Invalid client_max_body_size (semicolon)");
		}

		else if (it->type == DIRECTIVE && it->content == "return")	// return
		{
			it++;

			double code = std::strtod(it->content.c_str(), &end);
			if (*end || code < 0 || code > INT_MAX)					// Need to validate the rules here ----------------
				throw std::invalid_argument("[LocationConfig] Invalid return");
			_return[code] = (++it)->content;
			it++;
			if (it == _tokens.end() || it->type != SEMICOLON)
				throw std::invalid_argument("[ServerConfig] error_page (semicolon)");
		}
	}
	if (it->type == CLOSE_BRACE && it->level == _currentLevel)
		it++;
}

LocationConfig::~LocationConfig(void)
{

}

void	LocationConfig::PrintLocation(void)
{
	std::cout << YELLOW ;
	std::cout << "			Path : " << _path << std::endl;
	std::cout << "			Allowed method :" << std::endl;
	for (std::vector<std::string>::iterator it = _allowMethod.begin() ; it != _allowMethod.end() ; it++)
	{
		std::cout << "				- " << *it << std::endl;
	}
	std::cout << "			Auto Index : " << _autoIndex << std::endl;
	std::cout << "			Upload path : " << _uploadPath << std::endl;
	std::cout << "			CGI Extension : " << _cgiExtension << std::endl;
	std::cout << "			CGI Path : " << _cgiPath << std::endl;
	std::cout << "			Root : " << _root << std::endl;
	std::cout << "			Client body size max : " << _clientMaxBodySize << std::endl;
	std::cout << "			Return :" << std::endl;
	for (std::map< int , std::string >::iterator it = _return.begin() ; it != _return.end() ; it++)
	{
		std::cout << "				- " << it->first << " | " << it->second << std::endl;
	}
	std::cout << RESET;
}