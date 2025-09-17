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

void HttpResponse::setAutoIndex() {
	std::string	uri = _request->getUri();
	std::stringstream ss;

	ss << "<html><head><title>Index of " << uri
	<< "</title></head><body> <h1>Index of " << uri
	<< "</h1><hr><pre>";

	std::string path = _request->getRoot() + _request->getUri();
	DIR *dir = opendir(path.c_str());
	if (!dir) {
		std::cout << "Error opening directory\n";
		return ;
	}

	struct dirent *entry;
	while ((entry = readdir(dir)) != NULL) {
		if (std::string(entry->d_name) == ".")
			continue;
		ss << "<a href=\"" << entry->d_name << "\">" << entry->d_name << "</a><br>\n";
	}
	ss << "</pre><hr></body></html>";
	_res = ss.str();
	closedir(dir);
}

void HttpResponse::setTextContent() {
	std::string uri = _request->getUri();
	std::fstream file(uri.c_str(), std::ios::in | std::ios::binary);
	if (!file.is_open()) {
		if (_request->getAutoIndex())
			setAutoIndex();
		else if (!_htmlResponses[_request->getStatus()].empty()){
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