#ifndef Server_HPP
# define Server_HPP

#include "Includes.hpp"

class Client;

class Server {
	private :
		std::vector<struct pollfd>	_pollFds;
		std::map<int, Client*>		_clients;
		uint16_t					_port;
		uint32_t					_address;
		int							_status;
		int							_serverFd;
		int							_listenBackLog;

		Server(const Server & OtherServer);
		// Server & operator=(const Server & OtherServer);

		int		HandleFunctionError(std::string errFunction);
		int		CreateServerSocket();
		int		SetupListen();
		int		RunningServ();
		int		ConnectAndRead();
		int		AcceptNewConnection();
		int		ReadDataFromSocket(std::vector<struct pollfd>::iterator & it);
		void	AddClient(int newClientFd);
		void	DeleteClient(int clientFd, std::vector<struct pollfd>::iterator & it);

		
	public :
		Server();
		~Server();
} ;

#endif