#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include "ConfigIncludes.hpp"

class LocationConfig;

class ServerConfig {
	private :
		std::vector<int>				_listenPorts;
		std::vector<std::string>		_serverNames;
		std::string						_root;
		std::vector<std::string>		_indexFiles;
		std::map< int , std::string >	_errorPages;
		size_t							_clientMaxBodySize;
		std::vector< LocationConfig >	_locations;

	public :
		ServerConfig();
		~ServerConfig();
};

#endif