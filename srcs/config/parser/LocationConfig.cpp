#include "ServerConfig.hpp"
#include "LocationConfig.hpp"
#include "Utils.hpp"

////////////////////////////////////////////////////////////////////////////////////
//								Constructor & Destructor
////////////////////////////////////////////////////////////////////////////////////

LocationConfig::LocationConfig(std::vector<t_token> &tokenList, std::vector< t_token>::iterator &it, std::vector< LocationConfig > &locations)
: _locations(locations), _tokens(tokenList), _autoIndex(false), _clientMaxBodySize(0), _currentLevel(it->level)
{
	_validMethod.push_back("GET");
	_validMethod.push_back("POST");
	_validMethod.push_back("DELETE");
	_validMethod.push_back("PUT");
	_validMethod.push_back("HEAD");

	LocationConfigParser(it);
}

LocationConfig::~LocationConfig(void)
{

}

////////////////////////////////////////////////////////////////////////////////////
//										Methods
////////////////////////////////////////////////////////////////////////////////////

LocationConfig & LocationConfig::operator=(LocationConfig const &otherLocationConfig)
{
	this->_autoIndex = otherLocationConfig._autoIndex;
	this->_clientMaxBodySize = otherLocationConfig._clientMaxBodySize;
	this->_path = otherLocationConfig._path;
	this->_uploadPath = otherLocationConfig._uploadPath;
	this->_cgiExtension = otherLocationConfig._cgiExtension;
	this->_cgiPath = otherLocationConfig._cgiPath;
	this->_root = otherLocationConfig._root;
	this->_allowMethod = otherLocationConfig._allowMethod;
	this->_validMethod = otherLocationConfig._validMethod;
	this->_return = otherLocationConfig._return;
	return (*this);
}



void	LocationConfig::LocationConfigParser(std::vector< t_token>::iterator &it)
{
	it++;
	_path = (it++)->content;
	for (std::vector< LocationConfig >::iterator itLoc = _locations.begin() ; itLoc != _locations.end() ; itLoc++)
	{
		if (itLoc->_path == this->_path)
			ThrowErrorToken(" Already existing location path", *it);
	}

	if (it++->type != OPEN_BRACE)
		ThrowErrorToken(" Missing open brace", *it);

	while (!(it->type == CLOSE_BRACE && it->level == _currentLevel))
	{
		// std::cout << "	[LocationConfig] it->content : " << it->content << RESET << std::endl;
		if (it->type == SEMICOLON)
		{
			it++;
			continue;
		}
		else if (it->type == DIRECTIVE && it->content == "allow_methods")	// allowed Method
			AddListToVector(_allowMethod, it, _tokens, &_validMethod);
		else if (it->type == DIRECTIVE && it->content == "autoindex")	// auto index
			ParseAutoIndex(it);
		else if (it->type == DIRECTIVE				// paths
			&& (it->content == "upload_path" || it->content == "cgi_extension" || it->content == "cgi_path" || it->content == "root"))
				ParsePath(it);
		else if (it->type == DIRECTIVE && it->content == "client_max_body_size")	// client_max_body_size
			ParseClientMaxBodySize(it, _clientMaxBodySize, _tokens);
		else if (it->type == DIRECTIVE && it->content == "return")	// return
			ParseReturn(it);
	}
	if (it->type == CLOSE_BRACE && it->level == _currentLevel)
		it++;
}

void	LocationConfig::Check(ServerConfig &server)
{
	if (_clientMaxBodySize == 0)
		_clientMaxBodySize = server.getClientMaxBodySize(); // Set size inherited
	if (_root.empty())
		_root = server.getRoot();					// Set root inherited
	if (_allowMethod.empty())
		_allowMethod.push_back("GET");				// Set GET as default method
	if (std::find(_allowMethod.begin(), _allowMethod.end(), "POST") != _allowMethod.end() && _uploadPath.empty())
		ThrowError(" Missing upload path for POST method");
	if (!_cgiExtension.empty() && _cgiPath.empty())
		ThrowError(" Missing CGI path for CGI extension");
}


void	LocationConfig::PrintLocation(void)
{
	std::string indent = "|	|	|___ ";
	std::string list = "|	|	|	- ";

	std::cout << "|	|" << std::endl;
	std::cout << "|	|" << YELLOW << "==== LOCATION ====" << RESET << std::endl;
	std::cout << indent << "Path : " << _path << std::endl;
	std::cout << indent << "Allowed method :" << std::endl;
	for (std::vector<std::string>::iterator it = _allowMethod.begin() ; it != _allowMethod.end() ; it++)
	{
		std::cout << list << *it << std::endl;
	}
	std::cout << indent << "Auto Index : " << _autoIndex << std::endl;
	std::cout << indent << "Upload path : " << (_uploadPath == "" ? "UNDEFINED" : _uploadPath) << std::endl;
	std::cout << indent << "CGI Extension : "  << (_cgiExtension == "" ? "UNDEFINED" : _cgiExtension) << std::endl;
	std::cout << indent << "CGI Path : "  << (_cgiPath == "" ? "UNDEFINED" : _cgiPath) << std::endl;
	std::cout << indent << "Root : "  << (_root == "" ? "UNDEFINED" : _root) << std::endl;
	std::cout << indent << "Client body size max : " << _clientMaxBodySize << std::endl;
	std::cout << indent << "Return : " << _return.first << " | " << (_return.second == "" ? "UNDEFINED" : _return.second) << std::endl;
}