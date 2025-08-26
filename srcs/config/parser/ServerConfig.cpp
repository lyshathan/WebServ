#include "ServerConfig.hpp"
#include "Utils.hpp"

////////////////////////////////////////////////////////////////////////////////////
//								Constructor & Destructor
////////////////////////////////////////////////////////////////////////////////////

ServerConfig::ServerConfig(std::vector<t_token> &tokenList, std::vector< t_token>::iterator &it, std::vector< ServerConfig > &serversConfig)
: _serversConfig(serversConfig), _tokens(tokenList), _clientMaxBodySize(0),_currentLevel(it->level)
{
	ParseServerConfig(it);
}

ServerConfig::~ServerConfig(void)
{

}

////////////////////////////////////////////////////////////////////////////////////
//										Methods
////////////////////////////////////////////////////////////////////////////////////

ServerConfig & ServerConfig::operator=(ServerConfig const &otherServerConfig)
{
	this->_listenPorts = otherServerConfig._listenPorts;
	this->_serverNames = otherServerConfig._serverNames;
	this->_indexFiles = otherServerConfig._indexFiles;
	this->_root = otherServerConfig._root;
	this->_errorPages = otherServerConfig._errorPages;
	this->_clientMaxBodySize = otherServerConfig._clientMaxBodySize;
	this->_locations = otherServerConfig._locations;
	return (*this);
}



void	ServerConfig::ParseServerConfig(std::vector< t_token>::iterator &it)
{
	it++;
	if (it++->type != OPEN_BRACE)
		ThrowErrorToken(" Missing open brace", *it);

	while (!(it->type == CLOSE_BRACE && it->level == _currentLevel))
	{
		// std::cout << "[ServerConfig] it->content : " << it->content << RESET << std::endl;

		if (it->type == SEMICOLON)
		{
			it++;
			continue;
		}
		else if (it->type == DIRECTIVE && it->content == "listen")	// listen port
			ParseListenPort(it);
		else if (it->type == DIRECTIVE && it->content == "server_name")	// server names
			AddListToVector(_serverNames, it, _tokens, NULL);
		else if (it->type == DIRECTIVE && it->content == "index")	// index files
			AddListToVector(_indexFiles, it, _tokens, NULL);
		else if (it->type == DIRECTIVE && it->content == "root")	// root
			ParseRoot(it);
		else if (it->type == DIRECTIVE && it->content == "client_max_body_size")	// client_max_body_size
			ParseClientMaxBodySize(it, _clientMaxBodySize, _tokens);
		else if (it->type == DIRECTIVE && it->content == "error_page")	// error_page
			ParseErrorPage(it);
		else if (it->type == DIRECTIVE && it->content == "location")	// location
		{
			LocationConfig newLocation(_tokens, it, _locations);
			_locations.push_back(newLocation);
			// newLocation.PrintLocation();
		}
		else
			ThrowErrorToken(" Unknown directive", *it);
	}
}

void	ServerConfig::Check(GlobalConfig &global)
{
	if (_listenPorts.empty())
		ThrowError(" Server need at least a port");
	if (_root.empty())
		_root = "./www";										// Set default root
	if (_serverNames.empty())
		_serverNames.push_back("localhost"); 					// Set default servername
	if (_indexFiles.empty())
		_indexFiles.push_back("index.html");					// Set defaut index
	if (_clientMaxBodySize == 0)
		_clientMaxBodySize = global.getClientMaxBodySize();		// Inheritance of parent
	if (_errorPages.empty())
		_errorPages[404] = "www/404.html";						// Set default error page
	if (!_locations.empty())
	{
		for (std::vector< LocationConfig >::iterator itLoc = _locations.begin() ; itLoc != _locations.end() ; itLoc++)
		{
			itLoc->Check(*this);
		}
	}
	else
		std::cout << "No location set ???" << std::endl;		// What to do if no location set?

	SortLocation();
}

size_t	pathLenght(std::string path)
{
	size_t	count = 0;
	size_t pos = 0;

	pos = path.find("/", 0);

	while ((pos = path.find('/', pos)) != std::string::npos)
	{
		pos++;
		if (pos != path.size())
			count++;
	}
	return count;
}


void	ServerConfig::SortLocation(void)
{
	size_t	i = 0;

	while (i < (_locations.size() - 1))
	{
		size_t j = i + 1;
		while (j < _locations.size())
		{
			if (pathLenght(_locations[i].getPath()) > pathLenght(_locations[j].getPath()))
			{
				std::string tmp = _locations[i].getPath();
				_locations[i].setPath(_locations[j].getPath());
				_locations[j].setPath(tmp);
			}
			j++;
		}
		i++;
	}
}



void	ServerConfig::PrintServer(void)
{
	std::string indent = "|	|___ ";
	std::string list = "|	|	- ";

	std::cout << "|" << std::endl;
	std::cout << "|" << PURPLE << "==== SERVER ========" << RESET << std::endl;
	std::cout << indent << "Listen IP:port :" << std::endl;
	// for (std::vector<int>::iterator it = _listenPorts.begin() ; it != _listenPorts.end() ; it++)
	// {
	// 	std::cout << list << *it << std::endl;
	// }

	for (std::map<uint16_t, std::string>::iterator it = _portAndIP.begin() ; it != _portAndIP.end() ; it++)
	{
		std::cout << list << it->second << ":" << it->first << std::endl;
	}
	std::cout << indent << "Server Names :" << std::endl;
	for (std::vector<std::string>::iterator it = _serverNames.begin() ; it != _serverNames.end() ; it++)
	{
		std::cout << list << *it << std::endl;
	}
	std::cout << indent << "Root : " << (_root == "" ? "UNDEFINED" : _root) << std::endl;
	std::cout << indent << "Index files : " << std::endl;
	for (std::vector<std::string>::iterator it = _indexFiles.begin() ; it != _indexFiles.end() ; it++)
	{
		std::cout << list << *it << std::endl;
	}
	std::cout << indent << "Client max body size : " << _clientMaxBodySize << std::endl;
	std::cout << indent << "Error pages : " << std::endl;
	for (std::map< int , std::string >::iterator it = _errorPages.begin() ; it != _errorPages.end() ; it++)
	{
		std::cout << list << it->first << " | " << it->second << std::endl;
	}
	for (std::vector< LocationConfig >::iterator it = _locations.begin() ; it != _locations.end() ; it++)
	{
		it->PrintLocation();
	}
}

