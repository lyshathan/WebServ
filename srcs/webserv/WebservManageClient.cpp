#include "Webserv.hpp"
#include "../parsing/Client.hpp"


void Webserv::addClient(int newClientFd, const std::string &clientIP)
{
	_clients[newClientFd] = new Client(newClientFd, _config, clientIP);
	// Add to pollFds
	struct pollfd newClientPollFd;
	newClientPollFd.fd = newClientFd;
	newClientPollFd.events = POLLIN;
	newClientPollFd.revents = 0;
	_pollFds.push_back(newClientPollFd);
	//std::cout << "Validate client #" << newClientFd << std::endl;
}

void Webserv::deleteClient(int &clientFd, std::vector<struct pollfd>::iterator & it)
{
	// Check if client has active CGI and clean it up
	if (_clients.find(clientFd) != _clients.end() &&
		_clients[clientFd]->httpReq->getCGIState() != NULL) {
		cleanupCGI(clientFd, _clients[clientFd]->httpReq->getCGIState());
	}

	// Delete from list of clients
	Client *clientToDelete = _clients[clientFd];
	delete(clientToDelete);
	_clients.erase(clientFd);

	// Delete from pollFds
	close(clientFd);
	clientFd = -1;
	it = _pollFds.erase(it) - 1;
}