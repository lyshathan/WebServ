#include "Webserv.hpp"
#include "../parsing/Client.hpp"
#include "../ProjectTools.hpp"

void	Webserv::cleanServer(void)
{
	static bool cleaned = false;
	if (cleaned) return;
	cleaned = true;
	
	printLog(BLUE, "INFO", "Cleaning up server resources...");
	
	// Delete all clients first (they might have open fds)
	for (std::map<int, Client*>::iterator it = _clients.begin() ; it != _clients.end() ; )
	{
		if (it->second)
		{
			delete(it->second);
			it->second = NULL;
		}
		std::map<int, Client*>::iterator toErase = it;
		++it;
		_clients.erase(toErase);
	}
	
	// Clear CGI mappings
	_cgiToClient.clear();
	
	// Close all file descriptors
	for (std::vector<struct pollfd>::iterator it = _pollFds.begin(); it != _pollFds.end() ; ++it)
	{
		if (it->fd != -1)
		{
			close(it->fd);
			it->fd = -1;
		}
	}
	
	// Clear containers
	_pollFds.clear();
	_serverFds.clear();
	_serverInfos.clear();
	
	printLog(BLUE, "INFO", "Server cleanup completed.");
}

int	Webserv::handleFunctionError(std::string errFunction)
{
	std::cerr << RED << "[server] ERROR : " << errFunction << " failed" << RESET << std::endl;
	cleanServer();
	return (-1);
}