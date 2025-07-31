#include "ConfigIncludes.hpp"
#include "Config.hpp"
#include "GlobalConfig.hpp"
#include "ServerConfig.hpp"

bool myIsSpace(unsigned char c) {
    return (c == ' ' || c == '\t' || c == '\r' || c == '\f' || c == '\v');
}

bool	isSpecialChar(unsigned char c)
{
	return (c == '{' || c == '}'  || c == '#'  || c == ';');
}

void Config::CleanNewLine(void)
{
	for (std::vector<t_token>::iterator it = _tokens.begin() ; it != _tokens.end() ; it++)
	{
		if (it->type == NEWLINE)
		{
			it = _tokens.erase(it);
			if (it != _tokens.begin())
				it--;
		}
	}
}

void Config::AnalyseTokenContent(t_token &token)
{
	token.line = _lineNumber;

	if (token.content == "#")
		token.type = COMMENT;
	else if (token.content == "{")
		token.type = OPEN_BRACE;
	else if (token.content == "}")
		token.type = CLOSE_BRACE;
	else if (token.content == ";")
		token.type = SEMICOLON;
	else if (token.content == "\n")
		token.type = NEWLINE;
	else
	{
		if (_tokens.size() == 0 || _tokens.back().type == NEWLINE)
		{
			std::vector<std::string>::iterator find = std::find(_validDirective.begin(), _validDirective.end(), token.content);
			if (find == _validDirective.end())
			{
				std::cout << "invalid directive " << token.content <<std::endl;
				// std::ostringstream oss;
				// oss << "Config file invalid at :" << token.line;
				// throw std::invalid_argument(oss.str());
			}
			token.type = DIRECTIVE;
		}
		else
			token.type = VALUE;
	}
}

Config::Config(std::string filename): _braceLevel(0), _lineNumber(0)
{
	std::string	line;
	std::ifstream configFile(filename.c_str());
	t_token	token;

	std::cout << BLUE << "Reading from config file : " << filename << RESET <<std::endl;

	_validDirective.push_back("error_log");
	_validDirective.push_back("client_max_body_size");
	_validDirective.push_back("server");
	_validDirective.push_back("listen");
	_validDirective.push_back("server_name");

	while (getline(configFile, line))
	{
		_lineNumber++;

		for (size_t i = 0 ; i < line.length() ; )
		{
			std::string	word;

			if (line[i] == '#')
				break;
			if (isSpecialChar(line[i]))
			{
				token.content = line[i];
				AnalyseTokenContent(token);
				_tokens.push_back(token);
				i++;
			}
			while (line[i] && myIsSpace(line[i]))
			{
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
				AnalyseTokenContent(token);
				_tokens.push_back(token);
			}
		}
		token.content = "\n";
		AnalyseTokenContent(token);
		_tokens.push_back(token);
	}

	CleanNewLine();
}

void	Config::PrintTokens()
{
	std::string type[] = {"DIRECTIVE", "VALUE", "OPEN_BRACE", "CLOSE_BRACE", "SEMICOLON", "COMMENT", "NEWLINE"};

	for (int i = 0 ; i < _tokens.size() ; i++)
	{
		std::cout << "#" << i << "	[" 
			<< _tokens[i].line << "]	["
			<< _tokens[i].content << "]	"
			<< YELLOW << type[_tokens[i].type] << RESET << std::endl;
	}
}


Config::~Config(void)
{
	std::cout << RED << "Conf destructor called" << RESET <<std::endl;
}

const char* Config::ConfigException::what() const throw()
{
	return "Error: Invalid configuration file at line : ";
}