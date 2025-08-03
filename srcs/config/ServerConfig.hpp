#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include "ConfigIncludes.hpp"
#include "Token.hpp"
#include "LocationConfig.hpp"
// class LocationConfig;

class ServerConfig {
	private :
		std::vector<t_token>			_tokens;
		std::vector<int>				_listenPorts;
		std::vector<std::string>		_serverNames;
		std::string						_root;
		std::vector<std::string>		_indexFiles;
		std::map< int , std::string >	_errorPages;
		size_t							_clientMaxBodySize;
		std::vector< LocationConfig >	_locations;

		const size_t					_currentLevel;


	public :
		ServerConfig(std::vector<t_token> &tokenList, std::vector< t_token>::iterator &it);
		~ServerConfig();
		void	PrintServer();
};

#endif