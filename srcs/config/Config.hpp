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
		size_t						_level;
		t_type						_expectedToken;
		std::vector< std::vector<std::string> >	_validDir;
		std::string					_configFileName;

		// TOKENIZER
		void	InitValidDirectives();
		void	InitToken(std::ifstream &configFile);
		bool	IsValidDir(std::string content, size_t level);
		bool	IsDir(std::string content);
		void	AnalyzeTokenContent(t_token &token);
		void	AnalyzeCaseBrace(t_token &token);
		void	AnalyzeCaseSemicolon(t_token &token);
		void	AnalyzeCaseDirOrValue(t_token &token);

		// PARSER
		void	Parser();

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



