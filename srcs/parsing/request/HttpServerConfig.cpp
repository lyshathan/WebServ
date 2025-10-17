#include "HttpRequest.hpp"
#include "../../ProjectTools.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void HttpRequest::pickServerConfig() {
	_server = NULL;
	t_servState	state = NO_MATCH;

	struct sockaddr_in	serverAddr;
	socklen_t			addrLen = sizeof(serverAddr);
	getsockname(_clientfd, (struct sockaddr*)&serverAddr, &addrLen);
	uint32_t serverIP = ntohl(serverAddr.sin_addr.s_addr);
	std::string serverIPstr = inet_ntoa(serverAddr.sin_addr);
	uint16_t serverPort = ntohs(serverAddr.sin_port);

	const std::string host = _headers.find("host")->second;
	std::string searchedName = host.substr(0, host.find(':', 0));

	const std::vector< ServerConfig > &serverList = _config.getServerConfig();
	std::vector< ServerConfig >::const_iterator itServer = serverList.begin();
	for (; (itServer != serverList.end() && state != EXACT_MATCH) ; ++itServer)
	{
		const std::map< uint16_t, std::string> portAndIPMap = itServer->getPortAndIP();
		std::map< uint16_t, std::string>::const_iterator itPortAndIP = portAndIPMap.begin();
		for (; (itPortAndIP != portAndIPMap.end() && state != EXACT_MATCH) ; ++itPortAndIP)
		{
			uint32_t configIP = fromIPToIntHost(itPortAndIP->second);

			if (itPortAndIP->first == serverPort)
			{
				if (configIP == serverIP || itPortAndIP->second == "0.0.0.0")
				{
					const std::vector<std::string> serverNames = itServer->getServerName();
					std::vector<std::string>::const_iterator foundServerName = std::find(serverNames.begin(), serverNames.end(), searchedName);
					if (foundServerName != serverNames.end() && configIP == serverIP)
					{
						state = EXACT_MATCH;
						_server = &(*itServer);
					}
					else if (foundServerName != serverNames.end() && itPortAndIP->second == "0.0.0.0")
					{
						state = EXACT_MATCH_DEFAULT_IP;
						_server = &(*itServer);
					}
					else if (configIP == serverIP && state < MATCH_PORT_IP)
					{
						state = MATCH_PORT_IP;
						_server = &(*itServer);
					}
					else if (state < MATCH_PORT_DEFAULT_IP)
					{
						state = MATCH_PORT_DEFAULT_IP;
						_server = &(*itServer);
					}
				}
				else if (_server == NULL)
					_server = &(*itServer);
			}
		}
	}
	if (state == NO_MATCH)
	{
		const std::vector< ServerConfig > &config = _config.getServerConfig();
		_server = &(*config.begin());
	}
	std::stringstream ss;
	ss << _server->getID();
	printLog(PURPLE, "INFO", "Chosen server configuration : " + ss.str());

	// _server->printServer();
}


