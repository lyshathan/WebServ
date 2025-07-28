#include "Server.hpp"
#include "../parsing/Client.hpp"

int		Server::HandleFunctionError(std::string errFunction)
{
	std::cerr << RED << "[server] ERROR : " << errFunction << " error ( " << strerror(errno) << " )" << RESET << std::endl;
	_serverFd = -2;
	return (-1);
}