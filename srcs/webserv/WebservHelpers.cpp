#include "Webserv.hpp"
#include "../parsing/Client.hpp"


void Webserv::addCGIToPoll(Client *client, CgiHandler *cgi, std::vector<struct pollfd> &newPollFds) {

	int stdinFd = cgi->getStdinFd();
	int stdoutFd = cgi->getStdoutFd();

	struct pollfd stdout_pollfd = {stdoutFd, POLLIN, 0};
	newPollFds.push_back(stdout_pollfd);
	_cgiToClient[stdinFd] = client;

	if (cgi->getCgiStage() == CGI_WRITING && stdinFd != -1) {
		struct pollfd stdin_pollfd = {stdinFd, POLLOUT, 0};
		newPollFds.push_back(stdin_pollfd);
		_cgiToClient[stdoutFd] = client;
	}
}

void Webserv::signalClientReady(std::vector<int> &clientsNeedingOutput) {
	for (size_t i = 0; i < clientsNeedingOutput.size(); ++i) {
		for (size_t j = 0; j < _pollFds.size(); ++j) {
			if (_pollFds[j].fd == clientsNeedingOutput[i]) {
				_pollFds[j].events |= POLLOUT;
				break;
        	}
    	}
	}
	std::cerr << "\033[36m[DEBUG] Exiting signalClientReady\033[0m" << std::endl;
}

void Webserv::removePollFd(int fd)
{
    std::cerr << "\033[36m[DEBUG] Entering removePollFd for fd " << fd << "\033[0m" << std::endl;
    for (std::vector<struct pollfd>::iterator it = _pollFds.begin();
         it != _pollFds.end(); ++it) {
        if (it->fd == fd) {
            _pollFds.erase(it);
            std::cerr << "\033[31m[DEBUG] Removed fd " << fd << " from _pollFds\033[0m" << std::endl;
            break;
        }
    }

	std::map<int, Client*>::iterator clientIt = _clients.find(fd);
	if (clientIt != _clients.end()) {
		// Remove all CGI FDs that point to this client
		for (std::map<int, Client*>::iterator it = _cgiToClient.begin(); it != _cgiToClient.end(); ) {
			if (it->second == clientIt->second) {
				std::map<int, Client*>::iterator toErase = it++;
				_cgiToClient.erase(toErase);
			} else {
				++it;
			}
		}
		delete clientIt->second;
		_clients.erase(clientIt);
		std::cerr << "\033[31m[DEBUG] Deleted client for fd " << fd << "\033[0m" << std::endl;
	}

    close(fd);
    std::cerr << "\033[31m[DEBUG] Closed fd " << fd << "\033[0m" << std::endl;
    std::cerr << "\033[36m[DEBUG] Exiting removePollFd\033[0m" << std::endl;
}
