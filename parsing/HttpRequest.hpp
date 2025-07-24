#ifndef HTTP_REQUEST_HPP
#define HTPP_REQUEST_HPP

#include <string>
#include <algorithm>
#include <map>
#include <vector>

class HttpRequest {
	private:
		HttpRequest();

	public:
		~HttpRequest();
		
		std::string	method;
		std::string	uri;
		std::string version;
		std::map<std::string, std::vector<std::string> > headers;
		std::string body;

};

#endif