#include "Server.hpp"
#include "../parsing/Client.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

Server::Server(): _listenBackLog(10)
{
	std::cout << "---- SERVER ----" << std::endl;

	struct ServerConfig		conf1;
	conf1._conf_port = 4242;
	conf1._conf_host = "localhost";
	conf1._conf_address = INADDR_LOOPBACK; // = localhost
	_serverConfigs.push_back(conf1);


	struct ServerConfig		conf2;
	conf2._conf_port = 6060;
	conf2._conf_host = "localhost";
	conf2._conf_address = INADDR_LOOPBACK; // = localhost
	_serverConfigs.push_back(conf2);

// For translate hostname to uint32 address format
	// conf2._conf_host = "example.com";
	// conf2._conf_address = FromHostToAddress(conf2._conf_host);
	// struct in_addr addr_struct;
	// addr_struct.s_addr = htonl(conf2._conf_address);
	// printf("Address in good format: %s\n", inet_ntoa(addr_struct));


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
	for (size_t i = 0 ; i < _serverConfigs.size() ; i++)
	{
		struct sockaddr_in	socketAddress;
		int					opt = 1;
		int					serverFd;

		// Setup address and port of server socket
		std::memset(&socketAddress, 0, sizeof(socketAddress));
		socketAddress.sin_family = AF_INET; // IPv4
		socketAddress.sin_addr.s_addr = htonl(_serverConfigs[i]._conf_address);
		socketAddress.sin_port = htons(_serverConfigs[i]._conf_port);

		// Creation of socket
		serverFd = socket(socketAddress.sin_family, SOCK_STREAM, 0);
		if (serverFd == -1)
			return (HandleFunctionError("'socket'"));

		if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
			return (HandleFunctionError("'setsockopt(SO_REUSEADDR)'"));

		// Bind Server socket with address and port
		if ( bind(serverFd, (struct sockaddr *)&socketAddress, sizeof(socketAddress)) == -1)
			return (HandleFunctionError("'bind'"));

		std::cout << BLUE << "[Server] Server socket #" << serverFd << " created and bound to port [" << _serverConfigs[i]._conf_port << "]" << RESET << std::endl;

		_serverFds.push_back(serverFd);
	}
	return(1);
}

int Server::SetupListen(void)
{
	for (size_t i = 0 ; i < _serverFds.size() ; i++)
	{
		if (listen(_serverFds[i], _listenBackLog) == -1)
			return (HandleFunctionError("Listen"));
		std::cout << BLUE << "[Server] Listening on port : " << _serverConfigs[i]._conf_port << RESET << std::endl;
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
	for (size_t i = 0 ; i < _serverFds.size() ; i++)
	{
		std::cout << GREEN << "[Server] Setup PollFds : " << _pollFds[i].fd << " on port [" << _serverConfigs[i]._conf_port << "]" << RESET << std::endl;
	}
}