#ifndef LOCATIONCONFIG_HPP
#define LOCATIONCONFIG_HPP

#include "ConfigIncludes.hpp"
#include "../tokenizer/Token.hpp"

class LocationConfig {
	private :
		std::vector<t_token>		_tokens;
		std::string					_path;
		std::vector< std::string >	_allowMethod;
		bool						_autoIndex;
		std::string					_uploadPath;
		std::string					_cgiExtension;
		std::string					_cgiPath;
		std::string					_root;
		std::map< int , std::string >	_return;
		size_t						_clientMaxBodySize;

		const size_t				_currentLevel;

		void	LocationConfigParser(std::vector< t_token>::iterator &it);
		void	CheckForSemicolon(std::string type, std::vector< t_token>::iterator &it);
		void	AddToVector(std::vector< std::string > &vec, std::vector< t_token>::iterator &it);
		void	ParsePath(std::vector< t_token>::iterator &it);
		void	ParseReturn(std::vector< t_token>::iterator &it);
		void	ParseAutoIndex(std::vector< t_token>::iterator &it);
		void	ParseClientMaxBodySize(std::vector< t_token>::iterator &it);

	public :
		LocationConfig(std::vector<t_token> &tokenList, std::vector< t_token>::iterator &it);
		~LocationConfig();
		void	PrintLocation(void);
};

#endif