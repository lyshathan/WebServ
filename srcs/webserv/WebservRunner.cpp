#include "Webserv.hpp"
#include "../parsing/Client.hpp"
#include "../ProjectTools.hpp"

int	Webserv::runningServ(void)
{
	int	status;
	int	timeout = 3000;	// 3 seconds
	while (g_running)
	{
		status = poll(_pollFds.data(), _pollFds.size(), timeout);
		if (status == -1)
		{
			if (!g_running) // Check if shutdown was requested
				break;
			return (handleFunctionError("poll"));
		}
		else if (status == 0) // Timeout - this is useful for checking g_running
		{
			//std::cout << "[Server] Waiting ..." << std::endl;
			continue;
		}
		// Loop check for each socket
		if (connectAndRead() < 0)
			return(1);
	}
	return (0);
}

int	Webserv::connectAndRead(void)
{
	int	status;

	for (std::vector<struct pollfd>::iterator it = _pollFds.begin() ; it != _pollFds.end() ; it++)
	{
		if (! (it->revents & POLLIN)) // Socket i is not ready for now
			continue;

		it->revents = 0; // Reset revents to 0 so we can see if it has changed next time

		std::vector<int>::iterator find = std::find(_serverFds.begin(), _serverFds.end(), it->fd);
		if (find != _serverFds.end())
		{
			status = acceptNewConnection(*find);
			if (status == -1)
				return (-1);
			it = _pollFds.begin() + 1; // Restart the loop from the first client socket because of push back
		}
		else
		{
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

	std::stringstream ss;
	ss << serverFd;
	std::stringstream clientSs;
	clientSs << clientFd;
	printLog(YELLOW, "INFO", "New Client #" + clientSs.str() + " On Socket #" + ss.str());
	return (0);
}

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
		std::stringstream senderSs;
		senderSs << senderFd;
		if (bytesRead == 0)
			printLog(YELLOW, "INFO", "Client #" + senderSs.str() + " Closed Connection");
		else
			printLog(RED, "ERROR",  "Client #" + senderSs.str() + " recv error: " + strerror(errno));
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
	if (_clients.find(clientFd) == _clients.end())
		return -1;

	_clients[clientFd]->httpRes->parseResponse();
	int status = sendResponse(clientFd);

	if (status == -1) {
		// Client likely disconnected, clean up gracefully
		std::vector<struct pollfd>::iterator it = _pollFds.begin();
		for (; it != _pollFds.end(); ++it) {
			if (it->fd == clientFd) {
				deleteClient(clientFd, it);
				break;
			}
		}
		return 1; // Continue processing other clients
	}

	_clients[clientFd]->clearBuffer();
	return 1;
}

int Webserv::sendResponse(int clientFd) {
	int status;

	// Check if client still exists
	if (_clients.find(clientFd) == _clients.end())
		return -1;

	std::string resHeaders = _clients[clientFd]->httpRes->getResHeaders().c_str();
	status = send(clientFd, resHeaders.c_str(), resHeaders.length(), MSG_NOSIGNAL);
	if (status == -1)
		return -1; // Client likely disconnected

	bool isTextContent = _clients[clientFd]->httpRes->getIsTextContent();
	if (!isTextContent) {
		std::vector<char> binaryContent = _clients[clientFd]->httpRes->getBinRes();
		if (binaryContent.size() > 0) {
			status = send(clientFd, &binaryContent[0], binaryContent.size(), MSG_NOSIGNAL);
			if (status == -1)
				return -1; // Client likely disconnected
		}
	} else {
		std::string textContent = _clients[clientFd]->httpRes->getRes().c_str();
		if (textContent.size() > 0) {
			status = send(clientFd, textContent.c_str(), textContent.size(), MSG_NOSIGNAL);
			if (status == -1)
				return -1; // Client likely disconnected
		}
	}
	return (0);
}