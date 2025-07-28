#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

class Client {
	private:
		int	_fd;
	public:
		Client(int);
		~Client();

		HttpRequest		*httpReq;
		HttpResponse	*httpRes;
};

#endif