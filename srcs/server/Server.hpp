#ifndef Server_HPP
# define Server_HPP

#include "Includes.hpp"

class Client;

class Server {
	private :

		struct ServerConfig {
				std::string				_conf_host;
				uint32_t				_conf_address;
				uint16_t				_conf_port;
			};

		std::vector<ServerConfig>	_serverConfigs;
		std::vector<struct pollfd>	_pollFds;
		std::map<int, Client*>		_clients;
		std::vector<int>			_serverFds;
		int							_listenBackLog;

		uint32_t	FromHostToAddress(std::string hostname);
		int			HandleFunctionError(std::string errFunction);
		void		CleanServer();
		int			CreateServerSocket();
		int			SetupListen();
		void		SetupPollServer();
		int			RunningServ();
		int			ConnectAndRead();
		int			AcceptNewConnection(int &serverFd);
		int			ReadDataFromSocket(std::vector<struct pollfd>::iterator & it);
		void		AddClient(int newClientFd);
		void 		DeleteClient(int &clientFd, std::vector<struct pollfd>::iterator & it);
		
	public :
		Server();
		~Server();
} ;

#endif