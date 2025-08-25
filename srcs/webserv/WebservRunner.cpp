#include "Webserv.hpp"
#include "../parsing/Client.hpp"

int	Webserv::RunningServ(void)
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
			//std::cout << "[Server] Waiting ..." << std::endl;
			continue;
		}

		//std::cout << "Sockets are ready" << std::endl;
		// Loop check for each socket
		if (ConnectAndRead() < 0)
			return(1);
	}
}

int	Webserv::ConnectAndRead(void)
{
	int	status;

	for (std::vector<struct pollfd>::iterator it = _pollFds.begin() ; it != _pollFds.end() ; it++)
	{
		if (! (it->revents & POLLIN)) // Socket i is not ready for now
			continue;

		//std::cout << BLUE << "[Server] Socket #" << it->fd << " is ready for I/O operation" << RESET << std::endl;

		it->revents = 0; // Reset revents to 0 so we can see if it has changed next time

		std::vector<int>::iterator find = std::find(_serverFds.begin(), _serverFds.end(), it->fd);
		if (find != _serverFds.end())
		{
			//std::cout << "Accept new connection [" << it->fd << "]" << std::endl;
			status = AcceptNewConnection(*find);
			if (status == -1)
				return (-1);
			it = _pollFds.begin() + 1; // Restart the loop from the first client socket because of push back
		}
		else
		{
			//std::cout << "Read data [" << it->fd << "]" << std::endl;
			status = ReadDataFromSocket(it);
			if (status <= 0)
				return (-1);
		}

	}
	return (1);
}

int Webserv::AcceptNewConnection(int &serverFd)
{
	int			clientFd;

	clientFd = accept(serverFd, NULL, NULL); // Do we need to get the port and addresss of the new client socket ?
	if (clientFd == -1)
		return (HandleFunctionError("Accept"));

	// Add new client to pollFds and to _client map
	AddClient(clientFd);

	//std::cout << BLUE << "[Server] Accept new conncetion on client socket : " << clientFd << "for server " << serverFd << RESET << std::endl;
	return (0);
}

const ServerConfig* Webserv::getConfigForPort(uint16_t port) {
	std::map<uint16_t, const ServerConfig*>::const_iterator it = _portToConfig.find(port);
	if (it != _portToConfig.end())
		return it->second;
	return NULL;
}

int Webserv::ReadDataFromSocket(std::vector<struct pollfd>::iterator & it)
{
	char	buffer[BUFSIZ + 1];
	int 	senderFd;
	int		bytesRead;
	int		status;

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
		{
			// Handle recv errors - most are client-side issues, not server fatal errors
			if (errno == ECONNRESET)
			{
				std::cerr << YELLOW << "[server] Client #" << senderFd << " connection reset by peer" << RESET << std::endl;
			}
			else
			{
				// Other errors - log them but still don't crash the server
				std::cerr << YELLOW << "[server] Client #" << senderFd << " recv error: " << strerror(errno) << RESET << std::endl;
			}
			DeleteClient(it->fd, it);
		}
	}
	else
	{
		if (!std::strncmp("stop", buffer, 4))
		{
			//std::cout << GREEN << "Stopping server" << RESET << std::endl;
			CleanServer();
			return(0);
		}
		//std::cout << "\n\n------- Client #" << senderFd << " sent a message ------\n" << buffer << std::endl;

		_clients[it->fd]->appendBuffer(buffer, bytesRead);
		if (_clients[senderFd]->isReqComplete()) {
			const ServerConfig* config = getConfigForPort(1024);
			if (config) {
				_clients[senderFd]->httpReq->handleRequest(_clients[senderFd]->getRes(), *config);
			} else {
				std::cerr << "[server] No configuration found for port " << std::endl;
				return -1;
			}
			_clients[it->fd]->httpRes->parseResponse();
			std::string res = _clients[it->fd]->httpRes->getRes().c_str();
			status = send(it->fd, res.c_str(), res.length(), 0);
			_clients[it->fd]->clearBuffer();
			if (status == -1)
				return (HandleFunctionError("Send"));
		}
	}
	return (1);
}