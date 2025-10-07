#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#include "Includes.hpp"
#include "../config/Config.hpp"
#include "../config/server/ServerConfig.hpp"
#include <signal.h>
#include <sys/wait.h>

extern volatile sig_atomic_t g_running;

class Client;
class ServerConfig;
struct CgiState;

class Webserv {
	private :
		const Config						&_config;
		const std::vector<ServerConfig>		&_serverConfigs;

		std::vector<int>			_serverFds;
		std::map<int, Client*>		_clients;
		std::vector<struct pollfd>	_pollFds;
		std::map< int, std::pair< uint16_t, std::string> >	_serverInfos;
		int							_listenBackLog;
		std::map<int, int>			_cgiToClient;

		// uint32_t	fromHostToAddress(std::string hostname);
		int			handleFunctionError(std::string errFunction);
		void		cleanServer();
		int			createServerSocket();
		int			setupListen();
		void		setupPollServer();
		int			runningServ();
		int			connectAndRead();
		int			handleCGIEvents(std::vector<struct pollfd>::iterator &it, short events);
		int			acceptNewConnection(int &serverFd);
		int			readDataFromSocket(std::vector<struct pollfd>::iterator & it);
		int			sendResponse(int clientFd);
		void		addClient(int newClientFd, const std::string &clientIP);
		void		deleteClient(int &clientFd, std::vector<struct pollfd>::iterator & it);
		int			processAndSendResponse(int clientFd);

		void		addCGIToPoll(int clientFd);
		void		handleCGIWrite(int, CgiState*);
		void		handleCGIRead(int, CgiState*);
		void		handleCGICompletion(int, CgiState*);
		void		cleanupCGI(int, CgiState*);
		void		removeFdFromPoll(int fd);
		void		closeCGIStdin(CgiState *cgiState);
		bool		hasCGIContentLength(const std::string& cgiOutput, size_t& contentLength);

		bool		socketAlreadyExists(const uint16_t &port, const std::string &IP) const;

	public :
		Webserv(Config const &config);
		~Webserv();
} ;

#endif