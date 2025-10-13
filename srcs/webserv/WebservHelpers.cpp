#include "Webserv.hpp"
#include "../parsing/Client.hpp"

void Webserv::addCGIToPoll(Client *client, struct pollfd &pfd, std::vector<struct pollfd> &newPollFds) {
	std::cerr << "\033[36m[DEBUG] Entering addCGIToPoll for client fd " << pfd.fd << "\033[0m" << std::endl;
	CgiState *cgi = client->httpReq->getCGIState();

	struct pollfd stdout_pollfd = {cgi->stdout_fd, POLLIN, 0};
	_cgiToClient[cgi->stdout_fd] = pfd.fd;
	newPollFds.push_back(stdout_pollfd);
	std::cerr << "\033[35m[DEBUG] Added CGI stdout_fd " << cgi->stdout_fd << " to poll (POLLIN)\033[0m" << std::endl;
	
	if (cgi->state == CgiState::WRITING && cgi->stdin_fd != -1) {
		struct pollfd stdin_pollfd = {cgi->stdin_fd, POLLOUT, 0};
		_cgiToClient[cgi->stdin_fd] = pfd.fd;
		newPollFds.push_back(stdin_pollfd);
		std::cerr << "\033[35m[DEBUG] Added CGI stdin_fd " << cgi->stdin_fd << " to poll (POLLOUT)\033[0m" << std::endl;
	}
	std::cerr << "\033[36m[DEBUG] Exiting addCGIToPoll\033[0m" << std::endl;
}

void Webserv::removeFdFromPoll(int fd) {
	std::cerr << "\033[36m[DEBUG] Entering removeFdFromPoll for fd " << fd << "\033[0m" << std::endl;
	for (std::vector<struct pollfd>::iterator it = _pollFds.begin(); it != _pollFds.end(); ++it) {
		if (it->fd == fd) {
			_pollFds.erase(it);
			std::cerr << "\033[31m[DEBUG] Removed fd " << fd << " from _pollFds\033[0m" << std::endl;
			break;
		}
	}

	std::map<int, int>::iterator mapIt = _cgiToClient.find(fd);
	if (mapIt != _cgiToClient.end()) {
		_cgiToClient.erase(mapIt);
		std::cerr << "\033[31m[DEBUG] Removed fd " << fd << " from _cgiToClient\033[0m" << std::endl;
	}
	std::cerr << "\033[36m[DEBUG] Exiting removeFdFromPoll\033[0m" << std::endl;
}

void Webserv::cleanupCGI(Client *client, CgiState *cgiState, std::vector<int> &removeFds) {
	std::cerr << "\033[36m[DEBUG] Entering cleanupCGI\033[0m" << std::endl;
	if (!client || !cgiState) {
		std::cerr << "\033[31m[DEBUG] cleanupCGI: null client or cgiState\033[0m" << std::endl;
		return;
	}

	removeFds.push_back(cgiState->stdin_fd);
	removeFds.push_back(cgiState->stdout_fd);
	std::cerr << "\033[35m[DEBUG] Marked CGI stdin_fd " << cgiState->stdin_fd << " and stdout_fd " << cgiState->stdout_fd << " for removal\033[0m" << std::endl;

	if (cgiState->pid > 0) {
		kill(cgiState->pid, SIGTERM);
		waitpid(cgiState->pid, NULL, 0);
		std::cerr << "\033[31m[DEBUG] Killed CGI process pid " << cgiState->pid << "\033[0m" << std::endl;
	}

	delete cgiState;
	client->httpReq->setCGIState(NULL);
	std::cerr << "\033[32m[DEBUG] Cleaned up CGI state\033[0m" << std::endl;
	std::cerr << "\033[36m[DEBUG] Exiting cleanupCGI\033[0m" << std::endl;
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

    std::map<int, int>::iterator cgiIt = _cgiToClient.find(fd);
    if (cgiIt != _cgiToClient.end()) {
        _cgiToClient.erase(cgiIt);
        std::cerr << "\033[31m[DEBUG] Removed fd " << fd << " from _cgiToClient\033[0m" << std::endl;
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
