#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "ConfigIncludes.hpp"
#include "GlobalConfig.hpp"

class GlobalConfig;
class ServerConfig;

class Config {
	private :
		GlobalConfig				_globalConfig;
		std::vector< ServerConfig >	_serversConfig;

	public :
		Config(std::string filename);
		~Config();
};

#endif