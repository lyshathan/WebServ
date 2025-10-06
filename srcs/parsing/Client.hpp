#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../webserv/Webserv.hpp"
#include "request/HttpRequest.hpp"
#include "response/HttpResponse.hpp"

class Client {
	private:
		int					_fd;
		std::string			_reqBuffer;
		size_t				_recvSize;
		std::string			_clientIP;

		Client();
	public:
		Client(int, const Config &, const std::string &clientIP);
		~Client();

		bool				appendBuffer(const char *, size_t);
		bool				isReqComplete() const;
		void				clearBuffer();
		const std::string	&getRes()const;

		HttpRequest		*httpReq;
		HttpResponse	*httpRes;
};

#endif