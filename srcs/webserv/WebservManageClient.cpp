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
}

void Webserv::disconnectClient(int &fd)
{
	// // Check if client has active CGI and clean it up
	// if (_clients.find(clientFd) != _clients.end() &&
	// 	_clients[clientFd]->httpReq->getCGIState() != NULL) {
	// 	cleanupCGI(clientFd, _clients[clientFd]->httpReq->getCGIState());
	// }

	std::stringstream msg;
	msg << "Client #" << fd << " disconnected";
	printLog(BLUE, "INFO", msg.str());
	close (fd);
	delete _clients[fd];
	_clients.erase(fd);
	
	for (std::vector<struct pollfd>::iterator it = _pollFds.begin();
		it != _pollFds.end(); ++it) {
			if (it->fd == fd) {
				_pollFds.erase(it);
				break;
			}
		}
}