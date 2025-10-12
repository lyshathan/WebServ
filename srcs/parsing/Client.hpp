#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../webserv/Webserv.hpp"
#include "request/HttpRequest.hpp"
#include "response/HttpResponse.hpp"

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

class Client {
	private:
		size_t				_pollIndex;
		int					_fd;
		std::string			_reqBuffer;
		std::string			_resBuffer;
		size_t				_recvSize;
		size_t				_bytesSent;
		std::string			_clientIP;
		ClientState 		_state;

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

		int					handleCGIWrite(CgiState*);
		int					handleCGIRead(CgiState*);
		void				handleCGICompletion(CgiState*);
		void				parseCGIHeaders(CgiState *, size_t);
		void				parseSimpleCGIHeaders(CgiState *cgiState, size_t headerEnd);
		void				tryParseCGIHeaders(CgiState *cgiState);

		const				std::string	&getRes()const;
		int					getFd() const;
		size_t				getPollIndex();
		void				setState(ClientState);

		HttpRequest		*httpReq;
		HttpResponse	*httpRes;
};

#endif