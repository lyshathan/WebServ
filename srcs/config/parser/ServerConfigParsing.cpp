#include "ServerConfig.hpp"
#include "LocationConfig.hpp"
#include "../tokenizer/Token.hpp"

void	ServerConfig::ParseListenPort(std::vector< t_token>::iterator &it)
{
	char *end;

	double port = std::strtod((++it)->content.c_str(), &end);
	if (*end || port <= 0 || port > INT_MAX)
		throw std::invalid_argument("[ServerConfig] Invalid port");
	_listenPorts.push_back(port);
	CheckForSemicolon("listen", it);
}

void	ServerConfig::ParseClientMaxBodySize(std::vector< t_token>::iterator &it)
{
	char *end;

	double bodySize = std::strtod((++it)->content.c_str(), &end);
	if (bodySize <= 0 || bodySize > INT_MAX)
		throw std::invalid_argument("[ServerConfig] Invalid client_max_body_size");
	_clientMaxBodySize = bodySize;
	CheckForSemicolon("client_max_body_size", it);
}

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

void	ServerConfig::ParseErrorPage(std::vector< t_token>::iterator &it)
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
	CheckForSemicolon("error_page", it);
}


void	ServerConfig::AddToVector(std::vector< std::string > &vec, std::vector< t_token>::iterator &it)
{
	std::ostringstream				errorMsg;
	std::vector< t_token>::iterator	tmp = it;

	it++;
	while (it != _tokens.end() &&  it->type == VALUE)
	{
		vec.push_back(it->content);
		it++;
	}
	if (it == _tokens.end() || it->type != SEMICOLON)
	{
		errorMsg << "[ServerConfig] Invalid " << tmp->content << "(semicolon)";
		throw std::invalid_argument(errorMsg.str());
	}
}

void	ServerConfig::CheckForSemicolon(std::string type, std::vector< t_token>::iterator &it)
{
	std::ostringstream				errorMsg;
	std::vector< t_token>::iterator	tmp = it;

	it++;
	if (it == _tokens.end() || it->type != SEMICOLON)
	{
		errorMsg << "[ServerConfig] Invalid " << type << "(semicolon)";
		throw std::invalid_argument(errorMsg.str());
	}
}