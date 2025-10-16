#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#include "Includes.hpp"
#include "../parsing/request/HttpRequest.hpp"
#include "../config/Config.hpp"
#include "../config/server/ServerConfig.hpp"
#include "../parsing/request/HttpRequest.hpp"
#include "../ProjectTools.hpp"
#include <signal.h>
#include <sys/wait.h>

extern volatile sig_atomic_t g_running;

class Client;
class ServerConfig;
class CgiHandler;

class Webserv {
	private :
		const Config						&_config;
		const std::vector<ServerConfig>		&_serverConfigs;

		std::vector<int>									_serverFds;
		std::map<int, Client*>								_clients;
		std::vector<struct pollfd>							_pollFds;
		std::map< int, std::pair< uint16_t, std::string> >	_serverInfos;
		int													_listenBackLog;
		std::map<int, Client*>								_cgiToClient;

		int			handleFunctionError(std::string errFunction);
		bool		socketAlreadyExists(const uint16_t &port, const std::string &IP) const;
		void		cleanServer();
		int			createServerSocket();
		int			setupListen();
		void		setupPollServer();
		int			runningServ();
		int			connectAndRead(std::vector<struct pollfd> &, std::vector<int> &);

		int			acceptNewConnection(int &, std::vector<struct pollfd> &);
		void		addClient(int newClientFd, const std::string &clientIP, std::vector<struct pollfd> &);
		void		handleClientCGI(Client *, std::vector<struct pollfd> &, struct pollfd &pfd);

		void		disconnectClient(int &);

		void		handleEvents(Client *, struct pollfd &pfd, std::vector<struct pollfd> &,std::vector<int> &);

		void		addCGIToPoll(Client *, CgiHandler *, std::vector<struct pollfd> &);
		void		removePollFd(int fd);

		void		signalClientReady(Client *);
		void		checkClientTimeouts(std::vector<int> &);

	public :
		Webserv(Config const &config);
		~Webserv();
} ;

#endif