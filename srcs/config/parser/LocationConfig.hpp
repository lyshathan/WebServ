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
		std::vector< std::string >		_allowMethod;
		std::vector< std::string >		_validMethod;
		std::pair< int , std::string >	_return;

		const size_t				_currentLevel;

		void	LocationConfigParser(std::vector< t_token>::iterator &it);
		void	ParsePath(std::vector< t_token>::iterator &it);
		void	ParseReturn(std::vector< t_token>::iterator &it);
		void	ParseAutoIndex(std::vector< t_token>::iterator &it);


	public :
		LocationConfig(std::vector<t_token> &tokenList, std::vector< t_token>::iterator &it, std::vector< LocationConfig > &locations);
		~LocationConfig();
		LocationConfig & operator=(LocationConfig const &otherLocationConfig);
		void	PrintLocation(void);
		void	Check(ServerConfig &server);

		std::string	getPath(void) const;
		std::string	getRoot(void) const;
		void		setPath(std::string newPath);
};

#endif