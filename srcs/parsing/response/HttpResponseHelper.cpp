#include "HttpResponse.hpp"

/******************************************************************************/
/*						PARSE  HELPER FUNCTIONS								  */
/******************************************************************************/

std::string	HttpResponse::getTime() const {
	time_t rawtime;
	struct tm * timeinfo;
	char buffer [80];

	time (&rawtime);
	timeinfo = gmtime (&rawtime);
	strftime (buffer,80,"%a, %d %b %Y %H:%M:%S GMT", timeinfo);
	return std::string(buffer);
}

bool	HttpResponse::isTextContent() {
	if (_mimeType.find("text/") == 0 || _mimeType == "application/json" ||
		_mimeType == "application/javascript") {
			_isTextContent = true;
			return 1;
	} else {
		_isTextContent = false;
		return 0;
	}

}

std::string HttpResponse::getLastModifiedTime() const {
	struct stat			file_stat;
	int					status;
	struct tm 			* timeinfo;
	char				buffer [80];

	status = stat(_request->getUri().c_str(), &file_stat);
	if (status != 0)
		return getTime();
	timeinfo = gmtime(&file_stat.st_mtime);
	strftime (buffer,80,"%a, %d %b %Y %H:%M:%S GMT", timeinfo);
	return std::string(buffer);
}

void HttpResponse::initStatusPhrases(){
	_statusPhrases[200] = "OK";
	_statusPhrases[301] = "Moved Permanently";
	_statusPhrases[403] = "Forbidden";
	_statusPhrases[404] = "Not Found";
	_statusPhrases[500] = "Internal Server Error";
	_statusPhrases[400] = "Bad Request";
	_statusPhrases[201] = "Created";
	_statusPhrases[405] = "Method Not Allowed";
	_statusPhrases[500] = "Internal Server Error";
	_statusPhrases[204] = "No Content";
	_statusPhrases[413] = "Request Entity Too Large";
}

void HttpResponse::initHtmlResponses(){
	_htmlResponses[200] = "<!DOCTYPE html><html><head><title>OK</title></head>"
		"<body><h1>200 OK</h1><p>Your request has succeeded.</p></body></html>";
	_htmlResponses[201] = "<!DOCTYPE html><html><head><title>Created</title></head>"
		"<body><h1>201 Created</h1><p>File uploaded successfully.</p></body></html>";
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
	_htmlResponses[302] = "<!DOCTYPE html><html><head><title>302 Found</title></head>"
		"<body><h1>302 Found</h1><p>The server encountered an unexpected condition.</p></body></html>";
	_htmlResponses[405] = "<!DOCTYPE html><html><head><title>405 Not Allowed</title></head>"
		"<body><h1>405 Not Allowed</h1><p>The server encountered an unexpected condition.</p></body></html>";
	_htmlResponses[500] = "<!DOCTYPE html><html><head><title>500 Internal Server Error</title></head>"
		"<body><h1>500 Internal Server Error</h1><p>The server encountered an unexpected condition.</p></body></html>";
	_htmlResponses[413] = "<!DOCTYPE html><html><head><title>413 Request Entity Too Large</title></head>"
		"<body><h1>413 Request Entity Too Large</h1><p>The server encountered an unexpected condition.</p></body></html>";
}
