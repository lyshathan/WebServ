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
		void	initValidDirectives();
		void	initToken(std::ifstream &configFile);
		bool	isValidDir(std::string content, size_t level);
		bool	isDir(std::string content);
		void	analyzeTokenContent(t_token &token);
		void	analyzeCaseBrace(t_token &token);
		void	analyzeCaseSemicolon(t_token &token);
		void	analyzeCaseDirOrValue(t_token &token);

		// PARSER
		void	parser();
		void	checkConfig();

	public :
		Config();
		Config(std::string filename);
		~Config();
		Config & operator=(Config const &otherConfig);
		
		void	printTokens();
		void	PrintConfig();
		const std::vector< ServerConfig > & getServerConfig() const;

};

#endif



