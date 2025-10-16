#ifndef CGI_HANDLER_HPP
#define CGI_HANDLER_HPP

#include <string>
#include <vector>
#include <map>
#include "../parsing/Client.hpp"

class Client;

enum CgiState {
	CGI_WRITING,
	CGI_READING,
	CGI_DONE,
	CGI_ERROR
};

enum IOStatus {
	IO_COMPLETE,
	IO_INCOMPLETE,
	IO_ERROR
};

class CgiHandler {
	private:
		Client								*_client;
		pid_t								_pid;
		int									_stdinFd;
		int									_stdoutFd;
		size_t								_bytesWritten;
		std::map<std::string, std::string>	_cgiHeaders;
		std::string							_finalResponse;
		std::string							_outputBuffer;
		std::string							_inputBuffer;
		bool								_headersParsed;
		CgiState							_cgiStage;
		size_t								_headerPos;

		IOStatus							handleWrite();
		IOStatus							handleRead();
		void								handleCompletion();

		void								handleChild(int stdin_fd[2], int stdout_fd[2]);
		void								handleParent(int stdin_fd[2], int stdout_fd[2]);

		void								markError(const std::string &);
		char 								**getEnvArray();
		char 								**getArgvArray();

		void								tryParseCGIHeaders();
		void								parseCGIHeaders();
		void								parseBodySendResponse(int);

	public:
		CgiHandler(Client *);
		~CgiHandler();

		void								handleEvent(struct pollfd &, std::vector<int> &);
		void								cgiStart();

		bool								isFinished() const;
		bool								hasError() const;
		void								cleanUp(std::vector<int>&);
		void								markDone();

		const std::string					&getResponse() const;
		int									getStdinFd() const;
		int									getStdoutFd() const;
		bool								headersParsed() const;
		std::map<std::string, std::string>	getCgiHeaders() const;
		CgiState							getCgiStage() const;
		pid_t								getPid() const;
};

#endif