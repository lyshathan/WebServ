#include "Webserv.hpp"
#include "../parsing/Client.hpp"

void Webserv::addCGIToPoll(Client *client, struct pollfd &pfd) {
	CgiState *cgi = client->httpReq->getCGIState();

	struct pollfd stdout_pollfd = {cgi->stdout_fd, POLLIN, 0};
	_pollFds.push_back(stdout_pollfd);
	_cgiToClient[cgi->stdout_fd] = pfd.fd;
	
	if (cgi->state == CgiState::WRITING && cgi->stdin_fd != -1) {
		struct pollfd stdin_pollfd = {cgi->stdin_fd, POLLOUT, 0};
		_pollFds.push_back(stdin_pollfd);
		_cgiToClient[cgi->stdin_fd] = pfd.fd;
	}
}

void Webserv::removeFdFromPoll(int fd) {
	for (std::vector<struct pollfd>::iterator it = _pollFds.begin(); it != _pollFds.end(); ++it) {
		if (it->fd == fd) {
			_pollFds.erase(it);
			break;
		}
	}

	std::map<int, int>::iterator mapIt = _cgiToClient.find(fd);
	if (mapIt != _cgiToClient.end()) {
		_cgiToClient.erase(mapIt);
	}
}

void Webserv::cleanupCGI(Client *client, CgiState *cgiState) {
	if (!client || !cgiState)
		return;

	// Remove from poll before closing (defensive)
	if (cgiState->stdin_fd != -1) {
		removeFdFromPoll(cgiState->stdin_fd);
		close(cgiState->stdin_fd);
	}
	if (cgiState->stdout_fd != -1) {
		removeFdFromPoll(cgiState->stdout_fd);
		close(cgiState->stdout_fd);
	}

	if (cgiState->pid > 0) {
		kill(cgiState->pid, SIGTERM);
		waitpid(cgiState->pid, NULL, 0);
	}

	// Find a better way to handle this
	size_t index = client->getPollIndex();
	_pollFds[index].events |= POLLOUT;

	delete cgiState;
	client->httpReq->setCGIState(NULL);
}

void Webserv::closeCGIStdin(CgiState *cgiState) {
	if (cgiState->stdin_fd != -1) {
		close(cgiState->stdin_fd);
		removeFdFromPoll(cgiState->stdin_fd);
		cgiState->stdin_fd = -1;
	}
	cgiState->state = CgiState::READING;
}

void Webserv::removePollFd(int fd)
{
    for (std::vector<struct pollfd>::iterator it = _pollFds.begin();
         it != _pollFds.end(); ++it)
    {
        if (it->fd == fd) {
            _pollFds.erase(it);
            break;
        }
    }

    std::map<int, int>::iterator cgiIt = _cgiToClient.find(fd);
    if (cgiIt != _cgiToClient.end())
        _cgiToClient.erase(cgiIt);

    std::map<int, Client*>::iterator clientIt = _clients.find(fd);
    if (clientIt != _clients.end()) {
        delete clientIt->second;
        _clients.erase(clientIt);
    }

    close(fd);
}
