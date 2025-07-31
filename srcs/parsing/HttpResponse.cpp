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

void HttpResponse::parseResponse() {
	int	status = _request->getStatus();

	if (status == BAD_REQUEST)
		badRequest();
	if (status == NOT_FOUND)
		notFound();
}

const std::string& HttpResponse::getRes() const {return _res;}

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
	std::cout << "Response: " << res.str();
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



