
#include "Webserv.hpp"
#include "../parsing/Client.hpp"
#include "../ProjectTools.hpp"

////////////////////////////////////////////////////////////////////////////////////
//								Constructor & Destructor
////////////////////////////////////////////////////////////////////////////////////

Webserv::Webserv(Config const &config): _config(config), _serverConfigs(config.getServerConfig()), _listenBackLog(100)
{
	if (createServerSocket() < 0)
		return ;
	if (setupListen() < 0)
		return ;
	setupPollServer();

	runningServ();
}

Webserv::~Webserv() {
	cleanServer();
}

////////////////////////////////////////////////////////////////////////////////////
//										Methods
////////////////////////////////////////////////////////////////////////////////////

int Webserv::createServerSocket()
{
	for (size_t servIndex = 0 ; servIndex < _serverConfigs.size() ; servIndex++)
	{
		const std::map< uint16_t, std::string>& portAndIP = _serverConfigs[servIndex].getPortAndIP();
		for (std::map< uint16_t, std::string>::const_iterator PortIt = portAndIP.begin() ; PortIt != portAndIP.end() ; PortIt++ )
		{

			if (socketAlreadyExists(PortIt->first, PortIt->second))
				continue;

			struct sockaddr_in	socketAddress;
			int					opt = 1;
			int					serverFd;

			// Setup address and port of server socket
			std::memset(&socketAddress, 0, sizeof(socketAddress));
			socketAddress.sin_family = AF_INET;
			if (PortIt->second == "localhost")
				socketAddress.sin_addr.s_addr = ::fromIPToIntNetwork("127.0.0.1");
			else
				socketAddress.sin_addr.s_addr = ::fromIPToIntNetwork(PortIt->second); // Convert IP string to int address

			socketAddress.sin_port = htons(PortIt->first);

			// Creation of socket
			serverFd = socket(socketAddress.sin_family, SOCK_STREAM, 0);
			if (serverFd == -1)
				return (handleFunctionError("'socket'"));

			if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
				return (handleFunctionError("'setsockopt(SO_REUSEADDR)'"));

			// Bind Server socket with address and port
			if ( bind(serverFd, (struct sockaddr *)&socketAddress, sizeof(socketAddress)) == -1)
				return (handleFunctionError("'bind'"));

			_serverFds.push_back(serverFd);


			std::pair<uint16_t, std::string> info;
			info.first = PortIt->first;
			info.second = PortIt->second;
			_serverInfos[serverFd] = info;
			std::stringstream ss;
			ss << info.first;
			printLog(BLUE, "INFO", "Server Created: Host [" + info.second + "] Port [" + ss.str() + "]");
		}
	}
	return(1);
}


int Webserv::setupListen(void)
{
	for (size_t i = 0 ; i < _serverFds.size() ; i++)
	{
		if (listen(_serverFds[i], _listenBackLog) == -1)
			return (handleFunctionError("Listen"));
		std::map< int, std::pair< uint16_t, std::string> >::iterator find = _serverInfos.find(_serverFds[i]);
		std::stringstream ss;
		ss << find->second.first;
		printLog(BLUE, "INFO", "Listening on: " + find->second.second + ":" + ss.str());
	}
	return (1);
}

void Webserv::setupPollServer()
{
	struct pollfd	ServerPollFd;

	for (size_t i = 0 ; i < _serverFds.size() ; i++)
	{
		ServerPollFd.fd = _serverFds[i];
		ServerPollFd.events = POLLIN;
		ServerPollFd.revents = 0;
		_pollFds.push_back(ServerPollFd);
	}

}


bool	Webserv::socketAlreadyExists(const uint16_t &port, const std::string &IP) const
{
	// Normalize localhost to 127.0.0.1 for comparison
	std::string normalizedIP = IP;
	if (IP == "localhost")
		normalizedIP = "127.0.0.1";

	// Check for already existing IP AND port 
	std::map< int, std::pair< uint16_t, std::string> >::const_iterator it = _serverInfos.begin();
	for (; it != _serverInfos.end() ; ++it)
	{
		std::string existingIP = it->second.second;
		if (existingIP == "localhost")
			existingIP = "127.0.0.1";

		// In case IP:port already exists, skip
		if (it->second.first == port && existingIP == normalizedIP)
		{
			std::stringstream ss;
			ss << "[" << IP << ":" << port<< "] already existing, skipping config.";
			printLog(BLUE, "INFO", ss.str());
			return (true);
		}

		// In case 0.0.0.0:port already set, skip
		if (it->second.first == port && it->second.second == "0.0.0.0" && normalizedIP != "0.0.0.0")
		{
			std::stringstream ss;
			ss << "0.0.0.0:" << port << " already exists. Skipping [" << IP << ":" << port<< "]";
			printLog(BLUE, "INFO", ss.str());
			return (true);
		}
	}

	if (normalizedIP != "0.0.0.0")
	{
		for (size_t servIndex = 0 ; servIndex < _serverConfigs.size() ; servIndex++)
		{
			const std::map< uint16_t, std::string>& portAndIP = _serverConfigs[servIndex].getPortAndIP();
			for (std::map< uint16_t, std::string>::const_iterator itPort = portAndIP.begin() ; itPort != portAndIP.end() ; itPort++ )
			{
				// In case 0.0.0.0:port exists later in config file, skip
				if (itPort->first == port && itPort->second == "0.0.0.0")
				{
					std::stringstream ss;
					ss << "A more global server config [0.0.0.0:" << port << "] exists in config. Skipping [" << IP << ":" << port<< "]";
					printLog(BLUE, "INFO", ss.str());
					return (true);
				}
			}
		}
	}
	return (false);
}
