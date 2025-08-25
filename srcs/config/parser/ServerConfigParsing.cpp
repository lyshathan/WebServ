#include "ServerConfig.hpp"
#include "Utils.hpp"

void	ServerConfig::ParseListenPort(std::vector< t_token>::iterator &it)
{
	char *end;

	double port_d = std::strtod((++it)->content.c_str(), &end);
	if (*end || port_d < 1 || port_d > 65535 || std::isinf(port_d))
		ThrowErrorToken(" Invalid port", *it);
	int port = static_cast<int>(port_d);
	if (std::find(_listenPorts.begin(), _listenPorts.end(), port) != _listenPorts.end())
		ThrowErrorToken(" Port already used", *it);
	for (std::vector<ServerConfig>::iterator itSC = _serversConfig.begin(); itSC != _serversConfig.end(); ++itSC)
	{
		if (std::find(itSC->_listenPorts.begin(), itSC->_listenPorts.end(), port) != itSC->_listenPorts.end())
			ThrowErrorToken(" Port already used", *it);
	}
	_listenPorts.push_back(port);
	ACheckForSemicolon(it, _tokens);
}

void	ServerConfig::ParseRoot(std::vector< t_token>::iterator &it)
{
	if (!_root.empty())
		ThrowErrorToken(" Root is already set", *it);
	size_t check = IsValidDirPath((++it)->content);
	if (check != VALID)
	{
		if (check == NO_EXIST)
			ThrowErrorToken(" Path does not exist", *it);
		else if  (check == NOT_A_DIRECTORY)
			ThrowErrorToken(" Directory does not exist", *it);
		ThrowErrorToken(" Directory permission denied", *it);
	}
	_root = it->content;
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
		if (check == NO_EXIST)
			ThrowErrorToken(" Error file does not exist", *it);
		ThrowErrorToken(" Error file permission denied", *it);
	}
	while (it != itErrorPage)
	{
		double code_d = std::strtod(it->content.c_str(), &end);
		if (*end || code_d < 0 || code_d > INT_MAX)					// Need to validate the rules here ----------------
			ThrowErrorToken(" Invalid error code", *it);
		int code = static_cast<int>(code_d);
		if (_errorPages.find(code) != _errorPages.end())
			ThrowErrorToken(" Already existing code", *it);
		_errorPages[code] = itErrorPage->content;
		it++;
	}
	ACheckForSemicolon(it, _tokens);
}
