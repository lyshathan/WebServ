#include "Webserv.hpp"
#include "../parsing/Client.hpp"

int	Webserv::runningServ(void)
{
	int	status;
	int	timeout = 1000;
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
		if (connectAndRead() < 0) // Loop check for each socket
			return(1);
	}
	return (0);
}

int	Webserv::connectAndRead(void)
{
	for (size_t i = 0; i < _pollFds.size() ; ++i )
	{
		struct pollfd &pfd = _pollFds[i]; // Reference to the current FD

		if (pfd.revents & POLLIN)
			handleReadEvent(pfd);
		else if (pfd.revents & POLLOUT)
			handleWriteEvent(pfd);
		else if (pfd.revents & POLLHUP)
			disconnectClient(pfd.fd);
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

	std::stringstream msg;
	msg << "New Client #" << clientFd << " IP " << clientIP << " connected";
	printLog(BLUE, "INFO", msg.str());		
	return (0);
}

void Webserv::handleReadEvent(struct pollfd &pfd) {
	int status;

	std::vector<int>::iterator find = std::find(_serverFds.begin(), _serverFds.end(), pfd.fd);
	if (find != _serverFds.end()) {
		status = acceptNewConnection(*find);
		return;
	}

	Client *client = _clients[pfd.fd];
	int ret = client->readAndParseRequest(); // Reads and parses the request;
	
	if (ret == READ_COMPLETE) {
		client->httpReq->requestHandler(); //validate request;

		if (client->isCGI())
			handleCGIEvent(pfd, client); // Handle CGI Write event;
		else { 
			client->httpRes->parseResponse(); // prepare the response
			pfd.events = POLLOUT; // ready to send response
		}
	} else if (ret == READ_ERROR)
		disconnectClient(pfd.fd);
}

void Webserv::handleCGIEvent(struct pollfd &pfd, Client *client) {
	CgiState *cgi = client->httpReq->getCGIState();
	if (cgi->state == CgiState::PENDING_POLL) {
		addCGIToPoll(pfd.fd, cgi);
		return ;
	}
	// handleCGIWrite(clientFd, cgiState);
}

void Webserv::handleWriteEvent(struct pollfd &pfd) {
	Client *client = _clients[pfd.fd];

	int ret = client->writeResponse(); // Sends the response to the client

	if (ret == WRITE_COMPLETE) {
		if (client->connectionShouldClose()) // Checks if the connection should close
			disconnectClient(pfd.fd);
		else {
			client->resetClient(); // Resets the client if connection remains open;
			pfd.events = POLLIN;
		}
	} else if (ret == WRITE_ERROR)
		disconnectClient(pfd.fd);
}


//---------------------


// int	Webserv::handleCGIEvents(std::vector<struct pollfd>::iterator &it, short events)
// {
// 	int clientFd = _cgiToClient[it->fd];
// 	CgiState *cgiState = _clients[clientFd]->httpReq->getCGIState();

// 	if (events & POLLHUP) {
// 		handleCGICompletion(clientFd, cgiState);
// 		it = _pollFds.begin();
// 	}
// 	else if (it->fd == cgiState->stdin_fd && (events & POLLOUT)) {
// 		handleCGIWrite(clientFd, cgiState);
// 		it = _pollFds.begin();
// 	}
// 	else if (it->fd == cgiState->stdout_fd && (events & POLLIN)) {
// 		handleCGIRead(clientFd, cgiState);
// 		it = _pollFds.begin();
// 	}
// 	else
// 		++it;

// 	return (1);
// }