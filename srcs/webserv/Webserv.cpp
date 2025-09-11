
#include "Webserv.hpp"
#include "../parsing/Client.hpp"
#include "../ProjectTools.hpp"


////////////////////////////////////////////////////////////////////////////////////
//								Constructor & Destructor
////////////////////////////////////////////////////////////////////////////////////

Webserv::Webserv(Config const &config): _config(config), _serverConfigs(config.getServerConfig()), _listenBackLog(10)
{
	std::cout << "---- SERVER ----" << std::endl;

	// initServerInfo();

	if (createServerSocket() < 0)
		return ;


	if (setupListen() < 0)
		return ;

	setupPollServer();

	runningServ();
}

Webserv::~Webserv()
{
	std::cout << RED << "Webserv destructor calling" << RESET << std::endl;
}

////////////////////////////////////////////////////////////////////////////////////
//										Methods
////////////////////////////////////////////////////////////////////////////////////

int Webserv::createServerSocket()
{
	// std::cout << BLUE << "[Server] Create and bind server sockets :" << RESET << std::endl;
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
			socketAddress.sin_family = AF_INET; // IPv4
			if (PortIt->second == "localhost")
				socketAddress.sin_addr.s_addr = ::fromIPToIntNetwork("127.0.0.1"); // Specific loopback localhost
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


			//--------------NEW PART-----------------//
			std::pair<uint16_t, std::string> info;
			info.first = PortIt->first;
			info.second = PortIt->second;
			_serverInfos[serverFd] = info;
			// std::cout << "fd = " << serverFd << "	|	port = " << info.first << "	|	IP = " << info.second << std::endl;
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
		std::cout << BLUE << "[Server] Listening on : " << find->second.second << ":" << find->second.first << RESET << std::endl;
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

		// std::map< int, std::pair< uint16_t, std::string> >::iterator find = _serverInfos.find(_serverFds[i]);
		// std::cout << PURPLE << "[Server] Server fd added to _pollFds : " << "[" << _serverFds[i] << "] > "<< find->second.second << ":" << find->second.first << RESET << std::endl;
	}

}


bool	Webserv::socketAlreadyExists(const uint16_t &port, const std::string &IP) const
{
	// Normalize localhost to 127.0.0.1 for comparison
	std::string normalizedIP = IP;
	if (IP == "localhost")
		normalizedIP = "127.0.0.1";

	// First check if this exact IP:port already exists in created sockets
	std::map< int, std::pair< uint16_t, std::string> >::const_iterator it = _serverInfos.begin();
	for (; it != _serverInfos.end() ; ++it)
	{
		std::string existingIP = it->second.second;
		if (existingIP == "localhost")
			existingIP = "127.0.0.1";

		if (it->second.first == port && existingIP == normalizedIP)
		{
			std::cout << YELLOW << "Host:Port (" << IP << ":" << port<< ") already existing, skipping server." << RESET << std::endl;
			return (true);
		}

		// If we already have 0.0.0.0:port, skip any specific IP:port
		if (it->second.first == port && it->second.second == "0.0.0.0" && normalizedIP != "0.0.0.0")
		{
			std::cout << YELLOW << "0.0.0.0:" << port << " already exists. Skipping Host:Port (" << IP << ":" << port<< ")" << RESET << std::endl;
			return (true);
		}
	}

	// For any non-0.0.0.0 binding, check if 0.0.0.0:port exists anywhere in the config
	// If it does, skip this specific binding in favor of the global one
	if (normalizedIP != "0.0.0.0")
	{
		for (size_t servIndex = 0 ; servIndex < _serverConfigs.size() ; servIndex++)
		{
			const std::map< uint16_t, std::string>& portAndIP = _serverConfigs[servIndex].getPortAndIP();
			for (std::map< uint16_t, std::string>::const_iterator itPort = portAndIP.begin() ; itPort != portAndIP.end() ; itPort++ )
			{
				if (itPort->first == port && itPort->second == "0.0.0.0")
				{
					std::cout << YELLOW << "A more global server config (0.0.0.0:" << port << ") exists in config. Skipping Host:Port (" << IP << ":" << port<< ")" << RESET << std::endl;
					return (true);
				}
			}
		}
	}

	return (false);
}
