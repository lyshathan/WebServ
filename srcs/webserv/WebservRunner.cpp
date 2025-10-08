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
		else if (status == 0)
			continue;
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
		if (!(it->revents & (POLLIN | POLLOUT | POLLHUP)))
		{
			++it;
			continue;
		}

		short events = it->revents;
		it->revents = 0;

		std::vector<int>::iterator find = std::find(_serverFds.begin(), _serverFds.end(), it->fd);
		if (find != _serverFds.end())
		{
			status = acceptNewConnection(*find);
			if (status == -1)
				return (-1);
			it = _pollFds.begin() + 1;
		}
		else if (_cgiToClient.find(it->fd) != _cgiToClient.end()) {
			status = handleCGIEvents(it, events);
			if (status == -1)
				return (-1);
		}
		else {
			status = readDataFromSocket(it);
			if (status <= 0)
				return (-1);
			it = _pollFds.begin();
		}
	}
	return (1);
}

int	Webserv::handleCGIEvents(std::vector<struct pollfd>::iterator &it, short events)
{
	int clientFd = _cgiToClient[it->fd];
	CgiState *cgiState = _clients[clientFd]->httpReq->getCGIState();

	if (events & POLLHUP) {
		handleCGICompletion(clientFd, cgiState);
		it = _pollFds.begin();
	}
	else if (it->fd == cgiState->stdin_fd && (events & POLLOUT)) {
		handleCGIWrite(clientFd, cgiState);
		it = _pollFds.begin();
	}
	else if (it->fd == cgiState->stdout_fd && (events & POLLIN)) {
		handleCGIRead(clientFd, cgiState);
		it = _pollFds.begin();
	}
	else
		++it;

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
			processAndSendResponse(it->fd);  // CHECK HERE
			deleteClient(senderFd, it);
			return 1;
		} else if (_clients[senderFd]->isReqComplete()) {
			_clients[senderFd]->httpReq->requestBodyParser(_clients[senderFd]->getRes());
			_clients[senderFd]->httpReq->requestHandler();
			int newStatus = _clients[senderFd]->httpReq->getStatus();
			if (newStatus == CGI_PENDING) {
				addCGIToPoll(senderFd);
				it = _pollFds.begin();
			} else if (newStatus) {
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
		return 1;
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
	if (status == -1 || status < (ssize_t)completeResponse.length() || status == 0)
		return -1;
	return 0;
}