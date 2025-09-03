#include "Webserv.hpp"
#include "../parsing/Client.hpp"

////////////////////////////////////////////////////////////////////////////////////
//								Constructor & Destructor
////////////////////////////////////////////////////////////////////////////////////

Webserv::Webserv(Config const &config): _config(config), _serverConfigs(config.getServerConfig()), _listenBackLog(10)
{
	std::cout << "---- SERVER ----" << std::endl;

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
	std::cout << BLUE << "[Server] Create and bind server sockets :" << RESET << std::endl;
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
				socketAddress.sin_addr.s_addr = fromIPToInt("127.0.0.1"); // Specific loopback localhost
			else
				socketAddress.sin_addr.s_addr = fromIPToInt(PortIt->second); // Convert IP string to int address

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
			//std::cout << "fd = " << serverFd << "	|	port = " << info.first << "	|	IP = " << info.second << std::endl;
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

		std::map< int, std::pair< uint16_t, std::string> >::iterator find = _serverInfos.find(_serverFds[i]);
		std::cout << GREEN << "[Server] Server fd added to _pollFds : " << "[" << _serverFds[i] << "] > "<< find->second.second << ":" << find->second.first << RESET << std::endl;
	}

}

bool	Webserv::socketAlreadyExists(const uint16_t &port, const std::string &IP) const
{
	std::map< int, std::pair< uint16_t, std::string> >::const_iterator it = _serverInfos.begin();
	for (; it != _serverInfos.end() ; ++it)
	{
		if (it->second.first == port && it->second.second == IP)
			return (true);
	}
	return (false);
}
