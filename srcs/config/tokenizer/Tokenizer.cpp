#include "../parser/Config.hpp"

bool myIsSpace(unsigned char c);
bool isSpecialChar(unsigned char c);

void Config::initValidDirectives(void)
{
	std::vector<std::string> GlobalDir;
	GlobalDir.push_back("error_log");
	GlobalDir.push_back("client_max_body_size");
	GlobalDir.push_back("server");

	std::vector<std::string> ServiceDir;
	ServiceDir.push_back("listen");
	ServiceDir.push_back("host");
	ServiceDir.push_back("server_name");
	ServiceDir.push_back("root");
	ServiceDir.push_back("index");
	ServiceDir.push_back("error_page");
	ServiceDir.push_back("client_max_body_size");
	ServiceDir.push_back("location");

	std::vector<std::string> LocationDir;
	LocationDir.push_back("index");
	LocationDir.push_back("allow_methods");
	LocationDir.push_back("autoindex");
	LocationDir.push_back("cgi_extension");
	LocationDir.push_back("cgi_path");
	LocationDir.push_back("upload_path");
	LocationDir.push_back("return");
	LocationDir.push_back("root");
	LocationDir.push_back("client_max_body_size");
	LocationDir.push_back("error_page");

	_validDir.push_back(GlobalDir);
	_validDir.push_back(ServiceDir);
	_validDir.push_back(LocationDir);
}

void Config::initToken(std::ifstream &configFile)
{
	std::string		line;
	t_token			token;

	while (getline(configFile, line))
	{
		_lineNumber++;
		for (size_t i = 0 ; i < line.length() ; )
		{
			std::string	word;
			while (line[i] && myIsSpace(line[i]))
				i++;
			if (line[i] == '#')
				break;
			if (isSpecialChar(line[i]))
			{
				token.content = line[i];
				analyzeTokenContent(token);
				_tokens.push_back(token);
				i++;
			}
			while (line[i] && !myIsSpace(line[i]) && !isSpecialChar(line[i]))
			{
				word += line[i];
				i++;
			}
			if (!word.empty())
			{
				token.content = word;
				analyzeTokenContent(token);
				_tokens.push_back(token);
			}
		}
	}
	if (_level != GLOBAL)
		throwErrorToken(" Level error ", token);
	if (_tokens.back().type != SEMICOLON && _tokens.back().type != CLOSE_BRACE)
		throwErrorToken(" Unexpected end of file ", token);
	// printTokens();
}

void	Config::printTokens()
{
	std::string type[] = {"DIRECTIVE", "VALUE", "OPEN_BRACE", "CLOSE_BRACE", "SEMICOLON", "SEMICOLON_OR_VALUE", "PATH", "EQUAL" ,"UNDEFINED"};

	for (size_t i = 0 ; i < _tokens.size() ; i++)
	{
		std::cout << "#" << i
			<< "	l:" << _tokens[i].line
			<< PURPLE << "	lvl:" << _tokens[i].level << RESET
			<< "	[" << _tokens[i].content << "]"
			<< "	"<< YELLOW << type[_tokens[i].type] << RESET << std::endl;
	}
}
