#ifndef HTTP_RESPONSE_HPP
#define HTTP_RESPONSE_HPP

#include <string>
#include <sstream>
#include <ctime>
#include <time.h>
#include <fstream>
#include "../request/HttpRequest.hpp"
#include <sys/stat.h>

struct UserData {
	std::string username;
	std::string age;
	std::string city;
};

class Client;
class CgiHandler;

class HttpResponse {
	private:
		HttpResponse();

		HttpRequest 							*_request;
		Client									*_client;
		std::map<int, std::string>				_statusPhrases;
		std::map<int, std::string>				_htmlResponses;
		std::map<std::string, std::string>		_headers;
		std::string								_resHeaders;
		std::string								_responseStatus;
		std::string								_res;
		std::vector<char>						_binRes;
		std::string								_mimeType;
		static std::map<std::string, UserData>	_sessions;
		int										_status;
		std::map<std::string, std::string>		_mimeTypes;

	public:
		HttpResponse(HttpRequest *, Client *);
		~HttpResponse();

		void		parseResponse();
		void		initHtmlResponses();
		void		initStatusPhrases();
		void 		initMimeTypes();
		void 		buildBody();

		void		addHeader(const std::string &, const std::string &);
		void		setContentHeaders();
		void		setStatusLine();
		void		setConnectionHeader();
		void		setStatusSpecificHeaders();
		void		setAutoIndex(std::string);

		std::string	generateSession();
		std::string	buildSimpleHTML(const UserData& data);

		bool		handleCookie();
		bool		handleCookieGet(std::map<std::string, std::string>::iterator& cookieIt);
		bool		handleCookiePost(std::map<std::string, std::string>::iterator& cookieIt);
		void		cgiParseResponse();

		std::string getTime() const;
		std::string getMimeType(std::string) const;
		std::string getLastModifiedTime() const;

		const std::string 						&getRes() const;
		const std::vector<char> 				&getBinRes() const;
		int										getResStatus() const;
		const std::string 						&getResHeaders();
		static std::map<std::string, UserData>	&getSessions();

};

#endif