void Config::AnalyseTokenContent(t_token &token)
{
	std::cout << GREEN << "Analyse" << RESET ;
	// std::ostringstream	errorMsg;
	token.line = _lineNumber;

	if (token.content == "{")
	{
		// if (_expectedToken != OPEN_BRACE)
		// {
		// 	std::cout << RED << "[Config] unexpected \"{\" in " << _configFileName << ": " << token.line << RESET << std::endl;
		// 	throw std::invalid_argument(errorMsg.str());
		// }
		token.type = OPEN_BRACE;
		// _expectedToken = DIRECTIVE;
		_level++;
	}
	else if (token.content == "}")
	{
		token.type = CLOSE_BRACE;
		// if (_level == 0)
		// {
		// 	std::cout << RED << "[Config] unexpected \"}\" in " << _configFileName << ": " << token.line << RESET << std::endl;
		// 	throw std::invalid_argument(errorMsg.str());
		// }
		// _expectedToken = DIRECTIVE;
		_level--;
	}
	else if (token.content == ";")
	{
		// if (_expectedToken != SEMICOLON_OR_VALUE)
		// {
		// 	std::cout << RED << "[Config] unexpected end of file, expecting \";\" or \"}\" in " << _configFileName << ": " << token.line << RESET << std::endl;
		// 	throw std::invalid_argument(errorMsg.str());
		// }
		token.type = SEMICOLON;
	}
	else
	{
		if (_tokens.size() != 0 || _tokens.back().type == DIRECTIVE || _tokens.back().type == VALUE)
		{
			std::cout << "VALUE size : " << _tokens.size();
			token.type = VALUE;
			// _expectedToken = SEMICOLON_OR_VALUE;
		}
		else
		{
			std::cout << "DIRECTIVE level : " << _level;
			// if (!IsValidDir(token.content, _level))
			// {
			// 	if (!IsDir(token.content))
			// 		std::cout << RED << "[Config] \"" << token.content << "\"directive is not allowed here in " << _configFileName << ": " << token.line << RESET << std::endl;
			// 	else
			// 		std::cout << RED << "[Config] unknown directive \"" << token.content << "\" in " << _configFileName << ": " << token.line << RESET << std::endl;
			// 	throw std::invalid_argument(errorMsg.str());
			// }
			token.type = DIRECTIVE;
			// if (_level == GLOBAL)
			// 	_expectedToken = OPEN_BRACE;
			// else
			// 	_expectedToken = VALUE;
		}
	}
	token.level = _level;
	// std::cout << "Expected token type : " << _expectedToken << std::endl;
}


		// else if (it->type == DIRECTIVE && it->content == "root")	// root
		// {
		// 	_root = (++it)->content;
		// 	it++;
		// 	if (it == _tokens.end() || it->type != SEMICOLON)
		// 		throw std::invalid_argument("[ServerConfig] Invalid root (semicolon)");
		// }

		// else if (it->type == DIRECTIVE && it->content == "index")	// index files
		// {
		// 	it++;
		// 	while (it != _tokens.end() &&  it->type == VALUE)
		// 	{
		// 		_indexFiles.push_back(it->content);
		// 		it++;
		// 	}
		// }

		// else if (it->type == DIRECTIVE && it->content == "client_max_body_size")	// client_max_body_size
		// {
		// 	double bodySize = std::strtod((++it)->content.c_str(), &end);
		// 	if (bodySize <= 0 || bodySize > INT_MAX)
		// 		throw std::invalid_argument("[ServerConfig] Invalid client_max_body_size");
		// 	_clientMaxBodySize = bodySize;
		// 	it++;
		// 	if (it == _tokens.end() || it->type != SEMICOLON)
		// 		throw std::invalid_argument("[ServerConfig] Invalid client_max_body_size (semicolon)");
		// }