#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include "../location/LocationConfig.hpp"
#include "../global/GlobalConfig.hpp"

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
		const size_t					_ID;

		void	parseServerConfig(std::vector< t_token>::iterator &it);
		void	parseListenPort(std::vector< t_token>::iterator &it);
		void	parseRoot(std::vector< t_token>::iterator &it);
		void	parseErrorPage(std::vector< t_token>::iterator &it);
		void	setDefaultLocation(void);
		void	sortLocation();



	public :
		ServerConfig(std::vector<t_token> &tokenList, std::vector< t_token>::iterator &it, std::vector< ServerConfig >	&serversConfig);
		~ServerConfig();

		ServerConfig & operator=(ServerConfig const &otherServerConfig);
		void	printServer() const;
		void	check(GlobalConfig &global);

		size_t								getClientMaxBodySize(void) const;
		std::vector<int>					getListenPort(void) const;
		std::map< uint16_t, std::string>	getPortAndIP(void) const;
		std::vector<std::string>			getServerName(void) const;
		std::vector<std::string>			getIndexFiles(void) const;
		std::string							getRoot(void) const;
		std::map< int , std::string >		getErrorPages(void) const;
		const std::vector<LocationConfig>&	getLocations(void) const;
		size_t								getID(void) const;

};

#endif