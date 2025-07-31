#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "ConfigIncludes.hpp"
#include "GlobalConfig.hpp"
#include "Token.hpp"

class GlobalConfig;
class ServerConfig;

class Config {
	private :
		GlobalConfig				_globalConfig;
		std::vector< ServerConfig >	_serversConfig;
		int							_braceLevel;
		std::vector<t_token>		_tokens;
		size_t						_lineNumber;
		std::vector<std::string>	_validDirective;

		void	AnalyseTokenContent(t_token &token);
		void	CleanNewLine();

		class ConfigException : public std::exception
		{
			public:
				const char* what() const throw();
		};

	public :
		Config(std::string filename);
		~Config();
		void	PrintTokens();
};

#endif