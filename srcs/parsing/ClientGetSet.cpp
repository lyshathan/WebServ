#include "Client.hpp"

const std::string &Client::getRes() const { return _reqBuffer;}

int	Client::getFd() const { return _fd; }

size_t Client::getPollIndex() { return _pollIndex; }

void Client::setState(ClientState state) { _state = state; }

std::string	Client::getClientIp() const { return _clientIP; }

CgiHandler	*Client::getCgi() const { return _cgi; }

void	Client::setCgi() { _cgi = NULL; }

std::map<std::string, std::string> Client::getCgiEnv() const { return _env; }