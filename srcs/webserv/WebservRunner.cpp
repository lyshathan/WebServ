#include "Webserv.hpp"
#include "../parsing/Client.hpp"

int	Webserv::runningServ(void)
{
	std::cerr << "\033[36m[DEBUG] Entering runningServ\033[0m" << std::endl;
	int	status;
	int	timeout = 1000;
	while (g_running)
	{
		status = poll(_pollFds.data(), _pollFds.size(), timeout);
		if (status == -1)
		{
			std::cerr << "\033[31m[DEBUG] poll() returned -1\033[0m" << std::endl;
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
	std::cerr << "\033[36m[DEBUG] Exiting runningServ\033[0m" << std::endl;
	return (0);
}

int	Webserv::connectAndRead(void)
{
	std::cerr << "\033[36m[DEBUG] Entering connectAndRead\033[0m" << std::endl;
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
			std::cerr << "\033[33m[DEBUG] Accepting new connection on fd " << pfd.fd << "\033[0m" << std::endl;
			acceptNewConnection(pfd.fd, newPollFds);  // Do we need to treat this return value?
			continue;
		}
		// --- Client sockets ---
		std::map<int, Client*>::iterator clientIt = _clients.find(pfd.fd);
		if (clientIt != _clients.end()) {
			std::cerr << "\033[33m[DEBUG] Handling client events for fd " << pfd.fd << "\033[0m" << std::endl;
			Client *client = clientIt->second;
			if (!client) continue;
			handleEvents(client, pfd, newPollFds, removeFds);
			handleEvents(client, pfd, newPollFds, removeFds);
			continue;
		}
		// --- CGI FDs ---
		std::map<int, int>::iterator cgiIt = _cgiToClient.find(pfd.fd);
		if (cgiIt != _cgiToClient.end()) {
			std::cerr << "\033[35m[DEBUG] Handling CGI events for fd " << pfd.fd << "\033[0m" << std::endl;
			std::map<int, Client*>::iterator cgiClientIt = _clients.find(cgiIt->second);
			if (cgiClientIt == _clients.end() || !cgiClientIt->second) continue;
			// Client *client = cgiClientIt->second;
		}
	}
	for (size_t i = 0; i < removeFds.size(); ++i) {
        std::cerr << "\033[31m[DEBUG] Removing poll fd " << removeFds[i] << "\033[0m" << std::endl;
        removePollFd(removeFds[i]);
    }

    _pollFds.insert(_pollFds.end(), newPollFds.begin(), newPollFds.end());
	std::cerr << "\033[36m[DEBUG] Exiting connectAndRead\033[0m" << std::endl;
	return (1);
}

void Webserv::handleEvents(Client *client, struct pollfd &pfd, std::vector<struct pollfd> &newPollFds,
                               std::vector<int> &removeFds) {
	(void)newPollFds;
	std::cerr << "\033[36m[DEBUG] Entering handleEvents for fd " << pfd.fd << "\033[0m" << std::endl;
	if (pfd.revents & POLLHUP) {
		std::cerr << "\033[31m[DEBUG] POLLHUP on fd " << pfd.fd << "\033[0m" << std::endl;
		removeFds.push_back(pfd.fd);
	} else {
		if (pfd.revents & POLLIN) {
			std::cerr << "\033[33m[DEBUG] POLLIN on fd " << pfd.fd << "\033[0m" << std::endl;
			int ret = client->readAndParseRequest();
			std::cerr << "[DEBUG] readAndParseRequest returned " << ret << std::endl;
			if (ret == READ_COMPLETE) {
				std::cerr << "\033[32m[DEBUG] READ_COMPLETE for fd " << pfd.fd << "\033[0m" << std::endl;
				client->httpReq->requestHandler();

				if (client->isCGI()) {
					std::cerr << "\033[35m[DEBUG] Client is CGI, adding CGI to poll for fd " << pfd.fd << "\033[0m" << std::endl;
					pfd.events &= ~POLLIN;
				}
				else {
					std::cerr << "\033[33m[DEBUG] Not CGI, parsing response for fd " << pfd.fd << "\033[0m" << std::endl;
					client->httpRes->parseResponse();
					pfd.events |= POLLOUT;
				}
			} else if (ret == READ_ERROR) {
				std::cerr << "\033[31m[DEBUG] READ_ERROR for fd " << pfd.fd << "\033[0m" << std::endl;
				removeFds.push_back(pfd.fd);
			}
		}
		if (pfd.revents & POLLOUT) {
			std::cerr << "\033[33m[DEBUG] POLLOUT on fd " << pfd.fd << "\033[0m" << std::endl;
			int ret = client->writeResponse();
			std::cerr << "[DEBUG] writeResponse returned " << ret << std::endl;
			if (ret == WRITE_INCOMPLETE) {
				std::cerr << "\033[33m[DEBUG] WRITE_INCOMPLETE for fd " << pfd.fd << "\033[0m" << std::endl;
				return ;
			}
			std::cerr << "\033[31m[DEBUG] Removing fd after writeResponse " << pfd.fd << "\033[0m" << std::endl;
			removeFds.push_back(pfd.fd);
		}
	}
	std::cerr << "\033[36m[DEBUG] Exiting handleEvents for fd " << pfd.fd << "\033[0m" << std::endl;
}
