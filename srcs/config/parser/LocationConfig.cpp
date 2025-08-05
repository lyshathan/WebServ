#include "ServerConfig.hpp"
#include "LocationConfig.hpp"

////////////////////////////////////////////////////////////////////////////////////
//								Constructor & Destructor
////////////////////////////////////////////////////////////////////////////////////

LocationConfig::LocationConfig(std::vector<t_token> &tokenList, std::vector< t_token>::iterator &it, std::vector< LocationConfig > &locations): _locations(locations), _tokens(tokenList), _currentLevel(it->level), _clientMaxBodySize(0)
{
	LocationConfigParser(it);
}

LocationConfig::~LocationConfig(void)
{

}

////////////////////////////////////////////////////////////////////////////////////
//										Methods
////////////////////////////////////////////////////////////////////////////////////


void	LocationConfig::LocationConfigParser(std::vector< t_token>::iterator &it)
{
	it++;
	_path = (it++)->content;
	for (std::vector< LocationConfig >::iterator it = _locations.begin() ; it != _locations.end() ; it++)
	{
		if (it->_path == this->_path)
			// ThrowError("[LocationConfig] Already existing location path at line : ", *it);
			throw std::invalid_argument("[LocationConfig] Already existing location path");
	}

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
			if ((it + 1)->content != "GET" && (it + 1)->content != "POST" && (it + 1)->content != "DELETE")
				throw std::invalid_argument("[LocationConfig] Invalid method");
			AddToVector(_allowMethod, it);
		}
		else if (it->type == DIRECTIVE && it->content == "autoindex")	// auto index
			ParseAutoIndex(it);
		else if (it->type == DIRECTIVE				// paths
			&& (it->content == "upload_path" || it->content == "cgi_extension" || it->content == "cgi_path" || it->content == "root"))
				ParsePath(it);
		else if (it->type == DIRECTIVE && it->content == "client_max_body_size")	// client_max_body_size
			ParseClientMaxBodySize(it);
		else if (it->type == DIRECTIVE && it->content == "return")	// return
			ParseReturn(it);
	}
	if (it->type == CLOSE_BRACE && it->level == _currentLevel)
		it++;
}

void	LocationConfig::PrintLocation(void)
{
	std::string indent = "|	|	|___ ";
	std::string list = "|	|	|	- ";

	std::cout << "|	|" << std::endl;
	std::cout << "|	|" << YELLOW << "==== LOCATION ====" << RESET << std::endl;
	std::cout << indent << "Path : " << _path << std::endl;
	std::cout << indent << "Allowed method :" << std::endl;
	for (std::vector<std::string>::iterator it = _allowMethod.begin() ; it != _allowMethod.end() ; it++)
	{
		std::cout << list << *it << std::endl;
	}
	std::cout << indent << "Auto Index : " << _autoIndex << std::endl;
	std::cout << indent << "Upload path : " << _uploadPath << std::endl;
	std::cout << indent << "CGI Extension : " << _cgiExtension << std::endl;
	std::cout << indent << "CGI Path : " << _cgiPath << std::endl;
	std::cout << indent << "Root : " << _root << std::endl;
	std::cout << indent << "Client body size max : " << _clientMaxBodySize << std::endl;
	std::cout << indent << "Return :" << std::endl;
	for (std::map< int , std::string >::iterator it = _return.begin() ; it != _return.end() ; it++)
	{
		std::cout << list << it->first << " | " << it->second << std::endl;
	}
}