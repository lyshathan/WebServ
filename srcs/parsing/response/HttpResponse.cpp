#include "HttpResponse.hpp"

/******************************************************************************/
/*						CONSTRUCTORS & DESTRUCTORS							  */
/******************************************************************************/

HttpResponse::HttpResponse() {};

HttpResponse::HttpResponse(HttpRequest *req) : _request(req), _isTextContent(false), _res(""), _mimeType("") {
	initStatusPhrases();
	initHtmlResponses();
};

HttpResponse::~HttpResponse() {};

/******************************************************************************/
/*							PARSE FUNCTIONS								  */
/******************************************************************************/

void HttpResponse::parseResponse() {
	int	status = _request->getStatus();

	setStatusLine(status);
	if (status == 201) {
		postParseResponse(status);
	} else {
		setBody(status);
		setContentHeaders();
		setStatusSpecificHeaders(status);
	}
}

void HttpResponse::postParseResponse (int status) {
	std::stringstream ss;

	_isTextContent = true;
	_res = _htmlResponses[status];
	addHeader("Server: ", "webserv");
	addHeader("Date: ", getTime());
	addHeader("Content-Type: ", "text/html");
	ss << _res.size();
	addHeader("Content-Length: ", ss.str());
}