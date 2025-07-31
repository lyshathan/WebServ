#include "Server.hpp"
#include "../parsing/Client.hpp"

int	Server::RunningServ(void)
{

	int	status;
	int	timeout = 3000;	// 3 seconds
	while (1)
	{
		// Check if any socket is ready, else wait
		status = poll(_pollFds.data(), _pollFds.size(), timeout);
		if (status == -1)
			return (HandleFunctionError("poll"));
		else if (status == 0) // Is this condition util?
		{
			std::cout << "[Server] Waiting ..." << std::endl;
			continue;
		}

		std::cout << "Sockets are ready" << std::endl;
		// Loop check for each socket
		if (ConnectAndRead() < 0)
			return(1);
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

		std::vector<int>::iterator find = std::find(_serverFds.begin(), _serverFds.end(), it->fd);
		if (find != _serverFds.end())
		{
			std::cout << "Accept new connection [" << it->fd << "]" << std::endl;
			status = AcceptNewConnection(*find);
			if (status == -1)
				return (-1);
			it = _pollFds.begin() + 1; // Restart the loop from the first client socket because of push back
		}
		else
		{
			std::cout << "Read data [" << it->fd << "]" << std::endl;
			status = ReadDataFromSocket(it);
			if (status <= 0)
				return (-1);
		}
		
	}
	return (1);
}

int Server::AcceptNewConnection(int &serverFd)
{
	int			status;
	int			clientFd;

	clientFd = accept(serverFd, NULL, NULL); // Do we need to get the port and addresss of the new client socket ?
	if (clientFd == -1)
		return (HandleFunctionError("Accept"));

	// Add new client to pollFds and to _client map
	AddClient(clientFd);	

	std::cout << BLUE << "[Server] Accept new conncetion on client socket : " << clientFd << "for server " << serverFd << RESET << std::endl;

	std::ostringstream Oss;
	Oss << "Welcome. You are client fd (" << clientFd << ")" << std::endl;
	std::string Message = Oss.str();
	
	status = send(clientFd, Message.c_str(), Message.length(), 0);
	if (status == -1)
		return (HandleFunctionError("Send"));

	return (0);
}

int Server::ReadDataFromSocket(std::vector<struct pollfd>::iterator & it)
{
	char	buffer[BUFSIZ + 1];
	int 	senderFd;
	int		bytesRead;
	// int		status;

	senderFd = it->fd;
	bytesRead = recv(senderFd, buffer, BUFSIZ, 0);
	if (bytesRead <= 0)
	{
		if (bytesRead == 0)
		{
			std::cerr << YELLOW << "[server] Client #" << senderFd << " closed connection" << RESET << std::endl;
			DeleteClient(it->fd, it);
		}
		else
			return (HandleFunctionError("Recv"));
	}
	else
	{
		if (!std::strncmp("stop", buffer, 4))
		{
			std::cout << GREEN << "Stopping server" << RESET << std::endl;
			CleanServer();
			return(0);
		}

		buffer[bytesRead] = 0;
		std::cout << "Client #" << senderFd << " had a message --> " << buffer;

		_clients[senderFd]->httpReq->handleRequest(buffer);


		// Send the message to all the clients
		// std::ostringstream Oss;
		// Oss << "--> Client #" << senderFd << " says " << buffer;
		// std::string MessageToSend = Oss.str();
		// for (std::vector<struct pollfd>::iterator j = _pollFds.begin(); j != _pollFds.end() ; j++)
		// {

		// 	if (j->fd != _serverFd && j->fd != senderFd)
		// 	{
		// 		status = send(j->fd, MessageToSend.c_str(), MessageToSend.length(), 0);
		// 		if (status == -1)
		// 			return (HandleFunctionError("Send"));

		// 	}
		// }
	}
	return (1);
}