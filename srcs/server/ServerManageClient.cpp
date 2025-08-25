#include "Server.hpp"
#include "../parsing/Client.hpp"

void Server::AddClient(int newClientFd)
{
	// Add to list of clients
	_clients[newClientFd] = new Client(newClientFd);

	// Add to pollFds
	struct pollfd newClientPollFd;
	newClientPollFd.fd = newClientFd;
	newClientPollFd.events = POLLIN;
	newClientPollFd.revents = 0;
	_pollFds.push_back(newClientPollFd);
	//std::cout << "Validate client #" << newClientFd << std::endl;
}

void Server::DeleteClient(int &clientFd, std::vector<struct pollfd>::iterator & it)
{
	// Delete from list of clients
	Client *clientToDelete = _clients[clientFd];
	delete(clientToDelete);
	_clients.erase(clientFd);

	// Delete from pollFds
	close(clientFd);
	clientFd = -1;
	it = _pollFds.erase(it) - 1;
}