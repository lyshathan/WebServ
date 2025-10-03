#include "Config.hpp"
#include "server/ServerConfig.hpp"
#include "utils/Utils.hpp"
#include "../ProjectTools.hpp"

////////////////////////////////////////////////////////////////////////////////////
//								Constructor & Destructor
////////////////////////////////////////////////////////////////////////////////////

Config::Config(void): _braceLevel(0), _lineNumber(0), _level(GLOBAL), _expectedToken(DIRECTIVE), _quoteState(OUT) {}

Config::Config(std::string filename): _braceLevel(0), _lineNumber(0), _level(GLOBAL), _expectedToken(DIRECTIVE), _configFileName(filename), _quoteState(OUT)
{
	std::ifstream	configFile(filename.c_str());

	printLog(BLUE, "INFO", "Reading Config File: " + filename);
	initValidDirectives();
	initToken(configFile);

	parser();
	checkConfig();
}

Config::~Config(void) {}

////////////////////////////////////////////////////////////////////////////////////
//										Methods
////////////////////////////////////////////////////////////////////////////////////

Config & Config::operator=(Config const &otherConfig)
{
	this->_globalConfig = otherConfig._globalConfig;
	this->_serversConfig = otherConfig._serversConfig;
	this->_configFileName = otherConfig._configFileName;
	this->_tokens = otherConfig._tokens;
	return (*this);
}


void	Config::parser()
{
	GlobalConfig global;
	for (std::vector< t_token>::iterator it = _tokens.begin() ; it != _tokens.end() ; it++)
	{
		if (it->level == GLOBAL && it->type == DIRECTIVE)
		{
			if (it->content == "server")
			{
				ServerConfig newServer(_tokens, it, _serversConfig);
				_serversConfig.push_back(newServer);
				// newServer.printServer();
			}
			else if (it->content == "error_log")
			{
				global.setErrorLog((++it)->content);
				it++;
				if (it == _tokens.end() || it->type != SEMICOLON)
					throwErrorToken(" Invalid error_log", *it);
			}
			else if (it->content == "client_max_body_size")
			{
				global.setClientMaxBodySize((++it)->content, *it);
				it++;
				if (it == _tokens.end() || it->type != SEMICOLON)
					throwErrorToken(" Invalid client_max_body_size", *it);
			}
		}
		else
			throwErrorToken(" Invalid configuration file", *it);
	}
	_globalConfig = global;
}


void	Config::checkConfig(void)
{
	_globalConfig.check();
	if (_serversConfig.empty())
		throwError(" Need at least one server");
	for (std::vector< ServerConfig >::iterator itServ = _serversConfig.begin() ; itServ != _serversConfig.end() ; itServ++)
	{
		itServ->check(_globalConfig);
	}
}

const std::vector< ServerConfig > & Config::getServerConfig() const
{
	return (_serversConfig);
}


void	Config::PrintConfig(void)
{
	std::cout << "================================================================" << std::endl;
	_globalConfig.printGlobal();
	for (std::vector< ServerConfig >::iterator itServer = _serversConfig.begin() ; itServer != _serversConfig.end() ; itServer++)
	{
		itServer->printServer();
	}
	std::cout << "================================================================" << std::endl << std::endl;
}
