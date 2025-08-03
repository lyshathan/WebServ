#include "ServerConfig.hpp"
#include "LocationConfig.hpp"
#include "../tokenizer/Token.hpp"


////////////////////////////////////////////////////////////////////////////////////
//								Constructor & Destructor
////////////////////////////////////////////////////////////////////////////////////

LocationConfig::LocationConfig(std::vector<t_token> &tokenList, std::vector< t_token>::iterator &it): _tokens(tokenList), _currentLevel(it->level), _clientMaxBodySize(0)
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
	std::cout << YELLOW << "		-- CONFIGURING A LOCATION --" << RESET << std::endl;

	_path = (it+=2)->content;

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
			AddToVector(_allowMethod, it);
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