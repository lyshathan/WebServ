#include "HttpResponse.hpp"
#include "../Client.hpp"
#include "../../ProjectTools.hpp"

std::map<std::string, UserData> HttpResponse::_sessions;

/******************************************************************************/
/*						CONSTRUCTORS & DESTRUCTORS							  */
/******************************************************************************/

HttpResponse::HttpResponse() {};

HttpResponse::HttpResponse(HttpRequest *req, Client *client) : _request(req), _client(client), 
	_res(""), _mimeType("") {
	initStatusPhrases();
	initHtmlResponses();
	initMimeTypes();
};

HttpResponse::~HttpResponse() {};

/******************************************************************************/
/*							PARSE FUNCTIONS								  */
/******************************************************************************/

void HttpResponse::parseResponse() {
	_status = _request->getStatus();

	try {
		buildBody();
	} catch (int code) {
		_status = code;
		if (!_htmlResponses[_status].empty())
			_binRes.assign(_htmlResponses[_status].begin(),
               _htmlResponses[_status].end());
	}
	setStatusLine();
	setContentHeaders();
	setStatusSpecificHeaders();
	setConnectionHeader();

	// std::cout << std::string(_binRes.begin(), _binRes.end()) << std::endl;
}

void HttpResponse::buildBody() {
	std::string uri = _request->getUri();
	_mimeType = getMimeType(uri);

	if (handleCookie())
		return;

	if (_status == 200 && _request->isCGIActive()) {
		cgiParseResponse();
		return;
	}

	struct stat buf;
	if (!stat(uri.c_str(),&buf)) {
		if (S_ISDIR(buf.st_mode) && _status < 400) {
			setAutoIndex(uri);
			return ;
		}
		else if (S_ISREG(buf.st_mode)) {
			if (access(uri.c_str(),  R_OK) != 0)
				throw FORBIDDEN;
			std::fstream file(uri.c_str(), std::ios::in | std::ios::binary);
			if (!file.is_open())
				throw INTERNAL_ERROR;
			char timeStr[100];
			std::tm *gmt = std::gmtime(&buf.st_mtime);
			std::strftime(timeStr, sizeof(timeStr), "%a, %d %b %Y %H:%M:%S GMT", gmt);
			addHeader("Last-Modified: ", timeStr);
			_binRes.assign(std::istreambuf_iterator<char>(file),
				std::istreambuf_iterator<char>());
			return;
		}
	}
	if (!_htmlResponses[_status].empty()) {
		_binRes.assign(_htmlResponses[_status].begin(),
               _htmlResponses[_status].end());
	}
	else
		throw INTERNAL_ERROR;
}

void HttpResponse::cgiParseResponse() {
	std::string cgiRes = _request->getCGIRes();
	_binRes.assign(cgiRes.begin(), cgiRes.end());

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
}