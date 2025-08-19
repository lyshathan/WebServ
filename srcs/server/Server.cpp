#include "Server.hpp"
#include "../parsing/Client.hpp"


#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

////////////////////////////////////////////////////////////////////////////////////
//								Constructor & Destructor
////////////////////////////////////////////////////////////////////////////////////

Server::Server(Config const &config): _listenBackLog(10)
{
	std::cout << "---- SERVER ----" << std::endl;

	convertPorts(config);

	if (CreateServerSocket() < 0)
		return ;

	if (SetupListen() < 0)
		return ;

	SetupPollServer();

	RunningServ();
}

Server::~Server()
{
	std::cout << RED << "Server destructor calling" << RESET << std::endl;
}

////////////////////////////////////////////////////////////////////////////////////
//										Methods
////////////////////////////////////////////////////////////////////////////////////

void	Server::convertPorts(Config const &config)
{
	const std::vector< ::ServerConfig >& serverConf = config.GetServerConfig();
	for (std::vector< ::ServerConfig >::const_iterator it = serverConf.begin(); it != serverConf.end(); it++)
	{
		std::vector< int > intPorts = it->getListenPort();
		for (std::vector< int >::iterator iter = intPorts.begin() ; iter != intPorts.end() ; iter++)
		{
			uint16_t port = static_cast<u_int32_t>(*iter);
			_serverPorts.push_back(port);
			std::cout << port << std::endl;
		}
	}

}

uint32_t	Server::FromHostToAddress(std::string hostname)
{
	struct addrinfo		hints;
	struct addrinfo		*result;
	uint32_t			finalAddress = 0;

	// Initialize hints
	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;			// IPv4
	hints.ai_socktype = SOCK_STREAM;	// TCP socket

	int status = getaddrinfo(hostname.c_str(), NULL, &hints, &result);
	if (status != 0)
	{
		std::cerr << "[server] ERROR : getaddrinfo error ( " << strerror(errno) << " )" << RESET << std::endl;
		return (0);
	}
	
	// Loop through results nested list until first IPv4
	struct addrinfo		*ptr;
	for (ptr = result ; ptr != NULL ; ptr=ptr->ai_next)
	{
		if (ptr->ai_family == AF_INET)
		{
			struct sockaddr_in* validSocketAddr = (struct sockaddr_in*)ptr->ai_addr;
			finalAddress = ntohl(validSocketAddr->sin_addr.s_addr);
			break;
		}
	}
	freeaddrinfo(result);
	return(finalAddress);
}

int Server::CreateServerSocket()
{
	for (size_t i = 0 ; i < _serverPorts.size() ; i++)
	{
		struct sockaddr_in	socketAddress;
		int					opt = 1;
		int					serverFd;

		// Setup address and port of server socket
		std::memset(&socketAddress, 0, sizeof(socketAddress));
		socketAddress.sin_family = AF_INET; // IPv4
		socketAddress.sin_addr.s_addr = INADDR_ANY; //Accept connections from ANY IP address
		socketAddress.sin_port = htons(_serverPorts[i]);

		// Creation of socket
		serverFd = socket(socketAddress.sin_family, SOCK_STREAM, 0);
		if (serverFd == -1)
			return (HandleFunctionError("'socket'"));
		
		if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
			return (HandleFunctionError("'setsockopt(SO_REUSEADDR)'"));

		// Bind Server socket with address and port
		if ( bind(serverFd, (struct sockaddr *)&socketAddress, sizeof(socketAddress)) == -1)
			return (HandleFunctionError("'bind'"));

		std::cout << BLUE << "[Server] Server socket #" << serverFd << " created and bound to port [" << _serverPorts[i] << "]" << RESET << std::endl;

		_serverFds.push_back(serverFd);
	}
	return(1);
}

int Server::SetupListen(void)
{
	for (size_t i = 0 ; i < _serverPorts.size() ; i++)
	{
		if (listen(_serverFds[i], _listenBackLog) == -1)
			return (HandleFunctionError("Listen"));
		std::cout << BLUE << "[Server] Listening on port : " << _serverPorts[i] << RESET << std::endl;
	}
	return (1);
}

void Server::SetupPollServer(void)
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
	}
}