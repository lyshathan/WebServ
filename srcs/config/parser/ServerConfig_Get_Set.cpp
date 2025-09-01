#include "ServerConfig.hpp"
#include "Utils.hpp"

////////////////////////////////////////////////////////////////////////////////////
//								GET
////////////////////////////////////////////////////////////////////////////////////

size_t ServerConfig::getClientMaxBodySize(void) const
{
	return (_clientMaxBodySize);
}

std::map< uint16_t, std::string>	ServerConfig::getPortAndIP(void) const
{
	return (_portAndIP);
}


std::vector<int> ServerConfig::getListenPort(void) const
{
	return (_listenPorts);
}

std::vector<std::string> ServerConfig::getServerName(void) const
{
	return (_serverNames);
}

std::vector<std::string> ServerConfig::getIndexFiles(void) const
{
	return (_indexFiles);
}

std::string ServerConfig::getRoot(void) const
{
	return (_root);
}

const std::vector<LocationConfig>& ServerConfig::getLocations(void) const { return _locations; }
