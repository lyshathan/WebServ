#ifndef HTTP_RESPONSE_HPP
#define HTTP_RESPONSE_HPP

#include <string>
#include <sstream>
#include "HttpRequest.hpp"

class HttpResponse {
	private:
		HttpRequest *_request;
		HttpResponse();
		std::string	_res;
	public:
		HttpResponse(HttpRequest *);
		~HttpResponse();

		void	parseResponse();
		void	badRequest();
		void	notFound();

		const std::string &getRes() const;
};

#endif