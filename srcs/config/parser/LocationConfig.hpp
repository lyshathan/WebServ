#ifndef LOCATIONCONFIG_HPP
#define LOCATIONCONFIG_HPP

#include "../General.hpp"

class ServerConfig;

class LocationConfig {
	private :
		std::vector< LocationConfig >	&_locations;
		std::vector<t_token>			&_tokens;

		bool							_autoIndex;
		size_t							_clientMaxBodySize;
		std::string						_path;
		std::string						_uploadPath;
		std::string						_cgiExtension;
		std::string						_cgiPath;
		std::string						_root;
		std::vector< std::string >		_indexFiles;
		std::vector< std::string >		_allowMethod;
		std::vector< std::string >		_validMethod;
		std::pair< int , std::string >	_return;

		const size_t					_currentLevel;
		bool							_isExactPath;

		void	locationConfigParser(std::vector< t_token>::iterator &it);
		void	parsePath(std::vector< t_token>::iterator &it);
		void	parseReturn(std::vector< t_token>::iterator &it);
		void	parseAutoIndex(std::vector< t_token>::iterator &it);


	public :
		LocationConfig(ServerConfig &server, std::vector< LocationConfig > &locations, std::vector<t_token> &tokenList);
		LocationConfig(std::vector<t_token> &tokenList, std::vector< t_token>::iterator &it, std::vector< LocationConfig > &locations);
		~LocationConfig();
		LocationConfig & operator=(LocationConfig const &otherLocationConfig);
		void	printLocation(void);
		void	check(ServerConfig &server);

		std::string	getPath(void) const;
		std::string	getRoot(void) const;
		std::vector<std::string> getIndex(void) const;
		void		setPath(std::string newPath);
};

#endif