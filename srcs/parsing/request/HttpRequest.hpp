#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP

#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../../webserv/Webserv.hpp"
#include "../../config/parser/ServerConfig.hpp"

#define BAD_REQUEST 400
#define NOT_FOUND 404
#define MOVED_PERMANENTLY 301
#define OK 200
#define FORBIDDEN 403

class ServerConfig;

class HttpRequest {
	private:
		const Config						&_config;
		const ServerConfig					*_server;
		const LocationConfig				*_location;
		std::string							_method;
		std::string							_uri;
		std::string							_version;
		std::map<std::string, std::string>	_headers;
		std::string							_body;
		int									_status;

		std::pair< uint16_t, std::string> &_serverInfo;


		bool		parseFirstLine(std::string);
		bool		parseHeaders(std::string);
		bool		parseBody(std::string);

		void		requestHandler();
		bool		requestParser(std::string);

		bool		validateUri();
		bool		validateVersion(std::string);

		bool		pickServerConfig();
		bool		pickLocationConfig();
		bool		setUri(std::string &);
		void		setErrorPage();

		bool		extractUntil(std::string &, std::string &, const std::string &);
		std::string	trim(const std::string &);
		bool		mapHeaders(std::string &);
		bool		isValidTchar(char c);

		bool		isLocationPathValid();

		HttpRequest();
	public:
		HttpRequest(const Config& config, std::pair< uint16_t, std::string> &);
		~HttpRequest();

		void	handleRequest(std::string);

		const std::string&	getMethod() const;
		const std::string&	getUri() const;
		const std::string&	getVersion() const;
		void				cleanReqInfo();
		int					getStatus() const;
		std::map<std::string, std::string>& getHeaders();
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