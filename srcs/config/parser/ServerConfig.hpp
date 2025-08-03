#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include "../tokenizer/Token.hpp"
#include "ConfigIncludes.hpp"
#include "LocationConfig.hpp"

class ServerConfig {
	private :
		std::vector<t_token>			_tokens;
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
		void	ParseClientMaxBodySize(std::vector< t_token>::iterator &it);
		void	ParseErrorPage(std::vector< t_token>::iterator &it);
		void	AddToVector(std::vector< std::string > &vec, std::vector< t_token>::iterator &it);
		void	CheckForSemicolon(std::string type, std::vector< t_token>::iterator &it);


	public :
		ServerConfig(std::vector<t_token> &tokenList, std::vector< t_token>::iterator &it);
		~ServerConfig();
		void	PrintServer();
};

#endif