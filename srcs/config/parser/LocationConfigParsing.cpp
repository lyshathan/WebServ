#include "ServerConfig.hpp"
#include "LocationConfig.hpp"
#include "../tokenizer/Token.hpp"

void	LocationConfig::CheckForSemicolon(std::string type, std::vector< t_token>::iterator &it)
{
	std::ostringstream				errorMsg;
	std::vector< t_token>::iterator	tmp = it;

	it++;
	if (it == _tokens.end() || it->type != SEMICOLON)
	{
		errorMsg << "[LocationConfig] Invalid " << type << "(semicolon)";
		throw std::invalid_argument(errorMsg.str());
	}
}

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
	std::string str = it->content;
	if (it->content == "upload_path")
		_uploadPath = (++it)->content;
	else if (it->content == "cgi_extension")
		_cgiExtension = (++it)->content;
	else if (it->content == "cgi_path")
		_cgiPath = (++it)->content;
	else if (it->content == "root")
		_root = (++it)->content;
	CheckForSemicolon(str, it);
}

void	LocationConfig::ParseClientMaxBodySize(std::vector< t_token>::iterator &it)
{
	char *end;

	double bodySize = std::strtod((++it)->content.c_str(), &end);
	if (bodySize <= 0 || bodySize > INT_MAX)
		throw std::invalid_argument("[LocationConfig] Invalid client_max_body_size");
	_clientMaxBodySize = bodySize;
	CheckForSemicolon("client_max_body_size", it);
}

void	LocationConfig::ParseReturn(std::vector< t_token>::iterator &it)
{
	it++;
	char *end;

	double code = std::strtod(it->content.c_str(), &end);
	if (*end || code < 0 || code > INT_MAX)					// Need to validate the rules here ----------------
		throw std::invalid_argument("[LocationConfig] Invalid return");
	_return[code] = (++it)->content;
	CheckForSemicolon("return", it);
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
	CheckForSemicolon("autoindex", it);
}
