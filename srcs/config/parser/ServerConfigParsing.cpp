#include "ServerConfig.hpp"
#include "Utils.hpp"

void	ServerConfig::ParseListenPort(std::vector< t_token>::iterator &it)
{
	char *end;

	double port = std::strtod((++it)->content.c_str(), &end);
	if (*end || port < 1 || port > 65535 || std::isinf(port))
		throw std::invalid_argument("[ServerConfig] Invalid port");
	for (std::vector< ServerConfig >::iterator itSC = _serversConfig.begin() ; itSC != _serversConfig.end() ; itSC++)
	{
		if (std::find((itSC->_listenPorts).begin(), (itSC->_listenPorts).end(), port) != (itSC->_listenPorts).end())
		{
			std::ostringstream errorMsg;
			errorMsg << "[ServerConfig] Port already exists : " << port;
			throw std::invalid_argument(errorMsg.str());
		}
	}
	if (std::find(_listenPorts.begin(), _listenPorts.end(), port) == _listenPorts.end())
		_listenPorts.push_back(port);
	ACheckForSemicolon(it, _tokens);
}

void	ServerConfig::ParseRoot(std::vector< t_token>::iterator &it)
{
	size_t check = IsValidDirPath((++it)->content);
	if (check != VALID)
	{
		std::ostringstream errorMsg;
		errorMsg << "[ServerConfig] Error with root path : " << it->content;
		if (check == NO_EXIST)
		{
			errorMsg << " : Path does not exist.";
			throw std::invalid_argument(errorMsg.str());
		}
		else if  (check == NOT_A_DIRECTORY)
		{
			errorMsg << " : Directory does not exist.";
			throw std::invalid_argument(errorMsg.str());
		}
		errorMsg << " : Directory permission denied.";
		throw std::invalid_argument(errorMsg.str());
	}
	_root = it->content;
	ACheckForSemicolon(it, _tokens);
}


void	ServerConfig::ParseClientMaxBodySize(std::vector< t_token>::iterator &it)
{
	char *end;

	double bodySize = std::strtod((++it)->content.c_str(), &end);
	if (*end || bodySize <= 0 || bodySize > INT_MAX || std::isinf(bodySize))
		throw std::invalid_argument("[ServerConfig] Invalid client_max_body_size");
	_clientMaxBodySize = bodySize;
	ACheckForSemicolon(it, _tokens);
}

size_t	CountErrorCodes(std::vector<t_token> &tokenList, std::vector< t_token>::iterator it)
{
	size_t	count = 0;

	while (it != tokenList.end() && it->type == VALUE)
	{
		// std::cout << it->content << std::endl;
		it++;
		count++;
	}
	return (count);
}

void	ServerConfig::ParseErrorPage(std::vector< t_token>::iterator &it)
{
	char *end;
	size_t	check;

	it++;
	size_t	nbOfErrorCode = CountErrorCodes(_tokens, it);
	std::vector< t_token>::iterator itErrorPage = it + nbOfErrorCode - 1;
	check = IsValidFile(itErrorPage->content);
	if (check != VALID)
	{
		std::ostringstream errorMsg;
		errorMsg << "[ServerConfig] Error with Error file : " << itErrorPage->content;
		if (check == NO_EXIST)
		{
			errorMsg << " : Error file does not exist.";
			throw std::invalid_argument(errorMsg.str());
		}
		errorMsg << " : Error file permission denied.";
		throw std::invalid_argument(errorMsg.str());
	}
	while (it != itErrorPage)
	{
		double code = std::strtod(it->content.c_str(), &end);
		if (*end || code < 0 || code > INT_MAX)					// Need to validate the rules here ----------------
			throw std::invalid_argument("[ServerConfig] Invalid error_page");
		_errorPages[code] = itErrorPage->content;
		it++;
	}
	ACheckForSemicolon(it, _tokens);
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
