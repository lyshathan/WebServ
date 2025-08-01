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
	if (pos == std::string::npos)
		return "text/html";
	else {
		if (uri.substr(pos + 1) == "html")
			return "text/html";
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

// HTTP/1.1 200 OK
// Server: nginx/1.29.0
// Date: Fri, 01 Aug 2025 08:45:11 GMT
// Content-Type: text/html
// Content-Length: 193
// Last-Modified: Fri, 25 Jul 2025 13:45:02 GMT
// Connection: keep-alive
// ETag: "68838a5e-c1"
// Accept-Ranges: bytes

// <!DOCTYPE html>
// <html lang="en">
// <head>
//   <meta charset="UTF-8">
//   <title>Welcome</title>
// </head>
// <body>
//   <h1>Welcome to my Nginx server!</h1>
//   <p>This is the index page.</p>
// </body>
// </html>

void HttpResponse::notFound() {
	std::ostringstream res;
	std::map<std::string, std::string> headers;

	std::string htmlCode = "<html><head><title>404 Not Found</title></head><body><center> \
		<h1>404 Not Found</h1></center><hr><center>webserv</center></body></html>";

	headers["Date"] = "Wed, 30 Jul 2025 12:29:42 GMT";
	headers["Server"] = "Webserv";
	headers["Content-Type"] = "text/html";
	headers["Content-Length"] = "137";
	headers["Connection"] = "keep-alive";

	res << _request->getVersion() + " 404 Not Found\r\n";
	std::map<std::string, std::string>::iterator it = headers.begin();
	for (; it != headers.end(); it++)
		res << it->first << ": " << it->second << "\r\n";
	res << "\r\n";
	res << htmlCode;
	_res = res.str();
}

void HttpResponse::badRequest() {
	std::ostringstream res;
	std::map<std::string, std::string> headers;

	std::string htmlCode = "<html><head><title>400 Bad Request</title></head><body><center> \
		<h1>400 Bad Request</h1></center><hr><center>webserv</center></body></html>";

	headers["Date"] = "Wed, 30 Jul 2025 12:29:42 GMT";
	headers["Server"] = "Webserv";
	headers["Content-Type"] = "text/html";
	headers["Content-Length"] = "127";
	headers["Connection"] = "close";

	res << _request->getVersion() + " 400 Bad Request\r\n";
	std::map<std::string, std::string>::iterator it = headers.begin();
	for (; it != headers.end(); it++)
		res << it->first << ": " << it->second << "\r\n";
	res << "\r\n";
	res << htmlCode;
	//std::cout << "Response: " << res.str();
	_res = res.str();
}

// HTTP/1.1 400 Bad Request
// Server: nginx/1.29.0
// Date: Wed, 30 Jul 2025 12:29:42 GMT
// Content-Type: text/html
// Content-Length: 157
// Connection: close

// <html>
// <head><title>400 Bad Request</title></head>
// <body>
// <center><h1>400 Bad Request</h1></center>
// <hr><center>nginx/1.29.0</center>
// </body>
// </html>



