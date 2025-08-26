#include "ServerConfig.hpp"
#include "Utils.hpp"

void	ServerConfig::ParseListenPort(std::vector< t_token>::iterator &it)
{
	char		*end = NULL;
	std::string	IP;
	uint16_t	port;
	std::string	content;

	size_t find = ((++it)->content).find(':', 0);
	if (find <= it->content.size())
	{
		IP = it->content.substr(0, find);
		content = it->content.substr(find + 1, it->content.size());
	}
	else
	{
		IP = "localhost";
		content = it->content;
	}

	double port_d = std::strtod(content.c_str(), &end);
	if (*end || port_d < 1 || port_d > 65535 || std::isinf(port_d))
		ThrowErrorToken(" Invalid port", *it);
	port = static_cast<int>(port_d);
	for (std::vector<ServerConfig>::iterator itServ = _serversConfig.begin(); itServ != _serversConfig.end(); ++itServ)
	{
		for (std::map<uint16_t , std::string>::iterator itPort = itServ->_portAndIP.begin() ; itPort != itServ->_portAndIP.end() ; itPort++)
		{
			if (itPort->first == port && itPort->second == IP)
				ThrowErrorToken(" Port already used", *it);
		}
	}
	_portAndIP[port] = IP;
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
