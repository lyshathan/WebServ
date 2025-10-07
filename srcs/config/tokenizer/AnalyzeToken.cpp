#include "../Config.hpp"

void Config::analyzeTokenContent(t_token &token)
{
	std::ostringstream	errorMsg;
	token.line = _lineNumber;

	if (token.content == "{" || token.content == "}")
		analyzeCaseBrace(token);
	else if (token.content == ";")
		analyzeCaseSemicolon(token);
	else
		analyzeCaseDirOrValue(token);
	token.level = _level;
}

void	Config::analyzeCaseBrace(t_token &token)
{
	std::ostringstream	errorMsg;
	if (token.content == "{")
	{
		if (_expectedToken != OPEN_BRACE)
			throwErrorToken(" Unexpected \"{\" ", token);
		token.type = OPEN_BRACE;
		_expectedToken = DIRECTIVE;
		_level++;
	}
	else if (token.content == "}")
	{
		token.type = CLOSE_BRACE;
		if (_level == 0)
			throwErrorToken(" Unexpected \"}\" ", token);
		_expectedToken = DIRECTIVE;
		_level--;
	}
}

void	Config::analyzeCaseSemicolon(t_token &token)
{
	if (_expectedToken != SEMICOLON_OR_VALUE)
		throwErrorToken(" Unexpected end of file, expecting \";\" or \"}\"", token);
	token.type = SEMICOLON;
	_expectedToken = UNDEFINED;
}

void	Config::analyzeCaseDirOrValue(t_token &token)
{
	if (_tokens.size() != 0 && (_tokens.back().type == DIRECTIVE || _tokens.back().type == VALUE  || _tokens.back().type == EQUAL) && token.content != "location")
	{
		if (_expectedToken != VALUE && _expectedToken != SEMICOLON_OR_VALUE && _expectedToken != PATH)
			throwErrorToken(" Missing value", token);
		if (_tokens.back().content == "location" || _tokens.back().type == EQUAL)
		{
			if (token.content == "=")
			{
				token.type = EQUAL;
				_expectedToken = PATH;
			}
			else
			{
				if (_expectedToken != PATH)
					throwErrorToken(" Expecting path", token);
				token.type = PATH;
				_expectedToken = OPEN_BRACE;
			}
		}
		else
		{
			token.type = VALUE;
			_expectedToken = SEMICOLON_OR_VALUE;
		}
	}
	else
	{
		if (_expectedToken == SEMICOLON || _expectedToken == SEMICOLON_OR_VALUE)
			throwErrorToken(" Unexpected end of file, expecting \";\" or \"}\"", token);
		if (!isValidDir(token.content, _level))
		{
			if (isDir(token.content))
				throwErrorToken(" Directive not allowed here", token);
			else
				throwErrorToken(" Unknown directive", token);
		}
		token.type = DIRECTIVE;
		if (token.content == "location")
			_expectedToken = PATH;
		else if (_level == GLOBAL && token.content == "server")	////
			_expectedToken = OPEN_BRACE;
		else
			_expectedToken = VALUE;
	}
}

bool Config::isValidDir(std::string content, size_t level)
{
	std::vector<std::string>::iterator find = std::find(_validDir[level].begin(), _validDir[level].end(), content);
	if (find != _validDir[level].end())
		return (true);
	return (false);
}

bool Config::isDir(std::string content)
{
	for (size_t level = 0 ; level < 3 ; level++)
	{
		std::vector<std::string>::iterator find = std::find(_validDir[level].begin(), _validDir[level].end(), content);
		if (find != _validDir[level].end())
			return (true);
	}
	return (false);
}
