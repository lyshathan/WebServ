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
			if (pfd.revents & POLLHUP) {
				disconnectClient(pfd.fd);
			} else {
				if (pfd.revents & POLLIN)
					handleClientRead(client, pfd);
				if (pfd.revents & POLLOUT)
					handleClientWrite(client, pfd);
			}
			continue;
		}

		// --- CGI FDs ---
		std::map<int, int>::iterator cgiIt = _cgiToClient.find(pfd.fd);
		if (cgiIt != _cgiToClient.end()) {
			std::map<int, Client*>::iterator cgiClientIt = _clients.find(cgiIt->second);
			if (cgiClientIt == _clients.end() || !cgiClientIt->second) continue;
			Client *client = cgiClientIt->second;
			CgiState *cgi = client->httpReq->getCGIState();

			if (!cgi) continue;
			if (pfd.revents & POLLHUP) {
				client->handleCGICompletion(cgi);
				cleanupCGI(client, cgi);
			}
			else {
				if ((pfd.revents & POLLIN) && pfd.fd == cgi->stdout_fd)
					handleCGIReadEvent(client, cgi);
				if ((pfd.revents & POLLOUT) && pfd.fd == cgi->stdin_fd)
					handleCGIWriteEvent(client, cgi);
			}
		}
	}
	return (1);
}

void Webserv::handleClientRead(Client *client, struct pollfd &pfd) {
	int ret = client->readAndParseRequest(); // Reads and parses the request;

	if (ret == READ_COMPLETE) {
		client->httpReq->requestHandler(); //validate request;

		if (client->isCGI()) {
			addCGIToPoll(client, pfd); // Include CGI fds in the poll
			pfd.events &= ~POLLIN;
		}
		else {
			client->httpRes->parseResponse(); // prepare the response
			pfd.events |= POLLOUT;
		}
	} else if (ret == READ_ERROR)
		disconnectClient(pfd.fd);
}

void Webserv::handleClientWrite(Client *client, struct pollfd &pfd) {
	int ret = client->writeResponse(); // Sends the response to the client

	if (ret == WRITE_COMPLETE) {
		std::cout << "Write complete\n";
		if (client->connectionShouldClose())// Checks if the connection should close
			disconnectClient(pfd.fd);
		else {
			client->resetClient(); // Resets the client if connection remains open;
			pfd.events = POLLIN;
		}
	} else if (ret == WRITE_ERROR) {
		std::cout << "Write error\n";
		disconnectClient(pfd.fd);
	}
}

void Webserv::handleCGIReadEvent(Client *client, CgiState *cgi) {
	int ret = client->handleCGIRead(cgi);

	if (ret == READ_COMPLETE) {
		client->handleCGICompletion(cgi);
		cleanupCGI(client, cgi);
	} else if (ret == READ_ERROR)
		cleanupCGI(client, cgi);
}

void Webserv::handleCGIWriteEvent(Client *client, CgiState *cgi) {
	int ret = client->handleCGIWrite(cgi);

	if (ret == WRITE_COMPLETE) {
		closeCGIStdin(cgi);
	} else if (ret == WRITE_ERROR)
		cleanupCGI(client, cgi);
}