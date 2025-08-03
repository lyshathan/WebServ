#ifndef LOCATIONCONFIG_HPP
#define LOCATIONCONFIG_HPP

#include "ConfigIncludes.hpp"
#include "Token.hpp"

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


	public :
		LocationConfig(std::vector<t_token> &tokenList, std::vector< t_token>::iterator &it);
		~LocationConfig();
		void	PrintLocation(void);
};

#endif