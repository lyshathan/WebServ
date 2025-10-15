#include "HttpResponse.hpp"
#include "../Client.hpp"
#include "../../ProjectTools.hpp"

std::map<std::string, UserData> HttpResponse::_sessions;

/******************************************************************************/
/*						CONSTRUCTORS & DESTRUCTORS							  */
/******************************************************************************/

HttpResponse::HttpResponse() {};

HttpResponse::HttpResponse(HttpRequest *req, Client *client) : _request(req), _client(client),
	_isTextContent(false), _res(""), _mimeType("") {
	initStatusPhrases();
	initHtmlResponses();
};

HttpResponse::~HttpResponse() {};

/******************************************************************************/
/*							PARSE FUNCTIONS								  */
/******************************************************************************/

void HttpResponse::parseResponse() {
	std::cerr << "[RESPONSE] Parsing Response\n";
	int	status = _request->getStatus();

	setStatusLine(status);
	if (status == 201) {
		if (!handleCookie(status))
			postParseResponse(status);
	}
	else if (status == 204)
		deleteParseResponse();
	else if (status == 400 || status == 500)
		errorParseResponse(status);
	else if (status == 200 && _request->isCGIActive()) {
		std::cerr << "RESPONSE IS CGI\n";
		cgiParseResponse(status);
	}
	else
		successParseResponse(status);
	std::stringstream ss;
	ss << status;
	printLog(PURPLE, "INFO", "Response Sent Status Code: " + ss.str());
}

void HttpResponse::successParseResponse(int status) {
	if (!handleCookie(status))
		setBody(status);
	setContentHeaders();
	setStatusSpecificHeaders(status);
	setConnectionHeader(status);
}

void HttpResponse::cgiParseResponse(int status) {
	_isTextContent = true;
	_res = _request->getCGIRes();

	CgiHandler *cgi = _client->getCgi();
	if (cgi) {
		if (cgi->headersParsed()) {
			std::map<std::string, std::string>::iterator it;
			std::map<std::string, std::string> cgiHeaders = cgi->getCgiHeaders();
			if (!cgiHeaders.empty()) {
				it = cgiHeaders.find("content-type");
				if (it != cgiHeaders.end())
					_mimeType = it->second;
				else
					_mimeType = "text/html";
				for (it = cgiHeaders.begin(); it != cgiHeaders.end(); ++it) {
					if (it->first != "content-type" && it->first != "content-length") {
					addHeader(it->first + ": ", it->second);
					}
				}
			}
		} 
	} else
		_mimeType = "text/html";
	setContentHeaders();
	setConnectionHeader(status);
}

void HttpResponse::errorParseResponse(int status) {
	_isTextContent = true;
	_res = _htmlResponses[_request->getStatus()];
	setContentHeaders();
	setConnectionHeader(status);
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