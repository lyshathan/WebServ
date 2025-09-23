#include "HttpResponse.hpp"

/******************************************************************************/
/*						CONSTRUCTORS & DESTRUCTORS							  */
/******************************************************************************/

HttpResponse::HttpResponse() {};

HttpResponse::HttpResponse(HttpRequest *req) : _request(req),
	_isTextContent(false), _res(""), _mimeType("") {
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
	}
	else if (status == 204) {
		deleteParseResponse();
	} else if (status == 400) {
		_isTextContent = true;
		_res = _htmlResponses[_request->getStatus()];
		setContentHeaders();
		setConnectionHeader(status);
	} else if (status == 200 && !_request->getCGIRes().empty()) {
		_isTextContent = true;
		_res = _request->getCGIRes();
		_mimeType = "text/html";
		setContentHeaders();
		setConnectionHeader(status);
	}
	else {
		setBody(status);
		setContentHeaders();
		setStatusSpecificHeaders(status);
		setConnectionHeader(status);
	}
}

void HttpResponse::deleteParseResponse() {

	_isTextContent = true;
	_res = "";
	addHeader("Server: ", "webserv");
	addHeader("Date: ", getTime());
	addHeader("Content-Length: ", "0");
	addHeader("Connection: ", "close");
}

void HttpResponse::postParseResponse(int status) {
	std::stringstream ss;

	_isTextContent = true;
	_res = _htmlResponses[status];
	addHeader("Server: ", "webserv");
	addHeader("Date: ", getTime());
	addHeader("Content-Type: ", "text/html");
	ss << _res.size();
	addHeader("Content-Length: ", ss.str());
}