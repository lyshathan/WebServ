#ifndef Server_HPP
# define Server_HPP

#include "Includes.hpp"
#include "../parsing/Client.hpp"

class Server {
	private :
		std::map<int, Client>		_clients;
		uint16_t					_port;
		uint32_t					_address;
		int							_status;
		int							_serverFd;
		int							_listenBackLog;
		std::vector<struct pollfd>	_pollFds;

		Server(const Server & OtherServer);
		Server & operator=(const Server & OtherServer);

		void	CreateServerSocket();
		void	SetupListen();
		void	RunningServ();
		int		AcceptNewConnection();
		int		ReadDataFromSocket(std::vector<struct pollfd>::iterator & it);


	public :
		Server();
		~Server();
} ;

#endif