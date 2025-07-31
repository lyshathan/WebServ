#include "ConfigIncludes.hpp"
#include "Config.hpp"
#include "GlobalConfig.hpp"
#include "ServerConfig.hpp"

bool isNotSpace(unsigned char ch) {
    return !std::isspace(ch);
}

Config::Config(std::string filename)
{
	std::cout << "Config file : " << filename << std::endl;

	std::string	line;

	std::ifstream configFile(filename.c_str());
	while (getline(configFile, line))
	{
		std::string::iterator it = std::find_if(line.begin(), line.end(), isNotSpace);
        line = std::string(it, line.end());
		std::cout << line << std::endl;
	}
	
}

Config::~Config(void)
{

}
