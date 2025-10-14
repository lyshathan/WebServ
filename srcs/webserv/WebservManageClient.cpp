#include "Webserv.hpp"
#include "../parsing/Client.hpp"


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
	fcntl(clientFd, F_SETFL, O_NONBLOCK); // CHECK_HERE

	std::stringstream msg;
	msg << "New Client #" << clientFd << " IP " << clientIP << " connected";
	printLog(BLUE, "INFO", msg.str());
	return (0);
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