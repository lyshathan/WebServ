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
#include "../../config/server/ServerConfig.hpp"

#define DEFAULT 0
#define BAD_REQUEST 400
#define NOT_FOUND 404
#define MOVED_PERMANENTLY 301
#define MOVED_PERMANENTLY_302 302
#define OK 200
#define CREATED 201
#define FORBIDDEN 403
#define NOT_ALLOWED 405
#define INTERNAL_ERROR 500
#define NO_CONTENT 204

typedef enum	e_servState {
	NO_MATCH,
	MATCH_PORT,
	MATCH_PORT_DEFAULT_IP,
	MATCH_PORT_IP,
	EXACT_MATCH_DEFAULT_IP,
	EXACT_MATCH,
}				t_servState;

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
		std::map<std::string, std::string>	_body;
		std::map<std::string, std::string>	_extensions;
		int									_status;
		int									_clientfd;
		bool								_areHeadersParsed;


		bool		parseFirstLine(std::string);
		bool		parseHeaders(std::string);
		bool		parseBody(std::string);
		bool		parseMultiPartBody(std::map<std::string, std::string>::const_iterator &it, std::string);
		void		parseOnePart(std::string);

		bool		postHandler();
		bool		getHandler();
		bool		deleteHandler();
		bool		createFile();
		bool		deleteFile();

		bool		validateUri();
		bool		validateVersion(std::string);

		bool		setUri(std::string &);
		void		setErrorPage();
		void		setExtensions();
		bool		validateMethods();

		void		pickServerConfig();
		void		pickLocationConfig();

		bool		extractUntil(std::string &, std::string &, const std::string &);
		std::string	trim(const std::string &);
		bool		mapHeaders(std::string &);
		bool		isValidTchar(char c);

		bool		isLocationPathValid();
		bool		isUploadPathValid();
		bool		isDeletePathValid();
		bool		nameAttribute(std::string&, std::string &);
		void		generateName(std::string &);

		HttpRequest();
	public:
		HttpRequest(const Config& config, int &);
		~HttpRequest();

		void	requestHeaderParser(std::string);
		void	requestBodyParser(std::string);
		void	requestHandler();

		const std::string&					getMethod() const;
		const std::string&					getUri() const;
		const std::string&					getVersion() const;
		const std::string					getRoot() const;
		int									getStatus() const;
		bool								getHeadersParsed() const;
		std::map<std::string, std::string>&	getHeaders();
		bool								getAutoIndex() const;

		void 				setHeadersParsed();

		void				cleanReqInfo();
};

#endif
