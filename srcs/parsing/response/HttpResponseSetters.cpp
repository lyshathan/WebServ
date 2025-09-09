#include "HttpResponse.hpp"

/******************************************************************************/
/*								SETTERS										  */
/******************************************************************************/

void HttpResponse::setDefaultHeaders(int status) {
	(void)status;
	// _responseStatus = _request->getVersion() + " " +
	// 	std::to_string(status) + " " + _statusPhrases[status] + "\r\n";

	addHeader("Server: ", "webserv");
	addHeader("Date: ", getTime());
	addHeader("Content-Type: ", _mimeType);
	// if (_isTextContent)
	// 	addHeader("Content-Length: ", std::to_string(_res.size()));
	// else
	// 	addHeader("Content-Length: ", std::to_string(_binRes.size()));
}

void HttpResponse::addHeader(const std::string &key, const std::string &value) {
	_headers[key] = value + "\r\n";
}

void HttpResponse::setBody(int status) {
	_mimeType = getMimeType();
	if (status == OK || (status == NOT_FOUND && !(_request->getUri().empty()))){
		addHeader("Last-Modified: ", getLastModifiedTime());
		if (isTextContent())
			setTextContent();
		else
			setBinContent();
	} else {
		_isTextContent = true;
		_res = _htmlResponses[status];
	}
}

void HttpResponse::initStatusPhrases(){
	_statusPhrases[200] = "OK";
	_statusPhrases[301] = "Moved Permanently";
	_statusPhrases[403] = "Forbidden";
	_statusPhrases[404] = "Not Found";
	_statusPhrases[500] = "Internal Server Error";
	_statusPhrases[400] = "Bad Request";
}

void HttpResponse::initHtmlResponses(){
	_htmlResponses[200] = "<!DOCTYPE html><html><head><title>OK</title></head>"
		"<body><h1>200 OK</h1><p>Your request has succeeded.</p></body></html>";
	_htmlResponses[301] = "<!DOCTYPE html><html><head><title>Moved Permanently</title></head>"
		"<body><h1>301 Moved Permanently</h1><p>The requested resource has been moved.</p></body></html>";
	_htmlResponses[403] = "<!DOCTYPE html><html><head><title>Forbidden</title></head>"
		"<body><h1>403 Forbidden</h1><p>You don't have permission to access this resource.</p></body></html>";
	_htmlResponses[404] = "<!DOCTYPE html><html><head><title>Not Found</title></head>"
		"<body><h1>404 Not Found</h1><p>The requested resource could not be found on this server.</p></body></html>";
	_htmlResponses[500] = "<!DOCTYPE html><html><head><title>Internal Server Error</title></head>"
		"<body><h1>500 Internal Server Error</h1><p>The server encountered an unexpected condition.</p></body></html>";
	_htmlResponses[400] = "<!DOCTYPE html><html><head><title>Bad Request</title></head>"
		"<body><h1>400 Bad Request</h1><p>The server encountered an unexpected condition.</p></body></html>";
}

void HttpResponse::setTextContent() {
	std::string uri = _request->getUri();
	std::fstream file(uri.c_str(), std::ios::in | std::ios::binary);
	if (!file.is_open())
		return ;

	_res.clear();
	char buffer[4096];
	std::string chunk;
	while (file.read(buffer, sizeof(buffer))) {
		chunk.assign(buffer, file.gcount());
		_res += chunk;
	}
	if (file.gcount() > 0) {
		chunk.assign(buffer, file.gcount());
		_res += chunk;
	}
	file.close();
}

void	HttpResponse::setBinContent() {
	std::string uri = _request->getUri();
	std::fstream file(uri.c_str(), std::ios::in | std::ios::binary);
	if (!file.is_open())
		return ;

	char buffer[4096];
	while (file.read(buffer, sizeof(buffer))) {
		_binRes.insert(_binRes.end(), buffer, buffer + file.gcount());
	}
	if (file.gcount() > 0) {
		_binRes.insert(_binRes.end(), buffer, buffer + file.gcount());
	}
}
