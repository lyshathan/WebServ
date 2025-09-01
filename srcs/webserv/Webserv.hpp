#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#include "Includes.hpp"
#include "../config/parser/Config.hpp"
#include "../config/parser/ServerConfig.hpp"

class Client;
class ServerConfig;

class Webserv {
	private :

		const Config						&_config;
		const std::vector<ServerConfig>		&_serverConfigs;

		std::vector<struct pollfd>	_pollFds;
		std::map<int, Client*>		_clients;
		std::vector<int>			_serverFds;
		int							_listenBackLog;
		std::vector<uint16_t>		_serverPorts;
		std::map<int, const ServerConfig*> _portToConfig;

		void		convertPorts(Config const &config);
		uint32_t	fromHostToAddress(std::string hostname);
		uint32_t	fromIPToInt(const std::string &IPstr);
		int			handleFunctionError(std::string errFunction);
		void		cleanServer();
		int			createServerSocket();
		int			setupListen();
		void		setupPollServer(Config const &config);
		int			runningServ();
		int			connectAndRead();
		int			acceptNewConnection(int &serverFd);
		int			readDataFromSocket(std::vector<struct pollfd>::iterator & it);
		void		addClient(int newClientFd, int &);
		void		deleteClient(int &clientFd, std::vector<struct pollfd>::iterator & it);
		const	ServerConfig* getConfigForPort(int);

	public :
		Webserv(Config const &config);
		~Webserv();
} ;

#endif