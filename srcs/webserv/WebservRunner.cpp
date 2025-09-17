#include "Webserv.hpp"
#include "../parsing/Client.hpp"

int	Webserv::runningServ(void)
{

	int	status;
	int	timeout = 3000;	// 3 seconds
	while (1)
	{
		// check if any socket is ready, else wait
		status = poll(_pollFds.data(), _pollFds.size(), timeout);
		if (status == -1)
			return (handleFunctionError("poll"));
		else if (status == 0) // Is this condition useful?
		{
			//std::cout << "[Server] Waiting ..." << std::endl;
			continue;
		}

		//std::cout << "Sockets are ready" << std::endl;
		// Loop check for each socket
		if (connectAndRead() < 0)
			return(1);
	}
}

int	Webserv::connectAndRead(void)
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
			status = acceptNewConnection(*find);
			if (status == -1)
				return (-1);
			it = _pollFds.begin() + 1; // Restart the loop from the first client socket because of push back
		}
		else
		{
			//std::cout << "Read data [" << it->fd << "]" << std::endl;
			status = readDataFromSocket(it);
			if (status <= 0)
				return (-1);
		}

	}
	return (1);
}

int Webserv::acceptNewConnection(int &serverFd)
{
	int			clientFd;

	clientFd = accept(serverFd, NULL, NULL); // Do we need to get the port and addresss of the new client socket ?
	if (clientFd == -1)
		return (handleFunctionError("Accept"));

	// Add new client to pollFds and to _client map
	addClient(clientFd);

	//std::cout << BLUE << "[Server] Accept new conncetion on client socket : " << clientFd << "for server " << serverFd << RESET << std::endl;
	return (0);
}

// const ServerConfig* Webserv::getConfigForPort(int serverFd) {
// 	std::map<int, const ServerConfig*>::const_iterator it = _portToConfig.find(serverFd);
// 	if (it != _portToConfig.end())
// 		return it->second;
// 	return NULL;
// }

int Webserv::readDataFromSocket(std::vector<struct pollfd>::iterator & it)
{
	char	buffer[BUFSIZ];
	int 	senderFd;
	int		bytesRead;
	// int		status;

	senderFd = it->fd;
	bytesRead = recv(senderFd, buffer, BUFSIZ, 0);
	if (bytesRead <= 0)
	{
		if (bytesRead == 0)
			std::cerr << YELLOW << "[server] Client #" << senderFd << " closed connection" << RESET << std::endl;
		else
			std::cerr << YELLOW << "[server] Client #" << senderFd << " recv error: " << strerror(errno) << RESET << std::endl;
		deleteClient(it->fd, it);
	}
	else
	{
		_clients[it->fd]->appendBuffer(buffer, bytesRead);
		if (!_clients[senderFd]->httpReq->getHeadersParsed()) {
			_clients[senderFd]->httpReq->requestHeaderParser(_clients[senderFd]->getRes());
		}

		int	httpStatus = _clients[senderFd]->httpReq->getStatus();
		if (httpStatus != 0) {
			return processAndSendResponse(it->fd);
		} else if (_clients[senderFd]->isReqComplete()) {
			_clients[senderFd]->httpReq->requestBodyParser(_clients[senderFd]->getRes());
			_clients[senderFd]->httpReq->requestHandler();
			return processAndSendResponse(it->fd);
		}
	}
	return (1);
}

int Webserv::processAndSendResponse(int clientFd) {
	_clients[clientFd]->httpRes->parseResponse();
	int status = sendResponse(clientFd);
	_clients[clientFd]->clearBuffer();
	if (status == -1)
		return (handleFunctionError("Send"));
	return 1;
}

int Webserv::sendResponse(int clientFd) {
	int status;

	std::string resHeaders = _clients[clientFd]->httpRes->getResHeaders().c_str();
	status = send(clientFd, resHeaders.c_str(), resHeaders.length(), 0);
	if (status == -1)
		return (handleFunctionError("Send"));

	bool isTextContent = _clients[clientFd]->httpRes->getIsTextContent();
	if (!isTextContent) {
		std::vector<char> binaryContent = _clients[clientFd]->httpRes->getBinRes();
		status = send(clientFd, &binaryContent[0], binaryContent.size(), 0);
		if (status == -1)
			return (handleFunctionError("Send"));
	} else {
		std::string textContent = _clients[clientFd]->httpRes->getRes().c_str();
		status = send(clientFd, textContent.c_str(), textContent.size(), 0);
		if (status == -1)
			return (handleFunctionError("Send"));
	}
	return (0);
}