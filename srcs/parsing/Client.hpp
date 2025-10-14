#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../webserv/Webserv.hpp"
#include "request/HttpRequest.hpp"
#include "response/HttpResponse.hpp"
#include "../cgi/CgiHandler.hpp"

enum ReadState {
    READ_INCOMPLETE = 0,
    READ_COMPLETE = 1,
    READ_ERROR = -1
};

enum WriteState {
    WRITE_INCOMPLETE = 0,
    WRITE_COMPLETE = 1,
    WRITE_ERROR = -1
};

enum ClientState {
	READING_HEADERS,
	READING_BODY,
	REQUEST_READY,
	CGI_PROCESSING,
	SENDING_RESPONSE,
	DONE
};

class CgiHandler;

class Client {
	private:
		size_t						_pollIndex;
		int							_fd;
		std::string					_reqBuffer;
		std::string					_resBuffer;
		size_t						_recvSize;
		size_t						_bytesSent;
		std::string					_clientIP;
		ClientState 				_state;
		CgiHandler					*_cgi;
		time_t 						 _lastActivity;

		static const int CGI_TIMEOUT = 5000;

		Client();
	public:
		Client(int, const Config &, const std::string &clientIP, size_t);
		~Client();

		bool				isCGI();
		int					readAndParseRequest();
		int					writeResponse();
		bool				appendBuffer(const char *, size_t);
		bool				isReqComplete() const;
		bool				connectionShouldClose() const;
		void				resetClient();

		void				launchCGI();

		const				std::string	&getRes()const;
		int					getFd() const;
		size_t				getPollIndex();
		std::string			getClientIp() const;
		void				setState(ClientState);
		CgiHandler			*getCgi() const;
		void				updateActivity();
		bool 				hasTimedOut(time_t);


		HttpRequest		*httpReq;
		HttpResponse	*httpRes;
};

#endif