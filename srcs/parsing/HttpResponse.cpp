#include "HttpResponse.hpp"

/******************************************************************************/
/*						CONSTRUCTORS & DESTRUCTORS							  */
/******************************************************************************/

HttpResponse::HttpResponse() {};

HttpResponse::HttpResponse(HttpRequest *req) : _request(req), _res("") {};

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

std::string HttpResponse::getContent() const {
	std::fstream file (_request->getUri());
	if (!file.is_open())
		return "";
	std::ostringstream buffer;
	buffer << file.rdbuf();
	file.close();
	return buffer.str();
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

void HttpResponse::successfulRequest() {
	std::ostringstream	res;
	std::string 		content;

	content = getContent();

	res << _request->getVersion() + " 200 OK\r\n"
	<< "Server: webserv\r\n"
	<< "Date: " << getTime() << "\r\n"
	<< "Content-Type: " << getMimeType() << "\r\n"
	<< "Content-Length: " << content.size() << "\r\n"
	<< "Last-Modified: " << getLastModifiedTime() << "\r\n"
	<< "Connection: keep-alive" << "\r\n\r\n"
	<< content;

	_res = res.str();
}

void HttpResponse::notFound() {
	std::ostringstream res;
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

	res << _request->getVersion() + " 404 Not Found\r\n"
	<< "Server: webserv\r\n"
	<< "Date: " << getTime() << "\r\n"
	<< "Content-Type: " << getMimeType() << "\r\n"
	<< "Content-Length: "<< htmlCode.size() << "\r\n"
	<< "Connection: closed" << "\r\n\r\n"
	<< htmlCode;
	_res = res.str();
}

void HttpResponse::badRequest() {
	std::ostringstream res;
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

	res << _request->getVersion() + " 400 Bad Request\r\n"
	<< "Server: webserv\r\n"
	<< "Date: " << getTime() << "\r\n"
	<< "Content-Type: " << getMimeType() << "\r\n"
	<< "Content-Length: "<< htmlCode.size() << "\r\n"
	<< "Connection: closed" << "\r\n\r\n"
	<< htmlCode;
	_res = res.str();
}



