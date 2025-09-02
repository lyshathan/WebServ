#include "Webserv.hpp"
#include "../parsing/Client.hpp"

////////////////////////////////////////////////////////////////////////////////////
//								Constructor & Destructor
////////////////////////////////////////////////////////////////////////////////////

Webserv::Webserv(Config const &config): _config(config), _serverConfigs(config.getServerConfig()), _listenBackLog(10)
{
	std::cout << "---- SERVER ----" << std::endl;

	convertPorts(config);

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


void	Webserv::convertPorts(Config const &config)
{
	const std::vector< ::ServerConfig >& serverConf = config.getServerConfig();
	for (std::vector< ::ServerConfig >::const_iterator it = serverConf.begin(); it != serverConf.end(); it++)
	{
		std::vector< int > intPorts = it->getListenPort();
		for (std::vector< int >::iterator iter = intPorts.begin() ; iter != intPorts.end() ; iter++)
		{
			uint16_t port = static_cast<u_int32_t>(*iter);
			_serverPorts.push_back(port);
		}
	}

}


int Webserv::createServerSocket()
{
	for (size_t servIndex = 0 ; servIndex < _serverConfigs.size() ; servIndex++)
	{
		const std::map< uint16_t, std::string>& portAndIP = _serverConfigs[servIndex].getPortAndIP();
		for (std::map< uint16_t, std::string>::const_iterator PortIt = portAndIP.begin() ; PortIt != portAndIP.end() ; PortIt++ )
		{
			struct sockaddr_in	socketAddress;
			int					opt = 1;
			int					serverFd;

			// Setup address and port of server socket
			std::memset(&socketAddress, 0, sizeof(socketAddress));
			socketAddress.sin_family = AF_INET; // IPv4
			if (PortIt->second == "localhost")
				socketAddress.sin_addr.s_addr = INADDR_ANY; // Accept connections from ANY IP address
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

			std::cout << BLUE << "[Server] Server socket #" << serverFd << " created and bound to port [" << PortIt->first << "]" << RESET << std::endl;

			_serverFds.push_back(serverFd);
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
		std::cout << BLUE << "[Server] Listening on port : " << _serverPorts[i] << RESET << std::endl;
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
	for (size_t i = 0 ; i < _serverPorts.size() ; i++)
	{
		std::cout << GREEN << "[Server] Setup PollFds : " << _pollFds[i].fd << " on port [" << _serverPorts[i] << "]" << RESET << std::endl;

		// for (std::vector<ServerConfig>::const_iterator itServer = _config.getServerConfig().begin(); itServer != _config.getServerConfig().end(); ++itServer) {
		// 	const std::vector<int>& ports = itServer->getListenPort();
		// 	for (std::vector<int>::const_iterator it = ports.begin(); it != ports.end(); ++it) {
		// 		if (*it == _serverPorts[i]) {
		// 			_portToConfig[_pollFds[i].fd] = &(*itServer);
		// 			std::cout << GREEN << "[Server] Port " << *it << " associated with server config at " << _pollFds[i].fd << RESET << std::endl;
		// 		}
		// 	}
		// }
	}
}