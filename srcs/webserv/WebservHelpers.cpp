#include "Webserv.hpp"
#include "../parsing/Client.hpp"


void Webserv::addCGIToPoll(Client *client, CgiHandler *cgi, std::vector<struct pollfd> &newPollFds) {

	int stdinFd = cgi->getStdinFd();
	int stdoutFd = cgi->getStdoutFd();

	struct pollfd stdout_pollfd = {stdoutFd, POLLIN, 0};
	newPollFds.push_back(stdout_pollfd);
	_cgiToClient[stdinFd] = client;
	std::cerr << "\033[35m[DEBUG] Added CGI stdout_fd " << stdoutFd << " to poll (POLLIN)\033[0m" << std::endl;
	if (cgi->getCgiStage() == CGI_WRITING && stdinFd != -1) {
		struct pollfd stdin_pollfd = {stdinFd, POLLOUT, 0};
		newPollFds.push_back(stdin_pollfd);
		_cgiToClient[stdoutFd] = client;
		std::cerr << "\033[35m[DEBUG] Added CGI stdin_fd " << stdinFd << " to poll (POLLOUT)\033[0m" << std::endl;
	}
	std::cerr << "\033[36m[DEBUG] Exiting addCGIToPoll\033[0m" << std::endl;
}

void Webserv::signalClientReady(Client *client) {
	std::cerr << "\033[36m[DEBUG] Entering signalClientReady for client fd " << client->getFd() << "\033[0m" << std::endl;
	int fd = client->getFd();
	for (size_t i = 0; i < _pollFds.size(); ++i) {
		if (_pollFds[i].fd == fd) {
			_pollFds[i].events |= POLLOUT;
			std::cerr << "\033[32m[DEBUG] Set POLLOUT for client fd " << fd << "\033[0m" << std::endl;
			break;
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
        delete clientIt->second;
        _clients.erase(clientIt);
        std::cerr << "\033[31m[DEBUG] Deleted client for fd " << fd << "\033[0m" << std::endl;
    }

    close(fd);
    std::cerr << "\033[31m[DEBUG] Closed fd " << fd << "\033[0m" << std::endl;
    std::cerr << "\033[36m[DEBUG] Exiting removePollFd\033[0m" << std::endl;
}
