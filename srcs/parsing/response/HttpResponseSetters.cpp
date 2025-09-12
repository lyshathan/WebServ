#include "HttpResponse.hpp"

/******************************************************************************/
/*								SETTERS										  */
/******************************************************************************/

void HttpResponse::addHeader(const std::string &key, const std::string &value) {
	_headers[key] = value + "\r\n";
}

void HttpResponse::setConnectionHeader(int status) {
	if (status >= 400) {
		addHeader("Connection: ", "close");
	} else {
		addHeader("Connection: ", "keep-alive");
	}
}

void HttpResponse::setStatusSpecificHeaders(int status) {
	switch(status) {
		case MOVED_PERMANENTLY:
		case MOVED_PERMANENTLY_302:
			addHeader("Location: ", _request->getUri());
			break;
	}
}

void HttpResponse::setStatusLine(int status) {
	std::stringstream ss;

	ss << status;
	_responseStatus = _request->getVersion() + " " +
		ss.str() + " " + _statusPhrases[status] + "\r\n";
}

void HttpResponse::setContentHeaders() {
	std::stringstream ss;

	addHeader("Server: ", "webserv");
	addHeader("Date: ", getTime());
	addHeader("Content-Type: ", _mimeType);
	if (_isTextContent) ss << _res.size();
	else ss << _binRes.size();
	addHeader("Content-Length: ", ss.str());
}

void HttpResponse::setBody(int status) {
	(void)status;
	_mimeType = getMimeType();
	addHeader("Last-Modified: ", getLastModifiedTime());
	if (isTextContent())
		setTextContent();
	else
		setBinContent();
}

void HttpResponse::setTextContent() {
	std::string uri = _request->getUri();
	std::fstream file(uri.c_str(), std::ios::in | std::ios::binary);
	if (!file.is_open()) {
		if (!_htmlResponses[_request->getStatus()].empty()){
			_res = _htmlResponses[_request->getStatus()];
		}
		return ;
	}
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