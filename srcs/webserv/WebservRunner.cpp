#include "Webserv.hpp"
#include "../parsing/Client.hpp"
#include "../parsing/response/HttpResponse.hpp"

void Webserv::loopPool() {
	for (size_t i = 0; i < _pollFds.size(); ++i) {
		struct pollfd &pfd = _pollFds[i];

		std::cout << "[POLL ELEMENT] FD: " << pfd.fd << "\n";

		// Events we are polling for
		std::cout << "  Events: "
				<< ((pfd.events & POLLIN) ? "POLLIN " : "")
				<< ((pfd.events & POLLOUT) ? "POLLOUT " : "")
				<< ((pfd.events & POLLERR) ? "POLLERR " : "")
				<< ((pfd.events & POLLHUP) ? "POLLHUP " : "")
				<< ((pfd.events & POLLNVAL) ? "POLLNVAL " : "")
				<< "\n";

		// Events that actually occurred
		std::cout << "  Returned: "
				<< ((pfd.revents & POLLIN) ? "POLLIN " : "")
				<< ((pfd.revents & POLLOUT) ? "POLLOUT " : "")
				<< ((pfd.revents & POLLERR) ? "POLLERR " : "")
				<< ((pfd.revents & POLLHUP) ? "POLLHUP " : "")
				<< ((pfd.revents & POLLNVAL) ? "POLLNVAL " : "")
				<< "\n";
	}
}

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
		else if (status == 0) {
			continue;
		}
		if (connectAndRead() < 0) // Loop check for each socket
			return(1);
	}
	return (0);
}

int	Webserv::connectAndRead(void)
{
	std::vector<int> clientsNeedingOutput;
	std::vector<struct pollfd> newPollFds;
    std::vector<int> removeFds;

	for (size_t i = 0; i < _pollFds.size() ; ++i )
	{
		struct pollfd &pfd = _pollFds[i]; // Reference to the current FD

		if (pfd.revents == 0)
			continue;

		// --- Server sockets ---
		std::vector<int>::iterator find = std::find(_serverFds.begin(), _serverFds.end(), pfd.fd);
		if (find != _serverFds.end()) {
			acceptNewConnection(pfd.fd, newPollFds);  // Do we need to treat this return value?
			continue;
		}
		// --- Client sockets ---
		std::map<int, Client*>::iterator clientIt = _clients.find(pfd.fd);
		if (clientIt != _clients.end()) {
			// std::cout << "Event is for client " << pfd.fd << "\n";
			Client *client = clientIt->second;
			if (!client) continue;
			handleEvents(client, pfd, newPollFds, removeFds);
			continue;
		}
		// --- CGI FDs ---
		std::map<int, Client *>::iterator cgiIt = _cgiToClient.find(pfd.fd);
		if (cgiIt != _cgiToClient.end()) {
			Client *client = cgiIt->second;
			if (!client) continue;
			CgiHandler *cgi = client->getCgi();
			if (!cgi) continue;
			cgi->handleEvent(pfd, removeFds);
			if (cgi->isFinished()) {
				std::vector<struct pollfd>::iterator clientIt = _pollFds.begin();
				for (; clientIt != _pollFds.end(); ++clientIt)
					if (clientIt->fd == client->getFd())
						break;

				if (clientIt != _pollFds.end())
					clientIt->events |= POLLOUT;
				delete (client->getCgi());
				client->setCgi();
				client->setState(REQUEST_READY);
			}
		}
	}

	for (size_t i = 0; i < clientsNeedingOutput.size(); ++i) {
		for (size_t j = 0; j < _pollFds.size(); ++j) {
			if (_pollFds[j].fd == clientsNeedingOutput[i]) {
				_pollFds[j].events |= POLLOUT;
				break;
        	}
    	}
	}

	for (size_t i = 0; i < removeFds.size(); ++i)
        removePollFd(removeFds[i]);

    _pollFds.insert(_pollFds.end(), newPollFds.begin(), newPollFds.end());
	return (1);
}

void Webserv::handleEvents(Client *client, struct pollfd &pfd, std::vector<struct pollfd> &newPollFds,
                               std::vector<int> &removeFds) {
	if (pfd.revents & POLLHUP) {
		removeFds.push_back(pfd.fd);
	} else {
		if (pfd.revents & POLLIN) {
			int ret = client->readAndParseRequest();
			if (ret == READ_COMPLETE) {
				client->httpReq->requestHandler();

				if (client->isCGI()) {
					client->launchCGI();
					CgiHandler *cgi = client->getCgi();
					if (cgi) {
						addCGIToPoll(client, cgi, newPollFds);
						pfd.events &= ~POLLIN;
						client->setState(CGI_PROCESSING);
						return ;
					}
				} else {
					client->httpRes->parseResponse();
					pfd.events |= POLLOUT;
					return ;
				}
			} else if (ret == READ_ERROR) {
				removeFds.push_back(pfd.fd);
			}
		}
		if (pfd.revents & POLLOUT) {
			int ret = client->writeResponse();
			if (ret == WRITE_INCOMPLETE) {
				return ;
			}
			removeFds.push_back(pfd.fd);
		}
	}
}