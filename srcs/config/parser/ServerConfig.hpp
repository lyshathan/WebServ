#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include "LocationConfig.hpp"
#include "GlobalConfig.hpp"

class ServerConfig {
	private :
		std::vector< ServerConfig >		&_serversConfig;
		std::vector<t_token>			&_tokens;

		std::map< uint16_t, std::string>_portAndIP;
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
		void	SortLocation();


	public :
		ServerConfig(std::vector<t_token> &tokenList, std::vector< t_token>::iterator &it, std::vector< ServerConfig >	&serversConfig);
		~ServerConfig();

		ServerConfig & operator=(ServerConfig const &otherServerConfig);
		void	PrintServer();
		void	Check(GlobalConfig &global);

		size_t								getClientMaxBodySize(void) const;
		std::vector<int>					getListenPort(void) const;
		std::map< uint16_t, std::string>	getPortAndIP(void) const;
		std::vector<std::string>			getServerName(void) const;
		std::string							getRoot(void) const;
		const std::vector<LocationConfig>&	getLocations(void) const;

};

#endif