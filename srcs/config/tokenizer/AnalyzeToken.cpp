#include "../parser/Config.hpp"

void Config::AnalyzeTokenContent(t_token &token)
{
	// std::string level[] = {"global", "service", "location"};
	// std::string type[] = {"DIRECTIVE", "VALUE", "OPEN_BRACE", "CLOSE_BRACE", "SEMICOLON", "SEMICOLON_OR_VALUE", "PATH", "UNDEFINED"};

	// std::cout << GREEN << token.content << RESET << "	" ;
	std::ostringstream	errorMsg;
	token.line = _lineNumber;

	if (token.content == "{" || token.content == "}")
		AnalyzeCaseBrace(token);
	else if (token.content == ";")
		AnalyzeCaseSemicolon(token);
	else
		AnalyzeCaseDirOrValue(token);
	token.level = _level;
	// std::cout << "Type : " << type[token.type] << " - Expected token type : " << type[_expectedToken] << " - level : " << level[token.level] << std::endl;
}

void	Config::AnalyzeCaseBrace(t_token &token)
{
	std::ostringstream	errorMsg;
	if (token.content == "{")
	{
		if (_expectedToken != OPEN_BRACE)
		{
			std::cout << RED << "[Config] unexpected \"{\" in " << _configFileName << ": " << token.line << RESET << std::endl;
			throw std::invalid_argument(errorMsg.str());
		}
		token.type = OPEN_BRACE;
		_expectedToken = DIRECTIVE;
		_level++;
	}
	else if (token.content == "}")
	{
		token.type = CLOSE_BRACE;
		if (_level == 0)
		{
			std::cout << RED << "[Config] unexpected \"}\" in " << _configFileName << ": " << token.line << RESET << std::endl;
			throw std::invalid_argument(errorMsg.str());
		}
		_expectedToken = DIRECTIVE;
		_level--;
	}
}

void	Config::AnalyzeCaseSemicolon(t_token &token)
{
	std::ostringstream	errorMsg;
	if (_expectedToken != SEMICOLON_OR_VALUE)
		{
			std::cout << RED << "[Config] unexpected end of file, expecting \";\" or \"}\" in " << _configFileName << ": " << token.line << RESET << std::endl;
			throw std::invalid_argument(errorMsg.str());
		}
		token.type = SEMICOLON;
}

void	Config::AnalyzeCaseDirOrValue(t_token &token)
{
	std::ostringstream	errorMsg;
	if (_tokens.size() != 0 && (_tokens.back().type == DIRECTIVE || _tokens.back().type == VALUE))
	{
		if (_expectedToken != VALUE && _expectedToken != SEMICOLON_OR_VALUE && _expectedToken != PATH)
		{
			std::cout << RED << "[Config] missing value in " << _configFileName << ": " << token.line << RESET << std::endl;
			throw std::invalid_argument(errorMsg.str());
		}
		if (_tokens.back().content == "location")
		{
			token.type = PATH;
			_expectedToken = OPEN_BRACE;
		}
		else
		{
			token.type = VALUE;
			_expectedToken = SEMICOLON_OR_VALUE;
		}
	}
	else
	{
		if (!IsValidDir(token.content, _level))
		{
			if (!IsDir(token.content))
				std::cout << RED << "[Config] \"" << token.content << "\"directive is not allowed here in " << _configFileName << ": " << token.line << RESET << std::endl;
			else
				std::cout << RED << "[Config] unknown directive \"" << token.content << "\" in " << _configFileName << ": " << token.line << RESET << std::endl;
			throw std::invalid_argument(errorMsg.str());
		}
		token.type = DIRECTIVE;
		if (token.content == "location")
			_expectedToken = PATH;
		if (_level == GLOBAL && token.content == "server")
			_expectedToken = OPEN_BRACE;
		else
			_expectedToken = VALUE;
	}
}

bool Config::IsValidDir(std::string content, size_t level)
{
	std::vector<std::string>::iterator find = std::find(_validDir[level].begin(), _validDir[level].end(), content);
	if (find != _validDir[level].end())
		return (true);
	return (false);
}

bool Config::IsDir(std::string content)
{
	for (size_t level = 0 ; level < 3 ; level++)
	{
		std::vector<std::string>::iterator find = std::find(_validDir[level].begin(), _validDir[level].end(), content);
		if (find != _validDir[level].end())
			return (true);
	}
	return (false);
}
