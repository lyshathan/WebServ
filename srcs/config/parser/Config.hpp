#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "../General.hpp"
#include "GlobalConfig.hpp"

class GlobalConfig;
class ServerConfig;

class Config {
	private :
		GlobalConfig				_globalConfig;
		std::vector< ServerConfig >	_serversConfig;

		int							_braceLevel;
		size_t						_lineNumber;
		size_t						_level;
		t_type						_expectedToken;
		std::string					_configFileName;
		std::vector<t_token>		_tokens;
		std::vector< std::vector<std::string> >	_validDir;

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
		void	CheckConfig();

	public :
		Config(std::string filename);
		~Config();
		void	PrintTokens();
		void	PrintConfig();

};

#endif



