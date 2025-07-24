#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "HttpRequest.hpp"
#include <string>
#include <vector>
#include <algorithm>
#include <map>

class Client {
	private:
		std::string	_rawMsg;
	public:
		Client(std::string);
		~Client();

		std::string const getRawMsg()const;
		HttpRequest		getHttpRequest();
};

#endif