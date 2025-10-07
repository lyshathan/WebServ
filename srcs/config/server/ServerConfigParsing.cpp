#include "ServerConfig.hpp"
#include "../utils/Utils.hpp"
#include "../../ProjectTools.hpp"

void	ServerConfig::parseListenPort(std::vector< t_token>::iterator &it)
{
	char		*end = NULL;
	std::string	IP;
	uint16_t	port;
	std::string	content;

	size_t find = ((++it)->content).find(':', 0);
	if (find != std::string::npos)
	{
		IP = it->content.substr(0, find);
		if (IP == "localhost")
			IP = "127.0.0.1";
		content = it->content.substr(find + 1, it->content.size());
	}
	else
	{
		IP = "0.0.0.0";
		content = it->content;
	}

	// Validate IP format (must have exactly 4 octets between 0-255)
	try {
		fromIPToIntHost(IP);
	} catch (const std::runtime_error& e) {
		throwErrorToken(" Invalid IP address format", *it);
	}

	double port_d = std::strtod(content.c_str(), &end);
	if (*end || port_d < 1 || port_d > 65535 || std::isinf(port_d))
		throwErrorToken(" Invalid port", *it);
	port = static_cast<int>(port_d);
	_portAndIP[port] = IP;
	_listenPorts.push_back(port);
	checkForSemicolon(it, _tokens);
}

void	ServerConfig::parseRoot(std::vector< t_token>::iterator &it)
{
	if (!_root.empty())
		throwErrorToken(" Root is already set", *it);
	_root = (++it)->content;
	checkForSemicolon(it, _tokens);
}

void	ServerConfig::parseErrorPage(std::vector< t_token>::iterator &it)
{
	char *end;

	it++;
	size_t	nbOfErrorCode = CountErrorCodes(_tokens, it);
	std::vector< t_token>::iterator itErrorPage = it + nbOfErrorCode - 1;
	while (it != itErrorPage)
	{
		double code_d = std::strtod(it->content.c_str(), &end);
		if (*end || code_d < 300 || code_d > 599)
			throwErrorToken(" Invalid error code", *it);
		int code = static_cast<int>(code_d);
		if (_errorPages.find(code) != _errorPages.end())
			throwErrorToken(" Already existing code", *it);
		_errorPages[code] = itErrorPage->content;
		it++;
	}
	checkForSemicolon(it, _tokens);
}
