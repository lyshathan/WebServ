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

class Webserv {
	private :
		const Config						&_config;
		const std::vector<ServerConfig>		&_serverConfigs;

		std::vector<int>									_serverFds;
		std::map<int, Client*>								_clients;
		std::vector<struct pollfd>							_pollFds;
		std::map< int, std::pair< uint16_t, std::string> >	_serverInfos;
		int													_listenBackLog;
		std::map<int, int>									_cgiToClient;

		int			handleFunctionError(std::string errFunction);
		bool		socketAlreadyExists(const uint16_t &port, const std::string &IP) const;
		void		cleanServer();
		int			createServerSocket();
		int			setupListen();
		void		setupPollServer();
		int			runningServ();
		int			connectAndRead();

		int			acceptNewConnection(int &serverFd);
		void		addClient(int newClientFd, const std::string &clientIP);
		Client		*pickClient(struct pollfd &);
		void		handleClientRead(Client *client, struct pollfd &);
		void		handleClientWrite(Client *client, struct pollfd &);
		void		disconnectClient(int &);

		void		handleEvents(Client *, struct pollfd &pfd);

		void		handleCGIReadEvent(Client *, CgiState *);
		void		handleCGIWriteEvent(Client *, CgiState *);
		void 		cleanupCGI(Client *, CgiState *);
		void 		closeCGIStdin(CgiState *cgiState);

		void		addCGIToPoll(Client *, struct pollfd &);
		void		handleCGIEvents(Client *, struct pollfd &);
		void		removeFdFromPoll(int fd);

	public :
		Webserv(Config const &config);
		~Webserv();
} ;

#endif