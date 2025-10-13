#include "Webserv.hpp"
#include "../parsing/Client.hpp"

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
