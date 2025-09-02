#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../webserv/Webserv.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

class Client {
	private:
		int			_fd;
		std::string	_reqBuffer;
		size_t		_recvSize;

		Client();
	public:
		Client(int, const Config &);
		~Client();

		bool				appendBuffer(const char *, size_t);
		bool				isReqComplete() const;
		void				clearBuffer();
		const std::string	&getRes()const;

		HttpRequest		*httpReq;
		HttpResponse	*httpRes;
};

#endif