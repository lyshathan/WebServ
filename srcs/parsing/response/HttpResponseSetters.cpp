#include "HttpResponse.hpp"

/******************************************************************************/
/*								SETTERS										  */
/******************************************************************************/

void HttpResponse::addHeader(const std::string &key, const std::string &value) {
	_headers[key] = value + "\r\n";
}

void HttpResponse::setStatusLine() {
	std::stringstream ss;
	std::string version = _request->getVersion();

	if (version.empty())
		version = "HTTP/1.0";

	ss << _status;
	_responseStatus = version + " " +
		ss.str() + " " + _statusPhrases[_status] + "\r\n";
}

void HttpResponse::setConnectionHeader() {
	if (_status >= 400) {
		addHeader("Connection: ", "close");
	} else {
		addHeader("Connection: ", "keep-alive");
	}
}

void HttpResponse::setStatusSpecificHeaders() {
	if (_status == MOVED_PERMANENTLY || _status == FOUND)
		addHeader("Location: ", _request->getUri());
}

void HttpResponse::setContentHeaders() {
	std::stringstream ss;

	addHeader("Server: ", "webserv");
	addHeader("Date: ", getTime());
	ss << _binRes.size();
	addHeader("Content-Length: ", ss.str());
	if (!_mimeType.empty())
		addHeader("Content-Type: ", _mimeType);
}

void HttpResponse::setAutoIndex(std::string uri) {
	std::stringstream ss;

	ss << "<html><head><title>Index of " << uri
	<< "</title></head><body> <h1>Index of " << uri
	<< "</h1><hr><pre>";

	std::string path = uri;
	DIR *dir = opendir(path.c_str());
	if (!dir)
		throw INTERNAL_ERROR;

	struct dirent *entry;
	while ((entry = readdir(dir)) != NULL) {
		if (std::string(entry->d_name) == ".")
			continue;
		ss << "<a href=\"" << entry->d_name << "\">" << entry->d_name << "</a><br>\n";
	}
	ss << "</pre><hr></body></html>";
	std::string response = ss.str();
	_mimeType = "text/html";
	_binRes.assign(response.begin(), response.end());
	closedir(dir);
}