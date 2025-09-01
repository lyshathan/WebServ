#include "HttpResponse.hpp"

/******************************************************************************/
/*						CONSTRUCTORS & DESTRUCTORS							  */
/******************************************************************************/

HttpResponse::HttpResponse() {};

HttpResponse::HttpResponse(HttpRequest *req) : _request(req), _res(""), _isTextContent(false), _mimeType("") {};

HttpResponse::~HttpResponse() {};

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

std::string HttpResponse::getMimeType() const {
	std::string uri = _request->getUri();

	size_t pos = uri.find_last_of(".");
	if (pos == std::string::npos) {
		return "text/html";
	}
	else {
		std::string extension = uri.substr(pos + 1);
		if (extension == "html" || extension == "htm")
			return "text/html";
		else if (extension == "css")
			return "text/css";
		else if (extension == "js")
			return "text/javascript";
		else if (extension == "json")
			return "application/json";
		else if (extension == "png")
			return "image/png";
		else if (extension == "jpg" || extension == "jpeg")
			return "image/jpeg";
		else if (extension == "gif")
			return "image/gif";
		else if (extension == "svg")
			return "image/svg+xml";
		else if (extension == "ico")
			return "image/x-icon";
		else if (extension == "txt")
			return "text/plain";
		else if (extension == "xml")
			return "text/xml";
		else if (extension == "pdf")
			return "application/pdf";
	}
	return "application/octet-stream";
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
	_binRes.clear();
	_binRes.insert(_binRes.end(), htmlCode.begin(), htmlCode.end());

	headers << _request->getVersion() + " 404 Not Found\r\n"
	<< "Server: webserv\r\n"
	<< "Date: " << getTime() << "\r\n"
	<< "Content-Type: " << getMimeType() << "\r\n"
	<< "Content-Length: "<< _binRes.size() << "\r\n"
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



