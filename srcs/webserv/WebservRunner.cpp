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

	for (std::vector<struct pollfd>::iterator it = _pollFds.begin() ; it != _pollFds.end() ; )
	{
		if (!(it->revents & (POLLIN | POLLOUT | POLLHUP)))  // Socket i is not ready for now
		{
			++it;
			continue;
		}

		short events = it->revents; // Store events before resetting
		it->revents = 0; // Reset revents to 0 so we can see if it has changed next time

		// Handle server sockets (new connections)
		std::vector<int>::iterator find = std::find(_serverFds.begin(), _serverFds.end(), it->fd);
		if (find != _serverFds.end())
		{
			status = acceptNewConnection(*find);
			if (status == -1)
				return (-1);
			it = _pollFds.begin() + 1; // Restart the loop from the first client socket because of push back
		}
		// Handle CGI file descriptors
		else if (_cgiToClient.find(it->fd) != _cgiToClient.end()) {
			status = handleCGIEvents(it, events);
			if (status == -1)
				return (-1);
			// Iterator is managed inside handleCGIEvents
		}
		// Handle regular HTTP client sockets
		else {
			status = readDataFromSocket(it);
			if (status <= 0)
				return (-1);
			// readDataFromSocket may have called deleteClient, restart loop
			it = _pollFds.begin();
		}
	}
	return (1);
}

int	Webserv::handleCGIEvents(std::vector<struct pollfd>::iterator &it, short events)
{
	int clientFd = _cgiToClient[it->fd];
	CgiState *cgiState = _clients[clientFd]->httpReq->getCGIState();

	// CGI process has terminated (POLLHUP)
	if (events & POLLHUP) {
		handleCGICompletion(clientFd, cgiState);
		it = _pollFds.begin(); // Restart iterator after cleanup
	}
	// CGI stdin is ready for writing (send request body)
	else if (it->fd == cgiState->stdin_fd && (events & POLLOUT)) {
		handleCGIWrite(clientFd, cgiState);
		it = _pollFds.begin(); // Restart iterator (stdin may be removed)
	}
	// CGI stdout is ready for reading (receive response)
	else if (it->fd == cgiState->stdout_fd && (events & POLLIN)) {
		handleCGIRead(clientFd, cgiState);
		it = _pollFds.begin(); // Restart iterator
	}
	// Unknown CGI event, just continue
	else {
		++it;
	}

	return (1);
}

int Webserv::acceptNewConnection(int &serverFd)
{
	int					clientFd;
	struct sockaddr_in	clientAddr;

	socklen_t clientAddrLen = sizeof(clientAddr);
	clientFd = accept(serverFd, (struct sockaddr*)&clientAddr, &clientAddrLen);
	if (clientFd == -1)
		return (handleFunctionError("Accept"));

	// Add new client to pollFds and to _client map
	std::string clientIP = inet_ntoa(clientAddr.sin_addr);
	addClient(clientFd, clientIP);

	std::stringstream ss;
	ss << clientIP;
	printLog(BLUE, "INFO", "New Client IP " + ss.str() + " connected");
	return (0);
}

int Webserv::readDataFromSocket(std::vector<struct pollfd>::iterator & it)
{
	char	buffer[BUFSIZ];
	int 	senderFd;
	int		bytesRead;

	senderFd = it->fd;
	bytesRead = recv(senderFd, buffer, BUFSIZ, 0);
	if (bytesRead <= 0)
	{
		std::stringstream senderSs;
		senderSs << senderFd;
		if (bytesRead == 0)
			printLog(BLUE, "INFO", "Client #" + senderSs.str() + " Closed Connection");
		else
			printLog(RED, "ERROR",  "Client #" + senderSs.str() + " recv failed");
		deleteClient(senderFd, it);
	}
	else
	{
		_clients[it->fd]->appendBuffer(buffer, bytesRead);
		if (!_clients[senderFd]->httpReq->getHeadersParsed()) {
			_clients[senderFd]->httpReq->requestHeaderParser(_clients[senderFd]->getRes());
		}

		int	httpStatus = _clients[senderFd]->httpReq->getStatus();
		if (httpStatus == CGI_PENDING) {
			it = _pollFds.begin();
		} else if (httpStatus != 0) {
			return processAndSendResponse(it->fd);
		} else if (_clients[senderFd]->isReqComplete()) {
			_clients[senderFd]->httpReq->requestBodyParser(_clients[senderFd]->getRes());
			_clients[senderFd]->httpReq->requestHandler();
			int newStatus = _clients[senderFd]->httpReq->getStatus();
			if (newStatus == CGI_PENDING) {
				addCGIToPoll(senderFd);
				it = _pollFds.begin();
			} else {
				return processAndSendResponse(it->fd);
			}
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
	std::string completeResponse;

	if (_clients.find(clientFd) == _clients.end())
		return -1;

	completeResponse += _clients[clientFd]->httpRes->getResHeaders();
	if (_clients[clientFd]->httpRes->getIsTextContent()) {
		completeResponse += _clients[clientFd]->httpRes->getRes();
	} else {
		std::vector<char> binaryContent = _clients[clientFd]->httpRes->getBinRes();
		completeResponse.append(binaryContent.begin(), binaryContent.end());
	}

	ssize_t status = send(clientFd, completeResponse.c_str(), completeResponse.length(), MSG_NOSIGNAL);
	if (status == -1 || status < (ssize_t)completeResponse.length())
		return -1;

	return 0;
}