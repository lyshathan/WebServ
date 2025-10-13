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

		if (pfd.revents == 0)
			continue;
		// --- Server sockets ---
		std::vector<int>::iterator find = std::find(_serverFds.begin(), _serverFds.end(), pfd.fd);
		if (find != _serverFds.end()) {
			acceptNewConnection(*find);  // Do we need to treat this return value?
			continue;
		}
		// --- Client sockets ---
		std::map<int, Client*>::iterator clientIt = _clients.find(pfd.fd);
		if (clientIt != _clients.end()) {
			Client *client = clientIt->second;
			if (!client) continue;
			handleEvents(client, pfd);
			continue;
		}
		// --- CGI FDs ---
		std::map<int, int>::iterator cgiIt = _cgiToClient.find(pfd.fd);
		if (cgiIt != _cgiToClient.end()) {
			std::map<int, Client*>::iterator cgiClientIt = _clients.find(cgiIt->second);
			if (cgiClientIt == _clients.end() || !cgiClientIt->second) continue;
			Client *client = cgiClientIt->second;
			handleCGIEvents(client, pfd);
		}
	}
	return (1);
}

void Webserv::handleEvents(Client *client, struct pollfd &pfd) {
	if (pfd.revents & POLLHUP) {
		disconnectClient(pfd.fd);
	} else {
		if (pfd.revents & POLLIN) {
			int ret = client->readAndParseRequest();
			if (ret == READ_COMPLETE) {
				client->httpReq->requestHandler();

				if (client->isCGI()) {
					addCGIToPoll(client, pfd);
					pfd.events &= ~POLLIN;
				}
				else {
					client->httpRes->parseResponse(); 
					pfd.events |= POLLOUT;
				}
			} else if (ret == READ_ERROR)
				disconnectClient(pfd.fd);
		}
		if (pfd.revents & POLLOUT) {
			client->writeResponse();
			disconnectClient(pfd.fd);
		}
	}
}

void Webserv::handleCGIEvents(Client *client, struct pollfd &pfd) {
		CgiState *cgi = client->httpReq->getCGIState();
			if (!cgi) return;

		if (pfd.revents & POLLHUP) {
			client->handleCGICompletion(cgi);
			cleanupCGI(client, cgi);
		}
		else {
			if ((pfd.revents & POLLIN) && pfd.fd == cgi->stdout_fd) {
				int ret = client->handleCGIRead(cgi);
				if (ret == READ_COMPLETE) {
					client->handleCGICompletion(cgi);
					cleanupCGI(client, cgi);
				} else if (ret == READ_ERROR)
					cleanupCGI(client, cgi);
			}
			if ((pfd.revents & POLLOUT) && pfd.fd == cgi->stdin_fd) {
				int ret = client->handleCGIWrite(cgi);
				if (ret == WRITE_COMPLETE) {
					closeCGIStdin(cgi);
				} else if (ret == WRITE_ERROR)
					cleanupCGI(client, cgi);
			}
		}
}
// If to disconnect client, disconnect from _pollFd, _clients and _cgiToClient