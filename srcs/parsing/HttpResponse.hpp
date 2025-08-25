#ifndef HTTP_RESPONSE_HPP
#define HTTP_RESPONSE_HPP

#include <string>
#include <sstream>
#include <ctime>
#include <time.h>
#include <fstream>
#include "HttpRequest.hpp"
#include <sys/stat.h>

class HttpResponse {
	private:
		HttpRequest *_request;
		HttpResponse();

		std::string	_res;
	public:
		HttpResponse(HttpRequest *);
		~HttpResponse();

		void	parseResponse();
		void	successfulRequest();
		void	badRequest();
		void	notFound();

		std::string getTime() const;
		std::string getMimeType() const;
		std::string getContent() const;
		std::string getLastModifiedTime() const;

		const std::string &getRes() const;
};

#endif