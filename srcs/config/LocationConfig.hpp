#ifndef LOCATIONCONFIG_HPP
#define LOCATIONCONFIG_HPP

#include "ConfigIncludes.hpp"

class LocationConfig {
	private :
		std::string					_path;
		std::vector< std::string >	_allowMethod;
		bool						_autoIndex;
		std::string					_uploadPath;
		std::string					_cgiExtension;
		std::string					_cgiPath;
		size_t						_clientMaxBodySize;
		std::string					_redirectURL;
		int							_redirectCode;

	public :
		LocationConfig();
		~LocationConfig();
};

#endif