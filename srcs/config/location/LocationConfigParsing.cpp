#include "../server/ServerConfig.hpp"
#include "LocationConfig.hpp"
#include "../utils/Utils.hpp"

void	LocationConfig::parseCGI(std::vector< t_token>::iterator &it) {
	std::istringstream	ss(it->content);
	struct stat			statbuf;
	std::string			extension;
	std::string			path;
	std::string			extra;

	if (!(ss >> extension >> path))
		throwErrorToken("Failed to parse CGI directive", *it);
	if (ss >> extra)
		throwErrorToken("Too many arguments for CGI directive", *it);
	if (_cgiData.find(extension) != _cgiData.end())
		throwErrorToken("CGI extension already defined", *it);
	if (!extension.empty() && !path.empty())
		_cgiData[extension] = path;
	if (stat(path.c_str(), &statbuf) != 0)
		throwErrorToken("CGI executable does not exist", *it);
	if (!(statbuf.st_mode & S_IXUSR))
		throwErrorToken("CGI executable is not executable", *it);
}

void	LocationConfig::parsePath(std::vector< t_token>::iterator &it)
{
	std::string pathType = (it++)->content;
	// size_t check = isValidDirPath(it->content);
	// if (check != VALID)
	// {
	// 	if (check == NO_EXIST)
	// 		throwErrorToken(" Path does not exist", *it);
	// 	else if  (check == NOT_A_DIRECTORY)
	// 		throwErrorToken(" Directory does not exist", *it);
	// 	throwErrorToken(" Directory permission denied", *it);
	// }
	if (pathType == "upload_path")
		setString(_uploadPath, it->content, *it);
	else if (pathType == "cgi")
		parseCGI(it);
	else if (pathType == "root")
		setString(_root, it->content, *it);
	checkForSemicolon(it, _tokens);
}


void	LocationConfig::parseReturn(std::vector< t_token>::iterator &it)
{
	it++;
	char *end;

	if (_return.first != 0)
		throwErrorToken(" Return already defined", *it);
	double code_d = std::strtod(it->content.c_str(), &end);
	if (*end || code_d < 301 || code_d > 308)					// Need to validate the rules here ----------------
		throwErrorToken(" Invalid return code", *it);
	int code = static_cast<int>(code_d);
	if ((++it)->type != SEMICOLON)
	{
		// size_t check = isValidFile(it->content);
		// if (check != VALID)
		// {
		// 	if (check == NO_EXIST)
		// 		throwErrorToken(" Error file does not exist", *it);
		// 	throwErrorToken(" Error file permission denied", *it);
		// }
		_return.second = it->content;
		checkForSemicolon(it, _tokens);
	}
	else
		_return.second = "";
	_return.first = code;
}

void	LocationConfig::parseAutoIndex(std::vector< t_token>::iterator &it)
{
	it++;
	if (it->content == "on")
		_autoIndex = true;
	else if (it->content == "off")
		_autoIndex = false;
	else
		throwErrorToken(" Invalid autoindex", *it);
	checkForSemicolon(it, _tokens);
}

void	LocationConfig::parseErrorPage(std::vector< t_token>::iterator &it)
{
	char *end;

	it++;
	size_t	nbOfErrorCode = CountErrorCodes(_tokens, it);
	std::vector< t_token>::iterator itErrorPage = it + nbOfErrorCode - 1;
	while (it != itErrorPage)
	{
		double code_d = std::strtod(it->content.c_str(), &end);
		if (*end || code_d < 300 || code_d > 599)
			throwErrorToken(" Invalid error code", *it);
		int code = static_cast<int>(code_d);
		if (_errorPages.find(code) != _errorPages.end())
			throwErrorToken(" Already existing code", *it);
		_errorPages[code] = itErrorPage->content;
		it++;
	}
	checkForSemicolon(it, _tokens);
}