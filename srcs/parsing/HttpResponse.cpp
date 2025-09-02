#include "HttpResponse.hpp"

/******************************************************************************/
/*						CONSTRUCTORS & DESTRUCTORS							  */
/******************************************************************************/

HttpResponse::HttpResponse() {};

HttpResponse::HttpResponse(HttpRequest *req) : _request(req), _res(""), _isTextContent(false), _mimeType("") {};

HttpResponse::~HttpResponse() {};

/******************************************************************************/
/*							PARSE FUNCTIONS								  */
/******************************************************************************/

void HttpResponse::parseResponse() {
	int	status = _request->getStatus();

	if (status == OK)
		successfulRequest();
	if (status == BAD_REQUEST)
		badRequest();
	if (status == NOT_FOUND)
		notFound();
}

const std::string& HttpResponse::getRes() const {return _res;}
const std::string& HttpResponse::getResHeaders() const {return _headers;}
const std::vector<char>& HttpResponse::getBinRes() const {return _binRes;}
bool	HttpResponse::getIsTextContent() const {return _isTextContent;}

void HttpResponse::successfulRequest() {
	_mimeType = getMimeType();
	if (isTextContent()) {
		setTextRes();
	} else {
		setBinRes();
	}
}

void HttpResponse::setTextRes() {
	std::ostringstream	headers;

	setTextContent();

	headers << _request->getVersion() + " 200 OK\r\n"
	<< "Server: webserv\r\n"
	<< "Date: " << getTime() << "\r\n"
	<< "Content-Type: " << _mimeType << "\r\n"
	<< "Content-Length: " << _res.size() << "\r\n"
	<< "Last-Modified: " << getLastModifiedTime() << "\r\n"
	<< "Connection: keep-alive" << "\r\n\r\n";

	_headers = headers.str();
}

void HttpResponse::setBinRes() {
	std::ostringstream	headers;

	setBinContent();

	headers << _request->getVersion() + " 200 OK\r\n"
	<< "Server: webserv\r\n"
	<< "Date: " << getTime() << "\r\n"
	<< "Content-Type: " << _mimeType << "\r\n"
	<< "Content-Length: " << _binRes.size() << "\r\n"
	<< "Last-Modified: " << getLastModifiedTime() << "\r\n"
	<< "Connection: keep-alive" << "\r\n\r\n";

	_headers = headers.str();
}

void HttpResponse::notFound() {
	std::cout << "Not Found\n";
	std::ostringstream headers;
	std::ostringstream html;

	html << "<html><head><title>404 Not Found</title></head><body>"
		<< "<center><h1>404 Not Found</h1></center>"
		<< "<hr>"
		<< "<p><strong>Requested URI:</strong> " << _request->getUri() << "</p>"
		<< "<p><strong>Method:</strong> " << _request->getMethod() << "</p>"
		<< "<p><strong>Time:</strong> " << getTime() << "</p>"
		<< "<center>webserv</center>"
		<< "</body></html>";

	std::string htmlCode = html.str();
	_res = htmlCode;

	headers << _request->getVersion() + " 404 Not Found\r\n"
	<< "Server: webserv\r\n"
	<< "Date: " << getTime() << "\r\n"
	<< "Content-Type: " << getMimeType() << "\r\n"
	<< "Content-Length: "<< _res.size() << "\r\n"
	<< "Connection: closed" << "\r\n\r\n";

	_headers = headers.str();
}

void HttpResponse::badRequest() {
	std::ostringstream headers;
	std::ostringstream html;

	html << "<html><head><title>400 Bad Request</title></head><body>"
	<< "<center><h1>400 Bad Request</h1></center>"
	<< "<hr>"
	<< "<p><strong>Requested URI:</strong> " << _request->getUri() << "</p>"
	<< "<p><strong>Method:</strong> " << _request->getMethod() << "</p>"
	<< "<p><strong>Time:</strong> " << getTime() << "</p>"
	<< "<center>webserv</center>"
	<< "</body></html>";

	std::string htmlCode = html.str();
	_binRes.clear();
	_binRes.insert(_binRes.end(), htmlCode.begin(), htmlCode.end());

	headers << _request->getVersion() + " 400 Bad Request\r\n"
	<< "Server: webserv\r\n"
	<< "Date: " << getTime() << "\r\n"
	<< "Content-Type: " << getMimeType() << "\r\n"
	<< "Content-Length: "<< _binRes.size() << "\r\n"
	<< "Connection: closed" << "\r\n\r\n";

	_headers = headers.str();
}



