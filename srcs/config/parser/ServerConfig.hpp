#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include "LocationConfig.hpp"
#include "GlobalConfig.hpp"

class ServerConfig {
	private :
		std::vector< ServerConfig >		&_serversConfig;
		std::vector<t_token>			&_tokens;

		std::vector<int>				_listenPorts;
		std::vector<std::string>		_serverNames;
		std::vector<std::string>		_indexFiles;
		std::string						_root;
		std::map< int , std::string >	_errorPages;
		size_t							_clientMaxBodySize;
		std::vector< LocationConfig >	_locations;
		const size_t					_currentLevel;

		void	ParseServerConfig(std::vector< t_token>::iterator &it);
		void	ParseListenPort(std::vector< t_token>::iterator &it);
		void	ParseRoot(std::vector< t_token>::iterator &it);
		void	ParseErrorPage(std::vector< t_token>::iterator &it);


	public :
		ServerConfig(std::vector<t_token> &tokenList, std::vector< t_token>::iterator &it, std::vector< ServerConfig >	&serversConfig);
		~ServerConfig();
		void	PrintServer();
		void	Check(GlobalConfig &global);
		size_t	getClientMaxBodySize(void) const;
		std::string	getRoot(void) const;

};

#endif