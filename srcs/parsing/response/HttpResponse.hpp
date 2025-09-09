#ifndef HTTP_RESPONSE_HPP
#define HTTP_RESPONSE_HPP

#include <string>
#include <sstream>
#include <ctime>
#include <time.h>
#include <fstream>
#include "../request/HttpRequest.hpp"
#include <sys/stat.h>

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

	public:
		HttpResponse(HttpRequest *);
		~HttpResponse();

		void	parseResponse();
		void	initHtmlResponses();
		void	initStatusPhrases();

		void	successfulRequest();
		void	badRequest();
		void	notFound();

		void	setTextRes();
		void	setBinRes();
		void	setTextContent();
		void	setBinContent();
		void	setBody(int);
		void	addHeader(const std::string &, const std::string &);
		void	setDefaultHeaders(int);

		std::string getTime() const;
		std::string getMimeType() const;
		std::string getLastModifiedTime() const;
		bool		getIsTextContent() const;

		std::string responseHeader();

		const std::string &getRes() const;
		const std::vector<char> &getBinRes() const;
		const std::string &getResHeaders();

		bool isTextContent();
};

#endif