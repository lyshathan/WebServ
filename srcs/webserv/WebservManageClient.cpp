#include "Webserv.hpp"
#include "../parsing/Client.hpp"
#include "../parsing/response/HttpResponse.hpp"

void Webserv::addClient(int newClientFd, const std::string &clientIP, std::vector<struct pollfd> &newPollFds)
{
	// Add to pollFds
	struct pollfd newClientPollFd;
	newClientPollFd.fd = newClientFd;
	newClientPollFd.events = POLLIN;
	newClientPollFd.revents = 0;

	newPollFds.push_back(newClientPollFd);

	size_t index = _pollFds.size() - 1;
	_clients[newClientFd] = new Client(newClientFd, _config, clientIP, index);
	_clients[newClientFd]->updateActivity();
}

void Webserv::handleClientCGI(Client *client, std::vector<struct pollfd> &newPollFds, struct pollfd &pfd) {
	client->launchCGI();
	CgiHandler *cgi = client->getCgi();
	if (cgi) {
		addCGIToPoll(client, cgi, newPollFds);
		pfd.events &= ~POLLIN;
		client->setState(CGI_PROCESSING);
		return ;
	}
}

int Webserv::acceptNewConnection(int &serverFd, std::vector<struct pollfd> &newPollFds)
{
	int					clientFd;
	struct sockaddr_in	clientAddr;

	socklen_t clientAddrLen = sizeof(clientAddr);
	clientFd = accept(serverFd, (struct sockaddr*)&clientAddr, &clientAddrLen);
	if (clientFd == -1)
		return (handleFunctionError("Accept"));

	// Add new client to pollFds and to _client map
	std::string clientIP = inet_ntoa(clientAddr.sin_addr);
	addClient(clientFd, clientIP, newPollFds);
	fcntl(clientFd, F_SETFL, O_NONBLOCK);

	std::stringstream msg;
	msg << "New Client #" << clientFd << " IP " << clientIP << " connected";
	printLog(BLUE, "INFO", msg.str());
	return (0);
}

void Webserv::signalClientReady(Client *client) {
	std::vector<struct pollfd>::iterator clientIt = _pollFds.begin();
	for (; clientIt != _pollFds.end(); ++clientIt) {
		if (clientIt->fd == client->getFd()) {
			clientIt->events = POLLOUT;
			break;
		}
	}
	if (client->getCgi())
		delete (client->getCgi());
	client->setCgiNull();
	client->setState(REQUEST_READY);
}

void Webserv::checkClientTimeouts(std::vector<int> &removeFds) {
	time_t now = time(NULL);

	for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		Client *client = it->second;
		if (!client->hasTimedOut(now))
			continue;
		if (client->getClientState() == CONNECTION_IDLE) {
			std::vector<struct pollfd>::iterator clientIt = _pollFds.begin();
			for (; clientIt != _pollFds.end(); ++clientIt) {
				if (clientIt->fd == client->getFd()) {
					removeFds.push_back(clientIt->fd);
					clientIt->events = POLLOUT;
					break;
				}
			}
			continue;
		} else {
			CgiHandler *cgi = client->getCgi();
			if (client->getClientState() == CGI_PROCESSING)
				client->httpReq->setStatus(GATEWAY_TIMEOUT);
			else
				client->httpReq->setStatus(REQUEST_TIMEOUT);
			client->httpRes->parseResponse();
			if (cgi)
				cgi->cleanUp(removeFds);
			signalClientReady(client);
		}
	}
}