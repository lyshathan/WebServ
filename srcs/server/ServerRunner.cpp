#include "Server.hpp"
#include "../parsing/Client.hpp"

void	Server::RunningServ(void)
{

	int	status;
	int	timeout = 3000;	// 3 seconds
	while (1)
	{
		// Check if any socket is ready, else wait
		status = poll(_pollFds.data(), _pollFds.size(), timeout);
		if (status == -1)
		{
			std::cerr << RED << "[server] ERROR : Poll error ( " << strerror(errno) << " )" << RESET << std::endl;
			_serverFd = -2;
			return;
		}
		else if (status == 0) // Is this condition util?
		{
			std::cout << "[Server] Waiting ..." << std::endl;
			continue;
		}

		// Loop check for each socket
		if (ConnectAndRead() < 0)
			return;
	}
}

int	Server::ConnectAndRead(void)
{
	int	status;

	for (std::vector<struct pollfd>::iterator it = _pollFds.begin() ; it != _pollFds.end() ; it++)
	{
		if (! (it->revents & POLLIN)) // Socket i is not ready for now
			continue;

		std::cout << BLUE << "[Server] Socket #" << it->fd << " is ready for I/O operation" << RESET << std::endl;

		it->revents = 0; // Reset revents to 0 so we can see if it has changed next time

		if (it->fd == _serverFd)
		{
			std::cout << "Accept new connection [" << it->fd << "]" << std::endl;
			status = AcceptNewConnection();
			if (status == -1)
			{
				_serverFd = -2;
				return (-1);
			}
			it = _pollFds.begin() + 1; // Restart the loop from the first client socket because of push back
		}
		else
		{
			std::cout << "Read data [" << it->fd << "]" << std::endl;
			status = ReadDataFromSocket(it);
			if (status == -1)
			{
				_serverFd = -2;
				return (-1);
			}
		}

	}
	return (1);
}

int Server::AcceptNewConnection(void)
{
	int			status;
	int			clientFd;

	clientFd = accept(_serverFd, NULL, NULL); // Do we need to get the port and addresss of the new client socket ?
	if (clientFd == -1)
	{
		std::cerr << RED << "[server] ERROR : Poll error ( " << strerror(errno) << " )" << RESET << std::endl;
		_serverFd = -2;
		return (-1);
	}

	// Add new client to pollFds and to _client map
	struct pollfd NewClientPollFd;
	NewClientPollFd.fd = clientFd;
	NewClientPollFd.events = POLLIN;
	NewClientPollFd.revents = 0;
	_pollFds.push_back(NewClientPollFd);
	_clients[clientFd] = new Client(clientFd);

	std::cout << BLUE << "[Server] Accept new conncetion on client socket : " << clientFd << RESET << std::endl;

	std::ostringstream Oss;
	Oss << "Welcome. You are client fd (" << clientFd << ")" << std::endl;
	std::string Message = Oss.str();

	status = send(clientFd, Message.c_str(), Message.length(), 0);
	if (status == -1)
	{
		std::cerr << RED << "[server] ERROR : Send error ( " << strerror(errno) << " ) to client #" << clientFd << RESET << std::endl;
		_serverFd = -2;
		return (-1);
	}
	return (0);
}

int Server::ReadDataFromSocket(std::vector<struct pollfd>::iterator & it)
{
	char	buffer[BUFSIZ + 1];
	int 	senderFd;
	int		bytesRead;
	//int		status;

	senderFd = it->fd;
	bytesRead = recv(senderFd, buffer, BUFSIZ, 0);
	if (bytesRead <= 0)
	{
		if (bytesRead == 0)
		{
			std::cerr << YELLOW << "[server] Client #" << senderFd << " closed connection" << RESET << std::endl;
			close(senderFd);
			it = _pollFds.erase(it) - 1;
		}
		else
		{
			std::cerr << RED << "[server] ERROR : Recv error ( " << strerror(errno) << " ) to client #" << senderFd << RESET << std::endl;
			_serverFd = -2;
			return (-1);
		}
	}
	else
	{
		if (!std::strncmp("stop", buffer, 4))
		{
			std::cout << GREEN << "Stopping server" << RESET << std::endl;
			return(-1);
		}


		buffer[bytesRead] = 0;
		std::cout << "Client #" << senderFd << " had a message [" << buffer << "]\n";

		_clients[it->fd]->httpReq->handleRequest(buffer);

		std::ostringstream Oss;
		Oss << "--> Client #" << senderFd << " says " << buffer;
		std::string MessageToSend = Oss.str();

		// // Loop to send the message to all the clients
		// for (std::vector<struct pollfd>::iterator j = _pollFds.begin(); j != _pollFds.end() ; j++)
		// {
		// 	if (j->fd != _serverFd && j->fd != senderFd)
		// 	{
		// 		status = send(j->fd, MessageToSend.c_str(), MessageToSend.length(), 0);
		// 		if (status == -1)
		// 		{
		// 			std::cerr << RED << "[server] ERROR : Send error ( " << strerror(errno) << " ) to client #" << j->fd << RESET << std::endl;
		// 			_serverFd = -2;
		// 			return (-1);
		// 		}
		// 	}
		// }
	}
	return (0);
}
