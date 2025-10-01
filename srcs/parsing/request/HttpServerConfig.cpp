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
	std::string serverIPstr = inet_ntoa(serverAddr.sin_addr);  // "127.0.0.1"
	uint16_t serverPort = ntohs(serverAddr.sin_port);	//port

	const std::string host = _headers.find("host")->second;
	std::string searchedName = host.substr(0, host.find(':', 0));

	// std::cout << "Pick right server config, looking for : " << std::endl;
	// std::cout << "    - port : " << serverPort << "\n    - IP : " <<  serverIP << "\n    - server_name : " << searchedName << std::endl;


	const std::vector< ServerConfig > &serverList = _config.getServerConfig();  //
	std::vector< ServerConfig >::const_iterator itServer = serverList.begin();  //
	for (; (itServer != serverList.end() && state != EXACT_MATCH) ; ++itServer)                           // iterate through server list
	{
		const std::map< uint16_t, std::string> portAndIPMap = itServer->getPortAndIP();
		std::map< uint16_t, std::string>::const_iterator itPortAndIP = portAndIPMap.begin();
		for (; (itPortAndIP != portAndIPMap.end() && state != EXACT_MATCH) ; ++itPortAndIP)    // iterate through map of Port+IP
		{
			uint32_t configIP = fromIPToIntHost(itPortAndIP->second);
			// std::cout << "Check for : " << std::endl;
			// std::cout << "  - port : " << itPortAndIP->first << "\n  - IP : " << itPortAndIP->second << "(" << configIP << ")" << std::endl;

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
						// std::cout << GREEN << "Server config perfect match !" << RESET << std::endl;
					}
					else if (foundServerName != serverNames.end() && itPortAndIP->second == "0.0.0.0")
					{
						state = EXACT_MATCH_DEFAULT_IP;
						_server = &(*itServer);
						// std::cout << GREEN << "Set default server matching port AND server_name with DEFAULT IP" << RESET << std::endl;
					}
					else if (configIP == serverIP)
					{
						state = MATCH_PORT_IP;
						_server = &(*itServer);
						// std::cout << GREEN << "Set default server matching port AND IP " << _server->getID() <<  RESET << std::endl;
					}
					else if (state < MATCH_PORT_DEFAULT_IP)
					{
						state = MATCH_PORT_DEFAULT_IP;
						_server = &(*itServer);
						// std::cout << GREEN << "Set default server matching port AND DEFAULT IP " << _server->getID() <<  RESET << std::endl;
					}
				}
				else if (_server == NULL)
				{
					// state = MATCH_PORT;
					// std::cout << GREEN << "Set default server matching port" << RESET <<std::endl;

					_server = &(*itServer);
				}
			}
		}
	}
	if (state == NO_MATCH)
	{

		// std::cout << GREEN << "Not matching any server config, use first one as default" << RESET << std::endl;

		const std::vector< ServerConfig > &config = _config.getServerConfig();
		_server = &(*config.begin());
	}
	// std::cout << GREEN << "--> Chosen server config : " << _server->getID() << RESET << std::endl;
	// _server->printServer();

}


