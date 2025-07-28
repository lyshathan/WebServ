#include "Server.hpp"
#include "../parsing/Client.hpp"

Server::Server() : _port(4242), _address(INADDR_LOOPBACK), _status(0), _serverFd(-1), _listenBackLog(10)
{
	std::cout << "---- SERVER ----" << std::endl;

	CreateServerSocket();
	if (_serverFd < 0)
		return ;

	SetupListen();
	if (_serverFd < 0)
		return ;

	// Set server as the first element of PollFds
	std::cout << GREEN << "Server fd  [" << _serverFd << "]" << std::endl;
	struct pollfd	ServerPollFd;
	ServerPollFd.fd = _serverFd;
	ServerPollFd.events = POLLIN;
	ServerPollFd.revents = 0;
	_pollFds.push_back(ServerPollFd);
	std::cout << BLUE << "[Server] Setup PollFds : " << _port << RESET << std::endl;
	

	RunningServ();
}

Server::~Server()
{
	for (std::vector<struct pollfd>::iterator it = _pollFds.begin(); it != _pollFds.end() ; it++)
	{
		if (it->fd != - 1)
		{
			close(it->fd);
			it->fd = -1;
		}
	}
}

void Server::CreateServerSocket(void)
{
	struct sockaddr_in	socketAddress;
	int					status;
	int					opt = 1;

	// Setup address and port of server socket
	std::memset(&socketAddress, 0, sizeof(socketAddress));
	socketAddress.sin_family = AF_INET; // IPv4
	socketAddress.sin_addr.s_addr = htonl(_address);
	socketAddress.sin_port = htons(_port);

	// Creation of socket
	_serverFd = socket(socketAddress.sin_family, SOCK_STREAM, 0);
	if (_serverFd == -1)
	{
		std::cerr << RED << "[Server] ERROR : function `socket` failed." << RESET << std::endl;
		return;
	}
	std::cout << BLUE << "[Server] Server socket created, fd : " << _serverFd << RESET << std::endl;
	
	if (setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		std::cerr << RED << "[Server] ERROR : setsockopt(SO_REUSEADDR) failed" << RESET << std::endl;
		_serverFd = -2;
		return;
	}

	// Bind Server socket with address and port
	status = bind(_serverFd, (struct sockaddr *)&socketAddress, sizeof(socketAddress));
	if (status == -1)
	{
		std::cerr << RED << "[server] ERROR : Bind error ( " << strerror(errno) << " )" << RESET << std::endl;
		_serverFd = -2;
		return;
	}
	std::cout << BLUE << "[Server] Bound Server socket to port : " << _port << RESET << std::endl;
}

void Server::SetupListen(void)
{
	int	status;

	status = listen(_serverFd, _listenBackLog);
	if (status == -1)
	{
		std::cerr << RED << "[server] ERROR : Listen error ( " << strerror(errno) << " )" << RESET << std::endl;
		_serverFd = -2;
		return;
	}
	std::cout << BLUE << "[Server] Listening on port : " << _port << RESET << std::endl;
}

