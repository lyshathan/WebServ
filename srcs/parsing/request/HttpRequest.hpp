#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP

#include <string>
#include <sstream>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <algorithm>
#include <map>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <ctime>
#include <fstream>

#include "../../config/server/ServerConfig.hpp"
#include "../../config/Config.hpp"

#define DEFAULT 0
#define BAD_REQUEST 400
#define NOT_FOUND 404
#define MOVED_PERMANENTLY 301
#define FOUND 302
#define OK 200
#define CREATED 201
#define FORBIDDEN 403
#define NOT_ALLOWED 405
#define INTERNAL_ERROR 500
#define HTTP_NOT_SUPPORTED 505
#define NO_CONTENT 204
#define PAYLOAD_TOO_LARGE 413
#define REQUEST_TIMEOUT 408
#define GATEWAY_TIMEOUT 504
#define BAD_GATEWAY 502
#define REQUEST_URI_TOO_LONG 414
#define CGI_PENDING 1000
#define MAX_URI_LENGTH 8192

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
		std::string							_cgiRes;
		std::map<std::string, std::string>	_headers;
		std::map<std::string, std::string>	_body;
		std::string							_queries;
		std::string							_rawBody;
		std::map<std::string, std::string>	_extensions;
		std::vector<std::string>			_argv;
		int									_status;
		int									_clientfd;
		bool								_areHeadersParsed;
		bool								_isProccessingError;
		bool								_isCGI;
		std::string							_clientIP;
		bool								_isErrorAvailable;

		bool		parseFirstLine(std::string);
		bool		parseHeaders(std::string);
		bool		parseBody(std::string);
		bool		parseMultiPartBody(std::map<std::string, std::string>::const_iterator &it, std::string);
		void		parseOnePart(std::string);
		void		parseQueries();
		bool		parseChunk(std::string);

		bool		postHandler();
		void		getHandler();
		bool		deleteHandler();
		bool		createFile();
		bool		deleteFile();

		bool		isCGIPath();

		bool		validateUri();
		bool		validateVersion(std::string);

		bool		setUri(std::string &);
		void		setExtensions();
		bool		validateMethods();
		bool		checkReturn();

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
		HttpRequest(const Config& config, int &, const std::string& clientIP);
		~HttpRequest();

		int									requestHeaderParser(std::string);
		void								requestBodyParser(std::string);
		void								requestHandler();

		const std::string&					getMethod() const;
		const std::vector<std::string>		getServerName() const;
		const std::string&					getQueries() const;
		const std::vector<int>				getListenPort() const;
		const std::string&					getUri() const;
		const std::string&					getVersion() const;
		const std::string					getRoot() const;
		const std::string&					getCGIRes() const;
		int									getStatus() const;
		bool								getHeadersParsed() const;
		std::map<std::string, std::string>&	getHeaders();
		std::map<std::string, std::string>&	getBody();
		bool								getAutoIndex() const;
		bool								isCGIActive() const;
		size_t								getMaxBody() const;
		std::vector<std::string>			getArgv() const;
		bool								getErrorAvailable() const;
		void								setErrorPage();

		bool								checkCGI();

		void 								setHeadersParsed();
		void								setStatus(int);
		void								setCGIResult(const std::string &result);

		void								cleanReqInfo();
};

#endif
