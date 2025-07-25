#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "HttpRequest.hpp"

class Client {
	public:
		Client(std::string);
		~Client();

		HttpRequest		*httpReq;
};

#endif