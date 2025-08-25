#ifndef Server_HPP
# define Server_HPP

#include "Includes.hpp"
#include "../config/parser/Config.hpp"
#include "../config/parser/ServerConfig.hpp"

class Client;
class ServerConfig;

class Server {
	private :

		std::vector<struct pollfd>	_pollFds;
		std::map<int, Client*>		_clients;
		std::vector<int>			_serverFds;
		int							_listenBackLog;
		std::vector<uint16_t>		_serverPorts;
		std::map<uint16_t, const ServerConfig*> _portToConfig;

		void		convertPorts(Config const &config);
		uint32_t	FromHostToAddress(std::string hostname);
		int			HandleFunctionError(std::string errFunction);
		void		CleanServer();
		int			CreateServerSocket();
		int			SetupListen();
		void		SetupPollServer(Config const &config);
		int			RunningServ();
		int			ConnectAndRead();
		int			AcceptNewConnection(int &serverFd);
		int			ReadDataFromSocket(std::vector<struct pollfd>::iterator & it);
		void		AddClient(int newClientFd);
		void		DeleteClient(int &clientFd, std::vector<struct pollfd>::iterator & it);
		const	ServerConfig* getConfigForPort(uint16_t port);
		uint16_t	getPortFromFd(int fd) const;

	public :
		Server(Config const &config);
		~Server();
} ;

#endif