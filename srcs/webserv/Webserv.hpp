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

		std::vector<int>			_serverFds;
		std::map<int, Client*>		_clients;
		std::vector<struct pollfd>	_pollFds;
		std::map< int, std::pair< uint16_t, std::string> >	_serverInfos;
		int							_listenBackLog;


		uint32_t	fromHostToAddress(std::string hostname);
		uint32_t	fromIPToInt(const std::string &IPstr);
		int			handleFunctionError(std::string errFunction);
		void		cleanServer();
		int			createServerSocket();
		int			setupListen();
		void		setupPollServer();
		int			runningServ();
		int			connectAndRead();
		int			acceptNewConnection(int &serverFd);
		int			readDataFromSocket(std::vector<struct pollfd>::iterator & it);
		void		addClient(int newClientFd, int &);
		void		deleteClient(int &clientFd, std::vector<struct pollfd>::iterator & it);

		bool		socketAlreadyExists(const uint16_t &port, const std::string &IP) const;
		
	public :
		Webserv(Config const &config);
		~Webserv();
} ;

#endif