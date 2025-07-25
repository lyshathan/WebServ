#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP

#include <string>
#include <sstream>
#include <iostream>
#include <map>

#define BAD_REQUEST 400
#define NOT_FOUD 404
#define OK 200

class HttpRequest {
	private:
		std::string	_method;
		std::string	_uri;
		std::string	_version;
		std::map<std::string, std::string> _headers;
		std::string	_body;
		int			_status;

		void	parseFirstLine(std::string);
		void	handleParsing(std::string);
	public:
		HttpRequest(std::string);
		~HttpRequest();
		const std::string& getMethod() const { return _method; }
		const std::string& getUri() const { return _uri; }
		const std::string& getVersion() const { return _version; }
		const std::map<std::string, std::string>& getHeaders() const { return _headers; }
};

#endif

// "GET /index.html HTTP/1.1\r\nHost:localhost:8080\r\nUser-Agent:Mozilla/5.0\r\n\r\n"

// Request-Line = GET /index.html HTTP/1.1\r\n

// Headers = Host:localhost:8080\r\nUser-Agent:Mozilla/5.0

// {
// 	method:"GET"
// 	path:"/index.html"
// 	version:"HTTP/1.1"
// 	headers: {
// 		"Host":"localhost:8080",
// 		"User-Agent":"Mozilla/5.0"
// 	}
// 	body:NULL
// 	valid:true
// }