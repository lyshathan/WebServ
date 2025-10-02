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

class HttpResponse {
	private:
		HttpRequest *_request;
		HttpResponse();

		bool								_isTextContent;
		std::map<int, std::string>			_statusPhrases;
		std::map<int, std::string>			_htmlResponses;
		std::map<std::string, std::string>	_headers;
		std::string							_resHeaders;
		std::string							_responseStatus;
		std::string							_res;
		std::vector<char>					_binRes;
		std::string							_mimeType;
		static std::map<std::string, UserData>	_sessions;

	public:
		HttpResponse(HttpRequest *);
		~HttpResponse();

		void	parseResponse();
		void	initHtmlResponses();
		void	initStatusPhrases();

		void	setTextContent(int);
		void	setBinContent();
		void	setBody(int);
		void	addHeader(const std::string &, const std::string &);
		void	setContentHeaders();
		void	setStatusLine(int);
		void	setConnectionHeader(int);
		void	setStatusSpecificHeaders(int);
		void	setAutoIndex();

		std::string	generateSession();
		std::string	buildSimpleHTML(const UserData& data);

		bool	handleCookie(int);
		bool	handleCookieGet(std::map<std::string, std::string>::iterator& cookieIt);
		bool	handleCookiePost(std::map<std::string, std::string>::iterator& cookieIt);
		static std::map<std::string, UserData>& getSessions();
		void	postParseResponse(int);
		void	deleteParseResponse();

		std::string getTime() const;
		std::string getMimeType() const;
		std::string getLastModifiedTime() const;
		bool		getIsTextContent() const;

		const std::string &getRes() const;
		const std::vector<char> &getBinRes() const;
		const std::string &getResHeaders();

		bool isTextContent();
};

#endif