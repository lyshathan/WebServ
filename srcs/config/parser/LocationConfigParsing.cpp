#include "ServerConfig.hpp"
#include "LocationConfig.hpp"
#include "Utils.hpp"

void	LocationConfig::AddToVector(std::vector< std::string > &vec, std::vector< t_token>::iterator &it)
{
	std::ostringstream				errorMsg;
	std::vector< t_token>::iterator	tmp = it;

	it++;
	while (it != _tokens.end() &&  it->type == VALUE)
	{
		vec.push_back(it->content);
		it++;
	}
	if (it == _tokens.end() || it->type != SEMICOLON)
	{
		errorMsg << "[LocationConfig] Invalid " << tmp->content << "(semicolon)";
		throw std::invalid_argument(errorMsg.str());
	}
}

void	LocationConfig::ParsePath(std::vector< t_token>::iterator &it)
{
	std::string pathType = (it++)->content;
	size_t check = IsValidDirPath(it->content);
	if (check != VALID)
	{
		std::ostringstream errorMsg;
		errorMsg << "[ServerConfig] Error with path : " << it->content;
		if (check == NO_EXIST)
		{
			errorMsg << " : Path does not exist.";
			throw std::invalid_argument(errorMsg.str());
		}
		else if  (check == NOT_A_DIRECTORY)
		{
			errorMsg << " : Directory does not exist.";
			throw std::invalid_argument(errorMsg.str());
		}
		errorMsg << " : Directory permission denied.";
		throw std::invalid_argument(errorMsg.str());
	}

	if (pathType == "upload_path")
		_uploadPath = it->content;
	else if (pathType == "cgi_extension")
		_cgiExtension = it->content;
	else if (pathType == "cgi_path")
		_cgiPath = it->content;
	else if (pathType == "root")
		_root = it->content;
	ACheckForSemicolon(it, _tokens);
}

void	LocationConfig::ParseClientMaxBodySize(std::vector< t_token>::iterator &it)
{
	char *end;

	double bodySize = std::strtod((++it)->content.c_str(), &end);
	if (bodySize <= 0 || bodySize > INT_MAX)
		throw std::invalid_argument("[LocationConfig] Invalid client_max_body_size");
	_clientMaxBodySize = bodySize;
	ACheckForSemicolon(it, _tokens);
}

void	LocationConfig::ParseReturn(std::vector< t_token>::iterator &it)
{
	it++;
	char *end;

	double code = std::strtod(it->content.c_str(), &end);
	if (*end || code < 0 || code > INT_MAX)					// Need to validate the rules here ----------------
		throw std::invalid_argument("[LocationConfig] Invalid return");
	size_t check = IsValidFile((++it)->content);
	if (check != VALID)
	{
		std::ostringstream errorMsg;
		errorMsg << "[LocationConfig] Error with Error file : " << it->content;
		if (check == NO_EXIST)
		{
			errorMsg << " : Error file does not exist.";
			throw std::invalid_argument(errorMsg.str());
		}
		errorMsg << " : Error file permission denied.";
		throw std::invalid_argument(errorMsg.str());
	}
	_return[code] = it->content;
	ACheckForSemicolon(it, _tokens);
}

void	LocationConfig::ParseAutoIndex(std::vector< t_token>::iterator &it)
{
	it++;
	if (it->content == "on")
		_autoIndex = true;
	else if (it->content == "off")
		_autoIndex = false;
	else
		throw std::invalid_argument("[LocationConfig] Invalid autoindex");
	ACheckForSemicolon(it, _tokens);
}
