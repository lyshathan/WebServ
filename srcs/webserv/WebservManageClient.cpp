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
	std::stringstream msg;
	msg << "Client #" << fd << " disconnected";
	printLog(BLUE, "INFO", msg.str());
	
	// Clean up CGI FDs before deleting client
	if (_clients.find(fd) != _clients.end()) {
		Client *client = _clients[fd];
		
		// If client has active CGI, remove CGI FDs from poll and map
		if (client->getCgi()) {
			CgiHandler *cgi = client->getCgi();
			int stdinFd = cgi->getStdinFd();
			int stdoutFd = cgi->getStdoutFd();

			printLog(RED, "INFO", "Client has active CGI, cleaning up CGI FDs");
			
			// Remove CGI FDs from _cgiToClient map
			if (stdinFd > 0) _cgiToClient.erase(stdinFd);
			if (stdoutFd > 0) _cgiToClient.erase(stdoutFd);
			
			// Remove CGI FDs from _pollFds
			for (std::vector<struct pollfd>::iterator it = _pollFds.begin(); it != _pollFds.end(); ) {
				if ((stdinFd > 0 && it->fd == stdinFd) || (stdoutFd > 0 && it->fd == stdoutFd)) {
					msg.clear();
					msg << "Removing CGI fd " << it->fd << " from _pollFds";
					printLog(RED, "INFO", msg.str());
					it = _pollFds.erase(it);
				} else {
					++it;
				}
			}
		}
		
		delete client;
		_clients.erase(fd);
	}
	
	close(fd);
	
	// Remove client FD from _pollFds
	for (std::vector<struct pollfd>::iterator it = _pollFds.begin();
		it != _pollFds.end(); ++it) {
			if (it->fd == fd) {
				_pollFds.erase(it);
				break;
			}
		}
}