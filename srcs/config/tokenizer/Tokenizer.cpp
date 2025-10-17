#include "../Config.hpp"

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
	LocationDir.push_back("cgi");
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
	std::string		word;

	while (getline(configFile, line))
	{
		_lineNumber++;

		if (_quoteState != OUT && !word.empty())
		{
			word += " ";
		}

		for (size_t i = 0 ; i < line.length() ; )
		{
			// If not in quoted state, skip whitespaces
			if (_quoteState == OUT)
			{
				while (line[i] && myIsSpace(line[i]))
					i++;
				if (line[i] == '#')
					break;
			}

			if (line[i] == '\"' || line[i] == '\'' || _quoteState != OUT)
			{
				std::string newpart = handleQuotedToken(line, &i);
				word += newpart;
				if (_quoteState == OUT)
				{
					if (!word.empty())
					{
						token.content = word;
						analyzeTokenContent(token);
						_tokens.push_back(token);
						word = "";
					}
					continue;
				}
				else
				{
					break;
				}
			}
			if (_quoteState == OUT)
			{
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
					word = "";
				}
			}
		}
	}

	// Check for unclosed quotes
	if (_quoteState != OUT)
		throwErrorToken(" Unclosed quote ", token);

	if (_level != GLOBAL)
		throwErrorToken(" Level error ", token);
	if (_tokens.empty())
		throwError(" Empty config file ");
	else if (_tokens.back().type != SEMICOLON && _tokens.back().type != CLOSE_BRACE)
		throwErrorToken(" Unexpected end of file ", token);
}

std::string Config::handleQuotedToken(std::string &line, size_t *i)
{
	std::string word;

	if (_quoteState == OUT)
	{
		if (*i == '\'')
			_quoteState = SINGLE;
		else
			_quoteState = DOUBLE;
		(*i)++;
	}

	while (line[*i] && _quoteState != OUT)
	{
		if ((_quoteState == SINGLE && line[*i] == '\'') || (_quoteState == DOUBLE && line[*i] == '\"'))
		{
			(*i)++;
			_quoteState = OUT;
			break;
		}
		word += line[*i];
		(*i)++;
	}
	return (word);

}

void	Config::printTokens()
{
	std::string type[] = {"DIRECTIVE", "VALUE", "OPEN_BRACE", "CLOSE_BRACE", "SEMICOLON", "SEMICOLON_OR_VALUE", "PATH", "EQUAL" ,"UNDEFINED"};

	for (size_t i = 0 ; i < _tokens.size() ; i++)
	{
		std::cerr << "#" << i
			<< "	l:" << _tokens[i].line
			<< PURPLE << "	lvl:" << _tokens[i].level << RESET
			<< "	[" << _tokens[i].content << "]"
			<< "	"<< YELLOW << type[_tokens[i].type] << RESET << std::endl;
	}
}
