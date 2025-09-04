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

		bool				_isTextContent;
		std::string			_headers;
		std::string			_res;
		std::vector<char>	_binRes;
		std::string			_mimeType;
	public:
		HttpResponse(HttpRequest *);
		~HttpResponse();

		void	parseResponse();
		void	successfulRequest();
		void	badRequest();
		void	notFound();
		void	setTextRes();
		void	setBinRes();
		void	setTextContent();
		void	setBinContent();

		std::string getTime() const;
		std::string getMimeType() const;
		std::string getLastModifiedTime() const;
		bool		getIsTextContent() const;

		const std::string &getRes() const;
		const std::vector<char> &getBinRes() const;
		const std::string &getResHeaders() const;
		bool isTextContent();
};

#endif