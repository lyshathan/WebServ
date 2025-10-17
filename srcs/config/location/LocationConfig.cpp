#include "../server/ServerConfig.hpp"
#include "LocationConfig.hpp"
#include "../utils/Utils.hpp"

////////////////////////////////////////////////////////////////////////////////////
//								Constructor & Destructor
////////////////////////////////////////////////////////////////////////////////////

LocationConfig::LocationConfig(ServerConfig &server, std::vector< LocationConfig > &locations, std::vector<t_token> &tokenList) :
	_locations(locations),
	_tokens(tokenList),
	_autoIndex(false),
	_clientMaxBodySize(server.getClientMaxBodySize()),
	_path("/"),
	_uploadPath(""),
	_cgiData(),
	_root(server.getRoot()),
	_indexFiles(server.getIndexFiles()),
	_allowMethod(),
	_validMethod(),
	_return(std::make_pair(0, "")),
	_errorPages(),
	_currentLevel(0),
	_isExactPath(false)
{
	_allowMethod.push_back("GET");
}

LocationConfig::LocationConfig(std::vector<t_token> &tokenList, std::vector< t_token>::iterator &it, std::vector< LocationConfig > &locations)
: _locations(locations),
	_tokens(tokenList),
	_autoIndex(false),
	_clientMaxBodySize(0),
	_path(""),
	_uploadPath(""),
	_cgiData(),
	_root(""),
	_indexFiles(),
	_allowMethod(),
	_validMethod(),
	_return(std::make_pair(0, "")),
	_errorPages(),
	_currentLevel(it->level),
	_isExactPath(false)
{
	_validMethod.push_back("GET");
	_validMethod.push_back("POST");
	_validMethod.push_back("DELETE");
	_validMethod.push_back("PUT");
	_validMethod.push_back("HEAD");

	locationConfigParser(it);
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
	this->_root = otherLocationConfig._root;
	this->_allowMethod = otherLocationConfig._allowMethod;
	this->_validMethod = otherLocationConfig._validMethod;
	this->_return = otherLocationConfig._return;
	this->_errorPages = otherLocationConfig._errorPages;
	return (*this);
}


void	LocationConfig::locationConfigParser(std::vector< t_token>::iterator &it)
{
	it++;
	if (it->type == EQUAL)
	{
		_isExactPath = true;
		it++;
	}
	_path = (it++)->content;
	for (std::vector< LocationConfig >::iterator itLoc = _locations.begin() ; itLoc != _locations.end() ; itLoc++)
	{
		if (itLoc->_path == this->_path && itLoc->_isExactPath == this->_isExactPath)
			throwErrorToken(" Already existing location path", *it);
	}
	if (it++->type != OPEN_BRACE)
		throwErrorToken(" Missing open brace", *it);
	while (!(it->type == CLOSE_BRACE && it->level == _currentLevel))
	{
		if (it->type == SEMICOLON)
		{
			it++;
			continue;
		}
		else if (it->type == DIRECTIVE && it->content == "allow_methods")
			addListToVector(_allowMethod, it, _tokens, &_validMethod);
		else if (it->type == DIRECTIVE && it->content == "autoindex")
			parseAutoIndex(it);
		else if (it->type == DIRECTIVE && it->content == "index")
			addListToVector(_indexFiles, it, _tokens, NULL);
		else if (it->type == DIRECTIVE
			&& (it->content == "upload_path" || it->content == "cgi" || it->content == "root"))
				parsePath(it);
		else if (it->type == DIRECTIVE && it->content == "client_max_body_size")
			parseClientMaxBodySize(it, _clientMaxBodySize, _tokens);
		else if (it->type == DIRECTIVE && it->content == "return")
			parseReturn(it);
		else if (it->type == DIRECTIVE && it->content == "error_page")
			parseErrorPage(it);
	}
	if (it->type == CLOSE_BRACE && it->level == _currentLevel)
		it++;
}

void	LocationConfig::check(ServerConfig &server)
{
	if (_clientMaxBodySize == 0)
		_clientMaxBodySize = server.getClientMaxBodySize(); // Set size inherited
	if (_root.empty())
		_root = server.getRoot();							// Set root inherited
	if (_allowMethod.empty())
		_allowMethod.push_back("GET");						// Set GET as default method
	if (_indexFiles.empty())
		_indexFiles = server.getIndexFiles();				// Set index inherited
	if (_errorPages.empty())
		_errorPages = server.getErrorPages();				// Set error pages inherited
	if (std::find(_allowMethod.begin(), _allowMethod.end(), "POST") != _allowMethod.end() && _uploadPath.empty())
		throwError(" Missing upload path for POST method");

	const std::map<int, std::string>& serverErrPagesMap = server.getErrorPages();
	std::map<int, std::string>::const_iterator serverErrPages = serverErrPagesMap.begin();
	if (serverErrPages != serverErrPagesMap.end()) {
		for (; serverErrPages != serverErrPagesMap.end(); ++serverErrPages) {
			if (_errorPages.find(serverErrPages->first) == _errorPages.end())
				_errorPages[serverErrPages->first] = serverErrPages->second;
		}
	}
}


void	LocationConfig::printLocation(void) const
{
	std::string indent = "|	|	|___ ";
	std::string list = "|	|	|	- ";

	// std::cerr << "|	|" << std::endl;
	// std::cerr << "|	|" << YELLOW << "==== LOCATION ====" << RESET << std::endl;
	// std::cerr << indent << "Path : " << _path << std::endl;
	// std::cerr << indent << "Allowed method :" << std::endl;
	for (std::vector<std::string>::const_iterator it = _allowMethod.begin() ; it != _allowMethod.end() ; it++)
	{
		// std::cerr << list << *it << std::endl;
	}
	// std::cerr << indent << "Index files : " << std::endl;
	for (std::vector<std::string>::const_iterator it = _indexFiles.begin() ; it != _indexFiles.end() ; it++)
	{
		// std::cerr << list << *it << std::endl;
	}
	// std::cerr << indent << "Auto Index : " << _autoIndex << std::endl;
	// std::cerr << indent << "Upload path : " << (_uploadPath == "" ? "UNDEFINED" : _uploadPath) << std::endl;
	// std::cerr << indent << "CGI: " << std::endl;
	std::map<std::string, std::string>::const_iterator it = _cgiData.begin();
	for (; it != _cgiData.end(); ++it) {
		// std::cerr << list << it->first << " | " << it->second << std::endl;
	}
	// std::cerr << indent << "Root : "  << (_root == "" ? "UNDEFINED" : _root) << std::endl;
	// std::cerr << indent << "Client body size max : " << _clientMaxBodySize << std::endl;
	// std::cerr << indent << "Error pages : " << std::endl;
	for (std::map< int , std::string >::const_iterator it = _errorPages.begin() ; it != _errorPages.end() ; it++)
	{
		// std::cerr << list << it->first << " | " << it->second << std::endl;
	}
	// std::cerr << indent << "Return : " << _return.first << " | " << (_return.second == "" ? "UNDEFINED" : _return.second) << std::endl;
}

