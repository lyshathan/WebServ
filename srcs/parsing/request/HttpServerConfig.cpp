#include "HttpRequest.hpp"

bool HttpRequest::pickServerConfig() {
	_server = NULL;

	const std::string host = _headers.find("host")->second;
	std::string searchedName = host.substr(0, host.find(':', 0));

	std::cout << "Pick right server config, looking for : " << std::endl;
	std::cout << "    - port : " << _serverInfo.first << "\n    - IP : " << _serverInfo.second << "\n    - server_name : " << searchedName << std::endl;

	const std::vector< ServerConfig > &serverList = _config.getServerConfig();  //
	std::vector< ServerConfig >::const_iterator itServer = serverList.begin();  //
	for (; itServer != serverList.end() ; ++itServer)                           // iterate through server list
	{
		const std::map< uint16_t, std::string> portAndIPMap = itServer->getPortAndIP();
		std::map< uint16_t, std::string>::const_iterator itPortAndIP = portAndIPMap.begin();
		for (; itPortAndIP != portAndIPMap.end() ; ++itPortAndIP)    // iterate through map of Port+IP
		{
			// std::cout << "Check for : " << std::endl;
			// std::cout << "  - port : " << itPortAndIP->first << "\n  - IP : " << itPortAndIP->second << std::endl;
			if (itPortAndIP->first == _serverInfo.first && itPortAndIP->second == _serverInfo.second)
			{
				// Port and IP are matching !
				// std::cout << GREEN << "Port and IP are matching !" << RESET << std::endl;
				// std::cout << "Check for server_name : " << std::endl;
				if (_server == NULL)
					_server = _server = &(*itServer);
				const std::vector<std::string> serverNames = itServer->getServerName();
				std::vector<std::string>::const_iterator foundServerName = std::find(serverNames.begin(), serverNames.end(), searchedName);
				if (foundServerName != serverNames.end())
				{
					// Server Name is also matching !
					std::cout << GREEN << "Server config perfect match !   --> " << *foundServerName << RESET << std::endl;
					_server = &(*itServer);
					break;
				}
			}
		}
	}
	if (_server == NULL)
	{
		std::cout << GREEN << "Not matching any server config, use default one" << RESET << std::endl;
		const std::vector< ServerConfig > &config = _config.getServerConfig();
		_server = &(*config.begin());
	}
	_server->printServer();

	return false;
}