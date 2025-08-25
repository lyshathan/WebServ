#include "ServerConfig.hpp"
#include "LocationConfig.hpp"
#include "Utils.hpp"


void	LocationConfig::ParsePath(std::vector< t_token>::iterator &it)
{
	std::string pathType = (it++)->content;
	size_t check = IsValidDirPath(it->content);
	if (check != VALID)
	{
		if (check == NO_EXIST)
			ThrowErrorToken(" Path does not exist", *it);
		else if  (check == NOT_A_DIRECTORY)
			ThrowErrorToken(" Directory does not exist", *it);
		ThrowErrorToken(" Directory permission denied", *it);
	}
	if (pathType == "upload_path")
		setString(_uploadPath, it->content, *it);
	else if (pathType == "cgi_extension")
		setString(_cgiExtension, it->content, *it);
	else if (pathType == "cgi_path")
		setString(_cgiPath, it->content, *it);
	else if (pathType == "root")
		setString(_root, it->content, *it);
	ACheckForSemicolon(it, _tokens);
}


void	LocationConfig::ParseReturn(std::vector< t_token>::iterator &it)
{
	it++;
	char *end;

	if (_return.first != 0)
		ThrowErrorToken(" Return already defined", *it);
	double code_d = std::strtod(it->content.c_str(), &end);
	if (*end || code_d < 0 || code_d > INT_MAX)					// Need to validate the rules here ----------------
		ThrowErrorToken(" Invalid return code", *it);
	int code = static_cast<int>(code_d);
	if ((++it)->type != SEMICOLON)
	{
		size_t check = IsValidFile(it->content);
		if (check != VALID)
		{
			if (check == NO_EXIST)
				ThrowErrorToken(" Error file does not exist", *it);
			ThrowErrorToken(" Error file permission denied", *it);
		}
		_return.second = it->content;
		ACheckForSemicolon(it, _tokens);
	}
	else
		_return.second = "";
	_return.first = code;
}

void	LocationConfig::ParseAutoIndex(std::vector< t_token>::iterator &it)
{
	it++;
	if (it->content == "on")
		_autoIndex = true;
	else if (it->content == "off")
		_autoIndex = false;
	else
		ThrowErrorToken(" Invalid autoindex", *it);
	ACheckForSemicolon(it, _tokens);
}


