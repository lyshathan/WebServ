#include "Server.hpp"
#include "../parsing/Client.hpp"

void	Server::CleanServer(void)
{
	// Close all fds
	for (std::vector<struct pollfd>::iterator it = _pollFds.begin(); it != _pollFds.end() ; it++)
	{
		if (it->fd != - 1)
		{
			close(it->fd);
			it->fd = -1;
		}
	}

	// Delete all clients
	for (std::map<int, Client*>::iterator it = _clients.begin() ; it != _clients.end() ; it = _clients.begin() )
	{
		if (it->second)
		{
			delete(it->second);
			it->second = NULL;
		}
		_clients.erase(it);
	}
}

int	Server::HandleFunctionError(std::string errFunction)
{
	std::cerr << RED << "[server] ERROR : " << errFunction << " error ( " << strerror(errno) << " )" << RESET << std::endl;
	CleanServer();
	return (-1);
}